/*---------------------------------------------------------------------
C-  Function:   Snowmelt.c                                              
C-
C-  Purpose/    Compute melt rate of snowpack for each cell in the
C-  Methods:    overland plane.
C-
C-  Inputs:     rfinterp[], idwradius, idwexponent
C-
C-  Outputs:    grossrainrate[][], swemeltrate[][]
C-
C-  Controls:   meltopt (0,1,2,3...)
C-              meltopt = 0: no snow hydrology simulated
C-              meltopt = 1: temperature index snowmelt
C-              meltopt = 2: energy balance snowmelt
C-              meltopt = 3: snow gage melt rate data are input
C-
C-  Calls:      Min, Max
C-
C-  Called by:  WaterTransport
C-
C-  Created:    Mark Velleux
C-              HydroQual, Inc. (personal time)
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-  Date:       15-MAY-2006
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

void Snowmelt()
{
	//local variable declarations/definitions
	int
		chanlink,	//channel link number
		channode;	//channel node number

	float
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf;	//surface area of channel (m2)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Set the cell surface area
				//
				//if the cell is a channel cell
				if(imask[i][j] > 1)
				{
					//Get channel link and node
					chanlink = link[i][j];
					channode = node[i][j];

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)
					twch = twidth[chanlink][channode];			//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;
				}
				else	//cell is not a channel cell (overland only)
				{
					//no channel present, surface area is zero
					achsurf = 0.0;

				}	//end if imask > 1

				//if there is snow on the ground in this cell (sweov[][] > 0)
				if(sweov[i][j] > 0.0)
				{
					//if snowmelt is computed using a modified temperature index approach
					if(meltopt == 1)
					{
						//Snowmelt computed as snow-water equivalent (swe)
						//
						//if the air temperature is above the the melting point
						if(airtemp[i][j] > tsnow)
						{
							//compute snowmelt rate	(as swe) (m/s)
							swemeltrate[i][j] = (float)(atmelt * (airtemp[i][j] - tsnow)
								+ (1.0 - skyview[i][j]) * srmelt * (1.0 - albedo[i][j])
								* solarrad[i][j]);
						}
						else	//else, airtemp[][] <= tsnow
						{
							//set the snowmelt rate (as swe) to zero
							swemeltrate[i][j] = 0.0;

						}	//end if temperature > 0
					}
					//if snowmelt is computed using a simplified energy approach
					else if(meltopt == 2)
					{
						//Write error message to file
						fprintf(echofile_fp,
							"\n\nSnowopt = 2 not yet implemented...\n");

						//Report invalid snowopt
						fprintf(echofile_fp, "User selected snowopt = %d\n", snowopt);
					
						//Advise for valid snowopt
						fprintf(echofile_fp, "\n\nSelect snowopt <= 1.");

						exit(EXIT_FAILURE);	//abort
					}
					//if else snowmelt option not yet implemented
					else
					{
						//Write error message to file
						fprintf(echofile_fp,
							"\n\nSnowopt > 2 not implemented...\n");

						//Report invalid snowopt
						fprintf(echofile_fp, "User selected snowopt = %d\n", snowopt);
					
						//Advise for valid snowopt
						fprintf(echofile_fp, "\n\nSelect snowopt <= 2.");

						exit(EXIT_FAILURE);	//abort

					}  //end if snowopt = 1, 2, etc.


					//check if the snowmelt potential exceeds the available snow
					if(swemeltrate[i][j]*dt[idt] > sweov[i][j])
					{
						//limit snowmelt rate to available water supply
						swemeltrate[i][j] = sweov[i][j] / dt[idt];

					}	//end check of snowmelt potential
				}
				else	//else, sweov[][] = zero
				{
					//set the snowmelt rate (as swe) to zero
					swemeltrate[i][j] = 0.0;

				}	//end if temperature > 0

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of Function: Return to WaterTransport
}
