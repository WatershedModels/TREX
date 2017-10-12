/*---------------------------------------------------------------------
C-  Function:	PushSedimentStack.c
C-
C-	Purpose/	Compute the downward re-indexing of solids and chemicals
C-	Methods:	in the sediment stack of the channel network.
C-                        
C-              Volume is conserved.  The surface area between layers
C-              can differ.  When the maximum volume trigger is reached
C-              the surface layer is split into two, a portion of the
C-              layer volume is pushed down the stack with the remaining
C-              becomes the new surface layer.  The new surface layer
C-              thickness is be computed from the new layer volume and
C-              surface area.  Because the surface area can differ from 
C-              layer to layer, the total thickness of the new surface
C-              layer and old surface layer (after the split) may not
C-              equal the thickness of the one layer before the split.
C-              As a result, "jump" discontinuities in the thickness
C-              and elevation of the surface layer if changes in surface
C-              area between any two layers in the stack are large.
C-
C-              Stack elements are numbered in reverse order.  The
C-              bottom (deepest) element is layer one.  The top-most
C-              element (surface layer) layer is layer nstackch[i][j].
C-
C-
C-  Inputs:	    nstackch[][], vlayerch[][][], layer properties...
C-
C-  Outputs:    bursedchinmass[][][][], bursedchoutmass[][][][],
C-              burchemchinmass[][][][], burchemchoutmass[][][][],
C-              vlayerchnew[][][], nstackch[][]
C-
C-  Controls:   maxvlayerch[][][], stkchopt, nstackch0[][], maxstackch
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
C-	Revised:	Mark Velleux
C-              HydroQual, Inc. (personal time)
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-	Date:		06-SEP-2007
C-
C-	Revisions:	Revised double precision calculations for mass
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

void PushSedimentStack()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		ichem,		//loop index for chemical
		ilayer,		//index for the present position of the surface layer in the stack
		ilayer0,	//index for the original position of the surface layer in the stack
		row,		//row number of node
		col;		//column number of node

	double
		v1,				//volume of layer 1 (before collapse) (m3)
		v2,				//volume of layer 2 (before collapse) (m3)
		combinedvolume,	//combined volume of layers 1 and 2 (m3)
		m1,				//mass of solids/chemicals in layer 1 (before collapse) (g)
		m2,				//mass of solids/chemicals in layer 2 (before collapse) (g)
		combinedmass,	//combined mass of solids/chemicals in layers 1 and 2 (g)
		tmass;			//mass transferred by burial (kg)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//set the surface layer number
			ilayer = nstackch[i][j];

			//if the surface sediment layer volume > the maximum volume
			if(vlayerch[i][j][ilayer] > maxvlayerch[i][j][ilayer])
			{
				//Check for stack collapse...
				//
				//if the present number of layers (nstackch) == the
				//  maximum number of layers (maxstackch) AND the stack
				//  collapse option (stkchopt) == 1 AND the maximum
				//  number of stack elements (maxstackch) > 2
				if(nstackch[i][j] == maxstackch && stkchopt == 1 && maxstackch > 2)
				{
					//Stack collapse occurs...
					//
					//write message to file: alert user of stack collapse
//mlv					fprintf(echofile_fp, "\n\n\nSediment Stack Collapse:\n");
//mlv					fprintf(echofile_fp,       "  link = %d  node = %d  time = %9.4f\n",
//mlv						i, j, simtime);

					//Write message to screen
					printf("\n\n\nSediment Stack Collapse:\n");
					printf(      "  link = %d  node = %d  time = %9.4f\n",
						i, j, simtime);

					//Combine bottom two layers of stack...
					//
					//Note:  When layers collapse, the mass in layer 2
					//       is added to the mass in layer 1.  After the
					//       collapse, the combined mass is in layer 1...
					//
					//volume of layer 2 (upper) (m3)
					v2 = vlayerch[i][j][2];

					//volume of layer 1 (lower) (m3)
					v1 = vlayerch[i][j][1];

					//combined volume of layers 1 and 2 (m3)
					combinedvolume = v1 + v2;

					//Compute properties of the combined (merged) layer...
					//
					//Note:  The bed area of the merged layer is assumed
					//       to be the bed area of the upper layer that
					//       was collapsed into the bottom layer.  This
					//       can cause discontinuties between the sum
					//       of layer thicknesses and the elevation of
					//       each layer.  The elevation of the top
					//       interface of the merged layer is assumed
					//       to equal the elevation of the upper layer
					//       prior to the collapse.
					//
					//set volume of layer after collapse (m3)
					vlayerch[i][j][1] = combinedvolume;

					//Note:  The new layer volume needs to be reset so that
					//       mass balance calculations for subsurface layers
					//       in ChannelChemicalConcentration are correctly
					//       performed.
					//
					//       This is assigned here because layer volume updates
					//       in ChannelSolidsConcentration are only performed
					//       for the surface layer...
					//
					//reset the new layer volume to equal the combined volume (m3)
					vlayerchnew[i][j][1] = combinedvolume;

					//set bed area after collapse (m2)
					achbed[i][j][1] = achbed[i][j][2];

					//combined layer thickness (m)
					hlayerch[i][j][1] = (float)(combinedvolume / achbed[i][j][2]);

					//set minimum layer volume after collapse (m3)
					minvlayerch[i][j][1] = (float)(minvolch * combinedvolume);

					//set maximum layer volume after collapse (m3)
					maxvlayerch[i][j][1] = (float)(maxvolch * combinedvolume);

					//set the collapsed layer elevation (m) (top of collapsed surface layer)
					elevlayerch[i][j][1] = elevlayerch[i][j][2];

					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//mass of solids in layer 2 (g)
						m2 = csedch[isolid][i][j][2] * v2;

						//mass of solids in layer 1 (g)
						m1 = csedch[isolid][i][j][1] * v1;

						//combined mass of layers 1 and 2 (g)
						combinedmass = m1 + m2;

						//solids concentration in layer 1 after collapse (g/m3)
						csedch[isolid][i][j][1] = (float)(combinedmass / combinedvolume);

						//Update mass balance terms...
						//
						//Note:  For the purpose of mass balance accounting,
						//       mass moved during a collapse is treated as
						//       a mass loss (burial out) from Layer 2 and
						//       a gain (burial in) to Layer 1.
						//
						//compute the buried mass leaving Layer 2 (kg)
						bursedchoutmass[isolid][i][j][2] =
							bursedchoutmass[isolid][i][j][2] + (float)(m2 / 1000.0);

						//compute the buried mass entering Layer 1 (kg)
						bursedchinmass[isolid][i][j][1] =
							bursedchinmass[isolid][i][j][1] + (float)(m2 / 1000.0);

					}	//end loop over solids

					//if chemical transport is simulated
					if(ksim > 2)
					{
						//loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//mass of chemical in layer 2 (g)
							m2 = cchemch[ichem][i][j][2] * v2;

							//mass of chemical in layer 1 (g)
							m1 = cchemch[ichem][i][j][1] * v1;

							//combined mass of layers 1 and 2 (g)
							combinedmass = m1 + m2;

							//chemical concentration in layer 1 after collapse (g/m3)
							cchemch[ichem][i][j][1] = (float)(combinedmass / combinedvolume);
						
							//Update mass balance terms...
							//
							//Note:  For the purpose of mass balance accounting,
							//       mass moved during a collapse is treated as
							//       a mass loss (burial out) from Layer 2 and
							//       a gain (burial in) to Layer 1.
							//
							//compute the buried mass leaving Layer 2 (kg)
							burchemchoutmass[ichem][i][j][2] =
								burchemchoutmass[ichem][i][j][2] + (float)(m2 / 1000.0);

							//compute the buried mass entering Layer 1 (kg)
							burchemchinmass[ichem][i][j][1] =
								burchemchinmass[ichem][i][j][1] + (float)(m2 / 1000.0);

						}	//end loop over chemicals

					}	//end if ksim > 2

					//Push the remaining elements in the stack...
					//
					//Note:  At this point a void (empty layer) exists in
					//       position 2 in the stack.  The layers between
					//       the void and the top layer of the stack (i.e.
					//       layers 3 through nstackch[][]) must each be
					//       moved down the stack until the void layer is
					//       at the top of the stack (nstackch[][]).  The
					//       number of stack elements is then decremented
					//       by one: nstackch[][] = nstackch[][] - 1...
					//       
					//Move remaining layers down one element in the stack...
					//
					//loop over pushed layers
					for(k=3; k<=nstackch[i][j]; k++)
					{
						//push thickness (m)
						hlayerch[i][j][k-1] = hlayerch[i][j][k];

						//push bottom width (m)
						bwlayerch[i][j][k-1] = bwlayerch[i][j][k];

						//push bed area (m2)
						achbed[i][j][k-1] = achbed[i][j][k];

						//push layer volume (m3)
						vlayerch[i][j][k-1] = vlayerch[i][j][k];

						//push new layer volume (m3)
						vlayerchnew[i][j][k-1] = vlayerchnew[i][j][k];

						//push minimum layer volume (m3)
						minvlayerch[i][j][k-1] = minvlayerch[i][j][k];

						//push maximum layer volume (m3)
						maxvlayerch[i][j][k-1] = maxvlayerch[i][j][k];

						//push layer elevation (m)
						elevlayerch[i][j][k-1] = elevlayerch[i][j][k];

						//loop over solids
						for(isolid=1; isolid<=nsolids; isolid++)
						{
							//push solids concentration (g/m3)
							csedch[isolid][i][j][k-1] = csedch[isolid][i][j][k];

							//Update mass balance terms...
							//
							//Note:  For the purpose of mass balance accounting,
							//       mass moved during a collapse is treated as
							//       a mass loss (burial out) from Layer 2 and
							//       a gain (burial in) to Layer 1.
							//
							//
							//Compute solids mass transferred by burial (kg)
							tmass = csedch[isolid][i][j][k]
								* vlayerch[i][j][k] / 1000.0;

							//compute the buried mass leaving the upper pushed layer (kg)
							bursedchoutmass[isolid][i][j][k] =
								bursedchoutmass[isolid][i][j][k] + tmass;

							//compute the buried mass entering the lower pushed layer (kg)
							bursedchinmass[isolid][i][j][k-1] =
								bursedchinmass[isolid][i][j][k-1] + tmass;

						}	//end loop over solids

						//if chemical transport is simulated
						if(ksim > 2)
						{
							//loop over chemicals
							for(ichem=1; ichem<=nchems; ichem++)
							{
								//push chemical concentration (g/m3)
								cchemch[ichem][i][j][k-1] = cchemch[ichem][i][j][k];

								//Update mass balance terms...
								//
								//Note:  For the purpose of mass balance accounting,
								//       residual mass from an eroded surface layer
								//       is treated as a mass loss (burial out) from
								//       the eroded layer and a gain (burial in) to
								//       the incoming (new) surface layer.
								//
								//Compute chemical mass transferred by burial (kg)
								tmass = cchemch[ichem][i][j][k]
									* vlayerch[i][j][k] / 1000.0;

								//compute the buried mass leaving the upper pushed layer (kg)
								burchemchoutmass[ichem][i][j][k] =
									burchemchoutmass[ichem][i][j][k] + tmass;

								//compute the buried mass entering the lower pushed layer (kg)
								burchemchinmass[ichem][i][j][k-1] =
									burchemchinmass[ichem][i][j][k-1] + tmass;
						
							}	//end loop over chemicals

						}	//end if ksim > 2

					}	//end loop over pushed layers

					//Decrement the stack counter
					nstackch[i][j] = nstackch[i][j] - 1;

				}	//end if nstackch[][] = maxstackch && stkchopt = 1 && maxstackch > 2

				//Push the stack (after any collapse)...
				//
				//Note:  If the stack is allowed to collapse, there
				//       will always be room for another layer in
				//       the stack (i.e. nstackch < maxstackch)...
				//
				//if there is room for another layer in the stack
				if(nstackch[i][j] < maxstackch)
				{
					//Set the layer number for the new surface layer
					ilayer = nstackch[i][j] + 1;

					//Assign properties for new surface layer...
					//
					//Note:  The properties assigned to the new layer
					//       depend on the new layer's position in the
					//       stack.  If the new layer is replacing an
					//       old layer that had eroded earlier during
					//       the simulation (nstackch <= nstackch0),
					//       the properties (width, thickness, volume)
					//       are the same as the old layer.  If the new
					//       layer is above the initial sediment-water
					//       interface (nstackch > nstackch0), the
					//       properties are the same as those of the
					//       initial surface sediment layer.
					//
					//if the layer added is below the orignal sediment-
					//  water interface (ilayer <= nstackch0[i][j])
					if(ilayer <= nstackch0[i][j])
					{
						//Conditions for the new surface layer...
						//
						//Note:  The added layer is the new surface
						//       layer and is constructed from the
						//       mass now in Layer ilayer-1...
						//
						//Compute surface layer volume (and new volume)...
						//
						//Note:  The new volume of the new surface
						//       layer does not need to be reset
						//       because it will be calculated in
						//       ChannelSolidsConcentration...
						//
						//Compute volume of new surface layer (after split) (m3)
						vlayerch[i][j][ilayer] = vlayerch[i][j][ilayer-1]
							- vlayerch0[i][j][ilayer-1];

						//Restore the original bed area of the new surface layer (m2)
						achbed[i][j][ilayer] = achbed0[i][j][ilayer];

						//Restore the original bottom width of the new surface layer (m)
						bwlayerch[i][j][ilayer] = bwlayerch0[i][j][ilayer];

						//set the thickness of the new surface layer (m)
						hlayerch[i][j][ilayer] = (float)(vlayerch[i][j][ilayer] / achbed[i][j][ilayer]);

						//compute the new minimum layer volume (m3)
						minvlayerch[i][j][ilayer] = (float)(minvolch
							* vlayerch0[i][j][ilayer]);

						//compute the new maximum layer volume (m3)
						maxvlayerch[i][j][ilayer] = (float)(maxvolch
							* vlayerch0[i][j][ilayer]);

						//set the new layer elevation (m) (top of new surface layer)
						elevlayerch[i][j][ilayer] = elevlayerch[i][j][ilayer-1]
							+ hlayerch[i][j][ilayer];

						//if the channel elevation option > 0
						if(elevchopt > 0)
						{
							//set the new bed elevation (m) (top of new surface layer)
							elevationch[i][j] = elevlayerch[i][j][ilayer];

						}	//end if elevchopt > 0

						//Conditions for the pushed layer after split...
						//
						//Note:  The mass pushed down the stack is the
						//       mass remaining in Layer ilayer-1.
						//
						//Restore original volume of Layer ilayer-1 (m3)
						vlayerch[i][j][ilayer-1] = vlayerch0[i][j][ilayer-1];

						//Note:  The new layer volume needs to be reset so that
						//       mass balance calculations for subsurface layers
						//       in ChannelChemicalConcentration are correctly
						//       performed.
						//
						//       This is assigned here because layer volume updates
						//       in ChannelSolidsConcentration are only performed
						//       for the surface layer...
						//
						//reset the new layer volume to equal the present volume (m3)
						vlayerchnew[i][j][ilayer-1] = vlayerch[i][j][ilayer-1];

						//Restore original bed area of Layer ilayer-1 (m2)
						achbed[i][j][ilayer-1] = achbed0[i][j][ilayer-1];

						//Restore original bottom width of Layer ilayer-1 (m)
						bwlayerch[i][j][ilayer-1] = bwlayerch0[i][j][ilayer-1];

						//Restore original thickness of Layer ilayer-1 (m)
						hlayerch[i][j][ilayer-1] = hlayerch0[i][j][ilayer-1];

						//Restore original minimum volume of Layer ilayer-1 (m3)
						minvlayerch[i][j][ilayer-1] = (float)(minvolch
							* vlayerch0[i][j][ilayer-1]);

						//Restore original maximum volume of Layer ilayer-1 (m3)
						maxvlayerch[i][j][ilayer-1] = (float)(maxvolch
							* vlayerch0[i][j][ilayer-1]);
					}
					else	//else nstackch >= nstackch0
					{
						//get the original position of the surface layer
						ilayer0 = nstackch0[i][j];

						//Conditions for the new surface layer...
						//
						//Note:  The added layer is the new surface
						//       layer and is constructed from the
						//       mass now in Layer ilayer-1...
						//
						//Compute surface layer volume (and new volume)...
						//
						//Note:  The new volume of the new surface
						//       layer does not need to be reset
						//       because it will be calculated in
						//       ChannelSolidsConcentration...
						//
						//Compute volume of new surface layer after split (m3)
						vlayerch[i][j][ilayer] = vlayerch[i][j][ilayer-1]
							- vlayerch0[i][j][ilayer0];

						//Set the bed area of the new surface layer (m2)
						achbed[i][j][ilayer] = achbed0[i][j][ilayer0];

						//Set the bottom width of the new surface layer (m)
						bwlayerch[i][j][ilayer] = bwlayerch0[i][j][ilayer0];

						//set the standard thickness of the new surface layer after split (m)
						hlayerch[i][j][ilayer] = (float)(vlayerch[i][j][ilayer] / achbed[i][j][ilayer]);

						//set the minimum volume of new surface layer after split (m3)
						minvlayerch[i][j][ilayer] = (float)(minvolch
							* vlayerch0[i][j][ilayer0]);

						//set the maximum volume of new surface layer after split (m3)
						maxvlayerch[i][j][ilayer] = (float)(maxvolch
							* vlayerch0[i][j][ilayer0]);

						//set the new layer elevation (m) (top of new surface layer)
						elevlayerch[i][j][ilayer] = elevlayerch[i][j][ilayer-1]
							+ hlayerch[i][j][ilayer];

						//if the channel elevation option > 0
						if(elevchopt > 0)
						{
							//set the new bed elevation (m) (top of new surface layer)
							elevationch[i][j] = elevlayerch[i][j][ilayer];

						}	//end if elevchopt > 0

						//set the porosity of the new surface layer
						porositych[i][j][ilayer] = porositych[i][j][ilayer0];

						//Note:  The mass pushed down the stack is the
						//       mass remaining in Layer ilayer-1.
						//
						//Restore original volume of Layer ilayer-1 (m3)
						vlayerch[i][j][ilayer-1] = vlayerch0[i][j][ilayer0];

						//Note:  The new layer volume needs to be reset so that
						//       mass balance calculations for subsurface layers
						//       in ChannelChemicalConcentration are correctly
						//       performed.
						//
						//       This is assigned here because layer volume updates
						//       in ChannelSolidsConcentration are only performed
						//       for the surface layer...
						//
						//reset the new layer volume to equal the present volume (m3)
						vlayerchnew[i][j][ilayer-1] = vlayerch[i][j][ilayer-1];

						//Restore original bed area of Layer ilayer-1 (m2)
						achbed[i][j][ilayer-1] = achbed0[i][j][ilayer0];

						//Restore original bottom width of Layer ilayer-1 (m)
						bwlayerch[i][j][ilayer-1] = bwlayerch0[i][j][ilayer0];

						//Restore original thickness of Layer ilayer-1 (m)
						hlayerch[i][j][ilayer-1] = hlayerch0[i][j][ilayer0];

						//Restore original minimum volume of Layer ilayer-1 (m3)
						minvlayerch[i][j][ilayer-1] = (float)(minvolch
							* vlayerch0[i][j][ilayer0]);

						//Restore original maximum volume of Layer ilayer-1 (m3)
						maxvlayerch[i][j][ilayer-1] = (float)(maxvolch
							* vlayerch0[i][j][ilayer0]);

					}	//end if nstackch < nstackch0

					//Assign concentrations to the new surface layer...
					//
					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//Assign solids concentrations (g/m3)
						csedch[isolid][i][j][ilayer] = csedch[isolid][i][j][ilayer-1];

						//Compute solids mass transferred by burial (kg)
						tmass = csedch[isolid][i][j][ilayer-1]
							* vlayerch[i][j][ilayer-1] / 1000.0;

						//compute cumulative solids mass leaving the surface layer (kg)
						bursedchoutmass[isolid][i][j][ilayer] =
							bursedchoutmass[isolid][i][j][ilayer] + tmass;

						//compute the cumulative solids mass entering the subsurface layer (kg)
						bursedchinmass[isolid][i][j][ilayer-1] =
							bursedchinmass[isolid][i][j][ilayer-1] + tmass;

						//Note:  The layer that used to be at the surface is now a
						//       subsurface layer.  The erosion and deposition flux
						//       for the former surface layer must be set to zero.
						//
						//set the erosion outflux for the former surface layer (now a subsurface layer) to zero (g/s)
						erssedchoutflux[isolid][i][j][ilayer-1] = 0.0;

						//set the deposition influx for the former surface layer (now a subsurface layer) to zero (g/s)
						depsedchinflux[isolid][i][j][ilayer-1] = 0.0;

					}	//end loop over solids

					//if chemical transport is simulated (ksim > 2)
					if(ksim > 2)
					{
						//loop over chemicals
						for(ichem=1; ichem<=nchems; ichem++)
						{
							//Assign chemical concentrations (g/m3)
							cchemch[ichem][i][j][ilayer] = cchemch[ichem][i][j][ilayer-1];

							//Compute chemical mass transferred by burial (kg)
							tmass = cchemch[ichem][i][j][ilayer-1]
								* vlayerch[i][j][ilayer-1] / 1000.0;

							//compute cumulative chemical mass leaving the surface layer (kg)
							burchemchoutmass[ichem][i][j][ilayer] =
								burchemchoutmass[ichem][i][j][ilayer] + tmass;

							//compute the cumulative chemical mass entering the subsurface layer (kg)
							burchemchinmass[ichem][i][j][ilayer-1] =
								burchemchinmass[ichem][i][j][ilayer-1] + tmass;

							//Note:  The layer that used to be at the surface is now a
							//       subsurface layer.  The erosion and deposition flux
							//       for the former surface layer must be set to zero.
							//
							//set the erosion outflux for the former surface layer (now a subsurface layer) to zero (g/s)
							erschemchoutflux[ichem][i][j][ilayer-1] = 0.0;

							//set the deposition influx for the former surface layer (now a subsurface layer) to zero (g/s)
							depchemchinflux[ichem][i][j][ilayer-1] = 0.0;

						}	//end loop over chemicals

					}	//end if ksim > 2

					//Compute new channel geometry...
					//
					//set bottom width (m)
					bwidth[i][j] = bwlayerch[i][j][ilayer];

					//get row and column of node
					row = ichnrow[i][j];
					col = ichncol[i][j];

					//compute new channel bank height (m)
					hbank[i][j] = elevationov[row][col] - elevationch[i][j];

					//if the bank height goes to zero (or less)
					if(hbank[i][j] <= 0.0)
					{
						//Report error type (bank height <= zero) and abort
						SimulationError(9, i, j, nstackch[i][j]+1);

						exit(EXIT_FAILURE);					//abort

					}	//end if hbank[][] <= 0.0

					//compute new channel side slope (run per unit rise)
					sideslope[i][j] = (float)(0.5 * (twidth[i][j] - bwidth[i][j])
						/ hbank[i][j]);
				}
				else	//else all stack elements full (nstackch = maxstackch)
				{
					//Open simulation echo file in append mode
					echofile_fp = fopen(echofile,"a");

					//if the stack collapse option is zero (no collapse)
					if(stkchopt == 0)
					{
						//Stack full...
						//
						//write message to file: alert user of full stack
						fprintf(echofile_fp, "\n\n\nSediment Stack Full:\n\n");
						fprintf(echofile_fp,     "  Exceeded maximum number of layers. Set\n");
						fprintf(echofile_fp,     "  stkchopt = 1 or increase maxstackch...\n\n");
						fprintf(echofile_fp,     "  link = %d  node = %d  time = %9.4f\n\n",
							i, j, simtime);
						fprintf(echofile_fp,     "  stkchopt = %d  maxstackch = %d\n",
							stkchopt, maxstackch);

						//Write message to screen
						printf("\n\n\nSediment Stack Full:\n\n");
						printf(    "  Exceeded maximum number of layers. Set\n");
						printf(    "  stkchopt = 1 or increase maxstackch...\n\n");
						printf(    "  link = %d  node = %d  time = %9.4f\n\n",
							i, j, simtime);
						printf(    "  stkchopt = %d  maxstackch = %d \n",
							stkchopt, maxstackch);

						//Report error type (stack full) and abort
						SimulationError(11, i, j, nstackch[i][j]);
					}
					else	//else nstackch = maxstackch (the stack is full)
					{
						//Stack set to collapse but full condition found...
						//
						//write error to file: error in PushSedimentStack
						fprintf(echofile_fp, "\n\n\nSediment Stack Error:\n\n");
						fprintf(echofile_fp,     "  Exceeded maximum number of layers but\n");
						fprintf(echofile_fp,     "  stkchopt = 1 for stack collapse...\n\n");
						fprintf(echofile_fp,     "  link = %d  node = %d  time = %9.4f\n\n",
							i, j, simtime);
						fprintf(echofile_fp,     "  stkchopt = %d  maxstackch = %d \n\n",
							stkchopt, maxstackch);
						fprintf(echofile_fp,     "  Check PushSedimentStack for errors!!\n");

						//Write message to screen
						printf("\n\n\nSediment Stack Error:\n\n");
						printf(    "  Exceeded maximum number of layers but\n");
						printf(    "  stkchopt = 1 for stack collapse...\n\n");
						printf(    "  link = %d  node = %d  time = %9.4f\n\n",
							i, j, simtime);
						printf(    "  stkchopt = %d  maxstackch = %d \n\n",
							stkchopt, maxstackch);
						printf(    "  Check PushSedimentStack for errors!!\n");

					}	//end if stkchopt = 0

					//Close the echo file
					fclose(echofile_fp);

					exit(EXIT_FAILURE);					//abort

				}	//end if nstackch[i][j] < maxstackch

				//Note:  The old surface layer has now been split into
				//       two layers and the total number of layers in
				//       the stack has increased by one.
				//
				//increment the stack counter
				nstackch[i][j] = nstackch[i][j] + 1;

			}	//end if vlayerch[][][] >= maxvlayerch[][][]

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to NewStateStack
}
