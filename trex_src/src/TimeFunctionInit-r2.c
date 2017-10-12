/*---------------------------------------------------------------------
C-  Function:	TimeFunctionInit.c
C-		
C-	Purpose/	TimeFunctionInit is called at the start of the
C-  Methods:    simulation to set the starting values of parameters
C-              used control time series functions.
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   ksim
C-
C-  Calls:      WaterFunctionInit, SolidsFunctionInit,
C-              ChemicalFunctionInit
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
C-	Date:		23-OCT-2003
C-
C-	Revised:
C-
C-	Date:	
C-
C-	Revisions:
C-
C----------------------------------------------------------------------*/

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

void TimeFunctionInit()
{
	//Initialize general control and water transport functions 
	TimeFunctionInitWater();

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Initialize sediment transport functions
		TimeFunctionInitSolids();

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Initialize chemical transport functions
			TimeFunctionInitChemical();

		}	//end if ksim > 2

	}	//end if ksim > 1

	//Initialize environmental property functions
	TimeFunctionInitEnvironment();

//End of function: Return to trex
}
