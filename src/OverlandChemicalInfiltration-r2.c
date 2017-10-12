/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalInfiltration.c
C-
C-	Purpose/	Computes the infiltration flux of chemicals in the
C-	Methods:	overland plane (chemical infiltration to soils).
C-
C-				Chemical infiltration is linked to the soil column.
C-				The infiltration rate is used to compute the flux of
C-				chemicals entering or leaving each layer in the soil
C-				stack depending on the wetting front elevation.
C-
C-
C-  Inputs:	    infiltrationrate[][], fdissolvedov[][][][],
C-              fboundov[][][][]
C-
C-  Outputs:    infchemovoutflux[][][][][], infchemovinflux[][][][][]
C-
C-  Controls:   imask[][], elevlayerov[][][]
C-
C-  Calls:      None
C-
C-  Called by:  ChemicalTransport
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		26-DEC-2004
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

void OverlandChemicalInfiltration()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		ilayer,		//index for surface layer in the soil stack
		chanlink,	//channel link number
		channode;	//channel node number

	float
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf,	//surface area of channel portion of cell (m2)
		aovsurf,	//surface area of overland portion of cell (m2)
		watervol,	//water column volume of node (at time t) (m3)
		fmobile,	//sum of mobile fractions (dissolved + bound)
		wfe,		//wetting front elevation (m)
		outflow;	//flow rate leaving a bed layer (a velocity) (m/s)

	double
		potential,	//chemical infiltration mass potential (g)
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
				//Flux from the soil surface (infiltration)
				//
				//Note:  Leaching transports dissolved and bound phases
				//
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
					twch = twidth[chanlink][channode];			//channel top width at bank height (m)
					lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)

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

				//loop over number of chemical types
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//compute mobile fraction (sum of dissolved and bound)
					fmobile = fdissolvedov[ichem][i][j][0] + fboundov[ichem][i][j][0];

					//compute the leaching flux leaving the water column (g/s)
					infchemovoutflux[ichem][i][j][0] = infiltrationrate[i][j]
						* aovsurf * cchemov[ichem][i][j][0] * fmobile;

					//compute the deposition mass potential (g)
					potential = infchemovoutflux[ichem][i][j][0] * dt[idt];

					//Developer's Note:  When computing the mass available
					//                   for infiltration, the mass lost to
					//                   transformation processes should
					//                   only include the mass lost from
					//                   the mobile phase.  This will
					//                   require further code development.
					//
					//compute the mass available in the water column (g)
					available = watervol * cchemov[ichem][i][j][0] * fmobile
						- (biochemovoutflux[ichem][i][j][0]
						+ hydchemovoutflux[ichem][i][j][0]
						+ oxichemovoutflux[ichem][i][j][0]
						+ phtchemovoutflux[ichem][i][j][0]
						+ radchemovoutflux[ichem][i][j][0]
						+ vltchemovoutflux[ichem][i][j][0]
						+ udrchemovoutflux[ichem][i][j][0]) * dt[idt];

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//if the deposition potential > available mass
					if(potential > available)
					{
						//scale the chemical infiltration flux leaving the water column (g/s)
						infchemovoutflux[ichem][i][j][0] = (float)(available / dt[idt]);

					}	//end if potential > available

					//compute the leaching flux entering the surface soil layer (g/s)
					infchemovinflux[ichem][i][j][ilayer] = infchemovoutflux[ichem][i][j][0];

				}	//end loop over number of chemical types

				//compute the wetting front elevation (m)
				wfe = elevationov0[i][j] - infiltrationdepth[i][j];

				//Compute flux between soil layers...
				//
				//loop over layers (in reverse order: top down)
				for(ilayer=nstackov[i][j]; ilayer>=1; ilayer--)
				{
					//assign local value for soil layer volume (m3)
					soilvol = vlayerov[i][j][ilayer];

					//Determine if the wetting front is within this soil layer
					//
					//Two cases exist:
					//
					//   (1) wetting front is above bottom of this layer,
					//       so the flow across the interface is zero (no
					//       transport occurs)
					//
					//   (2) wetting front is below bottom of this layer, so
					//       the flow across the interface is equal to the
					//       infiltration rate at the soil-water interface
					//       (transport out of the layer occurs)
					//
					//Note:  elevlayerov[][][ilayer] = elevation at layer top
					//       elevlayerov[][][ilayer-1] = elevation at layer bottom
					//       elevlayerov[][][0] = elevation at bottom of stack
					//
					//if the wetting from elevation is above the layer bottom boundary
					if(wfe >= elevlayerov[i][j][ilayer-1])
					{
						//flow rate leaving the present layer (and entering the next) is zero
						outflow = 0.0;
					}
					else	//else the wetting front is below the layer bottom boundary
					{
						//flow rate leaving the present layer is the infiltration rate (m/s)
						outflow = infiltrationrate[i][j];

					}	//end if wfe >= elevlayerov[][][]...

					//loop over number of chemical types
					for(ichem=1; ichem<=nchems; ichem++)
					{
						//compute mobile fraction (sum of dissolved and bound)
						fmobile = fdissolvedov[ichem][i][j][ilayer] + fboundov[ichem][i][j][ilayer];

						//compute the infiltration flux leaving this layer (g/s)
						infchemovoutflux[ichem][i][j][ilayer] = outflow
							* aovsurf * cchemov[ichem][i][j][ilayer] * fmobile;

						//compute the infiltration mass potential (g)
						potential = infchemovoutflux[ichem][i][j][ilayer] * dt[idt];

						//Developer's Note:  When computing the mass available
						//                   for infiltration, the mass lost
						//                   to transformation processes should
						//                   only include the mass lost from
						//                   the mobile phase.  This will
						//                   require further code development.
						//
						//compute the mass available in the water column (g)
						available = soilvol * cchemov[ichem][i][j][ilayer] * fmobile
							- (biochemovoutflux[ichem][i][j][ilayer]
							+ hydchemovoutflux[ichem][i][j][ilayer]
							+ oxichemovoutflux[ichem][i][j][ilayer]
							+ phtchemovoutflux[ichem][i][j][ilayer]
							+ radchemovoutflux[ichem][i][j][ilayer]
							+ vltchemovoutflux[ichem][i][j][ilayer]
							+ udrchemovoutflux[ichem][i][j][ilayer]) * dt[idt];

						//if the available mass < 0.0
						if(available < 0.0)
						{
							//set the available mass (g) to zero
							available = 0.0;

						}	//end if available < 0.0

						//if the infiltration potential > available mass
						if(potential > available)
						{
							//scale the chemical infiltration flux leaving this layer (g/s)
							infchemovoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

						}	//end if potential > available

						//compute the infiltration flux entering the next layer (g/s)
						infchemovinflux[ichem][i][j][ilayer] = infchemovoutflux[ichem][i][j][0];

					}	//end loop over number of chemical types

				}	//end loop over layers

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to ChemicalTransport
}
