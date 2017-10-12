/*---------------------------------------------------------------------
C-  Function:   ReadChannelFpocFile.c
C-
C-  Purpose/    ReadChannelFpocFile is called to read the particle
C-  Methods:    fraction organic carbon (fpoc) and time function ID
C-              pointer for a specified solids (particle) type for
C-              each vertical layer in the sediment stack for each
C-              node of each link in the channel network
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    fpocch[solid][i][j][layer]
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupE
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		20-SEP-2004
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

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void ReadChannelFpocFile(int isolid, float scale)
{
	int
		chanlinks,	//number of channel links (dummy)
		chansolids,	//number of solids types (dummy)
		linknum,	//link number (dummy)
		linknodes,	//number of nodes in a link (dummy)
		nodenum,	//node number (dummy)
		layernum;	//layer number (dummy)

	//write message to screen
	printf("\n\n*****************************************\n");
	printf(	   "*                                       *\n");
	printf(	   "*   Reading Channel Particle Fpoc File  *\n");
	printf(	   "*                                       *\n");
	printf(	   "*****************************************\n\n\n");

	//Set the environmental properties file pointer to null
	envpropfile_fp = NULL;

	//Open the environmental properties file for reading
	envpropfile_fp=fopen(envpropfile,"r");

	//if the environmental properties file pointer is null, abort execution
	if(envpropfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Can't open Channel Environmental Properties File: %s \n",envpropfile);

		//Write message to screen
		printf("Can't open Channel Environmental Properties File: %s \n",envpropfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for environmental properties file to file
	fprintf(echofile_fp, "\n\n\n  Channel Particle Fpoc File  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, envpropfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(envpropfile_fp, "%s %d %s %d",	//read
		varname,		//dummy
		&chanlinks,		//number of channel links (local)
		varname,		//dummy
		&chansolids);	//number of channel solids types (local)

	//Error check: chanlinks must equal nlinks...
	//
	//If number of links does not equal the global value from the link file, abort...
	if(chanlinks != nlinks)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nChannel Particle Fpoc File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Channel Particle Fpoc File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel links

	//Error check: chansolids must equal nsolids...
	//
	//If number of solids does not equal the global value from the input file, abort...
	if(chansolids != nsolids)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nChannel Particle Fpoc File Error:\n");
		fprintf(echofile_fp,       "  chansolids = %5d   nsolids = %5d\n", chansolids, nsolids);

		//Write error message to screen
		printf("Channel Particle Fpoc File Error:\n");
		printf("  chansolids = %5d   nsolids = %5d\n", chansolids, nsolids);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel solids

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(envpropfile_fp, "%s %d %s %d",	//read
			varname,		//dummy
			&linknum,		//link number (local) (known from loop counter, read as dummy)
			varname,		//dummy
			&linknodes);	//number of nodes (local)

		//Two more error checks...
		//  1. data must be in link order (1 to nlinks); and
		//  2. channodes must equal nnodes for each link.
		//
		//The environmental properties  file data must be in
		//sequential order from 1 to nlinks.
		//
		//If link number read from file does not match the link index (i), abort...
		if(linknum != i)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nChannel Particle Fpoc File Error:\n");
			fprintf(echofile_fp,       "  link read = %d   link expected = %d\n",
				linknum, i);

			//Write error message to screen
			printf("Channel Particle Fpoc File Error:\n");
			printf("  link read = %d   link expected = %d\n",
				chanlinks, i);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//If number of nodes does not equal the global value from the node file, abort...
		if(linknodes != nnodes[i])
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nChannel Particle Fpoc File Error:\n");
			fprintf(echofile_fp,       "  link = %d   linknodes = %d   nnodes = %d\n",
				i, linknodes, nnodes[i]);

			//Write error message to screen
			printf("Channel Particle Fpoc File Error:\n");
			printf("  link = %d   linknodes = %d   nnodes = %d\n",
				i, linknodes, nnodes[i]);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//Loop of number of nodes in current link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Write label for environmental properties to file
			fprintf(echofile_fp, "\nLink  Node  NSTACK");
			fprintf(echofile_fp, "\n----  ----  ------\n");

			//Record 4
			fscanf(envpropfile_fp, "%s %d %s %d",	//read
				varname,			//dummy
				&nodenum,			//node number (local) (read as dummy)
				varname,			//dummy
				&nstackch0[i][j]);	//number of sediment stack elements (global)

				//Echo properties
				fprintf(echofile_fp, "%4d  %4d  %5d\n",
					i, j, nstackch0[i][j]);

			//loop over number of layers in reverse order
			for(k=nstackch0[i][j]; k>=1; k--)
			{
				//Write label to file
				fprintf(echofile_fp, "\n  Layer  Thickness (m)  Width (m)  Porosity");
				fprintf(echofile_fp, "\n  -----  -------------  ---------  --------\n");

				//Record 6
				fscanf(envpropfile_fp, "%s %d",	//read
					varname,				//dummy
					&layernum);				//layer number (local) (read as dummy)

				//Echo environmental properties to file
				fprintf(echofile_fp, "  %5d  %13.4f  %9.3f  %8.4f\n",
					k, hlayerch0[i][j][k], bwlayerch0[i][j][k], porositych[i][j][k]);

//mlv: need to fix labels...
				//Write label to file
				fprintf(echofile_fp, "\n  Grain Size Distribution:\n");
				fprintf(echofile_fp, "\n  Solid  GSD Fraction");
				fprintf(echofile_fp, "\n  -----  ------------");

				//Record 7a
				fscanf(envpropfile_fp, "%s",	//read
					varname);					//dummy

				//Loop of number of solids (particle types)
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Record 7b
					fscanf(envpropfile_fp, "%f",	//read
						&fpocch[isolid][i][j][k]);	//grain size distribution

					//Echo particle fpoc to file
					fprintf(echofile_fp, "\n  %5d  %12.7f",
						isolid, fpocch[isolid][i][j][k]);

					//Error Checking
					if((fpocch[isolid][i][j][k] < 0.0) || (fpocch[isolid][i][j][k] > 1.0))
					{
						//Write error message to file
						fprintf(echofile_fp, "Error:  particle fraction organic carbon < 0.0 or > 1.0!\n");

						exit(EXIT_FAILURE);	//exit the program

					}	//end check of fpocch[isolid][i][j][k]

				}	//end loop over number of solids

			}	//end loop over number of layers

		}	//end loop over number of nodes

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Close environmental properties file
	fclose(envpropfile_fp);

//End of Function: Return to ReadDataGroupE
}
