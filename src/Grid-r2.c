/*----------------------------------------------------------------------
C-  Function:	Grid.c
C-
C-	Purpose/    Write output at each grid cell at specified time gdt[idt]
C-	Methods:	in an individual file indexed by gridcount.
C-
C-
C-	Inputs:		name, gridvalue, conversion, gridcount
C-
C-	Outputs:	grid file at time = timeprintgrid
C-
C-	Controls:   None
C-
C-	Calls:		None
C-
C-	Called by:	WriteGrids
C-
C-	Created:	Rosalia Rojas-Sanchez
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (assumed date)
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux (CSU)
C-
C-	Date:		24-DEC-2003
C-
C-	Revisions:	Documentation, rewrite, reorganization
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
C---------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void Grid(char name[], float **gridvalue, float conversion, int gridcount)
{
	//Local variable declarations/definitions
	char
		*gridfile;	//file name for grid including extension

	//Declare local grid file pointer;
	FILE *gridfile_fp;

	//Allocate memory for simulation error file name
	gridfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Print input name file and extension to local string gridfile
	//this is of the form "name.gridcount"
	//
	//jfe note this is unprotected behavior if name and gridcount
	//exceed string length of gridfile....
	//we could eventually add a check for this
	sprintf(gridfile,"%s.%d",name,gridcount);

	//Set the local grid file pointer to null
	gridfile_fp = NULL;

	//open the grid file for writing
	gridfile_fp = fopen(gridfile, "w");

	//if the local grid file pointer is null, abort execution
	if(gridfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "\n\nError!  Can't create local Grid Output File : %s \n",gridfile);

		//Write message to screen
		printf("Error! Can't create local Grid Output File : %s \n",gridfile);
		exit(EXIT_FAILURE);	//abort
	}

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
				fprintf(gridfile_fp,"%e ",gridvalue[i][j]*conversion);
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

//End of function: Return to WriteGrids
}
