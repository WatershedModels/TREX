/*---------------------------------------------------------------------
C-  Function:   Snowfall.c                                              
C-
C-  Purpose/    Spatial interpolation of gross snowfall intensity for
C-  Methods:    each cell for the current time step when snowopt > 1.
C-              Options include Inverse Distance Weighting (IDW) and
C-              snowfall (swe) grids that exactly match the model grid.
C-              Snowfall is always specified as snow water equivalent
C-              (SWE).
C-
C-              If snow hydrology is simulated and snowopt > 1, the
C-              Rainfall and Snowfall modules are both called and
C-              separate rainfall and snowfall (as SWE) time functions
C-              are independently specified.  This allows rainfall and
C-              snowfall to both occur in a cell at the same time in a
C-              single time step.
C-
C-              Note: snowopt = 0 or 1 are handled in Rainfall.  All
C-                    other snowfall options are handled in Snowfall.
C-
C-  Inputs:     sfinterp[], sidwradius, sidwexponent
C-
C-  Outputs:    grossswerate[][]
C-
C-  Controls:   snowopt (0,1,2,3,...)
C-
C-              snowopt = 0: snow hydrology is not simulated
C-                (handled in Rainfall)
C-              snowopt = 1: snowfall computed from rainfall, air temp
C-                (handled in Rainfall)
C-              snowopt = 2: snow gages are data input (i=1, nsg)
C-                and spatially-distributed with IDW
C-              snowopt = 3: snowfall rates read from grid
C-                inputs (i.e. snowfall for each cell) and
C-                used as a step function without temporal
C-                interpolation
C-
C-              airtemp[][]
C-
C-  Calls:      ReadSnowGrid, Min, Max
C-
C-  Called by:  WaterTransport
C-
C-  Created:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		15-DEC-2006
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

void Snowfall()
{
	//local variable declarations/definitions
	float
		x,				//UTM x coordinate of grid cell i,j
		y,				//UTM y coordinate of grid cell i,j
		distance,		//distance from cell i,j to gage
		sumdistance,	//sum of inverse weighted distances
		sumsnow;		//sum of inverse weighted snowfall rates (as swe)

	//if snowfall requires spatial assignment from gage data
	if(snowopt == 2)
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
					//Initialize weighted sums for IDW interpolation
					sumdistance = 0.0;
					sumsnow = 0.0;

					//Initialize the gross snowfall (swe) rate for this time step
					grossswerate[i][j] = 0.0;

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
					//Loop over number of snow gages
					for(k=1; k<=nsg; k++)
					{
						//Compute distance between cell and snow gage
						distance = (float)(sqrt(pow((y-sgy[k]),2.0) + pow((x-sgx[k]),2.0)));

						//if distance <= sidwradius (Notre: sidwradius is snowfall IDW radius)
						if(distance <= sidwradius)
						{
							//if the gage is located within the current cell
							if(distance <= w)
							{
								//Note: If there is more than one gage located
								//      within a cell, the values for each gage
								//      will be given the same spatial weight
								//      and will end up being averaged to form
								//      the cell value for snowfall swe.
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
								(float)(1.0/pow(distance,sidwexponent));

							//sum weighted snowfall (swe) intensities
							sumsnow = sumsnow
								+ (float)(sfinterp[k]/pow(distance,sidwexponent));

						}  //end if distance <= sidwradius

					}  //end loop over number of snow gages

					//Compute IDW gross snowfall (swe) intensity for cell i,j
					//for current time step
					//
					//Note: if no gages are in range of the cell, the sum
					//      of weighted distances (sumdistance) will be
					//      zero.
					//
					//if the sum of weighted distances > zero
					if(sumdistance > 0.0)
					{
						//compute the IDW snowfall (swe) intensity (m/s)
						grossswerate[i][j] = sumsnow / sumdistance;
					}
					else	//else sumdistance <= 0.0
					{
						//set the IDW snowfall (swe) intensity to zero (m/s)
						grossswerate[i][j] = 0.0;

					}	//end if sumdistance > 0

				}	//end if imask[][] != nodatavalue

			}	//end loop over columns

		}	//end loop over rows
	}
	//else the snowfall option does not require spatial interpolation (snowopt = 3)
	else if(snowopt == 3)	//gridded snowfall (swe) option 
	{
		//Note:  This snowfall option assumes that the user specifies a
		//       grid of snowfall intensity values for each cell in the
		//       active model domain.  The snowfall values in each grid
		//       can vary cell-by-cell in space but are not interpolated
		//       in space and are used as a step function in time (no 
		//       temporal interpolation).  All snowfall is assumed to be
		//       input as swe.
		//
		//if simulation time >= time to read a new grid of snowfall (swe) intensities
		if(simtime >= timesnowgrid)
		{
			//Note:  Gross snowfall (swe) intensities are specified for
			//       each cell in ReadSnowGridFile
			//
			//Read snowfall (swe) grid from file
			ReadSnowGridFile();

			//increment sequential count of snowfall grids read events for file extension
			snowgridcount = snowgridcount + 1;

			//Set the new snowfall grid read time
			timesnowgrid = timesnowgrid + snowgridfreq;

		}	//end if simtime >= timesnowgrid
	}
	else	//else snow option not yet implemented
	{
		//Note:  There are two ways to trigger this error trap:
		//       snowopt < 2; and snowopt > 3.  For snowopt = 0
		//       or 1, Snowfall should not be called.  If it is,
		//       this indicates a serious programming error.  For
		//       snowopt < 0 or > 3, this indicates that the input
		//       value of snowopt is not valid.
		//
		//Write error message to file
		fprintf(echofile_fp,
			"\n\nSnowopt > 3 not implemented...\n");

		//Report invalid snowopt
		fprintf(echofile_fp, "User selected snowopt = %d\n", snowopt);
	
		//Advise for valid snowopt
		fprintf(echofile_fp, "\n\nSelect snowopt = 0 - 3");

		exit(EXIT_FAILURE);	//abort

	}	//end if snowopt = 2 or = 3...

//End of Function: Return to WaterTransport
}
