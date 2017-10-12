/*----------------------------------------------------------------------
C-  Function:   InitializeWater.c
C-
C-  Purpose/    Allocate memory for and initialize water variables
C-  Methods:    used in computations but not read from input files.
C-
C-  Inputs:     None
C-
C-  Outputs:    radaridpoint[][] for rainopt=3
C-              dadstormindex[][] for rainopt=4
C-
C-  Controls:   infopt, chnopt, rainopt
C-
C-  Calls:      None
C-
C-  Called by:  Initialize
C-
C-  Created:    Original Coding: Rosalia Rojas-Sanchez (and others)
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO 80523
C-
C-  Date:       19-JUN-2003 (last version)
C-
C-  Revised:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO  80225
C-
C-  Date:       08-JUL-2004
C-
C-  Revisions:  Complete rewrite.
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-	Date:       18-DEC-2006
C-
C-	Revisions:  Added initializations for snow hydrology
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO  80225
C-
C-  Date:       15-Aug-2007
C-
C-  Revisions:  Added initializations for rainopt = 2-4
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       29-APR-2008
C-
C-  Revisions:  Added flag to indicate when a flow reporting station
C-              coincides with an outlet.  Added overland distributed
C-              loads.
C-
C-  Revised:    John England, USBR
C-
C-  Date:       14-SEP-2008
C-
C-  Revisions:  Bug fixes and full testing for rainopt =  2-4
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

void InitializeWater()
{
	//local variable declarations/definitions
	int
		*stormcellcount,	//number of cells in each ellipse (rainopt = 4)
		cumstormcells,		//number of all cells in the storm within the watershed (rainopt = 4)
		distanceflag;		//interpolation distance flag (indicates if all radar gages
							//are outside of radius of current cell) (rainopt = 3)

	float
		x,				//UTM x coordinate of grid cell i,j (rainopt = 3 and 4)
		y,				//UTM y coordinate of grid cell i,j (rainopt = 3 and 4)
		xprimt,			//UTM stormorient-translated x coordinate of grid cell i,j (rainopt = 4)
		yprimt,			//UTM stormorient-translated y coordinate of grid cell i,j (rainopt = 4)
		xprimr,			//UTM stormorient-rotated x coordinate of grid cell i,j (rainopt = 4)
		yprimr,			//UTM stormorient-rotated y coordinate of grid cell i,j (rainopt = 4)
		distance,		//distance from cell i,j to gage (rainopt = 3)
		mindistance,	//minimum interpolation distance from cell i,j to gage (rainopt = 3)
		*longaxis,		//long axis of ellipse at rainarea[] (rainopt = 4)
		*shortaxis,		//short axis of ellipse at rainarea[] (rainopt = 4)
		ellipsevalue,	//value of ellipse eqn, determines if inside/outside area (rainopt = 4)
		pi,				//constant
		*areawtdad;		//area-weighted DAD cumulative depth

	//Initialize local constants
	pi = (float)(3.14159265358);
	cumstormcells	= 0;

	/*********************************************/
	/*        Initialize Global Counters         */
	/*********************************************/

	//Initialize iteration index used to control N-S or E-W routing in OverlandRoute
	iteration = 0;

	/**************************************************/
	/*        Initialize Total Volume Variables       */
	/**************************************************/

	initialwaterovvol = 0.0;	//initial water volume in overland plane
	finalwaterovvol = 0.0;	//final water volume in overland plane

	//if snowfall or snowmelt are simulated
	if(snowopt > 0 || meltopt > 0)
	{
		initialsweovvol = 0.0;	//initial snow (swe) volume in overland plane
		finalsweovvol = 0.0;	//final snow (swe) volume in overland plane

	}	//end if snowopt > 0 or meltopt > 0

	//Allocate memory for total volume by cell variables

	//initial water volume in an overland cell (m3)
	initialwaterov = (float **)malloc((nrows+1) * sizeof(float *));

	//final water volume in an overland cell (m3)
	finalwaterov = (float **)malloc((nrows+1) * sizeof(float *));

	//if snowfall or snowmelt are simulated
	if(snowopt > 0 || meltopt > 0)
	{
		//initial snow (swe) volume in an overland cell (m3)
		initialsweov =  (float **)malloc((nrows+1) * sizeof(float *));	

		//final snow (swe) volume in an overland cell (m3)
		finalsweov = (float **)malloc((nrows+1) * sizeof(float *));

	}	//end if snowopt > 0 or meltopt > 0

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate remaining memory for overland volume parameters
		//(values are initialized in ComputeInitialVolume)
		initialwaterov[i] = (float *)malloc((ncols+1) * sizeof(float));
		finalwaterov[i] = (float *)malloc((ncols+1) * sizeof(float));

		//if snowfall or snowmelt are simulated
		if(snowopt > 0 || meltopt > 0)
		{
			//Allocate remaining memory for snow volume parameters
			//(values are initialized in ComputeInitialVolume)
			initialsweov[i] =  (float *)malloc((ncols+1) * sizeof(float));	
			finalsweov[i] = (float *)malloc((ncols+1) * sizeof(float));

		}	//end if snowopt > 0 or meltopt > 0

	}  //end loop over rows

	//if channels are simulated
	if(chnopt == 1)
	{
		initialwaterchvol = 0.0;  //initial water volume in overland plane
		finalwaterchvol = 0.0;  //final water volume in overland plane

		//Allocate memory for total volume by node variables

		//initial water volume in a channel node (m3)
		initialwaterch = (float **)malloc((nlinks+1) * sizeof(float *));
		//final water volume in a channel node (m3)
		finalwaterch = (float **)malloc((nlinks+1) * sizeof(float *));

		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Allocate remaining memory for overland volume parameters
			//(values are initialized in ComputeInitialVolume)
			initialwaterch[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));
			finalwaterch[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));

		}  //end loop over links

	}  //end if chnopt == 1

	/**************************************************************************************/
	/*        Initialize Global Rainfall/Snowfall/Interception/Snowmelt Variables         */
	/**************************************************************************************/

	//Allocate memory for rainfall parameters
	grossrainrate = (float **)malloc((nrows+1) * sizeof(float *));  //gross rainfall rate (m/s)
	grossraindepth = (float **)malloc((nrows+1) * sizeof(float *));  //cumulative gross rainfall depth (m)
	grossrainvol = (float **)malloc((nrows+1) * sizeof(float *)); //gross rainfall volume (m3)
	netrainrate = (float **)malloc((nrows+1) * sizeof(float *));  //net rainfall rate (m/s)
	netrainvol = (float **)malloc((nrows+1) * sizeof(float *));  //net rainfall volume (m3)

	//rainfall intensity interpolated in time for gage [i]
	rfinterp = (float *)malloc((nrg+1) * sizeof(float));

	//if snowfall is simulated
	if(snowopt > 0)
	{
		//Allocate memory for snowfall (as swe) parameters
		grossswerate = (float **)malloc((nrows+1) * sizeof(float *));  //gross snowfall rate (as swe) (m/s)
		grossswedepth = (float **)malloc((nrows+1) * sizeof(float *));  //cumulative gross snowfall depth (as swe) (m)
		grossswevol = (float **)malloc((nrows+1) * sizeof(float *)); //gross snowfall volume (as swe) (m3)
		netswerate = (float **)malloc((nrows+1) * sizeof(float *));  //net snowfall rate (as swe) (m/s)
		netswevol = (float **)malloc((nrows+1) * sizeof(float *));  //net snowfall volume (as swe) (m3)

	}	//end if snowopt > 0

	//Allocate memory for interception parameters
	//remaining (unstatisfied) interception depth (m)
	interceptiondepth = (float **)malloc((nrows+1) * sizeof(float *));
	//cumulative interception volume (m3)
	interceptionvol = (float **)malloc((nrows+1) * sizeof(float *));

	//if snowmelt is simulated
	if(meltopt > 0)
	{
		//Allocate memory for snowmelt (as swe) parameters
		swemeltrate = (float **)malloc((nrows+1) * sizeof(float *));  //snowmelt rate (as swe) (m/s)
		swemeltdepth = (float **)malloc((nrows+1) * sizeof(float *));  //snowmelt depth (as swe) (m)
		swemeltvol = (float **)malloc((nrows+1) * sizeof(float *));  //snowmelt volume (as swe) (m3)

	}	//end if meltopt > 0

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate memory for rainfall parameters
		//(values are initialized in Rainfall and Interception)
		grossrainrate[i] = (float *)malloc((ncols+1) * sizeof(float));
		grossraindepth[i] = (float *)malloc((ncols+1) * sizeof(float));
		grossrainvol[i] = (float *)malloc((ncols+1) * sizeof(float));
		netrainrate[i] = (float *)malloc((ncols+1) * sizeof(float));
		netrainvol[i] = (float *)malloc((ncols+1) * sizeof(float));

		//if snowfall is simulated
		if(snowopt > 0)
		{
			//Allocate memory for snowfall (as swe) parameters
			grossswerate[i] = (float *)malloc((ncols+1) * sizeof(float));  //gross snowfall rate (as swe) (m/s)
			grossswedepth[i] = (float *)malloc((ncols+1) * sizeof(float));  //cumulative gross snowfall depth (as swe) (m)
			grossswevol[i] = (float *)malloc((ncols+1) * sizeof(float)); //gross snowfall volume (as swe) (m3)
			netswerate[i] = (float *)malloc((ncols+1) * sizeof(float));  //net snowfall rate (as swe) (m/s)
			netswevol[i] = (float *)malloc((ncols+1) * sizeof(float));  //net snowfall volume (as swe) (m3)

		}	//end if snowopt > 0

		//Allocate remaining memory for interception parameters
		interceptiondepth[i] = (float *)malloc((ncols+1) * sizeof(float));
		interceptionvol[i] = (float *)malloc((ncols+1) * sizeof(float));

		//if snowmelt is simulated
		if(meltopt > 0)
		{
			//Allocate memory for snowmelt (as swe) parameters
			swemeltrate[i] = (float *)malloc((ncols+1) * sizeof(float));  //snowmelt rate (as swe) (m/s)
			swemeltdepth[i] = (float *)malloc((ncols+1) * sizeof(float));  //snowmelt depth (as swe) (m)
			swemeltvol[i] = (float *)malloc((ncols+1) * sizeof(float));  //snowmelt volume (as swe) (m3)

		}	//end if meltopt > 0

		//Loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain
			if(imask[i][j] != nodatavalue)
			{
				//Set initial values for cumulative gross rainfall depth
				//these are used on right hand side of equation in Rainfall routine
				grossraindepth[i][j] = 0.0;

				//Initialize cumulative gross rainfall volume entering a cell (m3)
				grossrainvol[i][j] = 0.0;

				//Initialize cumulative net rainfall volume entering a cell (m3)
				netrainvol[i][j] = 0.0;

				//if snowfall is simulated
				if(snowopt > 0)
				{
					//Initialize gross swe depth (m)
					grossswedepth[i][j] = 0.0;  //cumulative gross snowfall depth (as swe) (m)

					//Initialize cumulative gross snowfall volume entering a cell (m3)
					grossswevol[i][j] = 0.0; //gross snowfall volume (as swe) (m3)

					//Initialize cumulative gross snowfall volume entering a cell (m3)
					netswerate[i][j] = 0.0;  //net snowfall rate (as swe) (m/s)

					//Initialize cumulative gross snowfall volume entering a cell (m3)
					netswevol[i][j] = 0.0;  //net snowfall volume (as swe) (m3)

				}	//end if snowopt > 0

				//Initialize interception parameters...
				//
				//Initialize interception depth (m) (input values in mm, convert to m)
				interceptiondepth[i][j] = (float)(interceptionclass[landuse[i][j]] * 0.001);

				//Initialize cumulative interception volume in a cell (m3)
				interceptionvol[i][j] = 0.0;

				//if snowmelt is simulated
				if(meltopt > 0)
				{
					//Initialize gross snowmelt depth (m)
					swemeltdepth[i][j] = 0.0;  //cumulative gross snowmelt depth (as swe) (m)

					//Initialize gross snowmelt volume (m)
					swemeltvol[i][j] = 0.0;  //cumulative gross snowmelt volume (as swe) (me)

				}	//end if meltopt > 0

			}  //end if imask[i][j] is not null (cell is is domain)

		}  //end loop over columns

	}  //end loop over rows

	//Initialize and determine nearest radar gage in watershed grid pointer
	//for radar rainfall option 3 using nearest neighbor interpolation
	//
	if(rainopt == 3)
	{
		//open the radarverify file in append mode (append to existing file)
		radarverifyfile_fp = fopen(radarverifyfile, "a");

		//Allocate memory for radar rainfall pointer
		//Location (row, col) of nearest neighbor radar "gage" for each overland cell
		radaridpoint = (int **)malloc((nrows+1) * sizeof(int *));

		//Initialize 0th array element for rainfall intensity interpolated in time
		//this is a hack to use an empty/unused memory location
		//to take care of partial-area radars over the watershed
		//or to take care of model domain cells inside the mask that are
		//outside the maximum radar interpolation distance
		rfinterp[0] = 0.0;

		//Initialize entire grid
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate remaining memory for radar rainfall pointer
			radaridpoint[i] = (int *)malloc((ncols+1) * sizeof(int ));

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//Initialize entire grid to nodata value regardless of location
				radaridpoint[i][j] = nodatavalue;

			}	//end loop over columns

		}	//end loop over rows

		//Write Grid to Radarverify File

		//Write label for radar rainfall pointer grid to radarverify file
		fprintf(radarverifyfile_fp, "\n\n\nRainfall Option 3 Radar Data\n");
		fprintf(radarverifyfile_fp, "Radar Rainfall Pointer Grid File: Rain Cell Locations\n");
		fprintf(radarverifyfile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		//Echo radar rainfall "standard grid" header to file
		fprintf(radarverifyfile_fp, "\nRadar Rainfall Pointer Grid Header Information:\n");
		fprintf(radarverifyfile_fp, "   Grid Rows = %5d\n", nrows);
		fprintf(radarverifyfile_fp, "   Grid Columns = %5d\n", ncols);
		fprintf(radarverifyfile_fp, "   Cell size = %10.2f (m)\n", w);
		fprintf(radarverifyfile_fp, "   Lower Left Corner X = %18.8f (m)\n", xllcorner);
		fprintf(radarverifyfile_fp, "   Lower Left Corner Y = %18.8f (m)\n", yllcorner);
		fprintf(radarverifyfile_fp, "   No Data Value = %6d\n", nodatavalue);

		//Determine Nearest Neighbor (restricted)
		//and set value of radaridpoint for those cells within the watershed
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//Raise the interpolation distance flag
					distanceflag = 1;

					//Translate grid location to UTM x,y coordinates
					//
					//This assumes an ESRI ASCII Grid format
					//and converts cell row and column (i,j)
					//to coordinate (x,y) located at grid cell center

					//x coordinate
					x = xllcorner + ((float)(j - 0.5)) * w;

					//y coordinate
					y = yllcorner + ((float)(nrows - i + 0.5)) * w;

					//set minimum distance
					//use local surrogate variable for radar cell spacing
					//so we do not overwrite original value
					mindistance = radarcellw;

					//Now perform nearest neighbor interpolation
					//
					//Loop over number of radar rain gages
					for(k=1; k<=nrg; k++)
					{
						//Compute distance between cell and rain gage
						distance = (float)(sqrt(pow((y-rgy[k]),2.0) + pow((x-rgx[k]),2.0)));

						//if distance <= user-entered minimum distance via radarcellw
						if(distance <= mindistance)
						{
							//Lower the distance flag
							distanceflag = 0;

							//set pointer value
							radaridpoint[i][j] = k;

							//set minimum distance
							mindistance = distance;

						}	//end if distance <= user-entered minimum distance

					}	//end loop over number of rain gages

					//if distance flag = 1, no radar gages were within the minimum distance
					if(distanceflag == 1)	//distance flag is raised
					{
						//set pointer (int) to zero,
						//this then sets rain rate to zero in Rainfall
						radaridpoint[i][j] = 0;

					}	//end if distance flag = 1

				}	//end if imask[i][j] is not null (cell is in domain)

			}	//end loop over columns

		}	//end loop over rows

		//print out entire generated radar pointer grid
		//
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//Echo design rain index grid value to file
				fprintf(radarverifyfile_fp, "  %7d", radaridpoint[i][j]);

			}	//end loop over columns

			//Start a new line for the next row of data in the radarverify file
			fprintf(radarverifyfile_fp, "\n");

		}	//end loop over rows

		//Close the Radarverify File
		fclose(radarverifyfile_fp);

	}	//End if rainopt ==3

	//Initialize and determine watershed grid pointer
	//for each rainfall time series for specific area
	//and D-A-D Storm rainfall option 4 using elliptical isohyet pattern
	//
	if(rainopt == 4)
	{
		//open the echo file in append mode (append to existing file)
		echofile_fp = fopen(echofile, "a");

		//open the DAD storm grid file (first and only time to write to file)
		dadstormgridfile_fp = fopen(dadstormgridfile, "w");

		//if the DAD storm grid file pointer is null, abort execution
		if(dadstormgridfile_fp==NULL)
		{
			//Write message to screen
			printf("Can't create D-A-D Storm Grid File:  %s\n",dadstormgridfile);
			exit(EXIT_FAILURE);	//abort

		}	//End if DAD storm grid file is null

		//Allocate memory for D-A-D storm rainfall pointer
		//Location (row, col) of rain "gage" within an areal ellipse
		dadstormindex = (int **)malloc((nrows+1) * sizeof(int *));

		//Initialize 0th array element for rainfall intensity interpolated in time
		//this is a hack to use an empty/unused memory location
		//to take care of partial-area elliptical storms over the watershed
		//or to take care of model domain cells inside the mask that are
		//outside the maximum radar interpolation distance
		rfinterp[0] = 0.0;

		//Initialize entire grid
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate remaining memory for DAD storm rainfall pointer
			dadstormindex[i] = (int *)malloc((ncols+1) * sizeof(int ));

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the watershed domain
				if(imask[i][j] !=nodatavalue)
				{
					//Here we initialize all pointer values inside the watershed mask to zero
					//this sets the interpolated rainrate to zero in Rainfall
					//then below we compute the pointer for each area
					dadstormindex[i][j] = 0;
				}
				else
				{
					//Initialize grid value to nodata value (outside mask)
					dadstormindex[i][j] = nodatavalue;

				}	//end if cell is in the domain

			}	//end loop over columns

		}	//end loop over rows

		//Determine parameters of ellipse and print to echo file
		//write header to echo file
		fprintf(echofile_fp, "\n\n\nRainfall Option 4 D-A-D Data Ellipse Parameters\n");
		fprintf(echofile_fp, "   Area        stormelong      long axis     short axis\n");
		fprintf(echofile_fp, "  (km^2)         'c' (m)        'a' (m)        'b' (m) \n");
		fprintf(echofile_fp, "~~~~~~~~~~~~  ~~~~~~~~~~~~~~ ~~~~~~~~~~~~~  ~~~~~~~~~~~~~\n");

		//Allocate memory for locals
		//long axis of ellipse at rainarea[]
		longaxis = (float *)malloc((nrg+1) * sizeof(float));
		//short axis of ellipse at rainarea[]
		shortaxis = (float *)malloc((nrg+1) * sizeof(float));

		//Loop over number of DAD storm areas (gages)
		for(k=1; k<=nrg; k++)
		{
			//determine ellipse long axis
			longaxis[k] = (float)(sqrt((stormelong*rainarea[k])/pi));

			//determine ellipse short axis
			shortaxis[k] = longaxis[k]/stormelong;

			//print to echo file
			fprintf(echofile_fp, " %8.1f %12.1f %16.2f %15.2f\n",rainarea[k]/1000000.0,
				stormelong, longaxis[k], shortaxis[k]);

		}	//end loop over number of DAD storm areas (gages)

		//Echo D-A-D storm rainfall "standard ESRI grid" header to file (example below)
		//		ncols         387
		//		nrows         147
		//		xllcorner     382985
		//		yllcorner     4342263
		//		cellsize      30
		//		NODATA_value  -9999

		//number of columns in grid
		fprintf(dadstormgridfile_fp,"ncols\t\t%d\n", ncols);

		//number of rows in grid
		fprintf(dadstormgridfile_fp,"nrows\t\t%d\n", nrows);

		//x location of grid lower left corner (m) (GIS projection)
		fprintf(dadstormgridfile_fp,"xllcorner\t%.2f\n", xllcorner);

		//y location projection of grid lower left corner (m) (GIS projection)
		fprintf(dadstormgridfile_fp,"yllcorner\t%.2f\n", yllcorner);

		//length of grid cell (m)
		fprintf(dadstormgridfile_fp,"cellsize\t%.2f\n", w);

		//no data value (null value)
		fprintf(dadstormgridfile_fp,"NODATA_value\t%d\n", nodatavalue);

		//Set the dadgridpointer values inside the watershed
		//using the user-entered nareas = nrg
		//by computing ellipse parameters for each area
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//Translate grid location to UTM x,y coordinates
					//
					//This assumes an ESRI ASCII Grid format
					//and converts cell row and column (i,j)
					//to coordinate (x,y) located at grid cell center

					//x coordinate
					x = xllcorner + ((float)(j - 0.5)) * w;

					//y coordinate
					y = yllcorner + ((float)(nrows - i + 0.5)) * w;

					//Translate x,y locations to ellipse origin
					//xprimt
					xprimt = x - raincenterx;

					//yprimt
					yprimt = y - raincentery;

					//Rotate coordinates based on stormorient
					//using equns 5&6 from Grossman (1984) p. 601
					//xprimr
					xprimr = (float)(xprimt*cos(stormorient) + yprimt*sin(stormorient));

					//yprimr
					yprimr = (float)(-xprimt*sin(stormorient) + yprimt*cos(stormorient));

					//Loop over the number of gages (areas) in reverse
					//this way we compute ellipse and pointer for the
					//outer-most (largest) area first, then overwrite with
					//inset smaller areas if they overlap

					//Loop over number of DAD storm areas (gages)
					for(k=nrg; k>=1; k--)
					{
						//storm is oriented horizontal along X as major axis
						//we rotate the x axis via stormorient,
						//so x is always the major axis
						//Determine ellipse value
						ellipsevalue = (float)(pow( xprimr,2)/pow(longaxis[k],2) +
							pow( yprimr,2)/pow(shortaxis[k],2));

						//if point is inside or on ellipse
						if(ellipsevalue <=1.0)
						{
							//Set grid pointer
							dadstormindex[i][j] = k;

							//Else value is zero = initialized

						}	//end if point is inside ellipse

					}	//end loop over number of DAD storm areas (gages)

				}	//end if imask[i][j] is not null (cell is is domain)

			}	//end loop over columns

		}	//end loop over rows

		//Now print out final grid to file for viewing/checking
		//Also determine areas within watershed of each pointer

		//Allocate memory for area-weighted DAD cumulative depth
		areawtdad = (float *)malloc((nrg+1) * sizeof(float));

		//Initialize basin average watershed Depth (inches)
		avgdepthwatershed = 0.0;

		//Allocate memory for local
		//number of cells in each ellipse
		stormcellcount = (int *)malloc((nrg+2) * sizeof(int));

		//Loop over number of DAD storm areas (gages), including zero
		for(k=0; k<=nrg; k++)
		{
			//initialize
			stormcellcount[k] = 0;

			//initialize area weight dad
			areawtdad[k] = 0.0;

		}	//end loop over number of DAD storm areas (gages)

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//write the grid value for that cell
				fprintf(dadstormgridfile_fp,"%d ",dadstormindex[i][j]);

				//Loop over number of DAD storm areas (gages), including zero
				for(k=0; k<=nrg; k++)
				{
					//count the number of cells in each ellipse
					if(dadstormindex[i][j] == k)
					{
						stormcellcount[k] = stormcellcount[k] +1;

					}	//end if cell is in this ellipse

				}	//end loop over number of DAD storm areas (gages)

			}	//end loop over columns

			//finish each line with a hard return
			fprintf(dadstormgridfile_fp,"\n");

		}	//end loop over rows

		//Print Storm Rainfall Areas for each ellipse to echo file
		//write header to echo file
		fprintf(echofile_fp, "\n\n  Rain Gage     No. Cells      Area (km^2)  %% Watershed \n");
		fprintf(echofile_fp, "~~~~~~~~~~~~  ~~~~~~~~~~~~~~ ~~~~~~~~~~~~~  ~~~~~~~~~~~~~\n");

		//Loop over number of DAD storm areas (gages), including zero
		for(k=0; k<=nrg; k++)
		{
			//print to echo file
			fprintf(echofile_fp, " %8d %12d %16.2f %15.3f\n",k, stormcellcount[k],
				stormcellcount[k]*w*w/1000000.0,stormcellcount[k]*100/(float)(ncells));

		}	//end loop over number of DAD storm areas (gages)

		//Determine basin-average depth
		//Loop over number of DAD storm areas (gages), including zero
		for(k=0; k<=nrg; k++)
		{
			//NOTE: average depth is in inches as we don't convert DAD depths
			//ASSUMED to be entered in inches from DAD tables

			//compute weighted average depth for each area
			areawtdad[k] = (float)(stormcellcount[k])*cumdaddepth[k]/(float)(ncells);

			//sum depths to obtain basin average
			avgdepthwatershed = avgdepthwatershed + areawtdad[k];

		}  //end loop over number of DAD storm areas (gages)


		//Determine the total rainfall area over the watershed
		//Loop over number of DAD storm areas (gages) - skip zero!
		for(k=1; k<=nrg; k++)
		{
			cumstormcells = cumstormcells + stormcellcount[k];
		}	//end loop over number of DAD storm areas (gages)

		//Print the total rainfall area over the watershed
		//and basin-average depth
		//print to echo file	  
		fprintf(echofile_fp, "\n  Basin-Average Storm Depth (inches)  within the watershed = %8.3f",
			avgdepthwatershed);

		//print to echo file
		fprintf(echofile_fp, "\n  Total Number of Rain Cells within the watershed = %8d",
			cumstormcells);
		//print to echo file
		fprintf(echofile_fp, "\n  Total Storm Area within the watershed = %8.1f km^2",
			(float)(cumstormcells*w*w/1000000.0) );

		//print to echo file
		fprintf(echofile_fp, "\n  Total number of Storm Cells within the watershed");
		fprintf(echofile_fp," including no rain = %d", cumstormcells + stormcellcount[0]);

		//jfe add percent area here:

		//print to echo file
		fprintf(echofile_fp, "\n  Percent of Watershed Covered by Storm Area = %6.3f",
			100.0*((float)(cumstormcells)/(float)((cumstormcells + stormcellcount[0]))) );

		//Close the Echo File
		fclose(echofile_fp);

		//Close the DAD Storm Grid File
		fclose(dadstormgridfile_fp);

	}	//End if rainopt == 4

	//If rainopt = 5 (rain grid files will be read)
	if(rainopt == 5)
	{
		//Allocate memory for rainfall grid file name
		raingridfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Initialize the rain grid counter
		//
		//if the simulation start time is greater than zero
		if(tstart > 0.0)
		{
			//set the rain grid count to zero
			raingridcount = (int)(tstart / raingridfreq);
		}
		else	//tstart equals zero
		{
			//set the rain grid count to zero
			raingridcount = 0;

		}	//end if tstart > 0.0

	}	//end if rainopt = 5

	/*****************************************************************************/
	/*        Initialize Global Infiltration/Transmission Loss Variables         */
	/*****************************************************************************/

	//if infiltration is simulated
	if(infopt > 0)
	{
		//Allocate memory for infiltration parameters
		//Note: infiltrationdepth is allocated in ReadInfiltrationFile....
		infiltrationrate = (float **)malloc((nrows+1) * sizeof(float *));	//infiltration rate (m/s)
		infiltrationvol = (float **)malloc((nrows+1) * sizeof(float *));	//infiltration volume (m3)

		//if the simulation type is hydrology only (ksim = 1)
		if(ksim == 1)
		{
			//set the maximum number of elements in the soil stack to 1
			maxstackov = 1;

			//Allocate memory for location (layer) in the overland soil stack
			nstackov = (int **)malloc((nrows+1) * sizeof(int *));	//soil stack layer

		}  //end if ksim = 1

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate remaining memory for infiltration parameters
			infiltrationrate[i] = (float *)malloc((ncols+1) * sizeof(float));
			infiltrationvol[i] = (float *)malloc((ncols+1) * sizeof(float));

			//if the simulation type is hydrology only (ksim = 1)
			if(ksim == 1)
			{
				//Allocate remaining memory for location (layer) in the overland soil stack
				nstackov[i] = (int *)malloc((ncols+1) * sizeof(int));	//soil stack layer

			}  //end if ksim = 1

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//Initialize cumulative infiltration volume (m3)
					infiltrationvol[i][j] = 0.0;

					//if the simulation type is hydrology only (ksim = 1)
					if(ksim == 1)
					{
						//Initialize overland soil stack layer (for ksim = 1, nstackov = 1)
						nstackov[i][j] = 1;

					}  //end if ksim = 1

				}  //end if imask[i][j] is not null (cell is is domain)

			}  //end loop over columns

		}  //end loop over rows

		//Initialize total sum of infiltration volumes
		totalinfiltvol = 0.0;

	}  //end if infopt > 0

	//if channels are simulated
	if(chnopt > 0)
	{
		//if channel transmission loss is simulated
		if(ctlopt > 0)
		{
			//Allocate memory for transmission loss parameters
			//Note: translossdepth is allocated in ReadTransmissionLossFile....
			translossrate = (float **)malloc((nlinks+1) * sizeof(float *));	//transmission loss rate (m/s)
			translossvol = (float **)malloc((nlinks+1) * sizeof(float *));	//transmission loss volume (m3)

			//if the simulation type is hydrology only (ksim = 1)
			if(ksim == 1)
			{
				//set the maximum number of elements in the sediment stack to 1
				maxstackch = 1;

				//Allocate memory for location (layer) in the channel sediment stack
				nstackch = (int **)malloc((nlinks+1) * sizeof(int *));	//sediment stack layer

			}	//end if ksim = 1

			//Loop over	number of links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate remaining memory	for	transmission loss parameters
				translossrate[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));
				translossvol[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));

				//if the simulation type is hydrology only (ksim = 1)
				if(ksim == 1)
				{
					//Allocate remaining memory	for	location (layer) in	the	channel sediment stack
					nstackch[i] = (int *)malloc((nnodes[i]+1) *	sizeof(int));	//sediment stack layer

				}	//end if ksim = 1

				//Loop over	number of nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Initialize cumulative	transmission loss volume (m3)
					translossvol[i][j] = 0.0;

					//if the simulation type is hydrology only (ksim = 1)
					if(ksim == 1)
					{
						//Initialize channel sediment stack layer (for ksim = 1, nstackch = 1)
						nstackch[i][j] = 1;

					}	//end if ksim = 1

				}  //end loop over nodes

			}  //end loop over links

			//Initialize total sum of transmission loss volumes
			totaltranslossvol = 0.0;

		}  //end if	ctlopt > 0

	}	//end if chnopt > 0

	/*****************************************************************/
	/*        Initialize Global Overland Transport Variables         */
	/*****************************************************************/

	//Allocate memory for overland flow parameters indexed by row and column (and direction)
	//new overland water depth (for next time t+dt) (m)
	hovnew = (float **)malloc((nrows+1) * sizeof(float *));
	//net overland flow (m3/s)
	dqov = (float **)malloc((nrows+1) * sizeof(float *));
	//gross overland flow into a cell (m3/s)
	dqovin = (float ***)malloc((nrows+1) * sizeof(float **));
	//gross overland flow out of a cell (m3/s)
	dqovout = (float ***)malloc((nrows+1) * sizeof(float **));
	//cumulative gross overland flow volume into a cell (m3)
	dqovinvol = (float ***)malloc((nrows+1) * sizeof(float **));
	//cumulative gross overland flow volume out of a cell (m3)
	dqovoutvol = (float ***)malloc((nrows+1) * sizeof(float **));
	//cumulative external flow volume entering a cell (m3)
	qwovvol = (float **)malloc((nrows+1) * sizeof(float *));
	//friction slope for overland flow from a cell (m3)
	sfov = (float ***)malloc((nrows+1) * sizeof(float **));

	//if snowfall or snowmelt are simulated
	if(snowopt > 0 || meltopt > 0)
	{
		//new snow depth (as swe) (for next time t+dt) (m)
		sweovnew = (float **)malloc((nrows+1) * sizeof(float *));

	}  //end if snowopt > 0 or meltopt > 0

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate additional/remaining memory for overland flow parameters
		hovnew[i] = (float *)malloc((ncols+1) * sizeof(float));
		dqov[i] = (float *)malloc((ncols+1) * sizeof(float));
		dqovin[i] = (float **)malloc((ncols+1) * sizeof(float *));
		dqovout[i] = (float **)malloc((ncols+1) * sizeof(float *));
		dqovinvol[i] = (float **)malloc((ncols+1) * sizeof(float *));
		dqovoutvol[i] = (float **)malloc((ncols+1) * sizeof(float *));
		qwovvol[i] = (float *)malloc((ncols+1) * sizeof(float));
		sfov[i] = (float **)malloc((ncols+1) * sizeof(float *));

		//if snowfall or snowmelt are simulated
		if(snowopt > 0 || meltopt > 0)
		{
			//new snow depth (as swe) (for next time t+dt) (m)
			sweovnew[i] = (float *)malloc((ncols+1) * sizeof(float));

		}  //end if snowopt > 0 or meltopt > 0

		//Loop over number of columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain
			if(imask[i][j] > 0)
			{
				//Initialize cumulative external overland flow volume array
				qwovvol[i][j] = 0.0;

				//Allocate remaining memory for gross overland flow parameters
				dqovin[i][j] = (float *)malloc(11 * sizeof(float));
				dqovout[i][j] = (float *)malloc(11 * sizeof(float));
				dqovinvol[i][j] = (float *)malloc(11 * sizeof(float));
				dqovoutvol[i][j] = (float *)malloc(11 * sizeof(float));
				sfov[i][j] = (float *)malloc(11 * sizeof(float));

				//loop over flow sources
				for(k=0; k<=10; k++)
				{
					//Initialize cumulative overland flow volumes
					dqovinvol[i][j][k] = 0.0;
					dqovoutvol[i][j][k] = 0.0;

					//initialize overland friction slope
					sfov[i][j][k] = 0.0;

				}  //end loop over flow sources

			}  //end if imask[i][j] is not nodatavalue (cell is is domain)

		}  //end loop over columns

	}  //end loop over rows

	//Initialize cumulative sum of external flow volume entering overland plane (m3)
	totalqwov = 0.0;

	/****************************************************************/
	/*        Initialize Global Channel Transport Variables         */
	/****************************************************************/

	//if channels are simulated
	if(chnopt == 1)
	{
		//Initialize scalars...
		//
		//Initialize maximum courant number for flow in channel network
		//mlv      maxcourant = 0.0;

		//Initialize channel network arrays indexed by link and node (and direction)
		//
		//Allocate memory for channel flow parameters

		//new channel water depth (for next time t+dt) (m)
		hchnew = (float **)malloc((nlinks+1) * sizeof(float *));
		//net nodal flow (for next time t+dt) (m3/s)
		dqch = (float **)malloc((nlinks+1) * sizeof(float *));
		//gross channel flow into a node (m3/s)
		dqchin = (float ***)malloc((nlinks+1) * sizeof(float **));
		//gross channel flow out of a node (m3/s)
		dqchout = (float ***)malloc((nlinks+1) * sizeof(float **));
		//cumulative gross channel flow volume into a node (m3)
		dqchinvol = (float ***)malloc((nlinks+1) * sizeof(float **));
		//cumulative gross channel flow volume out of a node (m3)
		dqchoutvol = (float ***)malloc((nlinks+1) * sizeof(float **));
		//cumulative external flow volume entering a node (m3)
		qwchvol = (float **)malloc((nlinks+1) * sizeof(float *));
		//link inflow from downstream boundary (m3/s)
		qinch = (float *)malloc((nlinks+1) * sizeof(float));
		//cumulative flow volume entering domain from a link (m3)
		qinchvol = (float *)malloc((nlinks+1) * sizeof(float));
		//link outflow to downstream boundary (m3/s)
		qoutch = (float *)malloc((nlinks+1) * sizeof(float));
		//cumulative flow volume leaving domain from a link (m3)
		qoutchvol = (float *)malloc((nlinks+1) * sizeof(float));
		//friction slope for channel flow from a node (m3)
		sfch = (float **)malloc((nlinks+1) * sizeof(float *));

		//Loop over number of links
		for(i=1; i<=nlinks; i++)
		{
			//Initialize link inflow volume (m3)
			qinchvol[i] = 0.0;

			//Initialize link outflow volume (m3)
			qoutchvol[i] = 0.0;

			//Allocate additional/remaining memory for channel flow parameters
			hchnew[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));
			dqch[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));
			dqchin[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));
			dqchout[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));
			dqchinvol[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));
			dqchoutvol[i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));
			qwchvol[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));
			sfch[i] = (float *)malloc((nnodes[i]+1) * sizeof(float));

			//Loop over number of nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Initialize cumulative external channel flow volume array
				qwchvol[i][j] = 0.0;

				//initialize channel friction slope
				sfch[i][j] = 0.0;

				//Allocate remaining memory for gross nodal inflow and outflow
				dqchin[i][j] = (float *)malloc(11 * sizeof(float));
				dqchout[i][j] = (float *)malloc(11 * sizeof(float));
				dqchinvol[i][j] = (float *)malloc(11 * sizeof(float));
				dqchoutvol[i][j] = (float *)malloc(11 * sizeof(float));

				//loop over flow sources
				for(k=0; k<=10; k++)
				{
					//Initialize cumulative channel flow volumes
					dqchinvol[i][j][k] = 0.0;
					dqchoutvol[i][j][k] = 0.0;

				}  //end loop over flow sources

			}  //end loop over nodes

		}  //end loop over links

		//Initialize cumulative sum of external flow volume entering channel network (m3)
		totalqwch = 0.0;

		//Initialize cumulative sum of flow volume entering channel network (m3)
		totalqinch = 0.0;

		//Initialize cumulative sum of flow volume exiting channel network (m3)
		totalqoutch = 0.0;

	}  //end if chnopt = 1

	/*************************************************************/
	/*        Initialize Global Reporting Station Arrays         */
	/*************************************************************/

	//Allocate memory for reported water discharges (export) time series

	//reported sum of overland and channel water discharge (units vary)
	qreportsum = (float *)malloc((nqreports+1) * sizeof(float));
	//reported overland water discharge (units vary)
	qreportov = (float *)malloc((nqreports+1) * sizeof(float));
	//reported channel water discharge (units vary)
	qreportch = (float *)malloc((nqreports+1) * sizeof(float));
	//discharge export conversion factor
	qconvert = (float *)malloc((nqreports+1) * sizeof(float));

	//flag indicating if a flow reporting station is also an outlet cell
	qrsoflag = (int *)malloc((nqreports+1) * sizeof(int));

	//Loop over number of flow reporting stations
	for(i=1; i<=nqreports; i++)
	{
		//Initialize flow reporting variables
		qreportsum[i] = 0.0;
		qreportov[i] = 0.0;
		qreportch[i] = 0.0;

		//loop over number of outlets
		for(j=1; j<=noutlets; j++)
		{
			//if flow reporting station location is also an outlet
			if(qreprow[i] == iout[j] && qrepcol[i] == jout[j])
			{
				//set flow reporting station outlet flag (store the outlet number)
				qrsoflag[i] = j;
			}

		}	//end loop over outlets

	}  //end loop over flow reporting stations

	/***************************************************/
	/*        Initialize Outlet Cell Variables         */
	/***************************************************/

	//Allocate memory for overland outlet cell flows

	//overland inflow entering domain from an outlet (m3/s)
	qinov = (float *)malloc((noutlets+1) * sizeof(float));
	//overland outflow exiting domain from an outlet (m3/s)
	qoutov = (float *)malloc((noutlets+1) * sizeof(float));
	//cumulative flow volume leaving outlet i (m3)
	qoutovvol = (float *)malloc((noutlets+1) * sizeof(float));
	//peak flow leaving outlet i (m3/s)
	qpeakov = (float *)malloc((noutlets+1) * sizeof(float));
	//time of peak flow leaving outlet i (hrs)
	tpeakov = (float *)malloc((noutlets+1) * sizeof(float));

	//Loop over number of outlet cells
	for(i=1; i<=noutlets; i++)
	{
		//Initialize overland outlet inflor to zero
		qinov[i] = 0.0;

		//Initialize overland outlet exit flow to zero
		qoutov[i] = 0.0;

		//Initialize cumulative overland outlet flow to zero
		qoutovvol[i] = 0.0;

		//Initialize peak outlet flow to zero
		qpeakov[i] = 0.0;

		//Initialize time of peak outlet flow to zero
		tpeakov[i] = 0.0;

	}  //end loop over outlets

	//Initialize cumulative sum of flow volume exiting overland plane (m3)
	totalqoutov = 0.0;

	//if channels are not simulated
	if(chnopt == 0)
	{
		//Set the outlet routing option
		outopt = 0;
	}
	else  //channel are simulated
	{
		//Allocate memory for channel outlet flows (defined by outlet)

		//peak flow leaving outlet i (m3/s)
		qpeakch = (float *)malloc((noutlets+1) * sizeof(float));
		//time of peak flow leaving outlet i (hrs)
		tpeakch = (float *)malloc((noutlets+1) * sizeof(float));

		//Loop over number of outlet cells
		for(i=1; i<=noutlets; i++)
		{
			//Initialize peak channel outlet flow to zero
			qpeakch[i] = 0.0;

			//Initialize time of peak channel outlet flow to zero
			tpeakch[i] = 0.0;

		}  //end loop over outlets

	}  //end if chnopt == 0

	/***************************************************************/
	/*        Initialize Forcing Function and BC Variables         */
	/***************************************************************/
	//
	//Include forcing functions (rainfall, external flows, loads) and boundary conditions
	//
	//Allocate memory for rainfall function arrays

	//next (upper) interval time for rainfall intensity time series
	nrft = (float *)malloc((nrg+1) * sizeof(float));
	//prior (lower) interval time for rainfall intensity time series
	prft = (float *)malloc((nrg+1) * sizeof(float));
	//intercept for rainfall intensity interpolation
	brf = (float *)malloc((nrg+1) * sizeof(float));
	//slope for rainfall intensity interpolation
	mrf = (float *)malloc((nrg+1) * sizeof(float));
	//pointer (to position in time series) for rainfall intensity
	rfpoint = (int *)malloc((nrg+1) * sizeof(int));

	//Allocate memory for overland external flow (point source) function arrays

	//next (upper) interval time for external overland flow time series
	nqwovt = (float *)malloc((nqwov+1) * sizeof(float));
	//prior (lower) interval time for external overland flow time series
	pqwovt = (float *)malloc((nqwov+1) * sizeof(float));
	//intercept for external overland flow interpolation
	bqwov = (float *)malloc((nqwov+1) * sizeof(float));
	//slope for external overland flow interpolation
	mqwov = (float *)malloc((nqwov+1) * sizeof(float));
	//pointer (to position in time series) for external overland flows
	qwovpoint = (int *)malloc((nqwov+1) * sizeof(int));
	//external overland flow interpolated in time for source [i]
	qwovinterp = (float *)malloc((nqwov+1) * sizeof(float));

	//Allocate memory for boundary condition (BC) water depth function arrays

	//next (upper) interval time for water depth BC time series
	nhbct = (float *)malloc((noutlets+1) * sizeof(float));
	//prior (lower) interval time for water depth BC  time series
	phbct = (float *)malloc((noutlets+1) * sizeof(float));
	//intercept for water depth BC interpolation
	bhbc = (float *)malloc((noutlets+1) * sizeof(float));
	//slope for water depth BC interpolation
	mhbc = (float *)malloc((noutlets+1) * sizeof(float));
	//pointer (to position in time series) for water depth BC
	hbcpoint = (int *)malloc((noutlets+1) * sizeof(int));
	//water depth BC interpolated in time for outlet[i]
	hbcinterp = (float *)malloc((noutlets+1) * sizeof(float));

	//if channels are simulated
	if(chnopt == 1)
	{
		//Allocate memory for external channel flow function arrays

		//next (upper) interval time for external channel flow time series
		nqwcht = (float *)malloc((nqwch+1) * sizeof(float));
		//prior (lower) interval time for external channel flow time series
		pqwcht = (float *)malloc((nqwch+1) * sizeof(float));
		//intercept for external channel flow interpolation
		bqwch = (float *)malloc((nqwch+1) * sizeof(float));
		//slope for external channel flow interpolation
		mqwch = (float *)malloc((nqwch+1) * sizeof(float));
		//pointer (to position in time series) for external channel flows
		qwchpoint = (int *)malloc((nqwch+1) * sizeof(int));
		//external channel flow interpolated in time for source[i]
		qwchinterp = (float *)malloc((nqwch+1) * sizeof(float));

	}  //end if chhnopt = 1

	/******************************************/
	/*        Initialize Export Files         */
	/******************************************/

	//if nqreports > 0 (if there are flow reporting stations...)
	if(nqreports > 0)
	{
		//Set the water export file pointer to null
		waterexpfile_fp = NULL;

		//Set water export file pointer
		waterexpfile_fp = fopen(waterexpfile, "w");

		//if the water export file pointer is null, abort execution
		if(waterexpfile_fp == NULL)
		{
			//Write message to screen
			printf("Can't create Water Export File: %s \n", waterexpfile);
			exit(EXIT_FAILURE);  //abort
		}

		//Write header labels to water export file...
		//
		//Write initial part of label
		fprintf(waterexpfile_fp, "Time (hours)");

		//Loop over number of flow reporting stations
		for(i=1; i<=nqreports; i++)
		{
			//Write intermediate part of label
			fprintf(waterexpfile_fp, ",Qov%d", i);

			//Write intermediate part of label
			fprintf(waterexpfile_fp, ",Qch%d", i);

			//Write intermediate part of label
			fprintf(waterexpfile_fp, ",Qsum%d ", i);

			//Check flow reporting units option (in reverse order)...
			//
			//if qunitsopt = 2, discharge reported in mm/hr
			if(qunitsopt[i] == 2)
			{
				//Write intermediate part of label
				fprintf(waterexpfile_fp, "(mm/hr)");

				//Set conversion factor to report flow in desired units
				qconvert[i] = (float) ( 1000.0 * 3600.0 / (w * w) );  //m3/s * qconvert gives mm/hr
			}
			else  //else discharge reported in m3/s
			{
				//Write intermediate part of label
				fprintf(waterexpfile_fp, "(m3/s)");

				//Set conversion factor to report flow in desired units
				qconvert[i] = 1.0;

			}  //end if qunitsopt = 2

		}  //end loop over flow reporting stations

		//Write final part of label
		fprintf(waterexpfile_fp,   "\n");

		//Close water export file
		fclose(waterexpfile_fp);

	}	//end if nqreports > 0

	/************************************************************/
	/*        Initialize Min and Max Statistics Variables       */
	/************************************************************/

	//Initialize minimum and maximum rainfall intensities
	maxrainintensity = -9e30;  //maximum rainfall intensity (m/s)
	minrainintensity = 9e30;  //minimum rainfall intensity (m/s)

	//if snow hydrology is simulated (infopt > 0)
	if(snowopt > 0)
	{
		//Initialize minimum and maximum rainfall intensities
		maxsweintensity = -9e30;  //maximum rainfall intensity (m/s)
		minsweintensity = 9e30;  //minimum rainfall intensity (m/s)

	}  //end if snowopt > 0

	//Initialize minimum and maximum overland water depths
	maxhovdepth = -9e30;  //maximum water depth in the overland plane (m)
	minhovdepth = 9e30;  //minimum water depth in the overland plane (m)

	//if infiltration is simulated (infopt > 0)
	if(infopt > 0)
	{
		//Initialize minimum and maximum infiltration water depths
		maxinfiltdepth = -9e30;  	//maximum cumulative infiltration depth (overland plane) (m)
		mininfiltdepth = 9e30;  //cumulative infiltration depth (overland plane) (m)

	}  //end if infopt > 0

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Initialize minimum and maximum channel water depths
		maxhchdepth = -9e30;  //maximum water depth in the channel network (m)
		minhchdepth = 9e30;  //minimum water depth in the channel network (m)

		//if channel transmission loss is simulated (ctlopt > 0)
		if(ctlopt > 0)
		{
			//Initialize minimum and maximum transmission loss water depths
			maxtranslossdepth = -9e30;  //maximum cumulative transmission loss depth (channel network) (m)
			mintranslossdepth = 9e30;  //minimum cumulative transmission loss depth (channel network) (m)

		}  //end if ctlopt > 0

	}  //end if chnopt > 0

	//End of function: Return to Initialize
}
