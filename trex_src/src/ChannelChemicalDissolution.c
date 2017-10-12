/*---------------------------------------------------------------------
C-  Function:	ChannelChemicalDissolution.c
C-
C-	Purpose/	Compute the dissolution flux of solids in the channel
C-	Methods:	network.  This is the reaction pathway for pure phase
C-              solid chemicals.  Mass from a solids state veriable is
C-              lost (an outflux is computed).  The mass influx to a
C-              chemical state variable is computed in the chemical
C-              yield module.
C-
C-
C-  Inputs:	    ds[], spgravity[], channel properties
C-
C-  Outputs:    dslsedchoutflux[][][][]
C-
C-  Controls:   imask[][], dslopt[], ncyields, cyldfrom[],
C-              cyldprocess[]
C-
C-  Calls:      None
C-
C-  Called by:  ChannelChemicalKinetics
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
C-	Date:		27-JUL-2005
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

void ChannelChemicalDissolution()
{
	//local variable declarations/definitions
	int
		ichem,			//loop index for chemical type
		isolid,			//loop index for solids (particle) type
		iyield,			//loop index for chemical reaction yields
		ilayer;			//index for surface layer in the sediment stack

	float
		hchan,			//depth of water in channel (m)
		bwch,			//bottom width of channel (m)
		hbch,			//bank height of channel (m)
		sslope,			//side slope of channel bank (dimensionless)
		twch,			//top width of channel at top of bank (m)
		achcross,		//cross sectional area of channel at present water depth (m2)
		watervol,		//water column volume of node (at time t) (m3)
		newwatervol,	//new water column volume of node (at time t) (m3)
		densityw;		//density of water (1000) (kg/m3)

	float
		kd,				//dissolution rate (m2/s)
		csol,			//chemical solubility limit (g/m3)
		alpha;			//surface area (of solids) for dissolution (m2)

	double
		potential,		//solids mass dissolution potential (g)
		available,		//solids mass available for dissolution (g)
		sedvol,			//sediment layer volume of node (at time t) (m3)
		newsedvol,		//new sediment layer volume of node (at time t+dt) (m3)
		newsolidsmass;	//new solids mass after dissolution (at time t+dt) (g)

	//Define local constants...
	//
	//set density of water = 1000 kg/m3
	densityw = (float)(1000.0);

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Assign channel characteristics
			bwch = bwidth[i][j];				//bottom width (m)
			hbch = hbank[i][j];					//bank height (m)
			sslope = sideslope[i][j];			//side slope (dimensionless)

			//Compute present water volume...
			//
			//if the present flow depth <= bank height
			if(hch[i][j] <= hbch)
			{
				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hch[i][j]) * hch[i][j];
			}
			else	//else flow depth greater than channel depth
			{
				//Channel top width at top of bank (m)
				twch = twidth[i][j];

				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hbch) * hbch
						+ (hch[i][j] - hbch) * twch;

			}	//end if hch <= bank height

			//compute present water column volume (m3) (at time t)
			watervol = achcross * chanlength[i][j];

			//Compute new water column volume...
			//
			//Assign new water depth (m) (at time t + dt)
			hchan = hchnew[i][j];

			//if the new flow depth <= bank height
			if(hchan <= hbch)
			{
				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hchan) * hchan;
			}
			else	//else flow depth greater than channel depth
			{
				//Channel top width at top of bank (m)
				twch = twidth[i][j];

				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hbch) * hbch
						+ (hchan - hbch) * twch;

			}	//end if hchan <= bank height

			//compute new water column volume (m3) (at time t + dt)
			newwatervol = achcross * chanlength[i][j];

			//set the surface sediment layer number
			ilayer = nstackch[i][j];

			//set the surface sediment layer volume (m3) (at time t)
			sedvol = vlayerch[i][j][ilayer];

			//set the surface sediment layer volume (m3) (at time t+dt)
			newsedvol = vlayerchnew[i][j][ilayer];

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
							alpha = (float)((6.0 * csedch[isolid][i][j][0] * watervol)
								/ (ds[isolid] * spgravity[isolid] * densityw));

							//compute the solids dissolution outflux for the water column (g/s)
							dslsedchoutflux[isolid][i][j][0] = kd * alpha
								* (csol - fdissolvedch[ichem][i][j][0] * cchemch[ichem][i][j][0]);

							//compute the dissolution potential (g)
							potential = dslsedchoutflux[isolid][i][j][0] * dt[idt];

							//Note:  The solids mass available for dissolution is the
							//       starting solids mass at time t minus the sum of
							//       mass transported by advection and dispersion (in
							//       all directions), deposition, and other reaction
							//       processes during the time step.  For parallelism,
							//       this could be computed as:
							//
							//       available = csedch[][][][0] * watervol
							//         - (advsedchoutflux[][][][0-10]
							//         + dspsedchoutflux[][][][1-10]
							//         + depsedchoutflux[][][][0]
							//         + tnssedchoutflux[][][][0]) * dt[idt];
							//
							//       For simplicity, this can also be computed as:
							//
							//       available = csedchnew[][][][0] * newwatervol;
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
							available = csedchnew[isolid][i][j][0] * newwatervol;

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
								dslsedchoutflux[isolid][i][j][0] = (float)(available / dt[idt]);

							}	//end if potential > available

							//Recompute new solids concentration after dissolution reaction...
							//
							//compute new solids mass (g)
							newsolidsmass = available
								- dslsedchoutflux[isolid][i][j][0] * dt[idt];

							//if the new solids mass >= 0
							if(newsolidsmass >= 0)
							{
								//compute the new solids concentration after dissolution (g/m3)
								csedchnew[isolid][i][j][0] = (float)(newsolidsmass / newwatervol);

							}	//end if newsolidsmass > 0

							//Compute dissolution flux for surface soil layer...
							//
							//set dissolution rate
							kd = kdsls[ichem];

							//compute surface area available for dissolution (m2)
							alpha = (float)((6.0 * csedch[isolid][i][j][ilayer] * sedvol)
								/ (ds[isolid] * spgravity[isolid] * densityw));

							//compute the solids dissolution outflux for the surface sediment (g/s)
							dslsedchoutflux[isolid][i][j][ilayer] = kd * alpha
								* (csol - fdissolvedch[ichem][i][j][ilayer] * cchemch[ichem][i][j][ilayer]);

							//compute the dissolution potential (g)
							potential = dslsedchoutflux[isolid][i][j][ilayer] * dt[idt];

							//Note:  The solids mass available for dissolution is the
							//       starting solids mass at time t minus the sum of
							//       mass transported by erosion and other reaction
							//       processes during the time step.  For parallelism,
							//       this could be computed as:
							//
							//       available = csedch[][][][ilayer] * watervol
							//         - (erssedchoutflux[][][][ilayer]
							//         + tnssedchoutflux[][][][ilayer]) * dt[idt];
							//
							//       For simplicity, this can also be computed as:
							//
							//       available = csedchnew[][][][ilayer] * newsedvol;
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
							available = csedchnew[isolid][i][j][ilayer] * newsedvol;

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
								dslsedchoutflux[isolid][i][j][ilayer] = (float)(available / dt[idt]);

							}	//end if potential > available

							//Recompute bed new solids concentration after dissolution reaction...
							//
							//compute bed new solids mass (g)
							newsolidsmass = available
								- dslsedchoutflux[isolid][i][j][ilayer] * dt[idt];

							//if the new solids mass >= 0
							if(newsolidsmass >= 0)
							{
								//compute the new solids concentration after dissolution (g/m3)
								csedchnew[isolid][i][j][ilayer] = (float)(newsolidsmass / newsedvol);

							}	//end if newsolidsmass >= 0

						}	//end if cyldto = ichem and cyldprocess = 8

					}	//end loop over yields

				}	//end if dslopt[] > 0

			}	//end loop over number of chemicals types

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChannelChemicalKinetics
}
