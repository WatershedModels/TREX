/*---------------------------------------------------------------------
C-  Function:	WriteMassBalanceSolids.c
C-
C-	Purpose/	Write detailed mass balance summary of sediment
C-	Methods:	transport on a cell and node basis at the end of
C-              a successful model run.
C-
C-
C-	Inputs:
C-
C-	Outputs:	mass balance text file
C-
C-	Controls:	ksim, chnopt
C-
C-	Calls:		None
C-
C-	Called by:	trex
C-
C-	Created:	Mark Velleux
C-				Department of Civil Engineering
C-              Colorado State University
C-				Fort Collins, CO 80523
C-
C-	Date:		05-FEB-2004
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

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

void WriteMassBalanceSolids()
{
	//local variable declarations/definitions
	int
		isolid;		//loop index for solids (particle) type

	int
		chanlink,	//channel link number
		channode;	//channel node number

	int uplink,		//link number of upstream link
		upnode,		//node number of upstream node
		downlink,	//link number of downstream link
		downnode,	//node number of downstream node
		row,		//row value of ichnrow[][] of a node
		col;		//col value of ichncol[][] of a node

	float
		bwch,		//bottom width of channel (m)
		hbch,		//bank height of channel (m)
		sslope,		//side slope of channel bank (dimensionless)
		lch,		//channel length (m) (includes sinuosity)
		twch,		//top width of channel at flow depth (m)
		achsurf;	//surface area of channel (m)

	//set the mass balance file pointer to null
	msbfile_fp = NULL;

	//open the mass balance file (in append mode)
	msbfile_fp = fopen(msbfile, "a");

	//if file pointer is still null, then abort
	if(msbfile_fp == NULL)
	{
        //write error message to file
		fprintf(echofile_fp, "Error! Can't open mass balance file %s\n", msbfile);

		//write error message to screen
		printf("Error! Can't open mass balance file %s\n", msbfile);
        exit(EXIT_FAILURE);	//exit with error
    }

	//loop over solids (include zero for sum of all solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Write label for present solids type
		//
		//if isolid = 0 (sum of all solids types)
		if(isolid == 0)
		{
			//Write header to file
			fprintf(msbfile_fp,
				"\n\n\nMass Balance for Total Solids (Sum of All Particle Types)\n\n");
		}
		else	//else isolid > 0 (an individual particle type)
		{
			//Label for an individual solids type
			fprintf(msbfile_fp, "\n\n\nMass Balance for Solids Type %d:  %s\n\n",
				isolid, particlename[isolid]);

		}	//end if isolid == 0

		//Write label for overland plane (Part 1)
		fprintf(msbfile_fp,"\nMass summary for the overland plane:\n\n");
		fprintf(msbfile_fp,"\nrow\tcol\timask\tlink\tnode\tinitial mass (kg)");

		//loop over advective flow sources
		for(k=0; k<=10; k++)
		{
			//Write label for overland plane (Part 2a: inflows)
			fprintf(msbfile_fp,"\tadvsedovinmass %d (kg)", k);

		}	//end loop over advective flow sources

		//loop over dispersive flow sources
		for(k=1; k<=10; k++)
		{
			//Write label for overland plane (Part 2b: dispersion inflows)
			fprintf(msbfile_fp,"\tdspsedovinmass %d (kg)", k);

		}	//end loop over dispersive flow sources

		//Write label for overland plane (Part 2c: erosion inflows)
		fprintf(msbfile_fp,"\terssedovinmass (kg)");

		//Write label for overland plane (Part 2d: deposition inflows)
		fprintf(msbfile_fp,"\tdepsedovinmass (kg)");

		//loop over advective flow sources
		for(k=0; k<=10; k++)
		{
			//Write label for overland plane (Part 3a: advection outflows)
			fprintf(msbfile_fp,"\tadvsedovoutmass %d (kg)", k);

		}	//end loop over advective flow sources

		//loop over dispersive flow sources
		for(k=1; k<=10; k++)
		{
			//Write label for overland plane (Part 3b: dispersion outflows)
			fprintf(msbfile_fp,"\tdspsedovoutmass %d (kg)", k);

		}	//end loop over flow sources

		//Write label for overland plane (Part 3c: erosion outflows)
		fprintf(msbfile_fp,"\terssedovoutmass (kg)");

		//Write label for overland plane (Part 3d: deposition outflows)
		fprintf(msbfile_fp,"\tdepsedovoutmass (kg)");

		//Write label for overland plane (Part 4)
		fprintf(msbfile_fp,"\tfinal mass (kg)\n");

		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//loop over columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] != nodatavalue)
				{
					//if the cell is a channel cell
					if(imask[i][j] > 1)
					{
						//Get channel link and node
						chanlink = link[i][j];
						channode = node[i][j];

						//Compute area of channel within cell...
						//
						//Assign channel characteristics
						bwch = bwidth[chanlink][channode];			//channel bottom width (m)
						hbch = hbank[chanlink][channode];			//channel bank height (m)
						sslope = sideslope[chanlink][channode];		//channel side slope
						lch = chanlength[chanlink][channode];		//channel length (m) (includes sinuosity)
						twch = twidth[chanlink][channode];			//channel top width at bank height (m)

						//Surface area of channel portion of cell (m2)
						achsurf = twch * lch;
					}
					else	//cell is not a channel cell (overland only)
					{
						//no channel present, surface area is zero
						achsurf = 0.0;

						//set chanlink to -1
						chanlink = -1;

						//set channode to -1
						channode = -1;

					}	//end if imask > 1

					//write summary for the current cell (Part 1)
					fprintf(msbfile_fp,"\n%d\t%d\t%d\t%d\t%d\t%f",
						i, j, imask[i][j], chanlink, channode,
						initialsolidsov[isolid][i][j][0]);

					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current cell (Part 2a: advection inflows)
						fprintf(msbfile_fp,"\t%f", advsedovinmass[isolid][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current cell (Part 2b: dispersion inflows)
						fprintf(msbfile_fp,"\t%f", dspsedovinmass[isolid][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 2c: erosion inflows)
					fprintf(msbfile_fp,"\t%f", erssedovinmass[isolid][i][j][0]);

					//write summary for the current cell (Part 2d: deposition inflows)
					fprintf(msbfile_fp,"\t%f", depsedovinmass[isolid][i][j][0]);

					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current cell (Part 3a: advection outflows)
						fprintf(msbfile_fp,"\t%f", advsedovoutmass[isolid][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current cell (Part 3b: dispersion outflows)
						fprintf(msbfile_fp,"\t%f", dspsedovoutmass[isolid][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 3c: erosion outflows)
					fprintf(msbfile_fp,"\t%f", erssedovoutmass[isolid][i][j][0]);

					//write summary for the current cell (Part 3d: deposition outflows)
					fprintf(msbfile_fp,"\t%f", depsedovoutmass[isolid][i][j][0]);

					//write summary for the current cell (Part 4)
					fprintf(msbfile_fp,"\t%f", finalsolidsov[isolid][i][j][0]);

				}	//end if imask[][] != nodatavalue

			}	//end loop over columns

			//write a blank line to file
			fprintf(msbfile_fp,"\n");

		}	//end loop over rows

		//if channels are simulated
		if(chnopt == 1)
		{
			//Write label for channel network (Part 1)
			fprintf(msbfile_fp,"\n\n\nMass summary for the channel network:\n\n");
			fprintf(msbfile_fp,"\nlink\tnode\tuplink\tupnode\tdownlink\tdownnode");
			fprintf(msbfile_fp,"\trow\tcol\tinitial mass (kg)");

			//loop over advective flow sources
			for(k=0; k<=10; k++)
			{
				//Write label for channel network (Part 2a: advective inflows)
				fprintf(msbfile_fp,"\tadvsedchinmass %d (kg)", k);

			}	//end loop over advective flow sources

			//loop over dispersive flow sources
			for(k=1; k<=10; k++)
			{
				//Write label for channel network (Part 2b: dispersive inflows)
				fprintf(msbfile_fp,"\tdspsedchinmass %d (kg)", k);

			}	//end loop over dispersive flow sources

			//Write label for channel network (Part 2c: erosion inflows)
			fprintf(msbfile_fp,"\terssedchinmass (kg)");

			//Write label for channel network (Part 2d: deposition inflows)
			fprintf(msbfile_fp,"\tdepsedchinmass (kg)");

			//loop over advective flow sources
			for(k=0; k<=10; k++)
			{
				//Write label for channel network (Part 3a: advective outflows)
				fprintf(msbfile_fp,"\tadvsedchoutmass %d (kg)", k);

			}	//end loop over advective flow sources

			//loop over dispersive flow sources
			for(k=1; k<=10; k++)
			{
				//Write label for channel network (Part 3b: dispersive outflows)
				fprintf(msbfile_fp,"\tdspsedchoutmass %d (kg)", k);

			}	//end loop over dispersive flow sources

			//Write label for channel network (Part 3c: erosion outflows)
			fprintf(msbfile_fp,"\terssedchoutmass (kg)");

			//Write label for channel network (Part 3d: deposition outflows)
			fprintf(msbfile_fp,"\tdepsedchoutmass (kg)");

			//Write label for channel network (Part 4)
			fprintf(msbfile_fp,"\tfinal mass (kg)\n");

			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Get overland cell row and column
					row = ichnrow[i][j];
					col = ichncol[i][j];

					//Find the upstream link and node...
					//
					//if it is the first node of the link
					if(j == 1)
					{
	//mlv:  expand reporting so that all possible uplink/node and downlink/node
	//mlv   combinations are reported.  Use 8 pairs of columns for uplink and
	//mlv   another 8 pairs of columns for downlink.  Loop over directions...
	//mlv   use updirection[][][] and downdirection[][][] to place the values
	//mlv   in the right columns and nupbranches[i][k] and ndownbranches[i][k]
	//mlv   to assign values...

	//mlv:  no. just report one valid upstream link even if there are more...

						//a link upstream of the current link
						uplink = -9999;

						//the upstream node is the last node of the upstream link
						upnode = -9999;   //nnodes[uplink];
					}
					else	//else it is not the first node of the link (j > 1)
					{
						//the upstream link is the current link
						uplink = i;

						//the upstream node is the prior node
						upnode = j - 1;

					}	//end if j = 1

					//Find the downstream link and node...
					//
					//if it not the last node of the link
					if(j < nnodes[i])
					{
						//the downstream link is the current link
						downlink = i;

						//the downstream node is the next node
						downnode = j + 1;
					}
					else	//else it is the last node of the link (j = nnodes)
					{
						//Determine the downstream link
						downlink = link[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
							[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

						//Determine downstream node (first node of link, except at boundaries)
						downnode = node[ichnrow[link[row][col]][nnodes[link[row][col]]+1]]
							[ichncol[link[row][col]][nnodes[link[row][col]]+1]];

					}	//end if j = 1

					//Compute area of channel within cell...
					//
					//Assign channel characteristics
					bwch = bwidth[i][j];		//channel bottom width (m)
					hbch = hbank[i][j];			//channel bank height (m)
					sslope = sideslope[i][j];	//channel side slope
					lch = chanlength[i][j];		//channel length (m) (includes sinuosity)
					twch = twidth[i][j];		//channel top width at bank height (m)

					//Surface area of channel portion of cell (m2)
					achsurf = twch * lch;

					//write summary for the current node (Part 1)
					fprintf(msbfile_fp,"\n%d\t%d\t%d\t%d",
						i, j, uplink, upnode);
					fprintf(msbfile_fp,"\t%d\t%d\t%d\t%d",
						downlink, downnode, row, col);
					fprintf(msbfile_fp,"\t%f",
						initialsolidsch[isolid][i][j][0]);
					
					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current node (Part 2a: inflows)
						fprintf(msbfile_fp,"\t%f", advsedchinmass[isolid][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current node (Part 2a: dispersive inflows)
						fprintf(msbfile_fp,"\t%f", dspsedchinmass[isolid][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 2c: erosion inflows)
					fprintf(msbfile_fp,"\t%f", erssedchinmass[isolid][i][j][0]);

					//write summary for the current cell (Part 2d: deposition inflows)
					fprintf(msbfile_fp,"\t%f", depsedchinmass[isolid][i][j][0]);

					//loop over advective flow sources
					for(k=0; k<=10; k++)
					{
						//write summary for the current cell (Part 3a: advection outflows)
						fprintf(msbfile_fp,"\t%f", advsedchoutmass[isolid][i][j][k]);

					}	//end loop over advective flow sources

					//loop over dispersive flow sources
					for(k=1; k<=10; k++)
					{
						//write summary for the current cell (Part 3b: dispersion outflows)
						fprintf(msbfile_fp,"\t%f", dspsedchoutmass[isolid][i][j][k]);

					}	//end loop over dispersive flow sources

					//write summary for the current cell (Part 3c: erosion outflows)
					fprintf(msbfile_fp,"\t%f", erssedchoutmass[isolid][i][j][0]);

					//write summary for the current cell (Part 3d: deposition outflows)
					fprintf(msbfile_fp,"\t%f", depsedchoutmass[isolid][i][j][0]);

					//write summary for the current node (Part 4)
					fprintf(msbfile_fp,"\t%f", finalsolidsch[isolid][i][j][0]);

				}	//end loop over nodes

				//write a blank line to file
				fprintf(msbfile_fp,"\n");

			}	//end loop over links

		}	//end if chnopt = 1

	}	//end loop over solids

	//close the mass balance file
	fclose(msbfile_fp);

//End of function: Return to WriteMassBalance
}
