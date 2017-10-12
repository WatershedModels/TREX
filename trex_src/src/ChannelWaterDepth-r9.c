/*----------------------------------------------------------------------
C-  Function:	ChannelWaterDepth.c
C-
C-	Purpose/	Computes channel water depth hch[][] in each node in the
C-	Methods:	channel network.  If depth in a node becomes unstable (a
C-				negative channel depth occurs), then aborts routine and
C-              calls Simulation Error to report error message.
C-				
C-
C-	Inputs:		dqch[][], bwidth[][], hbank[][], sideslope[][],
C-              deadstoragedepth[][], chanlength[][], dt[], hov[][],
C-              ichnrow[][], ichncol[][]  (Globals)
C-
C-	Outputs:	hchnew[][], hovnew[][] (values at time t+dt)(Globals)
C-              vchintermediate[][]
C-
C-	Controls:   ksim
C-
C-	Calls:		SimulationError
C-
C-	Called by:	WaterBalance
C-
C-	Created:	Original Coding: Rosalia Rojas-Sanchez
C-              Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (last version)
C-
C-	Revised:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-				
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		03-SEP-2003
C-
C-	Revisions:	Document variables and main computations, correct
C-				variable names; correct channel lengths	orientated
C-              in 8 directions; added trapezoidal channel geometry
C-				(rectangular/triangle special cases).
C-
C-  Revised:	Mark Velleux (CSU)
C-
C-  Date:		05-May-2005
C-
C-  Revisions:	Code to control floodplain water transfer between
C-              overland plane and channels moved to a separate
C-              process routine (FloodplainWaterTransfer).
C-
C-  Revised:    Mark Velleux
C-
C-	Date:       18-DEC-2006
C-
C-	Revisions:  Bug fix: corrected direct rain input, replaced
C-              netrainrate with grossrainrate.  Added direct
C-              snowfall input by grossswerate (assumes channel
C-              never freezes/has ice cover.
C-
C-	Revised:	Mark Velleux
C-				Masa Takamatsu
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		29-Jul-2010
C-
C-	Revisions:	Cumulative mass balance calculations were moved to
C-              ChannelWaterDepth to implement time step option
C-              (dtopt).
C-
C-  Revised:
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

void ChannelWaterDepth()
{
	//local variable declarations/definitions
	int 
		row,		//row number of present (upstream) node
		col;		//column number of present (upstream) node

	float
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		lch,		//channel length (m) (includes sinuosity)
		achnew,		//new cross-section area of water in channel (m2)
		achsurf,	//surface area of channel (m2)
		abed,		//surface area of channel bed (m2)
		vch,		//volume of water in channel (m3) (at depth hch)
		vchin,		//inflowing volume of water in channel from upstream node (m3)
		vchnew,		//new volume of water in channel (m3) (after flow transfer)
		vchbank,	//volume of water channel can hold at top of bank (m3)
		vchavail,	//available (unfilled) volume of channel before it is filled to top of bank (m3)
		twch;		//top width of channel at top of bank (m)

	float
		derivative;	//volumetric derivative (m3/s)

	//Compute Surface Water Balance...
	//
	//The depth of water in the channel network is the sum of water
	//inputs from net rainfall (equals gross rainfall, no interception),
	//external water sources, and internal water transfers, (flooding
	//and return flow from channels).
	//
	//Developer's Note:  Evaporation would be subtracted from the water
	//                   balance as well.  However, this feature is not
	//                   included in the present version...
	//
	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Loop over number of nodes for current link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Determine row/column location for current link/node
			row  = ichnrow[i][j];		//Row number of link i, node j
			col  = ichncol[i][j];		//Col number of link i, node j

			//Assign channel characteristics (present link/node)
			bwch = bwidth[i][j];				//channel bottom width (m)
			hbch = hbank[i][j];					//channel bank height (m)
			sslope = sideslope[i][j];			//channel side slope
			lch = chanlength[i][j];				//channel length (m) (includes sinuosity)
			twch = twidth[i][j];				//channel top width at top of bank
			vchbank = (bwch + sslope * hbch) * hbch * lch;	//channel volume at top of bank (m)

			//Compute surface area of channel...
			//
			//The channel (and overland) surface area of a cell is
			//constant and defined by the top width of the channel
			//at the bank height and the channel length (including
			//sinuosity):
			//
			//  achsurf = twch * lch
			//
			//Surface area of channel (m2)
			achsurf = twch * lch;

			//Build derivative term by term...
			//
			//  derivative = grossrainrate[row][col]*area
			//             + grossswerate[row][col]*area
			//             - translossrate[i][j]*area
			//             + dqch[i][j]
			//
			//    Gross rainfall and swefall represent
			//    rainfall and snowfall directly onto
			//    the surface water area of a channel 
			//    cell.  No interception occurs over the
			//    channel part of a cell.
			//
			//    Rainfall and snowfall directly onto a
			//    channel must be added to the net rain
			//
			//Note:  The derivative has units of m3/s.
			//
			//Start with net rain (m3/s) (apportioned for channel surface area)
			derivative = netrainrate[row][col] * achsurf;

			//Add net snowfall as swe (m3/s) (if simulated)
			if(snowopt > 0) derivative = derivative + netswerate[row][col] * achsurf;

			//Subtract channel transmission loss (m3/s) (if simulated)
			if(ctlopt > 0) derivative = derivative - (translossrate[i][j] * bwch * lch);

			//Add net channel flow rate (includes floodplain transfer)
			derivative = derivative + dqch[i][j];

			//Compute volume of water entering (or leaving) this node
			//over this time step
			vchin = derivative * dt[idt];		//channel inflow volume (m3)

			//Compute water volume in channel...
			//
			//if the starting channel water depth is less than the bank height
			if(hch[i][j] < hbch)
			{
				//Compute volume of water in channel: (b+zy)y * length
				vch = (float)(bwch + sslope * hch[i][j])
					* hch[i][j] * lch;

				//Compute available volume in channel (from top of
				//water surface to top of bank)
				vchavail = vchbank - vch;

				//if inflowing volume < = available volume in channel
				if(vchin <= vchavail)
				{
					//Add inflowing volume to channel...
					//
					//Compute new water volume in channel
					vchnew = vch + vchin;

					//Compute new channel cross-section area (m2)
					achnew = vchnew / lch;

					//Compute new channel depth...
					//
					//if sslope > 0 (the channel is trapezoidal or triangular)
					if(sslope > 0)
					{
						//Compute new channel depth (positive root from quadratic eqn)
						hchnew[i][j] = (float)((-bwch + sqrt(pow(bwch,2.0)
							- 4.0*sslope*(-achnew)))/(2.0*sslope));
					}
					else	//sslope = 0 (channel is rectangular)
					{
						//Compute new channel depth (from area and width)
						hchnew[i][j] = achnew / bwch;

					}	//end if sslope > 0
				}
				else	//inflowing volume exceeds available volume in channel
				{
					//First, fill channel to top of bank...
					//
					//New (interim) channel depth is bank height
					hchnew[i][j] = hbch;

					//Compute remaining inflowing volume
					vchin = vchin - vchavail;

					//Second, add remaining volume as a rectangular block...
					//
					//Set new channel depth
					hchnew[i][j] = hchnew[i][j] + vchin / achsurf;

				}	//end if vchin <= vchavail
			}
			else	//channel water depth is >= bank height
			{
				//Add all inflowing volume as a rectangular block
				//
				//Set new channel depth
				hchnew[i][j] = hch[i][j] + vchin / achsurf;

			}	//end if hch[][] < hbank

			//If the new channel depths is negative
			if(hchnew[i][j] < 0.0)
			{
				//Check for round-off error...
				//
				//if the magnitude is < the error tolerance
				if((float)fabs(hchnew[i][j]) < TOLERANCE)
				{
					//The error is round-off...
					//
					//Reset the new channel depth to exactly zero
					hchnew[i][j] = 0.0;
				}	
				else	//the error is numerical instability
				{
					//Report error type (negative water depth in channel) and location
					SimulationError(3, i, j, 0);

					exit(EXIT_FAILURE);	//abort

				}	//end check for round-off error

			}	//end if hchnew < 0.0

			//if the new depth is less than the TOLERANCE
			if(hchnew[i][j] < TOLERANCE)
			{
				//reset the depth to zero
				hchnew[i][j] = 0.0;

			}	//end if hchnew[i][j] < TOLERANCE

			/****************************************************/
			/*     Update Cumulative Mass Balance Variables     */
			/****************************************************/

			//Note:  The cumulative net rain and snow mass balances
			//       assume that no interception occurs over the the
			//       surface water part of channel cells.  Net rain
			//       and snow volumes are updated to account for direct
			//       precipitation to channels.
			//
			//Increment the cumulative net rainfall volume entering the channel part of a cell (m3)
			netrainvol[row][col] = netrainvol[row][col]
				+ grossrainrate[row][col] * achsurf * dt[idt];

			//if snowfall is simulated as falling snow
			if(snowopt > 0 && snowopt <= 3)
			{
				//Compute the cumulative net snowfall volume entering the channel part a cell (m3)
				netswevol[row][col] = netswevol[row][col]
					+ grossswerate[row][col] * achsurf * dt[idt];

			}	//endif snowopt > 1 && snowopt <= 3

			//Transmission Loss...
			//
			//if transmission loss is simulated
			if(ctlopt > 0)
			{
				//Compute the cumulative depth of transmission loss (m)
				translossdepth[i][j] = translossdepth[i][j]
					+ translossrate[i][j] * dt[idt];
				
				//Compute minimum transmission loss depth (cumulative) for node
				mintranslossdepth = Min(translossdepth[i][j],mintranslossdepth);

				//Compute maximum transmission loss depth (cumulative) for node
				maxtranslossdepth = Max(translossdepth[i][j],maxtranslossdepth);

				//Note:  achbed is a global sediment transport variable that
				//       holds the values of the channel bed surface area (m2).
				//       However, global sediment variables are not known to
				//       hydrology modules so the bed surface area must be
				//       computed locally from bottom width and length.
				//
				//Compute bed area of channel...
				//
				//Surface area of channel bed (m2)
				abed = bwch * lch;

				//Compute cumulative transmission loss volume for this node (m3)
				translossvol[i][j] = translossvol[i][j]
					+ translossrate[i][j] * abed * dt[idt];

			}	//end if ctlopt > 0

			//Gross outflow and inflow volumes...
			//
			//Note:  Gross flow volumes are calculated for
			//       connections in the 8 physical (raster)
			//       directions for channel conectivity.
			//
			//       Calculations for point sources (k=0),
			//       floodplains (k=9), and outlets (k=10)
			//       occur in separate loops...
			//
			//Loop over directions
			for(k=1; k<=8; k++)
			{
				//increment cumulative node outflow volume (m3)
				dqchoutvol[i][j][k] = dqchoutvol[i][j][k]
					+ dqchout[i][j][k] * dt[idt];

				//increment cumulative node inflow volume (m3)
				dqchinvol[i][j][k] = dqchinvol[i][j][k]
					+ dqchin[i][j][k] * dt[idt];

			}	//end loop over directions (1-8)

		}	//end loop over nodes

		//Cumulative mass balance for water volume at domain boundaries
		//
		//Note:  Flow for link outflow and inflow (qoutch[] and qinch[])
		//       are initialized to zero each time ChannelWaterRoute is
		//       called.  For links that do not connect to a boundary,
		//       the flow is always zero.  Only links that connect to
		//       a boundary can have a non-zero value.  For any link at
		//       any level, flow will be non-zero in one direction (out
		//       or in).  However, the logic needed to determine it a
		//       a link connects to an outlet requires a number of lines
		//       and controlling logic.  For simplicity, the code just
		//       performs the volume mass balance for all links even
		//       though most values are zero in order to reduce code
		//       complexity.
		//
		//Compute cumulative flow volume exiting domain from this link (m3)
		qoutchvol[i] = qoutchvol[i] + qoutch[i] * dt[idt];

		//Compute cumulative flow volume enterng domain from this link (m3)
		qinchvol[i] = qinchvol[i] + qinch[i] * dt[idt];

		//set node number for last node of link
		j = nnodes[i];

		//set direction for domain boundary (k=10)
		k = 10;

		//increment cumulative boundary node outflow volume (m3)
		dqchoutvol[i][j][10] = dqchoutvol[i][j][10]
			+ dqchout[i][j][10] * dt[idt];

		//increment cumulative boundary node inflow volume (m3)
		dqchinvol[i][j][10] = dqchinvol[i][j][10]
			+ dqchin[i][j][10] * dt[idt];

	}	//end loop over links

	//Cumulative mass balance for channel flow point sources...
	//
	//Loop over number of external flow sources
	for(k=1; k<=nqwch; k++)
	{
		//set link and node references for the flow point source
		i = qwchlink[k];
		j = qwchnode[k];

		//Developer's Note:  Since dqchinvol[][][0] is the same as qwchvol[][],
		//                   the qwchvol array could be eliminated.  For now both
		//                   arrays are retained because they provide a separate
		//                   check on point source flows.
		//
		//increment cumulative node flow volume
		dqchinvol[i][j][0] = dqchinvol[i][j][0]
			+ qwchinterp[k] * dt[idt];

		//Compute sum of external flow volumes (m3)
		qwchvol[i][j] = qwchvol[i][j] + qwchinterp[k] * dt[idt];

	}	//end loop over number of external flow sources

	//Cumulative mass balance for floodplain water transfers...
	//
	//Note:  These loop only cover floodplain flows (k=9) and
	//       occur here rather than in the main loop above in
	//       order to reduce the total number operations needed
	//       to perform mass balance calculations...
	//
	//if bi-directional floodplain transfer is simulated
	if(fldopt > 0)
	{
		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over number of nodes for current link
			for(j=1; j<=nnodes[i]; j++)
			{
				//Determine row/column location for current link/node
				row  = ichnrow[i][j];		//Row number of link i, node j
				col  = ichncol[i][j];		//Col number of link i, node j

				//increment cumulative node inflow volume (m3)
				dqchinvol[i][j][9] = dqchinvol[i][j][9]
					+ dqchin[i][j][9] * dt[idt];

				//increment cumulative cell outflow volume (m3)
				dqovoutvol[row][col][9] = dqovoutvol[row][col][9]
					+ dqovout[row][col][9] * dt[idt];

				//increment cumulative node outflow volume (m3)
				dqchoutvol[i][j][9] = dqchoutvol[i][j][9]
					+ dqchout[i][j][9] * dt[idt];

				//increment cumulative overland cell inflow volume (m3)
				dqovinvol[row][col][9] = dqovinvol[row][col][9]
					+ dqovin[row][col][9] * dt[idt];

			}	//end loop over nodes

		}	//end loop over links
	}
	else	//else fldopt = 0
	{
		//Water is only transferred from the overland plane to the
		//channel network (regardless of water surface elevations)
		//
		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over number of nodes for current link
			for(j=1; j<=nnodes[i]; j++)
			{
				//Determine row/column location for current link/node
				row  = ichnrow[i][j];		//Row number of link i, node j
				col  = ichncol[i][j];		//Col number of link i, node j

				//increment cumulative node inflow volume (m3)
				dqchinvol[i][j][9] = dqchinvol[i][j][9]
					+ dqchin[i][j][9] * dt[idt];

				//increment cumulative cell outflow volume (m3)
				dqovoutvol[row][col][9] = dqovoutvol[row][col][9]
					+ dqovout[row][col][9] * dt[idt];

			}	//end loop over nodes

		}	//end loop over links

	}	//end if fldopt > 0

	//Store flows at watershed (overland) outlets...
	//
	//Loop over number of outlets
	for(k=1; k<=noutlets; k++)
	{
		//if the outlet cell is a channel cell (imask > 1)
		if(imask[iout[k]][jout[k]] > 1)
		{
			//set link and node references for the outlet cell
			i = link[iout[k]][jout[k]];
			j = node[iout[k]][jout[k]];

			//Search for peak flow at this outlet...
			//
			//if qoutch[i] is greater than qpeakch[k]
			if(qoutch[i] > qpeakch[k])
			{
				//Save peak flow at outlet
				qpeakch[k] = qoutch[i];

				//Save time of peak flow;
				tpeakch[k] = (float)(simtime);

			}	//end if qoutch[i] > qpeakch[k]

		}	//end if imask[][] > 1

	}	//end loop over outlets

//End of Function: Return to WaterBalance
}
