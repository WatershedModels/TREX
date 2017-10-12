/*----------------------------------------------------------------------
C-  Function:	UpdateTimeFunctionEnvironment.c
C-
C-	Purpose/	UpdateTimeFunctionEnvironment is called to interpolate
C-  Methods:    values of time-dependent functions for a given dt based
C-              on input values at specific times.  Forcing functions
C-              for overland and channel environmental properties  are
C-              updated as needed for the next time step (t + dt).
C-              Linear intepolation between input values is performed.
C-              The convention used is:
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
C-	Inputs:		envgtf[][][], envovtf[][][], envchtf[][][],
C-              focovtf[][][], focchtf[][][]
C-              (at some time t in time series)
C-
C-	Outputs:	envgtfinterp[][], envovtfinterp[][], envchtfinterp[][],
C-              focovtfinterp[][], focchtfinterp[][]
C-              (at current simulation time)
C-
C-	Controls:	chnopt, npropg, npropov, npropch, nsolids,
C-              nenvgtf, nenvovtf, nenvchtf, nfpocovtf, nfpocchtf,
C-              nenvgtfpairs, nenvovtfpairs, nenvchtfpairs,
C-              nfpocovpairs, nfpocchpairs
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
C-	Date:		7-SEP-2004
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

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void UpdateTimeFunctionEnvironment()
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

	//Update general environmental functions...
	//
	//loop over number of general functions
	for(i=1; i<=npropg; i++)
	{
		//If it is time to update any time function for this property (simtime >= ntenvg[])
		if(simtime >= ntenvgtf[i])
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the functions are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Loop over number of functions
			for(j=1; j<=nenvgtf[i]; j++)
			{
				//if it is time to update values for this function...
				//
				//nwst = next time value in property time series
				//pwst = prior time value in property time series
				//
				if(simtime >= nenvgtft[i][j] || simtime < penvgtft[i][j])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = envgtftime[i][j][nenvgtfpairs[i][j]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

					//Pointer to the last "current" time interval index for this 
					//function.
					ip = envgtfpoint[i][j];
						
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= envgtftime[i][j][ip+1])
					{
						ip = ip + 1;

					}	//end while
							
					while((mtime < envgtftime[i][j][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					benvgtf[i][j] = envgtf[i][j][ip+1];

					//Compute slope (m) for new interval
					menvgtf[i][j] = (envgtf[i][j][ip] - envgtf[i][j][ip+1])
						/ (envgtftime[i][j][ip] - envgtftime[i][j][ip+1]);
					
					//Number of times cycled through the function time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);

					//Next time interval upper bound to recalculate this function
					nenvgtft[i][j] = (float)(ncycle * endtime) + envgtftime[i][j][ip+1];

					//Next time interval lower bound to recalculate this function
					penvgtft[i][j] = (float)(ncycle * endtime) + envgtftime[i][j][ip];

					//Save the pointer to the current time interval for this function
					envgtfpoint[i][j] = ip;

				}	//End if simtime >= envgtft or simtime < envgtft

				//Get the necessary time window bounds to update any function
				if (nenvgtft[i][j] < nt) nt = nenvgtft[i][j];
				if (penvgtft[i][j] > pt) pt = penvgtft[i][j];
					
			}	//end loop over number functions for the current property

			//Store the lower time window to update functions for this property (function with lowest time break)
			ntenvgtf[i] = nt;

		}	//end if simtime >= ntenvgtf[i]

 		//Loop over number of functions to update interpolated value
 		//each time step using current slope and intercept
 		for(j=1; j<=nenvgtf[i]; j++)
 		{
 			//Compute linearly interpolated values for this time step
 			envgtfinterp[i][j] = (float)(menvgtf[i][j] * (simtime - nenvgtft[i][j]) + benvgtf[i][j]);
 
 		}	//end loop over number of functions

	}	//end loop over number of general properties

	//Update overland environmental functions...
	//
	//loop over number of overland functions
	for(i=1; i<=npropov; i++)
	{
		//If it is time to update any time function for this property (simtime >= ntenvov[])
		if(simtime >= ntenvovtf[i])
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the functions are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Loop over number of functions
			for(j=1; j<=nenvovtf[i]; j++)
			{
				//if it is time to update values for this function...
				//
				//nenvovtft = next time value in property time series
				//penvovtft = prior time value in property time series
				//
				if(simtime >= nenvovtft[i][j] || simtime < penvovtft[i][j])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = envovtftime[i][j][nenvovtfpairs[i][j]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

					//Pointer to the last "current" time interval index for this 
					//function.
					ip = envovtfpoint[i][j];
						
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= envovtftime[i][j][ip+1])
					{
						ip = ip + 1;

					}	//end while
							
					while((mtime < envovtftime[i][j][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					benvovtf[i][j] = envovtf[i][j][ip+1];

					//Compute slope (m) for new interval
					menvovtf[i][j] = (envovtf[i][j][ip] - envovtf[i][j][ip+1])
						/ (envovtftime[i][j][ip] - envovtftime[i][j][ip+1]);
					
					//Number of times cycled through the function time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);

					//Next time interval upper bound to recalculate this function
					nenvovtft[i][j] = (float)(ncycle * endtime) + envovtftime[i][j][ip+1];

					//Next time interval lower bound to recalculate this function
					penvovtft[i][j] = (float)(ncycle * endtime) + envovtftime[i][j][ip];

					//Save the pointer to the current time interval for this function
					envovtfpoint[i][j] = ip;

				}	//End if simtime >= envovtft or simtime < envovtft

				//Get the necessary time window bounds to update any function
				if (nenvovtft[i][j] < nt) nt = nenvovtft[i][j];
				if (penvovtft[i][j] > pt) pt = penvovtft[i][j];
					
			}	//end loop over number functions for the current property

			//Store the lower time window to update functions for this property (function with lowest time break)
			ntenvovtf[i] = nt;

		}	//end if simtime >= ntenvovtf[i]

 		//Loop over number of functions to update interpolated value
 		//each time step using current slope and intercept
 		for(j=1; j<=nenvovtf[i]; j++)
 		{
 			//Compute linearly interpolated values for this time step
 			envovtfinterp[i][j] = (float)(menvovtf[i][j] * (simtime - nenvovtft[i][j]) + benvovtf[i][j]);
 
 		}	//end loop over number of functions

	}	//end loop over number of overland properties

	//if channels are simulated
	if(chnopt > 0)
	{
		//Update channel environmental functions...
		//
		//loop over number of channel functions
		for(i=1; i<=npropch; i++)
		{
			//If it is time to update any time function for this property (simtime >= ntenvch[])
			if(simtime >= ntenvchtf[i])
			{
				//Initialize the lower and upper bounds of the _next_ time window
				//in which the functions are to be updated.
				nt = 1.0e+6;	//lower bound for next time window
				pt = 0.0;		//upper bound for next time window

				//Loop over number of functions
				for(j=1; j<=nenvchtf[i]; j++)
				{
					//if it is time to update values for this function...
					//
					//nenvchtft = next time value in property time series
					//penvchtft = prior time value in property time series
					//
					if(simtime >= nenvchtft[i][j] || simtime < penvchtft[i][j])
					{
						//If the simulation time is greater than the last time specified,
						//start again (cyclically) with the first value in the array.  So
						//work with the current time, modulo the last entry in the array.
						endtime = envchtftime[i][j][nenvchtfpairs[i][j]];

						//mtime = mod(simtime,endtime)	(original code from ipx)
						//
						//mtime is remaining part of time series
						mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

						//Pointer to the last "current" time interval index for this 
						//function.
						ip = envchtfpoint[i][j];
							
						//Search upward or downward to get the index corresponding
						//to the current simulation time.
						while(mtime >= envchtftime[i][j][ip+1])
						{
							ip = ip + 1;

						}	//end while
								
						while((mtime < envchtftime[i][j][ip]) && (ip != 1))
						{
							ip = ip - 1;

						}	//end while

						//Linear interpolation is perfomed following the form:
						//
						//  y = m x + b
						//
						//Compute intercept (b) for new interval
						benvchtf[i][j] = envchtf[i][j][ip+1];

						//Compute slope (m) for new interval
						menvchtf[i][j] = (envchtf[i][j][ip] - envchtf[i][j][ip+1])
							/ (envchtftime[i][j][ip] - envchtftime[i][j][ip+1]);
						
						//Number of times cycled through the function time interval array for the
						//current value of the simulation time.
						ncycle = (int)(simtime / endtime);

						//Next time interval upper bound to recalculate this function
						nenvchtft[i][j] = (float)(ncycle * endtime) + envchtftime[i][j][ip+1];

						//Next time interval lower bound to recalculate this function
						penvchtft[i][j] = (float)(ncycle * endtime) + envchtftime[i][j][ip];

						//Save the pointer to the current time interval for this function
						envchtfpoint[i][j] = ip;

					}	//End if simtime >= envchtft or simtime < envchtft

					//Get the necessary time window bounds to update any function
					if (nenvchtft[i][j] < nt) nt = nenvchtft[i][j];
					if (penvchtft[i][j] > pt) pt = penvchtft[i][j];
						
				}	//end loop over number functions for the current property

				//Store the lower time window to update functions for this property (function with lowest time break)
				ntenvchtf[i] = nt;

			}	//end if simtime >= ntenvchtf[i]

 			//Loop over number of functions to update interpolated value
 			//each time step using current slope and intercept
 			for(j=1; j<=nenvchtf[i]; j++)
 			{
 				//Compute linearly interpolated values for this time step
 				envchtfinterp[i][j] = (float)(menvchtf[i][j] * (simtime - nenvchtft[i][j]) + benvchtf[i][j]);
	 
 			}	//end loop over number of functions

		}	//end loop over number of channel properties

	}	//end if chnopt > 0

	//if fpocovopt > 0 (overland particle fpoc is specified)
	if(fpocovopt > 0)
	{
		//Update overland particle functions...
		//
		//loop over number of overland particle functions
		for(i=1; i<=nsolids; i++)
		{
			//If it is time to update any time function for this property (simtime >= ntfpocov[])
			if(simtime >= ntfpocovtf[i])
			{
				//Initialize the lower and upper bounds of the _next_ time window
				//in which the functions are to be updated.
				nt = 1.0e+6;	//lower bound for next time window
				pt = 0.0;		//upper bound for next time window

				//Loop over number of functions
				for(j=1; j<=nfpocovtf[i]; j++)
				{
					//if it is time to update values for this function...
					//
					//nfpocovtft = next time value in property time series
					//pfpocovtft = prior time value in property time series
					//
					if(simtime >= nfpocovtft[i][j] || simtime < pfpocovtft[i][j])
					{
						//If the simulation time is greater than the last time specified,
						//start again (cyclically) with the first value in the array.  So
						//work with the current time, modulo the last entry in the array.
						endtime = fpocovtftime[i][j][nfpocovtfpairs[i][j]];

						//mtime = mod(simtime,endtime)	(original code from ipx)
						//
						//mtime is remaining part of time series
						mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

						//Pointer to the last "current" time interval index for this 
						//function.
						ip = fpocovtfpoint[i][j];
							
						//Search upward or downward to get the index corresponding
						//to the current simulation time.
						while(mtime >= fpocovtftime[i][j][ip+1])
						{
							ip = ip + 1;

						}	//end while
								
						while((mtime < fpocovtftime[i][j][ip]) && (ip != 1))
						{
							ip = ip - 1;

						}	//end while

						//Linear interpolation is perfomed following the form:
						//
						//  y = m x + b
						//
						//Compute intercept (b) for new interval
						bfpocovtf[i][j] = fpocovtf[i][j][ip+1];

						//Compute slope (m) for new interval
						mfpocovtf[i][j] = (fpocovtf[i][j][ip] - fpocovtf[i][j][ip+1])
							/ (fpocovtftime[i][j][ip] - fpocovtftime[i][j][ip+1]);
						
						//Number of times cycled through the function time interval array for the
						//current value of the simulation time.
						ncycle = (int)(simtime / endtime);

						//Next time interval upper bound to recalculate this function
						nfpocovtft[i][j] = (float)(ncycle * endtime) + fpocovtftime[i][j][ip+1];

						//Next time interval lower bound to recalculate this function
						pfpocovtft[i][j] = (float)(ncycle * endtime) + fpocovtftime[i][j][ip];

						//Save the pointer to the current time interval for this function
						fpocovtfpoint[i][j] = ip;

					}	//End if simtime >= fpocovtft or simtime < fpocovtft

					//Get the necessary time window bounds to update any function
					if (nfpocovtft[i][j] < nt) nt = nfpocovtft[i][j];
					if (pfpocovtft[i][j] > pt) pt = pfpocovtft[i][j];
						
				}	//end loop over number functions for the current property

				//Store the lower time window to update functions for this property (function with lowest time break)
				ntfpocovtf[i] = nt;

			}	//end if simtime >= ntfpocovtf[i]

 			//Loop over number of functions to update interpolated value
 			//each time step using current slope and intercept
 			for(j=1; j<=nfpocovtf[i]; j++)
 			{
 				//Compute linearly interpolated values for this time step
 				fpocovtfinterp[i][j] = (float)(mfpocovtf[i][j] * (simtime - nfpocovtft[i][j]) + bfpocovtf[i][j]);
		
 			}	//end loop over number of functions

		}	//end loop over number of solids

	}	//end if fpocovopt > 0

	//if channels are simulated
	if(chnopt > 0)
	{
		//if fpocchopt > 0 (channel particle foc is specified)
		if(fpocchopt > 0)
		{
			//Update channel particle functions...
			//
			//loop over number of channel particle functions
			for(i=1; i<=nsolids; i++)
			{
				//If it is time to update any time function for this property (simtime >= ntfpocch[])
				if(simtime >= ntfpocchtf[i])
				{
					//Initialize the lower and upper bounds of the _next_ time window
					//in which the functions are to be updated.
					nt = 1.0e+6;	//lower bound for next time window
					pt = 0.0;		//upper bound for next time window

					//Loop over number of functions
					for(j=1; j<=nfpocchtf[i]; j++)
					{
						//if it is time to update values for this function...
						//
						//nfpocchtft = next time value in property time series
						//pfpocchtft = prior time value in property time series
						//
						if(simtime >= nfpocchtft[i][j] || simtime < pfpocchtft[i][j])
						{
							//If the simulation time is greater than the last time specified,
							//start again (cyclically) with the first value in the array.  So
							//work with the current time, modulo the last entry in the array.
							endtime = fpocchtftime[i][j][nfpocchtfpairs[i][j]];

							//mtime = mod(simtime,endtime)	(original code from ipx)
							//
							//mtime is remaining part of time series
							mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

							//Pointer to the last "current" time interval index for this 
							//wind function.
							ip = fpocchtfpoint[i][j];
								
							//Search upward or downward to get the index corresponding
							//to the current simulation time.
							while(mtime >= fpocchtftime[i][j][ip+1])
							{
								ip = ip + 1;

							}	//end while
									
							while((mtime < fpocchtftime[i][j][ip]) && (ip != 1))
							{
								ip = ip - 1;

							}	//end while

							//Linear interpolation is perfomed following the form:
							//
							//  y = m x + b
							//
							//Compute intercept (b) for new interval
							bfpocchtf[i][j] = fpocchtf[i][j][ip+1];

							//Compute slope (m) for new interval
							mfpocchtf[i][j] = (fpocchtf[i][j][ip] - fpocchtf[i][j][ip+1])
								/ (fpocchtftime[i][j][ip] - fpocchtftime[i][j][ip+1]);
							
							//Number of times cycled through the function time interval array for the
							//current value of the simulation time.
							ncycle = (int)(simtime / endtime);

							//Next time interval upper bound to recalculate this function
							nfpocchtft[i][j] = (float)(ncycle * endtime) + fpocchtftime[i][j][ip+1];

							//Next time interval lower bound to recalculate this function
							pfpocchtft[i][j] = (float)(ncycle * endtime) + fpocchtftime[i][j][ip];

							//Save the pointer to the current time interval for this function
							fpocchtfpoint[i][j] = ip;

						}	//End if simtime >= fpocchtft or simtime < fpocchtft

						//Get the necessary time window bounds to update any function
						if (nfpocchtft[i][j] < nt) nt = nfpocchtft[i][j];
						if (pfpocchtft[i][j] > pt) pt = pfpocchtft[i][j];
							
					}	//end loop over number functions for the current property

					//Store the lower time window to update functions for this property (function with lowest time break)
					ntfpocchtf[i] = nt;

				}	//end if simtime >= ntfpocchtf[i]

 				//Loop over number of functions to update interpolated value
 				//each time step using current slope and intercept
 				for(j=1; j<=nfpocchtf[i]; j++)
 				{
 					//Compute linearly interpolated values for this time step
 					fpocchtfinterp[i][j] = (float)(mfpocchtf[i][j] * (simtime - nfpocchtft[i][j]) + bfpocchtf[i][j]);
			
 				}	//end loop over number of functions

			}	//end loop over number of solids

		}	//end if fpocchopt > 0

	}	//end if chnopt > 0

//End of function: Return to UpdateTimeFunction
}
