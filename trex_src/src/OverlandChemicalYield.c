/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalYield.c
C-
C-	Purpose/	Compute the yield flux of chemicals in the overland
C-	Methods:	plane when one chemical state variable reacts and is
C-              transformed into another chemical state variable.
C-
C-
C-  Inputs:	    channel properties,
C-              overland reaction outfluxes[][][][]
C-
C-  Outputs:    overland reaction in influxes[][][][]
C-
C-  Controls:   imask[][], ncyields
C-
C-  Calls:      None
C-
C-  Called by:  OverlandChemicalKinetics
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

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void OverlandChemicalYield()
{
	//local variable declarations/definitions
	int
		ilayer,		//index for surface layer in the sediment stack
		iyield,		//loop index for number of chemical yields
		ifrom,		//index for reacting chemical
		ito;		//index for product chemical

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//set the surface layer number
				ilayer = nstackch[i][j];

				//loop over number of chemical yields
				for(iyield=1; iyield<=ncyields; iyield++)
				{
					//set the index for the reacting chemical
					ifrom = cyldfrom[iyield];

					//set the index for the product chemical
					ito = cyldto[iyield];

					//if the reaction path is biodegradation
					if(cyldprocess[iyield] == 1)
					{
						//compute the biodegradation reaction influx for the water column
						biochemovinflux[ito][i][j][0] = biochemovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the biodegradation reaction influx for the surface sediment
						biochemovinflux[ito][i][j][ilayer] = biochemovoutflux[ifrom][i][j][ilayer]
							* cyield[iyield];

						//Developer's Note:  If mass transformation processes
						//                   occur in the subsurface sediment
						//                   layers, reaction influx terms for
						//                   these layers would need to be
						//                   computed here...

					}
					//else if the reaction path is hydrolysis
					else if(cyldprocess[iyield] == 2)
					{
						//compute the hydrolysis reaction influx for the water column
						hydchemovinflux[ito][i][j][0] = hydchemovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the hydrolysis reaction influx for the surface sediment
						hydchemovinflux[ito][i][j][ilayer] = hydchemovoutflux[ifrom][i][j][ilayer]
							* cyield[iyield];

						//Developer's Note:  If mass transformation processes
						//                   occur in the subsurface sediment
						//                   layers, reaction influx terms for
						//                   these layers would need to be
						//                   computed here...

					}
					//else if the reaction path is oxidation
					else if(cyldprocess[iyield] == 3)
					{
						//compute the oxidation reaction influx for the water column
						oxichemovinflux[ito][i][j][0] = oxichemovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the oxidation reaction influx for the surface sediment
						oxichemovinflux[ito][i][j][ilayer] = oxichemovoutflux[ifrom][i][j][ilayer]
							* cyield[iyield];

						//Developer's Note:  If mass transformation processes
						//                   occur in the subsurface sediment
						//                   layers, reaction influx terms for
						//                   these layers would need to be
						//                   computed here...

					}
					//else if the reaction path is photolysis
					else if(cyldprocess[iyield] == 4)
					{
						//compute the photolysis reaction influx for the water column
						phtchemovinflux[ito][i][j][0] = phtchemovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the photolysis reaction influx for the surface sediment
						phtchemovinflux[ito][i][j][ilayer] = phtchemovoutflux[ifrom][i][j][ilayer]
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
						radchemovinflux[ito][i][j][0] = radchemovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the radioactive decay reaction influx for the surface sediment
						radchemovinflux[ito][i][j][ilayer] = radchemovoutflux[ifrom][i][j][ilayer]
							* cyield[iyield];

						//Developer's Note:  If mass transformation processes
						//                   occur in the subsurface sediment
						//                   layers, reaction influx terms for
						//                   these layers would need to be
						//                   computed here...

					}
					//else if the reaction path is volatilization
					else if(cyldprocess[iyield] == 6)
					{
						//compute the volatilization reaction influx for the water column
						vltchemovinflux[ito][i][j][0] = vltchemovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the volatilization reaction influx for the surface sediment
						vltchemovinflux[ito][i][j][ilayer] = vltchemovoutflux[ifrom][i][j][ilayer]
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
						udrchemovinflux[ito][i][j][0] = udrchemovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the volatilization reaction influx for the surface sediment
						udrchemovinflux[ito][i][j][ilayer] = udrchemovoutflux[ifrom][i][j][ilayer]
							* cyield[iyield];

						//Developer's Note:  If mass transformation processes
						//                   occur in the subsurface sediment
						//                   layers, reaction influx terms for
						//                   these layers would need to be
						//                   computed here...

					}
					//else if the reaction path is dissolution
					else if(cyldprocess[iyield] == 8)
					{
						//Note:  Pure solid phase chemicals are simulated as solids
						//       and can dissolve.  When dissolution occurs, the
						//       solids outflux (e.g. dslsedoutflux) can generate
						//       a chemical influx (e.g. dslcheminflux)
						//
						//compute the dissolution reaction influx for the water column
						dslchemovinflux[ito][i][j][0] = dslsedovoutflux[ifrom][i][j][0]
							* cyield[iyield];

						//compute the dissolution reaction influx for the surface sediment
						dslchemovinflux[ito][i][j][ilayer] = dslsedovoutflux[ifrom][i][j][ilayer]
							* cyield[iyield];

						//Developer's Note:  If mass transformation processes
						//                   occur in the subsurface sediment
						//                   layers, reaction influx terms for
						//                   these layers would need to be
						//                   computed here...

					}	//end if/else if cyldprocess[] == 1, 2, 3, etc.

				}	//end loop over chemical yields

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to OverlandChemicalKinetics
}
