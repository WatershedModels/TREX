/*---------------------------------------------------------------------
C-  Function:   ReadDataGroupB.c
C-
C-  Purpose/    ReadDataGroupB is called at the start
C-  Methods:    of the simulation to read Data Group B
C-              (hydrologic and hydraulic simulation parameters) from
C-              the model input file.
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   ksim, infopt, chnopt, tplgyopt, rainopt, dbcopt[i]
C-
C-  Calls:      (in order of appearance/call in function)
C-              StripString, ReadMaskFile, ReadElevationFile,
C-              ReadSoilTypeFile, ReadLandUseFile, ReadStorageDepthFile,
C-              ReadLinkFile, ReadNodeFile, ReadChannelFile,
C-              ComputeChannelTopology,ComputeChannelLength,ComputeChannelElevation,
C-              ReadTopologyFile (dummy),
C-              ReadInitialWaterOverlandFile, ReadInitialWaterChannelFile,
C-              ReadDesignRainGrid, ReadRadarRainLocations, ReadRadarRainRates,
C-              ReadSpaceTimeStorm, ReadInitialSnowFile
C-
C-  Called by:  ReadInputFile
C-
C-  Created:    Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-              Aditya Maroo (CSU)
C-
C-  Date:       24-JUN-2004
C-
C-  Revised:    John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-  Date:       18-JUL-2004
C-
C-  Revisions:  Additional rain and snow options, Emacs formatting
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       27-JUL-2006
C-
C-  Revisions:  Added rainopt = 5 to permit use of grid-based
C-              rainfall intensities (variable in space) used
C-              as step functions in time
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       22-DEC-2006
C-
C-  Revisions:  Added options for snowfall and snowmelt (as SWE).
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-              John F. England, Jr.
C-              Bureau of Reclamation
C-              Flood Hydrology Group, D-8530
C-              Bldg. 67, Denver Federal Center
C-              Denver, CO 80225
C-
C-  Date:       15-AUG-2007
C-
C-  Revisions:  Integrated rainfall options 2-4 into code
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       15-JUN-2009
C-
C-  Revisions:  Added code to set bed slope of channels at outlets.
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
C----------------------------------------------------------------------*/

//trex global variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

