/*---------------------------------------------------------------------
C-  Function:	ChannelChemicalConcentration.c
C-
C-	Purpose/	Computes chemical concentrations in the water column
C-	Methods:	and sediment bed of the channel network for use during
C-              the next time step t + dt.
C-
C-
C-  Inputs:	    cchemch[][][][],
C-              advchemchinflux[][][][], advchemchoutflux[][][][],
C-              dspchemchinflux[][][][], dspchemchoutflux[][][][],
C-              depchemchinflux[][][][], depchemchoutflux[][][][],
C-              erschemchinflux[][][][], erschemchoutflux[][][][],
C-              infchemchinflux[][][][], infchemchoutflux[][][][],
C-              reaction_fluxes[][][][], reaction_fluxes[][][][],
C-              channel properties
C-
C-  Outputs:    cchemchnew[][][][], chemreportch[][]
C-
C-  Controls:   None
C-
C-  Calls:      SimulationError
C-
C-  Called by:  ChemicalBalance
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
C-	Date:		14-JUL-2004
C-
C-	Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       13-JUN-2008
C-
C-	Revisions:  Revised calculation of derivative for bed layers to
C-              account for porewater transport of mobile chemical by
C-              transmission loss as the wetting front moves through
C-              the bed.  Added loop to calculate derivatives and
C-              update concentrations for all layers in the stack
C-              (surface and subsurface).
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

void ChannelChemicalConcentration()
{
	//local variable declarations/definitions
	int
		ichem,				//loop index for chemical number (type)
		ilayer,				//index for layer in the sediment stack
		row,				//row number of node (for reporting)
		col;				//column number of node (for reporting)

	float
		hchan,				//new depth of water in channel (at time t + dt) (m)
		bwch,				//bottom width of channel (m)
		hbch,				//bank height of channel (m)
		sslope,				//side slope of channel bank (dimensionless)
		twch,				//top width of channel at top of bank (m)
		achcross,			//cross sectional area of channel (m2)
		watervol,			//water column volume of node (at time t) (m3)
		newwatervol;		//new water column volume of node (at time t + dt) (m3)

	float
		advectionin,		//gross advection influx term (in) (g/s)
		advectionout,		//gross advection outflux term (out) (g/s)
		dispersionin,		//gross dispersion influx term (in) (g/s)
		dispersionout,		//gross dispersion outflux term (out) (g/s)
		erosion,			//gross erosion term (in) (g/s)
		deposition,			//gross deposition term (out) (g/s)
		porewaterin,		//gross porewater release influx term (in) (g/s)
		porewaterout,		//gross porewater release outflux term (out) (g/s)
		infiltrationin,		//gross chemical infiltration (transmission loss) (leaching) influx term (in) (g/s)
		infiltrationout,	//gross chemical infiltration (transmission loss) (leaching) outflux term (out) (g/s)
		transformationin,	//gross transformation influx term (in) (g/s)
		transformationout;	//gross transformation outflux term (out) (g/s)

	double
		influx,				//sum of all influx terms for derivative (g/s)
		outflux,			//sum of all outflux terms for derivative (g/s)
		derivative,			//mass derivative for water column (g/s)
		available,			//chemical mass available for transport (g)
		potential,			//chemical mass transport potential (g)
		newmassch,			//new chemical mass in channel (at time t + dt) (g)
		sedinflux,			//sum of all influx terms for sediment bed derivative (g/s)
		sedoutflux,			//sum of all outflux terms for sediment bed derivative (g/s)
		sedderivative,		//mass derivative for sediment bed (g/s)
		sedmass,			//sediment layer chemical mass of node (at time t) (g)
		newsedmass,			//new sediment layer chemical mass of node (at time t + dt) (g)
		sedvol,				//sediment layer volume of node (at time t) (m3)
		newsedvol;			//new sediment layer volume of node (at time t + dt) (m3)

	float
		chemflow,			//chemical discharge (kg/s)
		sumfp;				//sum of all particulate chemical fractions (sumfp = 1 - fd - fb)

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
			//loop over number of chemicals
			for(ichem=1; ichem<=nchems; ichem++)
			{
				//Build derivative term by term...
				//
				//  derivative = advectionin[ichem][i][j][source=0-10]
				//             - advectionout[ichem][i][j][source=0-10]
				//             + dispersionin[ichem][i][j][source=0-10]
				//             - dispersionout[ichem][i][j][source=0-10]
				//             + erosion[ichem][i][j][layer=0 or ilayer]
				//             - deposition[ichem][i][j][layer=0 or ilayer]
				//             + porewaterin[ichem][i][j][layer=0 or ilayer]
				//             - porewaterout[ichem][i][j][layer=0 or ilayer]
				//             + infiltrationin[ichem][i][j][layer=0 or ilayer]
				//             - infiltrationout[ichem][i][j][layer=0 or ilayer]
				//             + W[i][j] {=Wwater * Cchem} (included in advection term)
				//             + kineticsin[ichem][i][j][layer]
				//             - kineticsout[ichem][i][j][layer]
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
				porewaterin = 0.0;
				porewaterout = 0.0;
				infiltrationin = 0.0;
				infiltrationout = 0.0;
				transformationin = 0.0;
				transformationout = 0.0;

				//Source 0 = point source/load
				//Sources 1-8 = channel flows by direction
				//Source 9 = floodplain transfer
				//
				//loop over directions/sources 
				for(k=0; k<=9; k++)
				{
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advchemchinflux[ichem][i][j][k];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advchemchoutflux[ichem][i][j][k];

					//compute dispersion influx term (g/s)
					dispersionin = dispersionin
						+ dspchemchinflux[ichem][i][j][k];

					//compute dispersion outflux term (g/s)
					dispersionout = dispersionout
						+ dspchemchoutflux[ichem][i][j][k];

				}	//end loop over directions

				//Source 10 (domain boundary)...
				//
				//if the link is an outlet link and it is the last node of the link
				if(qchoutlet[i] > 0 && j == nnodes[i])
				{
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advchemchinflux[ichem][i][j][10];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advchemchoutflux[ichem][i][j][10];

					//compute dispersion influx term (g/s)
					dispersionin = dispersionin
						+ dspchemchinflux[ichem][i][j][10];

					//compute dispersion outflux term (g/s)
					dispersionout = dispersionout
						+ dspchemchoutflux[ichem][i][j][10];

				}	//end if qchoutlet[i] > 0 && j == nnodes[i]

				//compute erosion term (g/s)
				erosion = erschemchinflux[ichem][i][j][0];

				//compute deposition term (g/s)
				deposition = depchemchoutflux[ichem][i][j][0];

				//compute porewater influx term (g/s)
				porewaterin = pwrchemchinflux[ichem][i][j][0];

				//compute porewater outflux term (g/s)
				porewaterout = pwrchemchoutflux[ichem][i][j][0];

				//compute infiltration outflux term (g/s)
				infiltrationout = infchemchoutflux[ichem][i][j][0];

				//compute transformation influx term (g/s)
				transformationin = biochemchinflux[ichem][i][j][0]
					+ hydchemchinflux[ichem][i][j][0]
					+ oxichemchinflux[ichem][i][j][0]
					+ phtchemchinflux[ichem][i][j][0]
					+ radchemchinflux[ichem][i][j][0]
					+ vltchemchinflux[ichem][i][j][0]
					+ udrchemchinflux[ichem][i][j][0];

				//compute transformation outflux term (g/s)
				transformationout = biochemchoutflux[ichem][i][j][0]
					+ hydchemchoutflux[ichem][i][j][0]
					+ oxichemchoutflux[ichem][i][j][0]
					+ phtchemchoutflux[ichem][i][j][0]
					+ radchemchoutflux[ichem][i][j][0]
					+ vltchemchoutflux[ichem][i][j][0]
					+ udrchemchoutflux[ichem][i][j][0];

				//assign the influx (g/s)
				influx = advectionin + dispersionin + erosion
					+ porewaterin + transformationin;

				//assign the outflux (g/s)
				outflux = advectionout + dispersionout + deposition
					+ porewaterout + infiltrationout + transformationout;

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
				available = cchemch[ichem][i][j][0] * watervol;

				//if the overall outflux potential exceeds the available mass
				if(potential > available)
				{
					//Scale the outflux potential
					outflux = (available / potential) * outflux;

				}	//end if potential > available

				//Compute cumulative masses for all processes
				//
				//compute cumulative chemical mass leaving water column by deposition (kg)
				depchemchoutmass[ichem][i][j][0] = depchemchoutmass[ichem][i][j][0]
					+ depchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by erosion (kg)
				erschemchinmass[ichem][i][j][0] = erschemchinmass[ichem][i][j][0]
					+ erschemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by porewater release (kg)
				pwrchemchinmass[ichem][i][j][0] = pwrchemchinmass[ichem][i][j][0]
					+ pwrchemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass leaving water column by infiltration (kg)
				infchemchoutmass[ichem][i][j][0] = infchemchoutmass[ichem][i][j][0]
					+ infchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass leaving water column by biodegradation (kg)
				biochemchoutmass[ichem][i][j][0] = biochemchoutmass[ichem][i][j][0]
					+ biochemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by biodegradation (kg)
				biochemchinmass[ichem][i][j][0] = biochemchinmass[ichem][i][j][0]
					+ biochemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass leaving water column by hydrolysis (kg)
				hydchemchoutmass[ichem][i][j][0] = hydchemchoutmass[ichem][i][j][0]
					+ hydchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by hydrolysis (kg)
				hydchemchinmass[ichem][i][j][0] = hydchemchinmass[ichem][i][j][0]
					+ hydchemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass leaving water column by oxidation (kg)
				oxichemchoutmass[ichem][i][j][0] = oxichemchoutmass[ichem][i][j][0]
					+ oxichemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by oxidation (kg)
				oxichemchinmass[ichem][i][j][0] = oxichemchinmass[ichem][i][j][0]
					+ oxichemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass leaving water column by photolysis (kg)
				phtchemchoutmass[ichem][i][j][0] = phtchemchoutmass[ichem][i][j][0]
					+ phtchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by photolysis (kg)
				phtchemchinmass[ichem][i][j][0] = phtchemchinmass[ichem][i][j][0]
					+ phtchemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass leaving water column by radioactive decay (kg)
				radchemchoutmass[ichem][i][j][0] = radchemchoutmass[ichem][i][j][0]
					+ radchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by radioactive decay (kg)
				radchemchinmass[ichem][i][j][0] = radchemchinmass[ichem][i][j][0]
					+ radchemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass leaving water column by volatilization (kg)
				vltchemchoutmass[ichem][i][j][0] = vltchemchoutmass[ichem][i][j][0]
					+ vltchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative chemical mass entering water column by volatilization (kg)
				vltchemchinmass[ichem][i][j][0] = vltchemchinmass[ichem][i][j][0]
					+ vltchemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass lost in water column by user-defined reaction (kg)
				udrchemchoutmass[ichem][i][j][0] = udrchemchoutmass[ichem][i][j][0]
					+ udrchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute cumulative chemical mass gain in water column by user-defined reaction (kg)
				udrchemchinmass[ichem][i][j][0] = udrchemchinmass[ichem][i][j][0]
					+ udrchemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//Compute masses for loads (source = 0)
				//
				//compute the cumulative inflowing chemical mass (kg)
				advchemchinmass[ichem][i][j][0] = advchemchinmass[ichem][i][j][0]
					+ advchemchinflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//compute the cumulative outflowing chemical mass (kg)
				advchemchoutmass[ichem][i][j][0] = advchemchoutmass[ichem][i][j][0]
					+ advchemchoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

				//Compute the cumulative (net) chemical mass entering the domain from a point source (kg)
				totalcwch[ichem] = totalcwch[ichem]
					+ (advchemchinflux[ichem][i][j][0]
					- advchemchoutflux[ichem][i][j][0]) * dt[idt] / 1000.0;

				//loop over sources (including floodplain, source 9)
				for(k=1; k<=9; k++)
				{
					//compute the cumulative inflowing chemical mass (kg)
					advchemchinmass[ichem][i][j][k] = advchemchinmass[ichem][i][j][k]
						+ advchemchinflux[ichem][i][j][k] * dt[idt] / 1000.0;

					//compute the cumulative outflowing chemical mass (kg)
					advchemchoutmass[ichem][i][j][k] = advchemchoutmass[ichem][i][j][k]
						+ advchemchoutflux[ichem][i][j][k] * dt[idt] / 1000.0;

					//compute the cumulative in-dispersing chemical mass (kg)
					dspchemchinmass[ichem][i][j][k] = dspchemchinmass[ichem][i][j][k]
						+ dspchemchinflux[ichem][i][j][k] * dt[idt] / 1000.0;

					//compute the cumulative out-dispersing chemical mass (kg)
					dspchemchoutmass[ichem][i][j][k] = dspchemchoutmass[ichem][i][j][k]
							+ dspchemchoutflux[ichem][i][j][k] * dt[idt] / 1000.0;

				}	//end loop over sources

				//Sum the chemical mass entering and exiting the domain
				//via boundaries/outlets...
				//
				//Note:  The mass of chemical transport from a link
				//       will always be zero unless the outlet
				//       number for the link is greater than zero.
				//
				//if the link is an outlet link and it is the last node of the link
				if(qchoutlet[i] > 0 && j == nnodes[i])
				{
					//compute the cumulative inflowing chemical mass (kg)
					advchemchinmass[ichem][i][j][10] = advchemchinmass[ichem][i][j][10]
						+ advchemchinflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//compute the cumulative outflowing chemical mass (kg)
					advchemchoutmass[ichem][i][j][10] = advchemchoutmass[ichem][i][j][10]
						+ advchemchoutflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//compute the cumulative in-dispersing chemical mass (kg)
					dspchemchinmass[ichem][i][j][10] = dspchemchinmass[ichem][i][j][10]
						+ dspchemchinflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//compute the cumulative out-dispersing chemical mass (kg)
					dspchemchoutmass[ichem][i][j][10] = dspchemchoutmass[ichem][i][j][10]
						+ dspchemchoutflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//Compute the cumulative chemical mass exiting the domain at this outlet (kg)
					totaladvchemoutch[ichem][qchoutlet[i]] = totaladvchemoutch[ichem][qchoutlet[i]]
						+ advchemchoutflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//Compute the cumulative chemical mass entering the domain at this outlet (kg)
					totaladvcheminch[ichem][qchoutlet[i]] = totaladvcheminch[ichem][qchoutlet[i]]
						+ advchemchinflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//Compute the cumulative chemical mass exiting the domain at this outlet (kg)
					totaldspchemoutch[ichem][qchoutlet[i]] = totaldspchemoutch[ichem][qchoutlet[i]]
						+ dspchemchoutflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//Compute the cumulative chemical mass entering the domain at this outlet (kg)
					totaldspcheminch[ichem][qchoutlet[i]] = totaldspcheminch[ichem][qchoutlet[i]]
						+ dspchemchinflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//compute chemical discharge (kg/s) (advection plus dispersion)
					chemflow = (float)((advchemchoutflux[ichem][i][j][10]
						+ dspchemchoutflux[ichem][i][j][10]) / 1000.0);

					//Search for peak export at this outlet...
					//
					//if the chemical discharge is greater than the peak value
					if(chemflow > chemflowpeakch[ichem][qchoutlet[i]])
					{
						//Save peak discharge at outlet
						chemflowpeakch[ichem][qchoutlet[i]] = chemflow;

						//Save time of peak discharge
						chemtimepeakch[ichem][qchoutlet[i]] = (float)(simtime);

					}	//end if chemflow > chemflowpeakch[][]

				}	//end if qchoutlet[i] > 0 and j = nnodes[i]

				//Note:  The derivative calculation below does not
				//       include (within cell) floodplain transfers.
				//
				//compute total derivative for the water column (g/s)
				derivative = influx - outflux;

				//Perform numerical integration for water column...
				//
				//Note:  For better error trapping, numerical integration
				//       is performed in three steps.  First, the new mass
				//       is computed.  Second new mass value is checked
				//       for instability.  Third, the new concentration is
				//       computed from the new mass and the new volume.
				//
				//compute the new channel mass (g)
				newmassch = available + derivative * dt[idt];

				//Compute the new channel chemical concentration...
				//
				//if the new cell water volume > zero
				if(newwatervol > 0.0)
				{
					//compute new water column chemical concentration (g/m3)
					cchemchnew[ichem][i][j][0] = (float)(newmassch / newwatervol);
				}
				else	//else newwatervol <= 0.0
				{
					//set the new concentration to zero
					cchemchnew[ichem][i][j][0] = 0.0;

				}	//end if newwatervol > 0.0

				//Check new channel concentration for round-off error...
				//
				//If the new channel concentration is negative
				if(cchemchnew[ichem][i][j][0] < 0.0)
				{
					//Check for round-off error...
					//
					//if the magnitude is < the error tolerance
					if((float)fabs(cchemchnew[ichem][i][j][0]) < TOLERANCE)
					{
						//The error is round-off...
						//
						//Reset the new concentration to exactly zero
						cchemchnew[ichem][i][j][0] = 0.0;
					}
					else	//else the error is (probably) numerical instability
					{
						//Warn user of possible numerical instability (do not abort)
						//
						//Report error type (negative concentration in channel) and location
						SimulationError(7, i, j, ichem);

						//Reset the new concentration to exactly zero anyway...
						cchemchnew[ichem][i][j][0] = 0.0;

					}	//end if fabs(cchemchnew[ichem][i][j][0]) < TOLERANCE

				}	//end if cchemchnew < 0.0

				//Check if present link/node is a chemical reporting station...
				//
				//Loop over number of chemical reporting stations
				for (k=1; k<=nchemreports; k++)
				{
					//If current cell is a chemical reporting station
					if(row == chemreprow[k] && col == chemrepcol[k])
					{
						//compute the sum of all particulate chemical fractions...
						//
						//compute sum of fp by difference from fd and fb
						sumfp = (float)(1.0 - fdissolvedch[ichem][i][j][0]
							- fboundch[ichem][i][j][0]);

						//store concentrations for reporting (g/m3)
						totchemreportch[ichem][k] = cchemch[ichem][i][j][0];
						dischemreportch[ichem][k] = cchemch[ichem][i][j][0]
							* fdissolvedch[ichem][i][j][0];
						bndchemreportch[ichem][k] = cchemch[ichem][i][j][0]
							* fboundch[ichem][i][j][0];
						prtchemreportch[ichem][k] = cchemch[ichem][i][j][0]
							* sumfp;

					}	//end if current cell is a reporting station

				}	//end loop over reporting stations

			}	//end loop over chemicals

			//Compute derivative for sediment layers...
			//
			//loop over layers (in reverse order: top down)
			for(ilayer=nstackch[i][j]; ilayer>=1; ilayer--)
			{
				//assign sediment layer volume (m3)
				sedvol = vlayerch[i][j][ilayer];

				//Note:  For the surface layer, the new layer volume
				//       is calculated in ChannelSolidsConcentration
				//       because the layer volume can change over time.
				//       For subsurface layers, the new layer volume
				//       should always equal the layer volume and is
				//       set/reset in the push and pop stack routines
				//       when the stack is updated.
				//
				//assign new sediment layer volume (m3)
				newsedvol = vlayerchnew[i][j][ilayer];

				//loop over number of chemicals
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//compute the mass available for transport/transformation (g)
					sedmass = cchemch[ichem][i][j][ilayer] * sedvol;

					//if the new sediment layer volume >= TOLERANCE
					if(newsedvol >= TOLERANCE)
					{
						//compute erosion term (g/s)
						erosion = erschemchoutflux[ichem][i][j][ilayer];

						//compute deposition term (g/s)
						deposition = depchemchinflux[ichem][i][j][ilayer];

						//compute porewater transport influx term (g/s)
						porewaterin = pwrchemchinflux[ichem][i][j][ilayer]
							+ infchemchinflux[ichem][i][j][ilayer];

						//compute porewater transport outflux term (g/s)
						porewaterout = pwrchemchoutflux[ichem][i][j][ilayer]
							+ infchemchoutflux[ichem][i][j][ilayer];

						//compute transformation influx term (g/s)
						transformationin = biochemchinflux[ichem][i][j][ilayer]
							+ hydchemchinflux[ichem][i][j][ilayer]
							+ oxichemchinflux[ichem][i][j][ilayer]
							+ phtchemchinflux[ichem][i][j][ilayer]
							+ radchemchinflux[ichem][i][j][ilayer]
							+ vltchemchinflux[ichem][i][j][ilayer]
							+ udrchemchinflux[ichem][i][j][ilayer];

						//compute transformation outflux term (g/s)
						transformationout = biochemchoutflux[ichem][i][j][ilayer]
							+ hydchemchoutflux[ichem][i][j][ilayer]
							+ oxichemchoutflux[ichem][i][j][ilayer]
							+ phtchemchoutflux[ichem][i][j][ilayer]
							+ radchemchoutflux[ichem][i][j][ilayer]
							+ vltchemchoutflux[ichem][i][j][ilayer]
							+ udrchemchoutflux[ichem][i][j][ilayer];

						//assign the influx (g/s)
						sedinflux = deposition + porewaterin + transformationin;

						//assign the outflux (g/s)
						sedoutflux = erosion + porewaterout + transformationout;

						//Note:  While the transport potential from the sum
						//       of individual process fluxes should never
						//       exceed the mass available for transport,
						//       roundoff error may still exist.  So, the
						//       total transport potential must be checked
						//       against the total mass available to ensure
						//       stability.
						//
						//compute the overall transport/transfer outflux potential (g)
						potential = sedoutflux * dt[idt];

						//Note:  The available mass is equal to sedmass defined above.
						//       We define available mass just so the code is parallel
						//       to the similar code used here and elsewhere...
						//
						//compute the mass available for transport/transformation (g)
						available = cchemch[ichem][i][j][ilayer] * sedvol;

						//if the overall outflux potential exceeds the available mass
						if(potential > available)
						{
							//Scale the outflux potential
							sedoutflux = (available / potential) * sedoutflux;

						}	//end if potential > available

						//Compute cumulative masses for all processes
						//
						//compute the cumulative chemical mass entering sediment bed by deposition (kg)
						depchemchinmass[ichem][i][j][ilayer] = depchemchinmass[ichem][i][j][ilayer]
							+ depchemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by erosion (kg)
						erschemchoutmass[ichem][i][j][ilayer] = erschemchoutmass[ichem][i][j][ilayer]
							+ erschemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by porewater release (kg)
						pwrchemchoutmass[ichem][i][j][ilayer] = pwrchemchoutmass[ichem][i][j][ilayer]
							+ pwrchemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass leaving sediment bed by infiltration (trans loss) (kg)
						infchemchoutmass[ichem][i][j][ilayer] = infchemchoutmass[ichem][i][j][ilayer]
							+ infchemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by infiltration (trans loss) (kg)
						infchemchinmass[ichem][i][j][ilayer] = infchemchinmass[ichem][i][j][ilayer]
							+ infchemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by biodegradation (kg)
						biochemchoutmass[ichem][i][j][ilayer] = biochemchoutmass[ichem][i][j][ilayer]
							+ biochemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by biodegradation (kg)
						biochemchinmass[ichem][i][j][ilayer] = biochemchinmass[ichem][i][j][ilayer]
							+ biochemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by hydrolysis (kg)
						hydchemchoutmass[ichem][i][j][ilayer] = hydchemchoutmass[ichem][i][j][ilayer]
							+ hydchemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by hydrolysis (kg)
						hydchemchinmass[ichem][i][j][ilayer] = hydchemchinmass[ichem][i][j][ilayer]
							+ hydchemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by oxidation (kg)
						oxichemchoutmass[ichem][i][j][ilayer] = oxichemchoutmass[ichem][i][j][ilayer]
							+ oxichemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by oxidation (kg)
						oxichemchinmass[ichem][i][j][ilayer] = oxichemchinmass[ichem][i][j][ilayer]
							+ oxichemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving bed by photolysis (kg)
						phtchemchoutmass[ichem][i][j][ilayer] = phtchemchoutmass[ichem][i][j][ilayer]
							+ phtchemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by photolysis (kg)
						phtchemchinmass[ichem][i][j][ilayer] = phtchemchinmass[ichem][i][j][ilayer]
							+ phtchemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by radioactive decay (kg)
						radchemchoutmass[ichem][i][j][ilayer] = radchemchoutmass[ichem][i][j][ilayer]
							+ radchemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by radioactive decay (kg)
						radchemchinmass[ichem][i][j][ilayer] = radchemchinmass[ichem][i][j][ilayer]
							+ radchemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by volatilization (kg)
						vltchemchoutmass[ichem][i][j][ilayer] = vltchemchoutmass[ichem][i][j][ilayer]
							+ vltchemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by volatilization (kg)
						vltchemchinmass[ichem][i][j][ilayer] = vltchemchinmass[ichem][i][j][ilayer]
							+ vltchemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute cumulative chemical mass leaving sediment bed by user-defined reaction (kg)
						udrchemchoutmass[ichem][i][j][ilayer] = udrchemchoutmass[ichem][i][j][ilayer]
							+ udrchemchoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute the cumulative chemical mass entering sediment bed by user-defined reaction (kg)
						udrchemchinmass[ichem][i][j][ilayer] = udrchemchinmass[ichem][i][j][ilayer]
							+ udrchemchinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

						//compute total derivative for the sediment layer (g/s)
						sedderivative = sedinflux - sedoutflux;

						//compute the new mass (g) of chemical in the sediment layer
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
							//compute new sediment layer concentration (g/m3)
							cchemchnew[ichem][i][j][ilayer] = (float)(newsedmass / newsedvol);
						}
						else	//else newsedmass <= 0.0
						{
							//set the new concentration (g/m3) to zero
							cchemchnew[ichem][i][j][ilayer] = 0.0;

						}	//end if newsedmass > 0.0
					}
					else	//else newsedvol < TOLERANCE
					{
						//set the new concentration to zero
						cchemchnew[ichem][i][j][ilayer] = 0.0;

					}	//end of newsedvol >= TOLERANCE

				}	//end loop over chemicals

			}	//end loop over layers

		}	//end loop over nodes

	}	//end loop over links

//End of function: Return to ChemicalBalance
}
