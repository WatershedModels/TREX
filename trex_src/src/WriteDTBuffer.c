/*---------------------------------------------------------------------
C-  Function:   WriteDTBuffer.c
C-
C-  Purpose\    WriteDTBuffer is called to write the sequence of time
C-  Methods:    step (dt) and time (dttime) value pairs that control 
C-              numerical integration to a temporary buffer file when
C-              the number of values to be stored exceeds allocated
C-              memory.
C-
C-  Inputs:     dt[], dttime[]
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  WaterTransport, trex
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

void WriteDTBuffer()
{
	//write message to screen
	printf("\n\n**********************************************\n");
	printf(	   "*                                            *\n");
	printf(	   "*   Writing Temporary Time Step Buffer File  *\n");
	printf(	   "*                                            *\n");
	printf(	   "**********************************************\n\n\n");

	//Set the time step buffer file pointer to null
	dtbufferfile_fp = NULL;

	//Open the time step buffer file for writing (append mode)
	dtbufferfile_fp=fopen(dtbufferfile,"a");

	//if the time step buffer file pointer is null, abort execution
	if(dtbufferfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Temporary Time Step Buffer File (dtbufferfile) : %s \n",dtbufferfile);

		//Write message to screen
		printf("Error! Can't open Temporary Time Step Buffer File (dtbufferfile) : %s \n",dtbufferfile);
		exit(EXIT_FAILURE);	//abort
	}

	//loop over buffer elements
	for(i=1; i<=bdt; i++)
	{
		//Write paired dt[] and dttime[] values to to file
		fprintf(dtbufferfile_fp, "%f  %f\n", dt[i-1], dttime[i-1]);

		//Reset dt[] and dttime[] arrays...
		//
		//reset dt[i-1] to an "uninitialized" value
		dt[i-1] = -1.0;

		//reset dttime[i-1] to an "uninitialized" value
		dttime[i-1] = -1.0;

	}	//end loop over buffer elements

	//Close Temporary Time Step Buffer File
	fclose(dtbufferfile_fp);

//End of function: Return to WaterTransport or trex
}
