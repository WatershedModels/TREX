/*----------------------------------------------------------------------
C-  Function:	ComputeChemicalGroupConcSums.c
C-
C-	Purpose/	Compute sums of chemical state variable concentrations
C-  Methods:    for chemical reporting groups as specified in Data
C-              Group D.  Concentration are determined as specified by
C-              the following values (itype = ...):
C-
C-              0 = water column (ilayer = 0)
C-              1 = surface soil/sediment (ilayer = nstackov/nstackch)
C-              2 = subsurface soil/sediment (ilayer = reporting depth)
C-
C-
C-	Inputs:		igrid, jgrid, itype, horizon, 
C-
C-	Outputs:	cgroupsum[]
C-
C-	Controls:   nchems, ncgroups, imask[][]
C-
C-	Calls:		none
C-
C-	Called by:	WriteGridsChemical
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		12-SEP-2004
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		13-JUN-2008
C-
C-	Revisions:  Revised function argument list to provide an index
C-              to determine whether concentration sums are for water
C-              column or surface soil/sediment.  Use itype argument
C-              as a switch (water or bed) and then set array index
C-              for layer as ilayer (water: ilayer = 0; bed: ilayer =
C-              nstackov[][] or nstackch[][]).  Added code to compute
C-              sorbed phase concentrations (iphase = 4).
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		23-AUG-2010
C-
C-	Revisions:  Added option to report concentrations in subsurface
C-              soil/sediment layers (itype = 2).  The subsurface
C-              layer is determined by the reporting horizon (depth
C-              below grade).  Concentrations are reported as point
C-              values (concentration in a layer) or depth-weighted
C-              averages depending on the value of ssropt (0 = point
C-              values, 1 = depth-averaged).  Depth-averaged values
C-              calculated as the thickness (depth) average of all
C-              layers between the surface and reporting horizon.
C-
C-	Revised:
C-
C-	Date:		
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void ComputeChemicalGroupConcSums(float *cgroupsum, int igrid, int jgrid, int itype, int iphase)
{
	//Local variable declarations/definitions
	int
		igroup,		//local counter for reporting group number
		ichem,		//local counter for chemical type
		isolid,		//local counter for solids type
		chanlink,	//link number of channel in channel cell
		channode,	//node number of channel in channel cell
		rlayer,		//layer to summarize results (reporting layer) (water column or soil/sediment) (loop limit)
		ilayer,		//layer to summarize results (water column or soil/sediment) (loop counter, array index)
		toplayer;	//topmost layer for depth averaging (subsurface soil/sediment)

	float
		rhe,		//reporting horizon elevation (m)
		fraction,	//fraction of chemical in phase iphase
		thickness,	//thickness of layer (water column, stack surface, etc.) (m)
		totaldepth;	//total depth, set to layer thickness or reporting horizon (m)

	//initialize chemical group sums...
	//
	//loop over chemical groups
	for(igroup=1; igroup<=ncgroups; igroup++)
	{
        //set group values to zero
		cgroupsum[igroup] = 0.0;

	}	//end loop over chemical groups

	//if the cell is a channel cell
	if(imask[igrid][jgrid] > 1)
	{
		//set channel link number
		chanlink = link[igrid][jgrid];

		//set channel node number
		channode = node[igrid][jgrid];

		//set layer reference based on itype
		//
		//if itype = 0 (water column)
		if(itype == 0)
		{
			//set the reporting layer to the water column
			rlayer = 0;

			//set the top layer to the water column
			toplayer = 0;

			//set the thickness to the water depth (m)
			thickness = hch[chanlink][channode];

			//set the total depth to the thickness (no averaging) (m)
			totaldepth = thickness;
		}
		else if(itype == 1)	//else itype = 1 (surface sediment layer)
		{
			//set the reporting layer to the surface sediment layer
			rlayer = nstackch[chanlink][channode];

			//set the top layer to the surface sediment layer
			toplayer = nstackch[chanlink][channode];

			//set the thickness to the reporting layer thickness (volume / area) (m)
			thickness = (float)(vlayerch[chanlink][channode][rlayer] / achbed[chanlink][channode][rlayer]);

			//set the total depth to the thickness (no averaging) (m)
			totaldepth = thickness;
		}
		else	//else itype > 1 (itype = 2) (subsurface sediment layer)
		{
			//set the reporting layer to the surface sediment layer
			rlayer = nstackch[chanlink][channode];

			//Reporting horizon elevation and layer...
			//
			//Note:  The reporting horizon is a fixed depth below the
			//       sediment surface.  As surface elevations change
			//       over time, the reporting horizon elevation
			//       "floats" so it is always a fixed depth below
			//       grade.
			//
			//compute the reporting horizon elevation (m)
			rhe = elevationch[chanlink][channode] - horizon;

			//loop over layers in reverse order: top down...
			//
			//while the layer bottom elevation > horizon (and it is not stack bottom)
			while(elevlayerch[chanlink][channode][rlayer-1] > rhe && rlayer >= 1)
			{
				//decrement the reporting layer (look deeper in the stack)
				rlayer = rlayer - 1;

			}	//end while elevlayerch[][][rlayer-1] > rhe && rlayer >= 1

			//Note:  If the reporting horizon elevation falls below the
			//       elevation at the bottom of the stack, it is reset
			//       to the bottom elevation.
			//
			//if the reporting horizon is below the bottom of the stack
			if(rhe < elevlayerch[chanlink][channode][0])
			{
				//reset the reporting horizon elevation to the stack bottom
				rhe = elevlayerch[chanlink][channode][0];

			}	//end if rhe < elevlayerch[][][0]

			//if ssropt is zero (subsurface concentration reported as point value)
			if(ssropt == 0)
			{
				//set the top layer to the subsurface sediment layer (reporting layer)
				toplayer = rlayer;

				//set the thickness to the reporting layer thickness (volume / area) (m)
				thickness = (float)(vlayerch[chanlink][channode][rlayer] / achbed[chanlink][channode][rlayer]);

				//set the total depth to the thickness (no averaging) (m)
				totaldepth = thickness;
			}
			else	//else if ssropt is not zero (subsurface concentration reported as average)
			{
				//set the top layer to the surface sediment layer
				toplayer = nstackch[chanlink][channode];

				//Note:  Thickness is determined in the loop over layers
				//       (below).

				//Determine total depth...
				//
				//Note:  Also, total depth over which averaging occurs is
				//       determined from elevation at the top of the stack
				//       and reporting horizon elevation.  The total depth
				//       may be zero if total mass of soilds (expressed as
				//       a height) is zero (i.e. all layers eroded) and the
				//       reporting horizon is reset to the bottom elevation
				//       of the stack.
				//
				//set total depth for averaging (m)
				totaldepth = elevationch[chanlink][channode] - rhe;

			}	//end if ssropt = 0

		}	//end if itype = 0, 1, ...

		//Note:  Reporting occurs for either a single layer or
		//       an average of multiple layers in the stack.
		//       When point values are reported, rlayer and
		//       toplayer have the same value.
		//
		//loop over layers for reporting
		for(ilayer=toplayer;ilayer>=rlayer;ilayer--)
		{
			//Determine layer thickness
			//
			//if itype is 2 (subsurface) and ssropt is > 0 (depth-weighted average)
			if(itype == 2 && ssropt > 0)
			{
				//if the layer ends above the reporting horizon elevation
				if(elevlayerch[chanlink][channode][ilayer-1] >= rhe)
				{
					//Layer is completely within reporting horizon...
					//
					//set the thickness to the layer thickness (volume / area) (m)
					thickness = (float)(vlayerch[chanlink][channode][ilayer] / achbed[chanlink][channode][ilayer]);
				}
				else	//else the layer ends below the reporting horizon
				{
					//Layer is partially within reporting horizon...
					//
					//set the thickness to thickness above horizon elevation (volume / area - ...) (m)
					thickness = (float)(vlayerch[chanlink][channode][ilayer] / achbed[chanlink][channode][ilayer]
						-  (rhe - elevlayerch[chanlink][channode][ilayer-1]));

				}	//end if elevlayerch[][][ilayer-1] >= rhe

			}	//end if itype = 2 and ssropt > 0

			//loop over chemicals
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//get group number for this chemical type
				igroup = cgroupnumber[ichem];

				//Set phase/fraction of chemical
				//
				//if the phase = 1 (dissolved)
				if(iphase == 1)
				{
					//the fraction is the dissolved fraction
					fraction = fdissolvedch[ichem][chanlink][channode][ilayer];
				}
				//else if the phase = 2 (bound)
				else if(iphase == 2)
				{
					//the fraction is the dissolved fraction
					fraction = fboundch[ichem][chanlink][channode][ilayer];
				}
				//else if the phase = 3 (particulate)
				else if(iphase == 3)
				{
					//initialize the fraction
					fraction = 0.0;

					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//the fraction is the sum of particulate fractions
						fraction = fraction +
							fparticulatech[ichem][isolid][chanlink][channode][ilayer];

					}	//end loop over solids
				}
				//else if the phase = 4 (sorbed)
				else if(iphase == 4)
				{
					//initialize the fraction
					fraction = 0.0;

					//Note:  For the water column, the sorbed chemical concentration
					//       (mg/kg) is computed as particlulate phase concentration
					//       (g/m3) divided by total solids concentration (g/m3) and
					//       multiplied by a factor of 1.0e6 to convert from g/g to
					//       mg/kg (ug/g).
					//
					//       For the soil or sediment bed, the sorbed concentration
					//       is computed as total chemical concentration divided by
					//       total solids concentration (g/m3) and multiplied by a
					//       factor of 1.0e6 to convert from g/g to mg/kg (ug/g).
					//
					//if itype = 0 (water column)
					if(itype == 0)
					{
						//loop over solids
						for(isolid=1; isolid<=nsolids; isolid++)
						{
							//the fraction is the sum of particulate fractions
							fraction = fraction +
								fparticulatech[ichem][isolid][chanlink][channode][ilayer];

						}	//end loop over solids
					}
					else	//else itype != 0 (sediment bed)
					{
						//the fraction is the total (always 1.0)
						fraction = 1.0;

					}	//end if itype = 0

					//if the solids concentration > 0
					if(csedch[0][chanlink][channode][ilayer] > 0.0)
					{
						//multiply by conversion from g/m3 to mg/kg
						fraction = (float)(fraction / csedch[0][chanlink][channode][ilayer]
							* 1.0e6);
					}
					else	//else (solids concentration = 0)
					{
						//set the fraction to zero
						fraction = 0.0;

					}	//end if solids concentration > 0
				}
				else	//else the phase = 0 (total chemical)
				{
					//the fraction is the total (always 1.0)
					fraction = 1.0;

				}	//end if iphase = 1

				//if the total depth is greater than zero
				if(totaldepth > 0.0)
				{
					//Note:  Group concentration sums are reported as either
					//       point (single layer) or depth averaged (multiple
					//       layer) concentrations.  For point concentrations
					//       layer thickness and total depth values are equal.
					//
					//add the concentration of each chemical type to its reporting group
					cgroupsum[igroup] = cgroupsum[igroup]
						+ cchemch[ichem][chanlink][channode][ilayer] * fraction
						* thickness / totaldepth;

				}	//end if totaldepth > 0.0

			}	//end loop over chemicals

		}	//end loop over averaging layers
	}
	else	//else the cell is an overland cell
	{
		//set layer reference based on itype
		//
		//if itype = 0 (water column)
		if(itype == 0)
		{
			//set the reporting layer to the water column
			rlayer = 0;

			//set the top layer to the water column
			toplayer = 0;

			//set the thickness to the water depth (m)
			thickness = hov[igrid][jgrid];

			//set the total depth to the thickness (no averaging) (m)
			totaldepth = thickness;
		}
		else if(itype == 1)	//else itype = 1 (surface sediment layer)
		{
			//set the reporting layer to the surface sediment layer
			rlayer = nstackov[igrid][jgrid];

			//set the top layer to the surface sediment layer
			toplayer = nstackov[igrid][jgrid];

			//set the thickness to the reporting layer thickness (volume / area) (m)
			thickness = (float)(vlayerov[igrid][jgrid][rlayer] / (w * w));

			//set the total depth to the thickness (no averaging) (m)
			totaldepth = thickness;
		}
		else	//else itype > 1 (itype = 2) (subsurface sediment layer)
		{
			//set the reporting layer to the surface sediment layer
			rlayer = nstackov[igrid][jgrid];

			//Reporting horizon elevation and layer...
			//
			//Note:  The reporting horizon is a fixed depth below the
			//       sediment surface.  As surface elevations change
			//       over time, the reporting horizon elevation
			//       "floats" so it is always a fixed depth below
			//       grade.
			//
			//compute the reporting horizon elevation (m)
			rhe = elevationov[igrid][jgrid] - horizon;

			//loop over layers in reverse order: top down...
			//
			//while the layer bottom elevation > horizon (and it is not stack bottom)
			while(elevlayerov[igrid][jgrid][rlayer-1] > rhe && rlayer >= 1)
			{
				//decrement the reporting layer (look deeper in the stack)
				rlayer = rlayer - 1;

			}	//end while elevlayerov[][][rlayer-1] > rhe && rlayer >= 1

			//Note:  If the reporting horizon elevation falls below the
			//       elevation at the bottom of the stack, it is reset
			//       to the bottom elevation.
			//
			//if the reporting horizon is below the bottom of the stack
			if(rhe < elevlayerov[igrid][jgrid][0])
			{
				//reset the reporting horizon elevation to the stack bottom
				rhe = elevlayerov[igrid][jgrid][0];

			}	//end if rhe < elevlayerov[][][0]

			//if ssropt is zero (subsurface concentration reported as point value)
			if(ssropt == 0)
			{
				//set the reporting top layer to the subsurface sediment layer
				toplayer = rlayer;

				//set the thickness to the reporting layer thickness (volume / area) (m)
				thickness = (float)(vlayerov[igrid][jgrid][rlayer] / (w * w));

				//set the total depth to the thickness (no averaging) (m)
				totaldepth = thickness;
			}
			else	//else if ssropt is not zero (subsurface concentration reported as average)
			{
				//set the top layer to the surface sediment layer
				toplayer = nstackov[igrid][jgrid];

				//Note:  Thickness is determined in the loop over layers
				//       (below).

				//Determine total depth...
				//
				//Note:  Also, total depth over which averaging occurs is
				//       determined from elevation at the top of the stack
				//       and reporting horizon elevation.  The total depth
				//       may be zero if total mass of soilds (expressed as
				//       a height) is zero (i.e. all layers eroded) and the
				//       reporting horizon is reset to the bottom elevation
				//       of the stack.
				//
				//set total depth for averaging (m)
				totaldepth = elevationov[igrid][jgrid] - rhe;

			}	//end if ssropt = 0

		}	//end if itype = 0, 1, ...

		//Note:  Reporting occurs for either a single layer or
		//       an average of multiple layers in the stack.
		//       When point values are reported, rlayer and
		//       toplayer have the same value.
		//
		//loop over layers for reporting
		for(ilayer=toplayer;ilayer>=rlayer;ilayer--)
		{
			//Determine layer thickness
			//
			//if itype is 2 (subsurface) and ssropt is > 0 (depth-weighted average)
			if(itype == 2 && ssropt > 0)
			{
				//if the layer ends above the reporting horizon elevation
				if(elevlayerov[igrid][jgrid][ilayer-1] >= rhe)
				{
					//Layer is completely within reporting horizon...
					//
					//set the thickness to the layer thickness (volume / area) (m)
					thickness = (float)(vlayerov[igrid][jgrid][ilayer] / (w * w));
				}
				else	//else the layer ends below the reporting horizon
				{
					//Layer is partially within reporting horizon...
					//
					//set the thickness to thickness above horizon elevation (volume / area - ...) (m)
					thickness = (float)(vlayerov[igrid][jgrid][ilayer] / (w * w)
						-  (rhe - elevlayerov[igrid][jgrid][ilayer-1]));

				}	//end if elevlayerov[][][ilayer-1] >= rhe

			}	//end if itype = 2 and ssropt > 0

			//loop over chemicals
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//get group number for this chemical type
				igroup = cgroupnumber[ichem];

				//Set phase/fraction of chemical
				//
				//if the phase = 1 (dissolved)
				if(iphase == 1)
				{
					//the fraction is the dissolved fraction
					fraction = fdissolvedov[ichem][igrid][jgrid][ilayer];
				}
				//else if the phase = 2 (bound)
				else if(iphase == 2)
				{
					//the fraction is the dissolved fraction
					fraction = fboundov[ichem][igrid][jgrid][ilayer];
				}
				//else if the phase = 3 (particulate)
				else if(iphase == 3)
				{
					//initialize the fraction
					fraction = 0.0;

					//loop over solids
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//the fraction is the sum of particulate fractions
						fraction = fraction +
							fparticulateov[ichem][isolid][igrid][jgrid][ilayer];

					}	//end loop over solids
				}
				//else if the phase = 4 (sorbed)
				else if(iphase == 4)
				{
					//initialize the fraction
					fraction = 0.0;

					//Note:  For the water column, the sorbed chemical concentration
					//       (mg/kg) is computed as particlulate phase concentration
					//       (g/m3) divided by total solids concentration (g/m3) and
					//       multiplied by a factor of 1.0e6 to convert from g/g to
					//       mg/kg (ug/g).
					//
					//       For the soil or sediment bed, the sorbed concentration
					//       is computed as total chemical concentration divided by
					//       total solids concentration (g/m3) and multiplied by a
					//       factor of 1.0e6 to convert from g/g to mg/kg (ug/g).
					//
					//if itype = 0 (water column)
					if(itype == 0)
					{
						//loop over solids
						for(isolid=1; isolid<=nsolids; isolid++)
						{
							//the fraction is the sum of particulate fractions
							fraction = fraction +
								fparticulateov[ichem][isolid][igrid][jgrid][ilayer];

						}	//end loop over solids
					}
					else	//else itype != 0 (sediment bed)
					{
						//the fraction is the total (always 1.0)
						fraction = 1.0;

					}	//end if itype = 0

					//if the solids concentration > 0
					if(csedov[0][igrid][jgrid][ilayer] > 0.0)
					{
						//multiply by conversion from g/m3 to mg/kg (ug/g)
						fraction = (float)(fraction / csedov[0][igrid][jgrid][ilayer]
							* 1.0e6);
					}
					else	//else (solids concentration = 0)
					{
						//set the fraction to zero
						fraction = 0.0;

					}	//end if solids concentration > 0
				}
				else	//else the phase = 0 (total chemical)
				{
					//the fraction is the total (always 1.0)
					fraction = 1.0;

				}	//end if iphase = 1

				//if the total depth is greater than zero
				if(totaldepth > 0.0)
				{
					//Note:  Group concentration sums are reported as either
					//       point (single layer) or depth averaged (multiple
					//       layer) concentrations.  For point concentrations
					//       layer thickness and total depth values are equal.
					//
					//add the concentration of each chemical type to its reporting group
					cgroupsum[igroup] = cgroupsum[igroup]
						+ cchemov[ichem][igrid][jgrid][ilayer] * fraction
						* thickness / totaldepth;

				}	//end if totaldepth > 0.0

			}	//end loop over chemicals

		}	//end loop over averaging layers

	}	//end if imask[][] > 1
				
//End of function: Return to WriteGridsChemical
}
