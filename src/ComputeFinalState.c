/*---------------------------------------------------------------------
C-  Function:	ComputeFinalState.c
C-
C-  Purpose/
C-  Methods:    Computes the final state of variables (water, solids,
C-              and chemicals) at the end of the simulation.
C-  Inputs:     
C-
C-  Outputs:    
C-
C-  Controls:   ksim
C-
C-  Calls:      ComputeFinalStateWater, ComputeFinalsStateSolds,
C-              ComputeFinalStateChemical
C-
C-  Called by:  trex
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
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void ComputeFinalState()
{
	//Compute initial water variables
	ComputeFinalStateWater();

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Compute initial solids concentrations 
		ComputeFinalStateSolids();

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Compute initial chemical concentrations 
			ComputeFinalStateChemical();

		}	//end if ksim > 2

	}	//end if ksim > 1

//End of function: Return to trex
}

