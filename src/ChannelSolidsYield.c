/*---------------------------------------------------------------------
C-  Function:	ChannelSolidsYield.c
C-
C-	Purpose/	Compute the yield flux of chemicals in the channel
C-	Methods:	network when one chemical state variable reacts and
C-              is transformed into another chemical state variable.
C-
C-
C-  Inputs:	    channel properties,
C-              channel reaction outfluxes[][][][]
C-
C-  Outputs:    channel reaction influxes[][][][]
C-
C-  Controls:   ncyields
C-
C-  Calls:      None
C-
C-  Called by:  ChannelSolidsKinetics
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		26-JUL-2005
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

void ChannelSolidsYield()
{
/*
	//local variable declarations/definitions
	int
		ilayer,		//index for surface layer in the sediment stack
		iyield,		//loop index for number of chemical yields
		ifrom,		//index for reacting chemical
		ito;		//index for product chemical

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//set the surface layer number
			ilayer = nstackch[i][j];

			//loop over number of chemical yields
			for(iyield=1; iyield<=nsyields; iyield++)
			{
				//set the index for the reacting chemical
				ifrom = syldfrom[iyield];

				//set the index for the product chemical
				ito = syldto[iyield];

				//if the reaction path is abrasion
				if(syldprocess[iyield] == 1)
				{
					//compute the abrasion influx for the water column
					abrsedchinflux[ito][i][j][0] = abrsedchoutflux[ifrom][i][j][0]
						* syield[iyield];

					//compute the biodegradation reaction influx for the surface sediment
					biochemchinflux[ito][i][j][ilayer] = biochemchoutflux[ifrom][i][j][ilayer]
						* cyield[iyield];

					//Developer's Note:  If mass transformation processes
					//                   occur in the subsurface sediment
					//                   layers, reaction influx terms for
					//                   these layers would need to be
					//                   computed here...

				}

				//else if the reaction path is radioactive decay
				else if(cyldprocess[iyield] == 5)
				{
					//compute the radioactive decay reaction influx for the water column
					radchemchinflux[ito][i][j][0] = radchemchoutflux[ifrom][i][j][0]
						* cyield[iyield];

					//compute the radioactive decay reaction influx for the surface sediment
					radchemchinflux[ito][i][j][ilayer] = radchemchoutflux[ifrom][i][j][ilayer]
						* cyield[iyield];

					//Developer's Note:  If mass transformation processes
					//                   occur in the subsurface sediment
					//                   layers, reaction influx terms for
					//                   these layers would need to be
					//                   computed here...

				}

				//else if the reaction path is the user-defined reaction
				else if(cyldprocess[iyield] == 7)
				{
					//compute the volatilization reaction influx for the water column
					udrchemchinflux[ito][i][j][0] = udrchemchoutflux[ifrom][i][j][0]
						* cyield[iyield];

					//compute the volatilization reaction influx for the surface sediment
					udrchemchinflux[ito][i][j][ilayer] = udrchemchoutflux[ifrom][i][j][ilayer]
						* cyield[iyield];

					//Developer's Note:  If mass transformation processes
					//                   occur in the subsurface sediment
					//                   layers, reaction influx terms for
					//                   these layers would need to be
					//                   computed here...

				}	//end if/else if cyldprocess[] == 1, 2, 3, etc.

			}	//end loop over chemical yields

		}	//end loop over nodes

	}	//end loop over links
*/
//End of function: Return to ChannelSolidsKinetics
}
