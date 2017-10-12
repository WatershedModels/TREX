/*---------------------------------------------------------------------
C-  Function:	ReadInitialSnowFile.c
C-
C-	Purpose/	ReadInitialSnowFile is called to read the initial
C-  Methods:    snow depth (as snow water equivalence, SWE) file that
C-              specifies the initial SWE of each active cell of the
C-              within the spatial domain of the simulation.  Note
C-              that snow (swe) depth values are only applied to the
C-              overland plane.
C-
C-  Inputs:		None
C-
C-  Outputs:	sweov[][]
C-
C-  Controls:	None
C-
C-  Calls:		None
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:	Mark Velleux
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		26-FEB-2007
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

void ReadInitialSnowFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n********************************************\n");
	printf(	   "*                                          *\n");
	printf(	   "*   Reading Initial Snow Depth (SWE) File  *\n");
	printf(	   "*                                          *\n");
	printf(	   "********************************************\n\n\n");

	//Set the initial snow depth (swe) file pointer to null
	snowfile_fp = NULL;

	//Open the initial snow depth (swe) file for reading
	snowfile_fp=fopen(snowfile,"r");

	//if the initial snow depth (swe) file pointer is null, abort execution
	if(snowfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Initial Snow Depth File : %s \n", snowfile);

		//Write message to screen
		printf("Error! Can't open Initial Snow Depth File : %s \n", snowfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for initial snow depth (swe) file to file
	fprintf(echofile_fp, "\n\n\n  Initial Snow Depth File: Grid Cell Snow Depth as SWE  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, snowfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(snowfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
		varname,		//dummy
		&gridcols,		//number of columns in grid
		varname,		//dummy
		&gridrows,		//number of rows in grid
		varname,		//dummy
		&xllcorner,		//x location of grid lower left corner (m) (GIS projection)
		varname,		//dummy
		&yllcorner,		//y location projection of grid lower left corner (m) (GIS projection)
		varname,		//dummy
		&cellsize,		//length of grid cell (m) (this means dx must equal dy)
		varname,		//dummy
		&nodatavalue);	//no data value (null value)

	//If number of grid rows, grid columns, or cell size do not equal global values, abort...
	if(gridrows != nrows || gridcols != ncols || cellsize != dx)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nInitial Snow Depth File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Initial Snow Depth File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n\n\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo snow depth characteristics to file
	fprintf(echofile_fp, "\nSnow Depth (as Snow Water Equivalence) Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Allocate memory for snow depth (swe) (overland)
	sweov = (float **)malloc((gridrows+1) * sizeof(float *));		//allocate memory

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate remaining memory for snow depth (swe) (overland)
		sweov[i] = (float *)malloc((gridcols+1) * sizeof(float));		//allocate memory

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(snowfile_fp, "%f", &sweov[i][j]);	//read snow depth (swe) value

			//Echo snow depth (swe) to file
			fprintf(echofile_fp, "  %8.2f", sweov[i][j]);

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Initial Snow Depth File
	fclose(snowfile_fp);

//End of function: Return to ReadDataGroupB
}
