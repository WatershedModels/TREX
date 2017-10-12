/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalConcentration.c
C-
C-	Purpose/	Computes chemical concentration in the water column
C-	Methods:	and soil bed of the overland plane for use during the
C-              next time step t + dt.
C-
C-
C-  Inputs:	    cchemov[][][][],
C-              advchemovinflux[][][][], advchemovoutflux[][][][],
C-              dspchemovinflux[][][][], dspchemovoutflux[][][][],
C-              depchemovinflux[][][][], depchemovoutflux[][][][],
C-              erschemovinflux[][][][], erschemovoutflux[][][][],
C-              pwrchemovinflux[][][][], pwrchemovoutflux[][][][],
C-              infchemovinflux[][][][], infchemovoutflux[][][][],
C-              reaction_fluxes[][][][], reaction_fluxes[][][][],
C-              channel properties, cell properties
C-
C-  Outputs:    cchemovnew[][][][], chemreportov[][]
C-
C-  Controls:   imask[][]
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
C-	Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       13-JUN-2008
C-
C-	Revisions:  Revised calculation of derivative for soil layers to
C-              account for porewater transport of mobile chemical by
C-              infiltration as the wetting front moves through the
C-              soil.  Added loop to calculate derivatives and
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

void OverlandChemicalConcentration()
{
	//local variable declarations/definitions
	int
		ichem,				//loop index for chemical number (type)
		ilayer,				//index for layer in the sediment stack
		isoil,				//soil type of cell
		chanlink,			//channel link number
		channode;			//channel node number

	float
		hcell,				//new depth of water in cell (at time t + dt) (m)
		lch,				//channel length (m) (includes sinuosity)
		twch,				//top width of channel at top of bank (m)
		achsurf,			//surface area of channel (m2)
		aovsurf,			//surface area of overland cell (m2)
		watervol,			//water column volume of cell (at time t) (m3)
		newwatervol;		//new water column volume of cell (at time t + dt) (m3)

	float
		advectionin,		//gross advection influx term (in) (g/s)
		advectionout,		//gross advection outflux term (out) (g/s)
		dispersionin,		//gross dispersion influx term (in) (g/s)
		dispersionout,		//gross dispersion outflux term (out) (g/s)
		erosion,			//gross erosion term (in) (g/s)
		deposition,			//gross deposition term (out) (g/s)
		porewaterin,		//gross porewater release influx term (in) (g/s)
		porewaterout,		//gross porewater release outflux term (out) (g/s)
		infiltrationin,		//gross chemical infiltration (leaching) influx term (in) (g/s)
		infiltrationout,	//gross chemical infiltration (leaching) outflux term (out) (g/s)
		transformationin,	//gross transformation influx term (in) (g/s)
		transformationout;	//gross transformation outflux term (out) (g/s)

	double
		influx,				//sum of all influx terms for derivative (g/s)
		outflux,			//sum of all outflux terms for derivative (g/s)
		derivative,			//mass derivative (g/s)
		available,			//chemical mass available for transport (g)
		potential,			//chemical mass transport potential (g)
		newmass,			//new chemical mass in cell (at time t + dt) (g)
		soilinflux,			//sum of all influx terms for soil bed derivative (g/s)
		soiloutflux,		//sum of all outflux terms for soil bed derivative (g/s)
		soilderivative,		//mass derivative for soil bed (g/s)
		soilmass,			//soil layer chemical mass of node (at time t) (g)
		newsoilmass,		//new soil layer chemical mass of node (at time t + dt) (g)
		soilvol,			//soil layer volume of cell (at time t) (m3)
		newsoilvol;			//new soil layer volume of cell (at time t + dt) (m3)

	float
		chemflow,			//chemical discharge (kg/s)
		sumfp;				//sum of all particulate chemical fractions (sumfp = 1 - fd - fb)

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
					//             + kineticsin[ichem][i][j]
					//             - kineticsout[ichem][i][j]
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
					porewaterin = 0.0;
					porewaterout = 0.0;
					infiltrationin = 0.0;
					infiltrationout = 0.0;
					transformationin = 0.0;
					transformationout = 0.0;

					//Compute derivative for water column...
					//
					//Source 0: point sources/sinks (no dispersion term)
					//
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advchemovinflux[ichem][i][j][0];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advchemovoutflux[ichem][i][j][0];

					//loop over sources (1,3,5,7)
					for(k=1; k<=7; k=k+2)
					{
						//compute advection influx term (g/s)
						advectionin = advectionin
							+ advchemovinflux[ichem][i][j][k];

						//compute advection outflux term (g/s)
						advectionout = advectionout
							+ advchemovoutflux[ichem][i][j][k];

						//compute dispersion influx term (g/s)
						dispersionin = dispersionin
							+ dspchemovinflux[ichem][i][j][k];

						//compute dispersion outflux term (g/s)
						dispersionout = dispersionout
							+ dspchemovoutflux[ichem][i][j][k];

					}	//end loop over sources

					//Source 9: floodplain transfer
					//
					//Note:  Values for source 9 will be zero
					//       unless the cell is a channel cell...
					//
					//if imask > 1 (if cell is a chennel cell)
					if(imask[i][j] > 1)
					{
						//compute advection influx term (g/s)
						advectionin = advectionin
							+ advchemovinflux[ichem][i][j][9];

						//compute advection outflux term (g/s)
						advectionout = advectionout
							+ advchemovoutflux[ichem][i][j][9];

						//compute dispersion influx term (g/s)
						dispersionin = dispersionin
							+ dspchemovinflux[ichem][i][j][9];

						//compute dispersion outflux term (g/s)
						dispersionout = dispersionout
							+ dspchemovoutflux[ichem][i][j][9];

					}	//end if imask[][] > 1

					//Source 10: domain boundary (outlet)
					//
					//Note:  Values for source 10 will be zero
					//       unless the cell is an outlet...
					//
					//compute advection influx term (g/s)
					advectionin = advectionin
						+ advchemovinflux[ichem][i][j][10];

					//compute advection outflux term (g/s)
					advectionout = advectionout
						+ advchemovoutflux[ichem][i][j][10];

					//compute dispersion influx term (g/s)
					dispersionin = dispersionin
						+ dspchemovinflux[ichem][i][j][10];

					//compute dispersion outflux term (g/s)
					dispersionout = dispersionout
						+ dspchemovoutflux[ichem][i][j][10];

					//Remaining sources...
					//
					//compute erosion term (g/s)
					erosion = erschemovinflux[ichem][i][j][0];

					//compute deposition term (g/s)
					deposition = depchemovoutflux[ichem][i][j][0];

					//compute porewater release influx term (g/s)
					porewaterin = pwrchemovinflux[ichem][i][j][0];

					//compute porewater release outflux term (g/s)
					porewaterout = pwrchemovoutflux[ichem][i][j][0];

					//compute infiltration outflux term (g/s)
					infiltrationout = infchemovoutflux[ichem][i][j][0];

					//compute transformation influx term (g/s)
					transformationin = biochemovinflux[ichem][i][j][0]
						+ hydchemovinflux[ichem][i][j][0]
						+ oxichemovinflux[ichem][i][j][0]
						+ phtchemovinflux[ichem][i][j][0]
						+ radchemovinflux[ichem][i][j][0]
						+ vltchemovinflux[ichem][i][j][0]
						+ udrchemovinflux[ichem][i][j][0];

					//compute transformation outflux term (g/s)
					transformationout = biochemovoutflux[ichem][i][j][0]
						+ hydchemovoutflux[ichem][i][j][0]
						+ oxichemovoutflux[ichem][i][j][0]
						+ phtchemovoutflux[ichem][i][j][0]
						+ radchemovoutflux[ichem][i][j][0]
						+ vltchemovoutflux[ichem][i][j][0]
						+ udrchemovoutflux[ichem][i][j][0];

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
					available = cchemov[ichem][i][j][0] * watervol;

					//if the overall outflux potential exceeds the available mass
					if(potential > available)
					{
						//Scale the outflux potential
						outflux = (available / potential) * outflux;

					}	//end if potential > available

					//Compute cumulative masses for all processes
					//
					//compute cumulative chemical mass leaving water column by deposition (kg)
					depchemovoutmass[ichem][i][j][0] = depchemovoutmass[ichem][i][j][0]
						+ depchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute the cumulative chemical mass entering the water column (kg)
					erschemovinmass[ichem][i][j][0] = erschemovinmass[ichem][i][j][0]
						+ erschemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute the cumulative chemical mass entering the water column by porewater release (kg)
					pwrchemovinmass[ichem][i][j][0] = pwrchemovinmass[ichem][i][j][0]
						+ pwrchemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass leaving the water column by infiltration (kg)
					infchemovoutmass[ichem][i][j][0] = infchemovoutmass[ichem][i][j][0]
						+ infchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass lost in water column by biodegradation (kg)
					biochemovoutmass[ichem][i][j][0] = biochemovoutmass[ichem][i][j][0]
						+ biochemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass gain in water column by biodegradation (kg)
					biochemovinmass[ichem][i][j][0] = biochemovinmass[ichem][i][j][0]
						+ biochemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass lost in water column by hydrolysis (kg)
					hydchemovoutmass[ichem][i][j][0] = hydchemovoutmass[ichem][i][j][0]
						+ hydchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass gain in water column by hydrolysis (kg)
					hydchemovinmass[ichem][i][j][0] = hydchemovinmass[ichem][i][j][0]
						+ hydchemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass lost in water column by oxidation (kg)
					oxichemovoutmass[ichem][i][j][0] = oxichemovoutmass[ichem][i][j][0]
						+ oxichemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass gain in water column by oxidation (kg)
					oxichemovinmass[ichem][i][j][0] = oxichemovinmass[ichem][i][j][0]
						+ oxichemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass lost in water column by photolysis (kg)
					phtchemovoutmass[ichem][i][j][0] = phtchemovoutmass[ichem][i][j][0]
						+ phtchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass gain in water column by photolysis (kg)
					phtchemovinmass[ichem][i][j][0] = phtchemovinmass[ichem][i][j][0]
						+ phtchemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass lost in water column by radioactive decay (kg)
					radchemovoutmass[ichem][i][j][0] = radchemovoutmass[ichem][i][j][0]
						+ radchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass gain in water column by radioactive decay (kg)
					radchemovinmass[ichem][i][j][0] = radchemovinmass[ichem][i][j][0]
						+ radchemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass lost in water column by volatilization (kg)
					vltchemovoutmass[ichem][i][j][0] = vltchemovoutmass[ichem][i][j][0]
						+ vltchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass gain in water column by volatilization (kg)
					vltchemovinmass[ichem][i][j][0] = vltchemovinmass[ichem][i][j][0]
						+ vltchemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass lost in water column by user-defined reaction (kg)
					udrchemovoutmass[ichem][i][j][0] = udrchemovoutmass[ichem][i][j][0]
						+ udrchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute cumulative chemical mass gain in water column by user-defined reaction (kg)
					udrchemovinmass[ichem][i][j][0] = udrchemovinmass[ichem][i][j][0]
						+ udrchemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//Compute masses for loads (source = 0)
					//
					//compute the cumulative inflowing chemical mass (kg)
					advchemovinmass[ichem][i][j][0] = advchemovinmass[ichem][i][j][0]
						+ advchemovinflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//compute the cumulative outflowing chemical mass (kg)
					advchemovoutmass[ichem][i][j][0] = advchemovoutmass[ichem][i][j][0]
						+ advchemovoutflux[ichem][i][j][0] * dt[idt] / 1000.0;

					//Compute the cumulative (net) chemical mass entering the domain from a point source (kg)
					totalcwov[ichem] = totalcwov[ichem]
						+ (advchemovinflux[ichem][i][j][0]
						- advchemovoutflux[ichem][i][j][0]) * dt[idt] / 1000.0;

					//loop over overland sources (1,3,5,7)
					for(k=1; k<=7; k=k+2)
					{
						//compute the cumulative inflowing chemical mass (kg)
						advchemovinmass[ichem][i][j][k] = advchemovinmass[ichem][i][j][k]
							+ advchemovinflux[ichem][i][j][k] * dt[idt] / 1000.0;

						//compute the cumulative outflowing chemical mass (kg)
						advchemovoutmass[ichem][i][j][k] = advchemovoutmass[ichem][i][j][k]
							+ advchemovoutflux[ichem][i][j][k] * dt[idt] / 1000.0;

						//compute the cumulative in-dispersing chemical mass (kg)
						dspchemovinmass[ichem][i][j][k] = dspchemovinmass[ichem][i][j][k]
							+ dspchemovinflux[ichem][i][j][k] * dt[idt] / 1000.0;

						//compute the cumulative out-dispersing chemical mass (kg)
						dspchemovoutmass[ichem][i][j][k] = dspchemovoutmass[ichem][i][j][k]
							+ dspchemovoutflux[ichem][i][j][k] * dt[idt] / 1000.0;

					}	//end loop over sources

					//Compute masses for floodplain (source = 9)
					//
					//Note:  These values will be zero unless the cell is
					//       an channel cell...
					//
					//if imask > 1 (if cell is a channel cell)
					if(imask[i][j] > 1)
					{
						//compute the cumulative inflowing chemical mass (kg)
						advchemovinmass[ichem][i][j][9] = advchemovinmass[ichem][i][j][9]
							+ advchemovinflux[ichem][i][j][9] * dt[idt] / 1000.0;

						//compute the cumulative outflowing chemical mass (kg)
						advchemovoutmass[ichem][i][j][9] = advchemovoutmass[ichem][i][j][9]
							+ advchemovoutflux[ichem][i][j][9] * dt[idt] / 1000.0;

						//compute the cumulative in-dispersing chemical mass (kg)
						dspchemovinmass[ichem][i][j][9] = dspchemovinmass[ichem][i][j][9]
							+ dspchemovinflux[ichem][i][j][9] * dt[idt] / 1000.0;

						//compute the cumulative out-dispersing chemical mass (kg)
						dspchemovoutmass[ichem][i][j][9] = dspchemovoutmass[ichem][i][j][9]
							+ dspchemovoutflux[ichem][i][j][9] * dt[idt] / 1000.0;

					}	//end if imask[][] > 1

					//Compute masses for boundaries (source = 10)
					//
					//Note:  These values will be zero unless the cell is
					//       an outlet cell...
					//
					//compute the cumulative inflowing chemical mass (kg)
					advchemovinmass[ichem][i][j][10] = advchemovinmass[ichem][i][j][10]
						+ advchemovinflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//compute the cumulative outflowing chemical mass (kg)
					advchemovoutmass[ichem][i][j][10] = advchemovoutmass[ichem][i][j][10]
						+ advchemovoutflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//compute the cumulative in-dispersing chemical mass (kg)
					dspchemovinmass[ichem][i][j][10] = dspchemovinmass[ichem][i][j][10]
						+ dspchemovinflux[ichem][i][j][10] * dt[idt] / 1000.0;

					//compute the cumulative out-dispersing chemical mass (kg)
					dspchemovoutmass[ichem][i][j][10] = dspchemovoutmass[ichem][i][j][10]
						+ dspchemovoutflux[ichem][i][j][10] * dt[idt] / 1000.0;

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
					//compute the new mass (g)
					newmass = available + derivative * dt[idt];

					//Compute the new cell chemical concentration...
					//
					//if the new cell water volume > zero
					if(newwatervol > 0.0)
					{
						//compute new water column solids concentration (g/m3)
						cchemovnew[ichem][i][j][0] = (float)(newmass / newwatervol);
					}
					else	//else newwatervol <= 0.0
					{
						//set the new concentration to zero
						cchemovnew[ichem][i][j][0] = 0.0;

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
							//Reset the new concentration to exactly zero
							cchemovnew[ichem][i][j][0] = 0.0;
						}	
						else	//else the error is (probably) numerical instability
						{
							//Warn user of possible numerical instability (do not abort)
							//
							//Report error type (negative concentration overland) and location
							SimulationError(6, i, j, ichem);

							//Reset the new concentration to exactly zero anyway...
							cchemovnew[ichem][i][j][0] = 0.0;

						}	//end if fabs(newmass) < TOLERANCE

					}	//end if newmass < 0.0

				}	//end loop over chemicals

				//Compute derivative for soil layers
				//
				//loop over layers (in reverse order: top down)
				for(ilayer=nstackov[i][j]; ilayer>=1; ilayer--)
				{
					//assign soil layer volume (m3)
					soilvol = vlayerov[i][j][ilayer];

					//Note:  For the surface layer, the new layer volume
					//       is calculated in OverlandSolidsConcentration
					//       because the layer volume can change over time.
					//       For subsurface layers, the new layer volume
					//       should always equal the layer volume and is
					//       set/reset in the push and pop stack routines
					//       when the stack is updated.
					//
					//assign new soil layer volume (m3)
					newsoilvol = vlayerovnew[i][j][ilayer];

					//loop over number of chemicals
					for(ichem=1; ichem<=nchems; ichem++)
					{
						//if the new soil layer volume >= TOLERANCE
						if(newsoilvol >= TOLERANCE)
						{
							//compute the mass available for transport/transformation (g)
							soilmass = cchemov[ichem][i][j][ilayer] * soilvol;

							//compute erosion term (g/s)
							erosion = erschemovoutflux[ichem][i][j][ilayer];

							//compute deposition term (g/s)
							deposition = depchemovinflux[ichem][i][j][ilayer];

							//compute porewater transport influx term (g/s)
							porewaterin = pwrchemovinflux[ichem][i][j][ilayer]
								+ infchemovinflux[ichem][i][j][ilayer];

							//compute porewater transport outflux term (g/s)
							porewaterout = pwrchemovoutflux[ichem][i][j][ilayer]
								+ infchemovoutflux[ichem][i][j][ilayer];

							//compute transformation influx term (g/s)
							transformationin = biochemovinflux[ichem][i][j][ilayer]
								+ hydchemovinflux[ichem][i][j][ilayer]
								+ oxichemovinflux[ichem][i][j][ilayer]
								+ phtchemovinflux[ichem][i][j][ilayer]
								+ radchemovinflux[ichem][i][j][ilayer]
								+ vltchemovinflux[ichem][i][j][ilayer]
								+ udrchemovinflux[ichem][i][j][ilayer];

							//compute transformation outflux term (g/s)
							transformationout = biochemovoutflux[ichem][i][j][ilayer]
								+ hydchemovoutflux[ichem][i][j][ilayer]
								+ oxichemovoutflux[ichem][i][j][ilayer]
								+ phtchemovoutflux[ichem][i][j][ilayer]
								+ radchemovoutflux[ichem][i][j][ilayer]
								+ vltchemovoutflux[ichem][i][j][ilayer]
								+ udrchemovoutflux[ichem][i][j][ilayer];

							//assign the influx (g/s)
							soilinflux = deposition + porewaterin + transformationin;

							//assign the outflux (g/s)
							soiloutflux = erosion + porewaterout + transformationout;

							//Note:  While the transport potential from the sum
							//       of individual process fluxes should never
							//       exceed the mass available for transport,
							//       roundoff error may still exist.  So, the
							//       total transport potential must be checked
							//       against the total mass available to ensure
							//       stability.
							//
							//compute the overall transport/transfer outflux potential (g)
							potential = soiloutflux * dt[idt];

							//Note:  The available mass is equal to soilmass defined above.
							//       We define available mass just so the code is parallel
							//       to the similar code used here and elsewhere...
							//
							//compute the mass available for transport/transformation (g)
							available = cchemov[ichem][i][j][ilayer] * soilvol;

							//if the overall outflux potential exceeds the available mass
							if(potential > available)
							{
								//Scale the outflux potential
								soiloutflux = (available / potential) * soiloutflux;

							}	//end if potential > available

							//Compute cumulative masses for all processes
							//
							//compute the cumulative chemical mass entering soil by deposition (kg)
							depchemovinmass[ichem][i][j][ilayer] = depchemovinmass[ichem][i][j][ilayer]
								+ depchemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass leaving soil by erosion (kg)
							erschemovoutmass[ichem][i][j][ilayer] = erschemovoutmass[ichem][i][j][ilayer]
								+ erschemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass leaving soil by porewater release (kg)
							pwrchemovoutmass[ichem][i][j][ilayer] = pwrchemovoutmass[ichem][i][j][ilayer]
								+ pwrchemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute the cumulative chemical mass leaving soil by infiltration (kg)
							infchemovoutmass[ichem][i][j][ilayer] = infchemovoutmass[ichem][i][j][ilayer]
								+ infchemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute the cumulative chemical mass entering soil by infiltration (kg)
							infchemovinmass[ichem][i][j][ilayer] = infchemovinmass[ichem][i][j][ilayer]
								+ infchemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass lost in soil by biodegradation (kg)
							biochemovoutmass[ichem][i][j][ilayer] = biochemovoutmass[ichem][i][j][ilayer]
								+ biochemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass gain in soil by biodegradation (kg)
							biochemovinmass[ichem][i][j][ilayer] = biochemovinmass[ichem][i][j][ilayer]
								+ biochemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass lost in soil by hydrolysis (kg)
							hydchemovoutmass[ichem][i][j][ilayer] = hydchemovoutmass[ichem][i][j][ilayer]
								+ hydchemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass gain in soil by hydrolysis (kg)
							hydchemovinmass[ichem][i][j][ilayer] = hydchemovinmass[ichem][i][j][ilayer]
								+ hydchemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass lost in soil by oxidation (kg)
							oxichemovoutmass[ichem][i][j][ilayer] = oxichemovoutmass[ichem][i][j][ilayer]
								+ oxichemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass gain in soil by oxidation (kg)
							oxichemovinmass[ichem][i][j][ilayer] = oxichemovinmass[ichem][i][j][ilayer]
								+ oxichemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass lost in soil by photolysis (kg)
							phtchemovoutmass[ichem][i][j][ilayer] = phtchemovoutmass[ichem][i][j][ilayer]
								+ phtchemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass gain in soil by photolysis (kg)
							phtchemovinmass[ichem][i][j][ilayer] = phtchemovinmass[ichem][i][j][ilayer]
								+ phtchemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass lost in soil by radioactive decay (kg)
							radchemovoutmass[ichem][i][j][ilayer] = radchemovoutmass[ichem][i][j][ilayer]
								+ radchemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass gain in soil by radioactive decay (kg)
							radchemovinmass[ichem][i][j][ilayer] = radchemovinmass[ichem][i][j][ilayer]
								+ radchemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass lost in soil by volatilization (kg)
							vltchemovoutmass[ichem][i][j][ilayer] = vltchemovoutmass[ichem][i][j][ilayer]
								+ vltchemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass gain in soil by volatilization (kg)
							vltchemovinmass[ichem][i][j][ilayer] = vltchemovinmass[ichem][i][j][ilayer]
								+ vltchemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass lost in soil by user-defined reaction (kg)
							udrchemovoutmass[ichem][i][j][ilayer] = udrchemovoutmass[ichem][i][j][ilayer]
								+ udrchemovoutflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute cumulative chemical mass gain in soil by user-defined reaction (kg)
							udrchemovinmass[ichem][i][j][ilayer] = udrchemovinmass[ichem][i][j][ilayer]
								+ udrchemovinflux[ichem][i][j][ilayer] * dt[idt] / 1000.0;

							//compute total derivative for the soil layer (g/s)
							soilderivative = soilinflux - soiloutflux;

							//compute the new mass (g) of chemical in the soil layer
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
								//compute new soil layer concentration (g/m3)
								cchemovnew[ichem][i][j][ilayer] = (float)(newsoilmass / newsoilvol);
							}
							else	//else newsoilmass <= TOLERANCE
							{
								//set the new concentration (g/m3) to zero
								cchemovnew[ichem][i][j][ilayer] = 0.0;

							}	//end if newsoilmass > TOLERANCE
						}
						else	//else newsoilvol < TOLERANCE
						{
							//set the new concentration to zero
							cchemovnew[ichem][i][j][ilayer] = 0.0;

						}	//end if newsoilvol >= TOLERANCE

					}	//end loop over chemicals

				}	//end if imask[][] != nodatavalue

			}	//end loop over layers

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
		//loop over number of chemical types
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Advection...
			//
			//Compute the cumulative chemical mass exiting the domain at this outlet (kg)
			totaladvchemoutov[ichem][i] = totaladvchemoutov[ichem][i]
				+ advchemovoutflux[ichem][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0;

			//Compute the cumulative chemical mass entering the domain at this outlet (kg)
			totaladvcheminov[ichem][i] = totaladvcheminov[ichem][i]
				+ advchemovinflux[ichem][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0;

			//Dispersion...
			//
			//Compute the cumulative chemical mass exiting the domain at this outlet (kg)
			totaldspchemoutov[ichem][i] = totaldspchemoutov[ichem][i]
				+ dspchemovoutflux[ichem][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0;

			//Compute the cumulative chemical mass entering the domain at this outlet (kg)
			totaldspcheminov[ichem][i] = totaldspcheminov[ichem][i]
				+ dspchemovinflux[ichem][iout[i]][jout[i]][10]
				* dt[idt] / 1000.0;

			//compute chemical discharge (kg/s) (advection plus dispersion)
			chemflow = (float)((advchemovoutflux[ichem][iout[i]][jout[i]][10]
				+ dspchemovoutflux[ichem][iout[i]][jout[i]][10]) / 1000.0);

			//Search for peak chemical discharge at this outlet...
			//
			//if the chemical discharge is greater than the peak value
			if(chemflow > chemflowpeakov[ichem][i])
			{
				//Save peak dischage at outlet (kg/s)
				chemflowpeakov[ichem][i] = chemflow;

				//Save time of peak flow (hours)
				chemtimepeakov[ichem][i] = (float)(simtime);

			}	//end if chemflow > chemflowpeakov[][]

		}	//end loop over chemicals

	}	//end loop over outlets

	//Loop over number of chemical reporting stations
	for (k=1; k<=nchemreports; k++)
	{
		//set row and column references for the chemical reporting station
		i = chemreprow[k];
		j = chemrepcol[k];

		//loop over number of chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//compute the sum of all particulate chemical fractions...
			//
			//compute sum of fp by difference from fd and fb
			sumfp = (float)(1.0 - fdissolvedov[ichem][i][j][0]
				- fboundov[ichem][i][j][0]);

			//store concentrations for reporting (g/m3)
			totchemreportov[ichem][k] = cchemov[ichem][i][j][0];
			dischemreportov[ichem][k] = cchemov[ichem][i][j][0]
				* fdissolvedov[ichem][i][j][0];
			bndchemreportov[ichem][k] = cchemov[ichem][i][j][0]
				* fboundov[ichem][i][j][0];
			prtchemreportov[ichem][k] = cchemov[ichem][i][j][0]
				* sumfp;

		}	//end loop over chemicals

	}	//end loop over reporting stations

//End of function: Return to ChemicalBalance
}
