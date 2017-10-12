/*---------------------------------------------------------------------
C-  Function:	OverlandSolidsAdvection.c
C-
C-	Purpose/	Compute the advective flux of solids in the overland
C-	Methods:	plane.
C-
C-
C-  Inputs:	    dqovin[][][], dqovout[][][]
C-
C-  Outputs:    advsedovinflux[][][][][], advsedovoutflux[][][][][]
C-
C-  Controls:   dbcopt, swpovopt
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
C-	Revised:	Mark Velleux
C-				HydroQual, Inc.
C-				1200 MacArthur Boulevard
C-				Mahwah, NJ 07430
C-
C-	Date:		28-April-2008
C-
C-	Revisions:	Reorganized code to assign point source loads and to
C-              store values at reporting stations.  Imbedded if/then
C-              in main row/column loops was replaced with code to
C-              loop over loads and reporting stations and make direct
C-              assignment of values.  Also added overland distributed
C-              solids loads.
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

void OverlandSolidsAdvection()
{
	//local variable declarations/definitions
	int
		icell,		//index for active cell
		isolid,		//loop index for solids (particle) type
		ii,			//row number of adjacent/inflowing cell
		jj,			//column number of adjacent/inflowing cell
		outlink,	//link number of outlet node
		outnode,	//node number of outlet node
		chanlink,	//channel link number
		channode;	//channel node number

	float
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at top of bank (m)
		achsurf,	//surface area of channel (m2)
		aovsurf,	//surface area of overland cell (m2)
		watervol;	//water column volume of cell (at time t) (m3)

	float
		cin[11],	//sediment concentration of inflowing cell/node (source 0-10)
		load,		//load (source or sink) (g/s)
		outflux;	//overland solids outflux (g/s) (sum for sources 0, 1, 3, 5, and 7)

	double
		potential,	//solids mass transport potential (g)
		available;	//solids mass available for transport (g)

	//Assignments for point source and distributed solids loads
	//
	//loop over number of solids types
	for(isolid=1; isolid<=nsolids; isolid++)
	{
		//Note:  The advective influx and outflux terms for each solids
		//       type must be initialized in order to sum point source
		//       and distributed loads to a cell because there could be
		//       more than one load to a cell (source 0).  However, the
		//       advective inflow and outflow terms for source zero do
		//       not need to be initialized because they are determined
		//       directly from dqovin[][][] and dqovout[][][] and do not
		//       need to be stored, summed, or used in subsequent
		//       calculations...
		//
		//Loop over rows
		for(i=1; i<=nrows; i++)
		{
			//Loop over columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain (is not null)
				if(imask[i][j] != nodatavalue)
				{
					//Initialize gross advection influx array for source zero
					advsedovinflux[isolid][i][j][0] = 0.0;

					//Initialize gross advection outflow array for source zero
					advsedovoutflux[isolid][i][j][0] = 0.0;

				}	//end if imask[][] != nodatavalue

			}	//end loop over columns

		}	//end loop over rows

		//Compute Advective Fluxes...
		//
		//Source 0: point sources
		//
		//loop over point sources
		for(k=1; k<=nswpov[isolid]; k++)
		{
			//set row and column references for the solids point source
			i = swpovrow[isolid][k];
			j = swpovcol[isolid][k];

			//if the overland solids loading option is zero
			if(swpovopt[isolid][k] == 0)
			{
				//Solids forcing functions are specifed as loads (kg/day)
				//
				//compute the load value (g/s) (could be a source or a sink)
				load = swpovinterp[isolid][k]
					* (float)(1000.0 / 86400.0);
			}
			else	//else swpovopt = 1
			{
				//Solids forcing functions are specifed as concentration (g/m3)
				//
				//Compute load from flow and concentration...
				//
				//if the point source flow is greater >= zero (a source)
				if(dqovin[i][j][0] >= 0.0)
				{
					//set the inflowing sediment concentration
					cin[0] = swpovinterp[isolid][k];
				}
				else	//else dqovin < 0 (a sink)
				{
					//set the outflowing sediment concentration
					cin[0] = csedov[isolid][i][j][0];

				}	//end if dqovin >= 0.0

				//compute the load value (g/s)  (could be a source or a sink)
				load = dqovin[i][j][0] * cin[0]
					* (float)(1000.0 / 86400.0);

			}	//end if swpovopt = 0

			//if the load is positive (a source)
			if(load >= 0.0)
			{
				//compute the inflowing sediment flux (g/s)
				advsedovinflux[isolid][i][j][0] = advsedovinflux[isolid][i][j][0]
					+ load;
			}
			else	//else the load is negative (a sink)
			{
				//compute the outflowing sediment flux (g/s)
				advsedovoutflux[isolid][i][j][0] = advsedovoutflux[isolid][i][j][0]
					+ load;

			}	//end if load >= 0.0

		}	//end loop over point sources

		//Source 0: distributed sources
		//
		//loop over distributed sources
		for(k=1; k<=nswdov[isolid]; k++)
		{
			//Loop over cells with a distributed load for this function
			for(icell=1; icell<=nswdovcells[isolid][k]; icell++)
			{
				//set row and column references for the solids distributed source
				i = swdovrow[isolid][k][icell];
				j = swdovcol[isolid][k][icell];

				//Distributed solids forcing functions are specifed as g/m2/mm rain
				//
				//Note:  Distributed loads are computed from a normalized load
				//       (g/m2/mm), surface area (m2) and rainfall (m/s) with a
				//       conversion factor (from m rain to mm rain) and assume
				//       that interception does occurs (rain = net rain).
				//
				//       The assumption that interception occurs is reasonable
				//       because roofs and other structures may intercept rain
				//       and have depression storage just like other surfaces.
				//       These processes are implicitly represented through the
				//       use of the net rainfall.
				//
				//compute the load value (g/s) (could be a source or a sink)
				load = swdovinterp[isolid][k] * swdovarea[isolid][k][i][j]
					* netrainrate[i][j] * (float)(1000.0);

				//if the load is positive (a source)
				if(load >= 0.0)
				{
					//compute the inflowing sediment flux (g/s)
					advsedovinflux[isolid][i][j][0] = advsedovinflux[isolid][i][j][0]
						+ load;
				}
				else	//else the load is negative (a sink)
				{
					//compute the outflowing sediment flux (g/s)
					advsedovoutflux[isolid][i][j][0] = advsedovoutflux[isolid][i][j][0]
						+ load;

				}	//end if load >= 0.0

			}	//end loop over cells for this distributed sources

		}	//end loop over distributed sources

	}	//end loop over solids

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Initialize overland solids transport arrays for use this time step...
				//
				//Loop over sources 1, 3, 5, and 7
				for(k=1; k<=7; k=k+2)
				{
					//Initialize gross advection inflow array
					advinflowov[i][j][k] = 0.0;

					//Initialize gross advection outflow array
					advoutflowov[i][j][k] = 0.0;

					//initialize inflowing concentration array
					cin[k] = 0.0;

				}	//end loop over directions/sources

				//Compute water column volume...
				//
				//if the cell is a channel cell
				if(imask[i][j] > 1)
				{
					//Get channel link and node
					chanlink = link[i][j];
					channode = node[i][j];

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)
					twch = twidth[chanlink][channode];			//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;
				}
				else	//cell is not a channel cell (overland only)
				{
					//no channel present, surface area is zero
					achsurf = 0.0;

				}	//end if imask > 1

				//compute surface area (m2)
				aovsurf = w * w - achsurf;

				//compute present water column volume (m3) (at time t)
				watervol = hov[i][j] * aovsurf;

				//Compute advective flows...
				//
				//Note:  Overland flow is limited to the four
				//       cardinal directions: N, E, S, and W,
				//       corresponding to positions 1, 3, 5,
				//       and 7 in the direction vector.  The
				//       loop over directions below is limited
				//       to these four directions...
				//
				//loop over overland sources (cardinal directions)
				for(k=1; k<=7; k=k+2)
				{
					//multiply advective inflow by the process scale factor
					advinflowov[i][j][k] = dqovin[i][j][k] * advovscale;

					//multiply advective outflow by the process scale factor
					advoutflowov[i][j][k] = dqovout[i][j][k] * advovscale;

				}	//end loop over overland sources

				//loop over number of solids types
				for(isolid=1; isolid<=nsolids; isolid++)
				{
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
							printf("\n\nWhoa!  Advection is on a diagonal??");
							printf("\n  Time = %10.5f",simtime);
							printf("\n     i = %d",i);
							printf("\n     j = %d",j);
							printf("\n    ii = %d",ii);
							printf("\n    jj = %d\n\n",jj);

						}	//end if k = 1 or 3 or 5 or 7

						//If the adjacent cell is in the domain
						if(ii >= 1 && jj >=1 && ii <= nrows && jj <=ncols &&
							imask[ii][jj] != nodatavalue)
						{
							//set the inflowing sediment concentration
							cin[k] = csedov[isolid][ii][jj][0];
						}
						else	//else the adjacent cell is not in the domain
						{
							//set the inflowing sediment concentration to zero
							cin[k] = 0.0;

						}	//end if the adjacent cell is in the domain

					}	//end loop over directions

					//Conditions for Source 9 (floodplain)...
					//
					//Note:  Concentrations for floodplain transfers
					//       are set in FloodplainSolidsTransfer
					//
					//Conditions for Source 10 (domain boundary)...
					//
					//Note:  Concentrations for domain boundaries are
					//       set in a separate loop over outlets at
					//       the end of this module.
					//
					//Compute Advective Fluxes...
					//
					//Sources 1, 3, 5, 7: overland flows
					//
					//loop over overland sources
					for(k=1; k<=7; k=k+2)
					{
						//compute the inflowing sediment flux (g/s)
						advsedovinflux[isolid][i][j][k] = advinflowov[i][j][k] * cin[k];

						//compute the outflowing sediment flux (g/s)
						advsedovoutflux[isolid][i][j][k] = advoutflowov[i][j][k]
							* csedov[isolid][i][j][0];

					}	//end loop over overland sources

					//Determine if outflow potential exceeds available mass...
					//
					//Compute the solids advective outflow potential...
					//
					//initialize the outflow potential
					potential = 0.0;

					//Note:  To compute the potential, we need to loop over
					//       sources 0, 1, 3, 5, and 7.  Fluxes at outlet
					//       cells (to include source 10) are handled in the
					//       loop over outlets below.
					//
					//compute the solids outflow potential (g) (source 0)
					potential = potential
						+ advsedovoutflux[isolid][i][j][0] * dt[idt];

					//loop over sources (sources 1, 3, 5, and 7)
					for(k=1; k<=7; k=k+2)
					{
						//compute the solids outflow potential (g)
						potential = potential
							+ advsedovoutflux[isolid][i][j][k] * dt[idt];

					}	//end loop over sources 1,3,5,7

					//compute the mass available for advection (g)
					available = csedov[isolid][i][j][0] * watervol
						- (depsedovoutflux[isolid][i][j][0] * dt[idt]);

					//if the available mass < 0.0
					if(available < 0.0)
					{
						//set the available mass (g) to zero
						available = 0.0;

					}	//end if available < 0.0

					//Note:  At this point we need to check whether there
					//       is enough solids of this type available in
					//       the water column to satify the full advection
					//       potential.  If not, the advection potential
					//       for this solids type is limited to available
					//       mass...
					//
					//if the overall outflux potential exceeds the available mass
					if(potential > available)
					{
						//scale the solids advection outflux (g/s) (source 0)
						advsedovoutflux[isolid][i][j][0] = (float)(available / potential
								* advsedovoutflux[isolid][i][j][0]);

						//loop over sources (1,3,5,7)
						for(k=1; k<=7; k=k+2)
						{
							//scale the chemical advection outflux (g/s)
							advsedovoutflux[isolid][i][j][k] = (float)(available / potential
									* advsedovoutflux[isolid][i][j][k]);

						}	//end loop over sources

					}	//end if potential > available

				}	//end loop over solids

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//Conditions for Source 10 (domain boundary)...
	//
	//Note:  These calculation only need to be performed
	//       if the cell is an outlet cell...
	//
	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//Compute water column volume...
		//
		//if the cell is a channel cell
		if(imask[iout[i]][jout[i]] > 1)
		{
			//Get channel link and node
			chanlink = link[iout[i]][jout[i]];
			channode = node[iout[i]][jout[i]];

			//Compute area of channel within cell...
			//
			//Assign channel characteristics
			lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)
			twch = twidth[chanlink][channode];			//channel top width at bank height (m)

			//Surface area of channel portion of cell (m2)
			achsurf = twch * lch;
		}
		else	//cell is not a channel cell (overland only)
		{
			//no channel present, surface area is zero
			achsurf = 0.0;

		}	//end if imask > 1

		//compute surface area (m2)
		aovsurf = w * w - achsurf;

		//compute present water column volume (m3) (at time t)
		watervol = hov[iout[i]][jout[i]] * aovsurf;

		//Scale advective flows...
		//
		//multiply advective inflow by the process scale factor
		advinflowov[iout[i]][jout[i]][10] = dqovin[iout[i]][jout[i]][10]
			* advovscale;

		//multiply advective outflow by the process scale factor
		advoutflowov[iout[i]][jout[i]][10] = dqovout[iout[i]][jout[i]][10]
			* advovscale;

		//loop over number of solids types
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//if the cell is not a channel OR the outlet option = 1
			if((imask[iout[i]][jout[i]] < 2) || (outopt == 1))
			{
				//if the domain BC option is zero (normal depth condition)
				if(dbcopt[i] == 0)
				{
					//the boundary concentration is zero
					cin[10] = 0.0;
				}
				else	//else dbcopt[] = 1 (boundary concentration specified)
				{
					//if the outlet has a channel outlet:
					//
					//if channels are simulated (chnopt > 0) and the
					//outlet cell is a channel cell (imask > 1) and the
					//node is the last node of the link (node = nnodes)
					if(chnopt > 0 && imask[iout[i]][jout[i]] > 1 &&
						node[iout[i]][jout[i]] == nnodes[link[iout[i]][jout[i]]])
					{
						//set the outlet link
						outlink = link[iout[i]][jout[i]];

						//set the outlet node (should be last node)
						outnode = node[iout[i]][jout[i]];

						//if the boundary depth > channel bank height
						if(hbcinterp[i] > hbank[outlink][outnode])
						{
							//set the boundary concentration to specified value
							cin[10] = sbcinterp[isolid][qchoutlet[i]];
						}
						else	//else hbcinterp < hbank
						{
							//overland boundary concentration is zero
							cin[10] =  0.0;

						}	//end if hchinterp > hbank

					}	//end if outlet has a channel outlet

				}	//end if dbcopt[] = 0
			}
			else	//overland flow is not routed through this outlet
			{
				//the boundary concentration is zero
				cin[10] = 0.0;

			}	//end if (imask[][] < 2) or (outopt = 1)

			//Compute Advective Fluxes...
			//
			//Source 10: domain boundary
			//
			//compute the inflowing sediment flux (g/s)
			advsedovinflux[isolid][iout[i]][jout[i]][10] =
				advinflowov[iout[i]][jout[i]][10] * cin[10];

			//compute the outflowing sediment flux (g/s)
			advsedovoutflux[isolid][iout[i]][jout[i]][10] =
				advoutflowov[iout[i]][jout[i]][10]
				* csedov[isolid][iout[i]][jout[i]][0];

			//Determine if outflow potential exceeds available mass...
			//
			//Compute the solids advective outflow potential...
			//
			//potential (g) for domain outflow (source 10)
			potential = advsedovoutflux[isolid][iout[i]][jout[i]][10] * dt[idt];

			//Compute overland outflux (Sources 0, 1, 3, 5, and 7)
			outflux = advsedovoutflux[isolid][iout[i]][jout[i]][0]
				+ advsedovoutflux[isolid][iout[i]][jout[i]][1]
				+ advsedovoutflux[isolid][iout[i]][jout[i]][3]
				+ advsedovoutflux[isolid][iout[i]][jout[i]][5]
				+ advsedovoutflux[isolid][iout[i]][jout[i]][7];

			//compute the mass available for advection (g)
			available = csedov[isolid][iout[i]][jout[i]][0] * watervol
				- (depsedovoutflux[isolid][iout[i]][jout[i]][0] + outflux)
				* dt[idt];

			//if the available mass < 0.0
			if(available < 0.0)
			{
				//set the available mass (g) to zero
				available = 0.0;

			}	//end if available < 0.0

			//Note:  At this point we need to check whether there
			//       is enough solids of this type available in
			//       the water column to satify the full advection
			//       potential.  If not, the advection potential
			//       for this solids type is limited to available
			//       mass...
			//
			//if the overall outflux potential exceeds the available mass
			if(potential > available)
			{
				//scale the domain boundary...
				//
				//scale the chemical advection outflux (g/s)
				advsedovoutflux[isolid][iout[i]][jout[i]][10] = (float)(available / potential
					* advsedovoutflux[isolid][iout[i]][jout[i]][10]);

			}	//end if potential > available

		}	//end loop over solids

	}	//end loop over outlets

//End of function: Return to SolidsTransport
}
