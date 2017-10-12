/*---------------------------------------------------------------------
C-  Function:	TruncateDT.c                                              
C-
C-	Purpose/    Truncates timestep values to 3 significant figures. 
C-
C-	Inputs:		dt[]
C-
C-	Outputs:	dttruncate, dt[] (truncated)
C-
C-	Controls:   None
C-
C-	Calls:		None
C-
C-	Called by:	WaterTransport
C-
C-	Created:	Masa Takamatsu
C-				Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

void TruncateDT()
{
	//local variable declarations/definitions
	float
		ndigits,			//number of additional digits beyond the characteristic in rounded dt values
		characteristic,		//characteristic (magnitude) of dt value (log10)
//mlv		mantissa,			//mantissa of dt value (log10)
		dttruncate;			//truncated time step value (value rounded down) (seconds)

	//Note:  When rounding dt values, the first significant digit is the
	//       characteristic.  The number of additional digits beyond the
	//       characteristic is determined by ndigits.  The total number
	//       of signfificant digits in rouded dt values is 1 + ndigits.
	//
	//Developer's Note:  The number of significant digits for rounded dt values
	//                   should really be specified as an input in Data Group A.
	//                   For now, this is left as a local parameter to simplify
	//                   code development.
	//
	//                   Also, ndigits should really be declared as integer but
	//                   is declared as float to eliminate the need to typecast
	//                   it to float when used to calculate dt (which is float).
	//
	//assign number of additional significant digits for rounding dt values
	ndigits = 0.0;

	//Calulate the characteristic...
	//
	//if the time step is greater than or equal to one
	if(dt[idt] >= 1.0)
	{
		//calculate time step characteristic
		characteristic = (float)((int)(log10(dt[idt])));
	}
	else	//else dt < 1.0 (it cannot ever be zero)
	{
		//calculate time step characteristic
		characteristic = (float)((int)(log10(dt[idt]))-1.0);

	}	//end if dt[] >= 1.0

	//calculated truncated (rounded) dt value (seconds)
	dttruncate = (float)((int)(dt[idt] / pow(10,characteristic-ndigits))
		* pow(10,characteristic-ndigits));

	//set dt to truncated value (seconds)
	dt[idt] = dttruncate;

//End of function: Return to WaterTransport
}
