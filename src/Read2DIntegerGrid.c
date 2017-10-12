/*---------------------------------------------------------------------
C-  Function:   Read2DIntegerGrid.c
C-
C-  Purpose/    Read2DInteger Grid is called at the start of the
C-  Methods:    simulation to read initial condition integer (int)
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
C-  Called by:  ReadRestart
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       03-JUN-2008
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

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void Read2DIntegerGrid(char name[], int **gridvalue)
{
	//local variable declarations/definitions
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

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
		printf("Can't open local Grid Input File : %s \n",gridfile);
		exit(EXIT_FAILURE);	//abort
	}
/*
	//Write label for grid file  to file
	fprintf(echofile_fp, "\n\n\n  Restart Grid File  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~\n");
*/
	//Record 1
	fgets(header, MAXHEADERSIZE, gridfile_fp);	//read header
/*
	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);
*/
	//Record 2
	fscanf(gridfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
/*
	//If number of grid rows, grid columns, or cell size do not equal global values, abort...
	if(gridrows != nrows || gridcols != ncols || cellsize != dx)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nGrid File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Grid File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort

	}	//end check of grid dimensions

	//Echo grid characteristics to file
	fprintf(echofile_fp, "\nSoil Layer Thickness Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);
*/
	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(gridfile_fp, "%d", &gridvalue[i][j]);	//read integer value from grid
/*
			//Echo value in overland cell to file
			fprintf(echofile_fp, "  %5d", gridvalue[i][j]);
*/
		}	//end loop over grid columns
/*
		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");
*/
	}	//end loop over grid rows

	//Close local file pointer
	fclose(gridfile_fp);

	//deallocate memory for local variables
	free(gridfile);

//End of function: Return to ReadRestart
}
