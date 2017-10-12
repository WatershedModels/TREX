/*---------------------------------------------------------------------
C-  Function:	ComputeFinalStateSolids.c
C-
C-  Purpose/    Computes the final solids concentrations at the
C-  Methods:    end of the simulation.
C-
C-
C-  Inputs:     csedov[][][][] (at tend),
C-              csedch[][][][] (at tend)
C-
C-  Outputs:    finalsolidsov[][][],
C-              finalsolidsch[][][]
C-
C-  Controls:   imask, chnopt
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
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		20-MAR-2004
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

void ComputeFinalStateSolids()
{
	//local variable declarations
	int
		isolid,		//loop index for solids (particle) type
		chanlink,	//channel link number
		channode;	//channel node number

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
				//Compute the suspended solids mass in the water column
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

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Compute suspended solids mass in this overland cell (kg)
					finalsolidsov[isolid][i][j][0] = (float)(csedov[isolid][i][j][0]
						* hov[i][j] * (w * w - achsurf) / 1000.0);

					//Compute total suspended solids mass in this overland cell (kg)
					finalsolidsov[0][i][j][0] = finalsolidsov[0][i][j][0]
						+ (float)(csedov[isolid][i][j][0]
						* hov[i][j] * (w * w - achsurf) / 1000.0);

					//Compute cumulative suspended solids mass in the overland plane (kg)
					finalsolidsmassov[isolid][0] = finalsolidsmassov[isolid][0]
						+ finalsolidsov[isolid][i][j][0];

					//Compute total suspended solids sums for each transport process...
					//
					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//total solids overland advective inflow (kg)
						advsedovinmass[0][i][j][k] = advsedovinmass[0][i][j][k]
							+ advsedovinmass[isolid][i][j][k];

						//total solids overland advective outflow (kg)
						advsedovoutmass[0][i][j][k] = advsedovoutmass[0][i][j][k]
							+ advsedovoutmass[isolid][i][j][k];

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//total solids overland dispersive inflow (kg)
						dspsedovinmass[0][i][j][k] = dspsedovinmass[0][i][j][k]
							+ dspsedovinmass[isolid][i][j][k];

						//total solids overland dispersive outflow (kg)
						dspsedovoutmass[0][i][j][k] = dspsedovoutmass[0][i][j][k]
							+ dspsedovoutmass[isolid][i][j][k];

					}	//end loop over dispersive flow sources

					//total solids overland erosion inflow (kg)
					erssedovinmass[0][i][j][0] = erssedovinmass[0][i][j][0]
						+ erssedovinmass[isolid][i][j][0];

					//total solids overland deposition inflow (kg)
					depsedovinmass[0][i][j][0] = depsedovinmass[0][i][j][0]
						+ depsedovinmass[isolid][i][j][0];

					//total solids overland erosion outflow (kg)
					erssedovoutmass[0][i][j][0] = erssedovoutmass[0][i][j][0]
						+ erssedovoutmass[isolid][i][j][0];

					//total solids overland deposition outflow (kg)
					depsedovoutmass[0][i][j][0] = depsedovoutmass[0][i][j][0]
						+ depsedovoutmass[isolid][i][j][0];

					//Compute the bedded solids mass in the soil stack
					//
					//loop over number of layers
					for(k=1; k<=nstackov[i][j]; k++)
					{
						//Compute bedded solids mass in this overland layer (kg)
						finalsolidsov[isolid][i][j][k] = (float)(csedov[isolid][i][j][k]
							* vlayerov[i][j][k] / 1000.0);

						//Compute total bedded solids mass in this overland layer (kg)
						finalsolidsov[0][i][j][k] = finalsolidsov[0][i][j][k]
							+ (float)(csedov[isolid][i][j][k]
							* vlayerov[i][j][k] / 1000.0);

						//Compute cumulative bedded solids mass in the overland plane (kg)
						finalsolidsmassov[isolid][k] = finalsolidsmassov[isolid][k]
							+ finalsolidsov[isolid][i][j][k];

					}	//end loop over layers

					//compute the cumulative solids mass leaving water column by deposition over domain (kg)
					totaldepsedoutov[isolid] = totaldepsedoutov[isolid]
						+ depsedovoutmass[isolid][i][j][0];

					//compute the cumulative total solids mass leaving water column by deposition over domain (kg)
					totaldepsedoutov[0] = totaldepsedoutov[0]
						+ depsedovoutmass[isolid][i][j][0];

					//compute the cumulative solids mass entering water column by erosion over domain (kg)
					totalerssedinov[isolid] = totalerssedinov[isolid]
						+ erssedovinmass[isolid][i][j][0];

					//compute the cumulative total solids mass entering water column by erosion over domain (kg)
					totalerssedinov[0] = totalerssedinov[0]
						+ erssedovinmass[isolid][i][j][0];

				}	//end loop over solids

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Compute the cumulative total solids mass exiting the domain at this outlet
			totaladvsedoutov[0][i] = totaladvsedoutov[0][i]
				+ advsedovoutmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative total solids mass entering the domain at this outlet
			totaladvsedinov[0][i] = totaladvsedinov[0][i]
				+ advsedovinmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative solids mass exiting the domain at all outlets
			totaladvsedoutov[isolid][0] = totaladvsedoutov[isolid][0]
				+ advsedovoutmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative total solids mass exiting the domain at all outlets
			totaladvsedoutov[0][0] = totaladvsedoutov[0][0]
				+ advsedovoutmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative solids mass entering the domain at all outlets
			totaladvsedinov[isolid][0] = totaladvsedinov[isolid][0]
				+ advsedovinmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative total solids mass entering the domain at all outlets
			totaladvsedinov[0][0] = totaladvsedinov[0][0]
				+ advsedovinmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative total solids mass exiting the domain at this outlet
			totaldspsedoutov[0][i] = totaldspsedoutov[0][i]
				+ dspsedovoutmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative total solids mass entering the domain at this outlet
			totaldspsedinov[0][i] = totaldspsedinov[0][i]
				+ dspsedovinmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative solids mass exiting the domain at all outlets
			totaldspsedoutov[isolid][0] = totaldspsedoutov[isolid][0]
				+ dspsedovoutmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative total solids mass exiting the domain at all outlets
			totaldspsedoutov[0][0] = totaldspsedoutov[0][0]
				+ dspsedovoutmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative solids mass entering the domain at all outlets
			totaldspsedinov[isolid][0] = totaldspsedinov[isolid][0]
				+ dspsedovinmass[isolid][iout[i]][jout[i]][10];

			//Compute the cumulative total solids mass entering the domain at all outlets
			totaldspsedinov[0][0] = totaldspsedinov[0][0]
				+ dspsedovinmass[isolid][iout[i]][jout[i]][10];

		}	//end loop over solids

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
					achcross = (float)(bwch + sslope * hflow) * hflow;
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

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Compute suspended solids mass in this channel node (kg)
					finalsolidsch[isolid][i][j][0] = (float)(csedch[isolid][i][j][0]
						* achcross * lch / 1000.0);

					//Compute total suspended solids mass in this channel node (kg)
					finalsolidsch[0][i][j][0] = finalsolidsch[0][i][j][0]
						+ (float)(csedch[isolid][i][j][0]
						* achcross * lch / 1000.0);

					//Compute cumulative suspended solids mass in the channel network (kg)
					finalsolidsmassch[isolid][0] = finalsolidsmassch[isolid][0]
						+ finalsolidsch[isolid][i][j][0];

					//Compute total suspended solids sums for each transport process...
					//
					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//total solids channel advective inflow (kg)
						advsedchinmass[0][i][j][k] = advsedchinmass[0][i][j][k]
							+ advsedchinmass[isolid][i][j][k];

						//total solids channel advective outflow (kg)
						advsedchoutmass[0][i][j][k] = advsedchoutmass[0][i][j][k]
							+ advsedchoutmass[isolid][i][j][k];

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//total solids channel dispersive inflow (kg)
						dspsedchinmass[0][i][j][k] = dspsedchinmass[0][i][j][k]
							+ dspsedchinmass[isolid][i][j][k];

						//total solids channel dispersive outflow (kg)
						dspsedchoutmass[0][i][j][k] = dspsedchoutmass[0][i][j][k]
							+ dspsedchoutmass[isolid][i][j][k];

					}	//end loop over dispersive flow sources

					//total solids channel erosion inflow (kg)
					erssedchinmass[0][i][j][0] = erssedchinmass[0][i][j][0]
						+ erssedchinmass[isolid][i][j][0];

					//total solids channel deposition inflow (kg)
					depsedchinmass[0][i][j][0] = depsedchinmass[0][i][j][0]
						+ depsedchinmass[isolid][i][j][0];

					//total solids channel erosion outflow (kg)
					erssedchoutmass[0][i][j][0] = erssedchoutmass[0][i][j][0]
						+ erssedchoutmass[isolid][i][j][0];

					//total solids channel deposition outflow (kg)
					depsedchoutmass[0][i][j][0] = depsedchoutmass[0][i][j][0]
						+ depsedchoutmass[isolid][i][j][0];

					//Compute the bedded solids mass in the sediment stack
					//
					//loop over number of layers
					for(k=1; k<=nstackch[i][j]; k++)
					{
						//Compute bedded solids mass in this channel layer (kg)
						finalsolidsch[isolid][i][j][k] = (float)(csedch[isolid][i][j][k]
							* vlayerch[i][j][k] / 1000.0);

						//Compute total bedded solids mass in this channel layer (kg)
						finalsolidsch[0][i][j][k] = finalsolidsch[0][i][j][k]
							+ (float)(csedch[isolid][i][j][k]
							* vlayerch[i][j][k] / 1000.0);

						//Compute cumulative bedded solids mass in the channel network (kg)
						finalsolidsmassch[isolid][k] = finalsolidsmassch[isolid][k]
							+ finalsolidsch[isolid][i][j][k];

					}	//end loop over layers

					//compute the cumulative solids mass leaving water column by deposition over domain (kg)
					totaldepsedoutch[isolid] = totaldepsedoutch[isolid]
						+ depsedchoutmass[isolid][i][j][0];

					//compute the cumulative total solids mass leaving water column by deposition over domain (kg)
					totaldepsedoutch[0] = totaldepsedoutch[0]
						+ depsedchoutmass[isolid][i][j][0];

					//compute the cumulative solids mass entering water column by erosion over domain (kg)
					totalerssedinch[isolid] = totalerssedinch[isolid]
						+ erssedchinmass[isolid][i][j][0];

					//compute the cumulative total solids mass entering water column by erosion over domain (kg)
					totalerssedinch[0] = totalerssedinch[0]
						+ erssedchinmass[isolid][i][j][0];

					//if the link is an outlet link and it is the last node of the link
					if(qchoutlet[i] > 0 && j == nnodes[i])
					{
						//Compute the cumulative summed solids mass exiting the domain at this outlet (advection out)
						totaladvsedoutch[0][qchoutlet[i]] = totaladvsedoutch[0][qchoutlet[i]]
							+ advsedchoutmass[isolid][i][j][10];

						//Compute the cumulative individual solids mass exiting the domain for all outlets (advection out)
						totaladvsedoutch[isolid][0] = totaladvsedoutch[isolid][0]
							+ advsedchoutmass[isolid][i][j][10];

						//Compute the cumulative summed solids mass exiting the domain for all outlets (advection out)
						totaladvsedoutch[0][0] = totaladvsedoutch[0][0]
							+ advsedchoutmass[isolid][i][j][10];

						//Compute the cumulative summed solids mass entering the domain at this outlet (advection in)
						totaladvsedinch[0][qchoutlet[i]] = totaladvsedinch[0][qchoutlet[i]]
							+ advsedchinmass[isolid][i][j][10];

						//Compute the cumulative individual solids mass entering the domain for all outlets (advection in)
						totaladvsedinch[isolid][0] = totaladvsedinch[isolid][0]
							+ advsedchinmass[isolid][i][j][10];

						//Compute the cumulative summed solids mass entering the domain for all outlets (advection in)
						totaladvsedinch[0][0] = totaladvsedinch[0][0]
							+ advsedchinmass[isolid][i][j][10];

						//Compute the cumulative summed solids mass exiting the domain at this outlet (dispersion out)
						totaldspsedoutch[0][qchoutlet[i]] = totaldspsedoutch[0][qchoutlet[i]]
							+ dspsedchoutmass[isolid][i][j][10];

						//Compute the cumulative individual solids mass exiting the domain for all outlets (dispersion out)
						totaldspsedoutch[isolid][0] = totaldspsedoutch[isolid][0]
							+ dspsedchoutmass[isolid][i][j][10];

						//Compute the cumulative summed solids mass exiting the domain for all outlets (dispersion out)
						totaldspsedoutch[0][0] = totaldspsedoutch[0][0]
							+ dspsedchoutmass[isolid][i][j][10];

						//Compute the cumulative summed solids mass entering the domain at this outlet (dispersion in)
						totaldspsedinch[0][qchoutlet[i]] = totaldspsedinch[0][qchoutlet[i]]
							+ dspsedchinmass[isolid][i][j][10];

						//Compute the cumulative individual solids mass entering the domain for all outlets (dispersion in)
						totaldspsedinch[isolid][0] = totaldspsedinch[isolid][0]
							+ dspsedchinmass[isolid][i][j][10];

						//Compute the cumulative summed solids mass entering the domain for all outlets (dispersion in)
						totaldspsedinch[0][0] = totaldspsedinch[0][0]
							+ dspsedchinmass[isolid][i][j][10];

					}	//end if qchoutlet[i] > 0 and j = nnodes[i]

				}	//end loop over solids

			}	//end loop over nodes

		}	//end loop over links

	}	//end if chnopt == 1

//End of function: Return to ComputeFinalState
}
