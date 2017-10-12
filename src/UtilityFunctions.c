/*----------------------------------------------------------------------
C-  File:		UtilityFunctions.c
C-
C-	Purpose/	
C-	Methods:	Concatenated group of general utility functions for
C-              statistics
C-
C-  Function
C-	 Listing:   Min, Max,
C-
C-	Inputs:		None
C-
C-	Outputs:	None
C-
C-	Controls:   None
C-
C-	Calls:		None
C-
C-	Called by:	None
C-
C-	Created:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-				Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		31-OCT-2003
C-
C-	Revised:	
C-				
C-
C-	Date:		
C-
C-	Revisions:	
C-
C-	Date:
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//C
//C
			/*************************************/ 
			/*               FUNCTION: Max       */ 
			/*************************************/ 

/* Finds the maximum between two floating numbers */ 

double Max(double one,double theOther)

{
	double maximum;

	if(one >= theOther)
		maximum = one;
	else
		maximum = theOther;

	return(maximum);

//end of function Max
}

//C
//C
			/*************************************/ 
			/*               FUNCTION: Min       */ 
			/*************************************/ 

/* Finds the minimum between two floating numbers */ 

double Min(double one,double theOther)

{
	double minimum;

	if(one <= theOther)
		minimum = one;
	else
		minimum = theOther;

	return(minimum);

//end of function Min
}
