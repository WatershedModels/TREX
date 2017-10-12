/*---------------------------------------------------------------------
C-  Function:	ReadInitialTransmissionLossFile.c
C-
C-  Purpose/    ReadInitialTransmissionLossFile is called to read the
C-  Methods:    initial transmission loss depth (depth to wetting front)
C-              of each node of each link in the channel network.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   ksim, ctlopt
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:    Mark Velleux
C-
C-  Date:       15-JUN-2005
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
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void ReadInitialTransmissionLossFile()
{
	//local variable declarations
	int
		chanlinks,	//number of channel links
		channodes;	//number of nodes in a link

	//write message to screen
	printf("\n\n***************************************************\n");
	printf(	   "*                                                 *\n");
	printf(	   "*   Reading Initial Transmission Loss Depth File  *\n");
	printf(	   "*                                                 *\n");
	printf(	   "***************************************************\n\n\n");

	//Set the channel transmission loss depth file pointer to null
	translossfile_fp = NULL;

	//Open the channel transmission loss depth file for reading
	translossfile_fp=fopen(translossfile,"r");

	//if the channel transmission loss depth file pointer is null, abort execution
	if(translossfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Initial Channel Transmission Loss Depth File: %s \n",translossfile);

		//Write message to screen
		printf("Can't open Initial Channel Transmission Loss Depth File: %s \n",translossfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for initial channel transmission loss depth file to file
	fprintf(echofile_fp, "\n\n\n  Initial Channel Tranmission Loss Depth File  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, translossfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Record 2
	fscanf(translossfile_fp, "%s %d",	//read
		varname,		//dummy
		&chanlinks);	//number of channel links (local))

	//If number of links does not equal the global value from the link file, abort...
	if(chanlinks != nlinks)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nInitial Channel Transmission Loss Depth File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Channel File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort
	}	//end check of number of channel links

	//Write second label for channel transmission loss properties file to file
	fprintf(echofile_fp, "\nLink  Node  Initial Transmission Loss Depth (m)  ");
	fprintf(echofile_fp, "\n----  ----  -----------------------------------\n\n");

	//Allocate memory for channel transmission loss depth
	translossdepth = (float **)malloc((nlinks+1) * sizeof(float *));	//transmission loss depth (m)

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(translossfile_fp, "%d %d",	//read
			&chanlinks,		//link number (known from loop counter, read as dummy)
			&channodes);	//number of channel links (local)

		//Two error checks...
		//
		//The channel file data must be in sequential order from 1 to nlinks.
		//
		//If link number read from file does not match the link index (i), abort...
		if(chanlinks != i)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nChannel File Error:\n");
			fprintf(echofile_fp,       "  link read = %5d   link expected = %5d\n",
				chanlinks, i);

			//Write error message to screen
			printf("Channel File Error:\n");
			printf("  link read = %5d   link expected = %5d\n",
				chanlinks, i);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//If number of nodes does not equal the global value from the node file, abort...
		if(channodes != nnodes[i])
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nChannel File Error:\n");
			fprintf(echofile_fp,       "  link = %5d   channodes = %5d   nnodes = %5d\n",
				i, channodes, nnodes[i]);

			//Write error message to screen
			printf("Channel File Error:\n");
			printf("  link = %5d   channodes = %5d   nnodes = %5d\n",
				i, channodes, nnodes[i]);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//Allocate remaining memory for channel transmission loss depth
		translossdepth[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//sediment hydraulic conductivity (m/s)

		//Loop over number of nodes in this link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Record 4
			fscanf(translossfile_fp, "%f",	//read
				&translossdepth[i][j]);		//transmission loss depth (m)

			//Echo initial channel transmission loss depth to file
			fprintf(echofile_fp, "%4d %5d %35.8f\n",
				i,
				j,
				translossdepth[i][j]);

		}	//end loop over nodes in current link

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Close Initial Channel Transmission Loss Depth File
	fclose(translossfile_fp);

//End of Function: Return to ReadDataGroupB
}
