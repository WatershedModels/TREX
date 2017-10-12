/*---------------------------------------------------------------------
C-  Function:	OverlandSolidsDispersion.c
C-
C-	Purpose/	Compute the dispersive flux of solids in the overland
C-	Methods:	plane.
C-
C-
C-  Inputs:	    csedov[][][][], csedch[][][][], hov[][], hch[][],
C-              sfov[][], cell properties...
C-
C-  Outputs:    dspsedovinflux[][][][][], dspsedovinmass[][][][][],
C-              dspsedovoutflux[][][][][], dspsedovoutmass[][][][][]
C-
C-  Controls:   dbcopt
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

void OverlandSolidsDispersion()
{
/*
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		adjlink,	//link number of adjacent node
		adjnode,	//node number of adjacent node
		adjrow,		//row number of adjacent (floodplain) cell
		adjcol,		//column number of adjacent (floodplain) cell
		row,		//row number of present node
		col,		//column number of present node
		downlink;	//link number of downstream link

	float
		hchan,		//depth of water in present node (m)
		bwch,		//bottom width of present node (m)
		hbch,		//bank height of present node (m)
		sslope,		//side slope of channel bank of present node (dimensionless)
		twch,		//top width of channel at top of bank of present node (m)
		aovcross,	//cross sectional area of present node (m2)
		aovadj,		//cross section area of adjacent node (m2)
//mlv		wp,			//wetted perimeter of channel at present flow depth (m)
//mlv		rh,			//hydraulic radius of the channel at the present water depth (m)
		sf,			//friction slope (dimensionless)
		g,			//gravitational acceleration (9.81) (m/s2)
		ustar;		//friction velocity (m/s)

	float
		cadj[11],		//sediment concentration of adjacent cell/node (g/m3)
		mixlength[11],	//mixing length between nodes (m)
		mixarea[11],	//mixing (average cross sectional) area between nodes (m2)
		elong,			//longitudinal dispersion coefficient (m2/s)
		cgrad;			//concentration gradient between nodes/floodplain (g/m3)

	//Define local constants...
	//
	//set gravitational acceleration = 9.81 m/s2
	g = (float)(9.81);

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Initialize overland dispersion arrays for use this time step...
				//
				//Loop over directions/sources (except zero for point sources)
				for(k=1; k<=10; k++)
				{
					//Initialize gross dispersion flow array
					dspflowov[i][j][k] = 0.0;

				}	//end loop over directions/sources

				//set the present water depth of the overland cell (m)
				hcell = hov[i][j];

				//loop over number of solids types
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Note:  Overland dispersion is limited to the
					//       four cardinal directions: N, E, S, and
					//       W, corresponding to positions 1, 3, 5,
					//       and 7 in the direction vector.  The
					//       loop over directions below is limited
					//       to these four directions...
					//
					//loop over cardinal directions
					for(k=1; k<=7; k=k+2)
					{
						//Compute location of the adjacent cell in this direction
						//
						//if k = 1 (North)
						if(k == 1)
						{
							//row of adjacent cell (row - 1 = north)
							ii = i - 1;

							//column of adjacent cell (same column)
							jj = j;
						}
						//else if k = 3 (East)
						else if(k == 3)
						{
							//row of adjacent cell (same row)
							ii = i;

							//column of adjacent cell (col + 1 = east)
							jj = j + 1;
						}
						//else if k = 5 (South)
						else if(k == 5)
						{
							//row of adjacent cell (row + 1 = south)
							ii = i + 1;

							//column of adjacent cell (same column)
							jj = j;
						}
						//else if k = 7 (West)
						else if(k == 7)
						{
							//row of adjacent cell (same row)
							ii = i;

							//column of adjacent cell (col - 1 = west)
							jj = j - 1;
						}
						//This condition should never occur...
						//
						//else flow is neither E-W or N-S
						else
						{
							//warn user of error
							printf("\n\nWhoa!  Dispersion is on a diagonal??");
							printf("\n  Time = %10.5f",simtime);
							printf("\n     i = %d",i);
							printf("\n     j = %d",j);
							printf("\n    ii = %d",ii);
							printf("\n    jj = %d\n\n",jj);

						}	//end if k = 1 (or 3 or 5 or 7)

						//If the adjacent cell is in the domain
						if(ii <= nrows && jj <=ncols && imask[ii][jj] != nodatavalue)
						{
							//set the adjacent cell solids concentration
							cadj[k] = csedov[isolid][ii][jj][0];

							//set the mixing length between cells (m)
							mixlength[k] = w;

							//Compute mixing area...
							//
							//Note: The mixing area is the cross sectional
							//      channel area between the nodes...
							//
we need to know if either cell is a channel cell and whether the flow path and
mixing length and area for overland dispersion are altered because of the channel...

							//set the cross sectional area of the adjacent node (m2)
							aovadj = hov[ii][jj] 
								* hch[adjlink][adjnode]) * hch[adjlink][adjnode];

							if the adjacent node water depth <= the bank height
							if(hch[adjlink][adjnode] <= hbank[adjlink][adjnode])




						}
						else	//else the adjacent cell is not in the domain
						{
							//set the inflowing sediment concentration to zero
							cin[k] = 0.0;

						}	//end if the adjacent cell is in the domain

					}	//end loop over directions




							//set friction slope (dimensionless)
							sf = (float)(fabs(sfov[i][j][k]));

							//Compute friction velocity...
							//
							//  ustar = (tau / rho)^0.5 = (g * rh * sf)^0.5
							//
							//  Note:  For overland plane, rh = h...
							//
							//set the friction velocity (m/s)
							ustar = (float)(sqrt(g * hcell * sf));






						//Set the upstream sediment concentration
						//
						//Note: The following code assumes that domain
						//      boundaries cannot be upstream branches
						//      If a boundary existed, the logic below
						//      would need to check if nupbranches[i][k]
						//      was equal to zero and then to set the
						//      upstream concentration to the boundary
						//      condition because csedch[][][0] is not
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
							cadj[k] = csedch[isolid][adjlink][adjnode][0];

							//set the mixing length between nodes (m)
							mixlength[k] = (float)(0.5 * (chanlength[adjlink][adjnode]
								* chanlength[i][j]));

							//Compute mixing area...
							//
							//Note: The mixing area is the cross sectional
							//      channel area between the nodes...
							//
							//if the adjacent node water depth <= the bank height
							if(hch[adjlink][adjnode] <= hbank[adjlink][adjnode])
							{
								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[adjlink][adjnode] + sideslope[adjlink][adjnode]
									* hch[adjlink][adjnode]) * hch[adjlink][adjnode];
							}
							else	//else water depth > bank height
							{
								//channel top width at top of bank
								twch = twidth[adjlink][adjnode];

								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[adjlink][adjnode] + sideslope[adjlink][adjnode]
									* hbank[adjlink][adjnode]) * hbank[adjlink][adjnode]
									+ twch * (hch[adjlink][adjnode] - hbank[adjlink][adjnode]);

							}	//end if hch <= hbank

							//set the (average) mixing area (m2)
							mixarea[k] = (float)(0.5 * (achadj + achcross));
						}
						//else if a downstream node exists in this direction
						else if(k == downdirection[i][j][0])
						{
							//set the adjacent node sediment concentration (g/m3)
							cadj[k] = csedch[isolid][i][j+1][0];

							//set the mixing length between node (m)
							mixlength[k] = (float)(0.5 * (chanlength[adjlink][adjnode]
								* chanlength[i][j]));

							//Compute mixing area...
							//
							//Note: The mixing area is the cross sectional
							//      channel area between the nodes...
							//
							//if the adjacent node water depth <= the bank height
							if(hch[adjlink][adjnode] <= hbank[adjlink][adjnode])
							{
								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[adjlink][adjnode] + sideslope[adjlink][adjnode]
									* hch[adjlink][adjnode]) * hch[adjlink][adjnode];
							}
							else	//else water depth > bank height
							{
								//channel top width at top of bank
								twch = twidth[adjlink][adjnode];

								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[adjlink][adjnode] + sideslope[adjlink][adjnode]
									* hbank[adjlink][adjnode]) * hbank[adjlink][adjnode]
									+ twch * (hch[adjlink][adjnode] - hbank[adjlink][adjnode]);

							}	//end if hch <= hbank

							//set the (average) mixing area (m2)
							mixarea[k] = (float)(0.5 * (achadj + achcross));
						}
						else	//else no branch exists in this direction
						{
							//set the sediment concentration to zero
							cadj[k] = 0.0;

							//set the mixing length between nodes to unity (to prevent divide by zero error)
							mixlength[k] = 1.0;

							//set the cross sectional area of the node to zero
							mixarea[k] = 0.0;

						}	//end if upbranches[i][k]

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
							cadj[k] = csedch[isolid][adjlink][adjnode][0];

							//set the mixing length between node (m)
							mixlength[k] = (float)(0.5 * (chanlength[adjlink][adjnode]
								* chanlength[i][j]));

							//Compute mixing area...
							//
							//Note: The mixing area is the cross sectional
							//      channel area between the nodes...
							//
							//if the adjacent node water depth <= the bank height
							if(hch[adjlink][adjnode] <= hbank[adjlink][adjnode])
							{
								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[adjlink][adjnode] + sideslope[adjlink][adjnode]
									* hch[adjlink][adjnode]) * hch[adjlink][adjnode];
							}
							else	//else water depth > bank height
							{
								//channel top width at top of bank
								twch = twidth[adjlink][adjnode];

								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[adjlink][adjnode] + sideslope[adjlink][adjnode]
									* hbank[adjlink][adjnode]) * hbank[adjlink][adjnode]
									+ twch * (hch[adjlink][adjnode] - hbank[adjlink][adjnode]);

							}	//end if hch <= hbank

							//set the (average) mixing area (m2)
							mixarea[k] = (float)(0.5 * (achadj + achcross));
						}
						//else if an upstream node exists in this direction
						else if(k == updirection[i][j][0])
						{
							//set the inflowing sediment concentration (g/m3)
							cadj[k] = csedch[isolid][i][j-1][0];

							//Compute mixing area...
							//
							//Note: The mixing area is the cross sectional
							//      channel area between the nodes...
							//
							//if the adjacent node water depth <= the bank height
							if(hch[i][j-1] <= hbank[i][j-1])
							{
								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[i][j-1] + sideslope[i][j-1]
									* hch[i][j-1]) * hch[i][j-1];
							}
							else	//else water depth > bank height
							{
								//channel top width at top of bank
								twch = twidth[i][j-1];

								//set the cross sectional area of the adjacent node (m2)
								achadj = (bwidth[i][j-1] + sideslope[i][j-1]
									* hbank[i][j-1]) * hbank[i][j-1]
									+ twch * (hch[i][j-1] - hbank[i][j-1]);

							}	//end if hch <= hbank

							//set the (average) mixing area (m2)
							mixarea[k] = (float)(0.5 * (achadj + achcross));
						}
						else	//else no branch exists in this direction
						{
							//set the upstream sediment concentration to zero
							cadj[k] = 0.0;

							//set the mixing length between nodes to unity (to prevent divide by zero error)
							mixlength[k] = 1.0;

							//set the cross sectional area of the node to zero
							mixarea[k] = 0.0;

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
					cadj[9] = csedov[isolid][adjrow][adjcol][0];

					//Compute transverse mixing length...
					//
					//Note: The mixing length is the transverse length of
					//      the channel and overland portions of the cell.
					//      from the cell center, this length is the half
					//      width of the channel (at the bank height) and
					//      the half width of the overland portion of the
					//      cell.  The sum of the half lengths is equal
					//      to half the cell width (length): 0.5 * w.
					//
					//set the transverse mixing length (m)
					mixlength[9] = (float)(0.5 * w);

					//Compute transverse mixing area...
					//
					//Note: The mixing area is the transverse cross sectional
					//      area between the channel and overland portions of
					//      the floodplain cell...
					//
					//Also: A slight disconnect exists here.  Grid cells are
					//      defined by the grid length w while channels are
					//      defined from cell center to cell center.  When
					//      computing dispersion, this half cell offset is
					//      neglected.
					//
					//set the transverse cross sectional area of the channel (m2)
					achcross = chanlength[i][j] * (hch[i][j] - hbank[i][j]);

					//set the transverse cross sectional area of the overland portion (m2)
					achadj = chanlength[i][j] * hov[adjrow][adjcol];

					//set the (average) transverse mixing area (m2)
					mixarea[9] = (float)(0.5 * (achadj + achcross));

					//Recall: Dispersion occurs along both edges of the
					//        channel so the dispersion is double...
					//
					//double the transverse mixing area
					mixarea[9] = (float)(2.0 * mixarea[9]);
				}
				else	//else hch <= hbank (no flooding)
				{
					//set the floodplain sediment concentration to zero
					cadj[k] = 0.0;

					//set the mixing length to unity (to prevent divide by zero error)
					mixlength[9] = 1.0;

					//set the cross sectional area to zero
					mixarea[9] = 0.0;

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
					if(dbcopt[i] == 0)
					{
						//the boundary concentration equals the concentration at the outlet (zero gradient)
						cadj[10] = csedch[isolid][i][j][0];

						//set the mixing length between nodes to unity (to prevent divide by zero error)
						mixlength[10] = 1.0;

						//set the cross sectional area of the node to zero
						mixarea[10] = 0.0;
					}
					else	//else dbcopt[] = 1 (boundary concentration specified)
					{
						//set the boundary concentration to specified value
						cadj[10] = sbcinterp[isolid][qchoutlet[i]];

						//Compute mixing length...
						//
						//set the mixing length for the boundary
						mixlength[10] = chanlength[i][j];

						//Compute mixing area...
						//
						//set the mixing area (m2)
						mixarea[10] = achcross;

					}	//end if dbcopt[] = 0
				}
				else	//else the downstream link is not a boundary
				{
					//set the boundary concentration to zero
					cadj[10] = 0.0;

					//set the mixing length to unity (to prevent divide by zero error)
					mixlength[10] = 1.0;

					//set the cross sectional area to zero
					mixarea[10] = 0.0;

				}	//end if downlink == 0

				//Compute Dispersion Coefficients and Fluxes...
				//
				//Flux for Sources 1-9: channel (longitudinal) dispersion (1-8),
				//                      floodplain (transverse) dispersion (9)
				//
				//loop over channel sources
				for(k=1; k<=9; k++)
				{
					//if the source is the channel
					if(k <= 8)
					{
						//Compute longitudinal dispersion coefficient...
						//
						//	elong = 250(h)(u*) (Julien, 1998; p 178)
						//
						//longitudinal dispersion coefficient (m2/s)
						elong = (float)(250.0 * hchan * ustar);

						//compute the bulk dispersion coefficient (m3/s)
						dspflowov[i][j][k] = (elong * mixarea[k] / mixlength[k]);
					}
					else	//else the source is the floodplain
					{
						//Compute transverse dispersion coefficient...
						//
						//	etrans = 0.6hu* (julien, 1998; p 178)
						//
						//transverse dispersion coefficient (m2/s)
						etrans = (float)(0.6 * hchan * ustar);

						//compute the bulk dispersion coefficient (m3/s)
						dspflowov[i][j][9] = (etrans * mixarea[9] / mixlength[9]);

					}	//end if k <= 8

					//multiply dispersion flow by the process scale factor
					dspflowov[i][j][k] = dspflowov[i][j][k] * dspovscale;

					//compute the gradient between nodes
					cgrad = csedov[i][j][isolid][0] - cadj[k];

					//if the gradient is positive (flux leaving the present node)
					if(cgrad > 0.0)
					{
						//compute the outgoing dispersion flux (g/s)
						dspsedovoutflux[isolid][i][j][k] = dspflowov[i][j][k]
							* cgrad;

						//set the inflowing dispersion flux (g/s) to zero
						dspsedovinflux[isolid][i][j][k] = 0.0;
					}
					//if the gradient is negative (flux entering the present node)
					else if(cgrad < 0.0)
					{
						//compute the incoming dispersion flux (g/s)
						dspsedovinflux[isolid][i][j][k] = dspflowov[i][j][k]
							* cgrad;

						//set the outflowing dispersion flux (g/s) to zero
						dspsedovoutflux[isolid][i][j][k] = 0.0;
					}
					else	//else the gradient is zero (cgrad = 0)
					{
						//set the inflowing dispersion flux to zero
						dspsedovinflux[isolid][i][j][k] = 0.0;

						//set the outflowing dispersion flux to zero
						dspsedovoutflux[isolid][i][j][k] = 0.0;

					}	//end if cgrad > 0 (or < 0)

				}	//end loop over sources

			}	//end loop over number of solids types

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//free memory for local arrays
	free(cadj);
	free(mixlength);
	free(mixarea);

//End of function: Return to SolidsTransport
*/
}

/*
					//if the overall outflux potential exceeds the available mass
					if(potential > available)
					{
						//loop over sources (include zero for negative loads/point sources)
						for(k=0; k<=8; k++)
						{
//mlv							//scale the dispersion outflow (m3/s) to the limit of available mass
//mlv							dspflowov[i][j][k] = (float)(available / potential
//mlv								* dspflowov[i][j][k]);

							//scale the dispersion outflux (g/s)
							dspsedovoutflux[isolid][i][j][k] = (float)(available / potential
								* dspsedovoutflux[isolid][i][j][k]);

						}	//end loop over sources

					}	//end if potential > available

*/
