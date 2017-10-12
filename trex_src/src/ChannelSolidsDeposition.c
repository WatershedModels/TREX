/*---------------------------------------------------------------------
C-  Function:	ChannelSolidsDeposition.c
C-
C-	Purpose/	Compute the deposition flux of solids in the channel
C-	Methods:	network.
C-
C-
C-  Inputs:	    ws[], tdcch[], channel properties
C-
C-  Outputs:    depflowch[][][]
C-
C-  Controls:   imask[][], depchopt, cncopt[]
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

void ChannelSolidsDeposition()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		ilayer;		//index for surface layer in the sediment stack

	float
		hchan,		//depth of water in channel (m)
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		twch,		//top width of channel at top of bank (m)
		achcross,	//cross sectional area of channel at present water depth (m2)
		wp,			//wetted perimeter of channel at present flow depth (m)
		rh,			//hydraulic radius of the channel at the present water depth (m)
		sf,			//friction slope (dimensionless)
		gammaw,		//unit weight of water (N/m3 = kg/m2/s2)
		densityw,	//density of water (1000) (kg/m3)
		g,			//gravitational acceleration (9.81) (m/s2)
		tau,		//average boundary shear stress (N/m2)
		watervol;	//water column volume of node (at time t) (m3)

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

	//loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//Compute Deposition Fluxes...
			//
			//Flux to channel bed
			//
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

				//Wetted perimeter for trapezoidal, triangular, or rectangular channel
				wp = (float)(bwch + 2.0 * hchan * sqrt(1.0 + sslope * sslope));
			}
			else	//Flow depth greater than channel depth
			{
				//Cross-sectional area for trapezoidal, triangular, or rectangular channel
				//plus rectangular area above bank height
				achcross = (bwch + sslope * hbch) * hbch
					+ (hchan - hbch) * twch;

				//Wetted perimeter for trapezoidal, triangular, or rectangular channel
				//plus rectangular perimeter above bank height
				wp = (float)(bwch + 2.0 * hbch * sqrt(1.0 + sslope * sslope)
					+ 2.0 * (hchan - hbch));

			}	//end if hchan <= bank height

			//Compute hydraulic radius (m)
			rh = achcross / wp;

			//set friction slope (dimensionless)
			sf = (float)(fabs(sfch[i][j]));

			//Compute shear stress...
			//
			// tau = gammaw * rh * sf = cdrag * rho * velocity^2
			//
			//Shear stress (tau) (N/m2)
			tau = (float)(gammaw * rh * sf);

			//compute present water column volume (m3) (at time t)
			watervol = achcross * chanlength[i][j];

			//set the surface layer number
			ilayer = nstackch[i][j];

			//loop over number of solids types
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Initialize gross deposition flow array for use this time step...
				depflowch[isolid][i][j] = 0.0;

				//if the deposition option > 1
				if(depchopt > 1)
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
							yy = (float)(1.0/sigma * (tcdch[isolid]/tau - 1.0));

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
						if(tau <= tcdch[isolid])
						{
							//set the probability of deposition to unity
							pdep = 1.0;
						}
						else	//else tau > tau critical for deposition
						{
							//Calculate yy (upper limit of integration)
							yy = (float)((1.0/sigma)
								* log(0.25*((tau/tcdch[isolid]) - 1.0)
								* exp(1.27*tcdch[isolid])));

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

						}	//end if tau <= tcdch
					}
					else	//else cncopt undefined (cncopt not 0 or 1)...
					{
						//set the probability of deposition to unity
						pdep = 1.0;

					}	//end if cncopt[isolid] = 0
				}
				else	//else depchopt <= 1
				{
					//set the probability of deposition to unity
					pdep = 1.0;

				}	//endif depchopt > 1

				//compute the effective deposition flow (m3/s)
				depflowch[isolid][i][j] = ws[isolid] * pdep * achbed[i][j][ilayer];

				//multiply deposition flow by the process scale factor
				depflowch[isolid][i][j] = depflowch[isolid][i][j] * depchscale;

				//compute the deposition potential (g)
				potential = depflowch[isolid][i][j]
					* csedch[isolid][i][j][0] * dt[idt];

				//Developer's Note:  If solids are subject to kinetic
				//                   reactions, the transformation flux
				//                   would need to be subtracted from
				//                   the available mass...
				//
				//compute the mass available for deposition (g)
				available = csedch[isolid][i][j][0] * watervol;

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
					depflowch[isolid][i][j] = (float)(available / potential
						* depflowch[isolid][i][j]);

				}	//end if potential > available

				//compute the deposition flux leaving water column (g/s)
				depsedchoutflux[isolid][i][j][0] = depflowch[isolid][i][j]
					* csedch[isolid][i][j][0];

				//set the deposition flux entering the sediment bed (g/s)
				depsedchinflux[isolid][i][j][ilayer] = depsedchoutflux[isolid][i][j][0];

			}	//end loop over number of solids types

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to SolidsTransport
}
