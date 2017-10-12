/*---------------------------------------------------------------------
C-  Function:   ReadOverlandFpocFile.c
C-
C-  Purpose\    ReadOverlandFpocFile is called to read the cell values
C-  Methods:    values for overland particle foc for each cell in the
C-              the spatial domain.
C-
C-  Inputs:     isolid, scale
C-
C-  Outputs:    fpocov[][][][]
C-
C-  Controls:   imask[][]
C-
C-  Calls:      None
C-
C-  Called by:  ReadDataGroupE
C-
C-  Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-				
C-	Date:		07-SEP-2004
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
C---------------------------------------------------------------------*/

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

void ReadOverlandFpocFile(int isolid, int ilayer, float scale)
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize;	//length of grid cell (m) (this means dx must equal dy)

	float
		fpocvalue;	//value of fpoc

	//write message to screen
	printf("\n\n*********************************\n");
	printf(	   "*                               *\n");
	printf(	   "*   Reading Overland Fpoc File  *\n");
	printf(	   "*                               *\n");
	printf(	   "*********************************\n\n\n");

	//Set the environmental property file pointer to null
	envpropfile_fp = NULL;

	//Open the environmental property file for reading
	envpropfile_fp=fopen(envpropfile,"r");

	//if the environmental property file pointer is null, abort execution
	if(envpropfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Environmental Property File : %s \n",envpropfile);

		//Write message to screen
		printf("Error! Can't open Environmental Property File : %s \n",envpropfile);
		exit(EXIT_FAILURE);	//abort
	}

	//Write label for environmental property file to file
	fprintf(echofile_fp, "\n\n\n  Particle Fpoc File: solid = %d  \n", isolid);
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, envpropfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(envpropfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nOverland Fpoc File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Overland Fpoc File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n\n\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort
	}	//end check of grid dimensions

	//Echo property characteristics to file
	fprintf(echofile_fp, "\nOverland Fpoc Characteristics: solid = %d\n", isolid);
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
			fscanf(envpropfile_fp, "%f", &fpocvalue);	//read property value

			//Echo property value to file
			fprintf(echofile_fp, "  %8.2f", fpocvalue);

			//if the cell is in the domain
			if(imask[i][j] > 0)
			{
				//Apply scale factor and store in global array
				fpocov[isolid][i][j][ilayer] = fpocvalue * scale;

			}	//end if imask[][] > 0

		}	//end loop over grid columns

		//Start a new line for the next row of data in the echo file 
		fprintf(echofile_fp, "\n");

	}	//end loop over grid rows

	//Close Environmental Property File
	fclose(envpropfile_fp);

//End of function: Return to ReadDataGroupE
}
