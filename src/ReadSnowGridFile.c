/*----------------------------------------------------------------------
C-  Function:	ReadSnowGridFile.c                                              
C-
C-	Purpose/	Read snowfall (swe) grid files for snowopt = 3.
C-  Methods:    Specification details are in Data Group B
C-
C-	Inputs:		snowgridfileroot, snowgridcount (passed arguments)
C-              snowconvunits, snowconvtime, snowscale (globals)
C-
C-	Outputs:	grossswerate[][], grossswedepth[][], grossswevol[][],
C-              minsweintensity, maxsweintensity
C-
C-	Controls:   simtime, timesnowgrid, snowgridfreq
C-
C-	Calls:		Min, Max
C-
C-	Called by:	Snowfall
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-	Date:		12-FEB-2009
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

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void ReadSnowGridFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize,	//length of grid cell (m) (this means dx must equal dy)
		snowrate;	//grid value of snowfall (swe) intensity for a cell

	//write message to screen
	printf("\n\n***************************************\n");
	printf(	   "*                                     *\n");
	printf(	   "*      Reading Snowfall Grid File     *\n");
	printf(	   "*                                     *\n");
	printf(	   "*   Simulation Time = %10.5f hrs  *\n", simtime);
	printf(	   "*   Snow Grid File = %5d            *\n", snowgridcount);
	printf(	   "*                                     *\n");
	printf(	   "***************************************\n\n\n");

	//Print snow grid file root name and grid count extension to string
	//to generate a name the form "snowgridfileroot.snowgridcount"
	//
	//jfe note this is unprotected behavior if snowgridfileroot and
	//snowgridcount exceed string length of snowgridfile....
	//we could eventually add a check for this
	sprintf(snowgridfile,"%s.%d",snowgridfileroot,snowgridcount);

	//Set the snowfall grid file pointer to null
	snowgridfile_fp = NULL;

	//Open the snowfall grid file for reading
	snowgridfile_fp=fopen(snowgridfile,"r");

	//if the snowfall grid file pointer is null, abort execution
	if(snowgridfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Snowfall Grid File : %s \n",snowgridfile);

		//Write message to screen
		printf("Can't open Snowfall Grid File : %s \n",snowgridfile);
		exit(EXIT_FAILURE);	//abort
	}

//	//Write label for snowfall grid file to file
//	fprintf(echofile_fp, "\n\n\n  Snow Grid File: Snowfall (SWE) Intensities  \n");
//	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, snowgridfile_fp);	//read header

//	//Echo header to file
//	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(snowgridfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nSnowfall (SWE) Grid File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Snowfall (SWE) Grid File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n\n\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort

	}	//end check of grid dimensions

//	//Echo snowfall (swe) characteristics to file
//	fprintf(echofile_fp, "\nSnowfall Grid Characteristics:\n");
//	fprintf(echofile_fp, "   Grid Rows = %5d\n", gridrows);
//	fprintf(echofile_fp, "   Grid Columns = %5d\n", gridcols);
//	fprintf(echofile_fp, "   Cell size = %10.2f (m)\n", cellsize);
//	fprintf(echofile_fp, "   No Data Value = %6d\n", nodatavalue);

	//Loop over number of rows
	for(i=1; i<=gridrows; i++)
	{
		//Loop over number of columns
		for(j=1; j<=gridcols; j++)
		{
			//Record 3
			fscanf(snowgridfile_fp, "%f", &snowrate);	//read gross snowfall (swe) rate

			//if the cell is in the domain
			if(imask[i][j] != nodatavalue)
			{
				//compute gross snowfall (swe) rate (m/s) (apply units conversions and scale factors)
				grossswerate[i][j] = snowrate
					* snowconvunits * snowconvtime * snowscale;

				//Save the minimum snowfall (swe) intensity value (m/s)
				minsweintensity = Min(minsweintensity,grossswerate[i][j]);

				//Save the maximum snowfall (swe) intensity value (m/s)
				maxsweintensity = Max(maxsweintensity,grossswerate[i][j]);

				//Compute the cumulative depth of snowfall (swe) for each cell (m)
				grossswedepth[i][j] = grossswedepth[i][j]
					+ grossswerate[i][j] * dt[idt];

				//Compute the cumulative gross snowfall (swe) volume entering a cell (m3)
				grossswevol[i][j] = grossswevol[i][j]
					+ grossswerate[i][j] * w * w * dt[idt];

			}  //end if cell is in domain (imask != nodata value)

		}	//end loop over grid columns

	}	//end loop over grid rows

	//Close Snowfall Grid File
	fclose(snowgridfile_fp);

//End of function: Return to Snowfall
}
