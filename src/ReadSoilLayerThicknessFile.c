/*---------------------------------------------------------------------
C-  Function:	ReadSoilLayerThicknessFile.c
C-
C-  Purpose/    Reads the thickness of each layer in the soil stack
C-  Methods:    for the overland plane at time zero.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    hlayerov0[][][] (at time zero)
C-
C-  Controls:   imask[][], nstackov0[][]
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupC
C-
C-  Created:    Mark Velleux (CSU)
C-              John England (USBR)
C-
C-  Date:       19-APR-2004
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

//trex global variable declarations for solids transport
#include "trex_solids_declarations.h"

void ReadSoilLayerThicknessFile(int ilayer)
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize,	//length of grid cell (m) (this means dx must equal dy)
		htemp;		//local variable to store layer thickness (m)

	//write message to screen
	printf("\n\n****************************************\n");
	printf(	   "*                                      *\n");
	printf(	   "*   Reading Soil Layer Thickness File  *\n");
	printf(	   "*                                      *\n");
	printf(	   "****************************************\n\n\n");

	//Set the soil layer thickness file pointer to null
	soillayerthicknessfile_fp = NULL;

	//Open the soil layer thickness file for reading
	soillayerthicknessfile_fp = fopen(soillayerthicknessfile,"r");

	//if the soil layer thickness file pointer is null, abort execution
	if(soillayerthicknessfile_fp == NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Soil Layer Thickness file: %s\n", soillayerthicknessfile);

		//Write message to screen
		printf("Error! Can't open Soil Layer Thickness file: %s\n", soillayerthicknessfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for soil layer thickness file  to file
	fprintf(echofile_fp, "\n\n\n  Soil Layer Thickness File for Layer: %3d  \n",
		ilayer);
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, soillayerthicknessfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(soillayerthicknessfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nSoil Layer Thickness File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Soil Layer Thickness File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort

	}	//end check of grid dimensions

	//Echo soil layer thickness characteristics to file
	fprintf(echofile_fp, "\nSoil Layer Thickness Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(soillayerthicknessfile_fp, "%f", &htemp);	//read initial solids concentration value

			//Echo initial solids in overland cell to file
			fprintf(echofile_fp, "  %10.4f", htemp);

			//if the cell is in the domain and the layer is occupied
			if(imask[i][j] > 0 && ilayer <= nstackov0[i][j])
			{
				//assign layer thickness to global variable
				hlayerov0[i][j][ilayer] = htemp;

			}	//end if imask > 0

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close soil layer thickness file
	fclose(soillayerthicknessfile_fp);

//End of function: Return to ReadDataGroupB
}
