/*---------------------------------------------------------------------
C-  Function:	PopSoilStack.c
C-
C-	Purpose/	Compute the upward re-indexing of solids and chemicals
C-	Methods:	in the soil stack of the overland plane.
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
C-              element (surface layer) layer is layer nstackov[i][j].
C-
C-  Inputs:	    nstackov[][],vlayerov[][][], layer properties...
C-
C-  Outputs:    bursedovinmass[][][][], bursedovoutmass[][][][],
C-              burchemovinmass[][][][], burchemovoutmass[][][][],
C-              nstackov[][]
C-
C-  Controls:   minvlayerov[][][], ksim, chnopt
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

void PopSoilStack()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		ilayer,		//index for surface layer in the sediment stack
		ichem,		//loop index for chemical
		chanlink,	//link number of a channel cell
		channode;	//node number of a channel cell

	double
		residualmass,	//residual solids mass in eroded surface layer (g)
		incomingmass,	//solids mass in the incoming subsurface layer (g)
		totalmass,		//total solids mass in the new (popped) surface layer (g)
		totalvolume;	//total volume of the new (popped) surface layer (m3)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//set the surface layer number
				ilayer = nstackov[i][j];

				//if the surface soil layer volume <= the minimum volume
				//trigger AND the number of stack elements > 1
				if(vlayerov[i][j][ilayer] <= minvlayerov[i][j][ilayer] && ilayer > 1)
				{
					//Pop the stack...
					//
					//Add the incoming layer to the residual mass/volume of
					//the surface layer...
					//
					//compute the total (residual + incoming) volume of the new surface layer (m3)
					totalvolume = vlayerov[i][j][ilayer] + vlayerov[i][j][ilayer-1];

					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//compute residual solids mass (g)
						residualmass = csedov[isolid][i][j][ilayer]
							* vlayerov[i][j][ilayer];

						//compute incoming solids mass (g)
						incomingmass = csedov[isolid][i][j][ilayer-1]
							* vlayerov[i][j][ilayer-1];

						//compute total solids mass (g)
						totalmass = residualmass + incomingmass;

						//compute solids concentration in the new surface layer (g/m3)
						csedov[isolid][i][j][ilayer-1] = (float)(totalmass
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
						bursedovoutmass[isolid][i][j][ilayer] =
							bursedovoutmass[isolid][i][j][ilayer] + (float)(residualmass / 1000.0);

						//compute the buried mass entering the new surface layer (kg)
						bursedovinmass[isolid][i][j][ilayer-1] =
							bursedovinmass[isolid][i][j][ilayer-1] + (float)(residualmass / 1000.0);

						//Note:  The layer that used to be at the surface is now
						//       eliminated.  The erosion and deposition flux for
						//       the former surface layer must be set to zero.
						//
						//set the erosion outflux for the former surface layer to zero (g/s)
						erssedovoutflux[isolid][i][j][ilayer] = 0.0;

						//set the deposition influx for the former surface layer to zero (g/s)
						depsedovinflux[isolid][i][j][ilayer] = 0.0;

					}	//end loop over solids

					//if chemical transport is simulated (ksim > 2)
					if(ksim > 2)
					{
						//loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//compute residual chemical mass (g)
							residualmass = cchemov[ichem][i][j][ilayer]
								* vlayerov[i][j][ilayer];

							//compute incoming chemical mass (g)
							incomingmass = cchemov[ichem][i][j][ilayer-1]
								* vlayerov[i][j][ilayer-1];

							//compute total chemical mass (g)
							totalmass = residualmass + incomingmass;

							//compute chemical concentration in the new surface layer (g/m3)
							cchemov[ichem][i][j][ilayer-1] = (float)(totalmass
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
							//                   calculation exist: scrchemovinmass
							//                   and scrchemovoutmass.  However, the
							//                   convention for tracking this mass
							//                   transfer needs further development.
							//
							//compute the buried mass leaving the eroded surface layer (kg)
							burchemovoutmass[ichem][i][j][ilayer] =
								burchemovoutmass[ichem][i][j][ilayer] + (float)(residualmass / 1000.0);

							//compute the buried mass entering the new surface layer (kg)
							burchemovinmass[ichem][i][j][ilayer-1] =
								burchemovinmass[ichem][i][j][ilayer-1] + (float)(residualmass / 1000.0);

							//Note:  The layer that used to be at the surface is now
							//       eliminated.  The erosion and deposition flux for
							//       the former surface layer must be set to zero.
							//
							//set the erosion outflux for the former surface layer to zero (g/s)
							erschemovoutflux[ichem][i][j][ilayer] = 0.0;

							//set the deposition influx for the former surface layer to zero (g/s)
							depchemovinflux[ichem][i][j][ilayer] = 0.0;

						}	//end loop over chemicals

					}	//end if ksim > 2

					//Assign new surface layer volume (and new volume)...
					//
					//Note:  The new volume of the new surface
					//       layer does not need to be reset
					//       because it will be calculated in
					//       OverlandSolidsConcentration...
					//
					//Assign total volume for new surface layer (m3)
					vlayerov[i][j][ilayer-1] = totalvolume;

					//Compute thickness of new surface layer (m)
					hlayerov[i][j][ilayer-1] = (float)(totalvolume
						/ aovground[i][j]);

					//Wipe the old surface layer from the stack
					//
					//set the old layer thickness to zero
					hlayerov[i][j][ilayer] = 0.0;

					//set the old layer volume to zero
					vlayerov[i][j][ilayer] = 0.0;

					//Note:  The "old layer new volume" is the
					//       value of vlayerovnew of the layer
					//       that is being removed from the
					//       stack...
					//
					//set the old layer new volume to zero
					vlayerovnew[i][j][ilayer] = 0.0;

					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//set the solids concentration to zero
						csedov[isolid][i][j][ilayer] = 0.0;

					}	//end loop over solids

					//if chemical transport is simulated (ksim > 2)
					if(ksim > 2)
					{
						//loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//set the chemical concentration to zero
							cchemov[ichem][i][j][ilayer] = 0.0;

						}	//end loop over chemicals

					}	//end if ksim > 2

					//Compute new soil layer geometry...
					//
					//if the overland elevation option > 0
					if(elevovopt > 0)
					{
						//Assign new ground elevation (m)
						elevationov[i][j] = elevlayerov[i][j][ilayer-1];

					}	//end if elevovopt > 0

					//if the cell is a channel cell (imask[][] > 1)
					if(imask[i][j] > 1)
					{
						//get link and node of cell
						chanlink = link[i][j];
						channode = node[i][j];

						//Compute new channel bank height (m)
						hbank[chanlink][channode] = elevationov[i][j]
							- elevationch[chanlink][channode];

						//if the bank height goes to zero (or less)
						if(hbank[chanlink][channode] <= 0.0)
						{
							//Report error type (bank height <= zero) and abort
							SimulationError(8, i, j, nstackov[i][j]+1);

							exit(EXIT_FAILURE);					//abort

						}	//end if hbank[][] <= 0.0

						//compute new channel side slope (run per unit rise)
						sideslope[chanlink][channode] = (float)(0.5
							* (twidth[chanlink][channode] - bwidth[chanlink][channode])
							/ hbank[chanlink][channode]);

					}	//end if imask > 1

					//Decrement the number of layers in the stack
					nstackov[i][j] = nstackov[i][j] - 1;

				}	//end if volume <= minvol AND nstackov[][] > 1

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to NewStateStack
}
