/*---------------------------------------------------------------------
C-  Function:	OverlandSolidsKinetics.c
C-
C-	Purpose/	Computes the transformation flux of solids subject to
C-	Methods:	kinetic reations (e.g. decomposition, flocculation,
C-              etc.) in the overland plane
C-
C-
C-  Inputs:	    None
C-
C-  Outputs:    None
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  SolidsTransport
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
C-	Date:		29-DEC-2003
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

void OverlandSolidsKinetics()
{
	//if any solids abrades (abrasion) (if abropt[0] > 0)
	if(abropt[0] > 0)
	{
		//Compute channel solids abrasion
//mlv		OverlandSolidsAbrasion();

	}	//end if abropt[0] > 0

	//if any solids mineralizes (if mnropt[0] > 0)
	if(mnropt[0] > 0)
	{
		//Compute overland solids mineralization
		OverlandSolidsMineralization();

	}	//end if mnropt[0] > 0

	//if the number of solids yields > 0
	if(nsyields > 0)
	{
		//Compute overland solids yields (transformation between solids types)
		OverlandSolidsYield();

	}	//end if ncyields > 0

//End of function: Return to SolidsTransport
}
