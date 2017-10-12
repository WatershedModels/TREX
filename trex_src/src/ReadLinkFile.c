/*---------------------------------------------------------------------
C-  Function:   ReadLinkFile.c
C-
C-  Purpose/    ReadLinkFile is called to read the location (row and
C-  Methods:    column) where each link of the channel network occurs
C-              within the overland plane.
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

void ReadLinkFile()
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
	printf(	   "*   Reading Link File  *\n");
	printf(	   "*                      *\n");
	printf(	   "************************\n\n\n");

	//Set the link file pointer to null
	linkfile_fp = NULL;

	//Open the link file for reading
	linkfile_fp=fopen(linkfile,"r");

	//if the link file pointer is null, abort execution
	if(linkfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Link File : %s \n",linkfile);

		//Write message to screen
		printf("Error! Can't open Link File : %s \n",linkfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for link file to file
	fprintf(echofile_fp, "\n\n\n  Link File: Delineation of Channel Links  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, linkfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(linkfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nLink File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Link File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo link characteristics to file
	fprintf(echofile_fp, "\nLink Characteristics:\n");
	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
	fprintf(echofile_fp, "   No Data Value = %6d\n\n\n", nodatavalue);

	//Allocate memory for link
	link = (int **)malloc((gridrows+1) * sizeof(int *));		//allocate memory

	//Allocate remaining memory for link and initialize array
	//
	//Loop over number of columns (including zeroeth position)
	for(i=0; i<=gridrows; i++)
	{
		//Allocate remaining memory for link
		link[i] = (int *)malloc((gridcols+1) * sizeof(int));		//allocate memory

		//Loop over number of columns (including zeroeth position)
		for(j=0; j<=gridcols; j++)
		{
			//Initialize link[][] array
			link[i][j] = 0;

		}	//end lop over columns
	}	//end loop over rows

	//Populate link array with data read from link file
	//
	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(linkfile_fp, "%d", &link[i][j]);	//read link

			//Echo link to file
			fprintf(echofile_fp, "  %5d", link[i][j]);

			//Compute the total number of links in network (number of links is largest link number)
			if(imask[i][j] != nodatavalue) nlinks = (int)Max(nlinks,link[i][j]);

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Link File
	fclose(linkfile_fp);

//End of function: Return to ReadDataGroupB
}
