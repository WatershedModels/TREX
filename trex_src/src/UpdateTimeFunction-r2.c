/*----------------------------------------------------------------------
C-  Function:   UpdateTimeFunction.c
C-
C-	Purpose/    UpdateTimeFunction is called to interpolate values of
C-  Methods:    time-dependent functions for a given dt based on input
C-              values at specific times.
C-
C-
C-	Inputs:     None
C-
C-	Outputs:    None
C-
C-	Controls:   ksim
C-
C-	Calls:	    UpdateTimeFunctionWater
C-              UpdateTimeFunctionSolids
C-              UpdateTimeFunctionChemical
C-
C-	Called by:  trex (main)
C-
C-	Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO 80523
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:       18-MAY-2004
C-
C-	Revised:
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

void UpdateTimeFunction()
{
	//Update water transport time functions
	UpdateTimeFunctionWater();

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Update sediment transport time functions
		UpdateTimeFunctionSolids();

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Update chemical transport time functions
			UpdateTimeFunctionChemical();

		}	//end if ksim > 2

	}	//end if ksim > 1

	//Update environmental property time functions
	UpdateTimeFunctionEnvironment();

//End of function: Return to trex
}