void ReadDataGroupB()
{
	//local variable declarations/definitions
	float
		convunits,	//conversion factors for length, area, volume or mass units
		convtime,	//conversion factor for time units
		scale;		//scale factor

	//open the echo file in append mode (append to existing file)
	echofile_fp = fopen(echofile, "a");

	//write message to screen
	printf("\n\n***************************\n");
	printf(    "*                         *\n");
	printf(    "*   Reading Data Group B  *\n");
	printf(    "*                         *\n");
	printf(    "***************************\n\n\n");

	//Record 1
	fgets(header, MAXHEADERSIZE, inputfile_fp);  	//read header line from input file

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s\n\n", header);

	//Record 2 (Part 1)
	fscanf(inputfile_fp, "%s", varname);  //read dummy

	//Allocate memory for mask file name
	maskfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Record 2 (Part 2)
	fgets(maskfile, MAXNAMESIZE, inputfile_fp);  //read mask file name

	//strip leading blanks, trailing blanks, final carriage return from string
	maskfile = StripString(maskfile);

	//Echo mask file name to file
	fprintf(echofile_fp, "\nMask File: %s\n\n", maskfile);

	//Read Mask File
	ReadMaskFile();

	//Record 3 (Part 1)
	fscanf(inputfile_fp, "%s", varname);  //read dummy

	//Allocate memory for elevation file name
	elevationfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Record 3 (Part 2)
	fgets(elevationfile, MAXNAMESIZE, inputfile_fp);  //read elevation file name

	//strip leading blanks, trailing blanks, final carriage return from string
	elevationfile = StripString(elevationfile);

	//Echo elevation file name to file
	fprintf(echofile_fp, "\nElevation File: %s\n\n", elevationfile);

	//Read Elevation File
	ReadElevationFile();

	//Record 4
	fscanf(inputfile_fp, "%s %d",  //read
		varname,  //dummy
		&infopt);  //inftration option

	//Echo Infiltration Option to file
	fprintf(echofile_fp, "\n\n\nInfiltration Option = %d\n", infopt);

	//Check value of ksim (if simulation type is hydrology only...)
	if(ksim == 1)
	{
		//Check value of infopt
		if(infopt == 1)
		{
			//Record 5
			fscanf(inputfile_fp, "%s %d",  //read
				varname,  //dummy
				&nsoils);  //Number of soil types

			//Echo Number of soil types to file
			fprintf(echofile_fp, "\n\nNumber of Soil types = %d\n", nsoils);

			//Allocate memory for infitration parameters
			khsoil = (float *)malloc((nsoils+1) * sizeof(float));	//soil hydraulic conductivity (m/s)
			capshsoil = (float *)malloc((nsoils+1) * sizeof(float));	//soil capillary suction head (m)
//mlv			porosityov = (float *)malloc((nsoils+1) * sizeof(float));	//soil porosity (dimensionless)
//mlv			thetarov = (float *)malloc((nsoils+1) * sizeof(float));	//soil residual moisture content (dimensionless)
			soilmd = (float *)malloc((nsoils+1) * sizeof(float));	//soil moisture deficit (dimensionless)
			soilname = (char **)malloc((nsoils+1) * sizeof(char *));	//soil name

			//Write label for land use parameters to file
			fprintf(echofile_fp, "\n  Hydraulic Conductivity (m/s)    ");
			fprintf(echofile_fp,   "  Capillary Suction Head (m)    ");
//mlv			fprintf(echofile_fp,   "  Soil Porosity (-)    ");
//mlv			fprintf(echofile_fp,   "  Soil Residual Moisture Content (-)    ");
			fprintf(echofile_fp,   "  Soil Moisture Deficit (-)    ");
			fprintf(echofile_fp,   "  Soil Type Description  \n");
			fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ");
			fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ");
			fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~  ");
//mlv			fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ");
//mlv			fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ");
			fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~~~\n");

			//Loop over the number of soil types
			for(i=1; i<=nsoils; i++)
			{
				//Allocate remaining memory for soilname...
				soilname[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));

				//Record 6 (Part 1)
				fscanf(inputfile_fp, "%f %f %f",  //read
					&khsoil[i],  //Soil Hydraulic Conductivity (m/s)
					&capshsoil[i],  //Soil Capillary Suction Head (m)
//mlv					&porosityov[i]);  //Soil Moisture Deficit (dimensionless)
//mlv					&thetarov[i]);  //Soil Residual Moisture Content (dimensionless)
					&soilmd[i]);  //Soil Moisture Deficit (dimensionless)

				//Record 6 (Part 2)
				fgets(soilname[i], MAXNAMESIZE, inputfile_fp);  //Read Soil Name

				//Echo Hydraulic Conductivity, Capillary Suction Head
				//  & Soil Moisture Deficit to file
				fprintf(echofile_fp, "%32.8f  %29.3f  %29.3f  %s",
					khsoil[i], capshsoil[i], soilmd[i], soilname[i]);

			}  //End loop over soil types

			//Record 7 (Part 1)
			fscanf(inputfile_fp, "%s", varname);  //read dummy

			//Allocate memory for soil file name
			soiltypefile = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Record 7 (Part 2)
			fgets(soiltypefile, MAXNAMESIZE, inputfile_fp);  //read soil file name

			//strip leading blanks, trailing blanks, final carriage return from string
			soiltypefile = StripString(soiltypefile);

			//Echo Soil Type Classification File name to file
			fprintf(echofile_fp, "\n\nSoil Type Classification File: %s\n",
				soiltypefile);

			//Allocate memory for soil types
			soiltype = (int ***)malloc((nrows+1) * sizeof(int **));  //allocate memory

			//Loop over number of rows
			for(i=1; i<=nrows; i++)
			{
				//Allocate additional memory for soil types
				soiltype[i] = (int **)malloc((ncols+1) * sizeof(int *));  //allocate memory

				//Loop over number of columns
				for(j=1; j<=ncols; j++)
				{
					//Allocate remaining memory for soil types
					soiltype[i][j] = (int *)malloc(2 * sizeof(int));  //allocate memory

				}  //end loop over columns

			}  //end loop over rows

			//Read Soil Type Classification File (for ksim = 1, soil layer = 1)
			ReadSoilTypeFile(1);

		}  //End check of infopt

		//Record 8
		fscanf(inputfile_fp, "%s %d",
			varname,  //dummy
			&nlands);  //Number of Land Use Classes

		//Echo Number of Land Use Classes to file
		fprintf(echofile_fp, "\n\nNumber of Land Use Classes %d\n", nlands);

		//Allocate memory to land use class parameters
		nmanningov = (float *)malloc((nlands+1) * sizeof(float));  //allocate memory
		interceptionclass = (float *)malloc((nlands+1) * sizeof(float));  //allocate memory
		landname = (char **)malloc((nlands+1) * sizeof(char *));  //allocate memory

		//Loop over number of land use classes to allocate remaining memory
		for(i=1; i<=nlands; i++)
		{
			landname[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		}

		//Print label for land use parameters to file
		fprintf(echofile_fp, "\n  Manning n     ");
		fprintf(echofile_fp,   "  Interception Depth (mm)     ");
		fprintf(echofile_fp,   "  Land Use Classification  \n");
		fprintf(echofile_fp,   "~~~~~~~~~~~~~   ");
		fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~~~~~   ");
		fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		//Loop over number of land use classes to read land use parameters
		for(i=1; i<=nlands; i++)
		{
			//Record 9 (Part 1)
			fscanf(inputfile_fp, "%f %f",  //read
				&nmanningov[i],  //Manning n
				&interceptionclass[i]);  //interception depth (mm) for this land use

			//Record 9 (Part 2)
			fgets(landname[i], MAXNAMESIZE, inputfile_fp);  //Read Land Use Class Name

			//Echo Manning n, interception depth for current land use, and
			//  land use classification name to echo file
			fprintf(echofile_fp, "%13.4f   %27.4f   %s",
				nmanningov[i], interceptionclass[i], landname[i]);

		}  //End loop over land use classes

		//Record 10 (Part 1)
		fscanf(inputfile_fp, "%s", varname);  //read dummy

		//Allocate memory for land use file name
		landusefile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 10 (Part 2)
		fgets(landusefile, MAXNAMESIZE, inputfile_fp);  //read land use file name

		//strip leading blanks, trailing blanks, final carriage return from string
		landusefile = StripString(landusefile);

		//Echo land use classification file name to file
		fprintf(echofile_fp, "\n\nLand Use Classification File %s\n\n", landusefile);

		//Read Land Use Classification File
		ReadLandUseFile();

	}  //End check of ksim = 1

	//Record 11 (Part 1) (depression storage depth for overland plane)
	fscanf(inputfile_fp, "%s", varname);  //read dummy

	//Allocate memory for storage depth file name
	storagedepthfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Record 11 (Part 2)
	fgets(storagedepthfile, MAXNAMESIZE, inputfile_fp);  //read storage depth file name

	//strip leading blanks, trailing blanks, final carriage return from string
	storagedepthfile = StripString(storagedepthfile);

	//Echo Storage Depth File name to file
	fprintf(echofile_fp, "\n\nStorage Depth File: %s\n",
		storagedepthfile);

	//Read Storage Depth File (storage depth for each grid cell in overland plane)
	ReadStorageDepthFile();

	//Record 12
	fscanf(inputfile_fp, "%s %d",  //read
		varname,  //dummy
		&chnopt);  //Channel Option

	//Echo Channel Option to file
	fprintf(echofile_fp, "\n\n\nChannel Option = %d\n", chnopt);

	//Check value of chnopt (if chnopt = 1 then read channel network info)
	if(chnopt == 1)
	{
		//Record 13
		fscanf(inputfile_fp, "%s %d %s %d %s %d %s %d",	//read
			varname,	//dummy
			&tplgyopt,	//topology option
			varname,	//dummy
			&ctlopt,	//channel transmission loss option
			varname,	//dummy
			&fldopt,	//floodplain transfer option
			varname,	//dummy
			&outopt);	//outlet routing option

		//Echo Topology Option and others to file
		fprintf(echofile_fp, "\n\nTopology Option = %d\n", tplgyopt);
		fprintf(echofile_fp,     "Channel Transmission Loss Option = %d\n", ctlopt);
		fprintf(echofile_fp,     "Floodplain Transfer Option = %d\n", fldopt);
		fprintf(echofile_fp,     "Outlet Routing Option = %d\n", outopt);

		//Check value of tplgyopt
		if(tplgyopt == 0)  //trex computes channel network topology
		{
			//Record 14 (Part 1)
			fscanf(inputfile_fp, "%s", varname);  //read dummy

			//Allocate memory for link file name
			linkfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Record 14 (Part 2)
			fgets(linkfile, MAXNAMESIZE, inputfile_fp);  //read link file name

			//strip leading blanks, trailing blanks, final carriage return from string
			linkfile = StripString(linkfile);

			//Echo Link File name to file
			fprintf(echofile_fp, "\n\nLink File: %s\n", linkfile);

			//Read Link File
			ReadLinkFile();

			//Record 15 (Part 1)
			fscanf(inputfile_fp, "%s", varname);  //read dummy

			//Allocate memory for node file name
			nodefile = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Record 15 (Part 2)
			fgets(nodefile, MAXNAMESIZE, inputfile_fp);  //read node file name

			//strip leading blanks, trailing blanks, final carriage return from string
			nodefile = StripString(nodefile);

			//Echo Node File name to file
			fprintf(echofile_fp, "\n\nNode File: %s\n",
				nodefile);

			//Read Node File
			ReadNodeFile();

			//Record 16 (Part 1) (channel properties including storage depth/dead storage)
			fscanf(inputfile_fp, "%s", varname);  //read dummy

			//Allocate memory for channel file name
			channelfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Record 16 (Part 2)
			fgets(channelfile, MAXNAMESIZE, inputfile_fp);  //read channel property file name

			//strip leading blanks, trailing blanks, final carriage return from string
			channelfile = StripString(channelfile);

			//Echo Channel Property File name to file
			fprintf(echofile_fp, "\n\nChannel File: %s\n", channelfile);

			//Read Channel Properties File
			ReadChannelFile();

			//Compute channel topology from link, node, and channel property files
			ComputeChannelTopology();

			//Compute stream length of each channel element (link/node)
			ComputeChannelLength();

			//Developer's Note:  The following code performs error checks to ensure
			//                   that channel geometry is consistent with physical
			//                   surface area and side slope constraints for a cell
			//                   (i.e. channel surface area cannot exceed cell area
			//                   and top width cannot be less than bottom width...).
			//
			//                   Once global variables for achsurf and aovsurf are
			//                   created, this error check code would be moved to
			//                   InitializeWater.c.
			//
			//Loop over number of links
			for(i=1; i<=nlinks; i++)
			{
				//Loop over number of nodes in this link
				for(j=1; j<=nnodes[i]; j++)
				{
					//Two error checks...
					//
					//The channel surface area at top of bank must be <= 90% of the cell area.
					//
					//Note:  The channel surface area at the top of bank is the product of
					//       top width and channel length = twidth[][] * chanlength[][]
					//
					//Also Note: channel length is includes sinuosity...
					//
					//If channel surface area > 0.90 * cell surface area, abort...
					if(twidth[i][j] * chanlength[i][j] > (float)(0.90 * w * w))
					{
						//Write error message to file
						fprintf(echofile_fp, "\n\n\nChannel File Error:\n");
						fprintf(echofile_fp,       "  Channel surface area exceeds 90%% of cell area...\n");
						fprintf(echofile_fp,       "  link = %d  node = %d\n",
							i, j);
						fprintf(echofile_fp,       "  top width = %f  cell width = %f\n",
							twidth[i][j], w);
						fprintf(echofile_fp,       "  channel length = %f  cell width = %f\n",
							chanlength[i][j], w);
						fprintf(echofile_fp,       "  channel area = %f  cell area = %f\n",
							twidth[i][j] * chanlength[i][j], w * w);
						fprintf(echofile_fp,     "/n  channel area must be less than: %f\n",
							(float)(0.9 * w * w));

						//Write error message to screen
						printf("Channel File Error:\n");
						printf("  Channel surface area exceeds 90%% of cell area...\n");
						printf("  link = %d  node = %d\n",
							i, j);
						printf("  Channel surface area exceeds 90%% of cell area...\n");
						printf("  link = %d  node = %d\n",
							i, j);
						printf("  top width = %f  cell width = %f\n",
							twidth[i][j], w);
						printf("  channel length = %f  cell width = %f\n",
							chanlength[i][j], w);
						printf("  channel area = %f  cell area = %f\n",
							twidth[i][j] * chanlength[i][j], w * w);
						printf("/n  channel area must be less than: %f\n",
							(float)(0.9 * w * w));

						exit(EXIT_FAILURE);					//abort

					}	//end if twidth * chanlength > 0.9 * w * w

					//If top width < bottom width, abort...
					if(twidth[i][j] < bwidth[i][j])
					{
						//Write error message to file
						fprintf(echofile_fp, "\n\n\nChannel File Error:\n");
						fprintf(echofile_fp,       "  Top width less than bottom width...\n");
						fprintf(echofile_fp,       "  link = %d  node = %d\n",
							i, j);
						fprintf(echofile_fp,       "  top width = %f  bottom width = %f\n",
							twidth[i][j], bwidth[i][j]);
						fprintf(echofile_fp,       "  bank height = %f  side slope = %f\n",
							hbank[i][j], sideslope[i][j]);

						//Write error message to screen
						printf("Channel File Error:\n");
						printf("  Top width less than bottom width...\n");
						printf("  link = %d  node = %d\n",
							i, j);
						printf("  top width = %f  bottom width = %f\n",
							twidth[i][j], bwidth[i][j]);
						printf("  bank height = %f  side slope = %f\n",
							hbank[i][j], sideslope[i][j]);

						exit(EXIT_FAILURE);					//abort

					}	//end if top width < bottom width

				}	//end loop over nodes in current link

			}	//end loop over number of links

			//Compute initial bed elevation of each channel element (link/node)
			ComputeChannelElevation();
		}
		else	//trex reads channel network topology directly from file
		{
			//Record 17 (Part 1) (external channel network topology/property file)
			fscanf(inputfile_fp, "%s", varname);  //read dummy

			//Allocate memory for topology file name
			topologyfile = (char *)malloc(MAXNAMESIZE * sizeof(char));  //allocate memory

			//Record 17 (Part 2)
			fgets(topologyfile, MAXNAMESIZE, inputfile_fp);  //read topology file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			topologyfile = StripString(topologyfile);

			//echo topology file name to file
			fprintf(echofile_fp, "\n\nTopology File %s\n\n", topologyfile);

			//Read topology file (for future development)
			//mlv  ReadChannelTopologyFile();

		}  //end if tplgyopt = 1

		//Note:  If ksim = 1, transmission loss parameters are read
		//       here.  Otherwise, they are read with other sediment
		//       properties in Data Group C.
		//
		//Check value of ksim (if simulation type is hydrology only...)
		if(ksim == 1)
		{
			//if transmission loss is simulated (ctlopt > 0)
			if(ctlopt > 0)
			{
				//Record 18 (Part 1) (transmission loss parameter file)
				fscanf(inputfile_fp, "%s", varname);  //read dummy

				//Allocate memory for channel transmission loss property file name
				channeltlossfile = (char *)malloc(MAXNAMESIZE * sizeof(char));  //allocate memory

				//Record 18 (Part 2)
				fgets(channeltlossfile, MAXNAMESIZE, inputfile_fp);  //read channel transmission loss property file name

				//strip leading blanks, trailing blanks, and final carriage return from string
				channeltlossfile = StripString(channeltlossfile);

				//echo channel transmission loss property file name to file
				fprintf(echofile_fp, "\n\nChannel Transmission Loss Property File %s\n\n",
					channeltlossfile);

				//Read channel transmission loss property file
				ReadChannelTransmissionFile();

			}	//end if ctlopt > 0

		}	//end if ksim = 1

	}	//end if chnopt = 1

	//Record 19 (Part 1)
	fscanf(inputfile_fp, "%s", varname);  //read dummy

	//Allocate memory for initial waterdepth overland file name
	wateroverlandfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Record 19 (Part 2)
	//read initial water depth overland file name
	fgets(wateroverlandfile, MAXNAMESIZE, inputfile_fp);

	//strip leading blanks, trailing blanks, and final carriage return from string
	wateroverlandfile = StripString(wateroverlandfile);

	//Echo Initial Water Overland to file
	fprintf(echofile_fp, "\n\nInitial Water Depth Overland: %s\n", wateroverlandfile);

	//Read initial water in storage overland file
	ReadInitialWaterOverlandFile();

	//if infopt > 0, read initial soil moisture and infiltration (wetting front) depths
	if(infopt > 0)
	{
/*
		//Record 20 (Part 1)
		fscanf(inputfile_fp, "%s", varname);  //read dummy

		//Allocate memory for initial soil moisture file name
		soilmoisturefile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 20 (Part 2)
		//read initial soil moisture file name
		fgets(soilmoisturefile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, final carriage return from string
		soilmoisturefile = StripString(soilmoisturefile);

		//Echo initial soil moisture (fraction saturation) file name to file
		fprintf(echofile_fp, "\n\nInitial Soil Moisutre: %s\n", soilmoisturefile);

		//Read initial soil moisture (fraction saturation) file
		ReadInitialSoilMoistureFile();
*/
		//Record 20 (Part 1)
		fscanf(inputfile_fp, "%s", varname);  //read dummy

		//Allocate memory for initial infiltration depth (depth to wetting front) file name
		infiltrationfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 20 (Part 2)
		//read initial infiltration depth file name
		fgets(infiltrationfile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, final carriage return from string
		infiltrationfile = StripString(infiltrationfile);

		//Echo initial infiltration depth (depth to wetting front) file name to file
		fprintf(echofile_fp, "\n\nInitial Infiltration Depth: %s\n", infiltrationfile);

		//Read initial infiltration depth file
		ReadInitialInfiltrationFile();

	}  //end if infopt > 0

	//Check value of channel option
	if(chnopt == 1)
	{
		//Record 21 (Part 1)
		fscanf(inputfile_fp, "%s", varname);  //read dummy

		//Allocate memory for initial water depth in channels file name
		waterchannelfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 21 (Part 2)
		//read initial water depth in channel network file name
		fgets(waterchannelfile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, final carriage return from string
		waterchannelfile = StripString(waterchannelfile);

		//Echo Initial Water in Channels to file
		fprintf(echofile_fp, "\n\nInitial Water Depth in Channels: %s\n", waterchannelfile);

		//Read initial water depth in channel network file
		ReadInitialWaterChannelFile();

		//if transmission loss is simulated (ctlopt > 0)
		if(ctlopt > 0)
		{
			//Developer's Note:  Channel sediments are assumed to always
			//                   be 100% saturated (Se = 1).  If channel
			//                   transission loss in ephemeral streams is
			//                   important, the initial saturation of the
			//                   sediment bed would also be read in the
			//                   initial transloss file...
			//
			//Record 22 (Part 1)
			fscanf(inputfile_fp, "%s", varname);  //read dummy

			//Allocate memory for initial transmission loss file name
			translossfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Record 22 (Part 2)
			//read initial transmission loss depth (depth to wetting front) file name
			fgets(translossfile, MAXNAMESIZE, inputfile_fp);

			//strip leading blanks, trailing blanks, final carriage return from string
			translossfile = StripString(translossfile);

			//Echo Initial Water in Channels to file
			fprintf(echofile_fp, "\n\nInitial Transmission Loss Depth: %s\n", translossfile);

			//Read initial transmission loss depth file
			ReadInitialTransmissionLossFile();

		}	//end if ctlopt > 0

	}  //end check of chnopt

	//Write label for hydrologic forcing functions/boundary conditions
	fprintf(echofile_fp, "\n\n  Hydrologic Forcing Functions/Boundary Conditions  \n");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Initialize the number of rain gages (nrg)
	//
	//Note:  To suppress looping over rain gages, the number of
	//       rain gages in the simulation is initialized to zero.
	//       If a rainfall option requires that the user specify
	//       the number of gages, the value of nrg is read from
	//       input.
	//
	//set the number of rain gages to zero
	nrg = 0;

	//Rainfall options and functions....
	//
	//Write label for rainfall functions to file
	fprintf(echofile_fp, "\n  Rainfall Functions  \n");
	fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Record 23
	fscanf(inputfile_fp, "%s %d",  //read
		varname,  //dummy
		&rainopt);  //Rainfall Data Interpolation Option

	//Echo Rain Option to file
	fprintf(echofile_fp, "Rainfall Interpolation Option = %d\n\n", rainopt);

	//Check for valid rainfall interpolation/modeling option
	if(rainopt < 0 || rainopt > 5)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  Rainfall option < 0 or > 5 not valid!\n");
		fprintf(echofile_fp,       "  User selected rainopt = %5d\n", rainopt);
		fprintf(echofile_fp,       "  Select rainopt = 0,1,2,3,4 or 5");

		//Write error message to screen
		printf("\n\n\nData Group Error:\n");
		printf(      "  Rainfall option < 0 or > 5 not valid!\n");
		printf(      "  User selected rainopt = %5d\n", rainopt);
		printf(      "  Select rainopt = 0,1,2,3,4 or 5");

		exit(EXIT_FAILURE);  //abort

	}  //End check on rainopt < 0 or rainopt > 5

	//if IDW spatial interpolation selected (rainopt = 1)
	if(rainopt == 1)
	{
		//Record 24 (read IDW parameters)
		fscanf(inputfile_fp, "%s %f %s %f",  //read
			varname,  //dummy
			&idwradius,  //IDW radius of influence
			varname,  //dummy
			&idwexponent);  //IDW weighting exponent

		//Echo IDW parameters to file
		fprintf(echofile_fp, "\nInverse Distance Weighting (IDW) selected.\n");
		fprintf(echofile_fp, "\n  IDW Radius of Influence (m) = %f", idwradius);
		fprintf(echofile_fp, "\n  IDW Weighting Exponent = %f\n", idwexponent);

	}	//end if rainopt = 1

	//Read rain gage data for options 0, 1, and 2
	//
	//if rainopt <= 2 (uniform or distributed gage data)
	if(rainopt <= 2)
	{
		//Record 25
		fscanf(inputfile_fp, "%s %d",  //read
			varname,  //dummy
			&nrg);  //Number of Rain Gages

		//Echo number of rain gages to file
		fprintf(echofile_fp, "Number of Rain Gages = %d\n\n", nrg);

		//error check for rainopt = 0 (uniform in space) and nrg > 1
		if(rainopt == 0 && nrg > 1)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nData Group Error:\n");
			fprintf(echofile_fp,       "  Rainfall option = %5d\n", rainopt);
			fprintf(echofile_fp,       "  Number of Rain Gages = %5d\n", nrg);
			fprintf(echofile_fp,       "  Only one rain gage can be specified");

			//Write error message to screen
			printf("\n\n\nData Group Error:\n");
			printf(      "  Rainfall option = %5d\n", rainopt);
			printf(      "  Number of Rain Gages = %5d\n", nrg);
			printf(      "  Only one rain gage can be specified");

			exit(EXIT_FAILURE);  //abort

		}  //end rainopt = 0 and nrg > 1

	}	//end if rainopt <= 2

	//Read rain gage conversion and scale factors
	//
	//if rainopt > 2 or nrg > 0 (there are rain gages)
	if(rainopt > 2 || nrg > 0)
	{
		//Record 26
		fscanf(inputfile_fp, "%s %f %s %f %s %f",  //read
			varname,  //dummy
			&rainconvunits,  //units conversion factor for rainfall data
			varname,  //dummy
			&rainconvtime,  //time conversion factor for rainfall data
			varname,  //dummy
			&rainscale);  //scale factor for rainfall data

		//Echo converson and scale factors to file
		fprintf(echofile_fp, "Rainfall Units Conversion Factor = %f\n", rainconvunits);
		fprintf(echofile_fp, "Rainfall Time Conversion Factor = %f\n", rainconvtime);
		fprintf(echofile_fp, "Rainfall Scale Factor = %f\n\n", rainscale);

	}	//end if rainopt > 2 or nrg > 0

	//Read rain gage data for options 0, 1, and 2
	//
	//if rainopt <= 2 (uniform or distributed gage data)
	if(rainopt <= 2)
	{
		//if raingages are specified (nrg > 0)
		if(nrg > 0)
		{
			//Allocate memory for rain gage parameters
			//
			//rain gage id
			rgid = (int *)malloc((nrg+1) * sizeof(int));

			//Gage x coordinate (m)
			rgx = (float *)malloc((nrg+1) * sizeof(float));

			//Gage y coordinate (m)
			rgy = (float *)malloc((nrg+1) * sizeof(float));

			//number of pairs in rainfall time series
			nrpairs = (int *)malloc((nrg+1) * sizeof(int));

			//value of rainfall intensity (m/s)
			rfintensity = (float **)malloc((nrg+1) * sizeof(float *));

			//time break in rainfall intensity time series (days)
			rftime = (float **)malloc((nrg+1) * sizeof(float *));

			//Loop over number of rain gages (rainfall functions)
			for(i=1; i<=nrg; i++)
			{
				//Record 27
				fscanf(inputfile_fp, "%s %d %f %f %d",  //read
					varname,  //dummy
					&rgid[i],  //Gage number
					&rgx[i],  //Gage x coordinate (m)
					&rgy[i],  //Gage y coordinate (m)
					&nrpairs[i]);  //Number of Pairs

				//Echo Gage number, Gage X-Coord, Gage Y-Coord and Number of Pairs to file
				fprintf(echofile_fp, "\n\nRain Gage Number = %d\n", rgid[i]);
				fprintf(echofile_fp, "\n   Location:\n");
				fprintf(echofile_fp, "      x coordinate = %f\n", rgx[i]);
				fprintf(echofile_fp, "      y coordinate = %f\n", rgy[i]);
				fprintf(echofile_fp, "\n   Number of Pairs in Time Series = %d\n\n", nrpairs[i]);

				//Allocate remaining memory for rainfall parameters
				rfintensity[i] = (float *)malloc((nrpairs[i]+1) * sizeof(float));
				rftime[i] = (float *)malloc((nrpairs[i]+1) * sizeof(float));  

				//Write label for rainfall time series to file
				fprintf(echofile_fp, "  Rainfall Intensity (m/s)     ");  //Defacto units!!!
				fprintf(echofile_fp, "  Time (hours)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

				//Loop over rainfall intensity time series
				for(j=1; j <= nrpairs[i]; j++)
				{
					//Record 28
					fscanf(inputfile_fp, "%f %f",  //read
						&rfintensity[i][j],  //Rainfall Intensity
						&rftime[i][j]);  //Rainfall Intensity Time Break

					//Apply conversion and scale factors
					rfintensity[i][j] = rfintensity[i][j]
						* rainconvunits * rainconvtime * rainscale;

					//Echo Rainfall Intensity, Rainfall Intensity Time Step to file
					fprintf(echofile_fp, "%28.8f %18.3f\n", rfintensity[i][j], rftime[i][j]);

				}  //end loop over rainfall intensity time series

			}  //end loop over rain gages (rainfall functions)

		}  //end if nrg > 0

		//if rainopt = 2 (design storm)
		if(rainopt == 2)
		{
			//Record 29 (Part 1)
			fscanf(inputfile_fp, "%s", varname);	//read dummy

			//Allocate memory for design rain grid file name
			designraingridfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

			//Record 29 (Part 2) (read design rain grid file name)
			fgets(designraingridfile, MAXNAMESIZE, inputfile_fp);

			//strip leading blanks, trailing blanks, and final carriage return from string
			designraingridfile = StripString(designraingridfile);

			//Echo design rain grid file name to file
			fprintf(echofile_fp, "\n\nDesign Rain Grid: %s\n", designraingridfile);

			//Read design rain grid file
			ReadDesignRainGrid();

		}  //end if rainopt = 2
	}
	//else if rainopt = 3 (radar rainfall, radar grid does not match model grid)
	else if(rainopt == 3)
	{
		//Record 30 (Part 1)
		fscanf(inputfile_fp, "%s", varname);	//read dummy

		//Allocate memory for radar rainfall location file name
		radarlocationfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 30 (Part 2)
		//read radar rainfall location file name
		fgets(radarlocationfile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, and final carriage return from string
		radarlocationfile = StripString(radarlocationfile);

		//Echo radar rainfall location file name to file
		fprintf(echofile_fp, "\nRadar Rainfall Location File: %s\n", radarlocationfile);

		//Record 31 (Part 1)
		fscanf(inputfile_fp, "%s", varname);	//read dummy

		//Allocate memory for radar rain rate file name
		radarrainfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 31 (Part 2)
		//read radar rain file name
		fgets(radarrainfile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, and final carriage return from string
		radarrainfile = StripString(radarrainfile);

		//Echo radar rain rate file name to file
		fprintf(echofile_fp, "\nRadar Rain Rate File: %s\n", radarrainfile);

		//Record 32 (Part 1)
		fscanf(inputfile_fp, "%s", varname);	//read dummy

		//Allocate memory for radar verification file name
		radarverifyfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 32 (Part 2)
		//read radar verification file name
		fgets(radarverifyfile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, and final carriage return from string
		radarverifyfile = StripString(radarverifyfile);

		//Echo radar rain verification file name to file
		fprintf(echofile_fp, "\nRadar Rain Verification File: %s\n", radarverifyfile);

		//Inform/Remind User where Radar Data are Echo Printed
		fprintf(echofile_fp, "\nRadar Data are Echoed in Radar Rain Verification File\n");

		//Open radar rain verification file and write header line
		radarverifyfile_fp = fopen(radarverifyfile,"w");

		//if the radar rain verification file pointer is null, abort execution
		if(radarverifyfile_fp==NULL)
		{
			//Write message to file
			fprintf(echofile_fp, "Error! Can't create Radar Verification File:  %s\n",radarverifyfile);

			//Write message to screen
			printf("Error! Can't create Radar Verification File:  %s\n",radarverifyfile);

			//Write message to screen
			printf("Can't create Radar Verification File:  %s\n",radarverifyfile);
			exit(EXIT_FAILURE);	//abort

		}	//End if radar verification file is null

		//Echo radar rain verification file name to  file
		fprintf(radarverifyfile_fp, "\n\nRadar Verification File:  %s\n", radarverifyfile);

		//Echo radar rainfall location file name to file
		fprintf(radarverifyfile_fp, "\n\nRadar Rainfall Location File: %s\n", radarlocationfile);

		//Echo radar rain rate file name to file
		fprintf(radarverifyfile_fp, "\n\nRadar Rain Rate File: %s\n", radarrainfile);

		//Close radar rain verification file
		fclose(radarverifyfile_fp);

		//NOTE:  We actually read the location and rate files *after* reading 
		//       verification file.  This way we know where to send the echo
		//       output and later computations!
		//
		//Read radar rainfall location file
		ReadRadarRainLocations();

		//Read radar rain rate file
		ReadRadarRainRates(rainconvunits, rainconvtime, rainscale);
	}
	//else if rainopt = 4 (depth-area-duration/DAD, space-time storm)
	else if(rainopt == 4)
	{
		//Record 33 (Part 1)
		fscanf(inputfile_fp, "%s", varname);	//read dummy

		//Allocate memory for space-time storm file name
		spacetimestormfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 33 (Part 2)
		//read space-time storm file name
		fgets(spacetimestormfile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, and final carriage return from string
		spacetimestormfile = StripString(spacetimestormfile);

		//Echo space-time storm file name to file
		fprintf(echofile_fp, "\n\nSpace Time Storm File: %s\n", spacetimestormfile);

		//Read space-time storm file
		ReadSpaceTimeStorm(rainconvunits, rainconvtime, rainscale);

		//Record 34 (Part 1)
		fscanf(inputfile_fp, "%s", varname);	//read dummy

		//Allocate memory for DAD storm grid file name
		dadstormgridfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 34 (Part 2)
		//read DAD storm grid file name
		fgets(dadstormgridfile, MAXNAMESIZE, inputfile_fp);

		//strip leading blanks, trailing blanks, and final carriage return from string
		dadstormgridfile = StripString(dadstormgridfile);

		//Echo DAD storm grid file name to echo file
		fprintf(echofile_fp, "\n\nDAD Storm Grid File: %s\n", dadstormgridfile);
	}
	//else if rainopt = 5 (gridded radar rainfall, radar grid matches model grid)
	else if(rainopt == 5)
	{
		//Record 35
		fscanf(inputfile_fp, "%s %f",  //read
			varname,  //dummy
			&raingridfreq);  //time (frequency) at which rainfall grids are read

		//Echo rainfall grid file read frequency to file
		fprintf(echofile_fp, "Rainfall Grid Read Frequency = %f (hours)\n\n",
			raingridfreq);

		//Record 36 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for rainfall grid root file name
		raingridfileroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 36 (Part 2)
		fgets(raingridfileroot, MAXNAMESIZE, inputfile_fp);	//read name of rainfall grid root file (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		raingridfileroot = StripString(raingridfileroot);

		//echo rainfall grid root file name
		fprintf(echofile_fp, "\n\nRainfall Grid File Root Name:  %s\n", raingridfileroot);

	}  //end if rainopt = 1, 2, 3, 4, 5...

	//Snowfall...
	//
	//
	//Initialize the number of snow gages (nsg)
	//
	//Note:  To suppress looping over snow gages, the number of
	//       snow gages in the simulation is initialized to zero.
	//       If a snowfall option requires that the user specify
	//       the number of gages, the value of nsg is read from
	//       input.
	//
	//set the number of snow gages to zero
	nsg = 0;

	//Snowfall (and Snowmelt) options and functions....
	//
	//Write label for snowfall functions to file
	fprintf(echofile_fp, "\n\n\n  Snowfall Functions  \n");
	fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Record 37a (Part 1)
	fscanf(inputfile_fp, "%s %d %s %d",  //read
		varname,  //dummy
		&snowopt,  //Snowfall Data Interpolation Option
		varname,  //dummy
		&meltopt);  //Snowmelt Method Option

	//Echo Snowfall and Snowmelt Option and Snow Temperature to file
	fprintf(echofile_fp, "Snowfall Option (snowopt) = %d\n\n", snowopt);
	fprintf(echofile_fp, "Snowmelt Option (meltopt) = %d\n\n", meltopt);

	//if snowfall or snowmelt are simulated (snowopt > 0 or meltopt > 0)
	if(snowopt > 0 || meltopt > 0)
	{
		//Record 37b (Part 2)
		fscanf(inputfile_fp, "%s %f",  //read
			varname,  //dummy
			&tsnow);  //temperature for snowfall or snowmelt (degrees C)

		//Echo Snow Temperature to file
		fprintf(echofile_fp, "Temperature for Snowfall/Snowmelt (degrees C) = %f\n\n", tsnow);

	}	//end if snowopt > 0 or meltopt > 0

	//if snowmelt is simulated (meltopt > 0)
	if(meltopt > 0)
	{
		//Record 37c (Part 3)
		fscanf(inputfile_fp, "%s %f",  //read
			varname,  //dummy
			&latitude);  //domain latitude for snowmelt (decimal degrees)

		//Echo Domain Latitude for Snowmelt to file
		fprintf(echofile_fp, "Domain Latitude for Snowmelt (decimal degrees) = %f\n\n", latitude);

	}	//end if or meltopt > 0

	//Check for valid snowfall interpolation/modeling option
	if(snowopt < 0 || snowopt > 4)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  snowfall option < 0 or > 4 not valid!\n");
		fprintf(echofile_fp,       "  User selected snowopt = %5d\n", snowopt);
		fprintf(echofile_fp,       "  Select snowopt = 0,1,2,3, or 4");

		//Write error message to screen
		printf("\n\n\nData Group Error:\n");
		printf(      "  snowfall option < 0 or > 4 not valid!\n");
		printf(      "  User selected snowopt = %5d\n", snowopt);
		printf(      "  Select snowopt = 0,1,2,3, or 4");

		exit(EXIT_FAILURE);  //abort

	}  //End check on snowopt < 0 or > 4

	//Developer's Note:  The block of code below is a placeholder for
	//                   developing new snowfall options.  In general,
	//                   snowfall is parallel to rainfall but not all
	//                   options that exist for rainfall make sense
	//                   for snowfall...
	//
	//                   However, all code below was modified so that
	//                   rainfall variables are replaced by snowfall
	//                   variables.  These variables are not declared
	//                   or defined so the water header files must be
	//                   updated before this code can be used.
	//
	//if snowopt = 2 (IDW spatial interpolation selected)
	if(snowopt == 2)
	{
		//Record 38 (read snow IDW parameters)
		fscanf(inputfile_fp, "%s %f %s %f",  //read
			varname,  //dummy
			&sidwradius,  //snow IDW radius of influence
			varname,  //dummy
			&sidwexponent);  //snow IDW weighting exponent

		//Echo IDW parameters to file
		fprintf(echofile_fp, "\nInverse Distance Weighting (IDW) selected.\n");
		fprintf(echofile_fp, "\n  Snowfall IDW Radius of Influence (m) = %f", sidwradius);
		fprintf(echofile_fp, "\n  Snowfall IDW Weighting Exponent = %f\n", sidwexponent);

		//Record 39
		fscanf(inputfile_fp, "%s %d",  //read
			varname,  //dummy
			&nsg);  //Number of Snow Gages

		//Echo number of snow gages to file
		fprintf(echofile_fp, "Number of Snow Gages = %d\n\n", nsg);

		//error check for snowopt = 0 (uniform in space) and nsg > 1
		if(snowopt == 0 && nsg > 1)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nData Group Error:\n");
			fprintf(echofile_fp,       "  Snowfall option = %5d\n", snowopt);
			fprintf(echofile_fp,       "  Number of Snow Gages = %5d\n", nsg);
			fprintf(echofile_fp,       "  Only one snow gage can be specified");

			//Write error message to screen
			printf("\n\n\nData Group Error:\n");
			printf(      "  Snowfall option = %5d\n", snowopt);
			printf(      "  Number of Snow Gages = %5d\n", nsg);
			printf(      "  Only one snow gage can be specified");

			exit(EXIT_FAILURE);  //abort
		}  //end snowopt = 0 and nsg > 1

		//if snowgages are specified (nsg > 0)
		if(nsg > 0)
		{
			//Record 40
			fscanf(inputfile_fp, "%s %f %s %f %s %f",  //read
				varname,  //dummy
				&convunits,  //units conversion factor
				varname,  //dummy
				&convtime,  //time conversion factor
				varname,  //dummy
				&scale);  //scale factor

			//Echo converson and scale factors to file
			fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
			fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
			fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

			//Allocate memory for snow gage parameters
			//
			//snow gage id
			sgid = (int *)malloc((nsg+1) * sizeof(int));

			//Gage x coordinate (m)
			sgx = (float *)malloc((nsg+1) * sizeof(float));

			//Gage y coordinate (m)
			sgy = (float *)malloc((nsg+1) * sizeof(float));

			//number of pairs in snowfall time series
			nspairs = (int *)malloc((nsg+1) * sizeof(int));

			//value of snowfall intensity (as swe) (m/s)
			sfintensity = (float **)malloc((nsg+1) * sizeof(float *));

			//time break in snowfall intensity time series (days)
			sftime = (float **)malloc((nsg+1) * sizeof(float *));

			//Loop over number of snow gages (snowfall functions)
			for(i=1; i<=nsg; i++)
			{
				//Record 41
				fscanf(inputfile_fp, "%s %d %f %f %d",  //read
					varname,  //dummy
					&sgid[i],  //Gage number
					&sgx[i],  //Gage x coordinate (m)
					&sgy[i],  //Gage y coordinate (m)
					&nspairs[i]);  //Number of Pairs

				//Echo Gage number, Gage X-Coord, Gage Y-Coord and Number of Pairs to file
				fprintf(echofile_fp, "\n\nSnow Gage Number = %d\n", sgid[i]);
				fprintf(echofile_fp, "\n   Location:\n");
				fprintf(echofile_fp, "      x coordinate = %f\n", sgx[i]);
				fprintf(echofile_fp, "      y coordinate = %f\n", sgy[i]);
				fprintf(echofile_fp, "\n   Number of Pairs in Time Series = %d\n\n", nspairs[i]);

				//Allocate remaining memory for snowfall parameters
				sfintensity[i] = (float *)malloc((nspairs[i]+1) * sizeof(float));
				sftime[i] = (float *)malloc((nspairs[i]+1) * sizeof(float));  

				//Write label for snowfall time series to file
				fprintf(echofile_fp, "  Snowfall Intensity (SWE) (m/s)     ");  //Defacto units!!!
				fprintf(echofile_fp, "  Time (hours)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

				//Loop over snowfall intensity time series
				for(j=1; j <= nspairs[i]; j++)
				{
					//Record 42
					fscanf(inputfile_fp, "%f %f",  //read
						&sfintensity[i][j],  //Snowfall Intensity (as swe)
						&sftime[i][j]);  //Snowfall Intensity Time Break

					//Apply conversion and scale factors
					sfintensity[i][j] = sfintensity[i][j]
						* convunits * convtime * scale;

					//Echo Snowfall Intensity, Snowfall Intensity Time Step to file
					fprintf(echofile_fp, "%34.8f %18.3f\n", sfintensity[i][j], sftime[i][j]);

				}  //end loop over snowfall intensity time series

			}  //end loop over snow gages (snowfall functions)

		}  //end if nsg > 0
	}
	//else if snowopt = 3 (SNOTEL/SNODAS gages)
	else if(snowopt == 3)
	{
		//Warning: Option not yet implemented...
		//
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  snowfall option = 3 not implemented!\n");

		//Write error message to screen
		printf("\n\n\nData Group Error:\n");
		printf(      "  snowfall option = 3 not implemented!\n");

		exit(EXIT_FAILURE);  //abort
	}
	//else if snowopt = 4 (gridded radar snowfall)
	else if(snowopt == 4)
	{
		//Record 43
		fscanf(inputfile_fp, "%s %f",  //read
			varname,  //dummy
			&snowgridfreq);  //time (frequency) at which snowfall grids are read

		//Echo snowfall grid file read frequency to file
		fprintf(echofile_fp, "snowfall Grid Read Frequency = %f (hours)\n\n",
			snowgridfreq);

		//Record 44
		fscanf(inputfile_fp, "%s %f %s %f %s %f",  //read
			varname,  //dummy
			&snowconvunits,  //units conversion factor for snowfall grids
			varname,  //dummy
			&snowconvtime,  //time conversion factor for anowfall grids
			varname,  //dummy
			&snowscale);  //scale factor for snowfall grids

		//Echo converson and scale factors to file
		fprintf(echofile_fp, "Snowfall Grid Units Conversion Factor = %f\n", snowconvunits);
		fprintf(echofile_fp, "Snowfall Grid Time Conversion Factor = %f\n", snowconvtime);
		fprintf(echofile_fp, "Snowfall Grid Scale Factor = %f\n\n", snowscale);

		//Record 45 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for snowfall grid root file name
		snowgridfileroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 45 (Part 2)
		fgets(snowgridfileroot, MAXNAMESIZE, inputfile_fp);	//read name of snowfall grid root file (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		snowgridfileroot = StripString(snowgridfileroot);

		//echo snowfall grid root file name
		fprintf(echofile_fp, "\n\nSnowfall Grid File Root Name:  %s\n", snowgridfileroot);

	}  //end if snowopt = 2...

	//if meltopt > 0 (if snowmelt is simutated)
	if(meltopt > 0)
	{
		//Record 46
		fscanf(inputfile_fp, "%s %f %s %f",  //read
			varname,		//dummy
			&atmelt,		//air temperature index for snow melt (m/s/degrees C)
			varname,		//dummy
			&srmelt);		//solar radiation index for snow melt (m/s/(W/m2))

		//Echo air temperature and solar radiation melt factors to file
		fprintf(echofile_fp, "Air Temperature Index for Snowmelt = %f\n", atmelt);
		fprintf(echofile_fp, "Solar Radiation Index for Snowmelt = %f\n\n", srmelt);

		//Record 47 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for DEM slope file name
		slopefile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 47 (Part 2)
		fgets(slopefile, MAXNAMESIZE, inputfile_fp);	//read name of DEM slope file (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		slopefile = StripString(slopefile);

		//echo DEM slope file name
		fprintf(echofile_fp, "\n\nDEM Slope Grid File:  %s\n", slopefile);

		//Read DEM slope file
		ReadSlopeFile();

		//Record 48 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for DEM aspect file name
		aspectfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 48 (Part 2)
		fgets(aspectfile, MAXNAMESIZE, inputfile_fp);	//read name of DEM aspect file (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		aspectfile = StripString(aspectfile);

		//echo DEM aspect file name
		fprintf(echofile_fp, "\n\nDEM Aspect Grid File:  %s\n", aspectfile);

		//Read DEM slope file
		ReadAspectFile();

		//Record 49 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for DEM skyview file name
		skyviewfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 49 (Part 2)
		fgets(skyviewfile, MAXNAMESIZE, inputfile_fp);	//read name of DEM skyview file (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		skyviewfile = StripString(skyviewfile);

		//echo land use skyview file name
		fprintf(echofile_fp, "\n\nLand Use Skyview Grid File:  %s\n", skyviewfile);

		//Read landuse skyview file
		ReadSkyviewFile();

	}  //end if meltopt > 0

	//if snowfall or snowmelt are simulated (snowopt > 0 or meltopt > 0)
	if(snowopt > 0 || meltopt > 0)
	{
		//Record 50 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for snow (swe) initial condition file name
		snowfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 50 (Part 2)
		fgets(snowfile, MAXNAMESIZE, inputfile_fp);	//read name of snow initial condition file (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		snowfile = StripString(snowfile);

		//echo initial snow file name
		fprintf(echofile_fp, "\n\nInitial Snow (SWE) Depth File:  %s\n", snowfile);

		//Read initial snow depth (as swe) file
		ReadInitialSnowFile();

	}	//end if snowopt > 0 or meltopt > 0

	//Record 51
	fscanf(inputfile_fp, "%s %d",  //read
		varname,  //dummy
		&nqwov);  //Number of External Flows

	//Echo Number of External Overland Flows to file
	fprintf(echofile_fp, "\n\nNumber of External Overland Flows = %d\n\n", nqwov);

	//if external overland flows are specified (nqwov > 0)
	if(nqwov > 0)
	{
		//Record 52
		fscanf(inputfile_fp, "%s %f %s %f %s %f",	//read
			varname,  //dummy
			&convunits,  //units conversion factor
			varname,  //dummy
			&convtime,  //time conversion factor
			varname,  //dummy
			&scale);  //scale factor

		//Echo converson and scale factors to file
		fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
		fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
		fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

		//Allocate memory for external overland flow parameters
		//
		//flow source row number
		qwovrow = (int *)malloc((nqwov+1) * sizeof(int));
		//flow source column number
		qwovcol = (int *)malloc((nqwov+1) * sizeof(int));
		//number of pairs
		nqwovpairs = (int *)malloc((nqwov+1) * sizeof(int));
		//flow description
		qwovdescription = (char **)malloc((nqwov+1) * sizeof(char *));
		//external overland flow (m3/s)
		qwov = (float **)malloc((nqwov+1) * sizeof(float *));
		//time break in flow function (days)
		qwovtime = (float **)malloc((nqwov+1) * sizeof(float *));

		//Allocate remaining memory for overland flow parameters
		for(i=1; i<=nqwov; i++)
		{
			//flow description
			qwovdescription[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
		}

		//Loop over number of overland flow functions
		for(i=1; i<=nqwov; i++)
		{
			//Record 53 (Part 1)
			fscanf(inputfile_fp, "%d %d %d",  //read
				&qwovrow[i],  //flow source row number
				&qwovcol[i],  //flow source column number
				&nqwovpairs[i]);  //number of pairs in time series

			//Record 53 (Part 2)
			fgets(qwovdescription[i], MAXNAMESIZE, inputfile_fp);  //Read Flow Description

			//Echo Overland Flow Source ID, Number Of Pairs, and Description to file
			fprintf(echofile_fp, "\nOverland Flow Source ID = %d", i);
			fprintf(echofile_fp, "\nRow Number = %d", qwovrow[i]);
			fprintf(echofile_fp, "\nColumn Number = %d", qwovcol[i]);
			fprintf(echofile_fp, "\nNumber Of Time Breaks = %d", nqwovpairs[i]);
			fprintf(echofile_fp, "\nDescription: %s\n\n", qwovdescription[i]);

			//Allocate remaining memory for flow parameters
			qwov[i] = (float *)malloc((nqwovpairs[i]+1) * sizeof(float));  //external flow (m3/s)
			qwovtime[i] = (float *)malloc((nqwovpairs[i]+1) * sizeof(float));  //time break (hrs)

			//Write label for external flow time series to file
			fprintf(echofile_fp, "  Flow (m3/s)     ");
			fprintf(echofile_fp, "  Time (hours)  \n");
			fprintf(echofile_fp, "~~~~~~~~~~~~~~~   ");
			fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

			//Loop over the number of pairs for this series
			for(j=1; j <= nqwovpairs[i]; j++)
			{
				//Record 54
				fscanf(inputfile_fp, "%f %f",  //read
					&qwov[i][j],  //External Overland Flow (m3/s)
					&qwovtime[i][j]);  //time break

				//Apply conversion and scale factors
				qwov[i][j] = qwov[i][j] * convunits * convtime * scale;

				//Echo external flow and time break to file
				fprintf(echofile_fp, "%15.6f %18.3f\n", qwov[i][j], qwovtime[i][j]);

			}  //end loop over number of pairs in time series

		}  //end loop over number of flow functions

	}  //end if nqwov > 0

	//Check value of chnopt (channel option)
	if(chnopt == 1)
	{
		//Record 55
		fscanf(inputfile_fp, "%s %d",  //read
			varname,  //dummy
			&nqwch);  //Number of External Flows

		//Echo Number of External Channel Flows to file
		fprintf(echofile_fp, "\n\nNumber of External Channel Flows = %d\n\n", nqwch);

		//if external flows are specified (nqwch > 0)
		if(nqwch > 0)
		{
			//Record 56
			fscanf(inputfile_fp, "%s %f %s %f %s %f",  //read
				varname,  //dummy
				&convunits,  //units conversion factor
				varname,  //dummy
				&convtime,  //time conversion factor
				varname,  //dummy
				&scale);  //scale factor

			//Echo converson and scale factors to file
			fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
			fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
			fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

			//Allocate memory for external channel flow parameters

			//flow source link number
			qwchlink = (int *)malloc((nqwch+1) * sizeof(int));
			//flow source node number
			qwchnode = (int *)malloc((nqwch+1) * sizeof(int));
			//number of pairs
			nqwchpairs = (int *)malloc((nqwch+1) * sizeof(int));
			//flow description
			qwchdescription = (char **)malloc((nqwch+1) * sizeof(char *));
			//external channel flow (m3/s)
			qwch = (float **)malloc((nqwch+1) * sizeof(float *));
			//time break in flow function (days)
			qwchtime = (float **)malloc((nqwch+1) * sizeof(float *));

			//Allocate remaining memory for flow description
			for(i=1; i<=nqwch; i++)
			{
				qwchdescription[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));
			}

			//Loop over number of flow functions
			for(i=1; i<=nqwch; i++)
			{
				//Record 57 (Part 1)
				fscanf(inputfile_fp, "%d %d %d",  //read
					&qwchlink[i],  //flow source link number
					&qwchnode[i],  //flow source node number
					&nqwchpairs[i]);  //number of pairs in time series

				//Record 57 (Part 2)
				fgets(qwchdescription[i], MAXNAMESIZE, inputfile_fp);  //Read Flow Description

				//Echo Flow Source ID, Number Of Pairs, and Description  to file
				fprintf(echofile_fp, "\nFlow Source ID = %d", i);
				fprintf(echofile_fp, "\nLink Number = %d", qwchlink[i]);
				fprintf(echofile_fp, "\nNode Number = %d", qwchnode[i]);
				fprintf(echofile_fp, "\nNumber Of Time Breaks = %d", nqwchpairs[i]);
				fprintf(echofile_fp, "\nDescription: %s\n\n", qwchdescription[i]);

				//Allocate remaining memory for flow parameters

				//external channel flow (m3/s)
				qwch[i] = (float *)malloc((nqwchpairs[i]+1) * sizeof(float));
				//time break (hrs)
				qwchtime[i] = (float *)malloc((nqwchpairs[i]+1) * sizeof(float));

				//Write label for external channel flow time series to file
				fprintf(echofile_fp, "  Flow (m3/s)     ");
				fprintf(echofile_fp, "  Time (hours)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~   ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

				//Loop over the number of pairs for this series
				for(j=1; j <= nqwchpairs[i]; j++)
				{
					//Record 58
					fscanf(inputfile_fp, "%f %f",  //read
						&qwch[i][j],  	//external channel flow (m3/s)
						&qwchtime[i][j]);  //time break

					//Apply conversion and scale factors
					qwch[i][j] = qwch[i][j] * convunits * convtime * scale;

					//Echo external flow and time break to file
					fprintf(echofile_fp, "%15.6f %18.3f\n", qwch[i][j], qwchtime[i][j]);

				}  //end loop over number of pairs in time series

			}  //end loop over number of flow functions

		}  //end if nqwch > 0

	}  //end if chnopt = 1

	//Write label for outlets (boundaries) to file
	fprintf(echofile_fp, "\n\n\n  Domain Outlet/Boundary Characteristics  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 59
	fscanf(inputfile_fp, "%s %d",  //read
		varname,  //dummy
		&noutlets);  //Number of Outlet Cells

	//Echo number of outlets to file
	fprintf(echofile_fp, "\nNumber of Domain Outlets: %d\n", noutlets);
	fprintf(echofile_fp, "\n\nOutlet     Row   Column  Overland Slope  dbcopt");
	fprintf(echofile_fp,   "\n------    -----  ------  --------------  ------\n");

	//Allocate memory for outlet cell properties

	iout = (int *)malloc((noutlets+1) * sizeof(int));  //outlet cell row
	jout = (int *)malloc((noutlets+1) * sizeof(int));  //outlet cell column
	sovout = (float *)malloc((noutlets+1) * sizeof(float));  //outlet cell ground slope
	dbcopt = (int *)malloc((noutlets+1) * sizeof(int));  //domain boundary condition option

	//if channels are simulated (chnopt = 1)
	if(chnopt == 1)
	{
		//Allocate memory for channel boundary properties
		qchoutlet = (int *)malloc((nlinks+1) * sizeof(int));  //outlet for channel link

		//Allocate memnory for bed slope at channel outlets
		schout = (float *)malloc((nlinks+1) * sizeof(float));	//slope at boundary of channel outlet

		//Initialize the channel link outlet vector...
		//
		//loop over links
		for(i=1; i<=nlinks; i++)
		{
			//initialize the outlet value to zero
			qchoutlet[i] = 0;

		}  //end loop over links

	}  //end if chnopt = 1

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//Record 60
		fscanf(inputfile_fp, "%s %d %d %f %d",  //read
			varname,  //dummy
			&iout[i],  //Outlet cell row location
			&jout[i],  //Outlet cell column location
			&sovout[i],  //Outlet cell overland slope
			&dbcopt[i]);  //Domain boundary condition option

		//Echo Outlet Cell ROW, COLUMN and SLOPE to file
		fprintf(echofile_fp, "%6d  %5d  %6d  %14.6f  %6d\n",
			i, iout[i], jout[i], sovout[i], dbcopt[i]);

		//The link number of the outlet is:
		//
		//  outlink = link[iout[i]][jout[i]];
		//
		//if channels are simulated (chnopt > 0) and the
		//outlet cell is a channel cell (imask > 1) and the
		//node is the last node of the link (node = nnodes)
		if(chnopt > 0 && imask[iout[i]][jout[i]] > 1
			&& node[iout[i]][jout[i]] == nnodes[link[iout[i]][jout[i]]])
		{
			//store the outlet number corresponding to this link
			qchoutlet[link[iout[i]][jout[i]]] = i;

			//set channel bed slope for this outlet
			schout[link[iout[i]][jout[i]]] = sovout[i];

		}  //end if chnopt > 0 and imask > 1...

		//if the domain boundary condition option = 1
		if(dbcopt[i] == 1)
		{
			//Record 61
			fscanf(inputfile_fp, "%s %f %s %f %s %f",  //read
				varname,  //dummy
				&convunits,  //units conversion factor
				varname,  //dummy
				&convtime,  //time conversion factor
				varname,  //dummy
				&scale);  //scale factor

			//Echo converson and scale factors to file
			fprintf(echofile_fp, "\nUnits Conversion Factor = %f\n", convunits);
			fprintf(echofile_fp,   "Time Conversion Factor = %f\n", convtime);
			fprintf(echofile_fp,   "Scale Factor = %f\n\n", scale);

			//Allocate memory for outlet boundary condition parameters
			//number of pairs
			nhbcpairs = (int *)malloc((noutlets+1) * sizeof(int));
			//bc description
			hbcdescription = (char **)malloc((noutlets+1) * sizeof(char *));
			//bc value (water depth) (m)
			hbc = (float **)malloc((noutlets+1) * sizeof(float *));
			//time break in bc function (days)
			hbctime = (float **)malloc((noutlets+1) * sizeof(float *));

			//Allocate remaining memory for overland bc parameters
			for(i=1; i<=noutlets; i++)
			{
				hbcdescription[i] = (char *)malloc(MAXNAMESIZE * sizeof(char)); //bc description
			}

			//Record 62 (Part 1)
			fscanf(inputfile_fp, "%d",  //read
				&nhbcpairs[i]);  //number of pairs in time series

			//Record 62 (Part 2)
			fgets(hbcdescription[i], MAXNAMESIZE, inputfile_fp);  //Reads bc Description

			//Echo domain bc characteristics and description to file
			fprintf(echofile_fp, "\nDomain Boundary = %d", i);
			fprintf(echofile_fp, "\nRow Number = %d", iout[i]);
			fprintf(echofile_fp, "\nColumn Number = %d", jout[i]);
			fprintf(echofile_fp, "\nNumber Of Time Breaks = %d", nhbcpairs[i]);
			fprintf(echofile_fp, "\nDescription: %s\n\n", hbcdescription[i]);

			//Allocate remaining memory for bc parameters
			hbc[i] = (float *)malloc((nhbcpairs[i]+1) * sizeof(float));  //bc (water depth) (m)
			hbctime[i] = (float *)malloc((nhbcpairs[i]+1) * sizeof(float));  //time break (hrs)

			//Write label for domain bc time series to file
			fprintf(echofile_fp, "  Depth (m)     ");
			fprintf(echofile_fp, "  Time (hours)  \n");
			fprintf(echofile_fp, "~~~~~~~~~~~~~   ");
			fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

			//Loop over the number of pairs for this series
			for(j=1; j<=nhbcpairs[i]; j++)
			{
				//Record 63
				fscanf(inputfile_fp, "%f %f",  //read
					&hbc[i][j],  //domain BC (m)
					&hbctime[i][j]);  //time break (hrs)

				//Apply conversion and scale factors
				hbc[i][j] = hbc[i][j] * convunits * convtime * scale;

				//Echo bc and time break to file
				fprintf(echofile_fp, "%15.6f %18.3f\n", hbc[i][j], hbctime[i][j]);

			}  //end loop over number of pairs in time series

		}  //end if dbcopt[i] = 1

	}  //end loop over outlets

	//Record 64
	fscanf(inputfile_fp, "%s %d",  //read
		varname,  //dummy
		&nqreports);  //Number of Flow Reporting Stations

	//Echo Number of Flow Reporting Stations to file
	fprintf(echofile_fp, "\n\n\nNumber of Flow Reporting Stations = %d\n", nqreports);

	//Allocate memory for flow reporting stations
	qreprow = (int *)malloc((nqreports+1) * sizeof(int));  //reporting station cell row
	qrepcol = (int *)malloc((nqreports+1) * sizeof(int));  //reporting station cell column
	qarea = (float *)malloc((nqreports+1) * sizeof(float));  //reporting station cell drainage area
	qunitsopt = (int *)malloc((nqreports+1) * sizeof(int));  //flow report units option

	//Write label for flow reports to file
	fprintf(echofile_fp, "\n  Station     ");
	fprintf(echofile_fp,   "  Cell Row     ");
	fprintf(echofile_fp,   "  Cell Column     ");
	fprintf(echofile_fp,   "  Drainage Area (km2)     ");
	fprintf(echofile_fp,   "  Report Units Option  ");
	fprintf(echofile_fp,   "  Station Name  \n");
	fprintf(echofile_fp,   "~~~~~~~~~~~   ");
	fprintf(echofile_fp,   "~~~~~~~~~~~~   ");
	fprintf(echofile_fp,   "~~~~~~~~~~~~~~~   ");
	fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~   ");
	fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~~~~~~~~  ");
	fprintf(echofile_fp,   "~~~~~~~~~~~~~~~~\n");

	//loop over number of flow reporting stations
	for(i=1; i<=nqreports; i++)
	{
		//Record 65
		fscanf(inputfile_fp, "%d %d %f %d %s",  //read
			&qreprow[i],		//reporting station cell row
			&qrepcol[i],		//reporting station cell column
			&qarea[i],			//reporting station cell (upstream) drainage Area
			&qunitsopt[i],		//flow report units option
			varname);			//station identifier

		//Echo Station (loop counter), Row, Column, Area, and Units Option to file
		fprintf(echofile_fp, "%11d   %12d   %15d   %22.3f   %23d %s\n",
			i, qreprow[i], qrepcol[i], qarea[i], qunitsopt[i], varname);

	}  //end loop over number of reports

	//Close the Echo File
	fclose(echofile_fp);

//End of function: Return to ReadInputFile
}
