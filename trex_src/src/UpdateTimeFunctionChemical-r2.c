/*----------------------------------------------------------------------
C-  Function:	UpdateTimeFunctionChemical.c
C-
C-	Purpose/	UpdateTimeFunctionChemical is called to interpolate
C-  Methods:    values of time-dependent functions for a given dt based
C-              on input values at specific times.  Forcing functions
C-              for overland and channel chemical loads are updated as
C-              needed for the next time step (t + dt).  Boundary
C-              conditions for chemicals are also updated.  Linear
C-              intepolation between input values is performed.  The
C-              convention used is:
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
C-	Inputs:		cwpov[][][], cwdov[][][], cwch[][][], cbc[][][],
C-              (at some time t in time series)
C-
C-	Outputs:	cwpovinterp[][][], cwdovinterp[][][], cwchinterp[][][],
C-              cbcinterp[][][]
C-              (at current simulation time)
C-
C-	Controls:	chnopt, nchems, ncwpov, ncwpovpairs, ncwdov, ncwdovpairs,
C-              ncwch, ncwchpairs, noutlets, dbcopt, ncbcpairs
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

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void UpdateTimeFunctionChemical()
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


	//Update overland chemical loads/forcing functions
	//(mass transfer and transformations etc.)
	//
	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//If it is time to update any overland point source chemical load time function (simtime >= ntcwpov[])
		if(simtime >= ntcwpov[i])
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the chemical loads are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Update overland point source chemical loads...
			//
			//Loop over number of overland loads for the current chemical
			for(j=1; j<=ncwpov[i]; j++)
			{
				//if it is time to update values for this chemical load...
				//
				//ncwpovt = next time value in overland chemical load time series
				//pcwpovt = prior overland chemical load time value in time series
				//
				if(simtime >= ncwpovt[i][j] || simtime < pcwpovt[i][j])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = cwpovtime[i][j][ncwpovpairs[i][j]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

					//Pointer to the last "current" time interval index for this 
					//chemical load.
					ip = cwpovpoint[i][j];
				
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= cwpovtime[i][j][ip+1])
					{
						ip = ip + 1;

					}	//end while
					
					while((mtime < cwpovtime[i][j][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bcwpov[i][j] = cwpov[i][j][ip+1];

					//Compute slope (m) for new interval
					mcwpov[i][j] = (cwpov[i][j][ip] - cwpov[i][j][ip+1])
						/ (cwpovtime[i][j][ip] - cwpovtime[i][j][ip+1]);

					//Number of times cycled through the chemical load time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);

					//Next time interval upper bounds to recalculate this chemical load.
					ncwpovt[i][j] = (float)(ncycle * endtime) + cwpovtime[i][j][ip+1];

					//Next time interval lower bounds to recalculate this chemical load.
					pcwpovt[i][j] = (float)(ncycle * endtime) + cwpovtime[i][j][ip];

					//Save the pointer to the current time interval for this chemical load.
					cwpovpoint[i][j] = ip;

				}	//	End if simtime >= ncwpovt or simtime < pcwpovt

				// Get the necessary time window bounds to update any chemical load.
				if (ncwpovt[i][j] < nt) nt = ncwpovt[i][j];
				if (pcwpovt[i][j] > pt) pt = pcwpovt[i][j];

			}	//end loop over number overland point source loads for current chemical

			//Store the lower time window to update chemical loads (for function with lowest time break)
			ntcwpov[i] = nt;

		}	//end if simtime >= ntcwpov[i]

 		//Loop over number of overland point source load functions to update interpolated value
 		//each time step using current slope and intercept
 		for(j=1; j<=ncwpov[i]; j++)
 		{
 			//Compute linearly interpolated external channel flows for this time step
 			cwpovinterp[i][j] = (float)(mcwpov[i][j] * (simtime - ncwpovt[i][j]) + bcwpov[i][j]);
 
 		}	//end loop over number of overland point source load functions

		//If it is time to update any overland distributed chemical load time function (simtime >= ntcwdov[])
		if(simtime >= ntcwdov[i])
		{
			//Initialize the lower and upper bounds of the _next_ time window
			//in which the chemical loads are to be updated.
			nt = 1.0e+6;	//lower bound for next time window
			pt = 0.0;		//upper bound for next time window

			//Update overland distributed chemical loads...
			//
			//Loop over number of overland loads for the current chemical
			for(j=1; j<=ncwdov[i]; j++)
			{
				//if it is time to update values for this chemical load...
				//
				//ncwdovt = next time value in overland chemical load time series
				//pcwdovt = prior overland chemical load time value in time series
				//
				if(simtime >= ncwdovt[i][j] || simtime < pcwdovt[i][j])
				{
					//If the simulation time is greater than the last time specified,
					//start again (cyclically) with the first value in the array.  So
					//work with the current time, modulo the last entry in the array.
					endtime = cwdovtime[i][j][ncwdovpairs[i][j]];

					//mtime = mod(simtime,endtime)	(original code from ipx)
					//
					//mtime is remaining part of time series
					mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

					//Pointer to the last "current" time interval index for this 
					//chemical load.
					ip = cwdovpoint[i][j];
				
					//Search upward or downward to get the index corresponding
					//to the current simulation time.
					while(mtime >= cwdovtime[i][j][ip+1])
					{
						ip = ip + 1;

					}	//end while
					
					while((mtime < cwdovtime[i][j][ip]) && (ip != 1))
					{
						ip = ip - 1;

					}	//end while

					//Linear interpolation is perfomed following the form:
					//
					//  y = m x + b
					//
					//Compute intercept (b) for new interval
					bcwdov[i][j] = cwdov[i][j][ip+1];

					//Compute slope (m) for new interval
					mcwdov[i][j] = (cwdov[i][j][ip] - cwdov[i][j][ip+1])
						/ (cwdovtime[i][j][ip] - cwdovtime[i][j][ip+1]);

					//Number of times cycled through the chemical load time interval array for the
					//current value of the simulation time.
					ncycle = (int)(simtime / endtime);

					//Next time interval upper bounds to recalculate this chemical load.
					ncwdovt[i][j] = (float)(ncycle * endtime) + cwdovtime[i][j][ip+1];

					//Next time interval lower bounds to recalculate this chemical load.
					pcwdovt[i][j] = (float)(ncycle * endtime) + cwdovtime[i][j][ip];

					//Save the pointer to the current time interval for this chemical load.
					cwdovpoint[i][j] = ip;

				}	//	End if simtime >= ncwdovt or simtime < pcwdovt

				// Get the necessary time window bounds to update any chemical load.
				if (ncwdovt[i][j] < nt) nt = ncwdovt[i][j];
				if (pcwdovt[i][j] > pt) pt = pcwdovt[i][j];

			}	//end loop over number overland loads for current chemical

			//Store the lower time window to update chemical loads (for function with lowest time break)
			ntcwdov[i] = nt;

		}	//end if simtime >= ntcwdov[i]

 		//Loop over number of overland distributed load functions to update interpolated value
 		//each time step using current slope and intercept
 		for(j=1; j<=ncwdov[i]; j++)
 		{
 			//Compute linearly interpolated external channel flows for this time step
 			cwdovinterp[i][j] = (float)(mcwdov[i][j] * (simtime - ncwdovt[i][j]) + bcwdov[i][j]);
 
 		}	//end loop over number of overland distributed load functions

	}	//end loop over chemicals

	//if channels are simulated
	if(chnopt > 0)
	{
		//Update channel chemical loads/forcing functions
		//(mass transfer and transformations etc.)
		//
		//Loop over number of chemicals
		for(i=1; i<=nchems; i++)
		{
			//If it is time to update any channel chemical load time function (simtime >= ntcwch[])
			if(simtime >= ntcwch[i])
			{
				//Initialize the lower and upper bounds of the _next_ time window
				//in which the chemical loads are to be updated.
				nt = 1.0e+6;	//lower bound for next time window
				pt = 0.0;		//upper bound for next time window

				//Update chemical loads...
				//
				//Loop over number of channel loads for the current chemical
				for(j=1; j<=ncwch[i]; j++)
				{
					//if it is time to update values for this chemical load...
					//
					//ncwcht = next time value in chemical load time series
					//pcwcht = prior chemical load time value in chemical load time series
					//
					if(simtime >= ncwcht[i][j] || simtime < pcwcht[i][j])
					{
						//If the simulation time is greater than the last time specified,
						//start again (cyclically) with the first value in the array.  So
						//work with the current time, modulo the last entry in the array.
						endtime = cwchtime[i][j][ncwchpairs[i][j]];

						//mtime = mod(simtime,endtime)	(original code from ipx)
						//
						//mtime is remaining part of time series
						mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;

						//Pointer to the last "current" time interval index for this 
						//chemical load.
						ip = cwchpoint[i][j];
					
						//Search upward or downward to get the index corresponding
						//to the current simulation time.
						while(mtime >= cwchtime[i][j][ip+1])
						{
							ip = ip + 1;

						}	//end while
						
						while((mtime < cwchtime[i][j][ip]) && (ip != 1))
						{
							ip = ip - 1;

						}	//end while

						//Linear interpolation is perfomed following the form:
						//
						//  y = m x + b
						//
						//Compute intercept (b) for new interval
						bcwch[i][j] = cwch[i][j][ip+1];

						//Compute slope (m) for new interval
						mcwch[i][j] = (cwch[i][j][ip] - cwch[i][j][ip+1])
							/ (cwchtime[i][j][ip] - cwchtime[i][j][ip+1]);

						//Number of times cycled through the chemical load time interval array for the
						//current value of the simulation time.
						ncycle = (int)(simtime / endtime);

						//Next time interval upper bounds to recalculate this chemical load.
						ncwcht[i][j] = (float)(ncycle * endtime) + cwchtime[i][j][ip+1];

						//Next time interval lower bounds to recalculate this chemical load.
						pcwcht[i][j] = (float)(ncycle * endtime) + cwchtime[i][j][ip];

						//Save the pointer to the current time interval for this chemical load.
						cwchpoint[i][j] = ip;

					}	//	End if simtime >= nswcht or simtime < pswcht

					// Get the necessary time window bounds to update any chemical load.
					if (ncwcht[i][j] < nt) nt = ncwcht[i][j];
					if (pcwcht[i][j] > pt) pt = pcwcht[i][j];

				}	//end loop over number chenncal loads for current chemical

				//Store the lower time window to update chemical loads (for function with lowest time break)
				ntcwch[i] = nt;

			}	//end if simtime >= ntcwch[i]

 			//Loop over number of channel load functions to update interpolated value
 			//each time step using current slope and intercept
 			for(j=1; j<=ncwch[i]; j++)
 			{
 				//Compute linearly interpolated external channel flows for this time step
 				cwchinterp[i][j] = (float)(mcwch[i][j] * (simtime - ncwcht[i][j]) + bcwch[i][j]);

 			}	//end loop over number of channel load functions

		}	//end loop over chemicals

	}	//end if chnopt > 0

	//Domain outlets/boundaries...
	//
	//Initialize the lower and upper bounds of the _next_ time window
	//in which the outlet water depths are to be updated.
	nt = 1.0e+6;	//lower bound for next time window
	pt = 0.0;		//upper bound for next time window

	//Loop over number of outlet water depth functions
	for(i=1; i<=noutlets; i++)
	{
		//if boundary conditions are specified for this outlet (dbcopt[] > 0)
		if(dbcopt[i] > 0)
		{
			//If it is time to update any boundary chemical concentration time function (simtime >= ntcbc[])
			if(simtime >= ntcbc[i])
			{
				//Update chemical BCs...
				//
				//Loop over number of chemicals
				for(j=1; j<=nchems; j++)
				{
					//if it is time to update values for this chemical BC...
					//
					//ncbct = next time value in chemical BC time series
					//pcbct = prior chemical BC time value in chemical BC time series
					//
					if(simtime >= ncbct[i][j] || simtime < pcbct[i][j])
					{
						//If the simulation time is greater than the last time specified,
						//start again (cyclically) with the first value in the array.  So
						//work with the current time, modulo the last entry in the array.
						endtime = cbctime[i][j][ncbcpairs[i][j]];

						//mtime = mod(simtime,endtime)	(original code from ipx)
						//
						//mtime is remaining part of time series
						mtime = (double)((simtime / endtime) - (int)(simtime / endtime)) * endtime;
					
						//Pointer to the last "current" time interval index for this 
						//chemical BC.
						ip = cbcpoint[i][j];
					
						//Search upward or downward to get the index corresponding
						//to the current simulation time.
						while(mtime >= cbctime[i][j][ip+1])
						{
							ip = ip + 1;

						}	//end while
						
						while((mtime < cbctime[i][j][ip]) && (ip != 1))
						{
							ip = ip - 1;

						}	//end while

						//Linear interpolation is perfomed following the form:
						//
						//  y = m x + b
						//
						//Compute intercept (b) for new interval
						bcbc[i][j] = cbc[i][j][ip+1];

						//Compute slope (m) for new interval
						mcbc[i][j] = (cbc[i][j][ip] - cbc[i][j][ip+1])
							/ (cbctime[i][j][ip] - cbctime[i][j][ip+1]);

						//Number of times cycled through the chemical BC time interval array for the
						//current value of the simulation time.
						ncycle = (int)(simtime / endtime);
					
						//Next time interval upper bounds to recalculate this chemical BC.
						ncbct[i][j] = (float)(ncycle * endtime) + cbctime[i][j][ip+1];
					
						//Next time interval lower bounds to recalculate this chemical BC.
						pcbct[i][j] = (float)(ncycle * endtime) + cbctime[i][j][ip];
					
						//Save the pointer to the current time interval for this chemical BC.
						cbcpoint[i][j] = ip;
					
					}	//	End if simtime >= ncbct or simtime < pcbct

					// Get the necessary time window bounds to update any chemical BC.
					if (ncbct[i][j] < nt) nt = ncbct[i][j];
					if (pcbct[i][j] > pt) pt = pcbct[i][j];

				}	//end loop over number of chemicals

				//Store the lower time window to update chemical boundary conditions (for function with lowest time break)
				ntcbc[i] = nt;

			}	//end if simtime >= ntcbc[i]

 			//Loop over number of solids types to update interpolated value
 			//each time step using current slope and intercept
 			for(j=1; j<=nsolids; j++)
 			{
 				//Compute linearly interpolated outlet boundary concentration for this time step
 				cbcinterp[i][j] = (float)(mcbc[i][j] * (simtime - ncbct[i][j]) + bcbc[i][j]);
	 
 			}	//end loop over solids types

		}	//end if dbcopt[i] > 0

	}	//end loop over outlets

//End of function: Return to UpdateTimeFunction
}
