/*---------------------------------------------------------------------
C-  Function:	ChannelChemicalAdvection.c
C-
C-	Purpose/	Compute the advective flux of chemicals in the channel
C-	Methods:	network.  Advection transports all chemical phases:
C-              dissolved, bound, and particulate.
C-
C-
C-  Inputs:	    advinflowch[][][], advoutflowch[][][]
C-
C-  Outputs:    advchemchinflux[][][][][], advchemchoutflux[][][][][]
C-
C-  Controls:   dbcopt, cwchopt
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

void ChannelChemicalAdvection()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		inlink,		//link number of inflowing node
		innode;		//node number of inflowing node

	float
		hchan,		//depth of water in channel (m)
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		twch,		//top width of channel at top of bank (m)
		achcross,	//cross sectional area of channel at present water depth (m2)
		watervol;	//water column volume of node (at time t) (m3)

	float
		cin[11],	//chemical concentration of inflowing cell/node (g/m3)
		load;		//load (source or sink) (g/s)

	double
		potential,	//solids mass transport potential (g)
		available;	//solids mass available for transport (g)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Initialize arrays for use this time step...
			//
			//Loop over all directions/sources
			for(k=0; k<=10; k++)
			{
				//initialize inflowing concentration array
				cin[k] = 0.0;

			}	//end loop over directions/sources

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

			//loop over number of chemical types
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//if it is the first node of the link
				if(j==1)
				{
					//loop over directions
					for(k=1; k<=8; k++)
					{
						//Set the inflowing chemical concentration
						//
						//Note: The following code assumes that domain
						//      boundaries cannot be upstream branches
						//      If a boundary existed, the logic below
						//      would need to check if nupbranches[i][k]
						//      was equal to zero and then to set the
						//      upstream concentration to the boundary
						//      condition because cchemch[][][0] is not
						//      defined for uplink=0 and upnode=0.
						//
						//if an upstream branch exists in this direction
						if(nupbranches[i][k] > -1)
						{
							//set the inflowing (upstream) link
							inlink = nupbranches[i][k];

							//set the inflowing (upstream) node
							innode = nnodes[inlink];

							//set the inflowing chemical concentration (g/m3)
							cin[k] = cchemch[ichem][inlink][innode][0];
						}
						//else if a downstream node exists in this direction
						else if(k == downdirection[i][j][0])
						{
							//set the inflowing chemical concentration (g/m3)
							cin[k] = cchemch[ichem][i][j+1][0];
						}
						else	//else no branch exists in this direction
						{
							//set the upstream chemical concentration to zero
							cin[k] = 0.0;

						}	//end if upbranches[i][k]

					}	//end loop over directions
				}
				//else if the node is an interior node (2 through nnodes[i]-1)
				else if(j >= 2 && j <= nnodes[i]-1)
				{
					//loop over directions
					for(k=1; k<=8; k++)
					{
						//if an upstream node is in this direction
						if(k == updirection[i][j][0])
						{
							//set the inflowing chemical concentration
							cin[k] = cchemch[ichem][i][j-1][0];
						}
						//else a downstream node is in this direction
						else if(k == downdirection[i][j][0])
						{
							//set the inflowing chemical concentration
							cin[k] = cchemch[ichem][i][j+1][0];
						}
						else	//else no node is in this direction
						{
							//set the upstream chemical concentration to zero
							cin[k] = 0.0;

						}	//end if k = updirection[i][k][0]

					}	//end loop over directions
				}
				//else it is the last node of the link (j = nnodes[i])
				else
				{
					//loop over directions
					for(k=1; k<=8; k++)
					{
						//if a downstream branch exists in this direction
						if(ndownbranches[i][k] > -1)
						{
							//set the inflowing (downstream) link
							inlink = ndownbranches[i][k];

							//set the inflowing (downstream) node
							innode = 1;

							//set the inflowing chemical concentration (g/m3)
							cin[k] = cchemch[ichem][inlink][innode][0];
						}
						//else if an upstream node exists in this direction
						else if(k == updirection[i][j][0])
						{
							//set the inflowing chemical concentration (g/m3)
							cin[k] = cchemch[ichem][i][j-1][0];
						}
						else	//else no branch exists in this direction
						{
							//set the upstream chemical concentration to zero
							cin[k] = 0.0;

						}	//end if upbranches[i][k]

					}	//end loop over directions

				}	//end if j = 1 (if/then/else if/else)

				//Conditions for Source 9 (floodplain)...
				//
				//Note:  Concentrations for floodplain transfers
				//       are set in FloodplainChemicalTransfer.
				//
				//Conditions for Source 10 (domain boundary)...
				//
				//Note:  A link is an outlet link only if the
				//       outlet number is greater than zero.
				//
				//if the link is an outlet link and it is the last node of the link
				if(qchoutlet[i] > 0 && j == nnodes[i])
				{
					//if the domain BC option is zero (normal depth condition)
					if(dbcopt[qchoutlet[i]] == 0)
					{
						//the boundary concentration is zero
						cin[10] = 0.0;
					}
					else	//else dbcopt[] = 1 (boundary concentration specified)
					{
						//set the boundary concentration to specified value
						cin[10] = cbcinterp[ichem][qchoutlet[i]];

					}	//end if dbcopt[] = 0
				}
				else	//else the link is not an outlet
				{
					//set the boundary concentration to zero
					cin[10] = 0.0;

				}	//end if qchoutlet[i] > 0 && j == nnodes[i]

				//Compute Advective Fluxes...
				//
				//Source 0: point sources
				//
				//loop over point sources
				for(k=1; k<=ncwch[ichem]; k++)
				{
					//if the current node has a point source (link/node match)
					if(cwchlink[ichem][k] == i && cwchnode[ichem][k] == j)
					{
						//Developer's Note:  The code below assumes that only
						//                   one point source/sink is specified
						//                   for a cell.  If more than one point
						//                   source/sink is specified, then the
						//                   advective flux would need to be
						//                   computed as a sum for all sources
						//                   and sinks...
						//
						//if the channel chemical loading option is zero
						if(cwchopt[ichem][k] == 0)
						{
							//Chemical forcing functions are specifed as loads (kg/day)
							//
							//compute the load value (g/s) (could be a source or a sink)
							load = cwchinterp[ichem][k]
								* (float)(1000.0 / 86400.0);
						}
						else	//else cwchopt = 1
						{
							//Chemical forcing functions are specifed as concentration (g/m3)
							//
							//Compute load from flow and concentration...
							//
							//if the point source flow is greater >= zero (a source)
							if(dqchin[i][j][0] >= 0.0)
							{
								//set the inflowing chemical concentration
								cin[0] = cwchinterp[ichem][k];
							}
							else	//else dqchin < 0 (a sink)
							{
								//set the inflowing chemical concentration
								cin[0] = cchemch[ichem][i][j][0];

							}	//end if dqchin >= 0.0

							//compute the load value (g/s)  (could be a source or a sink)
							load = dqchin[i][j][0] * cin[0]
								* (float)(1000.0 / 86400.0);

						}	//end if cwchopt = 0

						//if the load is positive (a source)
						if(load >= 0.0)
						{
							//compute the inflowing chemical flux (g/s)
							advchemchinflux[ichem][i][j][0] = load;

							//set the outflowing chemical flux to zero
							advchemchoutflux[ichem][i][j][0] = 0.0;
						}
						else	//else the load is negative (a sink)
						{
							//set the inflowing chemical flux to zero
							advchemchinflux[ichem][i][j][0] = 0.0;

							//compute the outflowing chemical flux (g/s)
							advchemchoutflux[ichem][i][j][0] = load;

						}	//end if load >= 0.0

					}	//end if cwchlink = i and cwchnode = j

				}	//end loop over point sources

				//Compute Advective Fluxes...
				//
				//Note:  Advection transport all phases of chemical
				//       (dissolved, bound, and particulate).
				//
				//Sources 1-8: channel flows
				//
				//loop over sources
				for(k=1; k<=8; k++)
				{
					//Note:  The scaled advective flows computed for
					//       sediment transport are used to compute
					//       corresponding chemical transport fluxes.
					//       
					//compute the inflowing chemical flux (g/s)
					advchemchinflux[ichem][i][j][k] = advinflowch[i][j][k]
						* cin[k];

					//compute the outflowing chemical flux (g/s)
					advchemchoutflux[ichem][i][j][k] = advoutflowch[i][j][k]
						* cchemch[ichem][i][j][0];

				}	//end loop over sources

				//Source 10: domain boundary
				//
				//if the link is an outlet link and it is the last node of the link
				if(qchoutlet[i] > 0 && j == nnodes[i])
				{
					//compute the inflowing chemical flux (g/s)
					advchemchinflux[ichem][i][j][10] = advinflowch[i][j][10]
						* cin[10];

					//compute the outflowing chemical flux (g/s)
					advchemchoutflux[ichem][i][j][10] = advoutflowch[i][j][10]
						* cchemch[ichem][i][j][0];

				}	//end if qchoutlet[i] > 0 && j == nnodes[i]

				//Compute the chemical advective outflow potential...
				//
				//Note:  Remember that floodplain transfers are not an
				//       outflow because the flow is simply transferred
				//       from the overland portion of the cell to the
				//       channel portion (subgrid scale transfer).
				//
				//initialize the outflow potential
				potential = 0.0;

				//loop over sources (include zero for negative loads/point sources)
				for(k=0; k<=8; k++)
				{
					//compute the solids outflow potential (g)
					potential = potential
						+ advchemchoutflux[ichem][i][j][k] * dt[idt];

				}	//end loop over sources 0-8

				//if the link is an outlet link and it is the last node of the link
				if(qchoutlet[i] > 0 && j == nnodes[i])
				{
					//increment potential (g) for domain outflow (source 10)
					potential = potential
						+ advchemchoutflux[ichem][i][j][10] * dt[idt];

				}	//end if qchoutlet[i] > 0 && j == nnodes[i]

				//compute the mass available for advection (g)
				available = cchemch[ichem][i][j][0] * watervol
					- (depchemchoutflux[ichem][i][j][0]
					+ biochemchoutflux[ichem][i][j][0]
					+ hydchemchoutflux[ichem][i][j][0]
					+ oxichemchoutflux[ichem][i][j][0]
					+ phtchemchoutflux[ichem][i][j][0]
					+ radchemchoutflux[ichem][i][j][0]
					+ vltchemchoutflux[ichem][i][j][0]
					+ udrchemchoutflux[ichem][i][j][0])
					* dt[idt];

				//if the available mass < 0.0
				if(available < 0.0)
				{
					//set the available mass (g) to zero
					available = 0.0;

				}	//end if available < 0.0

				//Note:  At this point we need to check whether there
				//       is enough chemical of this type available in
				//       the water column to satify the full advection
				//       potential.  If not, the advection potential
				//       for this chemical type is limited to available
				//       mass...
				//
				//if the overall outflux potential exceeds the available mass
				if(potential > available)
				{
					//loop over sources 0-8
					for(k=0; k<=8; k++)
					{
						//scale the chemical advection outflux (g/s)
						advchemchoutflux[ichem][i][j][k] = (float)(available / potential
							* advchemchoutflux[ichem][i][j][k]);

					}	//end loop over sources

					//scale the domain boundary (source 10)...
					//
					//if the link is an outlet link and it is the last node of the link
					if(qchoutlet[i] > 0 && j == nnodes[i])
					{
						//scale the chemical advection outflux (g/s)
						advchemchoutflux[ichem][i][j][10] = (float)(available / potential
								* advchemchoutflux[ichem][i][j][10]);

					}	//end if qchoutlet[i] > 0 && j == nnodes[i]

				}	//end if potential > available

			}	//end loop over number of chemical types

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChemicalTransport
}
