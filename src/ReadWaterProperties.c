/*---------------------------------------------------------------------
C-  Function:   ReadWaterProperties.c
C-
C-  Purpose/    ReadWaterProperties is called at the start of the
C-  Methods:    simulation to read initial condition channel properties
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
C-  Called by:  ReadRestart
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

void ReadWaterProperties(char name[])
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

	//open the grid file for reading
	gridfile_fp = fopen(gridfile, "r");

	//if the local grid file pointer is null, abort execution
	if(gridfile_fp == NULL)
	{
		//Write message to screen
		printf("Can't create local Grid Output File : %s \n",gridfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Record 1
	fgets(header, MAXHEADERSIZE, gridfile_fp);	//read header

	//loop over links
	for(i=1;i<=nlinks;i++)
	{
		//Record 2
		fscanf(gridfile_fp, "%s %s  %s %s",		//read
			varname,							//dummy
			varname,							//link number (dummy)
			varname,							//dummy
			varname);							//number of nodes (dummy)

		//loop over nodes
		for(j=1;j<=nnodes[i];j++)
		{
			//Record 3
			fscanf(gridfile_fp, "%s %s",		//read
				varname,						//dummy
				varname);						//node number (dummy)

			//Record 4
			fscanf(gridfile_fp, "%s %f",		//read
				varname,						//dummy
				&hch[i][j]);					//node water depth (m)

			//Record 5 (Part 1)
			fscanf(gridfile_fp, "%s", varname);		//dummy

			//loop over solids
			for(isolid=1;isolid<=nsolids;isolid++)
			{
				//Record 5 (Part 2)
				fscanf(gridfile_fp, "%f", &csedch[isolid][i][j][0]);	//read node solids concentration (g/m3)

			}	//end loop over solids

			//Record 6 (Part 1)
			fscanf(gridfile_fp, "%s", varname);		//dummy

			//loop over chemicals
			for(ichem=1;ichem<=nchems;ichem++)
			{
				//Record 6 (Part 2)
				fscanf(gridfile_fp, "%f", &cchemch[ichem][i][j][0]);	//read node chemical concentration (g/m3)

			}	//end loop over chemicals

		}	//end loop over nodes

	}	//end loop over links

	//Close local file pointer
	fclose(gridfile_fp);

	//deallocate memory for local variables
	free(gridfile);

//End of function: Return to ReadRestart
}
