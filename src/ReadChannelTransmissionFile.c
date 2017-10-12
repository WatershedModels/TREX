/*---------------------------------------------------------------------
C-  Function:	ReadChannelTransmissionFile.c
C-
C-  Purpose/    ReadChannelTransmissionFile is called to read the
C-  Methods:    transmission loss properties of each node of each link
C-              in the channel network.
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
C-  Created:    Mark Velleux (CSU)
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

void ReadChannelTransmissionFile()
{
	//local variable declarations
	int
		chanlinks,	//number of channel links
		channodes;	//number of nodes in a link

	//write message to screen
	printf("\n\n********************************************************\n");
	printf(	   "*                                                      *\n");
	printf(	   "*   Reading Channel Transmission Loss Properties File  *\n");
	printf(	   "*                                                      *\n");
	printf(	   "********************************************************\n\n\n");

	//Set the channel transmission loss property file pointer to null
	channeltlossfile_fp = NULL;

	//Open the channel transmission loss property file for reading
	channeltlossfile_fp=fopen(channeltlossfile,"r");

	//if the channel transmission loss property file pointer is null, abort execution
	if(channeltlossfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Channel Transmission Loss Property File: %s \n",channeltlossfile);

		//Write message to screen
		printf("Error! Can't open Channel Transmission Loss Property File: %s \n",channeltlossfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for channel transmission loss property file to file
	fprintf(echofile_fp, "\n\n\n  Channel Tranmission Loss Property File  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, channeltlossfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Record 2
	fscanf(channeltlossfile_fp, "%s %d",	//read
		varname,		//dummy
		&chanlinks);	//number of channel links (local))

	//If number of links does not equal the global value from the link file, abort...
	if(chanlinks != nlinks)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nChannel Transmission Loss Properties File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Channel File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort
	}	//end check of number of channel links

	//Write second label for channel transmission loss properties file to file
	fprintf(echofile_fp, "\nLink  Node  Hydraulic Conductivity (m/s)  ");
	fprintf(echofile_fp,   "Capillary Suction Head (m)  ");
	fprintf(echofile_fp,   "Sediment Moisture Deficit (-)");
	fprintf(echofile_fp, "\n----  ----  ----------------------------  ");
	fprintf(echofile_fp,   "--------------------------  ");
	fprintf(echofile_fp,   "-----------------------------\n\n");

	//Allocate memory for channel transmission loss properties
	khsed = (float **)malloc((nlinks+1) * sizeof(float *));			//sediment hydraulic conductivity (m/s)
	capshsed = (float **)malloc((nlinks+1) * sizeof(float *));		//sediment capillary suction head (m)
	sedmd = (float **)malloc((nlinks+1) * sizeof(float *));			//sediment moisture deficit (dimensionless)

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(channeltlossfile_fp, "%d %d",	//read
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

		//Allocate remaining memory for channel transmission loss properties
		khsed[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//sediment hydraulic conductivity (m/s)
		capshsed[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));		//sediment capillary suction head (m)
		sedmd[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//sediment moisture deficit (dimensionless)

		//Loop over number of nodes in this link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Record 4
			fscanf(channeltlossfile_fp, "%f %f %f",	//read
				&khsed[i][j],				//sediment hydraulic conductivity (m/s)
				&capshsed[i][j],			//sediment capillary suction head (m)
				&sedmd[i][j]);				//sediment moisture deficit (dimensionless)

			//Echo channel transmission loss properties to file
			fprintf(echofile_fp, "%4d %5d %29.12f %27.8f %30.8f\n",
				i,
				j,
				khsed[i][j],
				capshsed[i][j],
				sedmd[i][j]);

		}	//end loop over nodes in current link

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Close Channel Transmission Loss Property File
	fclose(channeltlossfile_fp);

//End of Function: Return to ReadDataGroupB
}
