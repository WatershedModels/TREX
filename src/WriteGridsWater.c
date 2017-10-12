/*----------------------------------------------------------------------
C-  Function:	WriteGridsWater.c                                              
C-
C-	Purpose/	Write optional output at each grid cell at time t.
C-  Methods:    Specification details are in Data Group F
C-
C-              WaterTransport Output and Units:
C-				  rainfall intensity (mm/hr)
C-                cumulative rainfall depth (mm),
C-				  infiltration rate (mm/hr),
C-                cumulative infiltration depth (mm),
C-                water discharge (m3/s),
C-                water depth (m),
C-                snowfall intensity (mm/hr),
C-                cumulative snowfall depth (mm),
C-                snowmelt rate (mm/hr),
C-                snow depth (m) (depth of snow on the ground as SWE)
C-
C-	Inputs:		simtime
C-
C-	Outputs:	none
C-
C-	Controls:   timeprintgrid
C-
C-	Calls:		Grid
C-
C-	Called by:	WriteGrids
C-
C-	Created:	Rosalia Rojas-Sanchez
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (assumed date)
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux (CSU)
C-
C-	Date:		20-MAR-2004
C-
C-	Revisions:	Documentation, complete rewrite, reorganization
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

void WriteGridsWater(int gridcount)
{
	//Local variable declarations/definitions

	float
		**waterflow,		//water flow in overland and channel cells for grid 
		**waterdepth;		//water depths in overland and channel cells for grid 

	//if the rainfall_rate file name is not null
	if(*rainrategrid != '\0')
	{
		//convert rainfall rate units from m/s to mm/hr
		//conversion factor = 3600000.0; and
		//write gross rainfall rate (excluding interception) grid values to file
		Grid(rainrategrid, grossrainrate, 3600000.0, gridcount);
	}

	//if the rainfall_depth file name is not null
	if(*raindepthgrid != '\0')
	{
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
			//convert infiltration rate units from m/s to mm/hr
			//conversion factor = 3600000.0; and
			//write infiltration rate grid values to file
			Grid(infrategrid, infiltrationrate, 3600000.0, gridcount);
		}

		//if the infiltration_depth file name is not null
		if(*infdepthgrid != '\0')
		{
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

	//if user specified snowfall using snow gages (snowopt > 1)
	if (snowopt > 1)
	{
		//if the snowfall_rate file name is not null
		if(*swefallrategrid != '\0')
		{
			//convert snowfall rate (as SWE) units from m/s to mm/hr
			//conversion factor = 3600000.0; and
			//write snowfall rate grid values to file
			Grid(swefallrategrid, grossswerate, 3600000.0, gridcount);
		}

		//if the snowfall_depth file name is not null
		if(*swefalldepthgrid != '\0')
		{
			//convert snowfall depth units from m to mm
			//conversion factor = 1000.0; and
			//write snowfall depth grid values to file
			Grid(swedepthgrid, grossswedepth, 1000.0, gridcount);
		}

	}	//end if snowopt > 1

	//if user specified snowmelt (meltopt > 0)
	if (meltopt > 0)
	{
		//if the snowmelt_rate file name is not null
		if(*swemeltrategrid != '\0')
		{
			//convert snowmelt rate units from m/s to mm/hr
			//conversion factor = 3600000.0; and
			//write snowmelt rate grid values to file
			Grid(swemeltrategrid, swemeltrate, 3600000.0, gridcount);
		}

		//if the snowmelt_depth file name is not null
		if(*swemeltdepthgrid != '\0')
		{
			//convert snowmelt depth units from m to mm
			//conversion factor = 1000.0; and
			//write snowmelt depth grid values to file
			Grid(swemeltdepthgrid, swemeltdepth, 1000.0, gridcount);
		}

	}	//End if meltopt > 0

	//if user specified snowfall or snowmelt (snowopt > 0 or meltopt > 0)
	if (snowopt > 0 || meltopt > 0)
	{
		//if the snowmelt_rate file name is not null
		if(*swedepthgrid != '\0')
		{
			//no conversion on units (m)
			//conversion factor = 1.0; and
			//write snowmelt rate grid values to file
			Grid(swedepthgrid, sweov, 1.0, gridcount);
		}

	}	//End if snowopt > 0 or meltopt > 0

//End of function: Return to WriteGrids
}
