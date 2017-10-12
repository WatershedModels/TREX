/*---------------------------------------------------------------------
C-  Function:	ChannelChemicalDispersion.c
C-
C-	Purpose/	Compute the dispersive flux of chemicals in the channel
C-	Methods:	network.
C-
C-
C-  Inputs:	    cchemch[][][][], cchemov[][][][], hch[][],
C-              channel properties...
C-
C-  Outputs:    dspchemchinflux[][][][][], dspchemchinmass[][][][][],
C-              dspchemchoutflux[][][][][], dspchemchoutmass[][][][][]
C-
C-  Controls:   dbcopt
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

void ChannelChemicalDispersion()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical tytpe
		adjlink,	//link number of adjacent node
		adjnode,	//node number of adjacent node
		adjrow,		//row number of adjacent (floodplain) cell
		adjcol,		//column number of adjacent (floodplain) cell
		row,		//row number of present node
		col,		//column number of present node
		downlink;	//link number of downstream link

	float
		cadj[11],		//sediment concentration of adjacent cell/node (g/m3)
		cgrad;			//concentration gradient between nodes/floodplain (g/m3)

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//loop over number of chemical types
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//if it is the first node of the link
				if(j==1)
				{
					//loop over directions
					for(k=1; k<=8; k++)
					{
						//Set the upstream sediment concentration
						//
						//Note: The following code assumes that domain
						//      boundaries cannot be upstream branches
						//      If a boundary existed, the logic below
						//      would need to check if nupbranches[i][k]
						//      was equal to zero and then to set the
						//      upstream concentration to the boundary
						//      condition because cchemch[][][0] is not
						//      defined for adjlink=0 and adjnode=0.
						//
						//if an upstream branch exists in this direction
						if(nupbranches[i][k] > -1)
						{
							//set the upstream link
							adjlink = nupbranches[i][k];

							//set the upstream node
							adjnode = nnodes[adjlink];

							//set the adjacent node sediment concentration (g/m3)
							cadj[k] = cchemch[ichem][adjlink][adjnode][0];

						}
						//else if a downstream node exists in this direction
						else if(k == downdirection[i][j][0])
						{
							//set the adjacent node sediment concentration (g/m3)
							cadj[k] = cchemch[ichem][i][j+1][0];
						}
						else	//else no branch exists in this direction
						{
							//set the sediment concentration to zero
							cadj[k] = 0.0;

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
							//set the adjacent node sediment concentration
							cadj[k] = cchemch[ichem][i][j-1][0];
						}
						//else a downstream node is in this direction
						else if(k == downdirection[i][j][0])
						{
							//set the adjacent node sediment concentration
							cadj[k] = cchemch[ichem][i][j+1][0];
						}
						else	//else no node is in this direction
						{
							//set the adjacent node sediment concentration to zero
							cadj[k] = 0.0;

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
							//set the downstream link
							adjlink = ndownbranches[i][k];

							//set the downstream node
							adjnode = 1;

							//set the sediment (gradient) concentration (g/m3)
							cadj[k] = cchemch[ichem][adjlink][adjnode][0];
						}
						//else if an upstream node exists in this direction
						else if(k == updirection[i][j][0])
						{
							//set the inflowing sediment concentration (g/m3)
							cadj[k] = cchemch[ichem][i][j-1][0];
						}
						else	//else no branch exists in this direction
						{
							//set the upstream sediment concentration to zero
							cadj[k] = 0.0;

						}	//end if upbranches[i][k]

					}	//end loop over directions

				}	//end if j = 1 (if/then/else if/else)

				//Conditions for Source 9 (floodplain)...
				//
				//if the water depth exceeds the bank height (flooding is occuring)
				if(hch[i][j] > hbank[i][j])
				{
					//set the floodplain cell row
					adjrow = ichnrow[i][j];

					//set the floopplain cell column
					adjcol = ichncol[i][j];

					//set the floodplain sediment concentration (g/m3)
					cadj[9] = cchemov[ichem][adjrow][adjcol][0];
				}
				else	//else hch <= hbank (no flooding)
				{
					//set the adjacent floodplain concentration to zero
					cadj[k] = 0.0;

				}	//end if hch > hbank

				//Conditions for Source 10 (domain boundary)...
				//
				//Determine row and column location of present node
				row = ichnrow[i][j];
				col = ichncol[i][j];

				//Determine the downstream link
				downlink = link[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
					[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

				//if the downstream link is an outlet/domain boundary
				if(downlink == 0)
				{
					//if the domain BC option is zero (normal depth condition)
					if(dbcopt[qchoutlet[i]] == 0)
					{
						//the boundary concentration equals the concentration at the outlet (zero gradient)
						cadj[10] = cchemch[ichem][i][j][0];
					}
					else	//else dbcopt[] = 1 (boundary concentration specified)
					{
						//set the boundary concentration to specified value
						cadj[10] = cbcinterp[ichem][qchoutlet[i]];

					}	//end if dbcopt[] = 0
				}
				else	//else the downstream link is not a boundary
				{
					//set the boundary concentration to zero
					cadj[10] = 0.0;

				}	//end if downlink == 0

				//Compute Dispersion Fluxes...
				//
				//Flux for Sources 1-9: channel (longitudinal) dispersion (1-8),
				//                      floodplain (transverse) dispersion (9)
				//
				//loop over channel sources
				for(k=1; k<=9; k++)
				{
					//compute the gradient between nodes
					cgrad = cchemch[ichem][i][j][0] - cadj[k];

					//if the gradient is positive (flux leaving the present node)
					if(cgrad > 0.0)
					{
						//compute the outgoing dispersion flux (g/s)
						dspchemchoutflux[ichem][i][j][k] = dspflowch[i][j][k]
							* cgrad;

						//set the inflowing dispersion flux (g/s) to zero
						dspchemchinflux[ichem][i][j][k] = 0.0;

						//compute the cumulative out-dispersing sediment mass (kg)
						dspchemchoutmass[ichem][i][j][k] = dspchemchoutmass[ichem][i][j][k]
							+ (float)(dspchemchoutflux[ichem][i][j][k] * dt[idt] / 1000.0);
					}
					//if the gradient is negative (flux entering the present node)
					else if(cgrad < 0.0)
					{
						//compute the incoming dispersion flux (g/s)
						dspchemchinflux[ichem][i][j][k] = dspflowch[i][j][k]
							* cgrad;

						//set the outflowing dispersion flux (g/s) to zero
						dspchemchoutflux[ichem][i][j][k] = 0.0;

						//compute the cumulative in-dispersing sediment mass (kg)
						dspchemchinmass[ichem][i][j][k] = dspchemchinmass[ichem][i][j][k]
							+ (float)(dspchemchinflux[ichem][i][j][k] * dt[idt] / 1000.0);
					}
					else	//else the gradient is zero (cgrad = 0)
					{
						//set the inflowing dispersion flux to zero
						dspchemchinflux[ichem][i][j][k] = 0.0;

						//set the outflowing dispersion flux to zero
						dspchemchoutflux[ichem][i][j][k] = 0.0;

					}	//end if cgrad > 0 (or < 0)

				}	//end loop over sources

				//Sum the solids mass entering and exiting the domain...
				//
				//Note:  The mass of solids transport from a link
				//       will always be zero unless the outlet
				//       number for the link is greater than zero.
				//
				//if the source is a boundary (source = 10)
				if(k == 10 && qchoutlet[i] > 0)
				{
					//Compute the cumulative solids mass exiting the domain at this outlet (kg)
					totaldspchemoutch[ichem][qchoutlet[i]] = totaldspchemoutch[ichem][qchoutlet[i]]
						+ (float)(dspchemchoutflux[ichem][i][j][k] * dt[idt] / 1000.0);

					//Compute the cumulative solids mass entering the domain at this outlet (kg)
					totaldspcheminch[ichem][qchoutlet[i]] = totaldspcheminch[ichem][qchoutlet[i]]
						+ (float)(dspchemchinflux[ichem][i][j][k] * dt[idt] / 1000.0);

					//Compute the cumulative solids mass exiting the domain for all outlets (kg)
					totaldspchemoutch[ichem][0] = totaldspchemoutch[ichem][0]
						+ (float)(dspchemchoutflux[ichem][i][j][k] * dt[idt] / 1000.0);

					//Compute the cumulative solids mass entering the domain for all outlets (kg)
					totaldspcheminch[ichem][0] = totaldspcheminch[ichem][0]
						+ (float)(dspchemchinflux[ichem][i][j][k] * dt[idt] / 1000.0);

				}	//end if k = 10 and qchoutlet > 0

			}	//end loop over number of solids types

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChemicalTransport
}
