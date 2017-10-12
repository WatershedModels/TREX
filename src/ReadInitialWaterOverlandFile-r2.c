/*---------------------------------------------------------------------
C-  Function:	ReadInitialWaterOverlandFile.c
C-
C-  Purpose/    Reads the initial water depth for each cell in the
C-  Methods:    overland plane at time zero.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    hov[][] (at time zero)
C-
C-  Controls:   imask[][]
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:  03-SEP-2003  Mark Velleux
C-                         John England
C-
C-  Revised:  
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void ReadInitialWaterOverlandFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n************************************************\n");
	printf(	   "*                                              *\n");
	printf(	   "*   Reading Initial Water Depth Overland File  *\n");
	printf(	   "*                                              *\n");
	printf(	   "************************************************\n\n\n");

	//Set the initial water depth overland file pointer to null
	wateroverlandfile_fp = NULL;

	//Open the initial water depth overland file for reading
	wateroverlandfile_fp=fopen(wateroverlandfile,"r");

	//if the initial water depth overland file pointer is null, abort execution
	if(wateroverlandfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Initial Water Depth Overland File : %s \n", wateroverlandfile);

		//Write message to screen
		printf("Error! Can't open Initial Water Depth Overland File : %s \n", wateroverlandfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for initial water depth overland file to file
	fprintf(echofile_fp, "\n\n\n  Initial Water Depth Overland File: Grid Cell Water Depths  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, wateroverlandfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(wateroverlandfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nInitial Water Depth Overland File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Initial Water Depth Overland File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo initial water depths overland to file
	fprintf(echofile_fp, "\nInitial Water Depths Overland Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Allocate memory for water depth (hov)
	hov = (float **)malloc((gridrows+1) * sizeof(float *));		//allocate memory

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate remaining memory for water depth (hov)
		hov[i] = (float *)malloc((gridcols+1) * sizeof(float));		//allocate memory

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(wateroverlandfile_fp, "%f", &hov[i][j]);	//read initial water depth value

			//Echo initial water depth in overland cell to file
			fprintf(echofile_fp, "  %10.4f", hov[i][j]);

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Initial Water Depth Overland File
	fclose(wateroverlandfile_fp);

//End of function: Return to ReadDataGroupB
}
