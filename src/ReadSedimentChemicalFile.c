/*----------------------------------------------------------------------
C-  Function:   ReadSedimentChemicalFile.c
C-
C-  Purpose/    ReadSedimentChemicalFile is called to read the initial
C-  Methods:    concentration of each chemical in each node of each link
C-              in the channel network
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    cchemch[chem][link][node][layer]
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupD
C-
C-  Created:    Mark Velleux
C-
C-  Date:       12-JUL-2004
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

void ReadSedimentChemicalFile()
{
	//local variable declarations
	int
		chanlinks,	//number of channel links
		chanchems,	//number of channel soils (sediments)
		linknum,	//link number of a link
		channodes,	//number of nodes in a link
		nodenum,	//node number of a node
		nstack,		//number of sediment layers at a node
		layernum,	//layer number of a layer
		ichem;		//loop index for chemical number

	float
		conc;		//chemical concentration (ug/g = mg/kg)

	//write message to screen
	printf("\n\n*******************************************************\n");
	printf(	   "*                                                     *\n");
	printf(	   "*   Reading Sediment Chemical Initial Condition File  *\n");
	printf(	   "*                                                     *\n");
	printf(	   "*******************************************************\n\n\n");

	//Set the sediment chemical file pointer to null
	sedimentchemicfile_fp = NULL;

	//Open the sediment chemical file for reading
	sedimentchemicfile_fp=fopen(sedimentchemicfile,"r");

	//if the sediment chemical file pointer is null, abort execution
	if(sedimentchemicfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Channel File: %s \n",sedimentchemicfile);

		//Write message to screen
		printf("Error! Can't open Channel File: %s \n",sedimentchemicfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for sediment chemical file to file
	fprintf(echofile_fp, "\n\n\n  Sediment Chemical Initial Condition File  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, sedimentchemicfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(sedimentchemicfile_fp, "%s %d %s %d",	//read
		varname,		//dummy
		&chanlinks,		//number of channel links (local))
		varname,		//dummy
		&chanchems);	//number of channel chemicals (local)

	//Error check: chanlinks must equal nlinks...
	//
	//If number of links does not equal the global value from the link file, abort...
	if(chanlinks != nlinks)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nSediment Chemical Initial Condition File Error:\n");
		fprintf(echofile_fp,       "  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		//Write error message to screen
		printf("Sediment Chemical Initial Condition File Error:\n");
		printf("  chanlinks = %5d   nlinks = %5d\n", chanlinks, nlinks);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel links

	//Error check: chanchem must equal nchem...
	//
	//If number of chemicals does not equal the global value from the input file, abort...
	if(chanchems != nchems)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nSediment Chemical Initial Condition File Error:\n");
		fprintf(echofile_fp,       "  chanchems = %5d   nchems = %5d\n", chanchems, nchems);

		//Write error message to screen
		printf("Sediment Chemical Initial Condition File Error:\n");
		printf("  chanchems = %5d   nchems = %5d\n", chanchems, nchems);

		exit(EXIT_FAILURE);					//abort

	}	//end check of number of channel solids

	//Write second label for sediment chemical distribution file to file
	fprintf(echofile_fp, "\nLink  Node  Layer  Chemical  Concentration (mg/kg");
	fprintf(echofile_fp, "\n----  ----  -----  --------  --------------------\n\n");

	//Loop over number of links
	for(i=1; i<=nlinks; i++)
	{
		//Record 3
		fscanf(sedimentchemicfile_fp, "%s %d %s %d",	//read
			varname,		//dummy
			&linknum,		//link number (known from loop counter, read as dummy)
			varname,		//dummy
			&channodes);	//number of channel links (local)

		//Two more error checks...
		//  1. data must be in link order (1 to nlinks); and
		//  2. channodes must equal nnodes for each link.
		//
		//The channel file data must be in sequential order from 1 to nlinks.
		//
		//If link number read from file does not match the link index (i), abort...
		if(linknum != i)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nSediment Chemical Initial Condition File Error:\n");
			fprintf(echofile_fp,       "  link read = %5d   link expected = %5d\n",
				chanlinks, i);

			//Write error message to screen
			printf("Sediment Chemical Initial Condition File Error:\n");
			printf("  link read = %5d   link expected = %5d\n",
				chanlinks, i);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//If number of nodes does not equal the global value from the node file, abort...
		if(channodes != nnodes[i])
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nSediment Chemical Initial Condition File Error:\n");
			fprintf(echofile_fp,       "  link = %5d   channodes = %5d   nnodes = %5d\n",
				i, channodes, nnodes[i]);

			//Write error message to screen
			printf("Sediment Chemical Initial Condition File Error:\n");
			printf("  link = %5d   channodes = %5d   nnodes = %5d\n",
				i, channodes, nnodes[i]);

			exit(EXIT_FAILURE);					//abort

		}	//end check of number of channel nodes

		//Loop of number of nodes in current link
		for(j=1; j<=nnodes[i]; j++)
		{
			//Record 4
			fscanf(sedimentchemicfile_fp, "%s %d %s %d",		//read
				varname,			//dummy
				&nodenum,			//node number
				varname,			//dummy
				&nstack);			//number of sediment stack elements

			//If number of layers does not equal the global value from the stack file, abort...
			if(nstack != nstackch0[i][j])
			{
				//Write error message to file
				fprintf(echofile_fp, "\n\n\nSediment Chemical Initial Condition File Error:\n");
				fprintf(echofile_fp,       "  link = %d   node = %d   nstack = %d   nstack0 = %d\n",
					i, j, nstack, nstackch0[i][j]);

				//Write error message to screen
				printf("Sediment Chemical Initial Condition File Error:\n");
				printf("  link = %d   node = %d   nstack = %d   nstack0 = %d\n",
					i, j, nstack, nstackch0[i][j]);

				exit(EXIT_FAILURE);					//abort

			}	//end check of number of channel nodes

			//loop over number of layers in reverse order
			for(k=nstackch0[i][j]; k>=1; k--)
			{
				//Record 5a
				fscanf(sedimentchemicfile_fp, "%s %d",		//read
					varname,		//dummy
					&layernum);		//chemical concentration (ug/g = mg/kg)

				//Loop of number of chemicals
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//Record 5b
					fscanf(sedimentchemicfile_fp, "%s %f",		//read
						varname,		//dummy
						&conc);			//chemical concentration (ug/g = mg/kg)

					//Echo sediment chemical distribution to file
					fprintf(echofile_fp, "%4d  %4d  %5d  %8d  %20.4f\n",
						i, j, k, ichem, conc);

					//Note:  Concentration values are input as ug/g.  These values
					//       are converted from ug/g to g/m3 in InitializeChemical
					//       (which occurs after solids grain size distributions
					//       are converted to concentrations in InitializeSolids).
					//
					//store concentrations as ug/g (mg/kg)
					cchemch[ichem][i][j][k] = conc;

				}	//end loop over number of chemicals

			}	//end loop over layers

		}	//end loop over number of nodes

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over number of links

	//Close Sediment Chemical Initial Condition File
	fclose(sedimentchemicfile_fp);

//End of Function: Return to ReadDataGroupD
}
