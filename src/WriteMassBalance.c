/*---------------------------------------------------------------------
C-  Function:	WriteMassBalance.c
C-
C-	Purpose/	Write detailed mass balance summary of flow (hydrology
C-	Methods:	and hydraulics), solids, and chemicals on a cell and
C-              node basis at the end of a succesful model run.
C-
C-
C-	Inputs:     None
C-
C-	Outputs:	mass balance text file
C-
C-	Controls:	ksim
C-
C-	Calls:		WriteMassBalanceWater,
C-              WriteMassBalanceSolids,
C-              WriteMassBalanceChemical
C-
C-	Called by:	trex
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		05-FEB-2004
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C--------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void WriteMassBalance()
{
	//Write mass balance for water
	WriteMassBalanceWater();

	//if solids are simulated
	if(ksim > 1)
	{
		//Write mass balance for solids
		WriteMassBalanceSolids();

		//if chemicals are simulated
		if(ksim > 2)
		{
			//Write mass balance for chemicals
			WriteMassBalanceChemical();

		}	//end if ksim > 2

	}	//end if ksim > 1

//End of function: Return to trex
}
