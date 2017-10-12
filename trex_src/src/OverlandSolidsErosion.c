/*---------------------------------------------------------------------
C-  Function:	OverlandSolidsErosion.c
C-
C-	Purpose/	Compute the erosion flux of solids in the overland
C-	Methods:	plane.
C-                        
C-
C-  Inputs:	    sfov[][][], tceov[], ayov[][], mexpov[][],
C-              hov[][], cell properties...
C-
C-  Outputs:    ersflowov[][][], taumaxov[][], taumaxtimeov[][]
C-
C-  Controls:   ersovopt, cncopt[]
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

void OverlandSolidsErosion()
{
	//local variable declarations/definitions
	int
		isolid,		//loop index for solids (particle) type
		ilayer,		//index for surface layer in the soil stack
		isoil,		//soil type of cell
		iland,		//land use classification of cell
		chanlink,	//channel link number
		channode;	//channel node number

	float
		hcell,		//depth of water in cell (m)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf,	//surface area of channel portion of cell (m2)
		aovsurf,	//surface area of overland portion of cell (m2)
		watervol;	//water column volume of cell (at time t) (m3)

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

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Assign local water depth (m)
				hcell = hov[i][j];

				//set the surface layer number
				ilayer = nstackov[i][j];

				//Assign soil type and land use classification indices...
				//
				//soil type of this cell
				isoil = soiltype[i][j][ilayer];

				//land use classification of this cell
				iland = landuse[i][j];

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

				//compute present water column volume (m3) (at time t)
				watervol = hcell * aovsurf;

				//if the overland erosion option > 1
				if(ersovopt > 1)
				{
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
					// tau = gammaw * rh * sf = cdrag * rho * velocity^2
					//
					//Shear stress (tau) (N/m2)
					tau = (float)(gammaw * hcell * sf);

				}	//end if ersovopt > 1

				//loop over number of solids types
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Initialize gross deposition flow array for use this time step...
					ersflowov[isolid][i][j] = 0.0;

					//if the erosion option > 1
					if(ersovopt > 1)
					{
						//Compute erosion flux from excess shear...
						//
						//if cncopt = 0 (non-cohesive)
						if(cncopt[isolid] == 0)
						{
							//if the shear stress exceeds the critical shear stress for erosion
							if(tau > tceov[isolid])
							{
								//compute the erosion amount (g/m2)
								epsilon = (float)(ayov[iland] / zageov[isolid]
									* pow((tau/tceov[isolid] - 1.0), mexpov[isoil]));
							}
							else	//else tau <= tceov
							{
								//set the erosion amount to zero
								epsilon = 0.0;

							}	//end if tau > tceov
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
							if(tau > tceov[isolid] && tau > taumaxov[i][j])
							{
								//compute the erosion amount (g/m2)
								epsilon = (float)(ayov[iland] / zageov[isolid]
									* pow((tau/tceov[isolid] - 1.0), mexpov[isoil])
									- ayov[iland] / zageov[isolid]
									* pow((taumaxov[i][j]/tceov[isolid] - 1.0), mexpov[isoil]));

								//store the maximum shear stress (N/m2)
								newtaumax = tau;

								//store the time when the maximum shear stress occured
								newtautime = (float)(simtime);
							}
							else	//else tau <= tceov or tau <= taumaxov
							{
								//set the erosion amount to zero
								epsilon = 0.0;

							}	//end if tau > tceov
						}
						else	//else cncopt undefined (cncopt not 0 or 1)...
						{
							//set the erosion amount to zero
							epsilon = 0.0;

						}	//end if cncopt[isolid] = 0
					}
					else	//else ersovopt <= 1
					{
						//Compute the transport rate (g/s)
						//
						//Note:  The transport rate is defined as the advective
						//       outflux minus the deposition flux.
						//
						//initialize the outflux (g/s)
						outflux = 0.0;

						//Compute the transport rate (g/s)
						//
						//Note:  The transport rate is defined as the mass
						//       in the water column minus the mass leaving
						//       the cell by advection and deposition during
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
						available = csedov[isolid][i][j][0] * watervol;

						//initialize the outflux (g/s)
						outflux = 0.0;

						//Note:  When looping over sources, Source 0 (point sources/sinks)
						//       is included to account for negative loads.  At this point
						//       in the calculation, Source 9 (floodplain transfer) should
						//       be zero and Source 10 (outlet transport) should always be
						//       zero except for outlets.  For simplicity, we just loop over
						//       Sources 0-10 in a single loop but this could be placed in
						//       a loop over Sources 0-8 and 10 if the cell is an outlet.
						//
						//loop over sources
						for(k=0; k<=10; k++)
						{
							//compute the gross outflux (g/s)
							outflux = outflux
								+ advsedovoutflux[isolid][i][j][k];

						}	//end loop over sources

						//compute the transport rate (g/s)
						transrateov[isolid][i][j] = (float)(available / dt[idt])
							- outflux - depsedovoutflux[isolid][i][j][0];

						//Note:  While the sum of individual process fluxes
						//       should never exceed the mass available for
						//       transport, roundoff error may still exist.
						//       than zero.  The check below limits the
						//       transport rate to positive values.
						//
						//if the transport rate is negative
						if(transrateov[isolid][i][j] < 0.0)
						{
							//set the transport rate to zero
							transrateov[isolid][i][j] = 0.0;

						}	//end if transrateov[][][] < 0.0

						//Compute erosion flux from difference between the transport
						//capacity and the transport rate...
						//
						//if the transport capacity > the transport rate
						if(transcapov[isolid][i][j] > transrateov[isolid][i][j])
						{
							//compute the erosion amount (g/m2)
							epsilon = (transcapov[isolid][i][j]
								- transrateov[isolid][i][j])
								* dt[idt] / aovsurf;
						}
						else	//else transport capacity <= transport rate
						{
							//set the erosion amount to zero
							epsilon = 0.0;

						}	//end if transport capacity > transport rate

					}	//endif ersovopt > 1

					//compute the bulk density of this solids type (kg/m3)
					bulkdensity = (float)(spgravity[isolid] * densityw
						* (1.0 - porosityov[isoil]));

					//compute the erosion flow (m3/s)
					ersflowov[isolid][i][j] = (float)(epsilon * aovsurf
						/ (bulkdensity * 1000.0) / dt[idt]);

					//multiply erosion flow by the process scale factor
					ersflowov[isolid][i][j] = ersflowov[isolid][i][j] * ersovscale;

					//compute the erosion mass potential (g)
					potential = ersflowov[isolid][i][j]
						* csedov[isolid][i][j][ilayer] * dt[idt];

					//Developer's Note:  If solids are subject to kinetic
					//                   reactions, the transformation flux
					//                   would need to be subtracted from
					//                   the available mass...
					//
					//compute the mass available in the surface sediment (g)
					available = vlayerov[i][j][ilayer] * csedov[isolid][i][j][ilayer];

					//Note:  At this point we need to check whether there
					//       is enough sediment of this type available in
					//       the bed to satify the full erosion potential.
					//       If not, the erosion potential for this solids
					//       type is limited to available mass...
					//
					//if the erosion potential > available sediment mass
					if(potential > available)
					{
						//limit the erosion flow to the available solids mass
						ersflowov[isolid][i][j] = (float)(available / potential
							* ersflowov[isolid][i][j]);

					}	//end if potential > available

					//compute the erosion flux leaving the sediment bed (g/s)
					erssedovoutflux[isolid][i][j][ilayer] = ersflowov[isolid][i][j]
						* csedov[isolid][i][j][ilayer];

					//set the erosion flux entering the water column (g/s)
					erssedovinflux[isolid][i][j][0] = erssedovoutflux[isolid][i][j][ilayer];

				}	//end loop over number of solids types

				//Update shear stress histories...
				//
				//if the erosion option for overland cells > 1
				if(ersovopt > 1)
				{
					//if the new shear stress > the present shear stress
					if(newtaumax > taumaxov[i][j])
					{
						//update the maximum shear stress (N/m2)
						taumaxov[i][j] = newtaumax;

						//update the time of occurence (hours)
						taumaxtimeov[i][j] = newtautime;

					}	//end if newtaumax > taumaxov

				}	//end if ersovopt > 1

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

//End of function: Return to SolidsTransport
}
