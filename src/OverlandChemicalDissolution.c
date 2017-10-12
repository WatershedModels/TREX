/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalDissolution.c
C-
C-	Purpose/	Compute the dissolution flux of solids in the overland
C-	Methods:	plane.  This is the reaction pathway for pure phase
C-              solid chemicals.  Mass from a solids state veriable is
C-              lost (an outflux is computed).  The mass influx to a
C-              chemical state variable is computed in the chemical
C-              yield module.
C-
C-
C-  Inputs:	    ds[], spgravity[], channel properties
C-
C-  Outputs:    dslsedovoutflux[][][][]
C-
C-  Controls:   imask[][], dslopt[], ncyields, cyldfrom[],
C-              cyldprocess[]
C-
C-  Calls:      None
C-
C-  Called by:  OverlandChemicalKinetics
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-				Zhonglong Zhang
C-              Environmental Laboratory - ERDC
C-              U.S. Army Corps of Engineers
C-              Vicksburg, MS
C-
C-	Date:		27-JULY-2005
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void OverlandChemicalDissolution()
{
	//local variable declarations/definitions
	int
		ichem,			//loop index for chemical type
		isolid,			//loop index for solids (particle) type
		iyield,			//loop index for chemical reaction yields
		ilayer,			//index for surface layer in the soil stack
		chanlink,		//channel link number
		channode;		//channel node number

	float
		hcell,			//depth of water in cell (m)
		lch,			//channel length (m) (includes sinuosity)
		twch,			//top width of channel at flow depth (m)
		achsurf,		//surface area of channel portion of cell (m2)
		aovsurf,		//surface area of overland portion of cell (m2)
		watervol,		//water column volume of cell (at time t) (m3)
		newwatervol,	//new water column volume of cell (at time t) (m3)
		densityw;		//density of water (1000) (kg/m3)

	float
		kd,				//dissolution rate (m2/s)
		csol,			//chemical solubility limit (g/m3)
		alpha;			//surface area (of solids) for dissolution (m2)

	double
		potential,		//solids mass dissolution potential (g)
		available,		//solids mass available for dissolution (g)
		soilvol,		//soil layer volume of node (at time t) (m3)
		newsoilvol,		//new soil layer volume of node (at time t+dt) (m3)
		newsolidsmass;	//new solids mass after dissolution (at time t+dt) (g)

	//Define local constants...
	//
	//set density of water = 1000 kg/m3
	densityw = (float)(1000.0);

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//set the present water depth of the overland cell (m)
				hcell = hov[i][j];

				//Compute cell surface area...
				//
				//if the cell is a channel cell
				if(imask[i][j] > 1)
				{
					//Get channel link and node
					chanlink = link[i][j];
					channode = node[i][j];

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					twch = twidth[chanlink][channode];		//channel top width at bank height (m)
					lch = chanlength[chanlink][channode];	//channel length (m) (includes sinuosity)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;
				}
				else	//cell is not a channel cell (overland only)
				{
					//no channel present, surface area is zero
					achsurf = 0.0;

				}	//end if imask > 1

				//compute surface area of cell
				aovsurf = w * w - achsurf;

				//compute present water column volume (m3) (at time t)
				watervol = hov[i][j] * aovsurf;

				//compute new water column volume (m3) (at time t+dt)
				newwatervol = hovnew[i][j] * aovsurf;

				//set the surface layer number
				ilayer = nstackov[i][j];

				//set the surface soil layer volume (m3) (at time t)
				soilvol = vlayerov[i][j][ilayer];

				//set the surface soil layer volume (m3) (at time t+dt)
				newsoilvol = vlayerovnew[i][j][ilayer];

				//loop over number of chemical types
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//if the chemical dissolves (dslopt[] > 0)
					if(dslopt[ichem] > 0)
					{
						//loop over chemical yields
						for(iyield=1; iyield<=ncyields; iyield++)
						{
							//if the reaction path is dissolution (process 8) and the product chemical is the current chemical
							if(cyldprocess[iyield] == 8 && cyldto[iyield] == ichem)
							{
								//set the reacting solids type
								isolid = cyldfrom[iyield];

								//set dissolution rate
								kd = kdslw[ichem];

								//set the chemical solubility (g/m3)
								csol = csolubility[ichem];

								//compute surface area available for dissolution (m2)
								alpha = (float)((6.0 * csedov[isolid][i][j][0] * watervol)
									/ (ds[isolid] * spgravity[isolid] * densityw));

								//compute the solids dissolution outflux for the water column (g/s)
								dslsedovoutflux[isolid][i][j][0] = kd * alpha
									* (csol - fdissolvedov[ichem][i][j][0] * cchemov[ichem][i][j][0]);

								//compute the dissolution potential (g)
								potential = dslsedovoutflux[isolid][i][j][0] * dt[idt];

								//Note:  The solids mass available for dissolution is the
								//       starting solids mass at time t minus the sum of
								//       mass transported by advection and dispersion (in
								//       all directions), deposition, and other reaction
								//       processes during the time step.  For parallelism,
								//       this could be computed as:
								//
								//       available = csedov[][][][0] * watervol
								//         - (advsedovoutflux[][][][0-10]
								//         + dspsedovoutflux[][][][1-10]
								//         + depsedovoutflux[][][][0]
								//         + tnssedovoutflux[][][][0]) * dt[idt];
								//
								//       For simplicity, this can also be computed as:
								//
								//       available = csedovnew[][][][0] * newwatervol;
								//
								//
								//Developer's Note:  This simplification is not as
								//                   accurate because the new solids
								//                   mass also includes mass transfer
								//                   influxes (erosion, reactions).
								//                   Influxes should be excluded to
								//                   prevent mass from both moving
								//                   and reacting in the same time
								//                   step...
								//
								//compute the mass available for dissolution (g)
								available = csedovnew[isolid][i][j][0] * newwatervol;

								//if the available mass < 0.0
								if(available < 0.0)
								{
									//set the available mass (g) to zero
									available = 0.0;

								}	//end if available < 0.0

								//Note:  At this point we need to check whether there
								//       is enough sediment of this type available in
								//       the water column to satify the full dissolution
								//       potential.  If not, the dissolution potential
								//       for this solids type is limited to available
								//       mass...
								//
								//if the overall outflux potential exceeds the available mass
								if(potential > available)
								{
									//scale the dissolution flux (g/s) to the limit of available mass
									dslsedovoutflux[isolid][i][j][0] = (float)(available / dt[idt]);

								}	//end if potential > available

								//Recompute new solids concentration after dissolution reaction...
								//
								//compute new solids mass (g)
								newsolidsmass = available
									- dslsedovoutflux[isolid][i][j][0] * dt[idt];

								//if the new solids mass >= 0
								if(newsolidsmass >= 0)
								{
									//compute the new solids concentration after dissolution (g/m3)
									csedovnew[isolid][i][j][0] = (float)(newsolidsmass / newwatervol);

								}	//end if newsolidsmass >= 0

								//Compute dissolution flux for surface soil layer...
								//
								//set dissolution rate
								kd = kdsls[ichem];

								//compute surface area available for dissolution (m2)
								alpha = (float)((6.0 * csedov[isolid][i][j][ilayer] * soilvol)
									/ (ds[isolid] * spgravity[isolid] * densityw));

								//compute the solids dissolution outflux for the surface soil (g/s)
								dslsedovoutflux[isolid][i][j][ilayer] = kd * alpha
									* (csol - fdissolvedov[ichem][i][j][ilayer] * cchemov[ichem][i][j][ilayer]);

								//compute the dissolution potential (g)
								potential = dslsedovoutflux[isolid][i][j][ilayer] * dt[idt];

								//Note:  The solids mass available for dissolution is the
								//       starting solids mass at time t minus the sum of
								//       mass transported by erosion and other reaction
								//       processes during the time step.  For parallelism,
								//       this could be computed as:
								//
								//       available = csedov[][][][ilayer] * watervol
								//         - (erssedovoutflux[][][][ilayer]
								//         + tnssedovoutflux[][][][ilayer]) * dt[idt];
								//
								//       For simplicity, this can also be computed as:
								//
								//       available = csedovnew[][][][ilayer] * newsoilvol;
								//
								//
								//Developer's Note:  This simplification is not as
								//                   accurate because the new solids
								//                   mass also includes mass transfer
								//                   influxes (erosion, reactions).
								//                   Influxes should be excluded to
								//                   prevent mass from both moving
								//                   and reacting in the same time
								//                   step...
								//
								//compute the mass available for dissolution (g)
								available = csedovnew[isolid][i][j][ilayer] * newsoilvol;

								//if the available mass < 0.0
								if(available < 0.0)
								{
									//set the available mass (g) to zero
									available = 0.0;

								}	//end if available < 0.0

								//if the overall outflux potential exceeds the available mass
								if(potential > available)
								{
									//scale the dissolution flux (g/s) to the limit of available mass
									dslsedovoutflux[isolid][i][j][ilayer] = (float)(available / dt[idt]);

								}	//end if potential > available

								//Recompute bed new solids concentration after dissolution reaction...
								//
								//compute bed new solids mass (g)
								newsolidsmass = available
									- dslsedovoutflux[isolid][i][j][ilayer] * dt[idt];

								//if the new solids mass >= 0
								if(newsolidsmass >= 0)
								{
									//compute the new solids concentration after dissolution (g/m3)
									csedovnew[isolid][i][j][ilayer] = (float)(newsolidsmass / newsoilvol);

								}	//end if newsolidsmass >= 0

							}	//end if cyldto = ichem and cyldprocess = 8

						}	//end loop over yields

					}	//end if dslopt[] > 0

				}	//end loop over number of chemicals types

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to OverlandChemicalKinetics
}
