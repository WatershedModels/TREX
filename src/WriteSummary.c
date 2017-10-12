/*----------------------------------------------------------------------
C-  Function:	WriteSummary.c                                              
C-
C-	Purpose/	Write water, solids and chemical summaries
C-              at the end of a model run.
C-	Methods:
C-
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   ksim
C-
C-	Calls:		WriteSummaryWater, WriteSummarySolids,
C-              WriteSummaryChemical
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
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		19-MAR-2004
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

void WriteSummary()
{
	//Write summary file for water transport variables 
	WriteSummaryWater();

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Write summary file for sediment transport variables 
		WriteSummarySolids();

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Write summary file for chemical transport variables 
			WriteSummaryChemical();

		}	//end if ksim > 2

	}	//end if ksim > 1

//End of function: Return to trex_main
}
