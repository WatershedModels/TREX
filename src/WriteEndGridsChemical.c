/*----------------------------------------------------------------------
C-  Function:	WriteEndGridsChemical.c                                              
C-
C-	Purpose/	Write end of run output at each grid cell at time t.
C-	Methods:	Specification details are in Data Group F
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

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void WriteEndGridsChemical(int gridcount)
{
	//Local variable declarations/definitions
	int
		igroup,		//local counter for reporting group
		ipath;		//index for transport path (erosion, deposition, net accumulation)

	float
		*cgroupsum,			//location group mass sum array (kg)
		***transport;	//local transport mass array (kg)

	//Allocate memory for local group sum and transport arrays
	cgroupsum = (float *)malloc((ncgroups+1) * sizeof(float));
	transport = (float ***)malloc((ncgroups+1) * sizeof(float **));

	//Loop over number of reporting groups
	for(igroup=1; igroup<=ncgroups; igroup++)
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

	//if the chemical gross erosion grid file (root) name is not null
	if(*chemerosiongridroot != '\0')
	{
		//transport path is erosion (ipath = 1)
		ipath = 1;

		//set values for transport
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
					//Compute chemical reporting groups sums
					ComputeChemicalGroupTransportSums(cgroupsum, i, j, ipath);

					//loop over chemical reporting groups
					for(igroup=1; igroup<=ncgroups; igroup++)
					{
						//assign cell transport
						transport[igroup][i][j] = cgroupsum[igroup];

					}	//end loop over chemical reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over chemical reporting groups
		for(igroup=1; igroup<=ncgroups; igroup++)
		{
			//Write grid of chemical transport (kg)
			Grid(chemerosiongrid[igroup], transport[igroup],
				1.0, gridcount);

		}	//end loop over chemical reporting groups

	}	//end if the chemical gross erosion grid file root name is not null

	//if the chemical gross deposition grid file (root) name is not null
	if(*chemdepositiongridroot != '\0')
	{
		//transport path is deposition (ipath = 2)
		ipath = 2;

		//set values for transport
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
					//Compute chemical reporting groups sums
					ComputeChemicalGroupTransportSums(cgroupsum, i, j, ipath);

					//loop over chemical reporting groups
					for(igroup=1; igroup<=ncgroups; igroup++)
					{
						//assign cell transport
						transport[igroup][i][j] = cgroupsum[igroup];

					}	//end loop over chemical reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over chemical reporting groups
		for(igroup=1; igroup<=ncgroups; igroup++)
		{
			//Write grid of chemical transport (kg)
			Grid(chemdepositiongrid[igroup], transport[igroup],
				1.0, gridcount);

		}	//end loop over chemical reporting groups

	}	//end if the chemical gross deposition grid file root name is not null

	//if the chemical net accumulation grid file (root) name is not null
	if(*chemnetaccumgridroot != '\0')
	{
		//transport path is net accumulation (ipath = 3)
		ipath = 3;

		//set values for transport
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
					//Compute chemical reporting groups sums
					ComputeChemicalGroupTransportSums(cgroupsum, i, j, ipath);

					//loop over chemical reporting groups
					for(igroup=1; igroup<=ncgroups; igroup++)
					{
						//assign cell transport
						transport[igroup][i][j] = cgroupsum[igroup];

					}	//end loop over chemical reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over chemical reporting groups
		for(igroup=1; igroup<=ncgroups; igroup++)
		{
			//Write grid of chemical transport (kg)
			Grid(chemnetaccumgrid[igroup], transport[igroup],
				1.0, gridcount);

		}	//end loop over chemical reporting groups

	}	//end if the chemical net accumulation grid file root name is not null

	//free memory for local arrays
	//
	//loop over reporting groups
	for(igroup=1; igroup<=ncgroups; igroup++)
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
	free(cgroupsum);

//End of function: Return to WriteGrids
}
