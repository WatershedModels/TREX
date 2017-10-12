/*---------------------------------------------------------------------
C-  Function:	ChannelChemicalDeposition.c
C-
C-	Purpose/	Compute the deposition flux of solids in the channel
C-	Methods:	network.  Deposition only transports the particulate
C-              chemical phase.
C-
C-
C-  Inputs:	    fparticulate[chem][isolid][][][0], channel properties,
C-              depinflowch[][][], depoutflowch[][][]
C-
C-  Outputs:    depchemchinflux[][][][][][],
C-              depchemchoutflux[][][][][][]
C-
C-  Controls:   imask[][], depchopt, cncopt[]
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

void ChannelChemicalDeposition()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		isolid,		//loop index for solids (particle) type
		ilayer;		//index for surface layer in the sediment stack

	float
		hchan,		//depth of water in channel (m)
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		twch,		//top width of channel at top of bank (m)
		achcross,	//cross sectional area of channel at present water depth (m2)
		watervol,	//water column volume of node (at time t) (m3)
		sumfp;		//sum of particulate fractions

	double
		potential,	//chemical deposition potential (g)
		available;	//chemical mass available for deposition (g)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Compute Deposition Fluxes...
			//
			//Note:  Deposition only transports the particulate phase
			//
			//Flux to channel bed
			//
			//Assign local water depth (m)
			hchan = hch[i][j];

			//Assign channel characteristics
			hbch = hbank[i][j];				//bank height (m)
			sslope = sideslope[i][j];		//side slope (dimensionless)
			twch = twidth[i][j];			//channel top width at top of bank (m)
			bwch = bwidth[i][j];			//bottom width (m)

			//Compute channel characteristics...
			//
			//if the flow depth <= bank height
			if(hchan <= hbch)
			{
				//Cross-sectional area for trapezoidal, triangular, or rectangular channel
				achcross = (bwch + sslope * hchan) * hchan;
			}
			else	//Flow depth greater than channel depth
			{
				//Cross-sectional area for trapezoidal, triangular, or rectangular channel
				//plus rectangular area above bank height
				achcross = (bwch + sslope * hbch) * hbch
					+ (hchan - hbch) * twch;

			}	//end if hchan <= bank height

			//compute present water column volume (m3) (at time t)
			watervol = achcross * chanlength[i][j];

			//set the surface layer number
			ilayer = nstackch[i][j];

			//loop over number of chemicals
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//Initialize gross deposition outflux array (from water column)
				depchemchoutflux[ichem][i][j][0] = 0.0;

				//initialize the sum of particulate fractions
				sumfp = 0.0;

				//loop over number of solids types
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//compute the deposition flux leaving water column (g/s)
					depchemchoutflux[ichem][i][j][0] = depchemchoutflux[ichem][i][j][0]
						+ depflowch[isolid][i][j] * cchemch[ichem][i][j][0]
						* fparticulatech[ichem][isolid][i][j][0];

                    //compute sum of particulate fractions
					sumfp = sumfp + fparticulatech[ichem][isolid][i][j][0];

				}	//end loop over number of solids types

				//if the sum of particulate fractions (sumfp) > 1.0
				if(sumfp > 1.0)
				{
					//reset sum of all particulate phases to exactly 1.0
					sumfp = (float)(1.0);

				}	//end if sumfp > 1.0

				//compute the deposition mass potential (g)
				potential = depchemchoutflux[ichem][i][j][0] * dt[idt];

				//Developer's Note:  When computing the mass available
				//                   for deposition, the mass lost to
				//                   transformation processes should
				//                   only include the mass lost from
				//                   the particulate phase.  This will
				//                   require further code development.
				//
				//compute the mass available in the water column (g)
				available = watervol * cchemch[ichem][i][j][0] * sumfp
					- (biochemchoutflux[ichem][i][j][0]
					+ hydchemchoutflux[ichem][i][j][0]
					+ oxichemchoutflux[ichem][i][j][0]
					+ phtchemchoutflux[ichem][i][j][0]
					+ radchemchoutflux[ichem][i][j][0]
					+ vltchemchoutflux[ichem][i][j][0]
					+ udrchemchoutflux[ichem][i][j][0]) * dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//if the deposition potential > available mass
				if(potential > available)
				{
					//scale the deposition flux leaving the water column (g/s)
					depchemchoutflux[ichem][i][j][0] = (float)(available / dt[idt]);

				}	//end if potential > available

				//compute the deposition flux entering the sediment bed (g/s)
				depchemchinflux[ichem][i][j][ilayer] = depchemchoutflux[ichem][i][j][0];

			}	//end loop over number of chemicals

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChemicalTransport
}
