/*----------------------------------------------------------------------
C-  Function:	ReadRainGridFile.c                                              
C-
C-	Purpose/	Read rainfall grid files for rainopt = 5.
C-  Methods:    Specification details are in Data Group B
C-
C-	Inputs:		raingridfileroot, raingridcount (passed arguments)
C-              rainconvunits, rainconvtime, rainscale (globals)
C-
C-	Outputs:	grossrainrate[][], grossraindepth[][], grossrainvol[][],
C-              minrainintensity, maxrainintensity
C-
C-	Controls:   simtime, timeraingrid, raingridfreq
C-
C-	Calls:		Min, Max
C-
C-	Called by:	Rainfall
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-	            John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		27-JUL-2006
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-	Date:		16-DEC-2006
C-
C-	Revisions:	Integrated code to compute gross rainfall depth
C-              and volume from rainfall (precipitation) rate.
C-              Added code for snow hydrology.
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

void ReadRainGridFile()
{
	//Local Variable Declarations
	int
		gridrows,	//number of rows in grid
		gridcols;	//number of columns in grid

	float
		cellsize,	//length of grid cell (m) (this means dx must equal dy)
		rainrate;	//grid value of rainfall intensity for a cell

	//write message to screen
	printf("\n\n***************************************\n");
	printf(	   "*                                     *\n");
	printf(	   "*      Reading Rainfall Grid File     *\n");
	printf(	   "*                                     *\n");
	printf(	   "*   Simulation Time = %10.5f hrs  *\n", simtime);
	printf(	   "*   Rain Grid File = %5d            *\n", raingridcount);
	printf(	   "*                                     *\n");
	printf(	   "***************************************\n\n\n");

	//Print rain grid file root name and grid count extension to string
	//to generate a name the form "raingridfileroot.raingridcount"
	//
	//jfe note this is unprotected behavior if raingridfileroot and
	//raingridcount exceed string length of raingridfile....
	//we could eventually add a check for this
	sprintf(raingridfile,"%s.%d",raingridfileroot,raingridcount);

	//Set the rainfall grid file pointer to null
	raingridfile_fp = NULL;

	//Open the rainfall grid file for reading
	raingridfile_fp=fopen(raingridfile,"r");

	//if the rainfall grid file pointer is null, abort execution
	if(raingridfile_fp==NULL)
	{
		//Write message to file
		fprintf(echofile_fp, "Error! Can't open Rainfall Grid File : %s \n",raingridfile);

		//Write message to screen
		printf("Error! Can't open Rainfall Grid File : %s \n",raingridfile);
		exit(EXIT_FAILURE);	//abort
	}

//	//Write label for rainfall grid file to file
//	fprintf(echofile_fp, "\n\n\n  Rain Grid File: Rainfall Intensities  \n");
//	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, raingridfile_fp);	//read header

//	//Echo header to file
//	fprintf(echofile_fp, "\n%s\n", header);

	//Record 2
	fscanf(raingridfile_fp, "%s %d %s %d %s %f %s %f %s %f %s %d",	//read
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
		fprintf(echofile_fp, "\n\n\nRainfall Grid File Error:\n");
		fprintf(echofile_fp,       "  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		fprintf(echofile_fp,       "  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		fprintf(echofile_fp,       "  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n", dx, dy, cellsize);

		//Write error message to screen
		printf("Rainfall Grid File Error:\n");
		printf("  nrows = %5d   grid rows = %5d\n", nrows, gridrows);
		printf("  ncols = %5d   grid cols = %5d\n", ncols, gridcols);
		printf("  dx = %12.4f   dy = %12.4f   cell size = %12.4f\n\n\n", dx, dy, cellsize);

		exit(EXIT_FAILURE);					//abort

	}	//end check of grid dimensions

//	//Echo elevation characteristics to file
//	fprintf(echofile_fp, "\nRainfall Grid Characteristics:\n");
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
			fscanf(raingridfile_fp, "%f", &rainrate);	//read gross rainfall rate

			//if the cell is in the domain
			if(imask[i][j] != nodatavalue)
			{
				//compute gross rainfall rate (m/s) (apply units conversions and scale factors)
				grossrainrate[i][j] = rainrate
					* rainconvunits * rainconvtime * rainscale;

				//if snow hydrology is simulated...
				//
				//Note: Only snowopt = 1 applies.  Other snowopt values are
				//      handled in Snowfall.
				//
				//if snowopt = 1
				if(snowopt == 1)
				{
					//Assign precipitation to rain or snow based on temperature
					//
					//Note: The precipitation is already assigned to grossrainrate
					//      If the cell air temperature is less than than or equal
					//      to zero, then the precipitation (rainfall) is assigned
					//      to grossswerate and grossrainrate is reset to zero.
					//
					//if the cell air temperature <= freezing (0 Celsius)
					if(airtemp[i][j] <= 0.0)
					{
						//set the gross snowfall (swe) rate (m/s)
						grossswerate[i][j] = grossrainrate[i][j];

						//reset the gross rainfall rate to zero
						grossrainrate[i][j] = 0.0;
					}
					else	//else 	airtemp[][] > snowtemp
					{
						//set the gross snowfall (swe) rate to zero
						grossswerate[i][j] = 0.0;

					}	//end if airtemp[][] <= 0.0

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

				}	//end if snowopt = 1

				//Note: If snowopt = 1 and the air temperature is below freezing,
				//      then the gross rainfall rate is zero...
				//
				//Save the minimum rainfall intensity value (m/s)
				minrainintensity = Min(minrainintensity,grossrainrate[i][j]);

				//Save the maximum rainfall intensity value (m/s)
				maxrainintensity = Max(maxrainintensity,grossrainrate[i][j]);

				//Compute the cumulative depth of rainfall for each cell (m)
				grossraindepth[i][j] = grossraindepth[i][j]
					+ grossrainrate[i][j] * dt[idt];

				//Compute the cumulative gross rainfall volume entering a cell (m3)
				grossrainvol[i][j] = grossrainvol[i][j]
					+ grossrainrate[i][j] * w * w * dt[idt];

			}  //end if cell is in domain (imask != nodata value)

		}	//end loop over grid columns

	}	//end loop over grid rows

	//Close Rainfall Grid File
	fclose(raingridfile_fp);

//End of function: Return to Rainfall
}
