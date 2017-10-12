/*----------------------------------------------------------------------
C-  Function:	OverlandWaterDepth.c
C-
C-	Purpose/	Updates overland water depths (x,y) in each grid cell,
C-	Methods:	checks for negative depth value (exit program option);
C-
C-
C-	Inputs:		netrainrate[][], infiltrationrate[][], netswerate[][],
C-              swemeltrate[][], dqov[][], hov[][], sweov[][]
C-              (values at time t)
C-
C-	Outputs:	hovnew[][] (values at time t+dt)
C-              vovintermediate[][]
C-              sweovnew[][]
C-
C-	Controls:   ksim
C-
C-	Calls:		SimulationError
C-
C-	Called by:	WaterBalance
C-
C-	Created:	Original Coding in Fortran - CASC2D User's Manual
C-				Julien, P.Y. and Saghafian, B. 1991
C-				Civil Engineering, Colorado State University
C-				Fort Collins, CO 80523
C-
C-				Code later converted from Fortran to C
C-				Last Update by Rosalia Rojas-Sanchez, CSU
C-
C-	Date:		19-JUN-2003
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux (CSU)
C-
C-	Date:		02-SEPT-2003
C-
C-	Revisions:	Document variables and main computations, restructure
C-              to separate transport components from water balance.
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		21-DEC-2006
C-
C-  Revisions:	Added option for snow hydrology and computation
C-              of the snowpack depth (as SWE).
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
C-              OverlandWaterDepth to implement time step option
C-              (dtopt).
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

void OverlandWaterDepth( )
{
	//local variable declarations/definitions
	int
		chanlink,	//channel link number
		channode;	//channel node number

	float
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf,	//surface area of channel portion of cell (m)
		aovsurf;	//surface area of overland portion of cell (m)

	float
		derivative;	//sum of rates -  rainfall, snowmelt, infiltration, flows (m/s)

	//Compute Surface Water Balance...
	//
	//The depth of water in the overland plane is the sum of water
	//inputs from net rainfall (gross rainfall minus interception),
	//snowmelt, external water sources, internal water transfers
	//and (flooding/return flow from channels) minus infiltration.
	//Because flooding/return flow from channels also depends on
	//water depth in the channel, floodplain interactions are
	//computed in ChannelDepth (and hov updated).
	//
	//Developer's Note: evapotranspiration would be subtracted from this
	//the water balance as well.  However, this feature not included in
	//present version...
	//
	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Check if the cell is a channel cell.  We don't
				//really need to check if chnopt > 0 because the
				//imask should be 1 but adding a chnopt check
				//makes the code a bit more "error proof"...
				//
				//if the cell is a channel cell and channels are simulated
				if((imask[i][j] > 1) && (chnopt > 0))
				{
					//Compute surface areas of overland and channel
					//potions of the cell...
					//
					//Get channel link and node at this row and column
					chanlink = link[i][j];
					channode = node[i][j];

					//jfe:  Note: these do not need to be computed locally because the
					//jfe:        values are constant and depend only on input.  The
					//jfe:        surface area can be computed at the time the channel
					//jfe:        property file is read and make achsurf a global...

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)
					twch = twidth[chanlink][channode];			//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;

					//Surface area of overland portion of cell (m2)
					aovsurf = w * w - achsurf;
				}
				else	//the cell is an overland cell
				{
					//Overland area of cell = entire area of cell
					aovsurf = w * w;

				}	//end if imask[][] > 1

				//Build derivative term by term...
				//
				//  derivative = netrainrate[i][j]
				//             - infiltrationrate[i][j]
				//             + snowmeltrate[i][j]
				//             + (dqov[i][j] / aovsurf)
				//
				//Note: the derivative has units of m/s and
				//      dqov is divided by the overland area
				//      of the cell to obtain a velocity.
				//
				//Start with net rain (apportioned for overland surface area)
				derivative = netrainrate[i][j];

				//Subtract infiltration
				if(infopt > 0) derivative = derivative - infiltrationrate[i][j];

				//Add snowmelt (as swe)
				if(meltopt > 0) derivative = derivative + swemeltrate[i][j];

				//Add net overland flow rate over overland area of cell
				derivative = derivative + dqov[i][j] / aovsurf;

				//update overland flow depth to new depth
				//new depth = current depth + derivative * time step
				hovnew[i][j] = hov[i][j] + derivative * dt[idt];

				//Flow depths must always be >= to zero. When hov < 0,
				//this simulation aborts.  Warn user and write error
				//message...
				//
				//if the overland water depth in this cell < 0
				if(hovnew[i][j] < 0.0)
				{
					//Check for round-off error...
					//
					//if the magnitude of the depth < the error tolerance
					if((float)fabs(hovnew[i][j]) < TOLERANCE)
					{
						//The error is round-off...
						//
						//Reset the new overland depth to exactly zero
						hovnew[i][j] = 0.0;
					}
					else	//the error is numerical instability
					{
						//simulation error number;	simerrorno = 1;
						//
						//Report error type (negative water depth in overland cell) and location
						SimulationError(1, i, j, 0);

						exit(EXIT_FAILURE);	//abort

					}	//end check for round-off error

				}	//end if negative depth

				//if the new depth is less than the TOLERANCE
				if(hovnew[i][j] < TOLERANCE)
				{
					//reset the depth to zero
					hovnew[i][j] = 0.0;

				}	//end if hovnew[i][j] < TOLERANCE

				//if snow hydrology (snowfall or snowmelt) is simulated
				if(snowopt > 0 || meltopt > 0)
				{
					//Build snowpack swe derivative term by term...
					//
					//  derivative = netswerate[i][j]
					//             - swemeltrate[i][j]
					//
					//initialize derivative
					derivative = 0.0;

					//Compute snowpack swe derivative (m/s)
					//
					//if snowfall is simulated
					if(snowopt > 0) derivative = derivative + netswerate[i][j];
					
					//if snowmelt is simulated
					if(meltopt > 0) derivative = derivative - swemeltrate[i][j];

					//update overland snowpack depth to new snowpack depth
					//new depth = current depth + derivative * time step
					sweovnew[i][j] = sweov[i][j] + derivative * dt[idt];

					//Snow depths must always be >= to zero. When sweov < 0,
					//this simulation aborts.  Warn user and write error
					//message...
					//
					//if the overland snow pack depth in this cell < 0
					if(sweovnew[i][j] < 0.0)
					{
						//Check for round-off error...
						//
						//if the magnitude of the depth < the error tolerance
						if((float)fabs(sweovnew[i][j]) < TOLERANCE)
						{
							//The error is round-off...
							//
							//Reset the new overland snowpack depth to exactly zero
							sweovnew[i][j] = 0.0;
						}
						else	//the error is numerical instability
						{
							//simulation error number;	simerrorno = 2;
							//
							//Report error type (negative swe depth in overland cell) and location
							SimulationError(2, i, j, 0);

							exit(EXIT_FAILURE);	//abort

						}	//end check for round-off error

					}	//end if negative depth

					//if the new swe depth is less than the TOLERANCE
					if(sweovnew[i][j] < TOLERANCE)
					{
						//reset the depth to zero
						sweovnew[i][j] = 0.0;

					}	//end if sweovnew[i][j] < TOLERANCE

				}	//endif snowopt > 0 or meltopt > 0

				/****************************************************/
				/*     Update Cumulative Mass Balance Variables     */
				/****************************************************/

				//Gross Rainfall...
				//
				//Compute the cumulative depth of rainfall for each cell (m)
				grossraindepth[i][j] = grossraindepth[i][j]
					+ grossrainrate[i][j] * dt[idt];

				//Compute the cumulative gross rainfall volume entering a cell (m3)
				grossrainvol[i][j] = grossrainvol[i][j]
					+ grossrainrate[i][j] * w * w * dt[idt];

				//Minimum and Maximun Rainfall Intensity...
				//
				//Note: If snowopt = 1 and the air temperature is below freezing,
				//      then the gross rainfall rate is zero...
				//
				//Save the minimum rainfall intensity value (m/s)
				minrainintensity = Min(minrainintensity,grossrainrate[i][j]);

				//Save the maximum rainfall intensity value (m/s)
				maxrainintensity = Max(maxrainintensity,grossrainrate[i][j]);

				//Gross Snowfall...
				//
				//if snow snowfall is simulated
				if(snowopt > 0)
				{
					//Compute the cumulative depth of snowfall (swe) for each cell (m)
					grossswedepth[i][j] = grossswedepth[i][j]
						+ grossswerate[i][j] * dt[idt];

					//Compute the cumulative gross snowfall (swe) volume entering a cell (m3)
					grossswevol[i][j] = grossswevol[i][j]
						+ grossswerate[i][j] * w * w * dt[idt];

					//Save the minimum snowfall (swe) intensity value (m/s)
					minsweintensity = Min(minsweintensity,grossswerate[i][j]);

					//Save the maximum snowfall (swe) intensity value (m/s)
					maxsweintensity = Max(maxsweintensity,grossswerate[i][j]);

				}	//endif snowopt > 0

				//Interception...
				//
				//if remaining interception depth > 0, some rain will be intercepted
				if(interceptiondepth[i][j] > 0.0)
				{
					//if the depth of gross rainfall exceeds the interception depth
					if(grossrainrate[i][j] * dt[idt] >= interceptiondepth[i][j])
					{
						//Add volume to cumulative interception volume (m3)
						interceptionvol[i][j] = interceptionvol[i][j]
							+ interceptiondepth[i][j] * aovsurf;

						//set the remaining (unsatisfied) interception depth to zero
						interceptiondepth[i][j] = 0.0;
					}
					else	//interception depth exceeds gross rainfall (depth)
					{
						//decrement the remaining (unsatisfied) interception depth
						interceptiondepth[i][j] = interceptiondepth[i][j]
							- grossrainrate[i][j] * dt[idt];

						//Add volume to cumulative interception volume in a cell (m3)
						interceptionvol[i][j] = interceptionvol[i][j]
							+ grossrainrate[i][j] * dt[idt] * aovsurf;

					}	//end if gross rain > interception

				}	//end if remaining interception depth > 0

				//Note:  The cumulative interception and net rain mass
				//       balances assume that interception occurs only
				//       over the overland part of a cell and excludes
				//       the surface water part of channel cells.  The
				//       net rain volume is updated in ChannelWaterDepth
				//       to account for direct precipitation to channels.
				//
				//Compute the cumulative net rainfall volume entering the overland part of a cell (m3)
				netrainvol[i][j] = netrainvol[i][j]
					+ netrainrate[i][j] * aovsurf * dt[idt];

				//if snowfall is simulated as falling snow
				if(snowopt > 0 && snowopt <= 3)
				{
					//if remaining interception depth > 0, some snow will be intercepted
					if(interceptiondepth[i][j] > 0.0)
					{
						//if the depth of gross snowfall (as swe) exceeds the interception depth
						if(grossswerate[i][j] * dt[idt] >= interceptiondepth[i][j])
						{
							//Add volume to cumulative interception volume (m3)
							interceptionvol[i][j] = interceptionvol[i][j]
								+ interceptiondepth[i][j] * aovsurf;

							//set the remaining (unsatisfied) interception depth to zero
							interceptiondepth[i][j] = 0.0;
						}
						else	//interception depth exceeds gross rainfall (depth)
						{
							//decrement the remaining (unsatisfied) interception depth
							interceptiondepth[i][j] = interceptiondepth[i][j]
								- grossswerate[i][j] * dt[idt];

							//Add volume to cumulative interception volume in a cell (m3)
							interceptionvol[i][j] = interceptionvol[i][j]
								+ grossswerate[i][j] * dt[idt] * aovsurf;

						}	//end if gross snow > interception

					}	//end if remaining interception depth > 0

					//Compute the cumulative net snowfall volume entering the overland part of a cell (m3)
					netswevol[i][j] = netswevol[i][j]
						+ netswerate[i][j] * aovsurf * dt[idt];

				}	//endif snowopt > 1 && snowopt <= 3

				//Snowmelt (Snow Pack Water Loss)...
				//
				//if snow snowmelt is simulated
				if(meltopt > 0)
				{
					//Compute the cumulative depth of snowmelt (as swe) (m)
					swemeltdepth[i][j] = swemeltdepth[i][j]
						+ swemeltrate[i][j] * dt[idt];

					//Compute cumulative snowmelt volume (as swe) for this cell (m3)
					swemeltvol[i][j] = swemeltvol[i][j]
						+ swemeltrate[i][j] * dt[idt] * aovsurf;

					//Compute minimum snowmelt depth (as swe) (cumulative) for cell (m)
					minswemeltdepth = Min(swemeltdepth[i][j],minswemeltdepth);

					//Compute maximum depth of snowmelt (as swe) (cumulative) for cell (m)
					maxswemeltdepth = Max(swemeltdepth[i][j],maxswemeltdepth);

				}	//endif meltopt > 0

				//Infiltration...
				//
				//Compute the cumulative depth of infitration
				infiltrationdepth[i][j] = infiltrationdepth[i][j]
					+ infiltrationrate[i][j] * dt[idt];
				
				//Compute minimum infiltration depth (cumulative) for cell
				mininfiltdepth = Min(infiltrationdepth[i][j],mininfiltdepth);

				//Compute maximum infiltration depth (cumulative) for cell
				maxinfiltdepth = Max(infiltrationdepth[i][j],maxinfiltdepth);

				//Compute cumulative infiltration volume for this cell (m3)
				infiltrationvol[i][j] = infiltrationvol[i][j]
					+ infiltrationrate[i][j] * dt[idt] * aovsurf;

				//Overland Flow (excluding point sources and floodplain transfers)...
				//
				//Note:  Cumulative mass balance calculations for floodplain
				//       water transfers occur in ChannelWaterDepth...
				//
				//Loop over directions 1, 3, 5, and 7
				for(k=1; k<=7; k=k+2)
				{
					//increment cumulative cell outflow volume (m3)
					dqovoutvol[i][j][k] = dqovoutvol[i][j][k]
						+ dqovout[i][j][k] * dt[idt];

					//increment cumulative cell inflow volume (m3)
					dqovinvol[i][j][k] = dqovinvol[i][j][k]
						+ dqovinvol[i][j][k] * dt[idt];

				}	//end loop over directions 1, 3, 5, and 7

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//Cumulative mass balance for overland flow point sources...
	//
	//Assign external overland flow point sources
	//
	//Loop over number of external flow sources
	for(k=1; k<=nqwov; k++)
	{
		//set row and column references for the flow point source
		i = qwovrow[k];
		j = qwovcol[k];

		//Increment cumulative cell flow volume
		dqovinvol[i][j][0] = dqovinvol[i][j][0]
			+ qwovinterp[k] * dt[idt];

		//Compute cumulative sum of external flow volumes (m3)
		qwovvol[i][j] = qwovvol[i][j] + qwovinterp[k] * dt[idt];

	}	//end loop over number of external flow sources

	//Comulative mass balance for overland flow leaving outlets...
	//
	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//increment cumulative cell outflow volume
		dqovoutvol[iout[i]][jout[i]][10] = dqovoutvol[iout[i]][jout[i]][10]
			+ qoutov[i] * dt[idt];

		//increment cumulative cell inflow volume
		dqovinvol[iout[i]][jout[i]][10] = dqovinvol[iout[i]][jout[i]][10]
			+ qinov[i] * dt[idt];

		//Compute cumulative flow volume exiting domain at this outlet (m3)
		qoutovvol[i] = qoutovvol[i] + qoutov[i] * dt[idt];

	}	//end loop over outlets

//End of function: Return to WaterBalance
}
