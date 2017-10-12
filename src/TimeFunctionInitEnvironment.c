/*---------------------------------------------------------------------
C-  Function:	TimeFunctionInitEnvironment.c
C-
C-	Purpose/	TimeFunctionInitEnvironment is called at the start of
C-  Methods:	the simulation to set the starting values of parameters
C-				used for environmental property time series functions.
C-
C-  Inputs:	    simtime, envgtftime[][][], nenvgtfpairs[][],
C-              envovtftime[][][], nenvovtfpairs[][],
C-              envchtftime[][][], nenvcgtfpairs[][]
C-
C-  Outputs:    envgtfpoint[], envovtfpoint[][], envchtfpoint[][]
C-
C-  Controls:   npropg, npropov, npropch, chnopt, fpocovopt, fpocchopt
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
C-	Date:		07-SEP-2004
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

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void TimeFunctionInitEnvironment()
{
	//local variable declarations/definitions
	int 
		ip;			//pointer to current position in time series

	//Initialize indices for general environmental property time series
	//
	//loop over number of general properties
	for(i=1; i<=npropg; i++)
	{
		//set the initial function update time to the simulation start time
		ntenvgtf[i] = (float)(tstart);

		//Loop over number of functions for the current property
		for(j=1; j<=nenvgtf[i]; j++)
		{
			//Set the time series position index
			ip = 1;

			//while the simulation time is greater than the ending
			//  time of the current solids load AND the time series
			//  index is less than the number of pairs input
			while((simtime > envgtftime[i][j][ip]) && (ip < nenvgtfpairs[i][j]))
			{
				//increment the time series position index
				ip = ip + 1;

			}	//end while

			//Pointer to the time series position index for this
			//sediment load time series corrsesponding to simtime
			envgtfpoint[i][j] = ip;

			//Set the next (upper) time for the function update window
			nenvgtft[i][j] = (float) simtime;

			//Set the prior (lower) time for the function update window
			penvgtft[i][j] = (float) simtime;

		}	//end loop over number of functions for current property

	}	//end loop over number of general properties

	//Initialize indices for overland environmental property time series
	//
	//loop over number of overland functions
	for(i=1; i<=npropov; i++)
	{
		//set the initial function update time to the simulation start time
		ntenvovtf[i] = (float)(tstart);

		//Loop over number of functions for the current property
		for(j=1; j<=nenvovtf[i]; j++)
		{
			//Set the time series position index
			ip = 1;

			//while the simulation time is greater than the ending
			//  time of the current solids load AND the time series
			//  index is less than the number of pairs input
			while((simtime > envovtftime[i][j][ip]) && (ip < nenvovtfpairs[i][j]))
			{
				//increment the time series position index
				ip = ip + 1;

			}	//end while

			//Pointer to the time series position index for this
			//sediment load time series corrsesponding to simtime
			envovtfpoint[i][j] = ip;

			//Set the next (upper) time for the function update window
			nenvovtft[i][j] = (float) simtime;

			//Set the prior (lower) time for the function update window
			penvovtft[i][j] = (float) simtime;

		}	//end loop over number of functions for current property

	}	//end loop over number of overland properties

	//if channels are simulated
	if(chnopt > 0)
	{
		//loop over number of channel functions
		for(i=1; i<=npropch; i++)
		{
			//set the initial function update time to the simulation start time
			ntenvchtf[i] = (float)(tstart);

			//Loop over number of functions for the current property
			for(j=1; j<=nenvchtf[i]; j++)
			{
				//Set the time series position index
				ip = 1;

				//while the simulation time is greater than the ending
				//  time of the current solids load AND the time series
				//  index is less than the number of pairs input
				while((simtime > envchtftime[i][j][ip]) && (ip < nenvchtfpairs[i][j]))
				{
					//increment the time series position index
					ip = ip + 1;

				}	//end while

				//Pointer to the time series position index for this
				//sediment load time series corrsesponding to simtime
				envchtfpoint[i][j] = ip;

				//Set the next (upper) time for the function update window
				nenvchtft[i][j] = (float) simtime;

				//Set the prior (lower) time for the function update window
				penvchtft[i][j] = (float) simtime;

			}	//end loop over number of functions for the current propery

		}	//end loop over number of channel properties

	}	//end if chnopt > 0

	//if overland particle fpoc is specified (fpocovopt > 0)
	if(fpocovopt > 0)
	{
		//Initialize indices for overland particle time series
		//
		//loop over number of solids
		for(i=1; i<=nsolids; i++)
		{
			//set the initial overland particle function update time to the simulation start time
			ntfpocovtf[i] = (float)(tstart);

			//Loop over number of functions for current solids type
			for(j=1; j<=nfpocovtf[i]; j++)
			{
				//Set the time series position index
				ip = 1;

				//while the simulation time is greater than the ending
				//  time of the current solids load AND the time series
				//  index is less than the number of pairs input
				while((simtime > fpocovtftime[i][j][ip]) && (ip < nfpocovtfpairs[i][j]))
				{
					//increment the time series position index
					ip = ip + 1;

				}	//end while

				//Pointer to the time series position index for this
				//sediment load time series corrsesponding to simtime
				fpocovtfpoint[i][j] = ip;

				//Set the next (upper) time for the function update window
				nfpocovtft[i][j] = (float) simtime;

				//Set the prior (lower) time for the function update window
				pfpocovtft[i][j] = (float) simtime;

			}	//end loop over number of functions for current solid

		}	//end loop over number of solids

	}	//end if fpocovopt > 0

	//if channels are simulated
	if(chnopt > 0)
	{
		//if channel particle fpoc is specified (fpocchopt > 0)
		if(fpocchopt > 0)
		{
			//loop over number of solids
			for(i=1; i<=nsolids; i++)
			{
				//set the initial channel particle function update time to the simulation start time
				ntfpocchtf[i] = (float)(tstart);

				//Loop over number of functions for current solids type
				for(j=1; j<=nfpocchtf[i]; j++)
				{
					//Set the time series position index
					ip = 1;

					//while the simulation time is greater than the ending
					//  time of the current solids load AND the time series
					//  index is less than the number of pairs input
					while((simtime > fpocchtftime[i][j][ip]) && (ip < nfpocchtfpairs[i][j]))
					{
						//increment the time series position index
						ip = ip + 1;

					}	//end while

					//Pointer to the time series position index for this
					//sediment load time series corrsesponding to simtime
					fpocchtfpoint[i][j] = ip;

					//Set the next (upper) time for the function update window
					nfpocchtft[i][j] = (float) simtime;

					//Set the prior (lower) time for the function update window
					pfpocchtft[i][j] = (float) simtime;

				}	//end loop over number of functions for the current solid

			}	//end loop over number of solids

		}	//end if fpocchopt > 0

	}	//end if chnopt > 0

//End of function: Return to TimeFunctionInit
}
