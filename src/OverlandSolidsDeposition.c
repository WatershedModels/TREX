/*---------------------------------------------------------------------
C-  Function:	OverlandSolidsDeposition.c
C-
C-	Purpose/	Compute the deposition flux of solids in the overland
C-	Methods:	plane.
C-
C-
C-  Inputs:	    ws[], tdcov[], cell properties
C-
C-  Outputs:    depflowov[][][]
C-
C-  Controls:   imask[][], depovopt, cncopt[]
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

void OverlandSolidsDeposition()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		ilayer,		//index for surface layer in the soil stack
		chanlink,	//channel link number
		channode;	//channel node number

	float
		hcell,		//depth of water in cell (m)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf,	//surface area of channel portion of cell (m2)
		aovsurf,	//surface area of overland portion of cell (m2)
		watervol;	//water column volume of node (at time t) (m3)

	float
		sf,			//friction slope (dimensionless)
		gammaw,		//unit weight of water (N/m3 = kg/m2/s2)
		densityw,	//density of water (1000) (kg/m3)
		g,			//gravitational acceleration (9.81) (m/s2)
		tau;		//average boundary shear stress (N/m2)

	float
		sigma,		//normal distribution standard deviation
		pi,			//pi = 3.14159...
		yy,			//upper limit of integration for the normal probability distribution
		fy,			//parameter for approximating the normal probability distribution
		xy,			//parameter for approximating the normal probability distribution
		pdep;		//probability of deposition (probability of staying)

	double
		potential,		//solids mass deposition potential (g)
		available;		//solids mass available for deposition (g)

	//Define local constants...
	//
	//set pi = 3.14159... (arccos(-1))
	pi = (float)acos(-1.0);

	//set density of water = 1000 kg/m3
	densityw = (float)(1000.0);

	//set gravitational acceleration = 9.81 m/s2
	g = (float)(9.81);

	//set unit weight of water = 9810 N/m3
	gammaw = densityw * g;

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Compute Deposition Fluxes...
				//
				//Flux to ground surface
				//
				//set the present water depth of the overland cell (m)
				hcell = hov[i][j];

				//Compute cell surface area...
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
					twch = twidth[chanlink][channode];			//channel top width at bank height (m)
					lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;
				}
				else	//cell is not a channel cell (overland only)
				{
					//no channel present, surface area is zero
					achsurf = 0.0;

				}	//end if imask > 1

				//compute surface area of cell
				aovsurf = w * w - achsurf;

				//Compute friction slope...
				//
				//Note:  The average shear stress for the cell
				//       is computed as the vector magnitude of
				//       the four possible contributing shear
				//       stress direction components.  Only two
				//       of the four components will be non-zero
				//       at any time: N-W or N-E or S-W or S-E.
				//
				//set friction slope (dimensionless)
				sf = (float)sqrt((pow(sfov[i][j][1], 2.0)	//North component
					+ pow(sfov[i][j][3], 2.0)				//East component
					+ pow(sfov[i][j][5], 2.0)				//South component
					+ pow(sfov[i][j][7], 2.0)));			//West component

				//Compute shear stress...
				//
				// tau = gammaw * h * sf = cdrag * rho * velocity^2
				//
				//Shear stress (tau) (N/m2)
				tau = (float)(gammaw * hcell * sf);

				//compute present water column volume (m3) (at time t)
				watervol = hov[i][j] * aovsurf;

				//set the surface layer number
				ilayer = nstackov[i][j];

				//loop over number of solids types
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Initialize gross deposition flow array for use this time step
					depflowov[isolid][i][j] = 0.0;

					//if the deposition option > 1
					if(depovopt > 1)
					{
						//Compute probability of deposition...
						//
						//if cncopt = 0 (non-cohesive, pdep by Gessler)
						if(cncopt[isolid] == 0)
						{
							//if the shear stress is greater than zero
							if(tau > 0.0)
							{
								//Note: For this function, the critical
								//      shear stress for deposition is
								//      defined as the shear stress above
								//      which half of the particles (50%)
								//      of this size settle...
								//
								//Gessler Formulation (Non-Cohesive):
								//
								//Normal distribution standard deviation
								sigma = (float)(0.57);

								//compute upper limit of integration (yy) for normal distribution 
								yy = (float)(1.0/sigma * (tcdov[isolid]/tau - 1.0));

								//if yy >= 0.0 (limit of integration >= 0)
								if(yy >= 0.0)
								{
									//Note:  This condition occurs when tau <= tau critical
									//       for deposition and we are in the upper half of
									//       the distribution (pdep >= 0.5)...
									//
									//Compute parameter fy to approximate normal
									//distribution probability integral
									fy = (float)((1.0/(pow((2.0*pi), 0.5)))
										* exp(-0.5*(pow(yy, 2.0))));

									//Compute parameter xy to approximate normal
									//distribution probability integral
									xy = (float)(pow((1.0 + 0.3327*yy), -1.0));

									//Approximate probability (Abramowitz and Stegun, 1972)
									//(probability of staying)
									pdep = (float)(1.0 - fy
										* (0.4362 * xy - 0.1202 * (pow(xy, 2.0))
										+ 0.9373 * (pow(xy, 3.0))));
								}
								else	//else yy < 0.0
								{
									//Note:  This condition occurs when tau > tau critical
									//       for deposition and we are in the lower half of
									//       the distribution (pdep < 0.5)...
									//
									//use the absolute value of the upper limit of integration
									yy = (float)(fabs(yy));

									//Compute parameter fy to approximate normal
									//distribution probability integral
									fy = (float)((1.0/(pow((2.0*pi), 0.5)))
										* exp(-0.5*(pow(yy, 2.0))));

									//Compute parameter xy to approximate normal
									//distribution probability integral
									xy = (float)(pow((1.0 + 0.3327*yy), -1.0));

									//Approximate probability (Abramowitz and Stegun, 1972)
									//(probability of not staying)
									pdep = (float)(1.0 - fy
										* (0.4362 * xy - 0.1202 * (pow(xy, 2.0))
										+ 0.9373 * (pow(xy, 3.0))));

									//Since tau > tau critical (yy < 0), the probability
									//of staying is (1 - the probability of not staying)...
									//
									//probability of deposition (probability of staying)
									pdep = (float)(1.0 - pdep);

								}	//end if yy >= 0.0
							}
							else	//else tau = 0
							{
								//set the probability of deposition to unity
								pdep = (float)(1.0);

							}	//end if tau > 0.0
						}
						//else if cncopt = 1 (cohesive, pdep by Partheniades)
						else if(cncopt[isolid] == 1)
						{
							//Note: For this function, the critical
							//      shear stress for deposition is
							//      defined as the shear stress above
							//      which all of the particles (100%)
							//      of this size settle...
							//
							//Partheniades Formulation (Cohesive):
							//
							//Normal distribution standard deviation (Assume: 0.49)
							sigma = (float)(0.49);

							//if the shear stress <= the critical shear stress for deposition
							if(tau <= tcdov[isolid])
							{
								//set the probability of deposition to unity
								pdep = 1.0;
							}
							else	//else tau > tau critical for deposition
							{
								//Calculate yy (upper limit of integration)
								yy = (float)((1.0/sigma)
									* log(0.25*((tau/tcdov[isolid]) - 1.0)
									* exp(1.27*tcdov[isolid])));

								//if yy < 0.0 (limit of integration < 0)
								if(yy < 0.0)
								{
									//Note:  This condition occurs when tau >= tau critical
									//       for deposition and we are in the lower half of
									//       the distribution (pdep > 0.5)...
									//
									//use the absolute value of the upper limit of integration
									yy = (float)(fabs(yy));

									//Compute parameter fy to approximate normal
									//distribution probability integral
									fy = (float)((1.0/(pow((2.0*pi), 0.5)))
										* exp(-0.5*(pow(yy, 2.0))));

									//Compute parameter xy to approximate normal
									//distribution probability integral
									xy = (float)(pow((1.0 + 0.3327*yy), -1.0));

									//Approximate probability (Abramowitz and Stegun, 1972)
									//(probability of staying)
									pdep = (float)(1.0 - fy
										* (0.4362 * xy - 0.1202 * (pow(xy, 2.0))
										+ 0.9373 * (pow(xy, 3.0))));
								}
								else	//else yy >= 0
								{
									//Note:  This condition occurs when tau >= tau critical
									//       for deposition and we are in the upper half of
									//       the distribution (pdep < 0.5)...
									//
									//Compute parameter fy to approximate normal
									//distribution probability integral
									fy = (float)((1.0/(pow((2.0*pi), 0.5)))
										* exp(-0.5*(pow(yy, 2.0))));

									//Compute parameter xy to approximate normal
									//distribution probability integral
									xy = (float)(pow((1.0 + 0.3327*yy), -1.0));

									//Approximate probability (Abramowitz and Stegun, 1972)
									//(probability of not staying)
									pdep = (float)(1.0 - fy
										* (0.4362 * xy - 0.1202 * (pow(xy, 2.0))
										+ 0.9373 * (pow(xy, 3.0))));

									//Since tau > tau critical (yy >= 0), the probability
									//of staying is (1 - the probability of not staying)...
									//
									//probability of deposition
									//(probability of staying)
									pdep = (float)(1.0 - pdep);

								}	//end if yy < 0

							}	//end if tau <= tcdov
						}
						else	//else cncopt undefined (cncopt not 0 or 1)...
						{
							//set the probability of deposition to unity
							pdep = 1.0;

						}	//end if cncopt[isolid] = 0
					}
					else	//else depovopt <= 1
					{
						//set the probability of deposition to unity
						pdep = 1.0;

					}	//endif depovopt > 1

					//compute the effective deposition flow (m3/s)
					depflowov[isolid][i][j] = ws[isolid] * pdep * aovsurf;

					//multiply deposition flow by the process scale factor
					depflowov[isolid][i][j] = depflowov[isolid][i][j] * depovscale;

					//compute the deposition potential (g)
					potential = depflowov[isolid][i][j]
						* csedov[isolid][i][j][0] * dt[idt];

					//Developer's Note:  If solids are subject to kinetic
					//                   reactions, the transformation flux
					//                   would need to be subtracted from
					//                   the available mass...
					//
					//compute the mass available for deposition (g)
					available = csedov[isolid][i][j][0] * watervol;

					//Note:  At this point we need to check whether there
					//       is enough sediment of this type available in
					//       the water column to satify the full deposition
					//       potential.  If not, the deposition potential
					//       for this solids type is limited to available
					//       mass...
					//
					//if the overall outflux potential exceeds the available mass
					if(potential > available)
					{
						//scale the deposition flow (m3/s) to the limit of available mass
						depflowov[isolid][i][j] = (float)(available / potential
							* depflowov[isolid][i][j]);

					}	//end if potential > available

					//compute the deposition flux leaving water column (g/s)
					depsedovoutflux[isolid][i][j][0] = depflowov[isolid][i][j]
						* csedov[isolid][i][j][0];

					//set the deposition flux entering the sediment bed (g/s)
					depsedovinflux[isolid][i][j][ilayer] = depsedovoutflux[isolid][i][j][0];

				}	//end loop over number of solids types

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to SolidsTransport
}
