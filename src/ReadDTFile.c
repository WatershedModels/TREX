/*---------------------------------------------------------------------
C-  Function:   ReadDTFile.c
C-
C-  Purpose\    ReadDTFile is called to read the sequence of time step
C-  Methods:    (dt) and time (dttime) value pairs that control
C-              numerical integration from an external file.
C-
C-  Inputs:     None
C-
C-  Outputs:    ndt, dt[], dttime[]
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupA
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

void ReadDTFile()
{
	//write message to screen
	printf("\n\n********************************************\n");
	printf(	   "*                                          *\n");
	printf(	   "*   Reading External Time Step Input File  *\n");
	printf(	   "*                                          *\n");
	printf(	   "********************************************\n\n\n");

	//Set the time step input file pointer to null
	dtinputfile_fp = NULL;

	//Open the time step input file for reading
	dtinputfile_fp=fopen(dtinputfile,"r");

	//if the time step input file pointer is null, abort execution
	if(dtinputfile_fp==NULL)
	{
		//Note:  File status information can only be sent to screen and
		//       cannot be sent to the echo file because the echo file is
		//       not created and opened until all Data Group A inputs
		//       have been read...
		//
		//Write message to screen
		printf("Error! Can't open External Time Step Input File (dtinputfile) : %s \n",dtinputfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Record 1
	fgets(header, MAXHEADERSIZE, dtinputfile_fp);	//read header

	//Record 2
	fscanf(dtinputfile_fp, "%s %d",	//read
		varname,	//dummy
		&ndt);		//number of time steps

	//Allocate memory for numerical integration time steps
	dt = (float *)malloc((ndt+1) * sizeof(float));		//allocate memory
	dttime = (float *)malloc((ndt+1) * sizeof(float));	//allocate memory

	//loop over number of time steps
	for(i=1; i<=ndt; i++)
	{
		//Record 3
		fscanf(dtinputfile_fp, "%f %f",	//read
			&dt[i],			//time step value (seconds)
			&dttime[i]);	//time break for time step (hours)

	}	//end loop over time step series

	//Close External Time Step Input file
	fclose(dtinputfile_fp);

//End of function: Return to ReadDataGroupA
}
