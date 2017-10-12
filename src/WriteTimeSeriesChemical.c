/*---------------------------------------------------------------------
C-  Function:	WriteTimeSeriesChemical.c                                              
C-
C-	Purpose/    The module writes times series reports of chemical export
C-	Methods:    (in g/m3 or kg/interval) at each specified location
C-              (reporting stations) for the current time t.
C-
C-	Inputs:		chemreport
C-
C-	Outputs:	
C-
C-	Controls:   nchemreports, chemconvert
C-
C-	Calls:		None
C-
C-	Called by:	WriteTimeSeries
C-
C-  Created:    Mark Velleux (CSU)
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-	Date:		20-MAR-2004
C-
C-  Revised:	Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:		31-JUL-2009
C-
C-  Revisions:	Added code so that errors caused by bad file names
C-              and paths are are reported to the simulation error
C-              file in addition to the screen.
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C--------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void WriteTimeSeriesChemical()
{
	//declare local variables
	int
		igroup,		//local counter for reporting group number
		ichem,		//local counter for chemical type
		replink,	//link of a reporting in a channel cell
		repnode;	//node of a reporting in a channel cell

	float
		**totcgroupsumov,	//reporting group sum for total chemical in the overland plane [igroup][ireport]
		**discgroupsumov,	//reporting group sum for dissolved chemical in the overland plane [igroup][ireport]
		**bndcgroupsumov,	//reporting group sum for bound chemical in the overland plane [igroup][ireport]
		**prtcgroupsumov,	//reporting group sum for particulate chemical in the overland plane [igroup][ireport]
		**totcgroupsumch,	//reporting group sum for total chemical in the channel network [igroup][ireport]
		**discgroupsumch,	//reporting group sum for dissolved chemical in the channel network [igroup][ireport]
		**bndcgroupsumch,	//reporting group sum for bound chemical in the channel network [igroup][ireport]
		**prtcgroupsumch,	//reporting group sum for particulate chemical in the channel network [igroup][ireport]
		totcgroupsumtot,	//reporting group sum for total chemical overland and in channels
		discgroupsumtot,	//reporting group sum for dissolved chemical overland and in channels
		bndcgroupsumtot,	//reporting group sum for bound chemical overland and in channels
		prtcgroupsumtot,	//reporting group sum for particulate chemical overland and in channels
		dqsum,				//sum of gross outflows (m3/s)
		dischargeov,		//overland flow as needed for units conversions (m3/s)
		dischargech;		//channel flow as needed for units conversions (m3/s)

	//If user requested reports for chemical discharges
	if(nchemreports > 0)
	{
		//Initialize group sums...
		//
		//Allocate memory
		totcgroupsumov = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for total chemical in the overland plane [igroup][ireport]
		discgroupsumov = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for dissolved chemical in the overland plane [igroup][ireport]
		bndcgroupsumov = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for bound chemical in the overland plane [igroup][ireport]
		prtcgroupsumov = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for particulate chemical in the overland plane [igroup][ireport]
		totcgroupsumch = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for total chemical in the channel network [igroup][ireport]
		discgroupsumch = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for dissolved chemical in the channel network [igroup][ireport]
		bndcgroupsumch = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for bound chemical in the channel network [igroup][ireport]
		prtcgroupsumch = (float **)malloc((ncgroups+1) * sizeof(float *));		//reporting group sum for particulate chemical in the channel network [igroup][ireport]

		//loop over chemical groups
		for(igroup=1; igroup<=ncgroups; igroup++)
		{
			//Allocate remaining memory
			totcgroupsumov[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for total chemical in the overland plane [igroup][ireport]
			discgroupsumov[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for dissolved chemical in the overland plane [igroup][ireport]
			bndcgroupsumov[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for bound chemical in the overland plane [igroup][ireport]
			prtcgroupsumov[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for particulate chemical in the overland plane [igroup][ireport]
			totcgroupsumch[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for total chemical in the channel network [igroup][ireport]
			discgroupsumch[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for dissolved chemical in the channel network [igroup][ireport]
			bndcgroupsumch[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for bound chemical in the channel network [igroup][ireport]
			prtcgroupsumch[igroup] = (float *)malloc((nchemreports+1) * sizeof(float));		//reporting group sum for particulate chemical in the channel network [igroup][ireport]

			//loop over reporting stations
			for(j=1; j<=nchemreports; j++)
			{
				//set group sum values to zero
				totcgroupsumov[igroup][j] = 0.0;
				discgroupsumov[igroup][j] = 0.0;
				bndcgroupsumov[igroup][j] = 0.0;
				prtcgroupsumov[igroup][j] = 0.0;
				totcgroupsumch[igroup][j] = 0.0;
				discgroupsumch[igroup][j] = 0.0;
				bndcgroupsumch[igroup][j] = 0.0;
				prtcgroupsumch[igroup][j] = 0.0;

			}	//end loop over reporting stations

		}	//end loop over chemical groups

		//Compute reporting groups sums...
		//
		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{ 
			//set the group number
			igroup = cgroupnumber[ichem];

			//loop over reporting stations
			for(j=1; j<=nchemreports; j++)
			{
				//Overland: total, dissolved, bound, and particulate chemical
				//
				//add the concentration of each chemicals type to its reporting group
				totcgroupsumov[igroup][j] = totcgroupsumov[igroup][j]
					+ totchemreportov[ichem][j];

				//add the concentration of each chemicals type to its reporting group
				discgroupsumov[igroup][j] = discgroupsumov[igroup][j]
					+ dischemreportov[ichem][j];

				//add the concentration of each chemicals type to its reporting group
				bndcgroupsumov[igroup][j] = bndcgroupsumov[igroup][j]
					+ bndchemreportov[ichem][j];

				//add the concentration of each chemicals type to its reporting group
				prtcgroupsumov[igroup][j] = prtcgroupsumov[igroup][j]
					+ prtchemreportov[ichem][j];

				//Channel: total, dissolved, bound, and particulate chemical
				//
				//add the concentration of each chemicals type to its reporting group
				totcgroupsumch[igroup][j] = totcgroupsumch[igroup][j]
					+ totchemreportch[ichem][j];

				//add the concentration of each chemicals type to its reporting group
				discgroupsumch[igroup][j] = discgroupsumch[igroup][j]
					+ dischemreportch[ichem][j];

				//add the concentration of each chemicals type to its reporting group
				bndcgroupsumch[igroup][j] = bndcgroupsumch[igroup][j]
					+ bndchemreportch[ichem][j];

				//add the concentration of each chemicals type to its reporting group
				prtcgroupsumch[igroup][j] = prtcgroupsumch[igroup][j]
					+ prtchemreportch[ichem][j];

			}	//end loop over reporting stations

		}	//end loop over chemicals

		//Write chemical discharges at selected locations
		//and print out in comma-delimited format
		//
		//loop over chemical groups
		for(igroup=1; igroup<=ncgroups; igroup++)
		{
			//Set the chemical export file pointer to null
			chemexpfile_fp = NULL;

			//open the chemical export file for writing in append mode
			chemexpfile_fp = fopen(chemexpfile[igroup], "a");

			//if the chemical export file pointer is null, abort execution
			if(chemexpfile_fp == NULL)
			{
				//Write message to file
				fprintf(echofile_fp, "Error! Can't open Chemical Export File:  %s\n", chemexpfile[igroup]);

				//Write message to screen
				printf("Error! Can't open Chemical Export File:  %s\n", chemexpfile[igroup]);
				exit(EXIT_FAILURE);	//abort
			}

			//Write current simulation time (hours)
			fprintf(chemexpfile_fp,"%f",simtime);

			//Loop over number of chemical reporting stations
			for(j=1; j<=nchemreports; j++)
			{
				//if the chemical units reporting option = 2
				if(chemunitsopt[j] == 2)
				{
					//if the cell of this chemical reporting station is a channel cell
					if(imask[chemreprow[j]][chemrepcol[j]] > 1)
					{
						//get link and node for this reporting station
						replink = link[chemreprow[j]][chemrepcol[j]];
						repnode = node[chemreprow[j]][chemrepcol[j]];
						
						//compute sum of all gross channel outflows...
						//
						//initialize sum of outflows to zero
						dqsum = 0.0;

						//loop over directions (1-8)
						for(k=1; k<=8; k++)
						{
							//compute sum of outflows (m3/s)
							dqsum = dqsum + dqchout[replink][repnode][k];

						}	//end loop over outflows

						//add flow out domain boundary (k=10)
						dqsum = dqsum + dqchout[replink][repnode][10];

						//channel discharge is product of channel outflow and scale factor
						dischargech = dqsum * advchscale;
					}
					else	//cell is overland cell (no channel)
					{
						//channel discharge is zero
						dischargech = 0.0;

					}	//end if imask[][] > 1

					//compute sum of all gross overland outflows...
					//
					//initialize sum of outflows to zero
					dqsum = 0.0;

					//loop over directions (1-8)
					for(k=1; k<=8; k++)
					{
						//compute sum of outflows (m3/s)
						dqsum = dqsum + dqovout[sedreprow[j]][sedrepcol[j]][k];

					}	//end loop over outflows

					//add flow out domain boundary (k=10)
					dqsum = dqsum + dqovout[sedreprow[j]][sedrepcol[j]][10];

					//overland discharge is product of overland outflow and scale factor
					dischargeov = dqsum * advovscale;

					//Report output in kg/day...
					//
					//multiply output export by conversion factor
					//and discharge and print to file
					//
					//Note:  The discharge sum only makes sense when the reporting
					//       station is at an outlet and the reporting unit is kg/day
					//
					//total chemical overland
					fprintf(chemexpfile_fp,",%e",
						totcgroupsumov[igroup][j] * dischargeov * chemconvert[j]);

					//total chemical channels
					fprintf(chemexpfile_fp,",%e",
						totchemreportch[igroup][j] * dischargech * chemconvert[j]);

					//sum overland and channel discharges
					totcgroupsumtot = (totcgroupsumov[igroup][j] * dischargeov
						+ totcgroupsumch[igroup][j] * dischargech);

					//total chemical overland and channel sum
					fprintf(chemexpfile_fp,",%e",
						totcgroupsumtot * chemconvert[j]);

					//dissolved chemical overland
					fprintf(chemexpfile_fp,",%e",
						discgroupsumov[igroup][j] * dischargeov * chemconvert[j]);

					//dissolved chemical channels
					fprintf(chemexpfile_fp,",%e",
						discgroupsumch[igroup][j] * dischargech * chemconvert[j]);

					//sum overland and channel discharges
					discgroupsumtot = (discgroupsumov[igroup][j] * dischargeov
						+ discgroupsumch[igroup][j] * dischargech);

					//dissolved chemical overland and channel sum
					fprintf(chemexpfile_fp,",%e",
						discgroupsumtot * chemconvert[j]);

					//bound chemical overland
					fprintf(chemexpfile_fp,",%e",
						bndcgroupsumov[igroup][j] * dischargeov * chemconvert[j]);

					//bound chemical channels
					fprintf(chemexpfile_fp,",%e",
						bndcgroupsumch[igroup][j] * dischargech * chemconvert[j]);

					//sum overland and channel discharges
					bndcgroupsumtot = (bndcgroupsumov[igroup][j] * dischargeov
						+ bndcgroupsumch[igroup][j] * dischargech);

					//bound chemical overland and channel sum
					fprintf(chemexpfile_fp,",%e",
						bndcgroupsumtot * chemconvert[j]);

					//particulate chemical overland
					fprintf(chemexpfile_fp,",%e",
						prtcgroupsumov[igroup][j] * dischargeov * chemconvert[j]);

					//particulate chemical channels
					fprintf(chemexpfile_fp,",%e",
						prtcgroupsumch[igroup][j] * dischargech * chemconvert[j]);

					//sum overland and channel discharges
					prtcgroupsumtot = (prtcgroupsumov[igroup][j] * dischargeov
						+ prtcgroupsumch[igroup][j] * dischargech);

					//particulate chemical overland and channel sum
					fprintf(chemexpfile_fp,",%e",
						prtcgroupsumtot * chemconvert[j]);
				}
				else	//chemical units reporting option != 2 (=1)
				{
					//Report output in g/m3 (mg/L)
					//
					//total chemical overland
					fprintf(chemexpfile_fp,",%e",
						totcgroupsumov[igroup][j] * chemconvert[j]);

					//total chemical channels
					fprintf(chemexpfile_fp,",%e",
						totcgroupsumch[igroup][j] * chemconvert[j]);

					//dissolved chemical overland
					fprintf(chemexpfile_fp,",%e",
						discgroupsumov[igroup][j] * chemconvert[j]);

					//dissolved chemical channels
					fprintf(chemexpfile_fp,",%e",
						discgroupsumch[igroup][j] * chemconvert[j]);

					//bound chemical overland
					fprintf(chemexpfile_fp,",%e",
						bndcgroupsumov[igroup][j] * chemconvert[j]);

					//bound chemical channels
					fprintf(chemexpfile_fp,",%e",
						bndcgroupsumch[igroup][j] * chemconvert[j]);

					//particulate chemical overland
					fprintf(chemexpfile_fp,",%e",
						prtcgroupsumov[igroup][j] * chemconvert[j]);

					//particulate chemical channels
					fprintf(chemexpfile_fp,",%e",
						prtcgroupsumch[igroup][j] * chemconvert[j]);

				}	//end if chemunitsopt[] == 2

			}	//end loop over	number of chemical reporting stations

			//print a carriage return after each complete line
			fprintf(chemexpfile_fp,"\n");

			//Close chemical export file
			fclose(chemexpfile_fp);

		}	//end loop over number of chemicals reporting groups

	}	//end if nsedports > 0

	//free memory for local arrays...
	//
	//loop over reporting groups
	for(igroup=1; igroup<=ncgroups; igroup++)
	{
		//free memory
		free(totcgroupsumov[igroup]);
		free(discgroupsumov[igroup]);
		free(bndcgroupsumov[igroup]);
		free(prtcgroupsumov[igroup]);
		free(totcgroupsumch[igroup]);
		free(discgroupsumch[igroup]);
		free(bndcgroupsumch[igroup]);
		free(prtcgroupsumch[igroup]);

	}	//end loop over reporting groups

	//free remaining memory
	free(totcgroupsumov);
	free(discgroupsumov);
	free(bndcgroupsumov);
	free(prtcgroupsumov);
	free(totcgroupsumch);
	free(discgroupsumch);
	free(bndcgroupsumch);
	free(prtcgroupsumch);

//End of function: Return to WriteTimeSeries
}
