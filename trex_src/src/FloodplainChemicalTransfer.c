/*---------------------------------------------------------------------
C-  Function:	FloodplainChemicalTransfer.c
C-
C-	Purpose/	Computes chemical transfer between overland and channel
C-	Methods:	portion of floodplain cells (cells with a channel,
C-				imask > 1).  Chemicals move from overland to channel or
C-				from channel to overland acording to the gross water
C-              flow (dqovout, dqchin, etc.).
C-
C-
C-  Inputs:	    cchemov[][][][], dqovin[][][9], dqovout[][][9],
C-              cchemch[][][][], dqchin[][][9], dqchout[][][9],
C-
C-  Outputs:    advchemovinflux[][][][9], advchemovoutflux[][][][9]
C-              advchemchinflux[][][][9], advchemchoutflux[][][][9]
C-
C-  Controls:   None
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

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

//trex global variable declarations for environmental conditions
#include "trex_environmental_declarations.h"

void FloodplainChemicalTransfer()
{
	//local variable declarations/definitions
	int
		ichem,				//loop index for chemical number (type)
		row,				//row number of node
		col;				//column number of node

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
		iflux,			//chemical infiltration outflux (g/s)
		outflux;		//chemical outflux (g/s) (sum for sources 0, 1, 3, 5, 7, and 10)

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
			//Source 9: floodplain tranfer
			//
			//Set floodplain fluxes...
			//
			//loop over chemicals
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//compute the overland inflowing chemical flux (g/s)
				advchemovinflux[ichem][row][col][9] = advinflowov[row][col][9]
					* cchemch[ichem][i][j][0];

				//compute the overland outflowing chemical flux (g/s)
				advchemovoutflux[ichem][row][col][9] = advoutflowov[row][col][9]
					* cchemov[ichem][row][col][0];

				//compute the channel inflowing chemical flux (g/s)
				advchemchinflux[ichem][i][j][9] = advinflowch[i][j][9]
					* cchemov[ichem][row][col][0];

				//compute the channel outflowing chemical flux (g/s)
				advchemchoutflux[ichem][i][j][9] = advoutflowch[i][j][9]
					* cchemch[ichem][i][j][0];

				//Determine if outflow potential exceeds available mass...
				//
				//Overland chemical floodplain transfer potential...
				//
				//potential (g) for overland chemicals transfer
				potential = advchemovoutflux[ichem][row][col][9] * dt[idt];

				//if infiltration is simulated (infopt > 0)
				if(infopt > 0)
				{
					//Cell chemical infiltration flux (not available for transfer) (g/s)
					iflux = infchemovoutflux[ichem][row][col][0];
				}
				else	//else infiltration is not simulated (infopt = 0)
				{
					//Set cell chemical infiltration flux to zero
					iflux = 0.0;

				}	//end if infopt > 0

				//Compute overland outflux (Sources 0, 1, 3, 5, 7, and 10) (g/s)
				outflux = advchemovoutflux[ichem][row][col][0]
					+ advchemovoutflux[ichem][row][col][1]
					+ advchemovoutflux[ichem][row][col][3]
					+ advchemovoutflux[ichem][row][col][5]
					+ advchemovoutflux[ichem][row][col][7]
					+ advchemovoutflux[ichem][row][col][10];

				//compute the overland mass available for transfer (g)
				available = cchemov[ichem][row][col][0] * watervolov
					- (depchemovoutflux[ichem][row][col][0]
					+ biochemovoutflux[ichem][row][col][0]
					+ hydchemovoutflux[ichem][row][col][0]
					+ oxichemovoutflux[ichem][row][col][0]
					+ phtchemovoutflux[ichem][row][col][0]
					+ radchemovoutflux[ichem][row][col][0]
					+ vltchemovoutflux[ichem][row][col][0]
					+ udrchemovoutflux[ichem][row][col][0] + iflux + outflux)
					* dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//Note:  At this point we need to check whether there
				//       is enough chemical of this type available in
				//       the water column to satify the full transfer
				//       potential.  If not, the transfer potential
				//       for this chemical type is limited to available
				//       mass...
				//
				//if the overall outflux potential exceeds the available mass
				if(potential > available)
				{
					//scale the chemical floodplain transfer flux (g/s) (source 9)
					advchemovoutflux[ichem][row][col][9] = (float)(available / potential
							* advchemovoutflux[ichem][row][col][9]);

				}	//end if potential > available

				//Channel chemical floodplain transfer potential...
				//
				//potential (g) for channel chemicals transfer
				potential = advchemchoutflux[ichem][i][j][9] * dt[idt];

				//if channel transmission loss is simulated (ctlopt > 0)
				if(ctlopt > 0)
				{
					//Node chemical infiltration flux (not available for transfer) (g/s)
					iflux = infchemchoutflux[ichem][i][j][0];
				}
				else	//else channel transmission loss is not simulated (ctlopt = 0)
				{
					//Set cell chemical infiltration flux to zero
					iflux = 0.0;

				}	//end if infopt > 0

				//Compute channel outflux (Sources 0, 1, 3, 5, 7, and 10) (g/s)
				outflux = advchemchoutflux[ichem][i][j][0]
					+ advchemchoutflux[ichem][i][j][1]
					+ advchemchoutflux[ichem][i][j][3]
					+ advchemchoutflux[ichem][i][j][5]
					+ advchemchoutflux[ichem][i][j][7]
					+ advchemchoutflux[ichem][i][j][10];

				//compute the overland mass available for transfer (g)
				available = cchemch[ichem][i][j][0] * watervolch
					- (depchemchoutflux[ichem][i][j][0]
					+ biochemchoutflux[ichem][i][j][0]
					+ hydchemchoutflux[ichem][i][j][0]
					+ oxichemchoutflux[ichem][i][j][0]
					+ phtchemchoutflux[ichem][i][j][0]
					+ radchemchoutflux[ichem][i][j][0]
					+ vltchemchoutflux[ichem][i][j][0]
					+ udrchemchoutflux[ichem][i][j][0] + iflux + outflux)
					* dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//Note:  At this point we need to check whether there
				//       is enough chemical of this type available in
				//       the water column to satify the full transfer
				//       potential.  If not, the transfer potential
				//       for this chemical type is limited to available
				//       mass...
				//
				//if the overall outflux potential exceeds the available mass
				if(potential > available)
				{
					//scale the chemical floodplain transfer flux (g/s) (source 9)
					advchemchoutflux[ichem][i][j][9] = (float)(available / potential
							* advchemchoutflux[ichem][i][j][9]);

				}	//end if potential > available

			}	//end loop chemicals

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChemicalTransport
}
