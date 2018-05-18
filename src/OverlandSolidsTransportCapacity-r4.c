/*---------------------------------------------------------------------
C-  Function:   OverlandSolidsTransportCapacity.c
C-
C-  Purpose/    Determines the equilibrium sediment tranport capacity
C-  Methods:    for each cell in the overland plane
C-
C-
C-  Inputs:     spgravity[], dqovout[][][], hov[][], sfov[][][], ds[]
C-
C-  Outputs:    transcapov[][][]
C-
C-  Controls:   None
C-
C-  Calls:      None
C-
C-  Called by:  SolidsTransport
C-
C-	Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO 80523
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-  Date:       29-DEC-2003
C-
C-  Revised:    Mark Velleux
C-              HDR Engineering
C-              1 International Boulevard, 10th Floor, Suite 1000
C-              Mahwah, NJ 07495
C-
C-  Date:       28-SEP-2017
C-
C-  Revisions:  Replaced modified Kilinc-Richardson equation with generalized
C-              overland transport capcity: qs = k (q-qc) ^ beta * sf ^ gamma.
C-              Added weighting exponent on dstar to apportion total transport
C-              capacity into fraction for each solids class simulated.
C-
C-  Revised:
C-
C-  Date:
C-
C-  Revisions:
C-
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void OverlandSolidsTransportCapacity()
{
	//local variable declarations/definitions
	int
		isoil,			//local index for soil type
		iland,			//local index for land use classification
		isolid,			//local index for solids type
		ilayer;			//index for surface layer in the sediment stack

	float
		sf,				//friction slope (dimensionless)
		q,				//unit discharge (m2/s)
		qc,				//critical erosion threshold (unit discharge) (m2/s)
		qs,				//total (all solids) overland transport capacity (kg/m/s)
		ktotal,			//overall overland transport capacity coefficient (effectively kg/m/s)
		fsolids,		//fraction of a solids type in the soil bed (dimensionless)
		sumcapacity,	//sum of solids transport capacity fractions (dimensionless)
		*fcapacity;		//fraction of solids transport capacity (dimensionless)

	//allocate memory for local arrays
	fcapacity = (float *)malloc((nsolids+1) * sizeof(float));	//fraction of solids transport capacity (dimensionless)

	//Loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Loop over columns
		for(j=1; j<=ncols; j++)
		{
			//if the cell is in the domain (is not null)
			if(imask[i][j] != nodatavalue)
			{
				//set the surface soil layer number
				ilayer = nstackov[i][j];

				//assign soil type for this cell
				isoil = soiltype[i][j][ilayer];

				//assign land use type for this cell
				iland = landuse[i][j];

				//See note regarding K-R equation (below)...
				//
				//assign critical erosion threshold for this cell
				qc = vcov[isoil] * hov[i][j];

				//loop over solids (include zero for total solids)
				for(isolid=0; isolid<=nsolids; isolid++)
				{
					//initialize the total transport capacity (g/s)
					transcapov[isolid][i][j] = 0.0;

				}	//end loop over solids

				//loop over directions
				for(k=1; k<=8; k++)
				{
					//set friction slope (dimensionless)
					sf = (float)(fabs(sfov[i][j][k]));

					//set unit discharge (m2/s)
					q = dqovout[i][j][k] / w;

					//Note:  The modified Kilinc-Richardson (K-R)
					//       transport capacity is used...
					//
					//       In raw form, the modified K-R equation is
					//       (see Julien, E&S 1998, p. 224, Eq. 11.21a):
					//
					//       qs = 25500 sf^1.66 q^2.035 (K/0.15) C P
					//
					//       where qs is in tons/m/s (1 ton = 2000 lb)
					//       (1 ton = 907.185 kg).  Rearranging and
					//       converting units to kg/m/s, the equation
					//       becomes:
					//
					//       qs = 1.542 x 10^8 sf^1.66 q^2.035 K C P
					//
					//       The equation describes the total transport
					//       capacity of all solids types present.
					//
					//       The K-R equation is further modified by a
					//       a critical motion threshold.  A critical
					//       unit flow (qc) is computed from a critical
					//       velocity for erosion and the flow depth as:
					//
					//       qc = vc * h
					//
					//       For q <= qc, qs is zero.  For q > qc, the
					//       K-R equation becomes:
					//
					//       qs = 1.542 x 10^8 sf^1.66 (q-qc)^2.035 K C P
					//
					//if the unit flow exceeds the critical threshold
					if(q > qc)
					{
						//K-R Equation:
						//
						//total (all solids) overland transport capacity (kg/m/s)
						//qs = (float)(1.542e8 * pow(sf, 1.66)
						//	* pow((q-qc), 2.035) * kusle[isoil]
						//	* cusle[iland] * pusle[iland]);

						//Generalized form for overland transport capacity:
						//
						//total (all solids) overland transport capacity (kg/m/s)
						//qs = (float)(ktotal * pow((q-qc), 2.035)* pow(sf, 1.66);

						//set total transport capcity coefficient for this soil and land use
						ktotal = ktranscapov[iland] * kusle[isoil]
							* cusle[iland] * pusle[iland];

						//total (all solids) overland transport capacity (kg/m/s)
						qs = (float)(ktotal * pow((q-qc), betasov[iland])
							* pow(sf, gammasov[iland]));
					}
					else	//else q <= qc
					{
						//set the total overland transport capacity to zero
						qs = 0.0;

					}	//end if q > qc

					//Compute the sum over all directions to get a total...
					//
					//total transport capacity (summed over all directions) (g/s)
					transcapov[0][i][j] = transcapov[0][i][j]
						+ (float)(qs * w * 1000.0);

				}	//end loop over directions

				//Apportion the total solids transport capacity by
				//solids type in the surface sediment layer
				//
				//Note:  In CASC2D, the overland transport capacity was
				//       apportioned by the abundence of solids in the
				//       water column.  In TREX, the bed solids grain
				//       size fraction and dimensionless diameter (d*)
				//       are used so that large grains have smaller
				//       transport capacities than small grains.
				//
				//compute transport fraction...
				//
				//initialize sum of transport capacity fractions
				sumcapacity = 0.0;

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Note:  csedov[0][][][] is the total solids concentration
					//       (sum of all solids types) for this cell/layer.
					//
					//if the total bed solids concentration > zero
					if(csedov[0][i][j][ilayer] > 0.0)
					{
						//compute the fraction of this solids type in the bed (soil layer)
						fsolids = csedov[isolid][i][j][ilayer] / csedov[0][i][j][ilayer];
					}
					else	//else csedov[0][i][j][ilayer] = 0
					{
						//set the fraction to zero
						fsolids = 0.0;

					}	//end if csedov[0][i][j][ilayer] > 0

					//Note:  dstar[] is always greater than zero...
					//
					//compute the transport capacity fraction for this solids type (numerator)
					fcapacity[isolid] = (float)(fsolids / pow(dstar[isolid], tcwexpov[iland]));

					//compute the sum of transport fractions (denominator)
					sumcapacity = sumcapacity + fcapacity[isolid];

				}	//end loop over solids

				//loop over solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//if the total bed solids concentration > zero
					if(csedov[0][i][j][ilayer] > 0.0)
					{
						//compute the fraction of this solids type in the bed (soil layer)
						fcapacity[isolid] = fcapacity[isolid] / sumcapacity;
					}
					else	//else csedov[0][i][j][ilayer] = 0
					{
						//set the fraction to zero
						fcapacity[isolid] = 0.0;

					}	//end if csedov[0][i][j][ilayer] > 0

					//compute apportioned transport capacity (g/s)
					transcapov[isolid][i][j] = fcapacity[isolid]
						* transcapov[0][i][j];

				}	//end loop over solids

			}	//end if imask[][] != nodatavalue

		}	//end loop over columns

	}	//end loop over rows

	//deallocate memory for local arrays
	free(fcapacity);

//End of function: Return to SolidsTransport
}
