/*---------------------------------------------------------------------
C-  Function:   ReadInitialChemicalChannelFile.c
C-
C-  Purpose/    ReadInitialChemicalChannelFile is called to read the
C-  Methods:    initial concentration of each chemical for the water
C-              column of each node of each link in the channel network.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    cchemch[ichem][i][j][0]
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupD
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		26-MAR-2004
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

void ReadInitialChemicalChannelFile()
{
	//local variable declarations
	int
		ichem;		//local loop index for number of chemicals

	int
		chanlinks,	//number of channel links (dummy)
		chanchems,	//number of chemical types (dummy)
		linknum,	//link number (dummy)
		linknodes,	//number of nodes in a link (dummy)
		nodenum;	//node number (dummy)

	float
		cchemtemp;	//local variable for initial chemical concentration (g/m3)

	//write message to screen
	printf("\n\n**********************************************************\n");
	printf(	   "*                                                        *\n");
	printf(	   "*   Reading Initial Chemical Concentration Channel File  *\n");
	printf(	   "*                                                        *\n");
	printf(	   "**********************************************************\n\n\n");

	//Set the initial (suspended) chemical channel file pointer to null
	initialchemchfile_fp = NULL;

	//Open the initial suspended chemical file for reading
	initialchemchfile_fp = fopen(initialchemchfile,"r");

	//if the initial suspended chemical file pointer is null, abort execution
	if(initialchemchfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Channel Initial Suspended Chemical file: %s \n",initialchemchfile);
		
		//Write message to screen
		printf("Error! Can't open Channel Initial Suspended Chemical file: %s \n",initialchemchfile);
	
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for channel initial suspended chemical file to file
	fprintf(echofile_fp, "\n\n\n  Channel Initial Suspended Chemicals  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, initialchemchfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Record 2
	fscanf(initialchemchfile_fp, "%s %d %s %d",	//read
		varname,		//dummy
		&chanlinks,		//number of channel links (local)
		varname,		//dummy
		&chanchems);	//number of channel chemical types (local)

	//Error check: chanlinks must equal nlinks...
	//
	//If number of links does not equal the global value from the link file, abort...
	if(chanlinks != nlinks)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nChemical Properties File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Chemical Properties File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel links

	//Error check: chanchems must equal nchems...
	//
	//If number of chemicals does not equal the global value from the input file, abort...
	if(chanchems != nchems)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nChannel Initial Chemical Concentration File Error:\n");
		fprintf(echofile_fp,       "  chanchems = %5d   nchems = %5d\n", chanchems, nchems);

		//Write error message to screen
		printf("Channel Initial Chemical Concentration File Error:\n");
		printf("  chanchems = %5d   nchems = %5d\n", chanchems, nchems);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel chemicals

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(initialchemchfile_fp, "%s %d %s %d",	//read
			varname,		//dummy
			&linknum,		//link number (local) (known from loop counter, read as dummy)
			varname,		//dummy
			&linknodes);	//number of nodes (local)

		//Two more error checks...
		//  1. data must be in link order (1 to nlinks); and
		//  2. channodes must equal nnodes for each link.
		//
		//The chemical properties  file data must be in
		//sequential order from 1 to nlinks.
		//
		//If link number read from file does not match the link index (i), abort...
		if(linknum != i)
		{
			//Write error message to file
			fprintf(echofile_fp,
				"\n\n\nChannel Initial Chemical Concentration File Error:\n");
			fprintf(echofile_fp, "  link read = %d   link expected = %d\n",
				linknum, i);

			//Write error message to screen
			printf("Channel Initial Chemical Concentration File Error:\n");
			printf("  link read = %d   link expected = %d\n",
				chanlinks, i);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//If number of nodes does not equal the global value from the node file, abort...
		if(linknodes != nnodes[i])
		{
			//Write error message to file
			fprintf(echofile_fp,
				"\n\n\nChannel Initial Chemical Concentration File Error:\n");
			fprintf(echofile_fp, "  link = %d   linknodes = %d   nnodes = %d\n",
				i, linknodes, nnodes[i]);

			//Write error message to screen
			printf("Channel Initial Chemical Concentration File Error:\n");
			printf("  link = %d   linknodes = %d   nnodes = %d\n",
				i, linknodes, nnodes[i]);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//Write label for channel initial chemical concentrations to file
		fprintf(echofile_fp, "Link  Node  Chemical Type  Cchemch (g/m3)\n");
		fprintf(echofile_fp, "----  ----  -------------  --------------\n");

		//Loop of number of nodes in current link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Record 4
			fscanf(initialchemchfile_fp, "%s %d",	//read
				varname,						//dummy
				&nodenum);						//node number (local) (read as dummy)
			
			//loop over chemical
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//Record 5
				fscanf(initialchemchfile_fp, "%f",	//read
					&cchemtemp);		//initial channel chemical concentration (g/m3) (local)

				//Echo chemical properties to file...
				fprintf(echofile_fp, "%4d  %4d  %13d  %14.4f\n",
					i, j, ichem, cchemtemp);

				//if the initial channel water depth is greater than zero
				if(hch[i][j] > 0.0)
				{
					//set the initial channel suspended chemical value
					cchemch[ichem][i][j][0] = cchemtemp;
				}
				else	//else hov[][] = 0
				{
					//set the initial chemical concentration to zero
					cchemch[ichem][i][j][0] = 0.0;

				}	//end if hch[][] > 0.0

			}	//end loop over chemical

			//Start a new line for the next row of data in the echo file 
			fprintf(echofile_fp, "\n");

		}	//end loop over number of nodes

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Close Channel Initial Suspended Chemical File
	fclose(initialchemchfile_fp);

//End of Function: Return to ReadDataGroupD
}
