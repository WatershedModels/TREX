/*---------------------------------------------------------------------
C-  Function:   ReadElevationFile.c
C-
C-  Purpose/    ReadElevationFile is called to read the elevation
C-  Methods:    file that specifies the elevation of each active cell
C-              (in the overland plane) within the spatial domain of
C-              the simulation.
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
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah NJ  07430
C-
C-  Date:       24-JAN-2007
C-
C-  Revisions:  Removed #include "trex_solids_declarations.h"
C-
C-  Revised:    Mark Velleux
C-
C-  Date:       18-MAR-2009
C-
C-  Revisions:  Added error trap for mask and elevation misalignment 
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

void ReadElevationFile()
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
	printf(	   "*   Reading Elevation File  *\n");
	printf(	   "*                           *\n");
	printf(	   "*****************************\n\n\n");

	//Set the elevation file pointer to null
	elevationfile_fp = NULL;

	//Open the elevation file for reading
	elevationfile_fp=fopen(elevationfile,"r");

	//if the elevation file pointer is null, abort execution
	if(elevationfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Elevation File : %s \n",elevationfile);

		//Write message to screen
		printf("Error! Can't open Elevation File : %s \n",elevationfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for elevation file to file
	fprintf(echofile_fp, "\n\n\n  Elevation File: Initial Overland Elevations  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, elevationfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(elevationfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nElevation File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Elevation File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n\n\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo elevation characteristics to file
	fprintf(echofile_fp, "\nElevation Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Allocate memory for overland elevation
	elevationov0 = (float **)malloc((gridrows+1) * sizeof(float *));	//initial elevation (m) (at time t = tstart)
	elevationov = (float **)malloc((gridrows+1) * sizeof(float *));		//present elevation (m) (at time t)

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate remaining memory for overland elevation
		elevationov0[i] = (float *)malloc((gridcols+1) * sizeof(float));	//initial elevation (m) (at time t = tstart)
		elevationov[i] = (float *)malloc((gridcols+1) * sizeof(float));		//present elevation (m) (at time t)

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(elevationfile_fp, "%f", &elevationov0[i][j]);	//read initial elevation value

			//Echo elevation to file
			fprintf(echofile_fp, "  %8.2f", elevationov0[i][j]);

			//set present overland elevation to initial elevation (m)
			elevationov[i][j] = elevationov0[i][j];

			//Look for possible misalignment with mask file...
			//
			//Note: A "misalignment" occurs when mask[][] is active and
			//      elevation[][] is "no data"...
			//
			//if the cell imask value is "active" (i.e. imask[][] > 0)
			//  and the elevation value is "nodata"
			if(imask[i][j] > 0 && elevationov[i][j] == nodatavalue)
			{
				//Abort due to misalignment...
				//
				//Write error message to file
				fprintf(echofile_fp, "\n\n\nElevation Misalignment Error:\n");
				fprintf(echofile_fp,       "  Location: row = %5d   col = %5d\n", i, j);
				fprintf(echofile_fp,       "    imask[%d][%d] = %5d\n", i, j, imask[i][j]);
				fprintf(echofile_fp,       "    elevationov[%d][%d] = %f\n", i, j, elevationov[i][j]);

				//Write error message to screen
				printf("Elevation Misalignment Error:\n");
				printf("  Location: row = %5d   col = %5d\n", i, j);
				printf("    imask[%d][%d] = %5d\n", i, j, imask[i][j]);
				printf("    elevationov[%d][%d] = %f\n", i, j, elevationov[i][j]);

			}	//end if imask[][] > 0 and elevationov [][] = "nodata"

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Elevation File
	fclose(elevationfile_fp);

//End of function: Return to ReadDataGroupB
}
