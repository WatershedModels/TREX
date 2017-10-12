/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalErosion.c
C-
C-	Purpose/	Compute the erosion flux of chemicals in the overland
C-	Methods:	plane.
C-                        
C-
C-  Inputs:	    ersflowov[][][], fparticulateov[][][][][]
C-
C-  Outputs:    erschemovoutflux[][][][][], erschemovinflux[][][][][]
C-
C-  Controls:   imask[][]
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
C-	Date:		12-JUL-2004
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

void OverlandChemicalErosion()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		isolid,		//loop index for solids (particle) type
		ilayer;		//index for surface layer in the soil stack

	float
		sumfp,		//sum of particulate fractions
		fmobile;	//sum of mobile fractions (dissolved + bound)

	double
		potential,	//chemical erosion mass potential (g)
		available,	//chemical mass available for transport (g)
		bulkvolume;	//bulk volume of sediment eroded (m3)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Compute Erosion Fluxes...
				//
				//Note:  Erosion only transports the particulate phase
				//
				//Flux from the soil surface
				//
				//set the surface layer number
				ilayer = nstackov[i][j];

				//loop over number of chemical types
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//Initialize gross erosion outflux array (from sediment bed)
					erschemovoutflux[ichem][i][j][ilayer] = 0.0;

					//initialize the sum of particulate fractions
					sumfp = 0.0;

					//loop over number of solids types
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						//compute the erosion flux leaving the sediment bed (g/s)
						erschemovoutflux[ichem][i][j][ilayer] = erschemovoutflux[ichem][i][j][ilayer]
							+ ersflowov[isolid][i][j] * cchemov[ichem][i][j][ilayer]
							* fparticulateov[ichem][isolid][i][j][ilayer];

						//compute sum of particulate fractions
						sumfp = sumfp + fparticulateov[ichem][isolid][i][j][ilayer];

					}	//end loop over number of solids types

					//if the sum of particulate fractions (sumfp) > 1.0
					if(sumfp > 1.0)
					{
						//reset sum of all particulate phases to exactly 1.0
						sumfp = (float)(1.0);

					}	//end if sumfp > 1.0

					//compute the erosion mass potential (g)
					potential = erschemovoutflux[ichem][i][j][ilayer] * dt[idt];

					//Developer's Note:  If chemicals are subject to kinetic
					//                   reactions, the transformation flux
					//                   would need to be subtracted from
					//                   the available mass...
					//
					//compute the mass available in the surface sediment (g)
					available = vlayerov[i][j][ilayer] * cchemov[ichem][i][j][ilayer] * sumfp
						- (biochemovoutflux[ichem][i][j][ilayer]
						+ hydchemovoutflux[ichem][i][j][ilayer]
						+ oxichemovoutflux[ichem][i][j][ilayer]
						+ phtchemovoutflux[ichem][i][j][ilayer]
						+ radchemovoutflux[ichem][i][j][ilayer]
						+ vltchemovoutflux[ichem][i][j][ilayer]
						+ udrchemovoutflux[ichem][i][j][ilayer]) * dt[idt];

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//if the erosion potential > available mass
					if(potential > available)
					{
						//scale the erosion flux leaving the soil bed (g/s)
						erschemovoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

					}	//end if potential > available

					//set the erosion flux entering the water column (g/s)
					erschemovinflux[ichem][i][j][0] = erschemovoutflux[ichem][i][j][ilayer];

					//Compute Porewater Release Fluxes...
					//
					//Note:  The saturated void space of soils can hold
					//       dissolved and bound (mobile) phase chemicals.
					//       As the bulk volume of the bed decreases due to
					//       erosion, chemicals in porewater are released
					//       to the surface water.  This release prevents
					//       unbounded increases in chemical concentrations
					//       as the soil layer volume goes to zero.
					//
					//
					//Developer's Note:  For now, porewater release is set
					//                   to zero for soils.  If implemented
					//                   the code would be similar to code
					//                   in ChannelChemicalErosion...
					//
					//set the porewater release flux leaving the soil bed (g/s) to zero
					pwrchemovoutflux[ichem][i][j][ilayer] = 0.0;

					//set the porewater release flux entering the water column (g/s)
					pwrchemovinflux[ichem][i][j][0] = pwrchemovoutflux[ichem][i][j][ilayer];

					//compute bulk volume of sediment released (m3)
					bulkvolume = vlayerov[i][j][ilayer] - vlayerovnew[i][j][ilayer];

					//compute fraction of mobile chemical in layer
					fmobile = fdissolvedov[ichem][i][j][ilayer]
						+ fboundov[ichem][i][j][ilayer];

					//Code for future development if porewater release is implemented...
					//
		/*			//if the bulk volume > zero
					if(bulkvolume > 0.0)
					{
						//compute the porewater release flux (g/s)
						pwrchemovoutflux[ichem][i][j][ilayer] = (float)(bulkvolume)
							* cchemov[ichem][i][j][ilayer] * fmobile / dt[idt];
					}
					else	//else bulkvolume <= zero
					{
						//set the porewater release flux leaving the soil bed (g/s) to zero
						pwrchemovoutflux[ichem][i][j][ilayer] = 0.0;

					}	//endif bulkvolume > 0.0

					//Note:  Since porewater release corresponds to erosion,
					//       the flux term must be multiplied by the erosion
					//       scale factor...
					//
					//multiply porewater flux by the erosion process scale factor
					pwrchemovoutflux[ichem][i][j][ilayer] = pwrchemovoutflux[ichem][i][j][ilayer]
						* erschscale;

					//compute the porewater release mass potential (g)
					potential = pwrchemovoutflux[ichem][i][j][ilayer] * dt[idt];

					//compute the mass available in porewater (g)
					available = vlayerov[i][j][ilayer] * cchemov[ichem][i][j][ilayer] * fmobile
						- (biochemovoutflux[ichem][i][j][ilayer]
						+ hydchemovoutflux[ichem][i][j][ilayer]
						+ oxichemovoutflux[ichem][i][j][ilayer]
						+ phtchemovoutflux[ichem][i][j][ilayer]
						+ radchemovoutflux[ichem][i][j][ilayer]
						+ vltchemovoutflux[ichem][i][j][ilayer]
						+ udrchemovoutflux[ichem][i][j][ilayer]) * dt[idt];

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//if the porewater release potential > available mass
					if(potential > available)
					{
						//scale the porewater release flux leaving the soil bed (g/s)
						pwrchemovoutflux[ichem][i][j][ilayer] = (float)(available / dt[idt]);

					}	//end if potential > available
		*/
				}	//end loop over number of chemical types

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to ChemicalTransport
}
