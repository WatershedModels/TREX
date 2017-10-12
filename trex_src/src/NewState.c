/*----------------------------------------------------------------------
C-  Function:	NewState.c
C-
C-	Purpose/	NewState is called to store new water depths, particle
C-	Methods:	concentrations, and chemical concentrations for use
C-              during the next (upcoming) time step (t + dt).
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:	ksim
C-
C-	Calls:		NewStateWater, NewStateSolids,
C-              NewStateChemical, NewStateStack
C-
C-	Called by:	trex (main)
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		20-MAR-2004
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

void NewState()
{
	//Compute new water depths 
	NewStateWater();

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Compute new solids concentrations 
		NewStateSolids();

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Compute new chemical concentrations 
			NewStateChemical();

		}	//end if ksim > 2

		//Compute new soil/sediment stack conditons
		NewStateStack();

	}	//end if ksim > 1
	
//End of function: Return to trex
}
