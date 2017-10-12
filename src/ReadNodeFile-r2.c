/*---------------------------------------------------------------------
C-  Function:   ReadNodeFile
C-
C-  Purpose/    ReadNodeFile is called to read the location
C-  Methods:    (row and column) of each node for each
C-              link of channel network within the overland
C-              plane.
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   link[][]
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupB
C-
C-  Created:    Mark Velleux
C-
C-  Date:       28-JUL-2004
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

void ReadNodeFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	//write message to screen
	printf("\n\n************************\n");
	printf(	   "*                      *\n");
	printf(	   "*   Reading Node File  *\n");
	printf(	   "*                      *\n");
	printf(	   "************************\n\n\n");

	//Set the node file pointer to null
	nodefile_fp = NULL;

	//Open the node file for reading
	nodefile_fp=fopen(nodefile,"r");

	//if the node file pointer is null, abort execution
	if(nodefile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Node File : %s \n",nodefile);

		//Write message to screen
		printf("Can't open Node File : %s \n",nodefile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for node file to file
	fprintf(echofile_fp, "\n\n\n  Node File: Delineation of Channel Nodes  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, nodefile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(nodefile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nNode File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Node File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo node characteristics to file
	fprintf(echofile_fp, "\nNode Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n\n\n", nodatavalue);

	//Allocate memory for node array
	node = (int **)malloc((gridrows+1) * sizeof(int *));	//allocate memory

	//Allocate remaining memory for node and initialize array
	//
	//Loop over number of columns (including zeroeth position)
	for(i=0; i<=gridrows; i++)
	{
		//Allocate remaining memory for node
		node[i] = (int *)malloc((gridcols+1) * sizeof(int));		//allocate memory

		//Loop over number of columns (including zeroeth position)
		for(j=0; j<=gridcols; j++)
		{
			//Initialize node[][] array
			node[i][j] = 0;

		}	//end lop over columns

	}	//end loop over rows

	//Allocate memory for number of nodes
	nnodes = (int *)malloc((nlinks+1) * sizeof(int));		//allocate memory

	//Initialize nnodes array
	//
	//Loop over links (including zeroeth position)
	for(i=0; i<=nlinks; i++)
	{
		//initialize the number of nodes in a link
		nnodes[i] = 0;

	}	//end loop over links

	//Populate node array with data read from node file
	//
	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(nodefile_fp, "%d", &node[i][j]);	//read node

			//Echo node to file
			fprintf(echofile_fp, "  %5d", node[i][j]);

			//Compute the total number of nodes for each link in network
			//(number of nodes is largest node number for that link)
			//
			//if the link number is greater than zero
			if(link[i][j] > 0)
			{
				//find the largest node number for this link
				nnodes[link[i][j]] = (int)Max(nnodes[link[i][j]],node[i][j]);

			}	//end if link[][] > 0

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Node File
	fclose(nodefile_fp);

//End of function: Return to ReadDataGroupB
}
