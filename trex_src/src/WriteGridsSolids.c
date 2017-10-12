/*----------------------------------------------------------------------
C-  Function:	WriteGridsSolids.c                                              
C-
C-	Purpose/	Write optional output at each grid cell at time t.
C-  Methods:    Specification details are in Data Group F
C-
C-              SolidsTransport Output and Units:
C-				  solids concentration in surface water (runoff and streams) (g/m3),
C-				  solids concentration in surface soil and sediment (g/m3)
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
C-	Date:		24-DEC-2003
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

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void WriteGridsSolids(int gridcount)
{
	//Local variable declarations/definitions
	int
		igroup;		//local counter for reporting group

	float
		*sgroupsum,			//local sum array for concentrations (g/m3)
		***concentration;	//local concentration array (g/m3)

	//Allocate memory for local group sum and concentration arrays
	sgroupsum = (float *)malloc((nsgroups+1) * sizeof(float));
	concentration = (float ***)malloc((nsgroups+1) * sizeof(float **));

	//Loop over number of reporting groups
	for(igroup=0; igroup<=nsgroups; igroup++)
	{
		//Allocate additional memory local concentration array
		concentration[igroup] = (float **)malloc((nrows+1) * sizeof(float *));

		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate remaining memory local concentration array
			concentration[igroup][i] = (float *)malloc((ncols+1) * sizeof(float));

		}	//end loop over rows

	}	//end loop over groups

	//if the water column solids concentration grid file (root) name is not null
	if(*solidsconcwatergridroot != '\0')
	{
		//set values for concentration: solids concentration in overland or channel cells
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
					//Compute solids reporting groups sums (water column)
					ComputeSolidsGroupConcSums(sgroupsum, i, j, 0);

					//loop over solids reporting groups (zero for sum of solids)
					for(igroup=0; igroup<=nsgroups; igroup++)
					{
						//assign cell concentrations
						concentration[igroup][i][j] = sgroupsum[igroup];

					}	//end loop over solids reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over solids reporting groups (zero for sum of solids)
		for(igroup=0; igroup<=nsgroups; igroup++)
		{
			//Write grid of water column solids concentrations
			Grid(solidsconcwatergrid[igroup], concentration[igroup],
				1.0, gridcount);

		}	//end loop over solids reporting groups

	}	//end if the water column solids grid file root name is not null

	//if the surface sediment layer solids concentration grid file (root) name is not null
	if(*solidsconcsurfgridroot != '\0')
	{
		//Loop over number of rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over number of columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is not null (it is in the active area)
				if(imask[i][j] != nodatavalue)
				{
					//Compute solids reporting groups sums (surface layer)
					ComputeSolidsGroupConcSums(sgroupsum, i, j, 1);

					//loop over solids reporting groups (zero for sum of solids)
					for(igroup=0; igroup<=nsgroups; igroup++)
					{
						//assign cell concentrations
						concentration[igroup][i][j] = sgroupsum[igroup];

					}	//end loop over solids reporting groups

				}	//end if imask != nodatatavalue

			}	//end loop over columns

		}	//end loop over rows

		//loop over solids reporting groups (zero for sum of solids)
		for(igroup=0; igroup<=nsgroups; igroup++)
		{
			//Write grid of water column solids concentrations
			Grid(solidsconcsurfgrid[igroup], concentration[igroup],
				1.0, gridcount);

		}	//end loop over solids reporting groups

	}	//end if the surface sediment layer solids grid file root name is not null

	//free memory for local arrays
	//
	//loop over reporting groups
	for(igroup=0; igroup<=nsgroups; igroup++)
	{
		//loop over rows

		for(i=1; i<=nrows; i++)
		{
			//deallocate remaining memory for local variables 
			free(concentration[igroup][i]);

		}	//end loop over rows

		//deallocate remaining memory for local variables 
		free(concentration[igroup]);

	}	//end loop over groups

	//deallocate remaining memory for local variables 
	free(concentration);
	free(sgroupsum);

//End of function: Return to WriteGrids
}
