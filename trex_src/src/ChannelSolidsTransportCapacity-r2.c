/*---------------------------------------------------------------------
C-  Function:	ChannelSolidsTransportCapacity.c
C-
C-	Purpose/	Determines the equilibrium sediment tranport capacity
C-	Methods:	for each node in the channel network
C-                        
C-
C-  Inputs:	    spgravity[], dqchout[][][], hch[][], sfch[][], ds[]
C-
C-  Outputs:    transcapch[][][]
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  SolidsTransport
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
C-	Date:		21-MAR-2004
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

void ChannelSolidsTransportCapacity()
{
	//local variable declarations/definitions
	int
		isolid;		//loop index for solids number (type)

	float
		hchan,		//depth of water in channel (m)
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		twch,		//top width of channel at top of bank (m)
		achcross,	//cross sectional area of channel at present water depth (m2)
		wp,			//wetted perimeter of channel at present flow depth (m)
		rh,			//hydraulic radius of the channel at the present water depth (m)
		dqsum,		//sum of gross outflows (to compute velocity) (m3/s)
		velocity,	//flow velocity at preent depth (m/s)
		sf,			//friction slope (dimensionless)
		vc,			//critical velocity for erosion (incipient motion threshold) (m/s)
		cw,			//concentration of sediment by weight (Wsed/Wtotal)
		cgm3,		//concentration of sediment (mass/volume) (g/m3)
		g;			//gravitational acceleration (9.81) (m/s2)

	//initialize value for gravitational acceleration (m/s2)
	g = (float)(9.81);

    //loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Assign present channel depth (m)
			hchan = hch[i][j];

			//Assign channel characteristics between nodes
			bwch = bwidth[i][j];			//bottom width (m)
			hbch = hbank[i][j];				//bank height (m)
			sslope = sideslope[i][j];		//side slope (dimensionless)
			twch = twidth[i][j];			//channel top width at top of bank (m)

			//Compute channel characteristics...
			//
			//if the flow depth is greater than zero
			if(hchan > 0.0)
			{
				//if the flow depth <= bank height
				if(hchan <= hbch)
				{
					//Cross-sectional area for trapezoidal, triangular, or rectangular channel
					achcross = (bwch + sslope * hchan) * hchan;

					//Wetted perimeter for trapezoidal, triangular, or rectangular channel
					wp = (float)(bwch + 2.0 * hchan * sqrt(1.0 + sslope * sslope));
				}
				else	//Flow depth greater than channel depth
				{
					//Cross-sectional area for trapezoidal, triangular, or rectangular channel
					//plus rectangular area above bank height
					achcross = (bwch + sslope * hbch) * hbch + (hchan - hbch) * twch;

					//Wetted perimeter for trapezoidal, triangular, or rectangular channel
					//plus rectangular perimeter above bank height
					wp = (float)(bwch + 2.0 * hbch * sqrt(1.0 + sslope * sslope)
						+ 2.0 * (hchan - hbch));

				}	//end if hchan <= bank height

				//Compute hydraulic radius (m)
				rh = achcross / wp;

				//compute sum of all gross channel outflows...
				//
				//initialize sum of outflows to zero
				dqsum = 0.0;

				//loop over directions (1-8)
				for(k=1; k<=8; k++)
				{
					//compute sum of outflows (m3/s)
					dqsum = dqsum + dqchout[i][j][k];

				}	//end loop over outflows

				//add flow out domain boundary (k=10)
				dqsum = dqsum + dqchout[i][j][10];

				//set flow velocity (m/s)
				velocity = dqsum / achcross;
			}
			else	//else hchan = 0.0 (it can't be < 0.0)
			{
				//set hydraulic radius to zero
				rh = 0.0;

				//set the sum of outflows to zero
				dqsum = 0.0;

				//set flow velocity to zero
				velocity = 0.0;

			}	//end if hchan > 0.0

			//set friction slope (dimensionless)
			sf = (float)(fabs(sfch[i][j]));

			//loop over number of solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Note:  A modified form of the Engelund and Hansen
				//       transport capacity is used and includes an
				//       explicit incipient motion threshold (vc)...
				//
				//set the incipient motion threshold (vc = Vcritical)
				vc = vcch[isolid];

				//if the flow velocity exceeds the particle critical velocity
				if(velocity > vc)
				{
					//Concentration by weight (weight sediment / weight total)
					cw = (float)(0.05 * (spgravity[isolid]/(spgravity[isolid]-1.0))
						* ((velocity-vc)*sf)/sqrt((spgravity[isolid]-1.0)*g*ds[isolid])
						* sqrt((rh*sf)/((spgravity[isolid]-1.0)*ds[isolid])));
				}
				else	//else velocity <= vc (Note: vc = vcch[])
				{
					//set the concentration (transport capacity) to zero
					cw = 0.0;

				}	//end if velocity > vc (Note: vc = vcch[])

				//Convert from Cw to Cg/m3 (g/m3) (Julien, 1998; p. 174)
				cgm3 = (float)((1000000.0 * spgravity[isolid] * cw)
					/ (spgravity[isolid] + (1.0 - spgravity[isolid]) * cw));

				//Compute transport capacity (g/s)
				transcapch[isolid][i][j] = dqsum * cgm3;

			}	//end loop over solids

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to SolidsTransport
}
