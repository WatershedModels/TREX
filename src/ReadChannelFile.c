/*---------------------------------------------------------------------
C-  Function:	ReadChannelFile.c
C-
C-  Purpose/    ReadChannelFile is called to read properties of each
C-  Methods:    node of each link in the channel network.
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
C-  Created:    Mark Velleux
C-
C-  Date:       06-AUG-2003
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

void ReadChannelFile()
{
	//local variable declarations
	int
		chanlinks,	//number of channel links
		channodes;	//number of nodes in a link

	//write message to screen
	printf("\n\n**************************************\n");
	printf(	   "*                                    *\n");
	printf(	   "*   Reading Channel Properties File  *\n");
	printf(	   "*                                    *\n");
	printf(	   "**************************************\n\n\n");

	//Set the channel file pointer to null
	channelfile_fp = NULL;

	//Open the channel file for reading
	channelfile_fp=fopen(channelfile,"r");

	//if the channel file pointer is null, abort execution
	if(channelfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Can't open Channel File: %s \n",channelfile);

		//Write message to screen
		printf("Can't open Channel File: %s \n",channelfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for channel properties file to file
	fprintf(echofile_fp, "\n\n\n  Channel File: Channel Network Characteristics  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, channelfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Record 2
	fscanf(channelfile_fp, "%s %d",	//read
		varname,		//dummy
		&chanlinks);	//number of channel links (local))

	//If number of links does not equal the global value from the link file, abort...
	if(chanlinks != nlinks)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nChannel File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Channel File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort
	}	//end check of number of channel links

	//Write second label for channel properties file to file
	fprintf(echofile_fp, "\nLink  Node  bwidth (m)  ");
	fprintf(echofile_fp,   "sideslope (1:z)  hbank (m)  nmanning  ");
	fprintf(echofile_fp,   "sinuosity  deadstorage (m)");
	fprintf(echofile_fp, "\n----  ----  ----------  ");
	fprintf(echofile_fp,   "---------------  ---------  --------  ");
	fprintf(echofile_fp,   "---------  ---------------\n\n");

	//Allocate memory for channel properties
	bwidth = (float **)malloc((nlinks+1) * sizeof(float *));			//bottom width (m)
	sideslope = (float **)malloc((nlinks+1) * sizeof(float *));			//sideslope (dimensionless)
	hbank = (float **)malloc((nlinks+1) * sizeof(float *));				//channel bank height (distance from bed to top of bank) (m)
	nmanningch = (float **)malloc((nlinks+1) * sizeof(float *));		//manning's n roughness coefficient for channels (n units)
	sinuosity = (float **)malloc((nlinks+1) * sizeof(float *));			//channel sinuosity (dimensionless)
	deadstoragedepth = (float **)malloc((nlinks+1) * sizeof(float *));	//channel dead storage depth (m)
	twidth = (float **)malloc((nlinks+1) * sizeof(float *));			//top width at top of bank (m)

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(channelfile_fp, "%d %d",	//read
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

		//Allocate remaining memory for channel properties
		bwidth[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));				//bottom width
		sideslope[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//sideslope
		hbank[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));				//channel bank height of channel (distance from bed to top of bank)
		nmanningch[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//manning's n roughness coefficient for channels
		sinuosity[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));			//channel sinuosity
		deadstoragedepth[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));	//channel dead storage depth (m)
		twidth[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));				//top width

		//Loop over number of nodes in this link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Record 4
			fscanf(channelfile_fp, "%f %f %f %f %f %f",	//read
				&bwidth[i][j],				//bottom width (m)
				&sideslope[i][j],			//sideslope (dimensionless)
				&hbank[i][j],				//channel bank height (distance from bed to top of bank) (m)
				&nmanningch[i][j],			//manning's n roughness coefficient for channels
				&sinuosity[i][j],			//channel sinuosity (dimensionless)
				&deadstoragedepth[i][j]);	//channel dead storage depth (m)

			//Echo channel properties to file
			fprintf(echofile_fp, "%4d %5d %11.3f %16.3f %10.3f %9.4f %10.3f %16.3f\n",
				i,
				j,
				bwidth[i][j],
				sideslope[i][j],
				hbank[i][j],
				nmanningch[i][j],
				sinuosity[i][j],
				deadstoragedepth[i][j]);

			//Compute channel top width at top of bank (m)
			twidth[i][j] = (float)(bwidth[i][j] + 2.0 * sideslope[i][j] * hbank[i][j]);

		}	//end loop over nodes in current link

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Close Channel File
	fclose(channelfile_fp);

//End of Function: Return to ReadDataGroupB
}
