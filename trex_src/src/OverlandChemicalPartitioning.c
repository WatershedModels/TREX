/*---------------------------------------------------------------------
C-  Function:	OverlandChemicalPartitioning.c
C-
C-	Purpose/	Compute the partitioning of chemicals in the overland
C-	Methods:	plane.  Partitioning determines all chemical phases:
C-              dissolved, bound, and particulate.
C-
C-
C-  Inputs:	    csedov[][][], cdocov[][][], fpocov[][][][],
C-              fdocov[][][], kb[], kp[], koc[]
C-
C-  Outputs:    fparticulateov[chem][solid][link][node][layer]
C-              fboundov[chem][link][node][layer],
C-              fdissolvedov[chem][link][node][layer]
C-
C-  Controls:	partopt[]
C-
C-  Calls:      None
C-
C-  Called by:  ChemicalTransport
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
C-	Date:		12-JUL-2004
C-
C-	Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       13-JUN-2008
C-
C-	Revisions:  Revised calculation of phase distribution for sediment
C-              to account for variable saturation of soil layers as
C-              the infiltration wetting front moves through the soil
C-              stack.  Updated comments for water column calculations.
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

void OverlandChemicalPartitioning()
{
	//local variable declarations/definitions
	int
		ichem,		//loop index for chemical type
		isolid,		//loop index for solids (particle) type
		ilayer,		//loop index for layer in the soil stack
		isoil;		//index for soil type [i][j][ilayer]

	float
		pic0,		//solids independent partition coefficient (m3/g)
		*pic,		//solids dependent/independent partition coefficient (m3/g) [isolid]
		fpoc,		//fraction particulate organic carbon (dimensionless)
		*m,			//solids partitioning basis (solids or poc concentration) (g/m3 = mg/L)
		sumpicm,	//sum of pic * m values
		sumfp,		//overal sum (fraction of total) of chemical sorbed to solids (dimensionless)
		fdoc,		//fraction dissolved organic carbon (dimensionless) (effective fration doc)
		pib,		//dissolved organic compound binding coefficient (m3/g) (effective binding coef.)
		b;			//doc binding basis (doc concentration) (g/m3)

	float
		densityw,	//density of water (kg/m3)
		densityp,	//particle density (kg/m3)
		fsolids,	//volume fraction of solids (volume solids / total volume)
		wcporosity,	//water column porosity (volume water / total volume) (no air space)
		wfe,		//wetting front elevation (m)
		fsaturated,	//fraction of soil pore space that is filled with water (volume water / total volume) (could be air space)
		phi;		//soil layer water-filled porosity (volume water / total volume) (when fsaturated = 1, phi = porosityov)

	//Allocate memory for local arrays
	pic = (float *)malloc((nsolids+1) * sizeof(float));		//partition coefficient (m3/g)
	m = (float *)malloc((nsolids+1) * sizeof(float));		//solids partitioning basis (g/m3 = mg/L)

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
				//Calculate water column porosity...
				//
				//initialize the volumefraction of solids to zero
				fsolids = 0.0;

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//set particle density (kg/m3)
					densityp = spgravity[isolid] * densityw;

					//compute volume fraction of solids (sum for all particle types) (dimensionless)
					fsolids = fsolids + csedov[isolid][i][j][0] / (float)(densityp * 1000.0);

				}	//end loop over solids

				//Note:  This calculation is based on the assumption that the water column
				//       never reaches the point where void space filled with air is ever
				//       part of the water column.  At least in concept, such a condition
				//       could occur as the depth of the water column goes to zero and the
				//       cell dries...
				//
				//compute water column porosity (volume water / total volume) (dimensionless)
				wcporosity = (float)(1.0 - fsolids);

				//loop over number of chemical types
				for(ichem=1; ichem<=nchems; ichem++)
				{
					//if the chemical partitions (partopt > 0)
					if(partopt[ichem] > 0)
					{
						//Water Column...
						//
						//Particulate phases:
						//
						//initialize the sum of pic * m (used to compute fparticulate)
						sumpicm = 0.0;

						//initialize the sum of all particulate fractions
						sumfp = 0.0;

						//loop over number of solids types
						for(isolid=1; isolid<=nsolids; isolid++)
						{
							//Note:  Partitioning is generalized for organic or
							//       inorganic chemicals.  Organics partition
							//       on a cabon normalized basis.  Inorganics
							//       partition on a solids basis.  This code
							//       works for either case and also accounts
							//       for particle dependent partitioning.
							//
							//if the chemical partitions to organic carbon
							if(partopt[ichem]> 1)
							{
								//set the fraction (particulate) organic carbon
								fpoc = fpocov[isolid][i][j][0];

								//set the solids independent partition coefficient (m3/g)
								pic0 = koc[ichem];

								//set the particulate organic carbon (poc) concentration (g/m3 = mg/L))
								m[isolid] = csedov[isolid][i][j][0] * fpoc;
							}
							else	//else the chemical partitions to solids
							{
								//Developer's Note:  The water column partition coefficient
								//                   could be assigned as a separate variable
								//                   to distinguish it from the sediment bed
								//                   partition coefficient...
								//
								//                   Change from kp[] to kpw[] and kps[]...
								//
								//set the solids independent partition coefficient (m3/g)
								pic0 = kp[ichem];

								//set the solids concentration (g/m3 = mg/L)
								m[isolid] =  csedov[isolid][i][j][0];

							}	//end if partopt[]

							//Adjust partition coefficient for porosity
							//
							//Note:  For the water column, the partition coefficient must
							//       be adjusted by porosity because "m" is on a total
							//       (bulk) volume basis (i.e. volume water plus volume
							//       solids) and the partition coefficient is defined as
							//       mass chemical / volume water.
							//
							//if the water column porosity > 0.0
							if(wcporosity > 0.0)
							{
								//correct partition coeffificent for porosity
								pic0 = pic0 / wcporosity;
							}
							else	//else wcporosity <= 0
							{
								//set the partition coefficient to effective infinity
								pic0 = (float)(1.0e30);

							}	//end if wcporosity > 0.0

							//Note:  For the water column, the particle dependent
							//       partition coefficient is used...
							//
							//Compute the particle dependent partition coefficient (m3/g)
							pic[isolid] = (float)(pic0 / (1.0 + m[isolid] * pic0 /nux[ichem]));

							//compute the sum of pic * m (used to compute fparticulate)
							sumpicm = sumpicm + pic[isolid] * m[isolid];

						}	//end loop over solids

						//Bound phase:
						//
						//if the chemical partitions to organic carbon
						if(partopt[ichem]> 1)
						{
							//set the effective fraction of doc for binding
							fdoc = fdocov[i][j][0];

							//set the organic binding coefficient (m3/g)
							pib = koc[ichem];

							//set the dissolved organic compound (doc) concentration (g/m3 = mg/L)
							b = cdocov[i][j][0] * fdoc;

						}
						else	//else the chemical partitions to solids
						{
							//set the organic binding coefficient (m3/g)
							pib = kb[ichem];

							//set the dissolved organic compound (doc) concentration (g/m3 = mg/L)
							b =  cdocov[i][j][0];

						}	//end if partopt[] > 1

						//Adjust binding coefficient for porosity
						//
						//if the water column porosity > 0.0
						if(wcporosity > 0.0)
						{
							//Note:  For the water column, the binding coefficient must
							//       be adjusted by porosity because "b" is on a total
							//       (bulk) volume basis (i.e. volume water plus volume
							//       solids) and the binding coefficient is defined as
							//       mass chemical / volume water.
							//   
							//correct binding coeffificent for porosity
							pib = pib / wcporosity;
						}
						else	//else wcporosity <= 0
						{
							//set the binding coefficient to zero
							pib = 0.0;

						}	//end if wcporosity > 0.0

						//loop over number of solids types
						for(isolid=1; isolid<=nsolids; isolid++)
						{
							//compute the fraction of the chemical sorbed to this solids type
							fparticulateov[ichem][isolid][i][j][0] = (float)(pic[isolid] * m[isolid]
								/ (1.0 + pib * b + sumpicm));

							//compute sum of all particulate phases
							sumfp = sumfp
								+ fparticulateov[ichem][isolid][i][j][0];

						}	//end loop over solids

						//Note:  Watch out for roundoff errors.  For large
						//       Kp values, the sum of particulate fractions
						//       can be > 1.0 (i.e. 1.0000001).  When this
						//       occurs individual values need to be scaled
						//       and the sum reset to exactly 1.0.
						//
						//if the sum of particulate fractions (sumfp) > 1.0
						if(sumfp > 1.0)
						{
							//loop over number of solids types
							for(isolid=1; isolid<=nsolids; isolid++)
							{
								//scale the fraction of the chemical sorbed to this solids type
								fparticulateov[ichem][isolid][i][j][0] = (float)(1.0 / sumfp)
									* fparticulateov[ichem][isolid][i][j][0];

							}	//end loop over solids

							//reset sum of all particulate phases to exactly 1.0
							sumfp = (float)(1.0);

						}	//end if sumfp > 1.0

						//compute the fraction of the chemical sorbed to doc
						fboundov[ichem][i][j][0] = (float)(pib * b / (1.0 + pib * b + sumpicm));

						//compute the dissolved fraction of the chemical
						fdissolvedov[ichem][i][j][0] = (float)(1.0 - sumfp
							- fboundov[ichem][i][j][0]);

						//Soil Column...
						//
						//loop over layers (in reverse order: top down)
						for(ilayer=nstackov[i][j]; ilayer>=1; ilayer--)
						{
							//assign soil type of this cell
							isoil = soiltype[i][j][ilayer];

							//Particulate phases:
							//
							//initialize the sum of pic * m (used to compute fparticulate)
							sumpicm = 0.0;

							//initialize the sum of all particulate fractions
							sumfp = 0.0;

							//Compute water-filled porosity of bed (volume water / total volume)
							//
							//       phi = fsaturated * porositych[][]
							//
							//Note:  The pore space of the soil stack may not always
							//       be 100% saturated with water and may also include
							//       void space willed with air...
							//
							//       total volume = volume of air + water + solids
							//
							//if infiltration is simulated (infopt > 0)
							if(infopt > 0)
							{
								//compute the wetting front elevation (m)
								wfe = elevationov0[i][j] - infiltrationdepth[i][j];

								//Determine if the wetting front is within this soil layer
								//
								//Three cases exist:
								//
								//   (1) wetting front is above top of this layer, so the
								//       water filled porosity equals the layer initial
								//       value (fsaturated = 1 - sedmd)
								//
								//   (2) wetting front is below bottom of this layer, so
								//       the water filled porosity equals the layer
								//       porosity (fsaturated = 1) (100% saturated)
								//
								//   (3) wetting from is within this layer, so the water
								//       filled porosity is computed as an average where
								//       the distance above the front is saturated and
								//       the distance below is the initial value
								//
								//Note:  elevlayerov[][][ilayer] = elevation at layer top
								//       elevlayerov[][][ilayer-1] = elevation at layer bottom
								//       elevlayerov[][][0] = elevation at bottom of stack
								//
								//if the wetting from elevation is above the layer upper boundary
								if(wfe >= elevlayerov[i][j][ilayer])
								{
									//fraction saturated equals layer initial value
									fsaturated = (float)(1.0 - soilmd[isoil]);
								}
								//else, if the wetting front is below the layer bottom boundary
								else if(wfe <= elevlayerov[i][j][ilayer-1])
								{
									//fraction saturated equals 1.0 (100% saturated)
									fsaturated = 1.0;
								}
								//else, the wetting front is within this layer
								else
								{
									//calculate fraction saturation of the layer
									fsaturated = (float)(((elevlayerov[i][j][ilayer]- wfe) * 1.0
										+ (wfe - elevlayerov[i][j][ilayer-1]) * (1.0 - soilmd[isoil]))
										/ (elevlayerov[i][j][ilayer] - elevlayerov[i][j][ilayer-1]));

								}	//end if/else if/else wfe >= elevlayerov[][][]...
							}
							else	//infiltration is not simulated (infopt = 0)
							{
								//set fraction saturated equals to zero
								fsaturated = 0.0;

							}	//end if infopt > 0

							//calculate water filled porosity (volume water / total volume)
							phi = fsaturated * porosityov[isoil];

							//loop over number of solids types
							for(isolid=1; isolid<=nsolids; isolid++)
							{
								//Note:  Partitioning is generalized for organic or
								//       inorganic chemicals.  Organics partition
								//       on a cabon normalized basis.  Inorganics
								//       partition on a solids basis.  This code
								//       works for either case.  Partitioning in
								//       the sediments is always independent
								//       of particle concentration (i.e. never use
								//       DiToro particle interaction model).
								//
								//if the chemical partitions to organic carbon
								if(partopt[ichem]> 1)
								{
									//set the fraction (particulate) organic carbon
									fpoc = fpocov[isolid][i][j][ilayer];

									//set the solids independent partition coefficient (m3/g)
									pic0 = koc[ichem];

									//set the particulate organic carbon (poc) concentration (g/m3 = mg/L))
									m[isolid] = csedov[isolid][i][j][ilayer] * fpoc;
								}
								else	//else the chemical partitions to solids
								{
									//set the solids independent partition coefficient (m3/g)
									pic0 = kp[ichem];

									//set the solids concentration (g/m3 = mg/L)
									m[isolid] =  csedov[isolid][i][j][ilayer];

								}	//end if partopt[]

								//Adjust partition coefficient for porosity
								//
								//if the soil layer porosity > 0.0
								if(phi > 0.0)
								{
									//correct partition coeffificent for porosity
									pic0 = pic0 / phi;
								}
								else	//else phi <= 0 (phi can never really be < 0...)
								{
									//set the partition coefficient to effective infinity
									pic0 = (float)(1.0e30);

								}	//end if porosityov[isoil] > 0.0

								//Note:  For the sediment column, the particle independent
								//       partition coefficient is used (never use DiToro
								//       particle interaction model)...
								//
								//Compute the (particle independent) partition coefficient (m3/g)
								pic[isolid] = pic0;

								//compute the sum of pic * m (used to compute fparticulate)
								sumpicm = sumpicm + pic[isolid] * m[isolid];

							}	//end loop over solids

							//Bound phase:
							//
							//if the chemical partitions to organic carbon
							if(partopt[ichem]> 1)
							{
								//set the effective fraction of doc for binding
								fdoc = fdocov[i][j][ilayer];

								//set the organic binding coefficient (m3/g)
								pib = koc[ichem];

								//set the effective dissolved organic compound (doc) concentration (g/m3 = mg/L)
								b = cdocov[i][j][ilayer] * fdoc;
							}
							else	//else the chemical partitions to solids
							{
								//set the organic binding coefficient (m3/g)
								pib = kb[ichem];

								//set the dissolved organic compound (doc) concentration (g/m3 = mg/L)
								b =  cdocov[i][j][ilayer];

							}	//end if partopt[] > 1

							//Adjust binding coefficient for water-filled porosity
							//
							//Note:  In soilss, DOC concentrations are reported on
							//       a pore volume basis: mass DOC / volume water.
							//       The binding coefficient must be adjusted for the
							//       fraction of the pore space that is filled with
							//       water because the soil may not be 100% saturated.
							//
							//if the water-filled porosity (phi) > 0.0
							if(phi > 0.0)
							{
								//correct binding coeffificent for saturation
								pib = pib / phi;
							}
							else	//else saturation <= 0
							{
								//set the binding coefficient to zero
								pib = 0.0;

							}	//end if phi > 0.0

							//loop over number of solids types
							for(isolid=1; isolid<=nsolids; isolid++)
							{
								//compute the fraction of the chemical sorbed to this solids type
								fparticulateov[ichem][isolid][i][j][ilayer] = (float)(pic[isolid] * m[isolid]
									/ (phi + pib * b + sumpicm));

								//compute sum of all particulate phases
								sumfp = sumfp
									+ fparticulateov[ichem][isolid][i][j][ilayer];

							}	//end loop over solids

							//Note:  Watch out for roundoff errors.  For large
							//       Kp values, the sum of particulate fractions
							//       can be > 1.0 (i.e. 1.0000001).  When this
							//       occurs individual values need to be scaled
							//       and the sum reset to exactly 1.0.
							//
							//if the sum of particulate fractions (sumfp) > 1.0
							if(sumfp > 1.0)
							{
								//loop over number of solids types
								for(isolid=1; isolid<=nsolids; isolid++)
								{
									//scale the fraction of the chemical sorbed to this solids type
									fparticulateov[ichem][isolid][i][j][ilayer] = (float)(1.0 / sumfp)
										* fparticulateov[ichem][isolid][i][j][ilayer];

								}	//end loop over solids

								//reset sum of all particulate phases to exactly 1.0
								sumfp = (float)(1.0);

							}	//end if sumfp > 1.0

							//compute the fraction of the chemical sorbed to doc
							fboundov[ichem][i][j][ilayer] = (float)(pib * b / (phi + pib * b + sumpicm));

							//compute the dissolved fraction of the chemical
							fdissolvedov[ichem][i][j][ilayer] = (float)(1.0 - sumfp
								- fboundov[ichem][i][j][ilayer]);

						}	//end loop over layers

					}	//end if partopt[] > 0

				}	//end loop over number of chemical types

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//Deallocate memory for local arrays
	free(pic);
	free(m);

//End of function: Return to ChemicalTransport
}
