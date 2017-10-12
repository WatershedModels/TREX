/*----------------------------------------------------------------------
C-  Function:	WriteEndGridsSolids.c                                              
C-
C-	Purpose/	Write end of run output at each grid cell at time t.
C-  Methods:    Specification details are in Data Group F
C-
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

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void WriteEndGridsSolids(int gridcount)
{
	//Local variable declarations/definitions
	int
		ilayer,		//index for surface layer in the soil stack
		igroup,		//local counter for reporting group
		ipath,		//index for transport path (erosion, deposition, net accumulation)
		chanlink,	//link number of channel in channel cell
		channode;	//node number of channel in channel cell

	float
		*sgroupsum,			//local array for solids group sums (g/m3)
		**netelevation,		//local net elvation array (g/m3)
		***transport;		//local transport array (g/m3)

	//Allocate memory for local arrays
	sgroupsum = (float *)malloc((nsgroups+1) * sizeof(float));
	netelevation = (float **)malloc((nrows+1) * sizeof(float *));
	transport = (float ***)malloc((nsgroups+1) * sizeof(float **));

	//Loop over number of rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate remaining memory local net elevation array
		netelevation[i] = (float *)malloc((ncols+1) * sizeof(float));

	}	//end loop over rows

	//Loop over number of reporting groups
	for(igroup=0; igroup<=nsgroups; igroup++)
	{
		//Allocate additional memory local transport array
		transport[igroup] = (float **)malloc((nrows+1) * sizeof(float *));

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate remaining memory local transport array
			transport[igroup][i] = (float *)malloc((ncols+1) * sizeof(float));

		}	//end loop over rows

	}	//end loop over groups

	//if the net elevation grid file name is not null
	if(*netelevationgrid != '\0')
	{
		//set values for transport: solids transport in overland or channel cells
		//
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is not null (it is in the active area)
				if(imask[i][j] != nodatavalue)
				{
/*
					//Use this code to report values for overland cells only...
					//
					//set the surface layer number
					ilayer = nstackov[i][j];

					//compute cell net elevation change (m)
					netelevation[i][j] = (elevlayerov[i][j][ilayer-1]
						+ (float)(vlayerov[i][j][ilayer] / aovground[i][j]))
						- elevationov0[i][j];
*/
					//if the mask value = 2 (the cell has a channel)
					if(imask[i][j] == 2)
					{
						//set channel link number
						chanlink = link[i][j];

						//set channel node number
						channode = node[i][j];

						//set surface layer number
						ilayer = nstackch[chanlink][channode];

						//compute cell net elevation change (m)
						netelevation[i][j] = (elevlayerch[chanlink][channode][ilayer-1]
							+ (float)(vlayerch[chanlink][channode][ilayer] / achbed[chanlink][channode][ilayer]))
							- elevationch0[chanlink][channode];
					}
					else	//else the cell is an overland cell only
					{
						//set the surface layer number
						ilayer = nstackov[i][j];

						//compute cell net elevation change (m)
						netelevation[i][j] = (elevlayerov[i][j][ilayer-1]
							+ (float)(vlayerov[i][j][ilayer] / aovground[i][j]))
							- elevationov0[i][j];

					}	//end if imask == 2

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//Developer's Note:  The raw net elevation grid is in meters.
		//                   It is reported in cm by setting the units
		//                   conversion factor for Grid() to 100.0.  It
		//                   can be scaled to meters or mm by changing
		//                   the conversion factor argument to 1.0 for
		//                   meters or or 1000.0 for mm.
		//
		//Write grid of net elevation change (cm)
		Grid(netelevationgrid, netelevation, 100.0, gridcount);

	}	//end if the net elevation grid file name is not null

	//if the solids gross erosion grid file (root) name is not null
	if(*solidserosiongridroot != '\0')
	{
		//transport path is erosion (ipath = 1)
		ipath = 1;

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is not null (it is in the active area)
				if(imask[i][j] != nodatavalue)
				{
					//Compute solids reporting groups sums
					ComputeSolidsGroupTransportSums(sgroupsum, i, j, ipath);

					//loop over solids reporting groups (zero for sum of solids)
					for(igroup=0; igroup<=nsgroups; igroup++)
					{
						//assign cell transport
						transport[igroup][i][j] = sgroupsum[igroup];

					}	//end loop over solids reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over solids reporting groups (zero for sum of solids)
		for(igroup=0; igroup<=nsgroups; igroup++)
		{
			//Write grid of water column solids transport
			Grid(solidserosiongrid[igroup], transport[igroup],
				1.0, gridcount);

		}	//end loop over solids reporting groups

	}	//end if the solids gross erosion grid file root name is not null

	//if the solids gross deposition grid file (root) name is not null
	if(*solidsdepositiongridroot != '\0')
	{
		//transport path is deposition (ipath = 2)
		ipath = 2;

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is not null (it is in the active area)
				if(imask[i][j] != nodatavalue)
				{
					//Compute solids reporting groups sums
					ComputeSolidsGroupTransportSums(sgroupsum, i, j, ipath);

					//loop over solids reporting groups (zero for sum of solids)
					for(igroup=0; igroup<=nsgroups; igroup++)
					{
						//assign cell transport
						transport[igroup][i][j] = sgroupsum[igroup];

					}	//end loop over solids reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over solids reporting groups (zero for sum of solids)
		for(igroup=0; igroup<=nsgroups; igroup++)
		{
			//Write grid of water column solids transport
			Grid(solidsdepositiongrid[igroup], transport[igroup],
				1.0, gridcount);

		}	//end loop over solids reporting groups

	}	//end if the solids gross deposition grid file root name is not null

	//if the solids net accumulation grid file (root) name is not null
	if(*solidsnetaccumgridroot != '\0')
	{
		//transport path is net accumulation (ipath = 3)
		ipath = 3;

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is not null (it is in the active area)
				if(imask[i][j] != nodatavalue)
				{
					//Compute solids reporting groups sums
					ComputeSolidsGroupTransportSums(sgroupsum, i, j, ipath);

					//loop over solids reporting groups (zero for sum of solids)
					for(igroup=0; igroup<=nsgroups; igroup++)
					{
						//assign cell transport
						transport[igroup][i][j] = sgroupsum[igroup];

					}	//end loop over solids reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over solids reporting groups (zero for sum of solids)
		for(igroup=0; igroup<=nsgroups; igroup++)
		{
			//Write grid of water column solids transport
			Grid(solidsnetaccumgrid[igroup], transport[igroup],
				1.0, gridcount);

		}	//end loop over solids reporting groups

	}	//end if the solids net accumulation grid file root name is not null

	//free memory for local arrays
	//
	//loop over reporting groups
	for(igroup=0; igroup<=nsgroups; igroup++)
	{
		//loop over rows

		for(i=1; i<=nrows; i++)
		{
			//deallocate remaining memory for local variables 
			free(transport[igroup][i]);

		}	//end loop over rows

		//deallocate remaining memory for local variables 
		free(transport[igroup]);

	}	//end loop over groups

	//deallocate remaining memory for local variables 
	free(transport);
	free(sgroupsum);

//End of function: Return to WriteGrids
}
