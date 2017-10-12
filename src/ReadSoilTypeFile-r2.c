/*---------------------------------------------------------------------
C-  Function:   ReadSoilTypeFile
C-  
C-  Purpose/    Reads the soil type classification file that defines
C-  Methods:    the soil type for each active cell (in the overland
C-              plane) within the spatial domain of the simulation.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      imask
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:    Mark Velleux
C-
C-  Date:       26-JUL-2004
C-
C-  Revised:  
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void ReadSoilTypeFile(int ilayer)
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n*****************************\n");
	printf(	   "*                           *\n");
	printf(	   "*   Reading Soil Type File  *\n");
	printf(	   "*                           *\n");
	printf(	   "*****************************\n\n\n");

	//Set the soil file pointer to null
	soiltypefile_fp = NULL;

	//Open the soil file for reading
	soiltypefile_fp = fopen(soiltypefile,"r");

	//if the soil file pointer is null, abort execution
	if(soiltypefile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Soil File: %s\n",soiltypefile);

		//Write message to screen
		printf("Error! Can't open Soil File: %s\n",soiltypefile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for soil file to file
	fprintf(echofile_fp, "\n\n\n  Soil File: Grid Cell Soil Classification  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, soiltypefile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(soiltypefile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nSoil File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Soil File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo soil characteristics to file
	fprintf(echofile_fp, "\nSoil Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n\n\n", nodatavalue);

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(soiltypefile_fp, "%d", &soiltype[i][j][ilayer]);	//read soil type

			//Echo soil type to file
			fprintf(echofile_fp, "  %5d", soiltype[i][j][ilayer]);

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Soil File
	fclose(soiltypefile_fp);

//End of function: Return to ReadDataGroupB
}
