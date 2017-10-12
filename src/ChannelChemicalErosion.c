/*---------------------------------------------------------------------
C-  Function:	ChannelChemicalErosion.c
C-
C-	Purpose/	Compute the erosion flux of chemicals and corresponding
C-  Methods:	porewater release flux in the channel network.
C-          
C-
C-  Inputs:     fparticulate[ichem][isolid][][][ilayer],
C-              ersflowch[][][] 
C-
C-  Outputs:    erschemchinflux[ichem][][][0],
C-              erschemchoutflux[ichem][][][ilayer]
C-
C-  Controls:   erschopt, cncopt[]
C-
C-  Calls:      None
C-
C-  Called by:  ChemicalTransport
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

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void ChannelChemicalErosion()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemicals
		isolid,		//loop index for solids (particle) type
		ilayer;		//index for surface layer in the sediment stack

	float
		sumfp,		//sum of particulate fractions
		fmobile;	//sum of mobile fractions (dissolved + bound)

	double
		potential,	//chemical erosion mass potential (g)
		available,	//chemical mass available for transport (g)
		bulkvolume;	//bulk volume of sediment eroded (m3)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//set the surface layer number
			ilayer = nstackch[i][j];

			//loop over number of chemical types
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//Compute Erosion Fluxes...
				//
				//Note:  Erosion only transports the particulate phase
				//
				//Flux from channel bed
				//
				//Initialize gross erosion outflux array (from sediment bed)
				erschemchoutflux[ichem][i][j][ilayer] = 0.0;

				//initialize the sum of particulate fractions
				sumfp = 0.0;

				//loop over number of solids types
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//compute the erosion flux leaving the sediment bed (g/s)
					erschemchoutflux[ichem][i][j][ilayer] = erschemchoutflux[ichem][i][j][ilayer]
						+ ersflowch[isolid][i][j] * cchemch[ichem][i][j][ilayer]
						* fparticulatech[ichem][isolid][i][j][ilayer];

                    //compute sum of particulate fractions
					sumfp = sumfp + fparticulatech[ichem][isolid][i][j][ilayer];

				}	//end loop over number of solids types

				//if the sum of particulate fractions (sumfp) > 1.0
				if(sumfp > 1.0)
				{
					//reset sum of all particulate phases to exactly 1.0
					sumfp = (float)(1.0);

				}	//end if sumfp > 1.0

				//compute the erosion mass potential (g)
				potential = erschemchoutflux[ichem][i][j][ilayer] * dt[idt];

				//Developer's Note:  When computing the mass available
				//                   for erosion, the mass lost to
				//                   transformation processes should
				//                   only include the mass lost from
				//                   the particulate phase.  This will
				//                   require further code development.
				//
				//compute the mass available in the surface sediment (g)
				available = vlayerch[i][j][ilayer] * cchemch[ichem][i][j][ilayer] * sumfp
					- (biochemchoutflux[ichem][i][j][ilayer]
					+ hydchemchoutflux[ichem][i][j][ilayer]
					+ oxichemchoutflux[ichem][i][j][ilayer]
					+ phtchemchoutflux[ichem][i][j][ilayer]
					+ radchemchoutflux[ichem][i][j][ilayer]
					+ vltchemchoutflux[ichem][i][j][ilayer]
					+ udrchemchoutflux[ichem][i][j][ilayer]) * dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//if the erosion potential > available mass
				if(potential > available)
				{
					//scale the erosion flux leaving the sediment bed (g/s)
					erschemchoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

				}	//end if potential > available

				//set the erosion flux entering the water column (g/s)
				erschemchinflux[ichem][i][j][0] = erschemchoutflux[ichem][i][j][ilayer];

				//if the erosion flux > zero
				if(erschemchoutflux[ichem][i][j][ilayer] > 0.0)
				{
					//Compute Porewater Release Fluxes...
					//
					//Note:  The saturated void space of sediments can hold
					//       dissolved and bound (mobile) phase chemicals.
					//       As the bulk volume of the bed decreases due to
					//       erosion, chemicals in porewater are released
					//       to the surface water.  This release prevents
					//       unbounded increases in chemical concentrations
					//       as the sediment layer volume goes to zero.
					//
					//compute bulk volume of sediment released (m3)
					bulkvolume = vlayerch[i][j][ilayer] - vlayerchnew[i][j][ilayer];

					//compute fraction of mobile chemical in layer
					fmobile = fdissolvedch[ichem][i][j][ilayer]
						+ fboundch[ichem][i][j][ilayer];

					//if the bulk volume > zero
					if(bulkvolume > 0.0)
					{
						//compute the porewater release flux (g/s)
						pwrchemchoutflux[ichem][i][j][ilayer] = (float)(bulkvolume)
							* cchemch[ichem][i][j][ilayer] * fmobile / dt[idt];
					}
					else	//else bulkvolume <= zero
					{
						//set the porewater release flux leaving the sediment bed (g/s) to zero
						pwrchemchoutflux[ichem][i][j][ilayer] = 0.0;

					}	//endif bulkvolume > 0.0

					//Note:  Since porewater release corresponds to erosion,
					//       the flux term must be multiplied by the erosion
					//       scale factor...
					//
					//multiply porewater flux by the erosion process scale factor
					pwrchemchoutflux[ichem][i][j][ilayer] = pwrchemchoutflux[ichem][i][j][ilayer]
						* erschscale;

					//compute the porewater release mass potential (g)
					potential = pwrchemchoutflux[ichem][i][j][ilayer] * dt[idt];

					//Developer's Note:  When computing the mass released
					//                   with porewater, the mass lost to
					//                   transformation processes should
					//                   only include the mass lost from
					//                   the dissolved phase.  This will
					//                   require further code development.
					//
					//compute the mass available in porewater (g)
					available = vlayerch[i][j][ilayer] * cchemch[ichem][i][j][ilayer] * fmobile
						- (biochemchoutflux[ichem][i][j][ilayer]
						+ hydchemchoutflux[ichem][i][j][ilayer]
						+ oxichemchoutflux[ichem][i][j][ilayer]
						+ phtchemchoutflux[ichem][i][j][ilayer]
						+ radchemchoutflux[ichem][i][j][ilayer]
						+ vltchemchoutflux[ichem][i][j][ilayer]
						+ udrchemchoutflux[ichem][i][j][ilayer]) * dt[idt];

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//if the porewater release potential > available mass
					if(potential > available)
					{
						//scale the porewater release flux leaving the sediment bed (g/s)
						pwrchemchoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

					}	//end if potential > available

					//set the porewater release flux entering the water column (g/s)
					pwrchemchinflux[ichem][i][j][0] = pwrchemchoutflux[ichem][i][j][ilayer];

				}	//end if the erosion flux (erschemchoutflux[ichem][i][j][ilayer]) > zero

			}	//end loop over number of chemical types

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChemicalTransport
}
