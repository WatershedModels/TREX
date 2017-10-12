/*---------------------------------------------------------------------
C-  Function:	ReadDataGroupF.c
C- 
C-  Purpose/    ReadDataGroupF is called at the start of the simulation
C-  Methods:    to read Data Group F (output controls) from the
C-              model input file.                    
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   ksim, ssrflag
C-
C-  Calls:      None
C-
C-  Called by:  ReadInputFile
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
C-	Date:		20-OCT-2003
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       23-AUG-2010
C-
C-  Revisions:  Implemented subsurface reporting.  Added records to
C-              report chemical concentrations in subsurface soil
C-              and sediment stack layers.
C-
C-	Revised:
C-
C-	Date:	
C-
C-	Revisions:
C-
C----------------------------------------------------------------------*/

//trex general variable declarations
#include "trex_general_declarations.h"

//trex global variable declarations for water transport
#include "trex_water_declarations.h"

//trex global variable declarations for sediment transport
#include "trex_solids_declarations.h"

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void ReadDataGroupF()
{
	//Local variable declarations/definitions
	int
		ssrflag;	//subsurface reporting flag (0 = no reporting, 1 = reporting on)

	//initize the subsurface reporting flag (set to no reporting/off)
	ssrflag = 0;

	//open the echo file in append mode (append to existing file)
	echofile_fp = fopen(echofile, "a");

	//write message to screen
	printf("\n\n***************************\n");
	printf(	   "*                         *\n");
	printf(	   "*   Reading Data Group F  *\n");
	printf(	   "*                         *\n");
	printf(	   "***************************\n\n\n");

	//Some housekeeping...
	//
	//Because the last field of the last record input was read using
	//fscanf and the next record input is read using fgets, we need to
	//skip past a carriage return left in the input file.
	//
	//The fscanf function does not read carriage returns (\n) at the
	//ends of records.  The fgets function is keyed to read all
	//characters up to and including the the first carriage return
	//encountered.  So, before we can read using fgets, we have to
	//skip the intervening carriage return character.
	//
	//Anytime fscan is followed by fgets, the extra carriage return
	//problem will occur.  To skip this extra character, read it
	//using fgets.  This is noted as "housekeeping" in the comments.
	//
	//Housekeeping: skip extra carriage return
	fgets(header, MAXHEADERSIZE, inputfile_fp);

	//Record 1
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s\n", header);

	//Record 2 (Header for grid outputs)
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s\n", header);

	if(nqreports > 0)
	{
		//Record 3 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for water expport file name
		waterexpfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 3 (Part 2)
		fgets(waterexpfile, MAXNAMESIZE, inputfile_fp);	//read name of water expport file (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		waterexpfile = StripString(waterexpfile);

		//echo water time export series file name
		fprintf(echofile_fp, "\n\nWater Export Time Series File Name:  %s\n", waterexpfile);

	}	//end if nqreports > 0

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//if the number of sediment reporting stations > 0
		if(nsedreports > 0)
		{
			//Record 4 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for sediment export file name
			sedexpfileroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 4 (Part 2)
			fgets(sedexpfileroot, MAXNAMESIZE, inputfile_fp);	//read name of sediment export root file (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			sedexpfileroot = StripString(sedexpfileroot);

			//echo sediment export time series root file name
			fprintf(echofile_fp, "\n\nSediment Export Time Series Root File Name:  %s\n", sedexpfileroot);

			//Record 5 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for sediment export file name extension
			sedextension = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 5 (Part 2)
			fgets(sedextension, MAXNAMESIZE, inputfile_fp);	//read sediment export file name extension (.exp)

			//strip leading blanks, trailing blanks, and final carriage return from string
			sedextension = StripString(sedextension);

			//echo sediment export file name extension
			fprintf(echofile_fp, "\n\nSediment Export Time Series File Name Extension:  %s\n", sedextension);

		}	//end if nsedreports > 0

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//if the number of chemical reporting stations > 0
			if(nchemreports > 0)
			{
				//Record 6 (Part 1)
				fscanf(inputfile_fp, "%s",	//read
					varname);				//dummy

				//Allocate memory for chemical export file name
				chemexprootfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

				//Record 6 (Part 2)
				fgets(chemexprootfile, MAXNAMESIZE, inputfile_fp);	//read name of chemical export root file (including any path)

				//strip leading blanks, trailing blanks, and final carriage return from string
				chemexprootfile = StripString(chemexprootfile);

				//echo chemical export time series root file name
				fprintf(echofile_fp, "\n\nChemical Export Time Series Root File Name:  %s\n", chemexprootfile);

				//Record 7 (Part 1)
				fscanf(inputfile_fp, "%s",	//read
					varname);				//dummy

				//Allocate memory for chemical export file name extension
				chemextension = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

				//Record 7 (Part 2)
				fgets(chemextension, MAXNAMESIZE, inputfile_fp);	//read chemical export file extension (.exp)

				//strip leading blanks, trailing blanks, and final carriage return from string
				chemextension = StripString(chemextension);

				//echo chemical export time series root file name
				fprintf(echofile_fp, "\n\nChemical Export Time Series File Name Extension:  %s\n", chemextension);

			}	//end if nchemreports > 0

		}	//end if ksim > 2

	}	//end if ksim > 1

	//Record 8 (Header for grid outputs)
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s\n", header);

	//Record 9 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for rainfall rate grid name
	rainrategrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 9 (Part 2)
	fgets(rainrategrid, MAXNAMESIZE, inputfile_fp);	//read name for rain rate grids (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	rainrategrid = StripString(rainrategrid);

	//echo rain rate grid name
	fprintf(echofile_fp, "\n\nRainfall Rate Grid File Name:  %s\n", rainrategrid);

	//Record 10 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for rainfall depth grid name
	raindepthgrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 10 (Part 2)
	fgets(raindepthgrid, MAXNAMESIZE, inputfile_fp);	//read name for rain depth grids (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	raindepthgrid = StripString(raindepthgrid);

	//echo rain depth grid name
	fprintf(echofile_fp, "\n\nRainfall Depth Grid File Name:  %s\n", raindepthgrid);

	//Record 11 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for infiltration rate grid name
	infrategrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 11 (Part 2)
	fgets(infrategrid, MAXNAMESIZE, inputfile_fp);	//read name for infiltration rate grids (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	infrategrid = StripString(infrategrid);

	//echo infiltration rate grid name
	fprintf(echofile_fp, "\n\nInfiltration Rate Grid File Name:  %s\n", infrategrid);

	//Record 12 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for infiltration depth grid name
	infdepthgrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 12 (Part 2)
	fgets(infdepthgrid, MAXNAMESIZE, inputfile_fp);	//read name for infiltration depth grids (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	infdepthgrid = StripString(infdepthgrid);

	//echo infiltration depth grid name
	fprintf(echofile_fp, "\n\nInfiltration Depth Grid File Name:  %s\n", infdepthgrid);

	//Record 13 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for water discharge grid name
	qgrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 13 (Part 2)
	fgets(qgrid, MAXNAMESIZE, inputfile_fp);	//read name for water discharge grids (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	qgrid = StripString(qgrid);

	//echo water discharge grid name
	fprintf(echofile_fp, "\n\nWater Discharge Grid File Name:  %s\n", qgrid);

	//Record 14 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for water depth grid name
	waterdepthgrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 14 (Part 2)
	fgets(waterdepthgrid, MAXNAMESIZE, inputfile_fp);	//read name for water depth grids (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	waterdepthgrid = StripString(waterdepthgrid);

	//echo water depth grid name
	fprintf(echofile_fp, "\n\nWater Depth Grid File Name:  %s\n", waterdepthgrid);

	//if snowfall > 1 (snowfall simulated using snow gage data)
	if(snowopt > 1)
	{
		//Record 15 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for snowfall rate grid name
		swefallrategrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 15 (Part 2)
		fgets(swefallrategrid, MAXNAMESIZE, inputfile_fp);	//read name for snowfall rate grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		swefallrategrid = StripString(swefallrategrid);

		//echo snowfall rate grid name
		fprintf(echofile_fp, "\n\nSnowfall (as SWE) Rate Grid File Name:  %s\n", swefallrategrid);

		//Record 16 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for snowfall depth grid name
		swefalldepthgrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 16 (Part 2)
		fgets(swefalldepthgrid, MAXNAMESIZE, inputfile_fp);	//read name for snowfall depth grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		swefalldepthgrid = StripString(swefalldepthgrid);

		//echo snowfall depth grid name
		fprintf(echofile_fp, "\n\nSnowfall (as SWE) Depth Grid File Name:  %s\n", swefalldepthgrid);

	}	//end if snowopt > 1

	//if snowmelt is simulated
	if(meltopt > 0)
	{
		//Record 17 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for snowmelt rate grid name
		swemeltrategrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 17 (Part 2)
		fgets(swemeltrategrid, MAXNAMESIZE, inputfile_fp);	//read name for snowmelt rate grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		swemeltrategrid = StripString(swemeltrategrid);

		//echo snowmelt rate grid name
		fprintf(echofile_fp, "\n\nSnowmelt (as SWE) Rate Grid File Name:  %s\n", swemeltrategrid);

		//Record 18 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for snowmelt depth grid name
		swemeltdepthgrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 18 (Part 2)
		fgets(swemeltdepthgrid, MAXNAMESIZE, inputfile_fp);	//read name for snowmelt depth grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		swemeltdepthgrid = StripString(swemeltdepthgrid);

		//echo snowmelt depth grid name
		fprintf(echofile_fp, "\n\nSnowmelt (as SWE) Depth Grid File Name:  %s\n", swemeltdepthgrid);

	}	//end if meltopt > 0

	//if snowfall or snowmelt is simulated
	if(snowopt > 0 || meltopt > 0)
	{
		//Record 19 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for snow depth grid name
		swedepthgrid = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 19 (Part 2)
		fgets(swedepthgrid, MAXNAMESIZE, inputfile_fp);	//read name for snow depth grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		swedepthgrid = StripString(swedepthgrid);

		//echo snow depth grid name
		fprintf(echofile_fp, "\n\nSnow Depth (as SWE) Grid File Name:  %s\n", swedepthgrid);

	}	//end if snowopt > 0 or meltopt > 0

	//if sediment transport is simulated
	if(ksim > 1)
	{
		//Record 20 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for solids water column concentration grid root name
		solidsconcwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 20 (Part 2)
		fgets(solidsconcwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column solids concentration grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		solidsconcwatergridroot = StripString(solidsconcwatergridroot);

		//echo solids water column concentration grid root name
		fprintf(echofile_fp,
			"\n\nWater Column Solids Concentration Grid Root Name:  %s\n",
			solidsconcwatergridroot);

		//Record 21 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for surface sediment solids concentration grid root name
		solidsconcsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 21 (Part 2)
		fgets(solidsconcsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment solids concentration grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		solidsconcsurfgridroot = StripString(solidsconcsurfgridroot);

		//echo surface sediment solids concentration grid root name
		fprintf(echofile_fp,
			"\n\nSurface Sediment Solids Concentration Grid Root Name:  %s\n",
			solidsconcsurfgridroot);

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Record 22 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column total concentration grid root name
			totchemconcwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 22 (Part 2)
			fgets(totchemconcwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column total chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			totchemconcwatergridroot = StripString(totchemconcwatergridroot);

			//echo chemical water column total concentration grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Total Chemical Concentration Grid Root Name:  %s\n",
				totchemconcwatergridroot);

			//Record 23 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column dissolved concentration grid root name
			dischemconcwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 23 (Part 2)
			fgets(dischemconcwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column dissolved chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			dischemconcwatergridroot = StripString(dischemconcwatergridroot);

			//echo chemical water column dissolved concentration grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Dissolved Chemical Concentration Grid Root Name:  %s\n",
				dischemconcwatergridroot);

			//Record 24 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column bound concentration grid root name
			bndchemconcwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 24 (Part 2)
			fgets(bndchemconcwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column bound chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			bndchemconcwatergridroot = StripString(bndchemconcwatergridroot);

			//echo chemical water column bound concentration grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Bound Chemical Concentration Grid Root Name:  %s\n",
				bndchemconcwatergridroot);

			//Record 25 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column particulate concentration grid root name
			prtchemconcwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 25 (Part 2)
			fgets(prtchemconcwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column particulate chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			prtchemconcwatergridroot = StripString(prtchemconcwatergridroot);

			//echo chemical water column particulate concentration grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Particulate Chemical Concentration Grid Root Name:  %s\n",
				prtchemconcwatergridroot);

			//Record 26 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column sorbed concentration grid root name
			srbchemconcwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 26 (Part 2)
			fgets(srbchemconcwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column sorbed chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			srbchemconcwatergridroot = StripString(srbchemconcwatergridroot);

			//echo chemical water column sorbed concentration grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Sorbed Chemical Concentration Grid Root Name:  %s\n",
				srbchemconcwatergridroot);

			//Record 27 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment total chemical concentration grid root name
			totchemconcsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 27 (Part 2)
			fgets(totchemconcsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment total chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			totchemconcsurfgridroot = StripString(totchemconcsurfgridroot);

			//echo surface sediment total chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Chemical Total Concentration Grid Root Name:  %s\n",
				totchemconcsurfgridroot);

			//Record 28 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment disslved chemical concentration grid root name
			dischemconcsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 28 (Part 2)
			fgets(dischemconcsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment dissolved chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			dischemconcsurfgridroot = StripString(dischemconcsurfgridroot);

			//echo surface sediment dissolved chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Chemical Dissolved Concentration Grid Root Name:  %s\n",
				dischemconcsurfgridroot);

			//Record 29 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment bound chemical concentration grid root name
			bndchemconcsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 29 (Part 2)
			fgets(bndchemconcsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment bound chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			bndchemconcsurfgridroot = StripString(bndchemconcsurfgridroot);

			//echo surface sediment bound chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Chemical Bound Concentration Grid Root Name:  %s\n",
				bndchemconcsurfgridroot);

			//Record 30 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment particulate chemical concentration grid root name
			prtchemconcsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 30 (Part 2)
			fgets(prtchemconcsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment particulate chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			prtchemconcsurfgridroot = StripString(prtchemconcsurfgridroot);

			//echo surface sediment particulate chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Chemical Particulate Concentration Grid Root Name:  %s\n",
				prtchemconcsurfgridroot);

			//Record 31 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment sorbed chemical concentration grid root name
			srbchemconcsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 31 (Part 2)
			fgets(srbchemconcsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment sorbed chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			srbchemconcsurfgridroot = StripString(srbchemconcsurfgridroot);

			//echo surface sediment sorbed chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Chemical Sorbed Concentration Grid Root Name:  %s\n",
				srbchemconcsurfgridroot);

			//Record 32 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for subsurface sediment total chemical concentration grid root name
			totchemconcsubgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 32 (Part 2)
			fgets(totchemconcsubgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for subsurface sediment total chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			totchemconcsubgridroot = StripString(totchemconcsubgridroot);

			//echo subsurface sediment total chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSubsurface Sediment Chemical Total Concentration Grid Root Name:  %s\n",
				totchemconcsubgridroot);

			//if the total chemical grid file name is not null, raise the subsurface reporting flag
			if(*totchemconcsubgridroot != '\0') ssrflag = 1;

			//Record 33 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for subsurface sediment disslved chemical concentration grid root name
			dischemconcsubgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 33 (Part 2)
			fgets(dischemconcsubgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for subsurface sediment dissolved chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			dischemconcsubgridroot = StripString(dischemconcsubgridroot);

			//echo subsurface sediment dissolved chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSubsurface Sediment Chemical Dissolved Concentration Grid Root Name:  %s\n",
				dischemconcsubgridroot);

			//if the dissolved chemical grid file name is not null, raise the subsurface reporting flag
			if(*dischemconcsubgridroot != '\0') ssrflag = 1;

			//Record 34 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for subsurface sediment bound chemical concentration grid root name
			bndchemconcsubgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 34 (Part 2)
			fgets(bndchemconcsubgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for subsurface sediment bound chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			bndchemconcsubgridroot = StripString(bndchemconcsubgridroot);

			//echo subsurface sediment bound chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSubsurface Sediment Chemical Bound Concentration Grid Root Name:  %s\n",
				bndchemconcsubgridroot);

			//if the bound chemical grid file name is not null, raise the subsurface reporting flag
			if(*bndchemconcsubgridroot != '\0') ssrflag = 1;

			//Record 35 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for subsurface sediment particulate chemical concentration grid root name
			prtchemconcsubgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 35 (Part 2)
			fgets(prtchemconcsubgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for subsurface sediment particulate chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			prtchemconcsubgridroot = StripString(prtchemconcsubgridroot);

			//echo subsurface sediment particulate chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSubsurface Sediment Chemical Particulate Concentration Grid Root Name:  %s\n",
				prtchemconcsubgridroot);

			//if the particulate chemical grid file name is not null, raise the subsurface reporting flag
			if(*prtchemconcsubgridroot != '\0') ssrflag = 1;

			//Record 36 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for subsurface sediment sorbed chemical concentration grid root name
			srbchemconcsubgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 36 (Part 2)
			fgets(srbchemconcsubgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for subsurface sediment sorbed chemical concentration grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			srbchemconcsubgridroot = StripString(srbchemconcsubgridroot);

			//echo subsurface sediment sorbed chemical concentration grid root name
			fprintf(echofile_fp,
				"\n\nSubsurface Sediment Chemical Sorbed Concentration Grid Root Name:  %s\n",
				srbchemconcsubgridroot);

			//if the sorbed chemical grid file name is not null, raise the subsurface reporting flag
			if(*srbchemconcsubgridroot != '\0') ssrflag = 1;

			//if the subsurface reporting flag is raised (ssrflag > 0)
			if(ssrflag > 0)
			{
				//Record 37
				fscanf(inputfile_fp, "%s %f %s %d",	//read
					varname,						//dummy
					&horizon,						//horizon (depth below grade) for subsurface reporting (m)
					varname,						//dummy
					&ssropt);						//subsurface reporting option

				//echo subsurface reporting horizon to file
				fprintf(echofile_fp,
					"\n\nSubsurface Sediment Layer Reporting Horizon (Depth Below Grade) (m):  %f\n",
					horizon);

				//echo subsurface reporting option to file
				fprintf(echofile_fp,
					"\n\nSubsurface Sediment Layer Reporting Option:  %d\n",
					ssropt);
			}
			else	//else ssrflag is not greater than zero
			{
				//set the subsurface reporting option to zero
				ssropt = 0;

			}	//end of ssrflag > 0

			//Record 38 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column total concentration grid root name
			dischemfracwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 38 (Part 2)
			fgets(dischemfracwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column dissolved chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			dischemfracwatergridroot = StripString(dischemfracwatergridroot);

			//echo chemical water column dissolved fraction grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Dissolved Chemical Fraction Grid Root Name:  %s\n",
				dischemfracwatergridroot);

			//Record 39 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column bound fraction grid root name
			bndchemfracwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 39 (Part 2)
			fgets(bndchemfracwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column bound chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			bndchemfracwatergridroot = StripString(bndchemfracwatergridroot);

			//echo chemical water column bound fraction grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Bound Chemical Fraction Grid Root Name:  %s\n",
				bndchemfracwatergridroot);

			//Record 40 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column mobile fraction grid root name
			mblchemfracwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 40 (Part 2)
			fgets(mblchemfracwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column mobile chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			mblchemfracwatergridroot = StripString(mblchemfracwatergridroot);

			//echo chemical water column mobile fraction grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Mobile Chemical Fraction Grid Root Name:  %s\n",
				mblchemfracwatergridroot);

			//Record 41 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical water column particulate fraction grid root name
			prtchemfracwatergridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 41 (Part 2)
			fgets(prtchemfracwatergridroot, MAXNAMESIZE, inputfile_fp);	//read root name for water column particulate chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			prtchemfracwatergridroot = StripString(prtchemfracwatergridroot);

			//echo chemical water column particulate fraction grid root name
			fprintf(echofile_fp,
				"\n\nWater Column Particulate Chemical Fraction Grid Root Name:  %s\n",
				prtchemfracwatergridroot);

			//Record 42 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment dissolved chemical fraction grid root name
			dischemfracsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 42 (Part 2)
			fgets(dischemfracsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment dissolved chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			dischemfracsurfgridroot = StripString(dischemfracsurfgridroot);

			//echo surface sediment dissolved chemical fraction grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Dissolved Chemical Fraction Grid Root Name:  %s\n",
				dischemfracsurfgridroot);

			//Record 43 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment bound chemical fraction grid root name
			bndchemfracsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 43 (Part 2)
			fgets(bndchemfracsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment bound chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			bndchemfracsurfgridroot = StripString(bndchemfracsurfgridroot);

			//echo surface sediment dissolved chemical fraction grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Bound Chemical Fraction Grid Root Name:  %s\n",
				bndchemfracsurfgridroot);

			//Record 44 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment modile chemical fraction grid root name
			mblchemfracsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 44 (Part 2)
			fgets(mblchemfracsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment mobile chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			mblchemfracsurfgridroot = StripString(mblchemfracsurfgridroot);

			//echo surface sediment bound chemical fraction grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Mobile Chemical Fraction Grid Root Name:  %s\n",
				mblchemfracsurfgridroot);

			//Record 45 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for surface sediment particulate chemical fraction grid root name
			prtchemfracsurfgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 45 (Part 2)
			fgets(prtchemfracsurfgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for surface sediment particulate chemical fraction grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			prtchemfracsurfgridroot = StripString(prtchemfracsurfgridroot);

			//echo surface sediment particulate chemical fraction grid root name
			fprintf(echofile_fp,
				"\n\nSurface Sediment Particulate Chemical Fraction Grid Root Name:  %s\n",
				prtchemfracsurfgridroot);

			//Record 46 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for chemical infiltration flux grid root name
			infchemfluxgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 46 (Part 2)
			fgets(infchemfluxgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for chemical infiltration flux grid (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			infchemfluxgridroot = StripString(infchemfluxgridroot);

			//echo chemical infiltration flux grid root name
			fprintf(echofile_fp,
				"\n\nChemical Infiltration Flux Grid Root Name:  %s\n",
				infchemfluxgridroot);

		}	//end if ksim > 2

		//Record 47 (Header for cumulative grid outputs)
		fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header

		//Record 48 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for net elevation grid name
		netelevationgrid= (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 48 (Part 2)
		fgets(netelevationgrid, MAXNAMESIZE, inputfile_fp);	//read name for net elevation grid (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		netelevationgrid= StripString(netelevationgrid);

		//echo net elevation grid name
		fprintf(echofile_fp,
			"\n\nNet Elevation Grid Name:  %s\n",
			netelevationgrid);

		//Record 49 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for solids gross erosion grid root name
		solidserosiongridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 49 (Part 2)
		fgets(solidserosiongridroot, MAXNAMESIZE, inputfile_fp);	//read root name for solids gross erosion grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		solidserosiongridroot = StripString(solidserosiongridroot);

		//echo solids gross erosion grid root name
		fprintf(echofile_fp,
			"\n\nSolids Gross Erosion Grid Root Name:  %s\n",
			solidserosiongridroot);

		//Record 50 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for solids gross deposition grid root name
		solidsdepositiongridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 50 (Part 2)
		fgets(solidsdepositiongridroot, MAXNAMESIZE, inputfile_fp);	//read root name for solids gross deposition grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		solidsdepositiongridroot = StripString(solidsdepositiongridroot);

		//echo solids gross deposition grid root name
		fprintf(echofile_fp,
			"\n\nSolids Gross Deposition Grid Root Name:  %s\n",
			solidsdepositiongridroot);

		//Record 51 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate memory for solids net accumulation grid root name
		solidsnetaccumgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 51 (Part 2)
		fgets(solidsnetaccumgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for solids net accumulation grids (including any path)

		//strip leading blanks, trailing blanks, and final carriage return from string
		solidsnetaccumgridroot = StripString(solidsnetaccumgridroot);

		//echo solids net accumulation grid root name
		fprintf(echofile_fp,
			"\n\nSolids Net Accumulation Grid Root Name:  %s\n",
			solidsnetaccumgridroot);

		//if chemical transport is simulated
		if(ksim > 2)
		{
			//Record 52 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for solids gross erosion grid root name
			chemerosiongridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 52 (Part 2)
			fgets(chemerosiongridroot, MAXNAMESIZE, inputfile_fp);	//read root name for solids gross erosion grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			chemerosiongridroot = StripString(chemerosiongridroot);

			//echo solids gross erosion grid root name
			fprintf(echofile_fp,
				"\n\nSolids Gross Erosion Grid Root Name:  %s\n",
				chemerosiongridroot);

			//Record 53 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for solids gross deposition grid root name
			chemdepositiongridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 53 (Part 2)
			fgets(chemdepositiongridroot, MAXNAMESIZE, inputfile_fp);	//read root name for solids gross deposition grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			chemdepositiongridroot = StripString(chemdepositiongridroot);

			//echo solids gross deposition grid root name
			fprintf(echofile_fp,
				"\n\nSolids Gross Deposition Grid Root Name:  %s\n",
				chemdepositiongridroot);

			//Record 54 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Allocate memory for solids net accumulation grid root name
			chemnetaccumgridroot = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

			//Record 54 (Part 2)
			fgets(chemnetaccumgridroot, MAXNAMESIZE, inputfile_fp);	//read root name for solids net accumulation grids (including any path)

			//strip leading blanks, trailing blanks, and final carriage return from string
			chemnetaccumgridroot = StripString(chemnetaccumgridroot);

			//echo solids net accumulation grid root name
			fprintf(echofile_fp,
				"\n\nSolids Net Accumulation Grid Root Name:  %s\n",
				chemnetaccumgridroot);

		}	//end if ksim > 2

	}	//end if ksim > 1

	//Record 55 (Header for summary outputs)
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s\n", header);

	//Record 56 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for dump file name
	dmpfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 56 (Part 2)
	fgets(dmpfile, MAXNAMESIZE, inputfile_fp);	//read name for dmpfile (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	dmpfile = StripString(dmpfile);

	//echo dump file name
	fprintf(echofile_fp, "\n\nDump File Name:  %s\n", dmpfile);

	//Record 57 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for mass balance file name
	msbfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 57 (Part 2)
	fgets(msbfile, MAXNAMESIZE, inputfile_fp);	//read name for msbfile (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	msbfile = StripString(msbfile);

	//echo mass balance file name
	fprintf(echofile_fp, "\n\nMass Balance File Name:  %s\n", msbfile);

	//Record 58 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Allocate memory for statistics file name
	statsfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 58 (Part 2)
	fgets(statsfile, MAXNAMESIZE, inputfile_fp);	//read name for statsfile (including any path)

	//strip leading blanks, trailing blanks, and final carriage return from string
	statsfile = StripString(statsfile);

	//echo summary statistics file name
	fprintf(echofile_fp, "\n\nSummary Statistics File Name:  %s\n", statsfile);

	//Close the Echo File
	fclose(echofile_fp);

//End of function: Return to ReadInputFile
}
