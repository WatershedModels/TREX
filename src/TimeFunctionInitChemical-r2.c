/*---------------------------------------------------------------------
C-  Function:	TimeFunctionInitChemical.c
C-
C-	Purpose/	TimeFunctionInitChemical is called at the start of the
C-  Methods:	simulation to set the starting values of parameters
C-				used for chemical transport time series functions.
C-
C-  Inputs:	    simtime, cwpovtime[][], ncwpovpairs[][], cwdovtime[][],
C-              ncwdovpairs[][], cwchtime[][], ncwchpairs[][], 
C-              cbctime[][], ncbcpairs[][]
C-
C-  Outputs:    cwpovpoint[], cwdovpoint[], cwchpoint[][], cbcpoint[][]
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
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void TimeFunctionInitChemical()
{
	//local variable declarations/definitions
	int 
		ip;		//pointer to current position in time series

	//Initialize indices for overland chemical load time series
	//
	//loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//set the initial overland point source chemical load function update time to the simulation start time
		ntcwpov[i] = (float)(tstart);

		//Loop over number of overland point source loads for current chemicals
		for(j=1; j<=ncwpov[i]; j++)
		{
			//Set the time series position index
			ip = 1;

			//while the simulation time is greater than the ending
			//  time of the current chemical load AND the time series
			//  index is less than the number of pairs input
			while((simtime > cwpovtime[i][j][ip]) && (ip < ncwpovpairs[i][j]))
			{
				//increment the time series position index
				ip = ip + 1;

			}	//end while

			//Pointer to the time series position index for this
			//sediment load time series corrsesponding to simtime
			cwpovpoint[i][j] = ip;

			//Set the next (upper) time for the function update window
			ncwpovt[i][j] = (float) simtime;

			//Set the prior (lower) time for the function update window
			pcwpovt[i][j] = (float) simtime;

		}	//end loop over number of point source loads for current chemical

		//set the initial overland distributed chemical load function update time to the simulation start time
		ntcwdov[i] = (float)(tstart);

		//Loop over number of overland distributed loads for current chemicals
		for(j=1; j<=ncwdov[i]; j++)
		{
			//Set the time series position index
			ip = 1;

			//while the simulation time is greater than the ending
			//  time of the current chemical load AND the time series
			//  index is less than the number of pairs input
			while((simtime > cwdovtime[i][j][ip]) && (ip < ncwdovpairs[i][j]))
			{
				//increment the time series position index
				ip = ip + 1;

			}	//end while

			//Pointer to the time series position index for this
			//sediment load time series corrsesponding to simtime
			cwdovpoint[i][j] = ip;

			//Set the next (upper) time for the function update window
			ncwdovt[i][j] = (float) simtime;

			//Set the prior (lower) time for the function update window
			pcwdovt[i][j] = (float) simtime;

		}	//end loop over number of distributed loads for current chemical

	}	//end loop over chemicals

	//if channels are simulated
	if(chnopt > 0)
	{
		//Initialize indices for channel chemical load time series
		//
		//loop over number of chemicals
		for(i=1; i<=nchems; i++)
		{
			//set the initial channel chemical load function update time to the simulation start time
			ntcwch[i] = (float)(tstart);

			//Loop over number of channel loads for current chemicals
			for(j=1; j<=ncwch[i]; j++)
			{
				//Set the time series position index
				ip = 1;

				//while the simulation time is greater than the ending
				//  time of the current chemical load AND the time series
				//  index is less than the number of pairs input
				while((simtime > cwchtime[i][j][ip]) && (ip < ncwchpairs[i][j]))
				{
					//increment the time series position index
					ip = ip + 1;

				}	//end while

				//Pointer to the time series position index for this
				//sediment load time series corrsesponding to simtime
				cwchpoint[i][j] = ip;

				//Set the next (upper) time for the function update window
				ncwcht[i][j] = (float) simtime;

				//Set the prior (lower) time for the function update window
				pcwcht[i][j] = (float) simtime;

			}	//end loop over number of loads for current chemical

		}	//end loop over chemicals

	}	//end if chnopt > 0

	//Initialize indices for outlet chemical boundary condition time series
	//
	//loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//if a time series is specified for this outlet
		if(dbcopt[i] > 0)
		{
			//Loop over number of chemicals
			for(j=1; j<=nchems; j++)
			{
				//set the initial chemical boundary condition update time to the simulation start time
				ntcbc[j] = (float)(tstart);

				//Set the time series position index
				ip = 1;

				//while the simulation time is greater than the ending
				//  time of the current chemical bc AND the time series
				//  index is less than the number of pairs input
				while((simtime > cbctime[i][j][ip]) && (ip < ncbcpairs[i][j]))
				{
					//increment the time series position index
					ip = ip + 1;
				}	//end while

				//Pointer to the time series position index for this
				//sediment load time series corrsesponding to simtime
				cbcpoint[i][j] = ip;
		
				//Set the next (upper) time for the function update window
				ncbct[i][j] = (float) simtime;

				//Set the prior (lower) time for the function update window
				pcbct[i][j] = (float) simtime;

			}	//end loop over number chemicals

		}	//end if dbcopt[] > 0

	}	//end loop over outlets

//End of function: Return to TimeFunctionInit
}
