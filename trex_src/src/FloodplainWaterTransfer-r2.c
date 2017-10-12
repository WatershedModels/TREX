/*----------------------------------------------------------------------
C-  Function:	FloodplainWaterTransfer.c
C-
C-	Purpose/	Computes water flow between overland and channel portion
C-	Methods:	of floodplain cells (cells with a channel, imask > 1).
C-				Water can move from the overland plane to the channel or
C-				from the channel to the overland plane acording to the
C-              channel bank height and the difference in water surface
C-              elevation.
C-
C-
C-	Inputs:		hch[][], bwidth[][], hbank[][], sideslope[][],
C-              deadstoragedepth[][], chanlength[][], dt[], hov[][],
C-              ichnrow[][], ichncol[][]  (Globals)
C-
C-	Outputs:	dqchin[][][9], dqchout[][][9]  (Globals)
C-              dqovin[][][9], dqovout[][][9]  (Globals)
C-
C-	Controls:   fldopt
C-
C-	Calls:		none
C-
C-	Called by:	WaterTransport
C-
C-	Created:	Mark Velleux
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
C-	Date:		03-MAY-2005
C-
C-	Revised:	Mark Velleux
C-				Masa Takamatsu
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		29-Jul-2010
C-
C-	Revisions:	Cumulative mass balance calculations moved to
C-              ChannelWaterdepth for time step option (dtopt)
C-              implementation.
C-
C-  Revised:
C-
C-	Revisions:
C-
C-	Date:
C-
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void FloodplainWaterTransfer()
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
		hnode,		//effective channel water depth (m)
		ach,		//cross-section area (at effective depth) of water in channel (m2)
		achsurf,	//surface area of channel (m2)
		tloss,		//node transmission loss volume (m3)
		qnode,		//node gross outflow volume (m3)
		vch,		//effective volume of water in channel (m3) (at depth hnode)
		vchbank,	//volume of water channel can hold at top of bank (m3)
		vchavail,	//available (unfilled) volume of channel before it is filled to top of bank (m3)
		twch;		//top width of channel at top of bank (m)

  	float
		hcell,		//effective overland cell water depth (m)
		scell,		//cell storage depth (m)
		icell,		//cell infiltration depth (m)
		qcell,		//cell gross outflow depth (m)
		vcell;		//effective volume of water in overland portion of cell (m3)

	float
		vtransfer;	//volume of water transferred between overland plane/channel (m3)

	float
		wseov,		//water surface elevation (wse) of the overland plane (m)
		wsech;		//water surface elevation (wse) of the channel (m)

	//Note:  fldopt is a switch to control the direction of floodplain
	//       water transfers.  When fldopt = 0, water transfer occurs
	//       from overland to channel only (one direction only).  When
	//       fldopt > 0, water transfer can be in either direction.
	//
	//if bi-directional floodplain transfer is simulated
	if(fldopt > 0)
	{
		//Water is transferred in either direction between the overland
		//plane and channel network depending on water surface elevation
		//
		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over number of nodes for current link
			for(j=1; j<=nnodes[i]; j++)
			{
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

				//Determine row/column location for current link/node
				row  = ichnrow[i][j];		//Row number of link i, node j
				col  = ichncol[i][j];		//Col number of link i, node j

				//Assign overland characteristics (row/cell of present link/node)
				scell = storagedepth[row][col];		//overland cell storage depth (m)

				//if infiltration is simulated (infopt > 0)
				if(infopt > 0)
				{
					//Cell infiltration depth (not available for transfer) (m)
					icell = infiltrationrate[row][col] * dt[idt];
				}
				else	//else infiltration is not simulated (infopt = 0)
				{
					//Set cell infiltration depth to zero
					icell = 0.0;

				}	//end if infopt > 0

				//Compute cell gross outflow depth...
				//
				//Note:  The cell gross outflow depth is computed from the
				//       sum of outflows in the N, E, S, and W directions
				//       divided by the cell surface area and multiplied
				//       by the time step.
				//
				//Compute cell outflow depth (not available for transfer) (m)
				qcell = ((dqovout[row][col][1] + dqovout[row][col][3]
					+ dqovout[row][col][5] + dqovout[row][col][7])
					/ (w * w - achsurf)) * dt[idt];

				//Compute effective overland depth for floodplain transfer (m)
				hcell = hov[row][col] - icell - qcell;

				//Error Trap: check for negative effective depth (round-off error)
				if(hcell < 0.0)
				{
					//set the effective floodplain water depth to zero
					hcell = 0.0;

				}	//end if hcell < 0.0

				//Compute effective volume of water in channel...
				//
				//Note:  The effective channel water volume is the
				//       starting volume minus the node gross outflow
				//       volume (including transmission loss):
				//
				//       vch = (b+zy)y * length - tloss - qnode
				//
				//if channel transmission loss is simulated (ctlopt > 0)
				if(ctlopt > 0)
				{
					//Node transmission loss volume (m3)
					tloss = translossrate[i][j] * bwch * lch * dt[idt];
				}
				else	//else ctlopt = 0
				{
					//set the transmission loss volume to zero
					tloss = 0.0;

				}	//end if ctlopt > 0

				//Node gross outflow volume (m3)
				qnode = (dqchout[i][j][1] + dqchout[i][j][2] + dqchout[i][j][3]
					+ dqchout[i][j][4] + dqchout[i][j][5] + dqchout[i][j][6]
					+ dqchout[i][j][7] + dqchout[i][j][8]) * dt[idt];

				//If the channel water depth is greater than the bank height
				if(hch[i][j] >= hbch)
				{
					//Compute effective channel water volume (above bank) (m3)
					vch = vchbank + ((hch[i][j] - hbch) * twch * lch)
						- tloss - qnode;
				}
				else	//channel water depth is >= bank height
				{
					//Compute effective channel water volume (below bank) (m3)
					vch = ((bwch + sslope * hch[i][j]) * hch[i][j] * lch)
						- tloss - qnode;

				}	//end if channel water depth is greater than the bank height

				//Error Trap: check for negative effective volume (round-off error)
				if(vch < 0.0)
				{
					//set the effective channel water depth to zero
					vch = 0.0;

				}	//end if vch < 0.0

				//Compute effective channel cross-section area (m2)
				ach = vch / lch;

				//Compute effective channel (node) depth...
				//
				//if sslope > 0 (the channel is trapezoidal or triangular)
				if(sslope > 0)
				{
					//If the effective water volume is greater than the volume at bank height
					if(vch > vchbank)
					{
						//Compute effective channel depth (m)
						hnode = hbch + (vch - vchbank) / (lch * twch);
					}
					else	//effective water volume is <= the volume at bank height
					{
						//Compute effective channel depth (positive root from quadratic eqn)
						hnode = (float)((-bwch + sqrt(pow(bwch,2.0)
							- 4.0*sslope*(-ach)))/(2.0*sslope));

					}	//end if effective water volume > volume at bank height
				}
				else	//sslope = 0 (channel is rectangular)
				{
					//Compute new channel depth (from area and width)
					hnode = ach / bwch;

				}	//end if sslope > 0

				//Compute available (unfilled) channel volume (from
				//water surface to top of bank)...
				//
				//If the effective channel water depth is less than the bank height
				if (hnode < hbch)
				{
					//Compute available (unfilled) volume in channel (m3)
					vchavail = vchbank - vch;
				}
				else	//else hnode >= hbch
				{
					//set vchavail to zero
					vchavail = 0.0;

				}	//end if hnode < hbch

				//Compute water transfer between channel and overland plane...
				//
				//Compute water surface elevations (wse)...
				//
				//Overland wse (m) (this is just for convenience...)
				wseov = hcell;

				//Channel wse (m)
				wsech = hnode - hbch;

				//Transfer water between overland plane and channel.  Three cases
				//exist depending on water surface elevation (wse) and whether the
				//overland water depth is sufficient for flow:
				//
				//   1. overland wse > channel wse
				//      a. channel water depth < bank height
				//      b. channel water depth >= bank height 
				//   2. channel wse > overland wse (hch always > hbank)
				//   3. overland wse = channel wse (no water transfer)
				//
				//
				//Note: For water to move from the overland portion of
				//      a cell to the channel, the available water depth
				//      must be greater than the storage depth of the cell.
				//      The available water depth is the starting depth
				//      (hov) minus the depth lost to infiltration and
				//      outflows to other cells in the overland plane.
				//
				//Case 1: if the overland water depth is sufficient for
				//        flow and the overland wse exceeds the channel
				//        wse
				//
				//if the overland water depth > storage depth AND the
				//  overland wse > the channel wse
				if((hcell > scell) && (wseov > wsech))
				{
					//Transfer water from overland plane to channel...
					//
					//Case 1a: if the channel water depth is less than the bank height
					if(hnode < hbch)
					{
						//Available volume of water in overland portion of cell (m3)
						vcell = (hcell - scell) * (w * w - achsurf);

						//if cell volume <= available channel volume
						if(vcell <= vchavail)
						{
							//Case 1a: Condition 1
							//
							//Volume of water transferred = vcell...
							//
							//Compute cumulative volume of water transferred
							vtransfer = vcell;
						}
						//incoming volume exceeds available volume (vcell > vchavail)
						else
						{
							//Case 1a: Condition 2
							//
							//First part of transfer...
							//
							//Remove vchavail from the overland
							//plane and add it to the channel to
							//fill the remaining volume of the
							//channel to top of bank.
							//
							//vtransfer1 = vchavail
							//
							//Second part of transfer...
							//
							//Level the overland and channel water
							//surface by removing water from the
							//overland plane so wsech = wsechov.
							//
							//vtransfer2 = (vcell-vchavail)
							//  * (1 - achsurf/(w*w))
							//
							//Putting both parts together below...
							//
							//Compute cumulative volume of water transferred (m3)
							vtransfer = (float)(vchavail + (vcell - vchavail)
								* (1.0 - achsurf / (w * w)));

						}	//end if vcell <= vchavail
					}
					//Case 1b: channel water depth >= bank height (wseov > wsech)
					else
					{
						//Compute volume of water in overland portion of cell
						//transported to channel.  The overland storage depth
						//(scell) must be subtracted from the overland depth
						//to obtain the proper volume...
						//
						//volume of water transported from overland part of cell (m3)
						vtransfer = (float)(((hcell - scell) - (hnode - hbch))
							* (w * w - achsurf) * (1.0 - (w * w - achsurf) / (w * w)));
						
					}	//end if hchnew < hbch

					//water transferred from overland plane to channel...
					//
					//store channel gross flows (m3/s)
					dqchin[i][j][9] = vtransfer / dt[idt];
					dqchout[i][j][9] = 0.0;

					//increment channel net flow (m3/s)
					dqch[i][j] = dqch[i][j] + vtransfer / dt[idt];

					//store overland plane gross flows (m3/s)
					dqovin[row][col][9] = 0.0;
					dqovout[row][col][9] = vtransfer / dt[idt];

					//decrement overland net flow (m3/s)
					dqov[row][col] = dqov[row][col] - vtransfer / dt[idt];
				}
				//Case 2: if the channel wse greater than the overland wse
				else if(wsech > wseov)
				{
					//Transfer water from channel to overland plane...
					//
					//Compute volume of water in channel transported to
					//the overland portion of cell (m3)
					vtransfer = (float)(((hnode - hbch) - hcell)
						* achsurf * (1.0 - achsurf / (w * w)));

					//water transferred from channel to overland plane...
					//
					//store channel gross flows (m3/s)
					dqchin[i][j][9] = 0.0;
					dqchout[i][j][9] = vtransfer / dt[idt];

					//decrement channel net flow (m3/s)
					dqch[i][j] = dqch[i][j] - vtransfer / dt[idt];

					//store overland plane gross flows (m3/s)
					dqovin[row][col][9] = vtransfer / dt[idt];
					dqovout[row][col][9] = 0.0;

					//increment overland net flow (m3/s)
					dqov[row][col] = dqov[row][col] + vtransfer / dt[idt];
				}
				//Case 3: 0verland wse = channel wse OR hcell <= scell
				//        (no water for transfer) (transfer = 0.0)
				else
				{
					//Note:  This is not needed but is here to make
					//       the code explicitly consider all cases
					//       even if no volume is transferred...
					//
					//Volume transferred (m3) is zero...
					vtransfer = 0.0;

					//no water is transferred...
					//
					//store channel flows (m3/s)
					dqchin[i][j][9] = 0.0;
					dqchout[i][j][9] = 0.0;

					//store overland plane flows (m3/s)
					dqovin[row][col][9] = 0.0;
					dqovout[row][col][9] = 0.0;

					//Note: For completeness, we could also show that zero
					//      is added to the cumulative node and cell flow
					//      volumes.  However, for simplicity we skip this
					//      step when vtransfer is exactly zero.
					//
					//      Similarly, we skip incrementing the net flows
					//      since the volume tranferred is zero...

				}	//end if overland > channel wse

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
				//Assign channel characteristics (present link/node)
				lch = chanlength[i][j];				//channel length (m) (includes sinuosity)
				twch = twidth[i][j];				//channel top width at top of bank

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

				//Determine row/column location for current link/node
				row  = ichnrow[i][j];		//Row number of link i, node j
				col  = ichncol[i][j];		//Col number of link i, node j

				//Assign overland characteristics (row/cell of present link/node)
				scell = storagedepth[row][col];		//overland cell storage depth (m)

				//if infiltration is simulated (infopt > 0)
				if(infopt > 0)
				{
					//Cell infiltration depth (not available for transfer) (m)
					icell = infiltrationrate[row][col] * dt[idt];
				}
				else	//else infiltration is not simulated (infopt = 0)
				{
					//Set cell infiltration depth to zero
					icell = 0.0;

				}	//end if infopt > 0

				//Compute cell gross outflow depth...
				//
				//Note:  The cell gross outflow depth is computed from the
				//       sum of outflows in the N, E, S, and W directions
				//       divided by the cell surface area and multiplied
				//       by the time step.
				//
				//Compute cell outflow depth (not available for transfer) (m)
				qcell = ((dqovout[row][col][1] + dqovout[row][col][3]
					+ dqovout[row][col][5] + dqovout[row][col][7])
					/ (w * w - achsurf)) * dt[idt];

				//Compute effective overland depth for floodplain transfer (m)
				hcell = hov[row][col] - icell - qcell;

				//Error Trap: check for negative effective depth (round-off error)
				if(hcell < 0.0)
				{
					//set the effective floodplain water depth to zero
					hcell = 0.0;

				}	//end if hcell < 0.0

				//Compute water transfer between channel and overland plane...
				//
				//Note: For water to move from the overland portion of
				//      a cell to the channel, the available water depth
				//      must be greater than the storage depth of the cell.
				//      The available water depth is the starting depth
				//      (hov) minus the depth lost to infiltration and
				//      outflows to other cells in the overland plane.
				//
				//if the overland water depth > storage depth
				if(hcell > scell)
				{
					//Available volume of water in overland portion of cell (m3)
					vcell = (hcell - scell) * (w * w - achsurf);

					//Compute cumulative volume of water transferred
					vtransfer = vcell;
					//water transferred from overland plane to channel...
					//
					//store channel gross flows (m3/s)
					dqchin[i][j][9] = vtransfer / dt[idt];
					dqchout[i][j][9] = 0.0;

					//increment channel net flow (m3/s)
					dqch[i][j] = dqch[i][j] + vtransfer / dt[idt];

					//store overland plane gross flows (m3/s)
					dqovin[row][col][9] = 0.0;
					dqovout[row][col][9] = vtransfer / dt[idt];

					//decrement overland net flow (m3/s)
					dqov[row][col] = dqov[row][col] - vtransfer / dt[idt];
				}
				else	//else hcell <= scell
				{
					//Note:  This is not needed but is here to make
					//       the code explicitly consider all cases
					//       even if no volume is transferred...
					//
					//Volume transferred (m3) is zero...
					vtransfer = 0.0;

					//no water is transferred...
					//
					//store channel flows (m3/s)
					dqchin[i][j][9] = 0.0;
					dqchout[i][j][9] = 0.0;

					//store overland plane flows (m3/s)
					dqovin[row][col][9] = 0.0;
					dqovout[row][col][9] = 0.0;

					//Note: For completeness, we could also show that zero
					//      is added to the cumulative node and cell flow
					//      volumes.  However, for simplicity we skip this
					//      step when vtransfer is exactly zero.
					//
					//      Similarly, we skip incrementing the net flows
					//      since the volume tranferred is zero...

				}	//end if hcell > scell

			}	//end loop over nodes

		}	//end loop over links

	}	//end if fldopt > 0

//End of Function: Return to WaterTransport
}
