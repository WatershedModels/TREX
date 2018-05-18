/*---------------------------------------------------------------------
C-  Function:	PushSoilStack.c
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
C-              element (surface layer) layer is layer nstackov[i][j].
C-
C-
C-  Inputs:	    nstackov[][], vlayerov[][][], layer properties...
C-
C-  Outputs:    bursedovinmass[][][][], bursedovoutmass[][][][],
C-              burchemovinmass[][][][], burchemovoutmass[][][][],
C-              vlayerovnew[][][], nstackov[][]
C-
C-  Controls:   maxvlayerov[][][], stkovopt, nstackov0[][], maxstackov
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

void PushSoilStack()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		ichem,		//loop index for chemical
		ilayer,		//index for the present position of the surface layer in the stack
		ilayer0,	//index for the original position of the surface layer in the stack
		chanlink,	//link number of a channel cell
		channode;	//node number of a channel cell

	double
		v1,				//volume of layer 1 (before collapse) (m3)
		v2,				//volume of layer 2 (before collapse) (m3)
		combinedvolume,	//combined volume of layers 1 and 2 (m3)
		m1,				//mass of solids/chemicals in layer 1 (before collapse) (g)
		m2,				//mass of solids/chemicals in layer 2 (before collapse) (g)
		combinedmass,	//combined mass of solids/chemicals in layers 1 and 2 (g)
		tmass;			//mass transferred by burial (kg)

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

				//if the surface soil layer volume > the maximum volume
				if(vlayerov[i][j][ilayer] > maxvlayerov[i][j][ilayer])
				{
					//Check for stack collapse...
					//
					//if the present number of layers (nstackov) == the
					//  maximum number of layers (maxstackov) AND the stack
					//  collapse option (stkovopt) == 1 AND the maximum
					//  number of stack elements (maxstackov) > 2
					if(nstackov[i][j] == maxstackov && stkovopt == 1 && maxstackov > 2)
					{
						//Stack collapse occurs...
						//
						//write message to file: alert user of stack collapse
		//mlv						fprintf(echofile_fp, "\n\n\nSoil Stack Collapse:\n");
		//mlv						fprintf(echofile_fp,       "  row = %d  col = %d  time = %9.4f\n",
		//mlv							i, j, simtime);

						//Write message to screen
						printf("\n\n\nSoil Stack Collapse:\n");
						printf(      "  row = %d  col = %d  time = %9.4f\n",
							i, j, simtime);

						//Combine bottom two layers of stack...
						//
						//Note:  When layers collapse, the mass in layer 2
						//       is added to the mass in layer 1.  After the
						//       collapse, the combined mass is in layer 1...
						//
						//volume of layer 2 (upper) (m3)
						v2 = vlayerov[i][j][2];

						//volume of layer 1 (lower) (m3)
						v1 = vlayerov[i][j][1];

						//combined volume of layers 1 and 2 (m3)
						combinedvolume = v1 + v2;

						//Compute properties of the combined (merged) layer...
						//
						//Note:  The ground area of the all soil layers
						//        is constant.
						//
						//set volume of layer after collapse (m3)
						vlayerov[i][j][1] = combinedvolume;

						//Note:  The new layer volume needs to be reset so that
						//       mass balance calculations for subsurface layers
						//       in OverlandChemicalConcentration are correctly
						//       performed.
						//
						//       This is assigned here because layer volume updates
						//       in OverlandSolidsConcentration are only performed
						//       for the surface layer...
						//
						//reset the new layer volume to equal the combined volume (m3)
						vlayerovnew[i][j][1] = combinedvolume;

						//combined layer thickness (m)
						hlayerov[i][j][1] = (float)(combinedvolume / aovground[i][j]);

						//set minimum layer volume after collapse (m3)
						minvlayerov[i][j][1] = (float)(minvolov * combinedvolume);

						//set maximum layer volume after collapse (m3)
						maxvlayerov[i][j][1] = (float)(maxvolov * combinedvolume);

						//set the collapsed layer elevation (m) (top of collapsed surface layer)
						elevlayerov[i][j][1] = elevlayerov[i][j][2];

						//loop over solids
						for(isolid=0; isolid<=nsolids; isolid++)
						{
							//mass of solids in layer 2 (g)
							m2 = csedov[isolid][i][j][2] * v2;

							//mass of solids in layer 1 (g)
							m1 = csedov[isolid][i][j][1] * v1;

							//combined mass of layers 1 and 2 (g)
							combinedmass = m1 + m2;

							//solids concentration in layer 1 after collapse (g/m3)
							csedov[isolid][i][j][1] = (float)(combinedmass / combinedvolume);

							//Update mass balance terms...
							//
							//Note:  For the purpose of mass balance accounting,
							//       mass moved during a collapse is treated as
							//       a mass loss (burial out) from Layer 2 and
							//       a gain (burial in) to Layer 1.
							//
							//compute the buried mass leaving Layer 2 (kg)
							bursedovoutmass[isolid][i][j][2] =
								bursedovoutmass[isolid][i][j][2] + (float)(m2 / 1000.0);

							//compute the buried mass entering Layer 1 (kg)
							bursedovinmass[isolid][i][j][1] =
								bursedovinmass[isolid][i][j][1] + (float)(m2 / 1000.0);

						}	//end loop over solids

						//if chemical transport is simulated
						if(ksim > 2)
						{
							//loop over chemicals
							for(ichem=1; ichem<=nchems; ichem++)
							{
								//mass of chemical in layer 2 (g)
								m2 = cchemov[ichem][i][j][2] * v2;

								//mass of chemical in layer 1 (g)
								m1 = cchemov[ichem][i][j][1] * v1;

								//combined mass of layers 1 and 2 (g)
								combinedmass = m1 + m2;

								//chemical concentration in layer 1 after collapse (g/m3)
								cchemov[ichem][i][j][1] = (float)(combinedmass / combinedvolume);
						
								//Update mass balance terms...
								//
								//Note:  For the purpose of mass balance accounting,
								//       mass moved during a collapse is treated as
								//       a mass loss (burial out) from Layer 2 and
								//       a gain (burial in) to Layer 1.
								//
								//compute the buried mass leaving Layer 2 (kg)
								burchemovoutmass[ichem][i][j][2] =
									burchemovoutmass[ichem][i][j][2] + (float)(m2 / 1000.0);

								//compute the buried mass entering Layer 1 (kg)
								burchemovinmass[ichem][i][j][1] =
									burchemovinmass[ichem][i][j][1] + (float)(m2 / 1000.0);

							}	//end loop over chemicals

						}	//end if ksim > 2

						//Push the remaining elements in the stack...
						//
						//Note:  At this point a void (empty layer) exists in
						//       position 2 in the stack.  The layers between
						//       the void and the top layer of the stack (i.e.
						//       layers 3 through nstackov[][]) must each be
						//       moved down the stack until the void layer is
						//       at the top of the stack (nstackov[][]).  The
						//       number of stack elements is then decremented
						//       by one: nstackov[][] = nstackov[][] - 1...
						//       
						//Move remaining layers down one element in the stack...
						//
						//loop over pushed layers
						for(k=3; k<=nstackov[i][j]; k++)
						{
							//push thickness (m)
							hlayerov[i][j][k-1] = hlayerov[i][j][k];

							//push layer volume (m3)
							vlayerov[i][j][k-1] = vlayerov[i][j][k];

							//push new layer volume (m3)
							vlayerovnew[i][j][k-1] = vlayerovnew[i][j][k];

							//push minimum layer volume (m3)
							minvlayerov[i][j][k-1] = minvlayerov[i][j][k];

							//push maximum layer volume (m3)
							maxvlayerov[i][j][k-1] = maxvlayerov[i][j][k];

							//push layer elevation (m)
							elevlayerov[i][j][k-1] = elevlayerov[i][j][k];

							//loop over solids
							for(isolid=0; isolid<=nsolids; isolid++)
							{
								//push solids concentration (g/m3)
								csedov[isolid][i][j][k-1] = csedov[isolid][i][j][k];

								//Update mass balance terms...
								//
								//Note:  For the purpose of mass balance accounting,
								//       mass moved during a collapse is treated as
								//       a mass loss (burial out) from Layer 2 and
								//       a gain (burial in) to Layer 1.
								//
								//Compute solids mass transferred by burial (kg)
								tmass = csedov[isolid][i][j][k]
									* vlayerov[i][j][k] / 1000.0;

								//compute the buried mass leaving the upper pushed layer (kg)
								bursedovoutmass[isolid][i][j][k] =
									bursedovoutmass[isolid][i][j][k] + tmass;

								//compute the buried mass entering the lower pushed layer (kg)
								bursedovinmass[isolid][i][j][k-1] =
									bursedovinmass[isolid][i][j][k-1] + tmass;

							}	//end loop over solids

							//if chemical transport is simulated
							if(ksim > 2)
							{
								//loop over chemicals
								for(ichem=1; ichem<=nchems; ichem++)
								{
									//push chemical concentration (g/m3)
									cchemov[ichem][i][j][k-1] = cchemov[ichem][i][j][k];

									//Update mass balance terms...
									//
									//Note:  For the purpose of mass balance accounting,
									//       mass moved during a collapse is treated as
									//       a mass loss (burial out) from Layer 2 and
									//       a gain (burial in) to Layer 1.
									//
									//Compute chemical mass transferred by burial (kg)
									tmass = cchemov[ichem][i][j][k]
										* vlayerov[i][j][k] / 1000.0;

									//compute the buried mass leaving the upper pushed layer (kg)
									burchemovoutmass[ichem][i][j][k] =
										burchemovoutmass[ichem][i][j][k] + tmass;

									//compute the buried mass entering the lower pushed layer (kg)
									burchemovinmass[ichem][i][j][k-1] =
										burchemovinmass[ichem][i][j][k-1]+ tmass;
							
								}	//end loop over chemicals

							}	//end if ksim > 2

						}	//end loop over pushed layers

						//Decrement the stack counter
						nstackov[i][j] = nstackov[i][j] - 1;

					}	//end if nstackov[][] = maxstackov && stkovopt = 1 && maxstackov > 2

					//Push the stack (after any collapse)...
					//
					//Note:  If the stack is allowed to collapse, there
					//       will always be room for another layer in
					//       the stack (i.e. nstackov < maxstackov)...
					//
					//if there is room for another layer in the stack
					if(nstackov[i][j] < maxstackov)
					{
						//Set the layer number for the new surface layer
						ilayer = nstackov[i][j] + 1;

						//Assign properties for new surface layer...
						//
						//Note:  The properties assigned to the new layer
						//       depend on the new layer's position in the
						//       stack.  If the new layer is replacing an
						//       old layer that had eroded earlier during
						//       the simulation (nstackov <= nstackov0),
						//       the properties (width, thickness, volume)
						//       are the same as the old layer.  If the new
						//       layer is above the initial soil-water (air)
						//       interface (nstackov > nstackov0), the
						//       properties are the same as those of the
						//       initial surface soil layer.
						//
						//if the layer added is below the orignal soil-
						//  water (air) interface (ilayer <= nstackov0[i][j])
						if(ilayer <= nstackov0[i][j])
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
							//       OverlandSolidsConcentration...
							//
							//Compute volume of new surface layer (after split) (m3)
							vlayerov[i][j][ilayer] = vlayerov[i][j][ilayer-1]
								- vlayerov0[i][j][ilayer-1];

							//set the thickness of the new surface layer (m)
							hlayerov[i][j][ilayer] = (float)(vlayerov[i][j][ilayer] / aovground[i][j]);

							//compute the new minimum layer volume (m3)
							minvlayerov[i][j][ilayer] = (float)(minvolov
								* vlayerov0[i][j][ilayer]);

							//compute the new maximum layer volume (m3)
							maxvlayerov[i][j][ilayer] = (float)(maxvolov
								* vlayerov0[i][j][ilayer]);

							//set the new layer elevation (m) (top of new surface layer)
							elevlayerov[i][j][ilayer] = elevlayerov[i][j][ilayer-1]
								+ hlayerov[i][j][ilayer];

							//if the overland elevation option > 0
							if(elevovopt > 0)
							{
								//set the new soil elevation (m) (top of new surface layer)
								elevationov[i][j] = elevlayerov[i][j][ilayer];

							}	//end if elevovopt > 0

							//Conditions for the pushed layer after split...
							//
							//Note:  The mass pushed down the stack is the
							//       mass remaining in Layer ilayer-1.
							//
							//Restore original volume of Layer ilayer-1 (m3)
							vlayerov[i][j][ilayer-1] = vlayerov0[i][j][ilayer-1];

							//Note:  The new layer volume needs to be reset so that
							//       mass balance calculations for subsurface layers
							//       in OverlandChemicalConcentration are correctly
							//       performed.
							//
							//       This is assigned here because layer volume updates
							//       in OverlandSolidsConcentration are only performed
							//       for the surface layer...
							//
							//reset the new layer volume to equal the present volume (m3)
							vlayerovnew[i][j][ilayer-1] = vlayerov[i][j][ilayer-1];

							//Restore original thickness of Layer ilayer-1 (m)
							hlayerov[i][j][ilayer-1] = hlayerov0[i][j][ilayer-1];

							//Restore original minimum volume of Layer ilayer-1 (m3)
							minvlayerov[i][j][ilayer-1] = (float)(minvolov
								* vlayerov0[i][j][ilayer-1]);

							//Restore original maximum volume of Layer ilayer-1 (m3)
							maxvlayerov[i][j][ilayer-1] = (float)(maxvolov
								* vlayerov0[i][j][ilayer-1]);
						}
						else	//else nstackov > nstackov0
						{
							//get the original position of the surface layer
							ilayer0 = nstackov0[i][j];

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
							//       OverlandSolidsConcentration...
							//
							//Compute volume of new surface layer after split (m3)
							vlayerov[i][j][ilayer] = vlayerov[i][j][ilayer-1]
								- vlayerov0[i][j][ilayer0];

							//set the thickness of the new surface layer after split (m)
							hlayerov[i][j][ilayer] = (float)(vlayerov[i][j][ilayer] / aovground[i][j]);

							//set the minimum volume of new surface layer after split (m3)
							minvlayerov[i][j][ilayer] = (float)(minvolov
								* vlayerov0[i][j][ilayer0]);

							//set the maximum volume of new surface layer after split (m3)
							maxvlayerov[i][j][ilayer] = (float)(maxvolov
								* vlayerov0[i][j][ilayer0]);

							//set the new layer elevation (m) (top of new surface layer)
							elevlayerov[i][j][ilayer] = elevlayerov[i][j][ilayer-1]
								+ hlayerov[i][j][ilayer];

							//if the overland elevation option > 0
							if(elevovopt > 0)
							{
								//set the new soil elevation (m) (top of new surface layer)
								elevationov[i][j] = elevlayerov[i][j][ilayer];

							}	//end if elevovopt > 0

							//Note:  The soil type of the new surface
							//       layer must be set.  This also sets
							//       the porosity for the new layer...
							//
							//set the soil type of the new surface layer
							soiltype[i][j][ilayer] = soiltype[i][j][ilayer0];

							//Note:  The mass pushed down the stack is the
							//       mass remaining in Layer ilayer-1.
							//
							//Restore original volume of Layer ilayer-1 (m3)
							vlayerov[i][j][ilayer-1] = vlayerov0[i][j][ilayer0];

							//Note:  The new layer volume needs to be reset so that
							//       mass balance calculations for subsurface layers
							//       in OverlandChemicalConcentration are correctly
							//       performed.
							//
							//       This is assigned here because layer volume updates
							//       in OverlandSolidsConcentration are only performed
							//       for the surface layer...
							//
							//reset the new layer volume to equal the present volume (m3)
							vlayerovnew[i][j][ilayer-1] = vlayerov[i][j][ilayer-1];

							//Restore original thickness of Layer ilayer-1 (m)
							hlayerov[i][j][ilayer-1] = hlayerov0[i][j][ilayer0];

							//Restore original minimum volume of Layer ilayer-1 (m3)
							minvlayerov[i][j][ilayer-1] = (float)(minvolov
								* vlayerov0[i][j][ilayer0]);

							//Restore original maximum volume of Layer ilayer-1 (m3)
							maxvlayerov[i][j][ilayer-1] = (float)(maxvolov
								* vlayerov0[i][j][ilayer0]);

						}	//end if nstackov < nstackov0

						//Assign concentrations to the new surface layer...
						//
						//loop over solids
						for(isolid=0; isolid<=nsolids; isolid++)
						{
							//Assign solids concentrations (g/m3)
							csedov[isolid][i][j][ilayer] = csedov[isolid][i][j][ilayer-1];

							//Compute solids mass transferred by burial (kg)
							tmass = csedov[isolid][i][j][ilayer-1]
								* vlayerov[i][j][ilayer-1] / 1000.0;

							//compute cumulative solids mass leaving the surface layer (kg)
							bursedovoutmass[isolid][i][j][ilayer] =
								bursedovoutmass[isolid][i][j][ilayer] + tmass;

							//compute the cumulative solids mass entering the subsurface layer (kg)
							bursedovinmass[isolid][i][j][ilayer-1] =
								bursedovinmass[isolid][i][j][ilayer-1] + tmass;

							//Note:  The layer that used to be at the surface is now a
							//       subsurface layer.  The erosion and deposition flux
							//       for the former surface layer must be set to zero.
							//
							//set the erosion outflux for the former surface layer (now a subsurface layer) to zero (g/s)
							erssedovoutflux[isolid][i][j][ilayer-1] = 0.0;

							//set the deposition influx for the former surface layer (now a subsurface layer) to zero (g/s)
							depsedovinflux[isolid][i][j][ilayer-1] = 0.0;

						}	//end loop over solids

						//if chemical transport is simulated (ksim > 2)
						if(ksim > 2)
						{
							//loop over chemicals
							for(ichem=1; ichem<=nchems; ichem++)
							{
								//Assign chemical concentrations (g/m3)
								cchemov[ichem][i][j][ilayer] = cchemov[ichem][i][j][ilayer-1];

								//Compute chemical mass transferred by burial (kg)
								tmass = cchemov[ichem][i][j][ilayer-1]
									* vlayerov[i][j][ilayer-1] / 1000.0;

								//compute cumulative chemical mass leaving the surface layer (kg)
								burchemovoutmass[ichem][i][j][ilayer] =
									burchemovoutmass[ichem][i][j][ilayer] + tmass;

								//compute the cumulative chemical mass entering the subsurface layer (kg)
								burchemovinmass[ichem][i][j][ilayer-1] =
									burchemovinmass[ichem][i][j][ilayer-1] + tmass;

								//Note:  The layer that used to be at the surface is now a
								//       subsurface layer.  The erosion and deposition flux
								//       for the former surface layer must be set to zero.
								//
								//set the erosion outflux for the former surface layer (now a subsurface layer) to zero (g/s)
								erschemovoutflux[ichem][i][j][ilayer-1] = 0.0;

								//set the deposition influx for the former surface layer (now a subsurface layer) to zero (g/s)
								depchemovinflux[ichem][i][j][ilayer-1] = 0.0;

							}	//end loop over chemicals

						}	//end if ksim > 2

						//if the cell is a chennel cell (imask > 1)
						if(imask[i][j] > 1)
						{
							//get link and node of cell
							chanlink = link[i][j];
							channode = node[i][j];

							//compute new channel bank height (m)
							hbank[chanlink][channode] = elevationov[i][j]
								- elevationch[chanlink][channode];

							//compute new channel side slope (run per unit rise)
							sideslope[chanlink][channode] = (float)(0.5
								* (twidth[chanlink][channode] - bwidth[chanlink][channode])
								/ hbank[chanlink][channode]);

						}	//end if imask[][] > 1
					}
					else	//else all stack elements full (nstackov = maxstackov)
					{
						//if the stack collapse option is zero (no collapse)
						if(stkovopt == 0)
						{
							//Stack full...
							//
							//write message to file: alert user of full stack
							fprintf(echofile_fp, "\n\n\nSoil Stack Full:\n\n");
							fprintf(echofile_fp,     "  Exceeded maximum number of layers. Set\n");
							fprintf(echofile_fp,     "  stkovopt = 1 or increase maxstackov...\n\n");
							fprintf(echofile_fp,     "  row = %d  col = %d  time = %9.4f\n\n",
								i, j, simtime);
							fprintf(echofile_fp,     "  stkovopt = %d  maxstackov = %d\n",
								stkovopt, maxstackov);

							//Write message to screen
							printf("\n\n\nSoil Stack Full:\n\n");
							printf(    "  Exceeded maximum number of layers. Set\n");
							printf(    "  stkovopt = 1 or increase maxstackov...\n\n");
							printf(    "  row = %d  col = %d  time = %9.4f\n\n",
								i, j, simtime);
							printf(    "  stkovopt = %d  maxstackov = %d \n",
								stkovopt, maxstackov);

							//Report error type (stack full) and abort
							SimulationError(10, i, j, nstackov[i][j]+1);
						}
						else	//else stkovopt == 1
						{
							//Stack set to collapse but full condition found...
							//
							//write error to file: error in PushSoilStack
							fprintf(echofile_fp, "\n\n\nSoil Stack Error:\n\n");
							fprintf(echofile_fp,     "  Exceeded maximum number of layers but\n");
							fprintf(echofile_fp,     "  stkovopt = 1 for stack collapse...\n\n");
							fprintf(echofile_fp,     "  row = %d  col = %d  time = %9.4f\n\n",
								i, j, simtime);
							fprintf(echofile_fp,     "  stkovopt = %d  maxstackov = %d \n\n",
								stkovopt, maxstackov);
							fprintf(echofile_fp,     "  Check PushSoilStack for errors!!\n");

							//Write message to screen
							printf("\n\n\nSoil Stack Error:\n\n");
							printf(    "  Exceeded maximum number of layers but\n");
							printf(    "  stkovopt = 1 for stack collapse...\n\n");
							printf(    "  row = %d  col = %d  time = %9.4f\n\n",
								i, j, simtime);
							printf(    "  stkovopt = %d  maxstackov = %d \n\n",
								stkovopt, maxstackov);
							printf(    "  Check PushSoilStack for errors!!\n");

						}	//end if stkovopt = 0

						//Close the echo file
						fclose(echofile_fp);

						exit(EXIT_FAILURE);					//abort

					}	//end if nstackov[i][j] < maxstackov

					//Note:  The old surface layer has now been split into
					//       two layers and the total number of layers in
					//       the stack has increased by one.
					//
					//increment the stack counter
					nstackov[i][j] = nstackov[i][j] + 1;

				}	//end if vlayerov[][][] >= maxvlayerov[][][]

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to NewStateStack
}
