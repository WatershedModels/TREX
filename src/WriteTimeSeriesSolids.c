/*---------------------------------------------------------------------
C-  Function:	WriteTimeSeriesSolids.c                                              
C-
C-	Purpose/    The module writes times series reports of sediment export
C-	Methods:    (in g/m3 or MT/day) at each specified location
C-              (reporting stations) for the current time t.
C-
C-	Inputs:		sedreportov[][], sedreportch[][], sedconvert[]
C-
C-	Outputs:	
C-
C-	Controls:   nsedreports, sedunitsopt[]
C-
C-	Calls:		None
C-
C-	Called by:	WriteTimeSeries
C-
C-	Created:	Rosalia Rojas-Sanchez
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (last version)
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux (CSU)
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		31-OCT-2003
C-
C-	Revisions:	Documentation, reorganization, complete rewrite
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

void WriteTimeSeriesSolids()
{
	//declare local variables
	int
		igroup,		//local counter for reporting group number
		isolid,		//local counter for solids type
		replink,	//link of a reporting in a channel cell
		repnode;	//node of a reporting in a channel cell

	float
		**sgroupsumov,	//reporting group sum for overland plane [igroup][ireport]
		**sgroupsumch,	//reporting group sum for channel network [igroup][ireport]
		sgroupsumtot,	//reporting group sum for overland plane and channel network
		dqsum,			//sum of gross outflows (m3/s)
		dischargeov,	//overland flow as needed for units conversions (m3/s)
		dischargech;	//channel flow as needed for units conversions (m3/s)

	//If user requested reports for sediment discharges
	if(nsedreports > 0)
	{
		//Initialize group sums...
		//
		//Allocate memory
		sgroupsumov = (float **)malloc((nsgroups+1) * sizeof(float *));		//reporting group sum for overland plane (units vary)
		sgroupsumch = (float **)malloc((nsgroups+1) * sizeof(float *));		//reporting group sum for channel network (units vary)

		//loop over solids groups (include zero for sum of all solids)
		for(igroup=0; igroup<=nsgroups; igroup++)
		{
			//Allocate remaining memory
			sgroupsumov[igroup] = (float *)malloc((nsedreports+1) * sizeof(float));		//reporting group sum for overland plane (units vary)
			sgroupsumch[igroup] = (float *)malloc((nsedreports+1) * sizeof(float));		//reporting group sum for channel network (units vary)

			//loop over reporting stations
			for(j=1; j<=nsedreports; j++)
			{
				//set group sum values to zero
				sgroupsumov[igroup][j] = 0.0;
				sgroupsumch[igroup][j] = 0.0;

			}	//end loop over reporting stations

		}	//end loop over solids groups

		//Compute reporting groups sums...
		//
		//loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{ 
			//set the group number
			igroup = sgroupnumber[isolid];

			//loop over reporting stations
			for(j=1; j<=nsedreports; j++)
			{
				//sum total solids overland
				sgroupsumov[0][j] = sgroupsumov[0][j]
					+ sedreportov[isolid][j];

				//add the concentration of each solids type to its reporting group
				sgroupsumov[igroup][j] = sgroupsumov[igroup][j]
					+ sedreportov[isolid][j];

				//sum total solids in channel
				sgroupsumch[0][j] = sgroupsumch[0][j]
					+ sedreportch[isolid][j];

				//add the concentration of each solids type to its reporting group
				sgroupsumch[igroup][j] = sgroupsumch[igroup][j]
					+ sedreportch[isolid][j];

			}	//end loop over reporting stations

		}	//end loop over solids

		//Write sediment discharges at selected locations
		//and print out in comma-delimited format
		//
		//loop over solids groups (include zero for sum of all solids)
		for(igroup=0; igroup<=nsgroups; igroup++)
		{
			//Set the sediment export file pointer to null
			sedexpfile_fp = NULL;

			//open the sediment export file for writing in append mode
			sedexpfile_fp = fopen(sedexpfile[igroup], "a");

			//if the sediment export file pointer is null, abort execution
			if(sedexpfile_fp == NULL)
			{
				//Write message to file
				fprintf(echofile_fp, "Error! Can't open Sediment Export File:  %s\n", sedexpfile[igroup]);

				//Write message to screen
				printf("Error! Can't open Sediment Export File:  %s\n", sedexpfile[igroup]);
				exit(EXIT_FAILURE);	//abort
			}

			//Write current simulation time (hours)
			fprintf(sedexpfile_fp,"%f",simtime);

			//Loop over number of sediment reporting stations
			for(j=1; j<=nsedreports; j++)
			{
				//if the sediment units reporting option = 2
				if(sedunitsopt[j] == 2)
				{
					//if the cell of this sediment reporting station is a channel cell
					if(imask[sedreprow[j]][sedrepcol[j]] > 1)
					{
						//get link and node for this reporting station
						replink = link[sedreprow[j]][sedrepcol[j]];
						repnode = node[sedreprow[j]][sedrepcol[j]];

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

					//Report output in MT/day...
					//
					//multiply output export by conversion factor
					//and discharge and print to file
					fprintf(sedexpfile_fp,",%e",
						sgroupsumov[igroup][j] * dischargeov * sedconvert[j]);

					//multiply output export by conversion factor
					//and discharge and print to file
					fprintf(sedexpfile_fp,",%e",
						sgroupsumch[igroup][j] * dischargech * sedconvert[j]);

					//Note:  The discharge sum only makes sense when the reporting
					//       station is at an outlet and the reporting unit is MT/day
					//
					//Compute group sum of overland and channel export and discharge
					sgroupsumtot = (sgroupsumov[igroup][j] * dischargeov
                        + sgroupsumch[igroup][j] * dischargech) * sedconvert[j]; 

					//print sum of export to file
					fprintf(sedexpfile_fp,",%e", sgroupsumtot);
				}
				else	//sediment units reporting option != 2 (=1)
				{
					//Report output in g/m3 (mg/L)
					//
					//multiply output export by conversion factor
					//and print to file
					fprintf(sedexpfile_fp,",%e",
						sgroupsumov[igroup][j] * sedconvert[j]);

					//multiply output export by conversion factor
					//and discharge and print to file
					fprintf(sedexpfile_fp,",%e",
						sgroupsumch[igroup][j] * sedconvert[j]);

				}	//end if sedunitsopt[] == 2

			}	//end loop over	number of sediment reporting stations

			//print a carriage return after each complete line
			fprintf(sedexpfile_fp,"\n");

			//Close sediment export file
			fclose(sedexpfile_fp);

		}	//end loop over number of solids reporting groups

	}	//end if nsedports > 0

	//free memory for local arrays...
	//
	//loop over reporting groups
	for(igroup=1; igroup<=nsgroups; igroup++)
	{
		//free memory
		free(sgroupsumov[igroup]);
		free(sgroupsumch[igroup]);

	}	//end loop over reporting groups

	//free remaining memory
	free(sgroupsumov);
	free(sgroupsumch);

//End of function: Return to WriteTimeSeries
}
