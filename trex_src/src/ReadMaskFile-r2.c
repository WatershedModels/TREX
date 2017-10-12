/*---------------------------------------------------------------------
C-  Function:   ReadMaskFile.c
C-
C-  Purpose/    ReadMaskFile is called to read the mask file (x-y
C-  Methods:    grid) that defines the extent of the simulation
C-              spatial domain.
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
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-  Date:       17-JUL-2003
C-
C-  Revised:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-  Date:       30-AUG-2005
C-
C-  Revisions:  Added code to count number of active cells in the
C-              model grid.  Use cell count to store row, column
C-              locations for each cell for code optimization.
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

extern void ReadMaskFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols,	//number of columns in grid
		maxcells,	//maximum number of cells in the grid
		icell,		//index for active grid cell number
		*row,		//local array for active grid cell row number
		*col;		//local array for active grid cell row number

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n************************\n");
	printf(	   "*                      *\n");
	printf(	   "*   Reading Mask File  *\n");
	printf(	   "*                      *\n");
	printf(	   "************************\n\n\n");

	//Set the mask file pointer to null
	maskfile_fp = NULL;

	//Open the mask file for reading
	maskfile_fp=fopen(maskfile,"r");

	//if the mask file pointer is null, abort execution
	if(maskfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Mask File : %s \n",maskfile);

		//Write message to screen
		printf("Error! Can't open Mask File : %s \n",maskfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for mask file to file
	fprintf(echofile_fp, "\n\n\n  Mask File: Delineation of Spatial Domain  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, maskfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(maskfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nMask File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Mask File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort

	}	//end check of grid dimensions

	//Echo mask characteristics to file
	fprintf(echofile_fp, "\nMask Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n\n\n", nodatavalue);

	//Allocate memory for mask
	imask = (int **)malloc((gridrows+1) * sizeof(int *));		//allocate memory

	//Note:  The number of active cells in the domain (imask > nodata)
	//       is not known until the mask is read.  Two locals arrays
	//       are used to store the row and column locations of each
	//       cell.  These arrays must be dimensioned for the maximum
	//       possible number of cells.  After the mask is read and
	//       the number of active cells is known, the data from the
	//       two local arrays is transferred to the global cell row
	//       (cellrow[]) and cell column (cellcol[]) arrays in order
	//       to minimize memory usage.
	//
	//compute the maximum number of cells in the domain
	maxcells = gridrows * gridcols;

	//Allocate memory for local grid cell references
	row = (int *)malloc((maxcells+1) * sizeof(int));		//allocate memory
	col = (int *)malloc((maxcells+1) * sizeof(int));		//allocate memory

	//initialize number of active cells to zero
	ncells = 0;

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate remaining memory for mask
		imask[i] = (int *)malloc((gridcols+1) * sizeof(int));		//allocate memory

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(maskfile_fp, "%d", &imask[i][j]);	//read mask value

			//Echo mask value to file
			fprintf(echofile_fp, "  %5d", imask[i][j]);

			//if the cell is in the active domain (imask != nodata)
			if(imask[i][j] != nodatavalue)
			{
				//increment the number of active grid cells
				ncells = ncells + 1;

				//store the row and column locations of this cell (local)
				row[ncells] = i;
				col[ncells] = j;

			}	//end if imask[][] != nodatavalue

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Echo Summary of mask characteristics to file
	fprintf(echofile_fp,"\n\n  Summary of Watershed Characteristics  ");
	fprintf(echofile_fp,  "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	fprintf(echofile_fp,"Total number of cells in rectangular domain: %5d\n\n", maxcells);
	fprintf(echofile_fp,"Number of active grid cells in watershed: %5d\n\n", ncells);
	fprintf(echofile_fp,"Watershed Area (km2): %.3f\n", ncells*w*w/1000/1000);

	//Allocate memory for global grid cell references
	cellrow = (int *)malloc((ncells+1) * sizeof(int *));		//allocate memory
	cellcol = (int *)malloc((ncells+1) * sizeof(int *));		//allocate memory

	//Transfer data from local to global cell references arrays...
	//
	//loop over active cells
	for(icell=1; icell<=ncells; icell++)
	{
		//store local cell values in global cell row and column arrays
		cellrow[icell] = row[icell];
		cellcol[icell] = col[icell];

	}	//end loop over active cells

	//deallocate memory for local variables 
	free(row);
	free(col);

	//Close Mask File
	fclose(maskfile_fp);

//End of function: Return to ReadDataGroupB
}
