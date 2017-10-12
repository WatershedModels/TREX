/*----------------------------------------------------------------------
C-  Function:	SolidsPropertyInit.c
C-
C-	Purpose/	SolidsPropertyInit is called from ReadDataGroupc at
C-	Methods:	the start of the simulation to allocate memory and set
C-				default values for solids reaction properties.
C-
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   chnopt
C-
C-	Calls:		None
C-
C-	Called by:	ReadDataGroupC
C-
C-	Created:	Mark Velleux
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
C-	Date:		27-JUL-2005
C-
C-	Revisions:
C-
C-	Date:
C-
C---------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void SolidsPropertyInit()
{
	/*************************************************************/
	/*        Initialize Chemical Process Option Variables       */
	/*************************************************************/

	//Allocate memory for solids reaction option arrays
	abropt = (int *)malloc((nsolids+1) * sizeof(int));			//solids abrasion option
	mnropt = (int *)malloc((nsolids+1) * sizeof(int));			//solids mineralization option

	//Set default values...
	//
	//Loop over number of solids (plus zeroeth element for process control)
	for(i=0; i<=nsolids; i++)
	{
		//set default values to zero (0 = process not simulated)
		abropt[i] = 0;			//solids abrasion option
		mnropt[i] = 0;			//solids mineralization option

	}	//end loop over solids

	/***********************************************************/
	/*        Initialize Chemical Partitioning Variables       */
	/***********************************************************/

	//Allocate memory for solids abrasion arrays


	//Set default values...
	//
	//Loop over number of solids
	for(i=1; i<=nsolids; i++)
	{
		//set default values


	}	//end loop over solids

	/***********************************************************/
	/*        Initialize Solids Mineralization Variables       */
	/***********************************************************/

//End of function: Return to ReadDataGroupC
}
