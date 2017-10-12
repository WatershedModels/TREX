/*---------------------------------------------------------------------
C-  Function:	ComputeFinalStateChemical.c
C-
C-  Purpose/
C-  Methods:    Computes the final state of chemical variables
C-              at the end of the simulation.
C-  Inputs:     
C-
C-  Outputs:    
C-
C-  Controls:   ksim
C-
C-  Calls:      None
C-
C-  Called by:  ComputeFinalState
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-				Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		12-SEP-2004
C-
C-	Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       13-JUN-2008
C-
C-	Revisions:  Added calculation of chemical mass leaving surface soil
C-              and sediment by infiltration and transmission loss as
C-              over duration of sumulation.
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

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void ComputeFinalStateChemical()
{
	//local variable declarations
	int
		ichem,		//loop index for chemical type
		chanlink,	//channel link number
		channode,	//channel node number
		ilayer;		//layer number in soil or sediment stack

	float
		hflow,		//flow depth of channel (m)
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achcross,	//cross section area of channel (m)
		achsurf;	//surface area of channel (m)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Compute the chemical mass in the water column
				//
				//if the cell is an overland cell
				if(imask[i][j] == 1)
				{
					//No channel exists, channel surface area is zero
					achsurf = 0.0;
				}
				//else if the cell is a channel cell
				else if(imask[i][j] == 2)
				{
					//Get channel link and node
					chanlink = link[i][j];
					channode = node[i][j];

					//Compute area of channel within cell...
					//
					//Assign channel flow depth
					hflow = hch[chanlink][channode];				//channel water depth (m)

					//Assign channel characteristics
					lch = chanlength[chanlink][channode];			//channel length (m) (includes sinuosity)
					twch = twidth[chanlink][channode];				//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;

				}	//end if imask == 1

				//loop over chemicals
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//Compute chemical mass in this overland cell (kg)
					finalchemov[ichem][i][j][0] = cchemov[ichem][i][j][0]
						* hov[i][j] * (w * w - achsurf) / 1000.0;

					//Compute cumulative chemical mass in the overland plane (kg)
					finalchemmassov[ichem][0] = finalchemmassov[ichem][0]
						+ finalchemov[ichem][i][j][0];

					//Compute the chemical mass in the soil stack
					//
					//loop over number of layers
					for(k=1; k<=nstackov[i][j]; k++)
					{
						//Compute chemical mass in this soil cell (kg)
						finalchemov[ichem][i][j][k] = cchemov[ichem][i][j][k]
							* vlayerov[i][j][k] / 1000.0;

						//Compute cumulative chemical mass in the entire soil layer (kg)
						finalchemmassov[ichem][k] = finalchemmassov[ichem][k]
							+ finalchemov[ichem][i][j][k];

					}	//end loop over layers

					//compute the cumulative chemical mass leaving water column by deposition over domain (kg)
					totaldepchemoutov[ichem] = totaldepchemoutov[ichem]
						+ depchemovoutmass[ichem][i][j][0];

					//compute the cumulative chemical mass leaving water column by infiltration over domain (kg)
					totalinfchemoutov[ichem][0] = totalinfchemoutov[ichem][0]
						+ infchemovoutmass[ichem][i][j][0];

					//compute the cumulative chemical mass entering water column by erosion over domain (kg)
					totalerscheminov[ichem] = totalerscheminov[ichem]
						+ erschemovinmass[ichem][i][j][0];

					//compute the cumulative chemical mass entering water column by porewater release over domain (kg)
					totalpwrcheminov[ichem] = totalpwrcheminov[ichem]
						+ pwrchemovinmass[ichem][i][j][0];

					//compute the cumulative chemical mass entering water column by transformations over domain (kg)
					totaltnscheminov[ichem] = totaltnscheminov[ichem]
						+ biochemovinmass[ichem][i][j][0]
						+ hydchemovinmass[ichem][i][j][0]
						+ oxichemovinmass[ichem][i][j][0]
						+ phtchemovinmass[ichem][i][j][0]
						+ radchemovinmass[ichem][i][j][0]
						+ vltchemovinmass[ichem][i][j][0]
						+ udrchemovinmass[ichem][i][j][0];

					//Note:  Because the number of layers in the soil stack can vary
					//       over time, the layer at the surface of the stack at the
					//       is not necessarily the layer through which any porewater
					//       transport occured.  However, for simplicity, the surface
					//       layer at the end of the simulation is used as the frame
					//       of reference for reporting chemical mass transported out
					//       of the surface layer by infiltrating porewater.
					//
					//Developer's Note:  A more consistent way of reporting this would
					//                   be to calculate the elevation of the wetting
					//                   relative to the initial land surface elevation
					//                   and then use the chose the layer number that
					//                   best matches the wetting front elevation for
					//                   cases where nstackov > nstackov0...
					//
					//set the surface layer number
					ilayer = nstackov[i][j];

					//compute the cumulative chemical mass leaving top soil layer by infiltration over domain (kg)
					totalinfchemoutov[ichem][1] = totalinfchemoutov[ichem][1]
						+ infchemovoutmass[ichem][i][j][ilayer];

				}	//end loop over chemicals

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Compute the cumulative chemical mass exiting the domain at all outlets (advection out)
			totaladvchemoutov[ichem][0] = totaladvchemoutov[ichem][0]
				+ advchemovoutmass[ichem][iout[i]][jout[i]][10];

			//Compute the cumulative chemical mass entering the domain at all outlets (advection in)
			totaladvcheminov[ichem][0] = totaladvcheminov[ichem][0]
				+ advchemovinmass[ichem][iout[i]][jout[i]][10];

			//Compute the cumulative chemical mass exiting the domain at all outlets (dispersion out)
			totaldspchemoutov[ichem][0] = totaldspchemoutov[ichem][0]
				+ dspchemovoutmass[ichem][iout[i]][jout[i]][10];

			//Compute the cumulative chemical mass entering the domain at all outlets (dispersion in)
			totaldspcheminov[ichem][0] = totaldspcheminov[ichem][0]
				+ dspchemovinmass[ichem][iout[i]][jout[i]][10];

		}	//end loop over chemicals

	}	//end loop over outlets

	//if channels are simulated
	if(chnopt == 1)
	{
		//Loop over links
		for(i=1; i<=nlinks; i++)
		{
			//Loop over nodes
			for(j=1; j<=nnodes[i]; j++)
			{
				//Compute area of channel within cell...
				//
				//Assign channel flow depth
				hflow = hch[i][j];				//channel water depth (m)

				//Assign channel characteristics
				bwch = bwidth[i][j];			//channel bottom width (m)
				hbch = hbank[i][j];				//channel bank height (m)
				sslope = sideslope[i][j];		//channel side slope
				lch = chanlength[i][j];			//channel length (m) (includes sinuosity)

				//if flow depth <= bank height (cell is not flooded)
				if(hflow <= hbch)
				{
					//channel cross-sectional area at flow depth (m2)
					achcross = (bwch + sslope * hflow) * hflow;
				}
				else	//cell is flooded
				{
					//channel top width of channel at top of bank (m)
					twch = twidth[i][j];

					//channel cross-sectional area at top of bank
					//plus retangular area above the bank height (m2)
					achcross = (bwch + sslope * hbch) * hbch
						+ (hflow - hbch) * twch;

				}	//end if hflow <= hbch

				//loop over chemicals
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//Compute chemical mass in this channel node (kg) (water column)
					finalchemch[ichem][i][j][0] = cchemch[ichem][i][j][0]
						* achcross * lch / 1000.0;

					//Compute cumulative chemical mass in the channel network (kg) (water column)
					finalchemmassch[ichem][0] = finalchemmassch[ichem][0]
						+ finalchemch[ichem][i][j][0];

					//Compute the bedded chemical in the sediment stack
					//
					//loop over number of layers
					for(k=1; k<=nstackch[i][j]; k++)
					{
						//Compute bedded chemical mass in this sediment node (kg)
						finalchemch[ichem][i][j][k] = cchemch[ichem][i][j][k]
							* vlayerch[i][j][k] / 1000.0;

						//Compute cumulative bedded chemical mass in the entire sediment layer (kg)
						finalchemmassch[ichem][k] = finalchemmassch[ichem][k]
							+ finalchemch[ichem][i][j][k];

					}	//end loop over layers

					//compute the cumulative chemical mass leaving water column by deposition over domain (kg)
					totaldepchemoutch[ichem] = totaldepchemoutch[ichem]
						+ depchemchoutmass[ichem][i][j][0];

					//compute the cumulative chemical mass leaving water column by infiltration over domain (kg)
					totalinfchemoutch[ichem][0] = totalinfchemoutch[ichem][0]
						+ infchemchoutmass[ichem][i][j][0];

					//compute the cumulative chemical mass entering water column by erosion over domain (kg)
					totalerscheminch[ichem] = totalerscheminch[ichem]
						+ erschemchinmass[ichem][i][j][0];

					//compute the cumulative chemical mass entering water column by powewater over domain (kg)
					totalpwrcheminch[ichem] = totalpwrcheminch[ichem]
						+ pwrchemchinmass[ichem][i][j][0];

					//compute the cumulative chemical mass entering water column by transformations over domain (kg)
					totaltnscheminch[ichem] = totaltnscheminch[ichem]
						+ biochemchinmass[ichem][i][j][0]
						+ hydchemchinmass[ichem][i][j][0]
						+ oxichemchinmass[ichem][i][j][0]
						+ phtchemchinmass[ichem][i][j][0]
						+ radchemchinmass[ichem][i][j][0]
						+ vltchemchinmass[ichem][i][j][0]
						+ udrchemchinmass[ichem][i][j][0];

					//Note:  Because the number of layers in the sediment bed can vary
					//       over time, the layer at the surface of the stack at the
					//       is not necessarily the layer through which any porewater
					//       transport occured.  However, for simplicity, the surface
					//       layer at the end of the simulation is used as the frame
					//       of reference for reporting chemical mass transported out
					//       of the surface layer by infiltrating porewater.
					//
					//Developer's Note:  A more consistent way of reporting this would
					//                   be to calculate the elevation of the wetting
					//                   relative to the initial bed surface elevation
					//                   and then use the chose the layer number that
					//                   best matches the wetting front elevation for
					//                   cases where nstackch > nstackch0...
					//
					//set the surface layer number
					ilayer = nstackch[i][j];

					//compute the cumulative chemical mass leaving top sediment layer by infiltration over domain (kg)
					totalinfchemoutch[ichem][1] = totalinfchemoutch[ichem][1]
						+ infchemchoutmass[ichem][i][j][ilayer];

					//if the link is an outlet link and it is the last node of the link
					if(qchoutlet[i] > 0 && j == nnodes[i])
					{
						//Compute the cumulative chemical mass exiting the domain for all outlets
						totaladvchemoutch[ichem][0] = totaladvchemoutch[ichem][0]
							+ advchemchoutmass[ichem][i][j][10];

						//Compute the cumulative chemical mass entering the domain for all outlets
						totaladvcheminch[ichem][0] = totaladvcheminch[ichem][0]
							+ advchemchinmass[ichem][i][j][10];

						//Compute the cumulative chemical mass exiting the domain for all outlets
						totaldspchemoutch[ichem][0] = totaldspchemoutch[ichem][0]
							+ dspchemchoutmass[ichem][i][j][10];

						//Compute the cumulative chemical mass entering the domain for all outlets
						totaldspcheminch[ichem][0] = totaldspcheminch[ichem][0]
							+ dspchemchinmass[ichem][i][j][10];

					}	//end if qchoutlet[i] > 0 and j = nnodes[i]

				}	//end loop over chemicals

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt == 1

//End of function: Return to ComputeFinalState
}
