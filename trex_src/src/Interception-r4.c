/*----------------------------------------------------------------------
C-  Function:	Interception.c                                              
C-
C-  Purpose/    Computes interception depth and net rainfall from gross
C-	Methods:    rainfall and net snowfall from gross snowfall.
C-
C-
C-	Inputs:		grossrainrate[][], grossswerate[][]
C-
C-	Outputs:	interceptiondepth[][], interceptionvol[][],
C-              netrainrate[][], netswerate[][]
C-
C-	Controls:	snowopt
C-
C-	Calls:		None
C-
C-	Called by:	WaterTransport
C-
C-	Created:	Rosalia Rojas-Sanchez (CSU)
C-
C-	Date:		19-JUN-2003 (date assumed)
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux (CSU)
C-
C-	Date:		19-AUG-2003
C-
C-	Revisions:	Documentation and streamlining.
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		15-DEC-2006
C-
C-  Revisions:	Added option for snowfall interception
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
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void Interception()
{
	//local variable declarations/definitions
	int
		chanlink,	//channel link number
		channode;	//channel node number

	float
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf;	//surface area of channel (m)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//if remaining interception depth > 0, some rain will be intercepted
				if(interceptiondepth[i][j] > 0.0)
				{
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

					//if the depth of gross rainfall exceeds the interception depth
					if(grossrainrate[i][j] * dt[idt] >= interceptiondepth[i][j])
					{
						//The net rainfall rate is computed from gross rainfall
						//and interception:
						//
						//  1. net rainfall is a rate (m/s),
						//	2. gross rainfall is a rate (m/s),
						//	3. interception is a depth (m) expressed as a
						//     rate	when divided by the time step.
						//
						//Compute net rainfall rate
						netrainrate[i][j] = grossrainrate[i][j]
							- interceptiondepth[i][j] / dt[idt];
					}
					else	//interception depth exceeds gross rainfall (depth)
					{
						//Set net rainfall rate to zero
						netrainrate[i][j] = 0.0;

					}	//end if gross rain > interception
				}
				else	//interception is satified (full), remaining interception is zero
				{
					//net rainfall rate = gross rainfall rate
					netrainrate[i][j] = grossrainrate[i][j];

				}	//end if remaining interception depth > 0

				//if snowfall is simulated as falling snow
				if(snowopt > 0 && snowopt <= 3)
				{
					//if remaining interception depth > 0, some snow will be intercepted
					if(interceptiondepth[i][j] > 0.0)
					{
						//if the depth of gross snowfall (as swe) exceeds the interception depth
						if(grossswerate[i][j] * dt[idt] >= interceptiondepth[i][j])
						{
							//The net snowfall rate is computed from gross snowfall
							//and interception:
							//
							//  1. net snowfall is a rate (m/s),
							//	2. gross snowfall is a rate (m/s),
							//	3. interception is a depth (m) expressed as a
							//     rate	when divided by the time step.
							//
							//Compute net snowfall rate
							netswerate[i][j] = grossswerate[i][j]
								- interceptiondepth[i][j] / dt[idt];
						}
						else	//interception depth exceeds gross rainfall (depth)
						{
							//Set net snowfall rate to zero
							netswerate[i][j] = 0.0;

						}	//end if gross snow > interception
					}
					else	//interception is satified (full), remaining interception is zero
					{
						//net snowfall rate = gross snowfall rate
						netswerate[i][j] = grossswerate[i][j];

					}	//end if remaining interception depth > 0

				}	//endif snowopt > 1 && snowopt <= 3

				//Note: the cumulative interception volume is computed as
				//      part of simulation final wrap-up calculations...

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of Function: Return to WaterTransport
}
