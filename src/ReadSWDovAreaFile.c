/*---------------------------------------------------------------------
C-  Function:	ReadSWDovAreaFile.c
C-
C-  Purpose/    Reads the surface area associated with an overland
C-  Methods:    distributed solids load.
C-
C-
C-  Inputs:     isolid, iload
C-
C-  Outputs:    nswdovcells[isolid][iload],
C-              sdwovrow[isolid][iloads][icell],
C-              sdwovcol[isolid][iloads][icell],
C-              swdovarea[isolid][iload][row][col]
C-
C-  Controls:   imask[][]
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupC
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       29-APR-2008
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

void ReadSWDovAreaFile(int isolid, int iload)
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
		cellsize,	//length of grid cell (m) (this means dx must equal dy)
		atemp,		//local variable to store area (m2)
		sumarea;	//local variable to compute the overall sum of surface areas (m2)


	//write message to screen
	printf("\n\n**************************************************\n");
	printf(	   "*                                                *\n");
	printf(	   "*   Reading Overland Distributed Load Area File  *\n");
	printf(	   "*                                                *\n");
	printf(	   "**************************************************\n\n\n");

	//Set the overland distributed solids load area file pointer to null
	swdovareafile_fp = NULL;

	//Open the overland distributed solids load area file for reading
	swdovareafile_fp = fopen(swdovareafile,"r");

	//if the overland distributed solids load area file pointer is null, abort execution
	if(swdovareafile_fp == NULL)
	{
		printf("Can't open Overland Distributed Solids Load Area file: %s\n",
			swdovareafile);	//Write message to screen
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for overland distributed solids load area file to file
	fprintf(echofile_fp, "\n\n\n  Overland Distributed Solids Load Area File  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	fprintf(echofile_fp, "\n\n    Solid: %3d   Load: %3d\n\n",
		isolid, iload);

	//Record 1
	fgets(header, MAXHEADERSIZE, swdovareafile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(swdovareafile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nOverland Distributed Solids Load Area File Error:\n");
		fprintf(echofile_fp,       "  isolid = %5d   iload = %5d\n", isolid, iload);
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Overland Distributed Solids Load Area File Error:\n");
		printf("  isolid = %5d   iload = %5d\n", isolid, iload);
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort

	}	//end check of grid dimensions

	//Echo overland distributed solids load area characteristics to file
	fprintf(echofile_fp, "\nDistributed Load Area Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Note:  Memory for [isolid] and [iload] dimensions of the swdovarea
	//       array was allocated in ReadDataGroupC.
	//
	//Allocate memory for distributed load surface areas
	swdovarea[isolid][iload] = (float **)malloc((nrows+1) * sizeof(float **));		//allocate memory

	//compute the maximum number of cells in the domain
	maxcells = gridrows * gridcols;

	//Allocate memory for local grid cell references
	row = (int *)malloc((maxcells+1) * sizeof(int));		//allocate memory
	col = (int *)malloc((maxcells+1) * sizeof(int));		//allocate memory

	//initialize number of overland distributed solids source cells to zero
	nswdovcells[isolid][iload] = 0;

	//initialize sum of surface areas to zero
	sumarea = 0.0;

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Allocate memory for distributed load surface areas
		swdovarea[isolid][iload][i] = (float *)malloc((ncols+1) * sizeof(float));	//allocate memory

		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(swdovareafile_fp, "%f", &atemp);	//read surface area for load (m2)

			//Echo surface area for load in overland cell to file
			fprintf(echofile_fp, "  %10.4f", atemp);

			//if the cell is in the domain and the area is greater than zero
			if(imask[i][j] > 0 && atemp > 0.0)
			{
				//increment the number of grid cells with distributed solids loads
				nswdovcells[isolid][iload] = nswdovcells[isolid][iload] + 1;

				//store the row and column locations of this cell (local)
				row[nswdovcells[isolid][iload]] = i;
				col[nswdovcells[isolid][iload]] = j;

				//assign area to global variable
				swdovarea[isolid][iload][i][j] = atemp;

				//compute sum of surface area (m2)
				sumarea = sumarea + atemp;

			}	//end if imask > 0

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Echo Summary of area characteristics to file
	fprintf(echofile_fp,"\n\n  Summary of Distributed Load Characteristics  ");
	fprintf(echofile_fp,  "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n\n");
	fprintf(echofile_fp,"Solids Type: %5d   Load Number: %5d\n\n", isolid, iload);
	fprintf(echofile_fp,"Number of grid cells with a distributed load: %5d\n",
		nswdovcells[isolid][iload]);
	fprintf(echofile_fp,"Overall surface area (m2): %f\n", sumarea);

	//Allocate memory for global load cell references...
	//
	//Note:  Initial memory for distributed load cell reference arrays (for the
	//       [isolid] and [iload] indices was allocated in ReadDataGroupC where
	//       those values were read.  At this point, only memory for the final
	//       array index needs to be allocated.
	//
	//Allocate remaining memory for global distributed load cell references
	swdovrow[isolid][iload] = (int *)malloc((nswdovcells[isolid][iload]+1) * sizeof(int));		//allocate memory
	swdovcol[isolid][iload] = (int *)malloc((nswdovcells[isolid][iload]+1) * sizeof(int));		//allocate memory

	//Transfer data from local to global cell references arrays...
	//
	//loop over active cells
	for(icell=1; icell<=nswdovcells[isolid][iload]; icell++)
	{
		//store local cell values in global cell row and column arrays
		swdovrow[isolid][iload][icell] = row[icell];
		swdovcol[isolid][iload][icell] = col[icell];

	}	//end loop over active cells

	//deallocate memory for local variables 
	free(row);
	free(col);

	//Close overland distributed solids load area file
	fclose(swdovareafile_fp);

//End of function: Return to ReadDataGroupC
}
