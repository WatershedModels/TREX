/*----------------------------------------------------------------------
C-  Function:	WriteEndGridsWater.c                                              
C-
C-	Purpose/	Write end of run output at each grid cell at time t.
C-  Methods:    Specification details are in Data Group F
C-
C-              WaterTransport Options (units):
C-				rainfall intensity (mm/hr), cumulative rainfall depth (mm),
C-				infiltration rate (mm/hr), cumulative infiltration depth (mm),
C-              water discharge (m3/s), water depth (m)
C-
C-	Inputs:		simtime
C-
C-	Outputs:	none
C-
C-	Controls:   timeprintgrid
C-
C-	Calls:		Grid
C-
C-	Called by:	WriteEndGrids
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		18-FEB-2005
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

void WriteEndGridsWater(int gridcount)
{
	//dummy line to avoid compiler warning anout unreference formal paramter...
	gridcount = gridcount;

	//For now, this routine is empty because no end of run
	//grids are output for water...
/*

	//Local variable declarations/definitions

	float
		**waterflow,		//water flow in overland and channel cells for grid 
		**waterdepth;		//water depths in overland and channel cells for grid 

	//if the rainfall_rate file name is not null
	if(*rainrategrid != '\0')
	{
		//Record 6
		//convert rainfall rate units from m/s to mm/hr
		//conversion factor = 3600000.0; and
		//write gross rainfall rate (excluding interception) grid values to file
		Grid(rainrategrid, grossrainrate, 3600000.0, gridcount);
	}

	//if the rainfall_depth file name is not null
	if(*raindepthgrid != '\0')
	{
		//Record 7
		//convert rainfall depth units from m to mm
		//conversion factor = 1000.0; and
		//write rainfall depth grid values to file
		Grid(raindepthgrid, grossraindepth, 1000.0, gridcount);
	}

	//if user specified infiltration
	if (infopt > 0)
	{
		//if the infiltration_rate file name is not null
		if(*infrategrid != '\0')
		{
			//Record 8
			//convert infiltration rate units from m/s to mm/hr
			//conversion factor = 3600000.0; and
			//write infiltration rate grid values to file
			Grid(infrategrid, infiltrationrate, 3600000.0, gridcount);
		}

		//if the infiltration_depth file name is not null
		if(*infdepthgrid != '\0')
		{
			//Record 9 
			//no conversion on units (m)
			//conversion factor = 1.0; and
			//write infiltration depth grid values to file
			Grid(infdepthgrid, infiltrationdepth, 1.0, gridcount);
		}
	}	//End if infopt > 0

	//if the water_discharge file name is not null
	if(*qgrid != '\0')
	{
		//set values for waterflow: water flows in overland or channel cells
		//
		//Allocate memory local water flow array
		waterflow = (float **)malloc((nrows+1) * sizeof(float *));

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate remaining memory local water flow array
			waterflow[i] = (float *)malloc((ncols+1) * sizeof(float));

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is not null (it is in the active area)
				if(imask[i][j] != nodatavalue)
				{
					//if the mask value = 2 (the cell has a channel)
					if(imask[i][j] == 2)
					{
						//the reported flow is the channel flow
						waterflow[i][j] = dqch[link[i][j]][node[i][j]];
					}
					else	//else the cell is an overland cell only
					{
						//the reported flow is the overland flow
						waterflow[i][j] = dqov[i][j];

					}	//end if imask == 2

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//Record 10
		//no conversion on units (m3/s)
		//conversion factor = 1.0; and
		//write grid values to file
		Grid(qgrid, waterflow, 1.0, gridcount);

		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//deallocate memory
			free(waterflow[i]);

		}	//end loop over rows

		//deallocate memory for local variables 
		free(waterflow);

	}	//end if qgrid != '\0'

	//if the water_depth file name is not null
	if(*waterdepthgrid != '\0')
	{
		//set values for waterdepth: water depths in overland or channel cells
		//
		//Allocate memory local water depth array
		waterdepth = (float **)malloc((nrows+1) * sizeof(float *));

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate remaining memory local water depth array
			waterdepth[i] = (float *)malloc((ncols+1) * sizeof(float));

			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is not null (it is in the active area)
				if(imask[i][j] != nodatavalue)
				{
					//if the mask value = 2 (the cell has a channel)
					if(imask[i][j] == 2)
					{
						//the reported water depth is the channel water depth
						waterdepth[i][j] = hch[link[i][j]][node[i][j]];
					}
					else	//else the cell is an overland cell only
					{
						//the reported water depth is the overland water depth
						waterdepth[i][j] = hov[i][j];

					}	//end if imask == 2

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//Record 11
		//no conversion on units (m)
		//conversion factor = 1.0; and
		//write water depth grid values to file
		Grid(waterdepthgrid, waterdepth, 1.0, gridcount);

		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//deallocate memory
			free(waterdepth[i]);

		}	//end loop over rows

		//deallocate memory for local variables 
		free(waterdepth);

	}	//End if the water_depth file name is not null
*/

//End of function: Return to WriteGrids
}
