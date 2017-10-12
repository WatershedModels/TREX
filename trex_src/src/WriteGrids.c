/*----------------------------------------------------------------------
C-  Function:	WriteGrids.c                                              
C-
C-	Purpose/
C-	Methods:	Write optional output at each grid cell at time t.
C-              Specification details are in Data Group F
C-
C-	Inputs:		simtime
C-
C-	Outputs:	none
C-
C-	Controls:   timeprintgrid
C-
C-	Calls:		WriteGridsWater, WriteGridsSolids, WriteGridsChemical
C-
C-	Called by:	trex
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

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void WriteGrids(int gridcount)
{
	//Write water grid files
	WriteGridsWater(gridcount);

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Write solids grid files
		WriteGridsSolids(gridcount);

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Write chemical grid files
			WriteGridsChemical(gridcount);

		}	//end if ksim > 2

	}	//end if ksim > 1

//End of function: Return to trex
}
