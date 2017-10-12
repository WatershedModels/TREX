/*---------------------------------------------------------------------
C-  Function:	TimeFunctionInitWater.c
C-		
C-	Purpose/	TimeFunctionInitWater is called at the start of the
C-  Methods:	simulation to set the starting values of parameters
C-				used for control general and water transport time
C-				series functions.
C-
C-  Inputs:		simtime, nrfpairs[], nsfpairs[], nqwovpairs[],
C-				nqwchpairs[], nqbcpairs[]
C-
C-  Outputs:    idt, pdt, gdt, rfpoint[], sfpoint[], qwovpoint[],
C-              qwchpoint[], hbcpoint[]
C-
C-  Controls:   chnopt, dbcopt[]
C-
C-  Calls:      None
C-
C-  Called by:  TimeFunctionInit
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
C-	Date:		23-OCT-2003
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

void TimeFunctionInitWater()
{
	//local variable declarations/definitions
	int 
		ip;		//pointer to current position in time series

	//Initialize indices for time step and print intervals
	//
	//Set the starting value of the time step index
	idt = 1;

	//while the simulation time is greater than the ending
	//  time of the current time step AND the time step
	//  index is less than the number of index values input
	while((simtime > dttime[idt]) && (idt < ndt))
	{
		//increment the time step index
		idt = idt + 1;

	}	//end while simtime < dttime...

	//Set the time series print interval index
	pdt = 1;

	//while the simulation time is greater than the ending
	//  time of the current time series print interval AND
	//  the time step index is less than the number of index
	//  values input
	while((simtime > printouttime[pdt]) && (pdt < nprintout))
	{
		//increment the time series print interval index
		pdt = pdt + 1;

	}	//end while simtime > printouttime...

	//Set the grid print interval index
	gdt = 1;

	//while the simulation time is greater than the ending
	//  time of the current grid print interval AND the time
	//  step index is less than the number of index values
	//  input
	while((simtime > printgridtime[gdt]) && (gdt < nprintgrid))
	{
		//increment the grid print interval index
		gdt = gdt + 1;

	}	//end while simtime > printgridtime...

	//Initialize indices for rainfall time series
	//
	//set the initial rainfall function update time to the simulation start time
	ntr = (float)(tstart);

	//Loop over number of rain gages (rainfall functions)
	for(i=1; i<=nrg; i++)
	{
		//Set the time series position index
		ip = 1;

		//while the simulation time is greater than the ending
		//  time of the current rainfall time AND the time
		//  series index is less than the number of pairs input
		while((simtime > rftime[i][ip]) && (ip < nrpairs[i]))
		{
			//increment the time series position index
			ip = ip + 1;

		}	//end while

		//Pointer to the time series position index for this
		//rainfall intensity time series corrsesponding to simtime
		rfpoint[i] = ip;

		//Set the next (upper) time for the function update window
		nrft[i] = (float) simtime;

		//Set the prior (lower) time for the function update window
		prft[i] = (float) simtime;

	}	//end loop over rain gages

	//if snowfall via gages is simulated (snowopt == 2)
	if(snowopt == 2)
	{
		//Initialize indices for snowfall time series
		//
		//set the initial snowfall function update time to the simulation start time
		nts = (float)(tstart);

		//Loop over number of snow gages (snowfall functions)
		for(i=1; i<=nsg; i++)
		{
			//Set the time series position index
			ip = 1;

			//while the simulation time is greater than the ending
			//  time of the current snowfall time AND the time
			//  series index is less than the number of pairs input
			while((simtime > sftime[i][ip]) && (ip < nspairs[i]))
			{
				//increment the time series position index
				ip = ip + 1;

			}	//end while

			//Pointer to the time series position index for this
			//snowfall intensity time series corrsesponding to simtime
			sfpoint[i] = ip;

			//Set the next (upper) time for the function update window
			nsft[i] = (float) simtime;

			//Set the prior (lower) time for the function update window
			psft[i] = (float) simtime;

		}	//end loop over snow gages

	}	//end if snowopt == 2

	//Initialize indices for overland flow (point source) time series
	//
	//set the initial overland point source function update time to the simulation start time
	ntqwov = (float)(tstart);

	//Loop over number of overland flow sources
	for(i=1; i<=nqwov; i++)
	{
		//Set the time series position index
		ip = 1;

		//while the simulation time is greater than the ending
		//  time of the current point source time AND the time
		//  series index is less than the number of pairs input
		while((simtime > qwovtime[i][ip]) && (ip < nqwovpairs[i]))
		{
			//increment the time series position index
			ip = ip + 1;

		}	//end while

		//Pointer to the time series position index for this
		//overland flow time series corrsesponding to simtime
		qwovpoint[i] = ip;

		//Set the next (upper) time for the function update window
		nqwovt[i] = (float) simtime;

		//Set the prior (lower) time for the function update window
		pqwovt[i] = (float) simtime;

	}	//end loop over overland flow sources

	//if channels are simulated
	if(chnopt > 0)
	{
		//Initialize indices for flow time series
		//
		//set the initial channel point source function update time to the simulation start time
		ntqwch = (float)(tstart);

		//Loop over number of flow functions
		for(i=1; i<=nqwch; i++)
		{
			//Set the time series position index
			ip = 1;

			//while the simulation time is greater than the ending
			//  time of the current flow time AND the time series
			//  index is less than the number of pairs input
			while((simtime > qwchtime[i][ip]) && (ip < nqwchpairs[i]))
			{
				//increment the time series position index
				ip = ip + 1;

			}	//end while

			//Pointer to the time series position index for this
			//external flow time series corrsesponding to simtime
			qwchpoint[i] = ip;

			//Set the next (upper) time for the function update window
			nqwcht[i] = (float) simtime;

			//Set the prior (lower) time for the function update window
			pqwcht[i] = (float) simtime;

		}	//end loop over number of flow functions

	}	//end if chnopt > 0

	//Initialize indices for outlet boundary condition (water depth) time series
	//
	//set the initial outlet boundary condition function update time to the simulation start time
	nthbc = (float)(tstart);

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//if a time series is specified for this outlet
		if(dbcopt[i] > 0)
		{
			//Set the time series position index
			ip = 1;

			//while the simulation time is greater than the ending
			//  time of the current boudary depth time AND the time
			//  series index is less than the number of pairs input
			while((simtime > hbctime[i][ip]) && (ip < nhbcpairs[i]))
			{
				//increment the time series position index
				ip = ip + 1;

			}	//end while

			//Pointer to the time series position index for this
			//water depth time series corrsesponding to simtime
			hbcpoint[i] = ip;

			//Set the next (upper) time for the function update window
			nhbct[i] = (float) simtime;

			//Set the prior (lower) time for the function update window
			phbct[i] = (float) simtime;

		}	//end if dbcopt[] > 0

	}	//end loop over outlet boundary conditions (water depths)

//End of function: Return to TimeFunctionInit
}
