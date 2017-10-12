/*---------------------------------------------------------------------
C-  Function:	SolidsBalance.c
C-
C-  Purpose/	Controls computation of solids concentrations in the
C-  Methods:	water column and soil/sediment bed of the overland
C-				and channel network for use the next time step (t + dt).
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   chnopt
C-
C-  Calls:      OverlandSolidsConcentration,
C-              ChannelSolidsConcentration
C-
C-  Called by:  trex (main)
C-
C-  Created:  	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-				
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		17-JUL-2003
C-
C-  Revised:  
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void SolidsBalance()
{
	//Compute solids concentrations in overland plane and underlying
	//soil layer for all cells in the domain (imask != nodatavalue).
	OverlandSolidsConcentration();

	//if channels are simulated
	if(chnopt > 0)
	{
		//Compute solids concentrations in channels and underlying
		//surface sediment bed for all links/nodes in the domain.
		ChannelSolidsConcentration();

	}	//end if chnopt > 0

//End of function: Return to trex
}
