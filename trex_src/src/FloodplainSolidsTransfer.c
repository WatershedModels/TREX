/*---------------------------------------------------------------------
C-  Function:	FloodplainSolidsTransfer.c
C-
C-	Purpose/	Computes solids transfer between overland and channel
C-	Methods:	portion of floodplain cells (cells with a channel,
C-				imask > 1).  Solids move from overland to channel or
C-				from channel to overland acording to the gross water
C-              flow (dqovout, dqchin, etc.).
C-
C-
C-  Inputs:	    csedov[][][][], dqovin[][][9], dqovout[][][9],
C-              csedch[][][][], dqchin[][][9], dqchout[][][9],
C-              advovopt, advchopt
C-
C-  Outputs:    advsedovinflow[][][][9], advsedovoutflow[][][][9]
C-              advsedovinflux[][][][9], advsedovoutflux[][][][9]
C-              advsedchinflow[][][][9], advsedchoutflow[][][][9]
C-              advsedchinflux[][][][9], advsedchoutflux[][][][9]
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
C-	Date:		05-MAY-2005
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

void FloodplainSolidsTransfer()
{
	//local variable declarations/definitions
	int
		isolid,			//loop index for solids number (type)
		row,			//row number of node
		col;			//column number of node

	float
		hchan,			//new depth of water in channel (at time t + dt) (m)
		bwch,			//bottom width of channel (m)
		hbch,			//bank height of channel (m)
		sslope,			//side slope of channel bank (dimensionless)
		lch,			//channel length (m) (includes sinuosity)
		twch,			//top width of channel at top of bank (m)
		achcross,		//cross sectional area of channel (m2)
		achsurf,		//surface area of channel portion of cell (m2)
		aovsurf,		//surface area of overland portion of cell (m2)
		watervolov,		//water column volume of overland portion of cell (m3)
		watervolch,		//water column volume of channel portion of cell (m3)
		outflux;		//solids outflux (g/s) (sum for sources 0, 1, 3, 5, 7, and 10)

	double
		potential,	//solids mass transport potential (g)
		available;	//solids mass available for transport (g)

    //loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//get row of present node
			row = ichnrow[i][j];

			//get column of present node
			col = ichncol[i][j];

			//Assign channel characteristics
			bwch = bwidth[i][j];				//bottom width (m)
			hbch = hbank[i][j];					//bank height (m)
			sslope = sideslope[i][j];			//side slope (dimensionless)
			lch = chanlength[i][j];				//channel length (m) (includes sinuosity)
			twch = twidth[i][j];				//channel top width at bank height (m)

			//Compute overland water column volume...
			//
			//Surface area of channel portion of cell (m2)
			achsurf = twch * lch;

			//compute surface area (m2)
			aovsurf = w * w - achsurf;

			//compute present water column volume (m3) (at time t)
			watervolov = hov[row][col] * aovsurf;

			//Compute channel water column volume...
			//
			//Assign local water depth (m)
			hchan = hch[i][j];

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

			//compute channel water column volume (m3) (at time t)
			watervolch = achcross * lch;

			//Compute Floodplain Advective Fluxes...
			//
			//Source 9: floodplain transfer
			//
			//Note:  Flows from the overland plane (advoutflowov and
			//       advinflowch) are scaled by the overland process
			//       scale factor.  Flows from the channel (advoutflowch
			//       and advinflowov) are scaled by the channel process
			//       scale factor.
			//
			//Set floodplain flows...
			//
			//multiply overland advective inflow by channel process scale factor
			advinflowov[row][col][9] = dqovin[row][col][9] * advchscale;

			//multiply overland advective outflow by overland process scale factor
			advoutflowov[row][col][9] = dqovout[row][col][9] * advovscale;

			//multiply channel advective inflow by overland process scale factor
			advinflowch[i][j][9] = dqchin[i][j][9] * advovscale;

			//multiply channel advective outflow by channel process scale factor
			advoutflowch[i][j][9] = dqchout[i][j][9] * advchscale;

			//Set floodplain fluxes...
			//
			//loop over solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//compute the overland inflowing sediment flux (g/s)
				advsedovinflux[isolid][row][col][9] = advinflowov[row][col][9]
					* csedch[isolid][i][j][0];

				//compute the overland outflowing sediment flux (g/s)
				advsedovoutflux[isolid][row][col][9] = advoutflowov[row][col][9]
					* csedov[isolid][row][col][0];

				//compute the channel inflowing sediment flux (g/s)
				advsedchinflux[isolid][i][j][9] = advinflowch[i][j][9]
					* csedov[isolid][row][col][0];

				//compute the channel outflowing sediment flux (g/s)
				advsedchoutflux[isolid][i][j][9] = advoutflowch[i][j][9]
					* csedch[isolid][i][j][0];

				//Determine if outflow potential exceeds available mass...
				//
				//Overland solids floodplain transfer potential...
				//
				//potential (g) for overland solids transfer
				potential = advsedovoutflux[isolid][row][col][9] * dt[idt];

				//Compute overland outflux (Sources 0, 1, 3, 5, 7, and 10)
				outflux = advsedovoutflux[isolid][row][col][0]
					+ advsedovoutflux[isolid][row][col][1]
					+ advsedovoutflux[isolid][row][col][3]
					+ advsedovoutflux[isolid][row][col][5]
					+ advsedovoutflux[isolid][row][col][7]
					+ advsedovoutflux[isolid][row][col][10];

				//compute the overland mass available for transfer (g)
				available = csedov[isolid][row][col][0] * watervolov
					- (depsedovoutflux[isolid][row][col][0] + outflux)
					* dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//Note:  At this point we need to check whether there
				//       is enough solids of this type available in
				//       the water column to satify the full transfer
				//       potential.  If not, the transfer potential
				//       for this solids type is limited to available
				//       mass...
				//
				//if the overall outflux potential exceeds the available mass
				if(potential > available)
				{
					//scale the solids floodplain transfer flux (g/s) (source 9)
					advsedovoutflux[isolid][row][col][9] = (float)(available / potential
							* advsedovoutflux[isolid][row][col][9]);

				}	//end if potential > available

				//Channel solids floodplain transfer potential...
				//
				//potential (g) for channel solids transfer
				potential = advsedchoutflux[isolid][i][j][9] * dt[idt];

				//Compute channel outflux (Sources 0, 1, 3, 5, 7, and 10)
				outflux = advsedchoutflux[isolid][i][j][0]
					+ advsedchoutflux[isolid][i][j][1]
					+ advsedchoutflux[isolid][i][j][3]
					+ advsedchoutflux[isolid][i][j][5]
					+ advsedchoutflux[isolid][i][j][7]
					+ advsedchoutflux[isolid][i][j][10];

				//compute the overland mass available for transfer (g)
				available = csedch[isolid][i][j][0] * watervolch
					- (depsedchoutflux[isolid][i][j][0] + outflux)
					* dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//Note:  At this point we need to check whether there
				//       is enough solids of this type available in
				//       the water column to satify the full transfer
				//       potential.  If not, the transfer potential
				//       for this solids type is limited to available
				//       mass...
				//
				//if the overall outflux potential exceeds the available mass
				if(potential > available)
				{
					//scale the solids floodplain transfer flux (g/s) (source 9)
					advsedchoutflux[isolid][i][j][9] = (float)(available / potential
							* advsedchoutflux[isolid][i][j][9]);

				}	//end if potential > available

			}	//end loop solids

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to SolidsTransport
}
