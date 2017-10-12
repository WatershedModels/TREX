/*---------------------------------------------------------------------
C-  Function:	ChannelSolidsErosion.c
C-
C-	Purpose/	Compute the erosion flux of solids in the channel
C-	Methods:	network.
C-                        
C-
C-  Inputs:	    sfch[][], tcech[], aych[][], mexpch[][],
C-              hch[][], channel properties...
C-
C-  Outputs:    ersflowch[][][], taummaxch[][], taumaxtimech[][]
C-
C-  Controls:   erschopt, cncopt[]
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
C-	Date:		26-JUN-2008
C-
C-	Revisions:	Revised calculation of transport rate.
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

void ChannelSolidsErosion()
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
		watervol;	//water column volume of node (at time t) (m3)

	float
		sf,			//friction slope (dimensionless)
		gammaw,		//unit weight of water (N/m3 = kg/m2/s2)
		densityw,	//density of water (1000) (kg/m3)
		g,			//gravitational acceleration (9.81) (m/s2)
		tau;		//average boundary shear stress (N/m2)

	float
		epsilon,		//erosion amount (kg/m2)
		bulkdensity,	//bulk density of a solids type in the sediment maxtrix (kg/m3)
		newtaumax,		//new value of the maximum shear stress at a node (N/m2)
		newtautime,		//new time at which the maximum shear stress occurred at a node (hours)
		outflux;		//outflux of solids (g/s)

	double
		potential,		//erosion mass potential (g)
		available;		//available mass of sediment in surface layer (g)

	//Define local constants...
	//
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
			//Compute Erosion Fluxes...
			//
			//Flux from channel bed
			//
			//Assign local water depth (m)
			hchan = hch[i][j];

			//Assign channel characteristics
			bwch = bwidth[i][j];			//bottom width (m)
			hbch = hbank[i][j];				//bank height (m)
			sslope = sideslope[i][j];		//side slope (dimensionless)
			twch = twidth[i][j];			//channel top width at top of bank (m)

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
				achcross = (bwch + sslope * hchan) * hchan
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
				//Initialize gross erosion flow array for use this time step...
				ersflowch[isolid][i][j] = 0.0;

				//if the erosion option > 1
				if(erschopt > 1)
				{
					//Compute erosion flux from excess shear...
					//
					//if cncopt = 0 (non-cohesive)
					if(cncopt[isolid] == 0)
					{
						//if the shear stress exceeds the critical shear stress for erosion
						if(tau > tcech[isolid])
						{
							//compute the erosion amount (g/m2)
							epsilon = (float)(aych[i][j] / zagech[isolid]
								* pow((tau/tcech[isolid] - 1.0), mexpch[i][j]));
						}
						else	//else tau <= tcech
						{
							//set the erosion amount to zero
							epsilon = 0.0;

						}	//end if tau > tcech
					}
					//else if cncopt = 1 (cohesive)
					else if(cncopt[isolid] == 1)
					{
						//Note:  For cohesive sediments, the erosion amount
						//       is controlled by the shear stress exposure
						//       history of the sediments at this location.
						//
						//if the shear stress exceeds the critical shear
						//stress for erosion and also exceeds the maximum
						//shear stress at this location
						if(tau > tcech[isolid] && tau > taumaxch[i][j])
						{
							//compute the erosion amount (g/m2)
							epsilon = (float)(aych[i][j] / zagech[isolid]
								* pow((tau/tcech[isolid] - 1.0), mexpch[i][j])
								- aych[i][j] / zagech[isolid]
								* pow((taumaxch[i][j]/tcech[isolid] - 1.0), mexpch[i][j]));

							//store the maximum shear stress (N/m2)
							newtaumax = tau;

							//store the time when the maximum shear stress occured
							newtautime = (float)(simtime);
						}
						else	//else tau <= tcech or tau <= taumaxch
						{
							//set the erosion amount to zero
							epsilon = 0.0;

						}	//end if tau > tcech
					}
					else	//else cncopt undefined (cncopt not 0 or 1)...
					{
						//set the erosion amount to zero
						epsilon = 0.0;

					}	//end if cncopt[isolid] = 0
				}
				else	//else erschopt <= 1
				{
					//Compute the transport rate (g/s)
					//
					//Note:  The transport rate is defined as the mass
					//       in the water column minus the mass leaving
					//       the node by advection and deposition during
					//       a time step.
					//
					//       transrate = available / dt - outflux - deposition
					//
					//Developer's Note:  If solids are subject to kinetic
					//                   reactions, the transformation flux
					//                   would need to be subtracted from
					//                   the available mass...
					//
					//compute mass available in the water column (g)
					available = csedch[isolid][i][j][0] * watervol;

					//initialize the outflux (g/s)
					outflux = 0.0;

					//Note:  When looping over sources, Source 0 (point sources/sinks)
					//       is included to account for negative loads.  At this point
					//       in the calculation, Source 9 (floodplain transfer) should
					//       be zero and Source 10 (outlet transport) should always be
					//       zero except for outlets.  For simplicity, we just loop over
					//       Sources 0-10 in a single loop but this could be placed in
					//       a loop over Sources 0-8 and 10 if the node is an outlet.
					//
					//loop over sources
					for(k=0; k<=10; k++)
					{
						//compute the gross outflux (g/s)
						outflux = outflux
							+ advsedchoutflux[isolid][i][j][k];

					}	//end loop over sources

					//compute the transport rate (g/s)
					transratech[isolid][i][j] = (float)(available / dt[idt])
						- outflux - depsedchoutflux[isolid][i][j][0];

					//Note:  While the sum of individual process fluxes
					//       should never exceed the mass available for
					//       transport, roundoff error may still exist.
					//       than zero.  The check below limits the
					//       transport rate to positive values.
					//
					//if the transport rate is negative
					if(transratech[isolid][i][j] < 0.0)
					{
						//set the transport rate to zero
						transratech[isolid][i][j] = 0.0;

					}	//end if transratech[][][] < 0.0

					//Compute erosion flux from difference between the transport
					//capacity and the transport rate...
					//
					//if the transport capacity > the transport rate
					if(transcapch[isolid][i][j] > transratech[isolid][i][j])
					{
						//compute the erosion amount (g/m2)
						epsilon = (transcapch[isolid][i][j]
							- transratech[isolid][i][j])
							* dt[idt] / achbed[i][j][ilayer];
					}
					else	//else transport capacity <= transport rate
					{
						//set the erosion amount to zero
						epsilon = 0.0;

					}	//end if transport capacity > transport rate

				}	//endif erschopt > 1

				//compute the bulk density of this solids type (kg/m3)
				bulkdensity = (float)(spgravity[isolid] * densityw
					* (1.0 - porositych[i][j][ilayer]));

				//compute the erosion flow (m3/s)
				ersflowch[isolid][i][j] = (float)(epsilon * achbed[i][j][ilayer]
					/ (bulkdensity * 1000.0) / dt[idt]);

				//multiply erosion flow by the process scale factor
				ersflowch[isolid][i][j] = ersflowch[isolid][i][j] * erschscale;

				//compute the erosion mass potential (g)
				potential = ersflowch[isolid][i][j]
					* csedch[isolid][i][j][ilayer] * dt[idt];

				//Developer's Note:  If solids are subject to kinetic
				//                   reactions, the transformation flux
				//                   would need to be subtracted from
				//                   the available mass...
				//
				//compute the mass available in the surface sediment (g)
				available = vlayerch[i][j][ilayer] * csedch[isolid][i][j][ilayer];

				//Note:  At this point we need to check whether there
				//       is enough sediment of this type available in
				//       the bed to satify the full erosion potential.
				//       If not, the erosion potential for this solids
				//       type is limited to available mass...
				//
				//if the erosion potential > available sediment mass
				if(potential > available)
				{
					//limit the erosion flow (m3/s) to the available solids mass
					ersflowch[isolid][i][j] = (float)(available / potential
						* ersflowch[isolid][i][j]);

				}	//end if potential > available

				//compute the erosion flux leaving the sediment bed (g/s)
				erssedchoutflux[isolid][i][j][ilayer] = ersflowch[isolid][i][j]
					* csedch[isolid][i][j][ilayer];

				//set the erosion flux entering the water column (g/s)
				erssedchinflux[isolid][i][j][0] = erssedchoutflux[isolid][i][j][ilayer];

			}	//end loop over number of solids types

			//Update shear stress histories...
			//
			//if the erosion option for channels > 1
			if(erschopt > 1)
			{
				//if the new shear stress > the present shear stress
				if(newtaumax > taumaxch[i][j])
				{
					//update the maximum shear stress (N/m2)
					taumaxch[i][j] = newtaumax;

					//update the time of occurence (hours)
					taumaxtimech[i][j] = newtautime;

				}	//end if newtaumax > taumaxch

			}	//end if erschopt > 1

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to SolidsTransport
}
