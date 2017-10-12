/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalDeposition.c
C-
C-	Purpose/	Compute the deposition flux of chemicals in the overland
C-	Methods:	plane.
C-
C-
C-  Inputs:	    depflowov[][][]
C-
C-  Outputs:    depflowov[][][]
C-
C-  Controls:   imask[][], depovopt, cncopt[]
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
C-	Date:		29-DEC-2003
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

void OverlandChemicalDeposition()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		isolid,		//loop index for solids (particle) type
		ilayer,		//index for surface layer in the soil stack
		chanlink,	//channel link number
		channode;	//channel node number

	float
		hcell,		//depth of water in cell (m)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf,	//surface area of channel portion of cell (m2)
		aovsurf,	//surface area of overland portion of cell (m2)
		watervol,	//water column volume of node (at time t) (m3)
		sumfp;		//sum of particulate fractions

	double
		potential,	//chemical deposition potential (g)
		available;	//chemical mass available for deposition (g)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Compute Deposition Fluxes...
				//
				//Note:  Deposition only transports the particulate phase
				//
				//Flux to overland soil surface
				//
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

				//loop over number of chemicals
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//Initialize gross deposition outflux array (from water column)
					depchemovoutflux[ichem][i][j][0] = 0.0;

					//initialize the sum of particulate fractions
					sumfp = 0.0;

					//loop over number of solids types
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//compute the deposition flux leaving water column (g/s)
						depchemovoutflux[ichem][i][j][0] = depchemovoutflux[ichem][i][j][0]
							+ depflowov[isolid][i][j] * cchemov[ichem][i][j][0]
							* fparticulateov[ichem][isolid][i][j][0];

						//compute sum of particulate fractions
						sumfp = sumfp + fparticulateov[ichem][isolid][i][j][0];

					}	//end loop over number of solids types

					//if the sum of particulate fractions (sumfp) > 1.0
					if(sumfp > 1.0)
					{
						//reset sum of all particulate phases to exactly 1.0
						sumfp = (float)(1.0);

					}	//end if sumfp > 1.0

					//compute the deposition mass potential (g)
					potential = depchemovoutflux[ichem][i][j][0] * dt[idt];

					//Developer's Note:  When computing the mass available
					//                   for deposition, the mass lost to
					//                   transformation processes should
					//                   only include the mass lost from
					//                   the particulate phase.  This will
					//                   require further code development.
					//
					//compute the mass available in the water column (g)
					available = watervol * cchemov[ichem][i][j][0] * sumfp
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
						//scale the deposition flux leaving the water column (g/s)
						depchemovoutflux[ichem][i][j][0] = (float)(available / dt[idt]);

					}	//end if potential > available

					//compute the deposition flux entering the sediment bed (g/s)
					depchemovinflux[ichem][i][j][ilayer] = depchemovoutflux[ichem][i][j][0];

				}	//end loop over number of chemicals

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to ChemicalTransport
}
