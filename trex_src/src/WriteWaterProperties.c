/*---------------------------------------------------------------------
C-  Function:   WriteWaterProperties.c
C-
C-  Purpose/    WriteWaterProperties is called at the end of the
C-  Methods:    simulation to write initial condition channel properties
C-              for surface water needed to restart simulations for
C-              storms in sequence.
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
C-  Called by:  WriteRestart
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       03-JUN-2008
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

void WriteWaterProperties(char name[])
{
	//local variable declarations/definitions
	int
		isolid,			//loop index for solids type
		ichem;			//loop index for chemical type

	char
		*gridfile;	//file name for grid including extension

	//Declare local grid file pointer;
	FILE *gridfile_fp;

	//Allocate memory for simulation error file name
	gridfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Print restart file name file and extension to local string gridfile
	sprintf(gridfile,"%s",name);

	//Set the local grid file pointer to null
	gridfile_fp = NULL;

	//open the grid file for writing
	gridfile_fp = fopen(gridfile, "w");

	//if the local grid file pointer is null, abort execution
	if(gridfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't create local Grid Output File : %s \n",gridfile);

		//Write message to screen
		printf("Error! Can't create local Grid Output File : %s \n",gridfile);
		exit(EXIT_FAILURE);	//abort
	}

	//print header text
	fprintf(gridfile_fp, "%s", header);

	//loop over links
	for(i=1;i<=nlinks;i++)
	{
		//print link info to file
		fprintf(gridfile_fp, "%s %d  %s %d\n",
			"Link", i, "NumNodes", nnodes[i]);

		//loop over nodes
		for(j=1;j<=nnodes[i];j++)
		{
			//print label and value for node id to file
			fprintf(gridfile_fp, "%s %d\n",
				"  Node", j);

			//print label and value for node water depth to file
			fprintf(gridfile_fp, "%s %f\n",
				"    Hch", hch[i][j]);

			//print label for node solids concentrations
			fprintf(gridfile_fp, "%s",
				"    Csedch");

			//loop over solids
			for(isolid=1;isolid<=nsolids;isolid++)
			{
				//print node solids concentrations
				fprintf(gridfile_fp, " %f", csedch[isolid][i][j][0]);

			}	//end loop over solids

			//print end of line for solids concentrations
			fprintf(gridfile_fp, "\n");

			//print label for node chemical concentrations
			fprintf(gridfile_fp, "%s",
				"    Cchemch");

			//loop over chemicals
			for(ichem=1;ichem<=nchems;ichem++)
			{
				//print node chemical concentrations
				fprintf(gridfile_fp, " %f", cchemch[ichem][i][j][0]);

			}	//end loop over chemicals

			//print end of line for chemical concentrations
			fprintf(gridfile_fp, "\n");

		}	//end loop over nodes

	}	//end loop over links

	//Close local file pointer
	fclose(gridfile_fp);

	//deallocate memory for local variables
	free(gridfile);

//End of function: Return to WriteRestart
}
