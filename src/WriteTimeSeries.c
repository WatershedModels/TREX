/*---------------------------------------------------------------------
C-  Function:	WriteTimeSeries.c                                              
C-
C-	Purpose/    The module writes times series reports of water, solids
C-	Methods:    and chemical export  at each specified location
C-              (reporting stations) for the current time t.
C-
C-	Inputs:		qreport, sedreport, chemreport
C-
C-	Outputs:	
C-
C-	Controls:   ksim, nqreports, nsedreports, nchemreports
C-
C-	Calls:		WriteTimeSeriesWater, WriteTimeSeriesSolids,
C-              WriteTimeSeriesChemical
C-
C-	Called by:	trex
C-
C-	Created:	Rosalia Rojas-Sanchez
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (last version)
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux (CSU)
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		20-MAR-2004
C-
C-	Revisions:	Documentation, reorganization, complete rewrite
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

void WriteTimeSeries()
{
	//if the number of flow reporting stations > 0
	if(nqreports > 0)
	{
		//Write water export files
		WriteTimeSeriesWater();

	}	//end if nqreports > 0

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//if the number of solids reporting stations > 0
		if(nsedreports > 0)
		{
			//Write solids export files
			WriteTimeSeriesSolids();

		}	//end if nsedreports > 0

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//if the number of chemical reporting stations > 0
			if(nchemreports > 0)
			{
				//Write chemical export files
				WriteTimeSeriesChemical();

			}	//end if nchemreports > 0

		}	//end if ksim > 2

	}	//end if ksim > 1

//End of function: Return to trex
}
