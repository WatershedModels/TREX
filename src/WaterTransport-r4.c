/*----------------------------------------------------------------------
C-  Function:	WaterTransport
C-
C-  Purpose/    WaterTransport is called to compute derivative
C-  Methods:    terms (rates) for water transport processes:
C-              rainfall, snowmelt, interception, infiltration,
C-              overland flow/routing, and channel flow/routing.
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   infopt, snowopt, meltopt, chnopt, ctlopt, dtopt
C-
C-  Calls:      Rainfall,
C-              Interception,
C-              Infiltration,
C-              OverlandWaterRoute,
C-              TransmissionLoss
C-              ChannelWaterRoute,
C-              FloodplainWaterTransfer
C-
C-  Called by:  trex (main)
C-
C-  Created:    Mark Velleux (CSU)
C-              John England (USBR)
C-
C-  Date:       19-AUG-2003
C-
C-  Revised:	Mark Velleux (CSU)
C-
C-	Date:		05-MAY-2005
C-
C-	Revisions:	Added call for floodplain water transfer process
C-              module (FloodplainWaterTransfer)
C-
C-  Revised:	Mark Velleux (CSU)
C-
C-	Date:		03-JUN-2005
C-
C-	Revisions:	Added call for channel transmission loss process
C-              module (TransmissionLoss)
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		20-DEC-2006
C-
C-  Revisions:	Added calls for snow hydrology process modules
C-              (Snowfall, Snowmelt)
C-
C-  Revised:	Masa Takamatsu
C-              Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		29-JUL-2010
C-
C-  Revisions:	Added logic to calculate iteration success for
C-              time step option (dtopt) implementation
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C-----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void WaterTransport()
{
	//Local variable declarations/definitions
	int
		successflag;	//flag to signal if numerical integration was successful (success = 0)

	float
		courant;		//controlling courant number for model domain (dimensionless)

/*
	float
		dtmax;			//maximum time step value  (s) --> temp!

	dtmax = (float)(120.0);
*/

	//set successflag to failure (failure is any value other than zero)
	successflag = 1;

	//while successflag is not zero (iteration failure)
	while(successflag != 0)
	{
		//Note: If snowopt = 0, then all precipitation is rainfall.
		//      if snowopt = 1, then rainfall could be rain or snow
		//      depending on the air temperature in each cell.
		//
		//compute gross rainfall rate (intensity)
		Rainfall();

		//Note: If snowopt > 1, then snowfall is simulated from snow
		//      gage data or as "snow on the ground" from snow depth
		//      data.  These options do not depend on air temperature.
		//
		//if snowfall is simulated from snow gage data
		if(snowopt > 1)
		{
			//compute snowfall rate (intensity) (as swe)
			Snowfall();
		}

		//Compute interception and net rainfall/snowfall rate
		Interception();
/*
		//compute rainfall interception and net rainfall rate
		RainInterception();

		//if snowfall is simulated as falling precipitation
		if(snowopt > 0 && snowopt <= 3)
		{
			//compute snowfall interception and net snowfall rate
			SnowInterception();
		}
*/
		//if snowmelt is simulated
		if(meltopt > 0)
		{
			//compute snowmelt rate (expressed as equivalent rate of water addition)
			Snowmelt();
		}

		//if infiltration is simulated
		if(infopt > 0)
		{
			//compute infiltration rate (average over integration time step)
			Infiltration();
		}
		
		//compute overland flows and routing
		OverlandWaterRoute();

		//if channels are simulated
		if(chnopt > 0)
		{
			//if channel transmission loss is simulated
			if(ctlopt > 0)
			{
				//compute channel transmission loss rate
				TransmissionLoss();

			}	//end if ctlopt > 0

			//compute channel flows and routing
			ChannelWaterRoute();

			//compute water transfers between overland plane and channels
			FloodplainWaterTransfer();
		}

		//if automated time-stepping is selected (dtopt = 1 or dtopt = 2)
		if(dtopt == 1 || dtopt == 2)
		{
			//Set maximum controlling Courant number...
			//
			//Note:  The controlling Courant number for the entire model
			//       domain is the larger of overland or channel values.
			//
			//controlling courant number (dimensionless)
			courant = (float)(Max(maxcourantov,maxcourantch));

			//if the domain courant number exceeds the maximum
			if(courant > maxcourant)
			{
				//Iteration Failure: dt too large
				//
				//set the success flag to indicate large dt failure
				successflag = successflag + 1;

				//calculate new time step...
				//
				//Note:  A relaxation factor is used to help obtain
				//       acceptable dt values faster and reduce the
				//       number of iterations for "convergence".
				//
				//calculate new (smaller) time step (seconds)
				dt[idt] = (float)(dt[idt] * relaxation * maxcourant / courant);

				//truncate time steps to a fixed number of significant digits
				TruncateDT();
			}
			else	//else (courant <= maxcourant)
			{
				//Note:  The code does not check if the Courant number
				//       is less than a minimum value because resetting
				//       dt to a larger value would cause dt to exceed
				//       the maximum dt value entered in Data Group A.
				//
				//set the success flag to indicate success
				successflag = 0;

				//if present time step value differs from prior value
				//  and simulation time is greater than start time
//				if(dt[idt] != dt[idt-1] && simtime > tstart)
				if(dt[idt] != dt[idt-1])
				{
					//Set time for saved time step...
					//
					//Developer's Note:  Further development may be needed to address loss
					//                   of precision because dttime is float and simtime
					//                   is double.
					//
					//Note:  Values for dttime value are stored in the [idt-1]
					//       element of the array because they indicate the end
					//       time for use of the prior dt value in the series.
					//
					//set time for successful time step (hrs)
					dttime[idt-1] = (float)(simtime);

					//increment total number of time steps in series
					ndt = ndt + 1;

					//increment timestep buffer index
					bdt = bdt + 1;

					//Note:  MAXBUFFER controls the size of the array used to
					//       store dt[] and dttime[] value pairs.  When all
					//       array elements are full, the buffer is written
					//       to file and cleared to store any further values.
					//
					//if buffer for stored time step values is full
					if(bdt == MAXBUFFERSIZE)
					{
						//write time step series (dt, dttime pairs) to buffer file
						WriteDTBuffer();

						//reset buffer counter (bdt) to zero (flush the buffer)
						bdt = 0;

						//move dt to the start of the array...
						//
						//Note:  Mass balance calculations depend on dt[idt].
						//       We need to flush the buffer and set idt to
						//       zero.  Because idt will equal zero, the value
						//       of dt stored in the MAXBUFFER array element
						//       must be moved to the zeroeth array element
						//       before mass balance calculations occur.
						//
						//store dt to the head of the array, dt[1]
						dt[0] = dt[idt];

						//reset the count of timesteps
						idt = 0;

					}	//end if bdt = maxbuffersize

				}	//end if dt[idt] not equal to dt[idt-1]

			}	//end if courant > maxcourant etc...
		}
		else	//else dtopt is not 1 or 2
		{
			//set the success flag to zero (successful iteration)
			successflag = 0;

		}	//end if dtopt = 1 or 2

	}	//end loop for numerical integration failure/success

//End of function: Return to trex
}
