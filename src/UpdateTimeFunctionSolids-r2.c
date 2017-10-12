/*----------------------------------------------------------------------
C-  Function:	UpdateTimeFunctionSolids.c
C-
C-	Purpose/	UpdateTimeFunctionSolids is called to interpolate values
C-  Methods:    of time-dependent functions for a given dt based on input
C-              values at specific times.  Forcing functions for overland
C-              and channel solids loads are updated as needed for the
C-              next time step (t + dt).  Boundary conditions for solids
C-              are also updated.  Linear intepolation between input
C-              values is performed.  The convention used is:
C-
C-                         f(t)_i+1 - f(t)_i
C-                 slope = -----------------
C-                            t_i+1 - t_i
C-
C-                 intercept = f(t)_i+1
C-
C-                 next updated time = t_i+1
C-
C-              where:  i  = lower (earlier) break position in time
C-                           series
C-
C-                     i+1 = upper (later) break position in time
C-                           series
C-
C-
C-	Inputs:		swpov[][][], swdov[][][], swch[][][], sbc[][][],
C-              (at some time t in time series)
C-
C-	Outputs:	swpovinterp[][][], swdovinterp[][][], swchinterp[][][],
C-              sbcinterp[][][]
C-              (at current simulation time)
C-
C-	Controls:	chnopt, nsolids, nswpov, nswpovpairs, nswdov, nswdovpairs,
C-              nswch, nswchpairs, noutlets, dbcopt, nsbcpairs
C-
C-	Calls:		None
C-
C-	Called by:	UpdateTimeFunction
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
C-	Date:		20-OCT-2003
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       29-APR-2008
C-
C-  Revisions:  Added overland distributed loads
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

void UpdateTimeFunctionSolids()
{
	//local variable declarations/definitions
	int 
		ip,			//pointer to current position in time series
		ncycle;		//number of completed cycles for a time series

	double
		endtime,	//end time of a time function (last time break in series)
		mtime;		//modulo end time

	float
		nt,			//next (upper) interval time for a time function
		pt;			//prior (lower) interval time for a time function

	//jfe: may need option here based on constant interp or linear interplation.
	//option may be done later to implement constant interpolation for forcing
	//functions and flow BCs;
	//for now - all functions updated using linear interpolation.

	//Update overland load functions...
	//
	//loop over number of solids
	for(i=1; i<=nsolids; i++)
	{
		//If it is time to update any overland point source solids load time function (simtime >= ntswpov[])
		if(simtime >= ntswpov[i])
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the sediment loads are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Loop over number of overland loads for current solids type
			for(j=1; j<=nswpov[i]; j++)
			{
				//if it is time to update values for this sediment load...
				//
				//nswpovt = next time value in overland solids load time series
				//pswpovt = prior overland solids load time value in time series
				//
				if(simtime >= nswpovt[i][j] || simtime < pswpovt[i][j])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = swpovtime[i][j][nswpovpairs[i][j]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
					
					//Pointer to the last "current" time interval index for this 
					//sediment load.
					ip = swpovpoint[i][j];
					
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= swpovtime[i][j][ip+1])
					{
						ip = ip + 1;

					}	//end while
						
					while((mtime < swpovtime[i][j][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bswpov[i][j] = swpov[i][j][ip+1];

					//Compute slope (m) for new interval
					mswpov[i][j] = (swpov[i][j][ip] - swpov[i][j][ip+1])
						/ (swpovtime[i][j][ip] - swpovtime[i][j][ip+1]);
					
					//Number of times cycled through the sediment load time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);

					//Next time interval upper bounds to recalculate this sediment load.
					nswpovt[i][j] = (float)(ncycle * endtime) + swpovtime[i][j][ip+1];

					//Next time interval lower bounds to recalculate this sediment load.
					pswpovt[i][j] = (float)(ncycle * endtime) + swpovtime[i][j][ip];

					//Save the pointer to the current time interval for this sediment load.
					swpovpoint[i][j] = ip;

				}	//	End if simtime >= nswpovt or simtime < pswpovt

				// Get the necessary time window bounds to update any sediment load.
				if (nswpovt[i][j] < nt) nt = nswpovt[i][j];
				if (pswpovt[i][j] > pt) pt = pswpovt[i][j];
					
			}	//end loop over number overland loads for current solid

			//Store the lower time window to update overland solids load functions (for function with lowest time break)
			ntswpov[i] = nt;

		}	//end if simtime >= ntswpov[i]

 		//Loop over number of overland load functions to update interpolated value
 		//each time step using current slope and intercept
 		for(j=1; j<=nswpov[i]; j++)
 		{
 			//Compute linearly interpolated external channel flows for this time step
 			swpovinterp[i][j] = (float)(mswpov[i][j] * (simtime - nswpovt[i][j]) + bswpov[i][j]);
 
 		}	//end loop over number of overland point source solids load functions

		//If it is time to update any overland distributed solids load time function (simtime >= ntswdov[])
		if(simtime >= ntswdov[i])
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the sediment loads are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Loop over number of overland loads for current solids type
			for(j=1; j<=nswdov[i]; j++)
			{
				//if it is time to update values for this sediment load...
				//
				//nswdovt = next time value in overland distributed solids load time series
				//pswdovt = prior overland distributed solids load time value in time series
				//
				if(simtime >= nswdovt[i][j] || simtime < pswdovt[i][j])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = swdovtime[i][j][nswdovpairs[i][j]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
					
					//Pointer to the last "current" time interval index for this 
					//sediment load.
					ip = swdovpoint[i][j];
					
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= swdovtime[i][j][ip+1])
					{
						ip = ip + 1;

					}	//end while
						
					while((mtime < swdovtime[i][j][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bswdov[i][j] = swdov[i][j][ip+1];

					//Compute slope (m) for new interval
					mswdov[i][j] = (swdov[i][j][ip] - swdov[i][j][ip+1])
						/ (swdovtime[i][j][ip] - swdovtime[i][j][ip+1]);
					
					//Number of times cycled through the sediment load time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);

					//Next time interval upper bounds to recalculate this sediment load.
					nswdovt[i][j] = (float)(ncycle * endtime) + swdovtime[i][j][ip+1];

					//Next time interval lower bounds to recalculate this sediment load.
					pswdovt[i][j] = (float)(ncycle * endtime) + swdovtime[i][j][ip];

					//Save the pointer to the current time interval for this sediment load.
					swdovpoint[i][j] = ip;

				}	//	End if simtime >= nswdovt or simtime < pswdovt

				// Get the necessary time window bounds to update any sediment load.
				if (nswdovt[i][j] < nt) nt = nswdovt[i][j];
				if (pswdovt[i][j] > pt) pt = pswdovt[i][j];
					
			}	//end loop over number overland loads for current solid

			//Store the lower time window to update overland solids load functions (for function with lowest time break)
			ntswdov[i] = nt;

		}	//end if simtime >= ntswdov[i]

 		//Loop over number of overland load functions to update interpolated value
 		//each time step using current slope and intercept
 		for(j=1; j<=nswdov[i]; j++)
 		{
 			//Compute linearly interpolated external channel flows for this time step
 			swdovinterp[i][j] = (float)(mswdov[i][j] * (simtime - nswdovt[i][j]) + bswdov[i][j]);
 
 		}	//end loop over number of overland distributed solids load functions

	}	//end loop over number of solids

	//if channels are simulated
	if(chnopt > 0)
	{
		//Update channel load functions...
		//
		//loop over number of solids
		for(i=1; i<=nsolids; i++)
		{
			//If it is time to update any channel solids load time function (simtime >= ntswch[])
			if(simtime >= ntswch[i])
			{
				//Initialize the lower and upper bounds of the _next_ time window
				//in which the sediment loads are to be updated.
				nt = 1.0e+6;	//lower bound for next time window
				pt = 0.0;		//upper bound for next time window

				//Update sediment loads...
				//
				//Loop over number of loads for current solids type
				for(j=1; j <= nswch[i]; j++)
				{
					//if it is time to update values for this sediment load...
					//
					//nswcht = next time value in sediment load time series
					//pswcht = prior sediment load time value in sediment load time series
					//
					if(simtime >= nswcht[i][j] || simtime < pswcht[i][j])
					{
						//If the simulation time is greater than the last time specified,
						//start again (cyclically) with the first value in the array.  So
						//work with the current time, modulo the last entry in the array.
						endtime = swchtime[i][j][nswchpairs[i][j]];

						//mtime = mod(simtime,endtime)	(original code from ipx)
						//
						//mtime is remaining part of time series
						mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
						
						//Pointer to the last "current" time interval index for this 
						//sediment load.
						ip = swchpoint[i][j];
						
						//Search upward or downward to get the index corresponding
						//to the current simulation time.
						while(mtime >= swchtime[i][j][ip+1])
						{
							ip = ip + 1;

						}	//end while
							
						while((mtime < swchtime[i][j][ip]) && (ip != 1))
						{
							ip = ip - 1;

						}	//end while

						//Linear interpolation is perfomed following the form:
						//
						//  y = m x + b
						//
						//Compute intercept (b) for new interval
						bswch[i][j] = swch[i][j][ip+1];

						//Compute slope (m) for new interval
						mswch[i][j] = (swch[i][j][ip] - swch[i][j][ip+1])
							/ (swchtime[i][j][ip] - swchtime[i][j][ip+1]);
						
						//Number of times cycled through the sediment load time interval array for the
						//current value of the simulation time.
						ncycle = (int)(simtime / endtime);

						//Next time interval upper bounds to recalculate this sediment load.
						nswcht[i][j] = (float)(ncycle * endtime) + swchtime[i][j][ip+1];

						//Next time interval lower bounds to recalculate this sediment load.
						pswcht[i][j] = (float)(ncycle * endtime) + swchtime[i][j][ip];

						//Save the pointer to the current time interval for this sediment load.
						swchpoint[i][j] = ip;

					}	//	End if simtime >= nswcht or simtime < pswcht

					// Get the necessary time window bounds to update any sediment load.
					if (nswcht[i][j] < nt) nt = nswcht[i][j];
					if (pswcht[i][j] > pt) pt = pswcht[i][j];
						
					//Compute linearly interpolated sediment load for this time step
					swchinterp[i][j] = mswch[i][j] * swch[i][j][swchpoint[i][j]] + bswch[i][j];

				}	//end loop over number loads for current solid

				//Store the lower time window to update sediment load functions (for function with lowest time break)
				ntswch[i] = nt;

			}	//end if simtime >= ntswch[i]

 			//Loop over number of channel load functions to update interpolated value
 			//each time step using current slope and intercept
 			for(j=1; j<=nswch[i]; j++)
 			{
 				//Compute linearly interpolated external channel flows for this time step
 				swchinterp[i][j] = (float)(mswch[i][j] * (simtime - nswcht[i][j]) + bswch[i][j]);

 			}	//end loop over number of channel load functions

		}	//end loop over number of solids

	}	//end if chnopt > 0

	//Domain outlets/boundaries...
	//
	//Initialize the lower and upper bounds of the _next_ time window
	//in which the outlet solids BCs are to be updated.
	nt = 1.0e+6;	//lower bound for next time window
	pt = 0.0;		//upper bound for next time window

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//if boundary conditions are specified for this outlet (dbcopt[] > 0)
		if(dbcopt[i] > 0)
		{
			//If it is time to update any boundary solids concentration time function (simtime >= ntsbc[])
			if(simtime >= ntsbc[i])
			{
				//Update solids boundary conditions...
				//
				//Loop over number of solids
				for(j=1; j<=nsolids; j++)
				{
					//if it is time to update values for this solids BC...
					//
					//nsbct = next time value in solids BC time series
					//psbct = prior solids BC time value in BC time series
					//
					if(simtime >= nsbct[i][j] || simtime < psbct[i][j])
					{
						//If the simulation time is greater than the last time specified,
						//start again (cyclically) with the first value in the array.  So
						//work with the current time, modulo the last entry in the array.
						endtime = sbctime[i][j][nsbcpairs[i][j]];

						//mtime = mod(simtime,endtime)	(original code from ipx)
						//
						//mtime is remaining part of time series
						mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
						
						//Pointer to the last "current" time interval index for this 
						//sediment BC.
						ip = sbcpoint[i][j];
						
						//Search upward or downward to get the index corresponding
						//to the current simulation time.
						while(mtime >= sbctime[i][j][ip+1])
						{
							ip = ip + 1;

						}	//end while
							
						while((mtime < sbctime[i][j][ip]) && (ip != 1))
						{
							ip = ip - 1;

						}	//end while

						//Linear interpolation is perfomed following the form:
						//
						//  y = m x + b
						//
						//Compute intercept (b) for new interval
						bsbc[i][j] = sbc[i][j][ip+1];

						//Compute slope (m) for new interval
						msbc[i][j] = (sbc[i][j][ip] - sbc[i][j][ip+1])
							/ (sbctime[i][j][ip] - sbctime[i][j][ip+1]);

						//Number of times cycled through the sediment BC time interval array for the
						//current value of the simulation time.
						ncycle = (int)(simtime / endtime);
						
						//Next time interval upper bounds to recalculate this sediment BC.
						nsbct[i][j] = (float)(ncycle * endtime) + sbctime[i][j][ip+1];
						
						//Next time interval lower bounds to recalculate this sediment BC.
						psbct[i][j] = (float)(ncycle * endtime) + sbctime[i][j][ip];
						
						//Save the pointer to the current time interval for this sediment BC.
						sbcpoint[i][j] = ip;
						
					}	//	End if simtime >= nsbct or simtime < psbct

					// Get the necessary time window bounds to update any sediment BC.
					if (nsbct[i][j] < nt) nt = nsbct[i][j];
					if (psbct[i][j] > pt) pt = psbct[i][j];
						
				}	//end loop over number solids

				//Store the lower time window to update sediment boundary conditions (for function with lowest time break)
				ntsbc[i] = nt;

			}	//end if simtime >= ntsbc[i]

 			//Loop over number of solids types to update interpolated value
 			//each time step using current slope and intercept
 			for(j=1; j<=nsolids; j++)
 			{
 				//Compute linearly interpolated outlet boundary concentration for this time step
 				sbcinterp[i][j] = (float)(msbc[i][j] * (simtime - nsbct[i][j]) + bsbc[i][j]);
	 
 			}	//end loop over solids types

		}	//end if dbcopt[i] > 0

	}	//end loop over number of outlets

//End of function: Return to UpdateTimeFunction
}
