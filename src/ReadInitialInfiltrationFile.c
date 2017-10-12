/*---------------------------------------------------------------------
C-  Function:	ReadInitialInfiltrationFile.c
C-
C-  Purpose/    ReadInitial InfiltrationFile is called to read the
C-  Methods:    initial depth of water infiltrated specified at the
C-              start of the simulation for each cell (in the overland
C-              plane) within the spatial domain of the simulation.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    infiltrationdepth[][]
C-
C-  Controls:   imask
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:    Mark Velleux (CSU)
C-
C-  Date:       20-AUG-2003
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void ReadInitialInfiltrationFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n**********************************************\n");
	printf(	   "*                                            *\n");
	printf(	   "*   Reading Initial Infiltration Depth File  *\n");
	printf(	   "*                                            *\n");
	printf(	   "**********************************************\n\n\n");

	//Set the initial infiltration depth file pointer to null
	infiltrationfile_fp = NULL;

	//Open the initial infiltration depth file for reading
	infiltrationfile_fp=fopen(infiltrationfile,"r");

	//if the initial infiltration depth file pointer is null, abort execution
	if(infiltrationfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Initial infiltration depth File : %s \n", infiltrationfile);

		//Write message to screen
		printf("Error! Can't open Initial infiltration depth File : %s \n", infiltrationfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for initial infiltration depth file to file
	fprintf(echofile_fp, "\n\n\n  Initial Infiltration Depth File: Grid Cell Infiltration Depths  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, infiltrationfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(infiltrationfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nInitial Infiltration Depth File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Initial Infiltration Depth File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo initial infiltration depth to file
	fprintf(echofile_fp, "\nInitial Infiltration Depth Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Allocate memory for infiltration depth
	infiltrationdepth = (float **)malloc((gridrows+1) * sizeof(float *));		//allocate memory

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate remaining memory for infiltration depth (h)
		infiltrationdepth[i] = (float *)malloc((gridcols+1) * sizeof(float));		//allocate memory

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(infiltrationfile_fp, "%f", &infiltrationdepth[i][j]);	//read initial infiltration depth value

			//Echo initial infiltration depth to file
			fprintf(echofile_fp, "  %10.4f", infiltrationdepth[i][j]);

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Initial Infiltration Depth File
	fclose(infiltrationfile_fp);

//End of function: Return to ReadDataGroupB
}
