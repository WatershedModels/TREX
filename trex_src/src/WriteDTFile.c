/*---------------------------------------------------------------------
C-  Function:   WriteDTFile.c
C-
C-  Purpose\    WriteDTFile is called at the end of the simulation to
C-  Methods:    write the sequence of time step (dt) and time (dttime)
C-              value pairs that control numerical integration to an
C-              external file.
C-
C-  Inputs:     header, ndt, dt[], dttime[]
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  trex
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-				
C-	Date:		29-JUL-2010
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

void WriteDTFile()
{
	//write message to screen
	printf("\n\n*********************************************\n");
	printf(	   "*                                           *\n");
	printf(	   "*   Writing External Time Step Output File  *\n");
	printf(	   "*                                           *\n");
	printf(	   "*********************************************\n\n\n");

	//Set the time step output file pointer to null
	dtoutputfile_fp = NULL;

	//Open the time step output file for writing
	dtoutputfile_fp=fopen(dtoutputfile,"w");

	//if the time step output file pointer is null, abort execution
	if(dtoutputfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open External Time Step Output File (dtoutputfile) : %s \n",dtoutputfile);

		//Write message to screen
		printf("Error! Can't open External Time Step Output File (dtoutputfile) : %s \n",dtoutputfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Recreate dt and dttime arrays to store full sequence
	//of paired values for numerical integration control...
	//
	//Deallocate (free) memory for dt and dttime values
	free(dt);
	free(dttime);

	//Allocate memory for for the full set of dt and dttime values
	dt = (float *)malloc((ndt+1) * sizeof(float));		//allocate memory
	dttime = (float *)malloc((ndt+1) * sizeof(float));	//allocate memory

	//Read dt, dttime pairs from temporary buffer file...
	//
	//write message to screen
	printf("\n\n");
	printf(     "...   Reading Temporary Time Step Buffer File  ...\n");
	printf(	    "\n\n");

	//Set the time step buffer file pointer to null
	dtbufferfile_fp = NULL;

	//Open the time step buffer file for reading
	dtbufferfile_fp=fopen(dtbufferfile,"r");

	//if the time step buffer file pointer is null, abort execution
	if(dtbufferfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Temporary Time Step Buffer File (dtbufferfile) : %s \n",dtbufferfile);

		//Write message to screen
		printf("Error! Can't open Temporary Time Step Buffer File (dtbufferfile) : %s \n",dtbufferfile);
		exit(EXIT_FAILURE);	//abort
	}

	//loop over number of time steps
	for(i=1; i<=ndt; i++)
	{
		//Record 3
		fscanf(dtbufferfile_fp, "%f %f",	//read
			&dt[i],			//time step value (seconds)
			&dttime[i]);	//time break for time step (hours)

	}	//end loop over time step series

	//Close External Time Step Input file
	fclose(dtbufferfile_fp);

	//Write final dt, dttime sequence to output file...
	//
	//write message to screen
	printf("\n\n");
	printf(     "...   Writing Final Time Step Sequence to File  ...\n");
	printf(	    "\n\n");

	//Record 1: Write file header
	fprintf(dtoutputfile_fp, "Time Step Series for: %s\n", inputfile);

	//Record 2: Write ndt (number of dt and dttime pairs)
	fprintf(dtoutputfile_fp, "%s %d\n", "NDT", ndt);

	//loop over buffer elements
	for(i=1; i<=ndt; i++)
	{
		//Write paired dt[] and dttime[] values to to file
		fprintf(dtoutputfile_fp, "%f  %f\n", dt[i], dttime[i]);

	}	//end loop over buffer elements

	//Close External Time Step Output file
	fclose(dtoutputfile_fp);

	//Remove buffer file
	remove(dtbufferfile);

//End of function: Return to trex
}
