/*---------------------------------------------------------------------
C-  Function:   Rainfall.c                                              
C-
C-  Purpose/    Spatial interpolation of gross rainfall (precipitation)
C-  Methods:    intensity for each cell for the current time step.
C-              Options include spatially uniform, Inverse Distance
C-              Weighting (IDW), uniform in space around a gage with a
C-              number of gages/locations, radar grid locations and
C-              rainrates, space-time design storms, and radar rain
C-              grids (where radar grids exactly match model grid).
C-
C-              If snow hydrology is simulated, gross precipitation is
C-              divided between rainfall and snowfall (as SWE) based
C-              on the air temperature for each cell.  In each cell,
C-              precipitation can occur as either rainfall or snowfall
C-              but never both at the same time.
C-
C-              Note: snowopt = 0 or 1 are handled in Rainfall.  All
C-                    other snowfall options are handled in Snowfall.
C-
C-  Inputs:     rfinterp[], idwradius, idwexponent
C-
C-  Outputs:    grossrainrate[][]
C-
C-  Controls:   rainopt (0,1,2,3,4,5...)
C-
C-              rainopt = 0: uniform rainfall in space
C-              rainopt = 1: rain gages are data input (i=1, nrg)
C-                and spatially-distributed with IDW
C-              rainopt = 2: design rainstorm (such as PMP), rain gages
C-                are data input (i=1, nrg) and uniform distribution in
C-                space with a grid mask/index (values from 1 to nrg)
C-              rainopt = 3: radar rainfall data input as point locations
C-                (i=1, nrg) read from file, and spatial interpolation
C-                is nearest neighbor
C-              rainopt = 4: observed extreme or design storm estimate
C-                (e.g from stochastic storm transposition) input
C-                as a single gage and distributed in space using
C-                an elliptical geometric pattern with user-entered
C-                parameters, or entered directly as a series of grids
C-                at specified time intervals
C-              rainopt = 5: rainfall rates read from grid
C-                inputs (i.e. rainfall for each cell) and
C-                used as a step function without temporal
C-                interpolation
C-
C-              snowopt = 0: snow hydrology is not simulated
C-                (all precipitation in a cell will be rain)
C-              snowopt = 1: snow hydrology is simulated
C-                (precipitation in a cell can be rain or snow,
C-                 depending on air temperature but never both)
C-              airtemp[][]
C-
C-  Calls:      ReadRainGrid, Min, Max
C-
C-  Called by:  WaterTransport
C-
C-  Created:    Original Coding in Fortran - CASC2D User's Manual
C-              Julien, P.Y. and Saghafian, B. 1991
C-              Civil Engineering, Colorado State University
C-              Fort Collins, CO 80523
C-              Code later converted from Fortran to C
C-              Last Update by Rosalia Rojas-Sanchez, CSU
C-
C-  Date:       19-JUN-2003
C-
C-  Revised:    John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-  Date:       18-JUL-2004
C-
C-  Revisions:  Documentation, reorganization, expanded input and
C-              spatial interpolation options
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:	    27-JUL-2006	
C-
C-  Revisions:	Added rainopt = 5 to permit use of grid-based
C-              rainfall intensities (variable in space) used
C-              as step functions in time.
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		15-DEC-2006
C-
C-  Revisions:	Restructured IDW algorithm.  Added option for snow
C-              hydrology with air temperature for precipitation as
C-              rainfall or snowfall (as SWE).
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-  Date:		15-AUG-2007
C-
C-  Revisions:	Integrated rainfall options 2-4 into code
C-
C-	Revised:	Mark Velleux
C-				Masa Takamatsu
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		29-Jul-2010
C-
C-	Revisions:	Cumulative mass balance calculations moved to
C-              OverlandWaterdepth for time step option (dtopt)
C-              implementation.
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void Rainfall()
{
	//local variable declarations/definitions
	float
		x,				//UTM x coordinate of grid cell i,j
		y,				//UTM y coordinate of grid cell i,j
		distance,		//distance from cell i,j to gage
		sumdistance,	//sum of inverse weighted distances
		sumrain;		//sum of inverse weighted rainfall rates

	//Developer's Note:  Rainfall.c was initially conceived as a module
	//                   where spatial interpolation of rainfall values
	//                   from individual rainfall gages would be assigned
	//                   to each cell in the model domain using IDW or
	//                   other approaches.  As this module was developed
	//                   and rainfall options added, a wider range of
	//                   approaches was implemented such that not all
	//                   options require spatial interpolation.
	//
	//                   Rainopt 0-1 were based on the uniform and
	//                   distributed rainfall options in CASC2D.  Rain
	//                   for each gage is linearly interpolated in time
	//                   (in UpdateTimeFunctionWater) before being
	//                   applied to the model grid as gross rainfall.
	//
	//                   Rainopt 2-4 were created by John England to
	//                   handle rainfall input as depth-area-duration
	//                   curves and to input rainfall using grids that
	//                   have a different cell size and do not exactly
	//                   align with the model domain.
	//
	//                   Rainopt 5 was later added to input rainfall
	//                   using grids that have exactly the same cell
	//                   size and alignment as the model domain.  This
	//                   option does not require spatial interpolation
	//                   because a unique value it assigned to each
	//                   model cell via an rainfall grid file.
	//
	//                   To improve code readability and simplify code
	//                   maintainability, Rainfall.c and other time
	//                   function update modules should be streamlined
	//                   to explicitly segregate rainfall options that
	//                   require spatial (and temporal) interpolation
	//                   into distinct blocks of code.
	//
	//if rainfall requires spatial assignment (rainopt = 0-4)
	if(rainopt >= 0 && rainopt <= 4)
	{
		//Loop over rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain (is not null)
				if(imask[i][j] != nodatavalue)
				{
					//Perform spatial interpolation
					//
					//Check rainopt...
					//
					//Uniform rain: rainopt = 0
					if(rainopt == 0)
					{
						//rainfall is uniform (there is only one gage)
						grossrainrate[i][j] = rfinterp[1];
					}
					//Spatially distributed rain: rainopt = 1
					else if(rainopt == 1)
					{
						//Initialize weighted sums for IDW interpolation
						sumdistance = 0.0;
						sumrain = 0.0;

						//Initialize the gross rainfall rate for this time step
						grossrainrate[i][j] = 0.0;

						//Translate grid location to UTM x,y coordinates
						//
						//This assumes an ESRI ASCII Grid format
						//and converts cell row and column (i,j)
						//to coordinate (x,y) located at grid cell center

						//x coordinate
						x = xllcorner + ((float)(j - 0.5)) * w;

						//y coordinate
						y = yllcorner + ((float)(nrows - i + 0.5)) * w;

						//perform Inverse Distance Weighting (IDW) interpolation
						//
						//Loop over number of rain gages
						for(k=1; k<=nrg; k++)
						{
							//Compute distance between cell and rain gage
							distance = (float)(sqrt(pow((y-rgy[k]),2.0) + pow((x-rgx[k]),2.0)));

							//if distance <= idwradius
							if(distance <= idwradius)
							{
								//if the gage is located within the current cell
								if(distance <= w)
								{
									//Note: If there is more than one gage located
									//      within a cell, the values for each gage
									//      will be given the same spatial weight
									//      and will end up being averaged to form
									//      the cell value for rainfall/precip.
									//
									//      The distance for weighting is set to the
									//      cell length (w) to prevent the weighting
									//      distance from being zero if the gage is
									//      located at the exact center of the cell.
									//
									//reset the distance to the cell length (m)
									distance = w;

								}  //end if distance <= w

								//sum weighted distances
								sumdistance = sumdistance +
									(float)(1.0/pow(distance,idwexponent));

								//sum weighted rainfall intensities
								sumrain = sumrain
									+ (float)(rfinterp[k]/pow(distance,idwexponent));

							}  //end if distance <= idwradius

						}  //end loop over number of rain gages

						//Compute IDW gross rainfall intensity for cell i,j
						//for current time step
						//
						//Note: if no gages are in range of the cell, the sum
						//      of weighted distances (sumdistance) will be
						//      zero.
						//
						//if the sum of weighted distances > zero
						if(sumdistance > 0.0)
						{
							//compute the IDW rainfall intensity (m/s)
							grossrainrate[i][j] = sumrain / sumdistance;
						}
						else	//else sumdistance <= 0.0
						{
							//set the IDW rainfall intensity to zero (m/s)
							grossrainrate[i][j] = 0.0;

						}	//end if sumdistance > 0
					}
					//Design storm and constant in space for each gage: rainopt = 2
					else if(rainopt == 2)
					{
						//Set interpolated gross rainfall rate value
						//for this cell based on design rain index
						//this assumes design rain index matches rain gage id
						grossrainrate[i][j] = rfinterp[designrainindex[i][j]];

					}
					//Radar rainfall using nearest neighbor: rainopt = 3
					else if(rainopt == 3)
					{
						//Set interpolated gross rainfall rate value
						//for this cell based on radar rainfall id pointer
						//this is determined once via initialization
						//in InitializeWater.c
						//this assumes radar field is fixed and does not move in time
						//rainfall rates for locations outside user-entered
						// mindistance are set to zero using rfinterp[0]=0
						//in InitializeWater
						grossrainrate[i][j] = rfinterp[radaridpoint[i][j]];

					}
					//Depth-Area-Duration Storm Rainfall and Stochastic
					//  Storm Transposition/Space-Time inputs: rainopt = 4
					else if(rainopt == 4)
					{
						//Set interpolated gross rainfall rate value
						//for this cell based on D-A-D rainfall index id pointer
						//this is determined once via initialization
						//in InitializeWater.c
						//this assumes DAD storm is fixed and does not move in time
						//rainfall rates for locations outside user-entered
						// mindistance are set to zero using rfinterp[0]=0
						//in InitializeWater
						grossrainrate[i][j] = rfinterp[dadstormindex[i][j]];

					}  //end if rainopt = 0, 1, 2, 3, or 4

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

					}	//end if snowopt = 1

				}	//end if imask[][] != nodatavalue

			}	//end loop over columns

		}	//end loop over rows
	}
	//else the rainfall option does not require spatial interpolation (rainopt = 5)
	else if(rainopt == 5)	//gridded rainfall option 
	{
		//Note:  This rainfall option assumes that the user specifies a
		//       grid of rainfall intensity values for each cell in the
		//       active model domain.  The rainfall values in each grid
		//       can vary cell-by-cell in space but are not interpolated
		//       in space and are used as a step function in time (no 
		//       temporal interpolation).
		//
		//if simulation time >= time to read a new grid of rainfall intensities
		if(simtime >= timeraingrid)
		{
			//Note:  Gross rainfall intensities are specified for
			//       each cell in ReadRainGridFile
			//
			//Read rainfall grid from file
			ReadRainGridFile();

			//increment sequential count of rainfall grids read events for file extension
			raingridcount = raingridcount + 1;

			//Set the new rainfall grid read time
			timeraingrid = timeraingrid + raingridfreq;

		}	//end if simtime >= timeraingrid
	}
	else	//else rain option not yet implemented
	{
		//Write error message to file
		fprintf(echofile_fp,
			"\n\nRainopt > 5 not implemented...\n");

		//Report invalid rainopt
		fprintf(echofile_fp, "User selected rainopt = %d\n", rainopt);
	
		//Advise for valid rainopt
		fprintf(echofile_fp, "\n\nSelect rainopt = 0 - 5");

		exit(EXIT_FAILURE);	//abort

	}	//end if rainopt >= 0 and <= 4...

//End of Function: Return to WaterTransport
}
