/*----------------------------------------------------------------------
C-  Function:	FreeMemory.c                                              
C-
C-	Purpose/	Free allocated memory at the end of a model run.
C-	Methods:
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   ksim
C-
C-	Calls:		FreeMemoryWater, FreeMemorySolids,
C-              FreeMemoryChemical
C-
C-	Called by:	trex, SimulationError
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
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		20-MAR-2004
C-
C-	Revisions:	Complete Rewrite
C-
C-	Date:		
C-
C-	Revisions:	
C-
C---------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void FreeMemory()
{
	//if sediment transport is simulated
	if(ksim > 1)
	{
		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Free memory for environmental variables 
			FreeMemoryEnvironment();

			//Free memory for chemical transport variables 
			FreeMemoryChemical();

		}	//end if ksim > 2

		//Free memory for sediment transport variables 
		FreeMemorySolids();

	}	//end if ksim > 1

	//Free memory for water transport variables 
	FreeMemoryWater();

	/**********************************************/
	/*        Free ReadDataGroupA Variables       */
	/**********************************************/
	
	//Free memory for numerical integration time steps
	free(dt);
	free(dttime);

	//Free memory for output print intervals
	free(printout);
	free(printouttime);

	//Free memory for grid print intervals
	free(printgrid);
	free(printgridtime);

//End of function: Return to trex_main or SimulationError
}
