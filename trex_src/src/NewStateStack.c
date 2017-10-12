/*----------------------------------------------------------------------
C-  Function:	NewStateStack.c
C-
C-	Purpose/	NewStateStack is called to store new soil and sediment
C-	Methods:	stack conditions for use during the next (upcoming)
C-				time step (t + dt).
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:	ksim, popovflag, pushovflag, chnopt, popchflag, pushchflag
C-
C-	Calls:		PopSoilStack, PushSoilStack,
C-              PopSedimentStack, PushSedimentStack
C-
C-	Called by:	NewState
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

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void NewStateStack()
{
	//local variable declarations/definitions
	int
		ilayer;		//loop index for surface layer

	//Set surface soil layer volumes...
	//
	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//set layer number for surface soil layer
				ilayer = nstackov[i][j];

				//set the surface soil layer volume (m3) (for use at time t + dt)
				vlayerov[i][j][ilayer] = vlayerovnew[i][j][ilayer];

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//check the pop and push flags for the overland soil stack
	//
	//if the soil stack pop (upward re-indexing) flag is raised
	if(popovflag == 1)
	{
		//Pop the soil stack
		PopSoilStack();

		//Reset (lower) the soil pop flag
		popovflag = 0;

	}	//end if popovflag == 1

	//if the soil stack push (downward re-indexing) flag is raised
	if(pushovflag == 1)
	{
		//Push the soil stack
		PushSoilStack();

		//Reset (lower) the soil push flag
		pushovflag = 0;

	}	//end if pushovflag == 1

	//if channels are simulated
	if(chnopt > 0)
	{
		//Loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//set layer number for surface sediment layer
				ilayer = nstackch[i][j];

				//set the surface sediment layer volume (m3) (for use at time t + dt)
				vlayerch[i][j][ilayer] = vlayerchnew[i][j][ilayer];

			}	//end loop over nodes

		}	//end loop over links

		//check the pop and push flags for the channel sediment stack
		//
		//if the sediment stack pop (upward re-indexing) flag is raised
		if(popchflag == 1)
		{
			//Pop the sediment stack
			PopSedimentStack();

			//Reset (lower) the sediment pop flag
			popchflag = 0;

		}	//end if popchflag == 1

		//if the sediment stack push (downward re-indexing) flag is raised
		if(pushchflag == 1)
		{
			//Push the sediment stack
			PushSedimentStack();

			//Reset (lower) the sediment push flag
			pushchflag = 0;

		}	//end if pushchflag == 1

	}	//end if chnopt > 0

//End of function: Return to NewState
}
