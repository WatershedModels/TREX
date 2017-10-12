/*---------------------------------------------------------------------
C-  Function:	WriteTimeSeriesWater.c                                              
C-
C-	Purpose/    The module writes times series reports of water export
C-	Methods:    (discharge in m3/s or mm/hr) at each specified location
C-              (reporting stations) for the current time t.
C-
C-	Inputs:		qreport
C-
C-	Outputs:	
C-
C-	Controls:   ksim, nqreports, qconvert
C-
C-	Calls:		None
C-
C-	Called by:	WriteTimeSeries
C-
C-	Created:	Rosalia Rojas-Sanchez
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (last version)
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux (CSU)
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		20-MAR-2004
C-
C-	Revisions:	Documentation, reorganization, complete rewrite
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		31-JUL-2009
C-
C-  Revisions:	Added code so that errors caused by bad file names
C-              and paths are are reported to the simulation error
C-              file in addition to the screen.
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void WriteTimeSeriesWater()
{
	//If user requested reports for discharges
	if(nqreports > 0)
	{
		//Set the water export file pointer to null
		waterexpfile_fp = NULL;

		//open the water export file for writing in append mode
		waterexpfile_fp = fopen(waterexpfile, "a");

		//if the water export file pointer is null, abort execution
		if(waterexpfile_fp == NULL)
		{
			//Write message to file
			fprintf(echofile_fp, "Error! Can't open Water Export File:  %s\n", waterexpfile);

			//Write message to screen
			printf("Error! Can't open Water Export File:  %s\n", waterexpfile);
			exit(EXIT_FAILURE);	//abort
		}

		//Write discharges at selected locations
		//and print out in comma-delimited format
		//
		//Write current simulation time (hours)
		fprintf(waterexpfile_fp,"%f",simtime);

		//Loop over number of discharge reporting stations
		for(i=1; i<=nqreports; i++)
		{
			//multiply output overland discharge by conversion factor
			//and print to file
			fprintf(waterexpfile_fp,",%f", qreportov[i]*qconvert[i]);

			//multiply output channel discharge by conversion factor
			//and print to file
			fprintf(waterexpfile_fp,",%f", qreportch[i]*qconvert[i]);

//mlv: delete the qreportsum or modify this code because the sum only
//     makes sense when the reporting station is at an outlet...

			//Compute sum of overland and channel discharge (m3/s)
			qreportsum[i] = qreportov[i] + qreportch[i];

			//multiply output sum of discharge by conversion factor
			//and print to file
			fprintf(waterexpfile_fp,",%f", qreportsum[i]*qconvert[i]);

		}	//end loop over	number of reporting	stations

		//print a carriage return after each complete line
		fprintf(waterexpfile_fp,"\n");

		//Close water export file
		fclose(waterexpfile_fp);

	}	//end if nqreports > 0

//End of function: Return to WriteTimeSeries
}
