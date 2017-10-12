/*---------------------------------------------------------------------
C-  Function:	PopSedimentStack.c
C-
C-	Purpose/	Compute the upward re-indexing of solids and chemicals
C-	Methods:	in the sediment stack of the channel network.
C-
C-              Volume is conserved.  The surface area between layers
C-              can differ.  If the minimum volume trigger is greater
C-              than zero, the residual volume of the old surface layer
C-              must be added to the volume of the new surface layer
C-              and the new layer thickness must be recomputed from the
C-              total volume (new + residual) and the new layer surface
C-              area.  This can lead to "jump" discontinuities in the
C-              thickness and elevation of the surface layer if changes
C-              in surface area between any two layers in the stack are
C-              large.
C-
C-              Stack elements are numbered in reverse order.  The
C-              bottom (deepest) element is layer one.  The top-most
C-              element (surface layer) layer is layer nstackch[i][j].
C-
C-  Inputs:	    nstackch[][],vlayerch[][][], layer properties...
C-
C-  Outputs:    bursedchinmass[][][][], bursedchoutmass[][][][],
C-              burchemchinmass[][][][], burchemchoutmass[][][][],
C-              nstackch[][]
C-
C-  Controls:   minvlayerch[][][], ksim
C-
C-  Calls:      None
C-
C-  Called by:  NewStateStack
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

void PopSedimentStack()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		ilayer,		//index for surface layer in the sediment stack
		ichem,		//loop index for chemical
		row,		//row number of node
		col;		//column number of node

	double
		residualmass,	//residual solids mass in eroded surface layer (g)
		incomingmass,	//solids mass in the incoming subsurface layer (g)
		totalmass,		//total solids mass in the new (popped) surface layer (g)
		totalvolume;	//total volume of the new (popped) surface layer (m3)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//set the surface layer number
			ilayer = nstackch[i][j];

			//if the surface sediment layer volume <= the minimum volume
			//trigger AND the present number of stack elements > 1
			if(vlayerch[i][j][ilayer] <= minvlayerch[i][j][ilayer] && ilayer > 1)
			{
				//Pop the stack...
				//
				//Add the incoming layer to the residual mass/volume of
				//the surface layer...
				//
				//compute the total (residual + incoming) volume of the new surface layer (m3)
				totalvolume = vlayerch[i][j][ilayer] + vlayerch[i][j][ilayer-1];

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//compute residual solids mass (g)
					residualmass = csedch[isolid][i][j][ilayer]
						* vlayerch[i][j][ilayer];

					//compute incoming solids mass (g)
					incomingmass = csedch[isolid][i][j][ilayer-1]
						* vlayerch[i][j][ilayer-1];

					//compute total solids mass (g)
					totalmass = residualmass + incomingmass;

					//compute solids concentration in the new surface layer (g/m3)
					csedch[isolid][i][j][ilayer-1] = (float)(totalmass
						/ totalvolume);

					//Update mass balance terms...
					//
					//Note:  For the purpose of mass balance accounting,
					//       residual mass from an eroded surface layer
					//       is treated as a mass loss (burial out) from
					//       the eroded layer and a gain (burial in) to
					//       the incoming (new) surface layer.  This is
					//       done for consistency with mass accounting
					//       convention and the frame of reference for
					//       the stack even though mass is being scoured
					//       from the surface rather than being buried
					//       to the subsurface (i.e. we have to account
					//       for mass movement even though a layer is
					//       being lost from the stack).
					//
					//Developer's Note:  We could account for the transfer
					//                   of residual mass as a scour.  The
					//                   variables needed for this type of
					//                   calculation exist: scrchemchinmass
					//                   and scrchemchoutmass.  However, the
					//                   convention for tracking this mass
					//                   transfer needs further development.
					//
					//compute the buried mass leaving the eroded surface layer (kg)
					bursedchoutmass[isolid][i][j][ilayer] =
						bursedchoutmass[isolid][i][j][ilayer] + (float)(residualmass / 1000.0);

					//compute the buried mass entering the new surface layer (kg)
					bursedchinmass[isolid][i][j][ilayer-1] =
						bursedchinmass[isolid][i][j][ilayer-1] + (float)(residualmass / 1000.0);

					//Note:  The layer that used to be at the surface is now
					//       eliminated.  The erosion and deposition flux for
					//       the former surface layer must be set to zero.
					//
					//set the erosion outflux for the former surface layer to zero (g/s)
					erssedchoutflux[isolid][i][j][ilayer] = 0.0;

					//set the deposition influx for the former surface layer to zero (g/s)
					depsedchinflux[isolid][i][j][ilayer] = 0.0;

				}	//end loop over solids

				//if chemical transport is simulated (ksim > 2)
				if(ksim > 2)
				{
					//loop over chemicals
					for(ichem=1; ichem<=nchems; ichem++)
					{
						//compute residual chemical mass (g)
						residualmass = cchemch[ichem][i][j][ilayer]
							* vlayerch[i][j][ilayer];

						//compute incoming chemical mass (g)
						incomingmass = cchemch[ichem][i][j][ilayer-1]
							* vlayerch[i][j][ilayer-1];

						//compute total chemical mass (g)
						totalmass = residualmass + incomingmass;

						//compute chemical concentration in the new surface layer (g/m3)
						cchemch[ichem][i][j][ilayer-1] = (float)(totalmass
							/ totalvolume);

						//Note:  For the purpose of mass balance accounting,
						//       residual mass from an eroded surface layer
						//       is treated as a mass loss (burial out) from
						//       the eroded layer and a gain (burial in) to
						//       the incoming (new) surface layer.  This is
						//       done for consistency with mass accounting
						//       convention and the frame of reference for
						//       the stack even though mass is being scoured
						//       from the surface rather than being buried
						//       to the subsurface (i.e. we have to account
						//       for mass movement even though a layer is
						//       being lost from the stack).
						//
						//Developer's Note:  We could account for the transfer
						//                   of residual mass as a scour.  The
						//                   variables needed for this type of
						//                   calculation exist: scrchemchinmass
						//                   and scrchemchoutmass.  However, the
						//                   convention for tracking this mass
						//                   transfer needs further development.
						//
						//compute the buried mass leaving the eroded surface layer (kg)
						burchemchoutmass[ichem][i][j][ilayer] =
							burchemchoutmass[ichem][i][j][ilayer] + (float)(residualmass / 1000.0);

						//compute the buried mass entering the new surface layer (kg)
						burchemchinmass[ichem][i][j][ilayer-1] =
							burchemchinmass[ichem][i][j][ilayer-1] + (float)(residualmass / 1000.0);

						//Note:  The layer that used to be at the surface is now
						//       eliminated.  The erosion and deposition flux for
						//       the former surface layer must be set to zero.
						//
						//set the erosion outflux for the former surface layer to zero (g/s)
						erschemchoutflux[ichem][i][j][ilayer] = 0.0;

						//set the deposition influx for the former surface layer to zero (g/s)
						depchemchinflux[ichem][i][j][ilayer] = 0.0;

					}	//end loop over chemicals

				}	//end if ksim > 2

				//Assign new surface layer volume (and new volume)...
				//
				//Note:  The new volume of the new surface
				//       layer does not need to be reset
				//       because it will be calculated in
				//       ChannelSolidsConcentration...
				//
				//Assign total volume for new surface layer (m3)
				vlayerch[i][j][ilayer-1] = totalvolume;

				//Compute thickness of new surface layer (m)
				hlayerch[i][j][ilayer-1] = (float)(vlayerch[i][j][ilayer-1]
					/ achbed[i][j][ilayer-1]);

				//Wipe the old surface layer from the stack
				//
				//set the old layer thickness to zero
				hlayerch[i][j][ilayer] = 0.0;

				//set the old layer width to zero
				bwlayerch[i][j][ilayer] = 0.0;

				//set the old bed area to zero
				achbed[i][j][ilayer] = 0.0;

				//set the old layer volume to zero
				vlayerch[i][j][ilayer] = 0.0;

				//Note:  The "old layer new volume" is the
				//       value of vlayerchnew of the layer
				//       that is being removed from the
				//       stack...
				//
				//set the old layer new volume to zero
				vlayerchnew[i][j][ilayer] = 0.0;

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//set the solids concentration to zero
					csedch[isolid][i][j][ilayer] = 0.0;

				}	//end loop over solids

				//if chemical transport is simulated (ksim > 2)
				if(ksim > 2)
				{
					//loop over chemicals
					for(ichem=1; ichem<=nchems; ichem++)
					{
						//set the chemical concentration to zero
						cchemch[ichem][i][j][ilayer] = 0.0;

					}	//end loop over chemicals

				}	//end if ksim > 2

				//if the channel elevation option > 0
				if(elevchopt > 0)
				{
					//Assign channel elevation (m)
					elevationch[i][j] = elevlayerch[i][j][ilayer-1];

				}	//end if elevchopt > 0

				//Compute new channel geometry...
				//
				//Assign channel bottom width (m)
				bwidth[i][j] = bwlayerch[i][j][ilayer-1];

				//get row and column of node
				row = ichnrow[i][j];
				col = ichncol[i][j];

				//Compute new channel bank height (m)
				hbank[i][j] = elevationov[row][col] - elevationch[i][j];

				//compute new channel side slope (run per unit rise)
				sideslope[i][j] = (float)(0.5 * (twidth[i][j] - bwidth[i][j])
					/ hbank[i][j]);

				//Decrement the number of layers in the stack
				nstackch[i][j] = nstackch[i][j] - 1;

			}	//end if volume <= minvol AND nstackch[][] > 1

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to NewStateStack
}
