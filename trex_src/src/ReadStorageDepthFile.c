/*---------------------------------------------------------------------
C-  Function:   ReadStorageDepthFile.c
C-
C-  Purpose/    ReadStorageDepthFile is called to read the storage
C-  Methods:    depth file that specifies the storage depth of each
C-              active cells (in the overland plane) within the spatial
C-              domain of the simulation.
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
C-  Date:       23-JUL-2003
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

void ReadStorageDepthFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n*********************************\n");
	printf(	   "*                               *\n");
	printf(	   "*   Reading Storage Depth File  *\n");
	printf(	   "*                               *\n");
	printf(	   "*********************************\n\n\n");

	//Set the storage depth file pointer to null
	storagedepthfile_fp = NULL;

	//Open the storage depth file for reading
	storagedepthfile_fp=fopen(storagedepthfile,"r");

	//if the storage depth file pointer is null, abort execution
	if(storagedepthfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Storage Depth File : %s \n",storagedepthfile);

		//Write message to screen
		printf("Error! Can't open Storage Depth File : %s \n",storagedepthfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for storage depth file to file
	fprintf(echofile_fp, "\n\n\n  Storage Depth File: Grid Cell Depression Storage Depths  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, storagedepthfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(storagedepthfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nStorage Depth File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Storage Depth File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo storage depth characteristics to file
	fprintf(echofile_fp, "\nStorage Depth Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Allocate memory for storage depth
	storagedepth = (float **)malloc((gridrows+1) * sizeof(float *));		//allocate memory

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate remaining memory for storage depth
		storagedepth[i] = (float *)malloc((gridcols+1) * sizeof(float));		//allocate memory

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(storagedepthfile_fp, "%f", &storagedepth[i][j]);	//read storage depth value

			//Echo storage depth to file (use native units)
			fprintf(echofile_fp, "  %8.2f", storagedepth[i][j]);

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Storage Depth File
	fclose(storagedepthfile_fp);

//End of function: Return to ReadDataGroupB
}
