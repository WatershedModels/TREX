/*---------------------------------------------------------------------
C-  Function:   ReadSoilStackFile
C-
C-  Purpose/    ReadSoilStackFile is called to read the
C-  Methods:    soil stack grid that defines the number of
C-              vertical elements in the soil stack for each
C-              active cell (in the overland plane) within
C-              the spatial domain of the simulation.
C-
C-  Inputs:     soilstackfile
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupC
C-
C-  Created:    Mark Velleux
C-
C-  Date:       15-MAR-2004
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

void ReadSoilStackFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols,	//number of columns in grid
		vstack;		//number of vertical elements in a stack

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n******************************\n");
	printf(	   "*                            *\n");
	printf(	   "*   Reading Soil Stack File  *\n");
	printf(	   "*                            *\n");
	printf(	   "******************************\n\n\n");

	//Set the soil stack file pointer to null
	soilstackfile_fp = NULL;

	//Open the soil stack file for reading
	soilstackfile_fp=fopen(soilstackfile,"r");

	//if the soil stack file pointer is null, abort execution
	if(soilstackfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Soil Stack File : %s \n", soilstackfile);

		//Write message to screen
		printf("Error! Can't open Soil Stack File : %s \n", soilstackfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for soil stack file to file
	fprintf(echofile_fp, "\n\n\n  Soil Stack File: Number of Vertical Elements  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, soilstackfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(soilstackfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nSoil Stack File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Soil Stack File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort

	}	//end check of grid dimensions

	//Echo soil stack characteristics to file
	fprintf(echofile_fp, "\nSoil Stack Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n\n\n", nodatavalue);

	//Allocate memory for initial number of soil stack elements overland
	nstackov0 = (int **)malloc((gridrows+1) * sizeof(int *));		//allocate memory

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate remaining memory for initial number of soil stack elements overland
		nstackov0[i] = (int *)malloc((gridcols+1) * sizeof(int));		//allocate memory

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
//mlv:  John, we need to go back over all the auxillary grid file read routines
//      and check for consistency about how the grid are read with respect to
//      null values, use of imask as a control, and definition of nodatavalue.
//      In general, nodatavalue is -9999 but for imask it is zero...

			//Record 3
			fscanf(soilstackfile_fp, "%d", &vstack);	//read number of vertical elements

			//if the cell is not null
			if(imask[i][j] > 0)
			{
				nstackov0[i][j] = vstack;

				//If the number of stack elements < 1 (nstackov0[][] < 1), abort...
				if(nstackov0[i][j] < 1)
				{
					//Write error message to file
					fprintf(echofile_fp, "\n\n\nSoil Stack File Error:\n");
					fprintf(echofile_fp,       "  Row = %d   Col = %d   nstackov0 = %d   Minimum Value = 1\n",
						i, j, nstackov0[i][j]);

					//Write error message to screen
					printf("Soil Stack File Error:\n");
					printf("  Row = %d   Col = %d   nstackov0 = %d   Minimum Value = 1\n",
						i, j, nstackov0[i][j]);

					exit(EXIT_FAILURE);					//abort

				}	//end if nstackov0 < 1

				//If the number of stack elements (nstackov0[][]) > maxstackov, abort...
				if(nstackov0[i][j] > maxstackov)
				{
					//Write error message to file
					fprintf(echofile_fp, "\n\n\nSoil Stack File Error:\n");
					fprintf(echofile_fp,       "  Row = %d   Col = %d   nstackov0 = %d   Maximum Value = %d\n",
						i, j, nstackov0[i][j], maxstackov);

					//Write error message to screen
					printf("Soil Stack File Error:\n");
					printf("  Row = %d   Col = %d   nstackov0 = %d   Maximum Value = %d\n",
						i, j, nstackov0[i][j], maxstackov);

					exit(EXIT_FAILURE);					//abort

				}	//end if nstackov0 > maxstackov

			}	//end if imask > 0

			//Echo initial number of soil stack elements overland to file
			fprintf(echofile_fp, "  %5d", vstack);

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Soil Stack File
	fclose(soilstackfile_fp);

//End of function: Return to ReadDataGroupB
}
