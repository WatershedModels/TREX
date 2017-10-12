/*----------------------------------------------------------------------
C-  Function:	WaterBalance.c
C-
C-	Purpose/	WaterBalance is called to update water depths
C-	Methods:	in overland cells and channels for the next time step
C-				(t + dt)
C-
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:	chnopt
C-
C-	Calls:		OverlandDepth; ChannelDepth
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
C-	Date:		02-SEPT-2003
C-
C-	Revised:
C-
C-	Date:	
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void WaterBalance()
{
	//Compute water depth in overland plane for all grid cells in the
	//domain (imask != nodatavalue).
	OverlandWaterDepth();

	//if channels are simulated
	if(chnopt > 0)
	{
		//Compute water depth in channels by link and node.
		//
		//Overland and channel water depths in channel cells are
		//adjusted to account for intracell (within the same cell)
		//water transfers between the channel and the adjoining
		//overland area of the cell (i.e. floodplain interactions).
		ChannelWaterDepth();

	}	//end if chnopt > 0

//End of function: Return to trex
}
