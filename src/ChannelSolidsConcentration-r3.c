/*---------------------------------------------------------------------
C-  Function:	ChannelSolidsConcentration.c
C-
C-	Purpose/	Computes solids concentration in the water column and
C-	Methods:	sediment bed of the channel network for use during the
C-              next time step t + dt.
C-
C-
C-  Inputs:	    csedch[][][][],
C-              advsedchinflux[][][][], advsedchoutflux[][][][],
C-              dspsedchinflux[][][][], dspsedchoutflux[][][][],
C-              depsedchinflux[][][][], depsedchoutflux[][][][],
C-              erssedchinflux[][][][], erssedchoutflux[][][][],
C-              channel properties
C-
C-  Outputs:    csedchnew[][][][], sedreportch[][], popchflag, pushchflag
C-
C-  Controls:   None
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

void ChannelSolidsConcentration()
{
	//local variable declarations/definitions
	int
		isolid,			//loop index for solids number (type)
		ilayer,			//index for surface layer in the sediment stack
		row,			//row number of node (for reporting)
		col;			//column number of node (for reporting)

	float
		hchan,			//new depth of water in channel (at time t + dt) (m)
		bwch,			//bottom width of channel (m)
		hbch,			//bank height of channel (m)
		sslope,			//side slope of channel bank (dimensionless)
		twch,			//top width of channel at top of bank (m)
		achcross,		//cross sectional area of channel (m2)
		watervol,		//water column volume of node (at time t) (m3)
		newwatervol;	//new water column volume of node (at time t + dt) (m3)

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
		newmassch,		//new solids mass in channel (at time t + dt) (g)
		sedinflux,		//sum of all influx terms for sediment bed derivative (g/s)
		sedoutflux,		//sum of all outflux terms for sediment bed derivative (g/s)
		sedderivative,	//mass derivative for sediment bed (g/s)
		sedmass,		//surface sediment layer solids mass of node (at time t) (g)
		newsedmass,		//new surface sediment layer solids mass of node (at time t + dt) (g)
		sedvol,			//surface sediment layer volume of node (at time t) (m3)
		newsedvol,		//new surface sediment layer volume of node (at time t + dt) (m3)
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

    //loop over links
	for(i=1; i<=nlinks; i++)
	{
		//loop over nodes
		for(j=1; j<=nnodes[i]; j++)
		{
			//get row of present node (for floodpain transfers and reporting)
			row = ichnrow[i][j];

			//get column of present node (for floodpain transfers and reporting)
			col = ichncol[i][j];

			//Assign channel characteristics
			bwch = bwidth[i][j];				//bottom width (m)
			hbch = hbank[i][j];					//bank height (m)
			sslope = sideslope[i][j];			//side slope (dimensionless)

			//Compute present water volume...
			//
			//if the present flow depth <= bank height
			if(hch[i][j] <= hbch)
			{
				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hch[i][j]) * hch[i][j];
			}
			else	//else flow depth greater than channel depth
			{
				//Channel top width at top of bank (m)
				twch = twidth[i][j];

				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hbch) * hbch
						+ (hch[i][j] - hbch) * twch;

			}	//end if hch <= bank height

			//compute present water column volume (m3) (at time t)
			watervol = achcross * chanlength[i][j];

			//Compute new water column volume...
			//
			//Assign new water depth (m) (at time t + dt)
			hchan = hchnew[i][j];

			//if the new flow depth <= bank height
			if(hchan <= hbch)
			{
				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hchan) * hchan;
			}
			else	//else flow depth greater than channel depth
			{
				//Channel top width at top of bank (m)
				twch = twidth[i][j];

				//Cross-sectional area (m2)
				achcross = (bwch + sslope * hbch) * hbch
						+ (hchan - hbch) * twch;

			}	//end if hchan <= bank height

			//compute new water column volume (m3) (at time t + dt)
			newwatervol = achcross * chanlength[i][j];

			//set the surface sediment layer number
			ilayer = nstackch[i][j];

			//Compute derivative for the water column
			//
			//loop over number of solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Build derivative term by term...
				//
				//  derivative = advectionin[isolid][i][j][source=0-8,10]
				//             - advectionout[isolid][i][j][source=0-8,10]
				//             + dispersionin[isolid][i][j][source=0-8,10]
				//             - dispersionout[isolid][i][j][source=0-8,10]
				//             + erosion[isolid][i][j][layer=0 or ilayer]
				//             - deposition[isolid][i][j][layer=0 or ilayer]
				//             + W[i][j] {=Wwater * Csed} (included in advection term)
				//             + kineticsin[isolid][i][j] {typically zero for solids}
				//             - kineticsout[isolid][i][j] {typically zero for solids}
				//
				//Note: The derivative has units of mass/time (g/s)
				//
				//Initialize derivative terms
				advectionin = 0.0;
				advectionout = 0.0;
				dispersionin = 0.0;
				dispersionout = 0.0;
				erosion = 0.0;
				deposition = 0.0;

				//Source 0 = point source/load
				//Sources 1-8 = channel flows by direction
				//Source 9 = floodplain transfer
				//
				//loop over directions/sources 
				for(k=0; k<=9; k++)
				{
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advsedchinflux[isolid][i][j][k];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advsedchoutflux[isolid][i][j][k];

					//compute dispersion influx term (g/s)
					dispersionin = dispersionin
						+ dspsedchinflux[isolid][i][j][k];

					//compute dispersion outflux term (g/s)
					dispersionout = dispersionout
						+ dspsedchoutflux[isolid][i][j][k];

				}	//end loop over directions

				//Source 10 (domain boundary)...
				//
				//if the link is an outlet link and it is the last node of the link
				if(qchoutlet[i] > 0 && j == nnodes[i])
				{
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advsedchinflux[isolid][i][j][10];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advsedchoutflux[isolid][i][j][10];

					//compute dispersion influx term (g/s)
					dispersionin = dispersionin
						+ dspsedchinflux[isolid][i][j][10];

					//compute dispersion outflux term (g/s)
					dispersionout = dispersionout
						+ dspsedchoutflux[isolid][i][j][10];

				}	//end if qchoutlet[i] > 0 && j == nnodes[i]

				//compute erosion term (g/s)
				erosion = erssedchinflux[isolid][i][j][0];

				//compute deposition term (g/s)
				deposition = depsedchoutflux[isolid][i][j][0];

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
				available = csedch[isolid][i][j][0] * watervol;

				//if the overall outflux potential exceeds the available mass
				if(potential > available)
				{
					//Scale the outflux potential
					outflux = (available / potential) * outflux;

				}	//end if potential > available

				//Compute cumulative masses transported for all processes...
				//
				//compute cumulative sediment mass leaving water column by deposition (kg)
				depsedchoutmass[isolid][i][j][0] = depsedchoutmass[isolid][i][j][0]
					+ (float)(depsedchoutflux[isolid][i][j][0] * dt[idt] / 1000.0);

				//compute the cumulative sediment mass entering the sediment bed by deposition (kg)
				depsedchinmass[isolid][i][j][ilayer] = depsedchinmass[isolid][i][j][ilayer]
					+ (float)(depsedchinflux[isolid][i][j][ilayer] * dt[idt] / 1000.0);

				//compute cumulative solids mass leaving the bed (kg)
				erssedchoutmass[isolid][i][j][ilayer] = erssedchoutmass[isolid][i][j][ilayer]
					+ (float)(erssedchoutflux[isolid][i][j][ilayer] * dt[idt] / 1000.0);

				//compute the cumulative solids mass entering the water column (kg)
				erssedchinmass[isolid][i][j][0] = erssedchinmass[isolid][i][j][0]
					+ (float)(erssedchinflux[isolid][i][j][0] * dt[idt] / 1000.0);

				//Compute masses for loads (source = 0)
				//
				//compute the cumulative inflowing sediment mass (kg)
				advsedchinmass[isolid][i][j][0] = advsedchinmass[isolid][i][j][0]
					+ (float)(advsedchinflux[isolid][i][j][0] * dt[idt] / 1000.0);

				//compute the cumulative outflowing sediment mass (kg)
				advsedchoutmass[isolid][i][j][0] = advsedchoutmass[isolid][i][j][0]
					+ (float)(advsedchoutflux[isolid][i][j][0] * dt[idt] / 1000.0);

				//Compute the cumulative (net) solids mass entering the domain from a point source (kg)
				totalswch[isolid] = totalswch[isolid]
					+ (float)((advsedchinflux[isolid][i][j][0]
					- advsedchoutflux[isolid][i][j][0]) * dt[idt] / 1000.0);

				//loop over sources (including floodplain, source 9)
				for(k=1; k<=9; k++)
				{
					//compute the cumulative inflowing sediment mass (kg)
					advsedchinmass[isolid][i][j][k] = advsedchinmass[isolid][i][j][k]
						+ (float)(advsedchinflux[isolid][i][j][k] * dt[idt] / 1000.0);

					//compute the cumulative outflowing sediment mass (kg)
					advsedchoutmass[isolid][i][j][k] = advsedchoutmass[isolid][i][j][k]
						+ (float)(advsedchoutflux[isolid][i][j][k] * dt[idt] / 1000.0);

					//compute the cumulative in-dispersing sediment mass (kg)
					dspsedchinmass[isolid][i][j][k] = dspsedchinmass[isolid][i][j][k]
						+ (float)(dspsedchinflux[isolid][i][j][k] * dt[idt] / 1000.0);

					//compute the cumulative out-dispersing sediment mass (kg)
					dspsedchoutmass[isolid][i][j][k] = dspsedchoutmass[isolid][i][j][k]
							+ (float)(dspsedchoutflux[isolid][i][j][k] * dt[idt] / 1000.0);

				}	//end loop over sources

				//Sum the solids mass entering and exiting the domain
				//via boundaries/outlets...
				//
				//Note:  The mass of solids transport from a link
				//       will always be zero unless the outlet
				//       number for the link is greater than zero.
				//
				//if the link is an outlet link and it is the last node of the link
				if(qchoutlet[i] > 0 && j == nnodes[i])
				{
					//compute the cumulative inflowing sediment mass (kg)
					advsedchinmass[isolid][i][j][10] = advsedchinmass[isolid][i][j][10]
						+ (float)(advsedchinflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute the cumulative outflowing sediment mass (kg)
					advsedchoutmass[isolid][i][j][10] = advsedchoutmass[isolid][i][j][10]
						+ (float)(advsedchoutflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute the cumulative in-dispersing sediment mass (kg)
					dspsedchinmass[isolid][i][j][10] = dspsedchinmass[isolid][i][j][10]
						+ (float)(dspsedchinflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute the cumulative out-dispersing sediment mass (kg)
					dspsedchoutmass[isolid][i][j][10] = dspsedchoutmass[isolid][i][j][10]
							+ (float)(dspsedchoutflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//Compute the cumulative solids mass exiting the domain at this outlet (kg)
					totaladvsedoutch[isolid][qchoutlet[i]] = totaladvsedoutch[isolid][qchoutlet[i]]
						+ (float)(advsedchoutflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//Compute the cumulative solids mass entering the domain at this outlet (kg)
					totaladvsedinch[isolid][qchoutlet[i]] = totaladvsedinch[isolid][qchoutlet[i]]
						+ (float)(advsedchinflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//Compute the cumulative solids mass exiting the domain at this outlet (kg)
					totaldspsedoutch[isolid][qchoutlet[i]] = totaldspsedoutch[isolid][qchoutlet[i]]
						+ (float)(dspsedchoutflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//Compute the cumulative solids mass entering the domain at this outlet (kg)
					totaldspsedinch[isolid][qchoutlet[i]] = totaldspsedinch[isolid][qchoutlet[i]]
						+ (float)(dspsedchinflux[isolid][i][j][10] * dt[idt] / 1000.0);

					//compute sediment discharge (kg/s) (advection plus dispersion)
					sedflow = (float)((advsedchoutflux[isolid][i][j][10]
						+ dspsedchoutflux[isolid][i][j][10]) / 1000.0);

					//Search for peak sediment discharge at this outlet...
					//
					//Note:  Peak values for individual solids types are
					//       computed below.  Peak values for the sum of
					//       all solids types are computed in a loop
					//       further below.
					//
					//if the sediment discharge is greater than the peak value
					if(sedflow > sedflowpeakch[isolid][qchoutlet[i]])
					{
						//Save peak discharge at outlet
						sedflowpeakch[isolid][qchoutlet[i]] = sedflow;

						//Save time of peak discharge
						sedtimepeakch[isolid][qchoutlet[i]] = (float)(simtime);

					}	//end if sedflow > sedflowpeakch[][]

				}	//end if qchoutlet[i] > 0 and j = nnodes[i]

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
				//Compute new channel mass...
				//
				//compute the new channel mass (g)
				newmassch = available + derivative * dt[idt];

				//Compute the new channel solids concentration...
				//
				//if the new cell water volume > zero
				if(newwatervol > 0.0)
				{
					//compute new water column solids concentration (g/m3)
					csedchnew[isolid][i][j][0] = (float)(newmassch / newwatervol);
				}
				else	//else newwatervol <= 0.0
				{
					//set the new concentration to zero
					csedchnew[isolid][i][j][0] = 0.0;

				}	//end if newwatervol > 0.0

				//Check new channel concentration for round-off error...
				//
				//If the new channel mass is negative
				if(newmassch < 0.0)
				{
					//Check for round-off error...
					//
					//if the magnitude is < the error tolerance
					if((float)fabs(newmassch) < TOLERANCE)
					{
						//The error is round-off...
						//
						//Reset the new concentration to exactly zero
						csedchnew[isolid][i][j][0] = 0.0;
					}	
					else	//else the error may be numerical instability
					{
						//Warn user of possible numerical instability (do not abort)
						//
						//Report error type (negative concentration in channel) and location
						SimulationError(5, i, j, isolid);

						//Reset the new concentration to exactly zero anyway...
						csedchnew[isolid][i][j][0] = 0.0;

					}	//end if fabs(newmassch) < TOLERANCE

				}	//end if newmassch < 0.0

				//Check if present link/node is a solids reporting station...
				//
				//Loop over number of flow reporting stations
				for (k=1; k<=nsedreports; k++)
				{
					//If current cell is a flow reporting station
					if(row == sedreprow[k] && col == sedrepcol[k])
					{
						//store the concentration for reporting (g/m3)
						sedreportch[isolid][k] = csedch[isolid][i][j][0];

					}	//end if current cell is a reporting station

				}	//end loop over reporting stations

			}	//end loop over solids

			//Search for peak total sediment discharge at outlets...
			//
			//Note:  Because the earlier code for channel outlets is not
			//       imbedded in a separate loop over solids, the search
			//       for the peak total solids discharge is placed after
			//       a separate loop to sum discharges for all solids
			//       types.
			//
			//if the link is an outlet link and it is the last node of the link
			if(qchoutlet[i] > 0 && j == nnodes[i])
			{
				//initialize outlet total (sum of all) solids discharge (kg/s)
				sedflowtotal = 0.0;

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//compute sediment discharge (kg/s) (advection plus dispersion)
					sedflow = (float)((advsedchoutflux[isolid][i][j][10]
						+ dspsedchoutflux[isolid][i][j][10]) / 1000.0);

					//compute total solids discharge (kg/s)
					sedflowtotal = sedflowtotal + sedflow;

				}	//end loop over solids

				//if the sediment discharge is greater than the peak value
				if(sedflowtotal > sedflowpeakch[0][qchoutlet[i]])
				{
					//Save total peak dischage at outlet (kg/s)
					sedflowpeakch[0][qchoutlet[i]] = sedflowtotal;

					//Save time of total peak flow (hours)
					sedtimepeakch[0][qchoutlet[i]] = (float)(simtime);

				}	//end if sedflowtotal > sedflowpeakch[0][]

			}	//end if qchoutlet[i] > 0 && j == nnodes[i]

			//Compute new (ending) volume of surface sediment layer...
			//
			//assign local value for surface sediment layer volume (m3)
			sedvol = vlayerch[i][j][ilayer];

			//initialize new sediment layer volume (m3)
			newsedvol = 0.0;

			//loop over solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//starting mass for this solids type (g)
				sedmass = csedch[isolid][i][j][ilayer] * sedvol;

				//Use scaled fluxes to compute the volume change in
				//the surface layer due to erosion and deposition
				//
				//deposited mass (gain) for this solids type (g)
				depositedmass = depsedchinflux[isolid][i][j][ilayer]
					* dt[idt];

				//eroded mass (loss) for this solids type (g)
				erodedmass = erssedchoutflux[isolid][i][j][ilayer]
					* dt[idt];

				//new (ending) mass for this solids type (g)
				newsedmass = sedmass + depositedmass - erodedmass;

				//Note:  Because the bulk density is computed from a
				//       combination of single and double precision
				//       values, it is typecast as float to truncate
				//       roundoff and then re-typecast as double to
				//       return the value to double precision.
				//
				//compute the bulk density of this solids type (kg/m3)
				bulkdensity = (double)((float)(spgravity[isolid] * densityw
						* (1.0 - porositych[i][j][ilayer])));

				//volume fraction of this solids type in surface layer (m3)
				deltavol = newsedmass / (bulkdensity * 1000.0);

				//compute new surface layer volume (m3)
				newsedvol = newsedvol + deltavol;

			}	//end loop over solids

			//Check for roundoff error...
			//
			//Note:  Watch out for roundoff errors.  As volume
			//       in a layer goes to zero (when erodedmass
			//       nearly equals sedmass + depositedmass),
			//       newsedvol can be a very small positive
			//       (or negative) number when it should be
			//       exactly zero.  These residual newsedvol
			//       values, combined with errors in layer
			//       masses cause erroneous concentration
			//       values...
			//
			//if the new surface layer volume < TOLERANCE
			if(newsedvol < TOLERANCE)
			{
				//reset the new volume (m3) to exactly zero
				newsedvol = 0.0;

			}	//end if newsedvol < TOLERANCE

			//Compute derivative for the surface sediment layer...
			//
			//loop over number of solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//if the new surface sediment layer volume >= TOLERANCE
				if(newsedvol >= TOLERANCE)
				{
					//compute present mass in surface layer (g)
					sedmass = csedch[isolid][i][j][ilayer] * sedvol;

					//compute erosion term (g/s)
					erosion = erssedchoutflux[isolid][i][j][ilayer];

					//compute deposition term (g/s)
					deposition = depsedchinflux[isolid][i][j][ilayer];

					//assign the influx (g/s)
					sedinflux = deposition;

					//assign the outflux (g/s)
					sedoutflux = erosion;

					//compute total derivative for the surface sediment layer (g/s)
					sedderivative = sedinflux - sedoutflux;

					//compute the new mass (g) of solids in the sediment layer
					newsedmass = sedmass + sedderivative * dt[idt];

					//Check for roundoff error...
					//
					//Note:  Watch out for roundoff errors.  As the mass
					//       in a layer goes to zero (when derivative*dt
					//       nearly equals sedmass), newsedmass can be a
					//       very small positive or negative number when
					//       it should be exactly zero.  These residual
					//       newsedmass values, combined with errors in
					//       layer volumes cause erroneous concentration
					//       values...
					//
					//if newsedmass > zero
					if(newsedmass > 0.0)
					{
						//compute new surface sediment layer concentration (g/m3)
						csedchnew[isolid][i][j][ilayer] = (float)(newsedmass / newsedvol);
					}
					else	//else newsedmass <= 0.0
					{
						//set the new concentration (g/m3) to zero
						csedchnew[isolid][i][j][ilayer] = 0.0;

					}	//end if newsedmass > 0.0
				}
				else	//else newsedvol < TOLERANCE
				{
					//set the new concentration to zero
					csedchnew[isolid][i][j][ilayer] = 0.0;

				}	//end of newsedvol >= TOLERANCE

			}	//end loop over solids

			//Assign global value of new surface sediment layer volume (m3)
			vlayerchnew[i][j][ilayer] = newsedvol;

			//if the surface sediment layer volume has reached
			//  the maximum volume limit
			if(newsedvol >= maxvlayerch[i][j][ilayer])
			{
				//raise the push flag for the channel network
				pushchflag = 1;

			}	//end if newsedvol>= maxvlayerch[][][]

			//if the surface sediment layer volume has reached the minimum
			//  volume limit and there are more layers in the stack
			if(newsedvol <= minvlayerch[i][j][ilayer] && ilayer > 1)
			{
				//raise the pop flag for the channel network
				popchflag = 1;

			}	//end if newsedvol >= maxvlayerch[][][]

			//Developer's Note:  If mass transport processes
			//                   occur in subsurface layers,
			//                   derivative terms for these
			//                   layers would be computed and
			//                   concentrations updated...

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to SolidsBalance
}
