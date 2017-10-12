/*----------------------------------------------------------------------
C-  Function:   UpdateTimeFunctionWater.c
C-
C-	Purpose/    UpdateTimeFunctionWater is called to interpolate values
C-  Methods:    of time-dependent functions for a given dt based on input
C-              values at specific times.  Forcing functions for rain, snow
C-              overland flows, and channel flows are updated as needed
C-              for the next time step (t + dt).  Boundary conditions for
C-              outlets are also updated.  Linear intepolation between
C-              input values is performed.  The convention used is:
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
C-	Inputs:	    rfintensity[][], sfintensity[][], qwov[][], qwch[][], hbc[][]
C-              (at some time t in time series)
C-
C-	Outputs:    rfinterp[][], sfinterp[][], qwovinterp[][], qwchinterp[][],
C-              (at current simulation time)
C-
C-	Controls:   chnopt, nrg, nrpairs, nsg, nspairs, nqwov, nqwovpairs,
C-              nqwch, nqwchpairs, noutlets, dbcopt, nqbcpairs
C-
C-	Calls:      None
C-
C-	Called by:  UpdateTimeFunction
C-
C-	Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO 80523
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:       18-MAY-2004
C-
C-	Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-	Date:       23-JAN-2007
C-
C-	Revisions:  Added time functions for snowfall
C-
C-	Revised:   John England, USBR
C-
C-	Date: 15-SEP-2008
C-
C-	Revisions: looping for rainfall options
C-
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void UpdateTimeFunctionWater()
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

	//if rainopt <=4 (uniform or distributed gage data - includes all cases)
	if(rainopt <= 4)
	{
		//If it is time to update any rainfall time function (simtime >= ntr)
		if(simtime >= ntr)
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the rainfall functions are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Loop over number of rain gages (rainfall functions)
			for(i=1; i<=nrg; i++)
			{
				//if it is time to update values for this rain gage...
				//
				//nrft = next time value in rainfall intensity time series
				//prft = prior rainfall time value in rainfall intensity time series
				//
				//if the simulation time is outside the window (>= next, < prior)
				if(simtime >= nrft[i] || simtime < prft[i])
				{
					//If the simulation time is greater than the last time specified,
					//cycle through the array...
					//
					//Get the end time of the time series (time value for last pair in series)
					endtime = rftime[i][nrpairs[i]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

					//Pointer to the last "current" time interval index for this
					//rainfall intensity time series
					ip = rfpoint[i];
					
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= rftime[i][ip+1])
					{
						ip = ip + 1;

					}	//end while
						
					while((mtime < rftime[i][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					brf[i] = rfintensity[i][ip+1];

					//Compute slope (m) for new interval
					mrf[i] = (rfintensity[i][ip] - rfintensity[i][ip+1])
						/ (rftime[i][ip] - rftime[i][ip+1]);
					
					//Number of times cycled through the time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);
					
					//Next time interval upper bounds to recalculate this rainfall function.
					nrft[i] = (float)(ncycle * endtime) + rftime[i][ip+1];
					
					//Next time interval lower bounds to recalculate this rainfall function.
					prft[i] = (float)(ncycle * endtime) + rftime[i][ip];
					
					//Save the pointer to the current time interval for this rainfall function.
					rfpoint[i] = ip;

				}	//	End if simtime >= nrft or simtime < prft
					
				// Get the necessary time window bounds to update any rainfall function.
				if (nrft[i] < nt) nt = nrft[i];
				if (prft[i] > pt) pt = prft[i];
					
			}	//end loop over rain gages (rainfall functions)

			//Store the lower time window to update rainfall functions (for function with lowest time break)
			ntr = nt;

		}	//end if simtime >= ntr

		//Loop over number of rain gages (rainfall functions) to update interpolated value
		//each time step using current slope and intercept
 		for(i=1; i<=nrg; i++)
 		{
 			//calculate interpolated rainfall value
 			rfinterp[i] = (float)(mrf[i] * (simtime - nrft[i]) + brf[i]);
 
 		}	//end loop over rain gages (rainfall functions)
	}
	else	//rainopt > 4
	{
		//insert update functions for other rainfall cases (i.e. radar, etc...)

	}	//end if rainopt <= 4

	//if snowopt = 2 (snowfall input using distributed gage data)
	if(snowopt == 2)
	{
		//If it is time to update any snowfall time function (simtime >= nts)
		if(simtime >= nts)
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the snowfall functions are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Loop over number of snow gages (snowfall functions)
			for(i=1; i<=nsg; i++)
			{
				//if it is time to update values for this snow gage...
				//
				//nsft = next time value in snowfall intensity time series
				//psft = prior snowfall time value in snowfall intensity time series
				//
				//if the simulation time is outside the window (>= next, < prior)
				if(simtime >= nsft[i] || simtime < psft[i])
				{
					//If the simulation time is greater than the last time specified,
					//cycle through the array...
					//
					//Get the end time of the time series (time value for last pair in series)
					endtime = sftime[i][nspairs[i]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

					//Pointer to the last "current" time interval index for this
					//snowfall intensity time series
					ip = sfpoint[i];
					
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= sftime[i][ip+1])
					{
						ip = ip + 1;

					}	//end while
						
					while((mtime < sftime[i][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bsf[i] = sfintensity[i][ip+1];

					//Compute slope (m) for new interval
					msf[i] = (sfintensity[i][ip] - sfintensity[i][ip+1])
						/ (sftime[i][ip] - sftime[i][ip+1]);
					
					//Number of times cycled through the time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);
					
					//Next time interval upper bounds to recalculate this snowfall function.
					nsft[i] = (float)(ncycle * endtime) + sftime[i][ip+1];
					
					//Next time interval lower bounds to recalculate this snowfall function.
					psft[i] = (float)(ncycle * endtime) + sftime[i][ip];
					
					//Save the pointer to the current time interval for this snowfall function.
					sfpoint[i] = ip;

				}	//	End if simtime >= nsft or simtime < psft
					
				// Get the necessary time window bounds to update any snowfall function.
				if (nsft[i] < nt) nt = nsft[i];
				if (psft[i] > pt) pt = psft[i];
					
			}	//end loop over snow gages (snowfall functions)

			//Store the lower time window to update snowfall functions (for function with lowest time break)
			nts = nt;

		}	//end if simtime >= nts

		//Loop over number of snow gages (snowfall functions) to update interpolated value
		//each time step using current slope and intercept
 		for(i=1; i<=nsg; i++)
 		{
 			//calculate interpolated snowfall value
 			sfinterp[i] = (float)(msf[i] * (simtime - nsft[i]) + bsf[i]);
 
 		}	//end loop over snow gages (snowfall functions)
	}
	else	//snowopt > 1
	{
		//insert update functions for other snowfall cases (i.e. radar, etc...)

	}	//end if snowopt <= 1













	//if overland point sources are simulated
	if(nqwov > 0)
	{
		//If it is time to update any overland flow point source (simtime >= ntqwov)
		if(simtime >= ntqwov)
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the overland point sources are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window
		
			//Loop over number of overland flow functions
			for(i=1; i<=nqwov; i++)
			{
				//if it is time to update values for this flow source...
				//
				//nqwovt = next time value in overland flow time series
				//pqwovt = prior time value in overland flow time series
				//
				if(simtime >= nqwovt[i] || simtime < pqwovt[i])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = qwovtime[i][nqwovpairs[i]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
					
					//Pointer to the last "current" time interval index for this 
					//flow boundary condition.
					ip = qwovpoint[i];
					
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= qwovtime[i][ip+1])
					{
						ip = ip + 1;

					}	//end while
						
					while((mtime < qwovtime[i][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bqwov[i] = qwov[i][ip+1];

					//Compute slope (m) for new interval
					mqwov[i] = (qwov[i][ip] - qwov[i][ip+1])
						/ (qwovtime[i][ip] - qwovtime[i][ip+1]);
					
					//Number of times cycled through the flow time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);
					
					//Next time interval upper bounds to recalculate this external flow BC.
					nqwovt[i] = (float)(ncycle * endtime) + qwovtime[i][ip+1];
					
					//Next time interval lower bounds to recalculate this external flow BC.
					pqwovt[i] = (float)(ncycle * endtime) + qwovtime[i][ip];
					
					//Save the pointer to the current time interval for this external flow BC.
					qwovpoint[i] = ip;
					
				}	//	End if simtime >= nqwcht or simtime < pqwcht

				// Get the necessary time window bounds to update any flow boundary condition.
				if (nqwovt[i] < nt) nt = nqwovt[i];
				if (pqwovt[i] > pt) pt = pqwovt[i];
					
			}	//end loop over number of flow functions

			//Store the lower time window to update external flow functions (for function with lowest time break)
			ntqwov = nt;

		}	//end if simtime >= ntqwov

 		//Loop over number of overland flow functions to update interpolated value
 		//each time step using current slope and intercept
 		for(i=1; i<=nqwov; i++)
 		{
 			//Compute linearly interpolated external overland flows for this time step
 			qwovinterp[i] = (float)(mqwov[i] * (simtime - nqwovt[i]) + bqwov[i]);
 
 		}	//end loop over number of overland flow functions

	}	//end if nqwov > 0

	//if channels are simulated
	if(chnopt > 0)
	{
		//If it is time to update any channel flow point source (simtime >= ntqwch)
		if(simtime >= ntqwch)
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the boundary conditions are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window
		
			//Loop over number of flow functions
			for(i=1; i<=nqwch; i++)
			{
				//if it is time to update values for this flow source...
				//
				//nqwcht = next time value in external flow time series
				//pqwcht = prior external flow time value in external flow time series
				//
				if(simtime >= nqwcht[i] || simtime < pqwcht[i])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = qwchtime[i][nqwchpairs[i]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
					
					//Pointer to the last "current" time interval index for this 
					//flow boundary condition.
					ip = qwchpoint[i];
					
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= qwchtime[i][ip+1])
					{
						ip = ip + 1;

					}	//end while
						
					while((mtime < qwchtime[i][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bqwch[i] = qwch[i][ip+1];

					//Compute slope (m) for new interval
					mqwch[i] = (qwch[i][ip] - qwch[i][ip+1])
						/ (qwchtime[i][ip] - qwchtime[i][ip+1]);
					
					//Number of times cycled through the flow time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);
					
					//Next time interval upper bounds to recalculate this external flow BC.
					nqwcht[i] = (float)(ncycle * endtime) + qwchtime[i][ip+1];
					
					//Next time interval lower bounds to recalculate this external flow BC.
					pqwcht[i] = (float)(ncycle * endtime) + qwchtime[i][ip];
					
					//Save the pointer to the current time interval for this external flow BC.
					qwchpoint[i] = ip;
					
				}	//	End if simtime >= nqwcht or simtime < pqwcht

				// Get the necessary time window bounds to update any flow boundary condition.
				if (nqwcht[i] < nt) nt = nqwcht[i];
				if (pqwcht[i] > pt) pt = pqwcht[i];
					
			}	//end loop over number of flow functions

			//Store the lower time window to update external flow functions (for function with lowest time break)
			ntqwch = nt;

		}	//end if simtime >= ntqwch

 		//Loop over number of channel flow functions to update interpolated value
 		//each time step using current slope and intercept
 		for(i=1; i<=nqwch; i++)
 		{
 			//Compute linearly interpolated external channel flows for this time step
 			qwchinterp[i] = (float)(mqwch[i] * (simtime - nqwcht[i]) + bqwch[i]);
 
 		}	//end loop over number of channel flow functions

	}	//end if chnopt > 0

	//Domain outlets/boundaries...
	//
	//If it is time to update any outlet/boundary water depth (simtime >= nthbc)
	if(simtime >= nthbc)
	{
		//Initialize the lower and upper bounds of the _next_ time window
		//in which the outlet water depths are to be updated.
		nt = 1.0e+6;	//lower bound for next time window
		pt = 0.0;		//upper bound for next time window

		//Loop over number of outlet water depth functions
		for(i=1; i<=noutlets; i++)
		{
			//if a time series is specified for this outlet
			if(dbcopt[i] > 0)
			{
				//if it is time to update values for this outlet...
				//
				//nhbct = next time value in water depth time series
				//phbct = prior time value in water depth time series
				//
				if(simtime >= nhbct[i] || simtime < phbct[i])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = hbctime[i][nhbcpairs[i]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
					
					//Pointer to the last "current" time interval index for this 
					//flow boundary condition.
					ip = hbcpoint[i];
					
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= hbctime[i][ip+1])
					{
						ip = ip + 1;

					}	//end while
						
					while((mtime < hbctime[i][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bhbc[i] = hbc[i][ip+1];

					//Compute slope (m) for new interval
					mhbc[i] = (hbc[i][ip] - hbc[i][ip+1])
						/ (hbctime[i][ip] - hbctime[i][ip+1]);
					
					//Number of times cycled through the flow time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);
					
					//Next time interval upper bounds to recalculate this BC.
					nhbct[i] = (float)(ncycle * endtime) + hbctime[i][ip+1];
					
					//Next time interval lower bounds to recalculate this BC.
					phbct[i] = (float)(ncycle * endtime) + hbctime[i][ip];
					
					//Save the pointer to the current time interval for this BC.
					hbcpoint[i] = ip;
					
				}	//	End if simtime >= nqwcht or simtime < pqwcht

				// Get the necessary time window bounds to update any flow boundary condition.
				if (nhbct[i] < nt) nt = nhbct[i];
				if (phbct[i] > pt) pt = phbct[i];
					
				//Compute linearly interpolated external flows for this time step
				hbcinterp[i] = mhbc[i] * hbc[i][hbcpoint[i]] + bhbc[i];

			}	//end if dbcopt[] > 0

		}	//end loop over outlets

		//Store the lower time window to update water depth functions (for function with lowest time break)
		nthbc = nt;
		
	}	//end if simtime >= nthbc

 	//Loop over number of outlets to update interpolated value
 	//each time step using current slope and intercept
 	for(i=1; i<=noutlets; i++)
 	{
		//if a time series is specified for this outlet
		if(dbcopt[i] > 0)
		{
			//Compute linearly interpolated external flow boundary conditions for this time step
			hbcinterp[i] = (float)(mhbc[i] * (simtime - nhbct[i]) + bhbc[i]);

		}	//end if dbcopt[i] > 0
	 
 	}	//end loop over number of outlets	    

//End of function: Return to UpdateTimeFunction
}
