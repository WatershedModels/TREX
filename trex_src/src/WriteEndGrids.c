/*----------------------------------------------------------------------
C-  Function:	WriteEndGrids.c                                              
C-
C-	Purpose/
C-	Methods:	Write end of run output at each grid cell at time t.
C-              Specification details are in Data Group F
C-
C-	Inputs:		simtime
C-
C-	Outputs:	none
C-
C-	Controls:   ksim
C-
C-	Calls:		WriteEndGridsWater, WriteEndGridsSolids,
C-				WriteEndGridsChemical
C-
C-	Called by:	trex
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
C-  Revised:
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

void WriteEndGrids(int gridcount)
{
	//Write water end grid files
	WriteEndGridsWater(gridcount);

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Write solids end grid files
		WriteEndGridsSolids(gridcount);

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Write chemical end grid files
			WriteEndGridsChemical(gridcount);

		}	//end if ksim > 2

	}	//end if ksim > 1

//End of function: Return to trex
}
