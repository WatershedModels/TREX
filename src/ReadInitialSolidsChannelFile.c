/*---------------------------------------------------------------------
C-  Function:   ReadInitialSolidsChannelFile.c
C-
C-  Purpose/    ReadInitialSolidsChannelFile is called to read the
C-  Methods:    initial concentration of each solids tytpe for each
C-              node of each link in the channel network.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    csedch[isolid][i][j][0]
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupC
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

void ReadInitialSolidsChannelFile()
{
	//local variable declarations
	int
		isolid;		//local loop index for number of solids

	int
		chanlinks,	//number of channel links (dummy)
		chansolids,	//number of solids types (dummy)
		linknum,	//link number (dummy)
		linknodes,	//number of nodes in a link (dummy)
		nodenum;	//node number (dummy)

	float
		csedtemp;	//local variable for initial solids concentration (g/m3)

	//write message to screen
	printf("\n\n********************************************************\n");
	printf(	   "*                                                      *\n");
	printf(	   "*   Reading Initial Solids Concentration Channel File  *\n");
	printf(	   "*                                                      *\n");
	printf(	   "********************************************************\n\n\n");

	//Set the initial (suspended) solids channel file pointer to null
	initialsschfile_fp = NULL;

	//Open the initial suspended solids file for reading
	initialsschfile_fp = fopen(initialsschfile,"r");

	//if the initial suspended solids file pointer is null, abort execution
	if(initialsschfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Channel Initial Suspended Solids file: %s \n",initialsschfile);	

		//Write message to screen
		printf("Error! Can't open Channel Initial Suspended Solids file: %s \n",initialsschfile);
	
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for channel initial suspended solids file to file
	fprintf(echofile_fp, "\n\n\n  Channel Initial Suspended Solids  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, initialsschfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Record 2
	fscanf(initialsschfile_fp, "%s %d %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nSediment Properties File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Sediment Properties File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel links

	//Error check: chansolids must equal nsolids...
	//
	//If number of solids does not equal the global value from the input file, abort...
	if(chansolids != nsolids)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nChannel Initial Solids Concentration File Error:\n");
		fprintf(echofile_fp,       "  chansolids = %5d   nsolids = %5d\n", chansolids, nsolids);

		//Write error message to screen
		printf("Channel Initial Solids Concentration File Error:\n");
		printf("  chansolids = %5d   nsolids = %5d\n", chansolids, nsolids);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel solids

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(initialsschfile_fp, "%s %d %s %d",	//read
			varname,		//dummy
			&linknum,		//link number (local) (known from loop counter, read as dummy)
			varname,		//dummy
			&linknodes);	//number of nodes (local)

		//Two more error checks...
		//  1. data must be in link order (1 to nlinks); and
		//  2. channodes must equal nnodes for each link.
		//
		//The sediment properties  file data must be in
		//sequential order from 1 to nlinks.
		//
		//If link number read from file does not match the link index (i), abort...
		if(linknum != i)
		{
			//Write error message to file
			fprintf(echofile_fp,
				"\n\n\nChannel Initial Solids Concentration File Error:\n");
			fprintf(echofile_fp, "  link read = %d   link expected = %d\n",
				linknum, i);

			//Write error message to screen
			printf("Channel Initial Solids Concentration File Error:\n");
			printf("  link read = %d   link expected = %d\n",
				chanlinks, i);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//If number of nodes does not equal the global value from the node file, abort...
		if(linknodes != nnodes[i])
		{
			//Write error message to file
			fprintf(echofile_fp,
				"\n\n\nChannel Initial Solids Concentration File Error:\n");
			fprintf(echofile_fp, "  link = %d   linknodes = %d   nnodes = %d\n",
				i, linknodes, nnodes[i]);

			//Write error message to screen
			printf("Channel Initial Solids Concentration File Error:\n");
			printf("  link = %d   linknodes = %d   nnodes = %d\n",
				i, linknodes, nnodes[i]);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//Write label for channel initial solids concentrations to file
		fprintf(echofile_fp, "Link  Node  Solid Type  Csedch (g/m3)\n");
		fprintf(echofile_fp, "----  ----  ----------  -------------\n");

		//Loop of number of nodes in current link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Record 4
			fscanf(initialsschfile_fp, "%s %d",	//read
				varname,						//dummy
				&nodenum);						//node number (local) (read as dummy)
			
			//loop over solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Record 5
				fscanf(initialsschfile_fp, "%f",	//read
					&csedtemp);		//initial channel solids concentration (g/m3) (local)

				//Echo sediment properties to file...
				fprintf(echofile_fp, "%4d  %4d  %10d  %13.4f\n",
					i, j, isolid, csedtemp);

				//if the initial channel water depth is greater than zero
				if(hch[i][j] > 0.0)
				{
					//set the initial channel suspended solids value
					csedch[isolid][i][j][0] = csedtemp;
				}
				else	//else hov[][] = 0
				{
					//set the initial solids concentration to zero
					csedch[isolid][i][j][0] = 0.0;

				}	//end if hch[][] > 0.0

			}	//end loop over solids

			//Start a new line for the next row of data in the echo file 
			fprintf(echofile_fp, "\n");

		}	//end loop over number of nodes

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Close Channel Initial Suspended Solids File
	fclose(initialsschfile_fp);

//End of Function: Return to ReadDataGroupC
}
