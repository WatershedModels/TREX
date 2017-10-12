/*---------------------------------------------------------------------
C-  Function:   ReadInitialWaterChannelFile.c
C-
C-  Purpose/    ReadInitialWaterChannelFile is called to read the
C-  Methods:    initial water depth file that specifies the depth of
C-              water in channels (link, node) at the start of the
C-              simulation.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:    Mark Velleux (CSU)
C-              John England (USBR)
C-
C-  Date:       03-SEP-2003
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

void ReadInitialWaterChannelFile()
{
	//Local Variable Declarations
	int
		chanlinks,	//number of channel links
		*channodes;	//number of nodes in a link

	//write message to screen
	printf("\n\n***********************************************\n");
	printf(	   "*                                             *\n");
	printf(	   "*   Reading Initial Water Depth Channel File  *\n");
	printf(	   "*                                             *\n");
	printf(	   "***********************************************\n\n\n");

	//Set the initial water depth overland file pointer to null
	waterchannelfile_fp = NULL;

	//Open the initial water depth overland file for reading
	waterchannelfile_fp=fopen(waterchannelfile,"r");

	//if the initial water depth overland file pointer is null, abort execution
	if(waterchannelfile_fp==NULL)
	{
		//Write message to screen
		fprintf(echofile_fp, "Error! Can't open Initial Water Depth Channel File : %s \n", waterchannelfile);

		//Write message to screen
		printf("Error! Can't open Initial Water Depth Channel File : %s \n", waterchannelfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for initial water depth overland file to file
	fprintf(echofile_fp, "\n\n\n  Initial Water Depth Channel File: Link/Node Water Depths  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, waterchannelfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(waterchannelfile_fp, "%s %d",	//read
		varname,		//dummy
		&chanlinks);	//number of channel links (local))

	//If number of links does not equal the global value from the link file, abort...
	if(chanlinks != nlinks)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nInitial Water Depth Channel File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Initial Water Depth Channel File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort
	}	//end check of number of channel links

	//Write second label for initial water depths file to file
	fprintf(echofile_fp, "\n Link    Node    Initial Water Depth (m)    Note (if any) ");
	fprintf(echofile_fp, "\n------  ------  -------------------------  ---------------\n\n");

	//Allocate memory
	channodes = (int *)malloc((nlinks+1) * sizeof(int));	//number of channel links
	hch = (float **)malloc((nlinks+1) * sizeof(float *));	//water depth in channel (link/node)

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(waterchannelfile_fp, "%d %d",	//read
			&chanlinks,		//link number (known from loop counter, read as dummy)
			&channodes[i]);	//number of channel links (local)

		//Two error checks...
		//
		//The channel file data must be in sequential order from 1 to nlinks.
		//
		//If link number read from file does not match the link index (i), abort...
		if(chanlinks != i)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nInitial Water Depth Channel File Error:\n");
			fprintf(echofile_fp,       "  link read = %5d   link expected = %5d\n",
				chanlinks, i);

			//Write error message to screen
			printf("Initial Water Depth Channel File Error:\n");
			printf("  link read = %5d   link expected = %5d\n",
				chanlinks, i);

			exit(EXIT_FAILURE);					//abort
		}	//end check of number of channel nodes

		//If number of nodes (in this link) does not equal the global value from the node file, abort...
		if(channodes[i] != nnodes[i])
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nInitial Water Depth Channel File Error:\n");
			fprintf(echofile_fp,       "  link = %5d   channodes = %5d   nnodes = %5d\n",
				i, channodes[i], nnodes[i]);

			//Write error message to screen
			printf("Initial Water Depth Channel File Error:\n");
			printf("  link = %5d   channodes = %5d   nnodes = %5d\n",
				i, channodes[i], nnodes[i]);

			exit(EXIT_FAILURE);					//abort
		}	//end check of number of channel nodes

		//Allocate remaining memory for hch[][] array
		hch[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));	//water depth in channel (link/node)

		//Loop over number of nodes in this link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Record 4
			fscanf(waterchannelfile_fp, "%f",	//read
				&hch[i][j]);		//initial water depth in channel link i, node j (m)

			//Echo initial water depth in channel to file
			fprintf(echofile_fp, "%4d  %6d  %25.5f",
				i, j, hch[i][j]);

			//if water depth is greater than bank height for this link/node
			if(hch[i][j] > hbank[i][j])
			{
				//Add warning to echo file and end the line
				fprintf(echofile_fp, "  %s\n",
					"initial depth > bank height");
			}
			else
			{
				//start a new line in the echo file
				fprintf(echofile_fp, "\n");
			}	//end if hchinit > bank height

		}	//end loop over nodes in current link

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Free local memory for channodes
	free(channodes);

	//Close Initial Water Depth Channel File
	fclose(waterchannelfile_fp);

//End of Function: Return to ReadDataGroupB
}
