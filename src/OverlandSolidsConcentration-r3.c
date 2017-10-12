/*---------------------------------------------------------------------
C-  Function:	OverlandSolidsConcentration.c
C-
C-	Purpose/	Computes solids concentration in the water column and
C-	Methods:	soil bed of the overland plane for use during the next
C-              time step t + dt.
C-
C-
C-  Inputs:	    csedov[][][][],
C-              advsedovinflux[][][][], advsedovoutflux[][][][],
C-              dspsedovinflux[][][][], dspsedovoutflux[][][][],
C-              depsedovinflux[][][][], depsedovoutflux[][][][],
C-              erssedovinflux[][][][], erssedovoutflux[][][][],
C-              channel properties, cell properties
C-
C-  Outputs:    csedovnew[][][][], sedreportov[][], popovflag,
C-              pushovflag
C-
C-  Controls:   imask[][]
C-
C-  Calls:      SimulationError
C-
C-  Called by:  SolidsBalance
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
C-	Revisions:	Reorganized code to store concentrations at reporting
C-              stations.  Moved loop over stations to end of module.
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

void OverlandSolidsConcentration()
{
	//local variable declarations/definitions
	int
		isolid,			//loop index for solids number (type)
		ilayer,			//index for surface layer in the sediment stack
		isoil,			//soil type of cell
		chanlink,		//channel link number
		channode;		//channel node number

	float
		hcell,			//new depth of water in cell (at time t + dt) (m)
		lch,			//channel length (m) (includes sinuosity)
		twch,			//top width of channel at top of bank (m)
		achsurf,		//surface area of channel (m2)
		aovsurf,		//surface area of overland cell (m2)
		watervol,		//water column volume of cell (at time t) (m3)
		newwatervol;	//new water column volume of cell (at time t + dt) (m3)

	float
		advectionin,	//gross advection influx term (in) (g/s)
		advectionout,	//gross advection outflux term (out) (g/s)
		dispersionin,	//gross dispersion influx term (in) (g/s)
		dispersionout,	//gross dispersion outflux term (out) (g/s)
		erosion,		//gross erosion term (in) (g/s)
		deposition;		//gross deposition term (out) (g/s)

	double
		influx,			//sum of all influx terms for derivative (g/s)
		outflux,		//sum of all outflux terms for derivative (g/s)
		derivative,		//mass derivative (g/s)
		available,		//solids mass available for transport (g)
		potential,		//solids mass transport potential (g)
		newmass,		//new solids mass in cell (at time t + dt) (g)
		soilinflux,		//sum of all influx terms for soil bed derivative (g/s)
		soiloutflux,	//sum of all outflux terms for soil bed derivative (g/s)
		soilderivative,	//mass derivative for soil bed (g/s)
		soilmass,		//surface soil layer solids mass of node (at time t) (g)
		newsoilmass,	//new surface soil layer solids mass of node (at time t + dt) (g)
		soilvol,		//surface soil layer volume of cell (at time t) (m3)
		newsoilvol,		//new surface soil layer volume of cell (at time t + dt) (m3)
		depositedmass,	//deposited mass of solids entering the surface layer (kg)
		erodedmass,		//eroded mass of solids leaving the surface layer (kg)
		deltavol,		//change in surface layer volume due to erosion/deposition (m3)
		densityw,		//density of water (1000) (kg/m3)
		bulkdensity;	//bulk density of a solids type in the sediment maxtrix (kg/m3)

	float
		sedflow,		//solids discharge (kg/s)
		sedflowtotal;	//total (sum of all) solids discharge (kg/s)

	//Define local constants...
	//
	//set density of water = 1000 kg/m3
	densityw = (float)(1000.0);

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//Assign new water depth (m) (at time t + dt)
				hcell = hovnew[i][j];

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

				//compute new water column volume (m3) (at time t + dt)
				newwatervol = hcell * aovsurf;

				//set the surface soil layer number
				ilayer = nstackov[i][j];

				//set the soil type of this cell
				isoil = soiltype[i][j][ilayer];

				//initialize the new volume of surface layer (m3)
				vlayerovnew[i][j][ilayer] = vlayerov[i][j][ilayer];

				//Compute derivative for the water column
				//
				//loop over number of solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Build derivative term by term...
					//
					//  derivative = advectionin[isolid][i][j][source=0-10]
					//             - advectionout[isolid][i][j][source=0-10]
					//             + dispersionin[isolid][i][j][source=0-10]
					//             - dispersionout[isolid][i][j][source=0-10]
					//             + erosion[isolid][i][j][layer=0 or ilayer]
					//             - deposition[isolid][i][j][layer=0 or ilayer]
					//             + W[i][j] {=Wwater * Csed} (included in advection term)
					//             + kineticsin[isolid][i][j] {typically zero for solids}
					//             - kineticsout[isolid][i][j] {typically zero for solids}
					//
					//Note: The derivative has units of mass/time (g/s)
					//
					//Initialize derivative terms (g/s)
					advectionin = 0.0;
					advectionout = 0.0;
					dispersionin = 0.0;
					dispersionout = 0.0;
					erosion = 0.0;
					deposition = 0.0;

					//Compute derivative for water column...
					//
					//Source 0: point sources/sinks (no dispersion term)
					//
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advsedovinflux[isolid][i][j][0];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advsedovoutflux[isolid][i][j][0];

					//loop over sources (1,3,5,7)
					for(k=1; k<=7; k=k+2)
					{
						//compute advection influx term (g/s)
						advectionin = advectionin
							+ advsedovinflux[isolid][i][j][k];

						//compute advection outflux term (g/s)
						advectionout = advectionout
							+ advsedovoutflux[isolid][i][j][k];

						//compute dispersion influx term (g/s)
						dispersionin = dispersionin
							+ dspsedovinflux[isolid][i][j][k];

						//compute dispersion outflux term (g/s)
						dispersionout = dispersionout
							+ dspsedovoutflux[isolid][i][j][k];

					}	//end loop over sources

					//Source 9: floodplain
					//
					//Note:  Values for source 9 will be zero
					//       unless the cell is a channel cell...
					//
					//if imask > 1 (if cell is a channel cell)
					if(imask[i][j] > 1)
					{
						//compute advection influx term (g/s)
						advectionin = advectionin
							+ advsedovinflux[isolid][i][j][9];

						//compute advection outflux term (g/s)
						advectionout = advectionout
							+ advsedovoutflux[isolid][i][j][9];

						//compute dispersion influx term (g/s)
						dispersionin = dispersionin
							+ dspsedovinflux[isolid][i][j][9];

						//compute dispersion outflux term (g/s)
						dispersionout = dispersionout
							+ dspsedovoutflux[isolid][i][j][9];

					}	//end if imask[][] > 1

					//Source 10: domain boundary (outlet)
					//
					//Note:  Values for source 10 will be zero
					//       unless the cell is an outlet...
					//
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advsedovinflux[isolid][i][j][10];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advsedovoutflux[isolid][i][j][10];

					//compute dispersion influx term (g/s)
					dispersionin = dispersionin
						+ dspsedovinflux[isolid][i][j][10];

					//compute dispersion outflux term (g/s)
					dispersionout = dispersionout
						+ dspsedovoutflux[isolid][i][j][10];

					//Remaining sources...
					//
					//compute erosion term (g/s)
					erosion = erssedovinflux[isolid][i][j][0];

					//compute deposition term (g/s)
					deposition = depsedovoutflux[isolid][i][j][0];

					//assign the influx (g/s)
					influx = advectionin + dispersionin + erosion;

					//assign the outflux (g/s)
					outflux = advectionout + dispersionout + deposition;

					//Note:  While the transport potential from the sum
					//       of individual process fluxes should never
					//       exceed the mass available for transport,
					//       roundoff error may still exist.  So, the
					//       total transport potential must be checked
					//       against the total mass available to ensure
					//       stability.
					//
					//compute the overall transport/transfer outflux potential (g)
					potential = outflux * dt[idt];

					//compute the mass available for transport/transformation (g)
					available = csedov[isolid][i][j][0] * watervol;

					//if the overall outflux potential exceeds the available mass
					if(potential > available)
					{
						//Scale the outflux potential
						outflux = (available / potential) * outflux;

					}	//end if potential > available

					//Compute cumulative masses for all processes
					//
					//Note:  Cumulative masses for all water column
					//       processes are computed below for all sinks
					//       and sources.  Ideally, mass calculations
					//       would be performed in each of the process
					//       routines.  However, because sink terms may
					//       be scaled, previously computed masses would
					//       need to be recomputed from scaled fluxes.
					//       Consequently, it is more efficient to
					//       perform these mass caclulations only after
					//       all scaling is completed.  Mass values for
					//       for sources are also computed here for
					//       consistency.
					//
					//compute cumulative sediment mass leaving water column by deposition (kg)
					depsedovoutmass[isolid][i][j][0] = depsedovoutmass[isolid][i][j][0]
						+ (float)(depsedovoutflux[isolid][i][j][0] * dt[idt] / 1000.0);

					//compute the cumulative sediment mass entering the sediment bed by deposition (kg)
					depsedovinmass[isolid][i][j][ilayer] = depsedovinmass[isolid][i][j][ilayer]
						+ (float)(depsedovinflux[isolid][i][j][ilayer] * dt[idt] / 1000.0);

					//compute cumulative solids mass leaving the bed (kg)
					erssedovoutmass[isolid][i][j][ilayer] = erssedovoutmass[isolid][i][j][ilayer]
						+ (float)(erssedovoutflux[isolid][i][j][ilayer] * dt[idt] / 1000.0);

					//compute the cumulative solids mass entering the water column (kg)
					erssedovinmass[isolid][i][j][0] = erssedovinmass[isolid][i][j][0]
						+ (float)(erssedovinflux[isolid][i][j][0] * dt[idt] / 1000.0);

					//Compute masses for loads (source = 0)
					//
					//compute the cumulative inflowing solids mass (kg)
					advsedovinmass[isolid][i][j][0] = advsedovinmass[isolid][i][j][0]
						+ (float)(advsedovinflux[isolid][i][j][0] * dt[idt] / 1000.0);

					//compute the cumulative outflowing solids mass (kg)
					advsedovoutmass[isolid][i][j][0] = advsedovoutmass[isolid][i][j][0]
						+ (float)(advsedovoutflux[isolid][i][j][0] * dt[idt] / 1000.0);

					//Compute the cumulative (net) solids mass entering the domain from a point source (kg)
					totalswov[isolid] = totalswov[isolid]
						+ (float)((advsedovinflux[isolid][i][j][0]
						- advsedovoutflux[isolid][i][j][0]) * dt[idt] / 1000.0);

					//loop over overland sources (1,3,5,7)
					for(k=1; k<=7; k=k+2)
					{
						//compute the cumulative inflowing solids mass (kg)
						advsedovinmass[isolid][i][j][k] = advsedovinmass[isolid][i][j][k]
							+ (float)(advsedovinflux[isolid][i][j][k] * dt[idt] / 1000.0);

						//compute the cumulative outflowing solids mass (kg)
						advsedovoutmass[isolid][i][j][k] = advsedovoutmass[isolid][i][j][k]
							+ (float)(advsedovoutflux[isolid][i][j][k] * dt[idt] / 1000.0);

						//compute the cumulative in-dispersing solids mass (kg)
						dspsedovinmass[isolid][i][j][k] = dspsedovinmass[isolid][i][j][k]
							+ (float)(dspsedovinflux[isolid][i][j][k] * dt[idt] / 1000.0);

						//compute the cumulative out-dispersing solids mass (kg)
						dspsedovoutmass[isolid][i][j][k] = dspsedovoutmass[isolid][i][j][k]
							+ (float)(dspsedovoutflux[isolid][i][j][k] * dt[idt] / 1000.0);

					}	//end loop over sources

					//Compute masses for floodplain (source = 9)
					//
					//Note:  These values will be zero unless the cell is
					//       a channel cell...
					//
					//if imask > 1 (if cell is a channel cell)
					if(imask[i][j] > 1)
					{
						//compute the cumulative inflowing solids mass (kg)
						advsedovinmass[isolid][i][j][9] = advsedovinmass[isolid][i][j][9]
							+ (float)(advsedovinflux[isolid][i][j][9] * dt[idt] / 1000.0);

						//compute the cumulative outflowing solids mass (kg)
						advsedovoutmass[isolid][i][j][9] = advsedovoutmass[isolid][i][j][9]
							+ (float)(advsedovoutflux[isolid][i][j][9] * dt[idt] / 1000.0);

						//compute the cumulative in-dispersing solids mass (kg)
						dspsedovinmass[isolid][i][j][9] = dspsedovinmass[isolid][i][j][9]
							+ (float)(dspsedovinflux[isolid][i][j][9] * dt[idt] / 1000.0);

						//compute the cumulative out-dispersing solids mass (kg)
						dspsedovoutmass[isolid][i][j][9] = dspsedovoutmass[isolid][i][j][9]
							+ (float)(dspsedovoutflux[isolid][i][j][9] * dt[idt] / 1000.0);

					}	//end if imask[][] > 1

					//Compute masses for boundaries (source = 10)
					//
					//Note:  These values will be zero unless the cell is
					//       an outlet cell...
					//
					//compute the cumulative inflowing solids mass (kg)
					advsedovinmass[isolid][i][j][10] = advsedovinmass[isolid][i][j][10]
						+ (float)(advsedovinflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute the cumulative outflowing solids mass (kg)
					advsedovoutmass[isolid][i][j][10] = advsedovoutmass[isolid][i][j][10]
						+ (float)(advsedovoutflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute the cumulative in-dispersing solids mass (kg)
					dspsedovinmass[isolid][i][j][10] = dspsedovinmass[isolid][i][j][10]
						+ (float)(dspsedovinflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute the cumulative out-dispersing solids mass (kg)
					dspsedovoutmass[isolid][i][j][10] = dspsedovoutmass[isolid][i][j][10]
						+ (float)(dspsedovoutflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute total derivative for the water column (g/s)
					derivative = influx - outflux;

					//Perform numerical integration for water column...
					//
					//Note:  For better error trapping, numerical integration
					//       is performed in three steps.  First, the new mass
					//       is computed.  Second, the new concentration is
					//       computed from the new mass and the new volume.
					//       Third, the new mass is checked for instability.
					//
					//compute the new mass (g)
					newmass = available + derivative * dt[idt];

					//Compute the new cell solids concentration...
					//
					//if the new cell water volume > zero
					if(newwatervol > 0.0)
					{
						//compute new water column solids concentration (g/m3)
						csedovnew[isolid][i][j][0] = (float)(newmass / newwatervol);
					}
					else	//else newwatervol <= 0.0
					{
						//set the new concentration to zero
						csedovnew[isolid][i][j][0] = 0.0;

					}	//end if newwatervol > 0.0

					//Check for instability...
					//
					//If the new mass is negative
					if(newmass < 0.0)
					{
						//Check for round-off error...
						//
						//if the magnitude is < the error tolerance
						if((float)fabs(newmass) < TOLERANCE)
						{
							//The error is round-off...
							//
							//Reset the concentration to exactly zero
							csedovnew[isolid][i][j][0] = 0.0;
						}	
						else	//else the error is (probably) numerical instability
						{
							//Warn user of possible numerical instability (do not abort)
							//
							//Report error type (negative concentration overland) and location
							SimulationError(4, i, j, isolid);

							//Reset the concentration to exactly zero anyway...
							csedovnew[isolid][i][j][0] = 0.0;

						}	//end if fabs(newmass) < TOLERANCE

					}	//end if newmass < 0.0

				}	//end loop over solids

				//Compute new (ending) volume of surface soil layer...
				//
				//assign local value for surface soil layer volume (m3)
				soilvol = vlayerov[i][j][ilayer];

				//initialize new soil layer volume (m3)
				newsoilvol = 0.0;

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//starting mass for this solids type (g)
					soilmass = csedov[isolid][i][j][ilayer] * soilvol;

					//Use scaled fluxes to compute the volume change in
					//the surface layer due to erosion and deposition
					//
					//deposited mass (gain) for this solids type (g)
					depositedmass = depsedovinflux[isolid][i][j][ilayer]
						* dt[idt];

					//eroded mass (loss) for this solids type (g)
					erodedmass = erssedovoutflux[isolid][i][j][ilayer]
						* dt[idt];

					//new (ending) mass for this solids type (g)
					newsoilmass = soilmass + depositedmass - erodedmass;

					//Note:  Because the bulk density is computed from a
					//       combination of single and double precision
					//       values, it is typecast as float to truncate
					//       roundoff and then re-typecast as double to
					//       return the value to double precision.
					//
					//compute the bulk density of this solids type (kg/m3)
					bulkdensity = (double)((float)(spgravity[isolid] * densityw
							* (1.0 - porosityov[isoil])));

					//volume fraction of this solids type in surface layer (m3)
					deltavol = newsoilmass / (bulkdensity * 1000.0);

					//compute new surface layer volume (m3)
					newsoilvol = newsoilvol + deltavol;

				}	//end loop over solids

				//Check for roundoff error...
				//
				//Note:  Watch out for roundoff errors.  As volume
				//       in a layer goes to zero (when erodedmass
				//       nearly equals soilmass + depositedmass),
				//       newsoilvol can be a very small positive
				//       (or negative) number when it should be
				//       exactly zero.  These residual newsoilvol
				//       values, combined with errors in layer
				//       masses cause erroneous concentration
				//       values...
				//
				//if the new surface layer volume < TOLERANCE
				if(newsoilvol < TOLERANCE)
				{
					//reset the new volume (m3) to exactly zero
					newsoilvol = 0.0;

				}	//end if newsoilvol < TOLERANCE

				//Compute derivative for the surface soil layer...
				//
				//loop over number of solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//if the new surface soil layer volume >= TOLERANCE
					if(newsoilvol >= TOLERANCE)
					{
						//compute available mass (g)
						soilmass = csedov[isolid][i][j][ilayer] * soilvol;

						//compute erosion term (g/s)
						erosion = erssedovoutflux[isolid][i][j][ilayer];

						//compute deposition term (g/s)
						deposition = depsedovinflux[isolid][i][j][ilayer];

						//assign influx (g/s)
						soilinflux = deposition;

						//assign the outflux (g/s)
						soiloutflux = erosion;

						//compute total derivative for the surface soil layer (g/s)
						soilderivative = soilinflux - soiloutflux;

						//compute the new mass (g) of solids in the soil layer
						newsoilmass = soilmass + soilderivative * dt[idt];

						//Check for roundoff error...
						//
						//Note:  Watch out for roundoff errors.  As the mass
						//       in a layer goes to zero (when derivative*dt
						//       nearly equals soilmass), newsoilmass can be a
						//       very small positive or negative number when
						//       it should be exactly zero.  These residual
						//       newsoilmass values, combined with errors in
						//       layer volumes cause erroneous concentration
						//       values...
						//
						//if newsoilmass > TOLERANCE
						if(newsoilmass > TOLERANCE)
						{
							//compute new surface soil layer concentration (g/m3)
							csedovnew[isolid][i][j][ilayer] = (float)(newsoilmass / newsoilvol);
						}
						else	//else newsoilmass <= TOLERANCE
						{
							//set the new concentration (g/m3) to zero
							csedovnew[isolid][i][j][ilayer] = 0.0;

						}	//end if newsoilmass > TOLERANCE
					}
					else	//else newsoilvol < TOLERANCE
					{
						//set the new concentration to zero
						csedovnew[isolid][i][j][ilayer] = 0.0;

					}	//end if newsoilvol >= TOLERANCE

				}	//end loop over solids

				//Assign global value of new surface soil layer volume (m3)
				vlayerovnew[i][j][ilayer] = newsoilvol;

				//if the surface soil layer volume has reached
				//  the maximum volume limit
				if(newsoilvol >= maxvlayerov[i][j][ilayer])
				{
					//raise the push flag for the overland plane
					pushovflag = 1;

				}	//end if newsoilvol>= maxvlayerov[][][]

				//if the surface soil layer volume has reached the minimum
				//  volume limit and there are more layers in the stack
				if(newsoilvol <= minvlayerov[i][j][ilayer] && ilayer > 1)
				{
					//raise the pop flag for the overland plane
					popovflag = 1;

				}	//end if newsoilvol >= maxvlayerov[][][]

				//Developer's Note:  If mass transport processes
				//                   occur in subsurface layers,
				//                   derivative terms for these
				//                   layers would be computed and
				//                   concentrations updated...

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//Compute cumulative mass transport for outlets (domain boundaries)...
	//
	//Developer's Note:  Because these calculation are only needed for
	//                   outlet cells, cumulative mass calculations for
	//                   outlets occur below.
	//
	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//initialize outlet total (sum of all) solids discharge (kg/s)
		sedflowtotal = 0.0;

		//loop over number of solids types
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Advection...
			//
			//Compute the cumulative solids mass exiting the domain at this outlet (kg)
			totaladvsedoutov[isolid][i] = totaladvsedoutov[isolid][i]
				+ (float)(advsedovoutflux[isolid][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0);

			//Compute the cumulative solids mass entering the domain at this outlet (kg)
			totaladvsedinov[isolid][i] = totaladvsedinov[isolid][i]
				+ (float)(advsedovinflux[isolid][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0);

			//Dispersion...
			//
			//Compute the cumulative solids mass exiting the domain at this outlet (kg)
			totaldspsedoutov[isolid][i] = totaldspsedoutov[isolid][i]
				+ (float)(dspsedovoutflux[isolid][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0);

			//Compute the cumulative solids mass entering the domain at this outlet (kg)
			totaldspsedinov[isolid][i] = totaldspsedinov[isolid][i]
				+ (float)(dspsedovinflux[isolid][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0);

			//compute sediment discharge (kg/s) (advection plus dispersion)
			sedflow = (float)((advsedovoutflux[isolid][iout[i]][jout[i]][10]
				+ dspsedovoutflux[isolid][iout[i]][jout[i]][10]) / 1000.0);

			//Search for peak sediment discharge at this outlet...
			//
			//if the sediment discharge is greater than the peak value
			if(sedflow > sedflowpeakov[isolid][i])
			{
				//Save peak dischage at outlet (kg/s)
				sedflowpeakov[isolid][i] = sedflow;

				//Save time of peak flow (hours)
				sedtimepeakov[isolid][i] = (float)(simtime);

			}	//end if sedflow > sedflowpeakov[][]

			//compute total solids discharge (kg/s)
			sedflowtotal = sedflowtotal + sedflow;

		}	//end loop over solids

		//Search for peak sediment discharge at this outlet...
		//
		//if the sediment discharge is greater than the peak value
		if(sedflowtotal > sedflowpeakov[0][i])
		{
			//Save total peak dischage at outlet (kg/s)
			sedflowpeakov[0][i] = sedflowtotal;

			//Save time of total peak flow (hours)
			sedtimepeakov[0][i] = (float)(simtime);

		}	//end if sedflowtotal > sedflowpeakov[0][]

	}	//end loop over outlets

	//Loop over number of solids reporting stations
	for (k=1; k<=nsedreports; k++)
	{
		//set row and column references for the solids reporting station
		i = sedreprow[k];
		j = sedrepcol[k];

		//loop over number of solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//store the concentration for reporting (g/m3)
			sedreportov[isolid][k] = csedov[isolid][i][j][0];

		}	//end loop over solids

	}	//end loop over reporting stations

//End of function: Return to SolidsBalance
}
