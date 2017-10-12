/*---------------------------------------------------------------------
C-  Function:	WriteSummarySolids.c
C-
C-	Purpose/	Write summary solids and sediment transport information
C-	Methods:	at the end of a successful model run.
C-
C-	Inputs:
C-
C-	Outputs:
C-
C-	Controls:	
C-
C-	Calls:		None
C-
C-	Called by:	WriteSummary
C-
C-	Created:	Rosalia Rojas-Sanchez
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		19-JUN-2003 (assumed date)
C-
C-	Revised:	John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-              Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-	Date:		19-MAR-2004
C-
C-	Revisions:	Documentation, reorganization, expanded reporting
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

void WriteSummarySolids()
{
	//local variable declarations
	int
		isolid;		//loop index for solids (particle) type

	double
		*initialmass,	//initial mass in domain (m3) [solid]
		*finalmass,		//final mass in domain (m3) [solid]
		*totalmassin,	//total mass entering domain (m3) [solid]
		*totalmassout,	//total mass exiting domain (m3) [solid]
		*sourcemass,	//sum of all sources (m3) [solid]
		*sinkmass,		//sum of all sinks (m3) [solid]
		*mberror;		//mass balance error (%) [solid]

	//set the summary statistics file pointer to null
	statsfile_fp = NULL;

	//open the summary statistics file (in append mode)
	statsfile_fp = fopen(statsfile, "a");

	//if file pointer is still null, then abort
	if(statsfile_fp == NULL)
	{
		//write error message to file
		fprintf(echofile_fp, "Error! Can't open Summary Statistics file %s\n", statsfile);

		//write error message to screen
		printf("Error! Can't open Summary Statistics file %s\n", statsfile);
		exit(EXIT_FAILURE);	//exit with error
    }

	//Alocate memory for local arrays
	initialmass = (double *)malloc((nsolids+1) * sizeof(double));	//initial mass in the water column (kg) [solid]
	finalmass = (double *)malloc((nsolids+1) * sizeof(double));		//final mass in the water column (kg) [solid]
	totalmassin = (double *)malloc((nsolids+1) * sizeof(double));	//total mass entering domain (m3) [solid]
	totalmassout = (double *)malloc((nsolids+1) * sizeof(double));	//total mass exiting domain (m3) [solid]
	sourcemass = (double *)malloc((nsolids+1) * sizeof(double));	//sum of all sources (m3) [solid]
	sinkmass = (double *)malloc((nsolids+1) * sizeof(double));		//sum of all sinks (m3) [solid]
	mberror = (double *)malloc((nsolids+1) * sizeof(double));		//mass balance error (%) [solid]

	//Initialize local arrays
	//
	//loop over solids (use zero for sum of all solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//set initial values to zero
		initialmass[isolid] = 0.0;	//initial mass in the water column (kg) [solid]
		finalmass[isolid] = 0.0;	//final mass in the water column (kg) [solid]
		totalmassin[isolid] = 0.0;	//total mass entering domain (m3) [solid]
		totalmassout[isolid] = 0.0;	//total mass exiting domain (m3) [solid]
		sourcemass[isolid] = 0.0;	//sum of all sources (m3) [solid]
		sinkmass[isolid] = 0.0;		//sum of all sinks (m3) [solid]
		mberror[isolid] = 0.0;		//mass balance error (%) [solid]

	}	//end loop over solids

	//loop over solids (in loop, use zero for sum of all solids types)
	for(isolid=1; isolid<=nsolids; isolid++)
	{
		//Compute total and individual solids masses in domain at
		//simulation start and end...
		//
		//compute initial overland water column solids mass at start (kg)
		initialmass[isolid] = initialsolidsmassov[isolid][0];

		//Compute final overland water column solids mass at end (kg)
		finalmass[isolid] = finalsolidsmassov[isolid][0];

		//compute initial overland water column total solids mass at start (kg)
		initialmass[0] = initialmass[0]
			+ initialsolidsmassov[isolid][0];

		//Compute final overland water column total solids mass at end (kg)
		finalmass[0] = finalmass[0]
			+ finalsolidsmassov[isolid][0];

		//if channels are simulated
		if(chnopt > 0)
		{
			//add initial channel water column solids mass at start (kg)
			initialmass[isolid] = initialmass[isolid]
				+ initialsolidsmassch[isolid][0];

			//add final channel water column solids mass at end (kg)
			finalmass[isolid] = finalmass[isolid]
				+ finalsolidsmassch[isolid][0];

			//compute initial overland water column total solids mass at start (kg)
			initialmass[0] = initialmass[0]
				+ initialsolidsmassch[isolid][0];

			//Compute final overland water column total solids mass at end (kg)
			finalmass[0] = finalmass[0]
				+ finalsolidsmassch[isolid][0];

		}	//end if chnopt > 0

		//Compute cumulative individual and total solids mass entering domain
		//during the simulation...
		//
		//Compute cumulative solids mass entering domain overland (water column) (kg)
		totalmassin[isolid] = totalswov[isolid]
			+ totaladvsedinov[isolid][0] + totaldspsedinov[isolid][0];

		//Add cumulative mass entering the overland water column by erosion (kg)
		totalmassin[isolid] = totalmassin[isolid]
			+ totalerssedinov[isolid];

		//Compute cumulative total solids mass entering domain overland (water column) (kg)
		totalmassin[0] = totalmassin[0] + totalswov[isolid]
			+ totaladvsedinov[isolid][0] + totaldspsedinov[isolid][0];

		//Add cumulative total mass entering the overland water column by erosion (kg)
		totalmassin[0] = totalmassin[0]
			+ totalerssedinov[isolid];

		//Compute cumulative individual and total solids mass exiting domain
		//during the simulation...
		//
		//Compute cumulative mass leaving the overland plane (kg)
		totalmassout[isolid] = totaladvsedoutov[isolid][0]
			+ totaldspsedoutov[isolid][0];

		//Add cumulative mass leaving the overland water column by deposition (kg)
		totalmassout[isolid] = totalmassout[isolid]
			+ totaldepsedoutov[isolid];

		//Compute cumulative total solids mass exiting domain overland (water column) (kg)
		totalmassout[0] = totalmassout[0] + totaladvsedoutov[isolid][0]
			+ totaldspsedoutov[isolid][0];

		//Add cumulative total mass entering the overland water column by deposition (kg)
		totalmassout[0] = totalmassout[0]
			+ totaldepsedoutov[isolid];

		//if channels are simulated
		if(chnopt > 0)
		{
			//Compute cumulative individual and total solids mass entering domain
			//during the simulation...
			//
			//Compute cumulative solids mass entering domain in channels (water column) (kg)
			totalmassin[isolid] = totalmassin[isolid]
				+ totalswch[isolid] + totaladvsedinch[isolid][0]
				+ totaldspsedinch[isolid][0];

			//Add cumulative mass entering the channel water column by erosion (kg)
			totalmassin[isolid] = totalmassin[isolid]
				+ totalerssedinch[isolid];

			//Compute cumulative total solids mass entering domain overland (water column) (kg)
			totalmassin[0] = totalmassin[0]
				+ totalswch[isolid] + totaladvsedinch[isolid][0]
				+ totaldspsedinch[isolid][0];

			//Add cumulative total mass entering the channel water column by erosion (kg)
			totalmassin[0] = totalmassin[0]
				+ totalerssedinch[isolid];

			//Compute cumulative individual and total solids mass exiting domain
			//during the simulation...
			//
			//Compute cumulative mass leaving the channel network (kg)
			totalmassout[isolid] = totalmassout[isolid]
				+ totaladvsedoutch[isolid][0] + totaldspsedoutch[isolid][0];

			//Add cumulative mass leaving the channel water column by deposition (kg)
			totalmassout[isolid] = totalmassout[isolid]
				+ totaldepsedoutch[isolid];

			//Compute cumulative total mass leaving the channel network (kg)
			totalmassout[0] = totalmassout[0]
				+ totaladvsedoutch[isolid][0] + totaldspsedoutch[isolid][0];

			//Add cumulative total mass leaving the channel water column by deposition (kg)
			totalmassout[0] = totalmassout[0]
				+ totaldepsedoutch[isolid];

		}	//end if chnopt > 0

	}	//end loop over solids

	//loop over solids
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Compute sum of water column solids source masses (kg)
		sourcemass[isolid] = initialmass[isolid]
			+ totalmassin[isolid];

		//Compute sum of water column solids sink masses (kg)
		sinkmass[isolid] = finalmass[isolid]
			+ totalmassout[isolid];

		//if sourcemass[] > zero
		if(sourcemass[isolid] > 0.0)
		{
			//Compute solids mass balance error (percent)
			mberror[isolid] = (sourcemass[isolid] - sinkmass[isolid])
				/ sourcemass[isolid] * 100.0;
		}
		else	//else sourcemass[] = 0 (it can't be < 0)
		{
			//set the solids mass balance error to zero
			mberror[isolid] = 0.0;

		}	//end if sourcemass[] > 0.0

		//Summary of Solids Transport Output
		fprintf(statsfile_fp, "\n\nSUMMARY OF SOLIDS TRANSPORT OUTPUT\n");
		fprintf(statsfile_fp,     "==================================\n\n");

		//Write label for present solids type
		//
		//if isolid = 0 (sum of all solids types)
		if(isolid == 0)
		{
			//Label for total solids
			fprintf(statsfile_fp, "Total Solids (Sum of All Particle Types)\n\n");
		}
		else	//else isolid > 0 (an individual particle type)
		{
			//Label for an individual solids type
			fprintf(statsfile_fp, "Solids Type %d:  %s\n\n",
				isolid, particlename[isolid]);

		}	//end if isolid == 0

		fprintf(statsfile_fp, "Summary for Water Column (Layer 0)\n");
		fprintf(statsfile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		//Loop over outlets
		for(i=1; i<=noutlets; i++)
		{
			fprintf(statsfile_fp,
				"Outlet: %d (Row %d, Col %d)\n", i, iout[i], jout[i]);

			//if channels are not simulated or the outlet option is 1
			if(chnopt == 0 || outopt == 1)
			{
				fprintf(statsfile_fp,
					"  Peak Overland Solids Discharge (kg/s)....................= ");
				fprintf(statsfile_fp,"%15.2e\n", sedflowpeakov[isolid][i]);

				fprintf(statsfile_fp,
					"  Time to Peak Solids Discharge (hours)....................= ");
				fprintf(statsfile_fp,"%15.2e\n", sedtimepeakov[isolid][i]);

			}	//end if chnopt = 0 or outopt = 1

			//if channels are simulated
			if(chnopt > 0)
			{
				fprintf(statsfile_fp,
					"  Peak Channel Solids Discharge (kg/s).....................= ");
				fprintf(statsfile_fp,"%15.2e\n", sedflowpeakch[isolid][i]);

				fprintf(statsfile_fp,
					"  Time to Peak Solids Discharge (hours)....................= ");
				fprintf(statsfile_fp,"%15.2e\n", sedtimepeakch[isolid][i]);

			}	//end of chnopt > 0

			fprintf(statsfile_fp, "\n");

		}	//end loop over outlets

		fprintf(statsfile_fp,
			"Initial Solids Mass Within Water Column (kg)...............= ");
		fprintf(statsfile_fp,"%15.2e\n\n", initialmass[isolid]);

		fprintf(statsfile_fp,
			"Cumulative Solids Mass Entering By Advection (kg)..........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvsedinov[isolid][0]+totaladvsedinch[isolid][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvsedinov[isolid][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Advection (kg)...........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaladvsedinov[isolid][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Advection (kg)............= ");

			fprintf(statsfile_fp,"%15.2e\n\n",
				totaladvsedinch[isolid][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Cumulative Solids Mass Entering By Dispersion (kg).........= ");

		//if channels are simulated
		if(chnopt > 1)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspsedinov[isolid][0]+totaldspsedinch[isolid][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspsedinov[isolid][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Dispersion (kg)..........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaldspsedinov[isolid][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Dispersion (kg)...........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaldspsedinch[isolid][0]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Cumulative Solids Mass Entering By Erosion (kg)............= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalerssedinov[isolid]+totalerssedinch[isolid]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totalerssedinov[isolid]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"  Overland Erosion (kg).............= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totalerssedinov[isolid]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Erosion (kg)..............= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totalerssedinch[isolid]);

		}	//end if channels are not simulated

		fprintf(statsfile_fp,
			"Solids Mass Entering Domain from External Sources (kg).....= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",totalswov[isolid]+totalswch[isolid]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",totalswov[isolid]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Sources (kg).............= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totalswov[isolid]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Sources (kg)..............= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totalswch[isolid]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Solids Mass Leaving By Advection (kg)...........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvsedoutov[isolid][0]+totaladvsedoutch[isolid][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaladvsedoutov[isolid][0]);
			
		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Advection (kg)...........= ");

		fprintf(statsfile_fp,"%15.2e\n",
			totaladvsedoutov[isolid][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Advection (kg)............= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaladvsedoutch[isolid][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Solids Mass Leaving By Dispersion (kg)..........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspsedoutov[isolid][0]+totaldspsedoutch[isolid][0]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldspsedoutov[isolid][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Dispersion (kg)..........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaldspsedoutov[isolid][0]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Dispersion (kg)...........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaldspsedoutch[isolid][0]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Solids Mass Leaving By Deposition (kg)..........= ");

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldepsedoutov[isolid]+totaldepsedoutch[isolid]);
		}
		else	//else channels are not simulated
		{
			fprintf(statsfile_fp,"%15.2e\n",
				totaldepsedoutov[isolid]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"  Overland Deposition (kg)..........= ");
		fprintf(statsfile_fp,"%15.2e\n",
			totaldepsedoutov[isolid]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"  Channel Deposition (kg)...........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",
				totaldepsedoutch[isolid]);

		}	//end if channels are simulated

		fprintf(statsfile_fp,
			"Cumulative Mass Entering Domain, M_in (kg).................= ");
		fprintf(statsfile_fp,"%15.2e\n",totalmassin[isolid]);

		fprintf(statsfile_fp,
			"Cumulative Mass Leaving the Water Column, M_out (kg).......= ");
		fprintf(statsfile_fp,"%15.2e\n",totalmassout[isolid]);

		//if totalmassin[] > zero
		if(totalmassin[isolid] > 0.0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_out to M_in (%%)............................= ");
			fprintf(statsfile_fp,"%15.2f\n\n",
				(totalmassout[isolid]/totalmassin[isolid])*100.0);
		}
		else	//else totalmassin[] = 0.0 (it can't be < 0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_out to M_in (%%)............................= ");
			fprintf(statsfile_fp,"%s\n\n",
				"Not Calculated");

		}	//end if totalmassin[] > 0.0

		fprintf(statsfile_fp,
			"Final Solid Mass Within Water Column, M_final (kg).........= ");
		fprintf(statsfile_fp,"%15.2e\n",finalmass[isolid]);

		//if totalmassin[] > zero
		if(totalmassin[isolid] > 0.0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_final to M_in (%%)..........................= ");
			fprintf(statsfile_fp,"%15.2f\n\n",
				(finalmass[isolid]/totalmassin[isolid])*100.0);
		}
		else	//else totalmassin[] = 0.0 (it can't be < 0)
		{
			fprintf(statsfile_fp,
				"Percentage of M_final to M_in (%%)..........................= ");
			fprintf(statsfile_fp,"%s\n\n",
				"Not Calculated");

		}	//end if totalmassin[] > 0.0

		fprintf(statsfile_fp,
			"Percent Mass Balance Error (%%).............................= ");
		fprintf(statsfile_fp,"%15.2f\n\n",mberror[isolid]);

		//print solids concentration min/max summary
		fprintf(statsfile_fp,
			"\nSUSPENDED SOLIDS MINIMUM AND MAXIMUM VALUES\n");
		fprintf(statsfile_fp,
			  "===========================================\n\n");

		fprintf(statsfile_fp,
			"Min. Overland Suspended Solids Concentration (g/m3)........= ");
		fprintf(statsfile_fp,"%15.2e\n",mincsedov0[isolid]);

		fprintf(statsfile_fp,
			"Max. Overland Suspended Solids Concentration (g/m3)........= ");
		fprintf(statsfile_fp,"%15.2e\n\n",maxcsedov0[isolid]);

		fprintf(statsfile_fp,
			"Min. Overland Surface Soil Solids Concentration (g/m3).....= ");
		fprintf(statsfile_fp,"%15.2e\n",mincsedov1[isolid]);

		fprintf(statsfile_fp,
			"Max. Overland Surface Soil Solids Concentration (g/m3).....= ");
		fprintf(statsfile_fp,"%15.2e\n\n",maxcsedov1[isolid]);

		//if channels are simulated
		if(chnopt > 0)
		{
			fprintf(statsfile_fp,
				"Min. Channel Suspended Solids Concentration (g/m3).........= ");
			fprintf(statsfile_fp,"%15.2e\n",mincsedch0[isolid]);

			fprintf(statsfile_fp,
				"Max. Channel Suspended Solids Concentration (g/m3).........= ");
			fprintf(statsfile_fp,"%15.2e\n\n",maxcsedch0[isolid]);

			fprintf(statsfile_fp,
				"Min. Channel Surface Sediment Solids Concentration (g/m3)..= ");
			fprintf(statsfile_fp,"%15.2e\n",mincsedch1[isolid]);

			fprintf(statsfile_fp,
				"Max. Channel Surface Sediment Solids Concentration (g/m3)..= ");
			fprintf(statsfile_fp,"%15.2e\n\n",maxcsedch1[isolid]);
		}
		else	//else channels are not simulated
		{
			//print message that channel flow not simulated...
			fprintf(statsfile_fp, "\nChannel transport was not simulated\n\n");

		}	//end if chnopt = 1

		//add blank space for formatting...
		fprintf(statsfile_fp, "\n\n\n");

	}	//end loop over solids

	//close the summary statistics file
	fclose(statsfile_fp);

	//free memory for local arrays
	free(initialmass);
	free(finalmass);
	free(totalmassin);
	free(totalmassout);
	free(sourcemass);
	free(sinkmass);
	free(mberror);

//End of function: Return to WriteSummary
}
