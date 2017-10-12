/*---------------------------------------------------------------------
C-  Function:   Write3DDoubleGrid.c
C-
C-  Purpose/    Write3DDoubleGrid is called at the end of the
C-  Methods:    simulation to write initial condition real (double)
C-              grid files needed to restart simulations for storms
C-              in sequence.
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

void Write3DDoubleGrid(char name[], double ***gridvalue, int ilayer)
{
	//local variable declarations/definitions
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

	//print ESRI ASCII grid format header for the grid of the form:
	//
	//		ncols         387
	//		nrows         147
	//		xllcorner     382985
	//		yllcorner     4342263
	//		cellsize      30
	//		NODATA_value  -9999

	//number of columns in grid
	fprintf(gridfile_fp,"ncols\t\t%d\n", ncols);

	//number of rows in grid
	fprintf(gridfile_fp,"nrows\t\t%d\n", nrows);

	//x location of grid lower left corner (m) (GIS projection)
	fprintf(gridfile_fp,"xllcorner\t%.2f\n", xllcorner);

	//y location projection of grid lower left corner (m) (GIS projection)
	fprintf(gridfile_fp,"yllcorner\t%.2f\n", yllcorner);

	//length of grid cell (m) (this means dx must equal dy)
	fprintf(gridfile_fp,"cellsize\t%.2f\n", w);

	//no data value (null value)
	fprintf(gridfile_fp,"NODATA_value\t%d\n", nodatavalue);

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//if the mask location is not null
			if(imask[i][j] != nodatavalue)
			{
				//write the grid value for that cell
				fprintf(gridfile_fp,"%e ",gridvalue[i][j][ilayer]);
			}
			else	//the mask location is null
			{
				//write nodata value
				fprintf(gridfile_fp,"%d ",nodatavalue);

			}	//end if imask[][] != nodata

		}	//end loop over cols

		//finish each line with a hard return
		fprintf(gridfile_fp,"\n");

	}	//end loop over rows

	//Close local file pointer
	fclose(gridfile_fp);

	//deallocate memory for local variables
	free(gridfile);

//End of function: Return to trex
}
