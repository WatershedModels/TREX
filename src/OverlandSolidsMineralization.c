/*---------------------------------------------------------------------
C-  Function:	OverlandSolidsMineralization.c
C-
C-	Purpose/	Compute the biodegradation flux of chemicals in the
C-	Methods:	overland plane.  Chemical biodegradation is assumed
C-				to occur at the same rate for all chemical phases.
C-
C-
C-  Inputs:	    channel properties,
C-              kbiowov[], kbiosov[]
C-
C-  Outputs:    biochemovoutflux[][][][]
C-
C-  Controls:   imask[][], bioopt[]
C-
C-  Calls:      None
C-
C-  Called by:  OverlandChemicalKinetics
C-
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO
C-
C-	Date:		15-MAY-2005
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

void OverlandSolidsMineralization()
{
/*
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		ilayer,		//index for surface layer in the sediment stack
		chanlink,	//channel link number
		channode;	//channel node number

	float
		hcell,		//depth of water in cell (m)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf,	//surface area of channel portion of cell (m2)
		aovsurf,	//surface area of overland portion of cell (m2)
		watervol;	//water column volume of node (at time t) (m3)
		
	float
		kw,			//water column reaction rate (1/s)
		ks;			//soil layer reaction rate (1/s)

	double
		potential,	//chemical biodegradation mass potential (g)
		available,	//chemical mass available for transport (g)
		soilvol;	//soil layer volume of node (at time t) (m3)

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

				//set the surface layer number
				ilayer = nstackov[i][j];

				//set the surface soil layer volume (m3)
				soilvol = vlayerov[i][j][ilayer];

				//loop over number of chemical types
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//if the chemical biodegrades
					if(bioopt[ichem] > 0)
					{
						//Compute biodegradation flux for water column...
						//
						//Note:  Biodegradation rates may be either first order (1/s)
						//       or second order (m3/cell/s).  When a second oder rate
						//       is input, bacterial concentrations must be specified.
						//       When multiplied by the bacterial concentration, the
						//       resulting rate value is in 1/s.  When a first order
						//       rate is input, the default bacterial concentration
						//       is 1.0.
						//
						//compute the effective first order biodegradation rate (1/s)
		//mlv						kw = kbiowov[ichem] * bacteriaov[i][j][0];

						//compute the effective first order biodegradation rate (1/s)
						kw = kbiowov[ichem];

						//compute the biodegradation flux in the water column (g/s)
						biochemovoutflux[ichem][i][j][0] = kw * cchemov[ichem][i][j][0]
							* watervol;

						//compute the chemical biodegradation mass potential (g)
						potential = biochemovoutflux[ichem][i][j][0] * dt[idt];

						//compute the mass available in the water column (g)
						available = watervol * cchemov[ichem][i][j][0];

						//Developer's Note:  The check for available < 0.0 is not
						//                   needed here but is needed in general.
						//                   It is left here as a template for
						//                   future code development.

						//if the available mass < 0.0
						if(available < 0.0)
						{
							//set the available mass (g) to zero
							available = 0.0;

						}	//end if available < 0.0

						//if the biodegradation potential > available mass
						if(potential > available)
						{
							//scale the biodegrdation flux for the water column (g/s)
							biochemovoutflux[ichem][i][j][0] = (float)(available / dt[idt]);

						}	//end if potential > available

						//Compute biodegradation flux for surface sediment layer...
						//
						//compute the effective first order biodegradation rate (1/s)
		//mlv						ks = kbiosov[ichem] * bacteriaov[i][j][ilayer];

						//compute the effective first order biodegradation rate (1/s)
						ks = kbiosov[ichem];

						//compute the biodegradation flux in the sediment bed (g/s)
						biochemovoutflux[ichem][i][j][ilayer] = (float)(ks * cchemov[ichem][i][j][ilayer]
								* soilvol);

						//compute the chemical biodegradation mass potential (g)
						potential = biochemovoutflux[ichem][i][j][ilayer] * dt[idt];

						//compute the mass available in the sediment layer (g)
						available = soilvol * cchemov[ichem][i][j][ilayer];

						//if the available mass < 0.0
						if(available < 0.0)
						{
							//set the available mass (g) to zero
							available = 0.0;

						}	//end if available < 0.0

						//if the biodegrdation potential > available mass
						if(potential > available)
						{
							//scale the biodegradation flux for the sediment (g/s)
							biochemovoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

						}	//end if potential > available

						//Developer's Note:  If reaction processes occur in the
						//                   subsurface sediment layers, flux
						//                   terms for these layers would need
						//                   to be computed here...

					}	//end if bioopt[] > 0

				}	//end loop over number of chemical types

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows
*/
//End of function: Return to OverlandChemicalKinetics
}
