/*----------------------------------------------------------------------
C-  Function:	NewStateSolids.c
C-
C-	Purpose/	NewStateSolids is called to store new particle
C-	Methods:	concentrations for use during the next (upcoming)
C-				time step (t + dt).
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:	chnopt
C-
C-	Calls:		None
C-
C-	Called by:	NewState
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
C-	Date:		20-MAR-2004
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		13-JUN-2008
C-
C-	Revisions:  Added a loop over subsurface layers to update chemical
C-              concentrations for overland plane and channels.  Added
C-              calculation of total solids concentrations.
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void NewStateSolids()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids number (type)
		ilayer;		//loop index for surface layer

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//set layer number for surface soil layer
				ilayer = nstackov[i][j];

				//Note:  The sum of all solids (TSS) is stored in the zeroeth
				//       elements of the [isolids] dimension of the csedov and
				//       csedch arrays.  The total solids concentration is used
				//       in several places throughout the code.  By storing the
				//       total solids concentration as csed##[0][i][j][ilayer],
				//       the need for repetative calculations to dynamically
				//       compute this value is eliminated.
				//
				//initialize the total solids concentration for the water column
				csedov[0][i][j][0] = 0.0;

				//initialize the total solids concentration for the surface soil layer
				csedov[0][i][j][ilayer] = 0.0;

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//set overland solids concentration in water column (g/m3) (for use at time t + dt)
					csedov[isolid][i][j][0] = csedovnew[isolid][i][j][0];

					//Determine the minimum and the maximum solids concentrations in water
					mincsedov0[isolid] = Min(mincsedov0[isolid],csedov[isolid][i][j][0]);
					maxcsedov0[isolid] = Max(maxcsedov0[isolid],csedov[isolid][i][j][0]);

					//set overland solids concentration in surface sediment (g/m3) (for use at time t + dt)
					csedov[isolid][i][j][ilayer] = csedovnew[isolid][i][j][ilayer];

					//Determine the minimum and the maximum solids concentrations in the surface soil layer
					mincsedov1[isolid] = Min(mincsedov1[isolid],csedov[isolid][i][j][ilayer]);
					maxcsedov1[isolid] = Max(maxcsedov1[isolid],csedov[isolid][i][j][ilayer]);

					//add the present solids type to the total concentration for the water column (g/m3)
					csedov[0][i][j][0] = csedov[0][i][j][0] + csedov[isolid][i][j][0];

					//add the present solids type to the total concentration for the surface soil layer (g/m3)
					csedov[0][i][j][ilayer] = csedov[0][i][j][ilayer] + csedov[isolid][i][j][ilayer];

				}	//end loop over solids

				//Determine the minimum and the maximum total solids concentrations in water
				mincsedov0[0] = Min(mincsedov0[0],csedov[0][i][j][0]);
				maxcsedov0[0] = Max(maxcsedov0[0],csedov[0][i][j][0]);

				//Determine the minimum and the maximum total solids concentrations in the surface layer
				mincsedov1[0] = Min(mincsedov1[0],csedov[0][i][j][ilayer]);
				maxcsedov1[0] = Max(maxcsedov1[0],csedov[0][i][j][ilayer]);

				//Developers Note:  The code below is used to update solids concentrations
				//                  in subsurface layers.  It is commented out and not used
				//                  at present because the concentration of solids in the
				//                  subsurface is not updated in OverlandSolidsConcentration
				//                  yet.  Once particle reactions are implemented, the code
				//                  below would need to be activated...
				//
/*
				//loop over subsurface layers (in reverse order: top down)
				for(ilayer=nstackov[i][j]-1; ilayer>=1; ilayer--)
				{
					//initialize the total solids concentration for the subsurface soil layer
					csedov[0][i][j][ilayer] = 0.0;

					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//set overland soilds concentration in subsurface layer (g/m3) (for use at time t + dt)
						csedov[isolid][i][j][ilayer] = csedovnew[isolid][i][j][ilayer];

						//compute the total solids in the subsurface layer
						csedov[0][i][j][ilayer] = csedov[0][i][j][ilayer] + csedov[isolid][i][j][ilayer];

					}	//end loop over solids

				}	//end loop over subsurface layers
*/
			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//if channels are simulated
	if(chnopt > 0)
	{
		//Loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//set layer number for surface sediment layer
				ilayer = nstackch[i][j];

				//Note:  The sum of all solids (TSS) is stored in the zeroeth
				//       elements of the [isolids] dimension of the csedov and
				//       csedch arrays.  The total solids concentration is used
				//       in several places throughout the code.  By storing the
				//       total solids concentration as csed##[0][i][j][ilayer],
				//       the need for repetative calculations to dynamically
				//       compute this value is eliminated.
				//
				//initialize the total solids concentration for the water column
				csedch[0][i][j][0] = 0.0;

				//initialize the total solids concentration for the surface sediment layer
				csedch[0][i][j][ilayer] = 0.0;

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//set channel solids concentration in water column (g/m3) (for use at time t + dt)
					csedch[isolid][i][j][0] = csedchnew[isolid][i][j][0];

					//Determine the minimum and the maximum solids concentrations in water
					mincsedch0[isolid] = Min(mincsedch0[isolid],csedch[isolid][i][j][0]);
					maxcsedch0[isolid] = Max(maxcsedch0[isolid],csedch[isolid][i][j][0]);

					//set channel solids concentration in surface sediment (g/m3) (for use at time t + dt)
					csedch[isolid][i][j][ilayer] = csedchnew[isolid][i][j][ilayer];

					//Determine the minimum and the maximum solids concentrations in surface sediment
					mincsedch1[isolid] = Min(mincsedch1[isolid],csedch[isolid][i][j][ilayer]);
					maxcsedch1[isolid] = Max(maxcsedch1[isolid],csedch[isolid][i][j][ilayer]);

					//add the present solids type to the total concentration for the water column (g/m3)
					csedch[0][i][j][0] = csedch[0][i][j][0] + csedch[isolid][i][j][0];

					//add the present solids type to the total concentration for the surface sediment layer (g/m3)
					csedch[0][i][j][ilayer] = csedch[0][i][j][ilayer] + csedch[isolid][i][j][ilayer];

				}	//end loop over solids

				//Determine the minimum and the maximum total solids concentrations in water
				mincsedch0[0] = Min(mincsedch0[0],csedch[0][i][j][0]);
				maxcsedch0[0] = Max(maxcsedch0[0],csedch[0][i][j][0]);

				//Determine the minimum and the maximum total solids concentrations in the surface layer
				mincsedch1[0] = Min(mincsedch1[0],csedch[0][i][j][ilayer]);
				maxcsedch1[0] = Max(maxcsedch1[0],csedch[0][i][j][ilayer]);

				//Developers Note:  The code below is used to update solids concentrations
				//                  in subsurface layers.  It is commented out and not used
				//                  at present because the concentration of solids in the
				//                  subsurface is not updated in ChannelSolidsConcentration
				//                  yet.  Once particle reactions are implemented, the code
				//                  below would need to be activated...
				//
/*
				//loop over subsurface layers (in reverse order: top down)
				for(ilayer=nstackch[i][j]-1; ilayer>=1; ilayer--)
				{
					//initialize the total solids concentration for the subsurface sediment layer
					csedch[0][i][j][ilayer] = 0.0;

					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//set channel soilds concentration in subsurface layer (g/m3) (for use at time t + dt)
						csedch[isolid][i][j][ilayer] = csedchnew[isolid][i][j][ilayer];

						//compute the total solids in the subsurface layer
						csedch[0][i][j][ilayer] = csedch[0][i][j][ilayer] + csedch[isolid][i][j][ilayer];

					}	//end loop over solids

				}	//end loop over subsurface layers
*/
			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt > 0

//End of function: Return to NewState
}
