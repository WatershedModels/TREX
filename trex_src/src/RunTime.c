/*----------------------------------------------------------------------
C-  Function:   RunTime.c
C-
C-  Purpose/
C-  Methods:    Computes elapsed cpu time for a simulation run.
C-
C-
C-  Inputs:     clockstart (time in seconds at simulation start)
C-              clockstop (time in seconds at simulation end)
C-
C-  Outputs:    elapsedtime (local)
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  trex.c
C-
C-
C-  Created:    Rosalia Rojas-Sanchez
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO 80523
C-
C-  Date:       19-JUN-2003 (date assumed)
C-
C-  Revised:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-				
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-  Date:       13-JUN-2005
C-
C-  Revisions:  Documentation, streamlining, revised function call.
C-
C-  Date:
C-
C-  Revisions:
C-
C---------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

void RunTime(time_t clockstart, time_t clockstop)
{
	//local variable declarations/definitions
	double 
		elapsedtime;  //elapsed CPU time for the simulation (seconds)

	//Compute elapsed cpu time...
	//
	//  timestart and timestop are reported in units of seconds
	//  this is via function time(.).
	//  The arithmetic data type is time_t (see K&R C p. 255-256).
	//  The total run time is the difference between the starting and ending times
	//  computed using the standard C intrinsic function difftime.
	//
	//  difftime(time_t time2, time_t time1) returns time2-time1 expressed in seconds
	//

	//elapsed time of simulation (seconds)
	elapsedtime = (double)(difftime(clockstop, clockstart));

	//Send the simulation run time to both echo file and
	//summary statistics files

	//set the echo file pointer to null
	echofile_fp = NULL;

	//open the echo file (in append mode)
	echofile_fp = fopen(echofile, "a");

	//if echo file pointer is still null, then abort
	if(echofile_fp == NULL)
	{
		//write error message to screen
		printf("Error! Can't open Echo file %s\n", echofile);
		exit(EXIT_FAILURE);	//exit with error
	}

	//set the summary statistics file pointer to null
	statsfile_fp = NULL;

	//open the summary statistics file (in append mode)
	statsfile_fp = fopen(statsfile, "a");

	//if file pointer is still null, then abort
	if(statsfile_fp == NULL)
	{
		//write error message to screen
		printf("Error! Can't open Summary Stats file %s\n", statsfile);
		exit(EXIT_FAILURE);	//exit with error
	}

	//Print the performance summary label to echo file
	fprintf(echofile_fp,
		"\n\n\nRuntime Performance Summary\n");
	fprintf(echofile_fp,
		"~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Print the simulation end time (in hours) to echo file
	fprintf(echofile_fp, "\nTREX model run successfully completed! ");
	fprintf(echofile_fp,
		"\nProgram stopped at simulation hour:%12.4f\n",simtime);

	//Print the performance summary label to summary stats file
	fprintf(statsfile_fp,
		"\n\n\nRuntime Performance Summary\n");
	fprintf(statsfile_fp,
		"~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Print the simulation end time (in hours) to summary statistics file
	fprintf(statsfile_fp, "\nTREX model run successfully completed! ");
	fprintf(statsfile_fp,
		"\nProgram stopped at simulation hour:%12.4f\n",simtime);

	//if the elasped time < 60 seconds
	if(elapsedtime < 60.0)	//print run time in seconds
	{
		//Print the elapsed run time in seconds to echo file
		fprintf(echofile_fp, 
			"\nTREX Run Time:%10.2f seconds\n", elapsedtime);

		//Print the elapsed run time in seconds to summary statistics file
		fprintf(statsfile_fp, 
			"\nTREX Run Time:%10.2f seconds\n", elapsedtime);
	}
	//else if the elapsed time >= 60 seconds and < 3600 seconds (1 hour)
	else if((elapsedtime >= 60.0) && (elapsedtime < 3600.0))
	{ 
		//Print the elapsed run time in minutes to echo file
		fprintf(echofile_fp, 
			"\nTREX Run Time:%10.2f minutes\n", elapsedtime/60.0);

		//Print the elapsed run time in minutes to summary statistics file
		fprintf(statsfile_fp, 
			"\nTREX Run Time:%10.2f minutes\n", elapsedtime/60.0);
	}
	else	//else the elapsed time is more than 3600 seconds (1 hour)
	{
		//Print the elapsed run time in hours to echo file
		fprintf(echofile_fp, 
			"\nTREX Run Time:%10.2f hours\n", elapsedtime/3600.0);

		//Print the elapsed run time in hours to summary statistics file
		fprintf(statsfile_fp, 
			"\nTREX Run Time:%10.2f hours\n", elapsedtime/3600.0);

	}	//end if the elasped time >= 60 seconds

	//close the echo file
	fclose(echofile_fp);

	//close the summary stats file
	fclose(statsfile_fp);

//End of Function: Return to trex
}
