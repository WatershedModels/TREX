/*---------------------------------------------------------------------
C-  Function:   ReadDataGroupE.c
C-
C-  Purpose/    ReadDataGroupE is called at the start of the simulation
C-  Methods:    to read Data Group E (environmental conditions) from
C-              the model input file.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   ksim, chnopt, fpocovopt, fpocchopt
C-
C-  Calls:      None
C-
C-  Called by:  ReadInputFile
C-
C-  Created:    Mark Velleux
C-              John England
C-
C-  Date:       09-JUL-2004
C-
C-  Revised:
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

void ReadDataGroupE()
{
	//local variable declarations/definitions
	int
		iprop,			//loop index for environmental property
		itf,			//loop index for time function
		pid,			//property identification number
		ntf,			//number of time functions for a property
		tfid,			//time function identification number
		nlayers,		//number of soil layers
		ilayer,			//loop index for soil layer
		isolid;			//loop index for solids type

	float
		stationelev,	//station elevation (m)
		convunits,		//conversion factors for length, area, volume or mass units
		convtime,		//conversion factor for time units
		scale;			//scale factor

	//open the echo file in append mode (append to existing file)
	echofile_fp = fopen(echofile, "a");

	//write message to screen
	printf("\n\n***************************\n");
	printf(	   "*                         *\n");
	printf(	   "*   Reading Data Group E  *\n");
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

	//Read general properties...
	//
	//Record 1
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header from file

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s\n\n", header);

	//Record 2
	fscanf(inputfile_fp, "%s %d",	//read
		pname,						//Read property name/description
		&npropg);					//Number of General Environmental Properties

	//Echo number of properties file
	fprintf(echofile_fp, "\nNumber of General Env. Properties = %d\n", npropg);
	fprintf(echofile_fp, "\nDescription: %s\n", pname);

	//if the number of properties < 0, abort...
	if(npropg < 0)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  Number of properties must be >= 0\n");
		fprintf(echofile_fp,       "  User selected npropg = %d\n", npropg);
		fprintf(echofile_fp,       "  Select npropg >= 0");

		//Write error message to screen
		printf(              "\n\n\nData Group Error:\n");
		printf(                    "  Number of properties must be >= 0\n");
		printf(                    "  User selected npropg = %d\n", npropg);
		printf(                    "  Select npropg >= 0");

		exit(EXIT_FAILURE);					//abort

	}	//end if npropg < 0

	//Allocate memory for general environmental properties and time functions
	pidg = (int *)malloc((npropg+1) * sizeof(int));					//general property identification number
	envg = (float ***)malloc((npropg+1) * sizeof(float **));		//general property cell value
	envgtfid = (int ***)malloc((npropg+1) * sizeof(int **));		//general property time function id (pointer)
	stnelevenvg = (float **)malloc((npropg+1) * sizeof(float *));	//general property station elevation
	nenvgtf = (int *)malloc((npropg+1) * sizeof(int));				//number of general property time functions
	nenvgtfpairs = (int **)malloc((npropg+1) * sizeof(int *));		//number of general time functions pairs
	envgtf = (float ***)malloc((npropg+1) * sizeof(float *));		//general time function value
	envgtftime = (float ***)malloc((npropg+1) * sizeof(float *));	//general time function time break

	//Allocate memory for environmental property file names
	envpropfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Allocate memory for environmental time function pointer file names
	envtfpointerfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//loop over number of general properties
	for(iprop=1; iprop<=npropg; iprop++)
	{
		//Record 3
		fscanf(inputfile_fp, "%s %d %s %f %s %f %s %d",	//read
			varname,				//dummy
			&pid,					//property identification number
			varname,				//dummy
			&convunits,				//units conversion factor
			varname,				//dummy
			&scale,					//scale factor
			varname,				//dummy
			&ntf);					//number of time functions for this property

		//Assign property id (used as an array index)
		pidg[iprop] = pid;

		//Assign number of time functions for this propery
		nenvgtf[iprop] = ntf;

		//Echo environmental properties to file
		fprintf(echofile_fp, "\n\n  Specification of General Environmental Property  ");
		fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
		fprintf(echofile_fp,   "\n  Property Name = %s", pname);
		fprintf(echofile_fp,   "\n  Property ID = %d", pidg[iprop]);
		fprintf(echofile_fp,   "\n  Units Conversion Factor = %f", convunits);
		fprintf(echofile_fp,   "\n  Scale Factor = %f", scale);
		fprintf(echofile_fp,   "\n  Number of Time Functions for Property = %d\n\n", nenvgtf[iprop]);

		//Allocate remaining memory for general environmental properties cell values...
		//
		//Allocate memory (rows)
		envg[iprop] = (float **)malloc((nrows+1) * sizeof(float *));		//general property cell value
		envgtfid[iprop] = (int **)malloc((nrows+1) * sizeof(int *));		//general property time function id (pointer)

		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate memory (cols)
			envg[iprop][i] = (float *)malloc((ncols+1) * sizeof(float));	//general property cell value
			envgtfid[iprop][i] = (int *)malloc((ncols+1) * sizeof(int));	//general property time function id (pointer)

			//loop over columns
			for(j=1; j<=ncols; j++)
			{
				//initialize cell values to zero
				envg[iprop][i][j] = 0.0;		//general property cell value
				envgtfid[iprop][i][j] = 0;		//general property time function id (pointer)

			}	//end loop over cols

		}	//end loop over rows

		//Allocate additional memory for general environmental time functions
		stnelevenvg[iprop] = (float *)malloc((nenvgtf[iprop]+1) * sizeof(float));			//general property station elevation
		nenvgtfpairs[iprop] = (int *)malloc((nenvgtf[iprop]+1) * sizeof(int));		//number of overland time functions pairs
		envgtf[iprop] = (float **)malloc((nenvgtf[iprop]+1) * sizeof(float *));		//general time function value
		envgtftime[iprop] = (float **)malloc((nenvgtf[iprop]+1) * sizeof(float *));	//general time function time break

		//Read property grid file names and process...
		//
		//Note:  One property grid file is read for general properties.
		//       Cell values are assigned to specific global property
		//       variables in UpdateEnvironment.
		//
		//Record 4 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 4 (Part 2)
		fgets(envpropfile, MAXNAMESIZE, inputfile_fp);	//Read name of property grid file

		//strip leading blanks, trailing blanks, and final carriage return from string
		envpropfile = StripString(envpropfile);

		//Echo general environmental property file name to file
		fprintf(echofile_fp, "\n\nProperty File: %s\n", envpropfile);

		//Read general environmental property condition file
		ReadGeneralEnvironmentFile(iprop, convunits, scale);

		//if time functions are specified
		if(nenvgtf[iprop] > 0)
		{
			//Record 5 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Record 5 (Part 2)
			fgets(envtfpointerfile, MAXNAMESIZE, inputfile_fp);	//Read name of time function pointer grid file

			//strip leading blanks, trailing blanks, and final carriage return from string
			envtfpointerfile = StripString(envtfpointerfile);

			//Echo overland environmental time function pointer file name to file
			fprintf(echofile_fp, "\n\nTime function Pointer File: %s\n",
				envtfpointerfile);

			//Read overland environmental time function pointer file
			ReadGeneralTFPointerFile(iprop);

			//Case-specific memory allocation for air temperature:
			//
			//if the property is air temperature, then allocate memory
			if(pid == 2)
			{
				//Allocate memory for air temperature lapse rate
				atlapse = (float *)malloc((nenvgtf[iprop]+1) * sizeof(float));		//allocate memory

			}	//end if pid = 2

			//Loop over number of time functions
			for(itf=1; itf<=nenvgtf[iprop]; itf++)
			{
				//Record 6 (Part 1)
				fscanf(inputfile_fp, "%s %d %s %f",	//read
					varname,						//dummy
					&tfid,							//time function id
					varname,						//dummy
					&stationelev);					//station elevation (local)

				//Record 6 (Part 2)
				fgets(tfname, MAXNAMESIZE, inputfile_fp);	//Read time function name/description

				//Echo function id and description to file
				fprintf(echofile_fp, "\n\n%s %d  %s %f  %s %s\n",
					"Function: ", tfid,
					"Station Elevation: ", stationelev,
					"Description: ", tfname);

				//check function id for error...
				//
				//if tfid does not equal the expected value (tfid != itf)
				if(tfid != itf)
				{
					//Write error message to file
					fprintf(echofile_fp, "\n\n\nGeneral Env. Time Function Error:\n");
					fprintf(echofile_fp,       "  Function ID Read = %d\n", tfid);
					fprintf(echofile_fp,       "  Function ID Expected = %d\n", itf);

					//Write error message to screen
					printf(              "\n\n\nGeneral Env. Time Function Error:\n");
					printf(                    "  Function ID Read = %d\n", tfid);
					printf(                    "  Function ID Expected = %d\n", itf);

					exit(EXIT_FAILURE);					//abort

				}	//end if tfid != itf

				//store station elevation in global array
				stnelevenvg[iprop][itf] = stationelev;

				//Record 7
				fscanf(inputfile_fp, "%s %f %s %f %s %f",	//read
					varname,				//dummy
					&convunits,				//units conversion factor
					varname,				//dummy
					&convtime,				//time conversion factor
					varname,				//dummy
					&scale);				//scale factor

				//Echo converson and scale factors to file
				fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
				fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
				fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

				//Record 8
				fscanf(inputfile_fp, "%s %d",	//read
					varname,					//dummy
					&nenvgtfpairs[iprop][itf]);	//number of pairs in time function series

				//Echo number of pairs to file
				fprintf(echofile_fp, "\n\n %s %d\n",
					"Number of Pairs: ", nenvgtfpairs[iprop][itf]);

				//Allocate remaining memory for general environmental time functions
				envgtf[iprop][itf] = (float *)malloc((nenvgtfpairs[iprop][itf]+1) * sizeof(float));		//allocate memory
				envgtftime[iprop][itf] = (float *)malloc((nenvgtfpairs[iprop][itf]+1) * sizeof(float));	//allocate memory

				//Write label for function time series to file
				fprintf(echofile_fp, "  Function Value    ");	//time function units vary...
				fprintf(echofile_fp, "  Time (days)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~  ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

				//Loop over number of pairs in current function for current property
				for(k=1; k<=nenvgtfpairs[iprop][itf]; k++)
				{
					//Record 9
					fscanf(inputfile_fp, "%f %f",		//read
						&envgtf[iprop][itf][k],			//time function value
						&envgtftime[iprop][itf][k]);	//time function time break

					//Apply conversion and scale factors
					envgtf[iprop][itf][k] = envgtf[iprop][itf][k] * convunits * convtime * scale;

					//Echo function value and time break to file
					fprintf(echofile_fp, "%18.3f  %15.3f\n",
						envgtf[iprop][itf][k], envgtftime[iprop][itf][k]);

				}	//end loop over number of pairs

				//if the property is air temperature, then read the laspe rate
				if(pid == 2)
				{
					//Record 10
					fscanf(inputfile_fp, "%s %f",	//read
						varname,					//dummy
						&atlapse[itf]);				//air temperature lapse rate (degrees C/km)

					//convert laspe rate from (degrees C/km) to (degrees C/m)
					atlapse[itf] = (float)(atlapse[itf] / 1000.0);

				}	//end if pid = 2

			}	//end loop over number of time functions

		}	//end if nenvgtf[iprop] > 0

	}	//end loop over general properties

	//if chemical transport is simulated
	if(ksim > 2)
	{
		//Read overland properties...
		//
		//Record 11
		fscanf(inputfile_fp, "%s %d",	//read
			pname,						//Read property name/description
			&npropov);					//Number of Overland Environmental Properties

		//Echo number of properties file
		fprintf(echofile_fp, "\nNumber of Overland Env. Properties = %d\n", npropov);
		fprintf(echofile_fp, "\nDescription: %s\n", pname);

		//if the number of properties < 0, abort...
		if(npropov < 0)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nData Group Error:\n");
			fprintf(echofile_fp,       "  Number of properties must be >= 0\n");
			fprintf(echofile_fp,       "  User selected npropov = %d\n", npropov);
			fprintf(echofile_fp,       "  Select npropov >= 0");

			//Write error message to screen
			printf(              "\n\n\nData Group Error:\n");
			printf(                    "  Number of properties must be >= 0\n");
			printf(                    "  User selected npropov = %d\n", npropov);
			printf(                    "  Select npropov >= 0");

			exit(EXIT_FAILURE);					//abort

		}	//end if npropov < 0

		//Allocate memory for overland environmental properties and time functions
		pidov = (int *)malloc((npropov+1) * sizeof(int));					//overland property identification number
		envov = (float ****)malloc((npropov+1) * sizeof(float ***));		//overland property cell value
		envovtfid = (int ****)malloc((npropov+1) * sizeof(int ***));		//overland property time function id (pointer)
		nenvovtf = (int *)malloc((npropov+1) * sizeof(int));				//number of overland property time functions
		nenvovtfpairs = (int **)malloc((npropov+1) * sizeof(int *));		//number of overland time functions pairs
		envovtf = (float ***)malloc((npropov+1) * sizeof(float *));			//overland time function value
		envovtftime = (float ***)malloc((npropov+1) * sizeof(float *));		//overland time function time break

		//loop over number of overland properties
		for(iprop=1; iprop<=npropov; iprop++)
		{
			//Record 12
			fscanf(inputfile_fp, "%s %d %s %f %s %f %s %d",	//read
				varname,				//dummy
				&pid,					//property identification number
				varname,				//dummy
				&convunits,				//units conversion factor
				varname,				//dummy
				&scale,					//scale factor
				varname,				//dummy
				&ntf);					//number of time functions for this property

			//Assign property id (used as an array index)
			pidov[iprop] = pid;

			//Assign number of time functions for this propery
			nenvovtf[iprop] = ntf;

			//Echo environmental properties to file
			fprintf(echofile_fp, "\n\n  Specification of Overland Environmntal Property  ");
			fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
			fprintf(echofile_fp,   "\n  Property Name = %s", pname);
			fprintf(echofile_fp,   "\n  Property ID = %d", pidov[iprop]);
			fprintf(echofile_fp,   "\n  Units Conversion Factor = %f", convunits);
			fprintf(echofile_fp,   "\n  Scale Factor = %f", scale);
			fprintf(echofile_fp,   "\n  Number of Time Functions for Property = %d\n\n", nenvovtf[iprop]);

			//Allocate remaining memory for overland environmental properties cell values...
			//
			//Allocate memory (rows)
			envov[iprop] = (float ***)malloc((nrows+1) * sizeof(float **));		//overland property cell value
			envovtfid[iprop] = (int ***)malloc((nrows+1) * sizeof(int **));		//overland property time function id (pointer)

			//loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Allocate memory (cols)
				envov[iprop][i] = (float **)malloc((ncols+1) * sizeof(float *));	//overland property cell value
				envovtfid[iprop][i] = (int **)malloc((ncols+1) * sizeof(int *));	//overland property time function id (pointer)

				//loop over columns
				for(i=1; i<=ncols; i++)
				{
					//the number of layers is the maximum number of layers
					nlayers = maxstackov;

					//Allocate memory (layers)
					envov[iprop][i][j] = (float *)malloc((nlayers+1) * sizeof(float));	//overland property cell value
					envovtfid[iprop][i][j] = (int *)malloc((nlayers+1) * sizeof(int));	//overland property time function id (pointer)

					//loop over layers
					for(k=0; k<=nlayers; k++)
					{
						//initialize cell values
						envov[iprop][i][j][k] = 0.0;	//overland property cell value
						envovtfid[iprop][i][j][k] = 0;	//overland property time function id (pointer)

					}	//end loop over layers

				}	//end loop over columns

			}	//end loop over rows

			//Allocate additional memory for overland environmental time functions
			nenvovtfpairs[iprop] = (int *)malloc((nenvovtf[iprop]+1) * sizeof(int));		//number of overland time functions pairs
			envovtf[iprop] = (float **)malloc((nenvovtf[iprop]+1) * sizeof(float *));		//overland time function value
			envovtftime[iprop] = (float **)malloc((nenvovtf[iprop]+1) * sizeof(float *));	//overland time function time break

			//Read property grid file names and process...
			//
			//Note:  One property grid file is read for the water column
			//       and then one file is read for each layer in the soil
			//       stack.  The soil stack files are read in reverse order.
			//
			//Record 13 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
				varname);				//dummy

			//Record 13 (Part 2)
			fgets(envpropfile, MAXNAMESIZE, inputfile_fp);	//Read name of property grid file

			//strip leading blanks, trailing blanks, and final carriage return from string
			envpropfile = StripString(envpropfile);

			//Echo overland environmental property file name to file (Layer 0)
			fprintf(echofile_fp, "\n\nLayer: %d  Property File: %s\n",
				0, envpropfile);

			//Read environmental property condition file for water column (Layer 0)
			ReadOverlandEnvironmentFile(iprop, 0, convunits, scale);

			//if time functions are specified
			if(nenvovtf[iprop] > 0)
			{
				//Record 14 (Part 1)
				fscanf(inputfile_fp, "%s",	//read
					varname);				//dummy

				//Record 14 (Part 2)
				fgets(envtfpointerfile, MAXNAMESIZE, inputfile_fp);	//Read name of time function pointer grid file

				//strip leading blanks, trailing blanks, and final carriage return from string
				envtfpointerfile = StripString(envtfpointerfile);

				//Echo overland environmental time function pointer file name to file
				fprintf(echofile_fp, "\n\nTime function Pointer File: %s\n",
					envtfpointerfile);

				//Read overland environmental time function pointer file for water column (Layer 0)
				ReadOverlandEnvironmentTFPointerFile(iprop, 0);

			}	//end if nenvovtf[] > 0

			//if the process specified is not light extinction (pid != 8)
			if(pid != 8)
			{
				//Loop over layers in reverse order (!!) (top down) (bottom is Layer 1)
				for(ilayer=maxstackov; ilayer>=1; ilayer--)
				{
					//Record 15 (Part 1)
					fscanf(inputfile_fp, "%s",	//read
						varname);				//dummy

					//Record 15 (Part 2)
					fgets(envpropfile, MAXNAMESIZE, inputfile_fp);	//Read name of property grid file

					//strip leading blanks, trailing blanks, and final carriage return from string
					envpropfile = StripString(envpropfile);

					//Echo overland environmental property file name to file
					fprintf(echofile_fp, "\n\nLayer: %d  Property File: %s\n",
						ilayer, envpropfile);

					//Read environmental property file for this soil layer
					ReadOverlandEnvironmentFile(iprop, ilayer, convunits, scale);

					//if time functions are specified
					if(nenvovtf[iprop] > 0)
					{
						//Record 16 (Part 1)
						fscanf(inputfile_fp, "%s",	//read
							varname);				//dummy

						//Record 16 (Part 2)
						fgets(envtfpointerfile, MAXNAMESIZE, inputfile_fp);	//Read name of time function pointer grid file

						//strip leading blanks, trailing blanks, and final carriage return from string
						envtfpointerfile = StripString(envtfpointerfile);

						//Echo overland environmental time function pointer file name to file
						fprintf(echofile_fp, "\n\nTime function Pointer File: %s\n",
							envtfpointerfile);

						//Read overland environmental time function pointer file for soil stack
						ReadOverlandEnvironmentTFPointerFile(iprop, ilayer);

					}	//end if nenvovtf[iprop] > 0

				}	//loop over layers (in reverse order)

			}	//end if pid != 8

			//if time functions are specified
			if(nenvovtf[iprop] > 0)
			{
				//Loop over number of time functions
				for(itf=1; itf<=nenvovtf[iprop]; itf++)
				{
					//Record 17 (part 2)
					fscanf(inputfile_fp, "%s %d",	//read
						varname,					//dummy
						&tfid);						//time function id

					//Record 17 (Part 2)
					fgets(tfname, MAXNAMESIZE, inputfile_fp);	//Read time function name/description

					//Echo function id and description to file
					fprintf(echofile_fp, "\n\n%s %d  %s %s\n",
						"Function: ", tfid,
						"Description: ", tfname);

					//check function id for error...
					//
					//if tfid does not equal the expected value (tfid != itf)
					if(tfid != itf)
					{
						//Write error message to file
						fprintf(echofile_fp, "\n\n\nOverland Env. Time Function Error:\n");
						fprintf(echofile_fp,       "  Function ID Read = %d\n", tfid);
						fprintf(echofile_fp,       "  Function ID Expected = %d\n", itf);

						//Write error message to screen
						printf(              "\n\n\nOverland Env. Time Function Error:\n");
						printf(                    "  Function ID Read = %d\n", tfid);
						printf(                    "  Function ID Expected = %d\n", itf);

						exit(EXIT_FAILURE);					//abort

					}	//end if tfid != itf

					//Record 18
					fscanf(inputfile_fp, "%s %f %s %f %s %f",	//read
						varname,				//dummy
						&convunits,				//units conversion factor
						varname,				//dummy
						&convtime,				//time conversion factor
						varname,				//dummy
						&scale);				//scale factor

					//Echo converson and scale factors to file
					fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
					fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
					fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

					//Record 19
					fscanf(inputfile_fp, "%s %d",	//read
						varname,					//dummy
						&nenvovtfpairs[iprop][itf]);	//number of pairs in time function series

					//Echo number of pairs to file
					fprintf(echofile_fp, "\n\n %s %d\n",
						"Number of Pairs: ", nenvovtfpairs[iprop][itf]);

					//Allocate remaining memory for overland environmental time functions
					envovtf[iprop][itf] = (float *)malloc((nenvovtfpairs[iprop][itf]+1) * sizeof(float));	//allocate memory
					envovtftime[iprop][itf] = (float *)malloc((nenvovtfpairs[iprop][itf]+1) * sizeof(float));	//allocate memory

					//Write label for function time series to file
					fprintf(echofile_fp, "  Function Value    ");	//time function units vary...
					fprintf(echofile_fp, "  Time (days)  \n");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~  ");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

					//Loop over number of pairs in current function for current property
					for(k=1; k<=nenvovtfpairs[iprop][itf]; k++)
					{
						//Record 20
						fscanf(inputfile_fp, "%f %f",	//read
							&envovtf[iprop][itf][k],		//time function value
							&envovtftime[iprop][itf][k]);	//time function time break

						//Apply conversion and scale factors
						envovtf[iprop][itf][k] = envovtf[iprop][itf][k] * convunits * convtime * scale;

						//Echo function value and time break to file
						fprintf(echofile_fp, "%18.3f  %15.3f\n",
							envovtf[iprop][itf][k], envovtftime[iprop][itf][k]);

					}	//end loop over number of pairs

				}	//end loop over number of time functions

			}	//end if nenvovtf[iprop] > 0

		}	//end loop over overland properties

		//Read channel properties...
		//
		//if channel are simulated
		if(chnopt > 0)
		{
			//Read channel properties...
			//
			//Record 21
			fscanf(inputfile_fp, "%s %d",	//read
				pname,						//Read property name/description
				&npropch);					//Number of Channel Environmental Properties

			//Echo number of properties file
			fprintf(echofile_fp, "\nNumber of Channel Env. Properties = %d\n", npropch);
			fprintf(echofile_fp, "\nDescription: %s\n", pname);

			//if the number of properties < 0, abort...
			if(npropch < 0)
			{
				//Write error message to file
				fprintf(echofile_fp, "\n\n\nData Group Error:\n");
				fprintf(echofile_fp,       "  Number of properties must be >= 0\n");
				fprintf(echofile_fp,       "  User selected npropch = %d\n", npropch);
				fprintf(echofile_fp,       "  Select npropch >= 0");

				//Write error message to screen
				printf(              "\n\n\nData Group Error:\n");
				printf(                    "  Number of properties must be >= 0\n");
				printf(                    "  User selected npropch = %d\n", npropch);
				printf(                    "  Select npropch >= 0");

				exit(EXIT_FAILURE);					//abort

			}	//end if npropch < 0

			//Allocate memory for channel environmental properties and time functions
			pidch = (int *)malloc((npropch+1) * sizeof(int));					//channel property identification number
			envch = (float ****)malloc((npropch+1) * sizeof(float ***));		//channel property cell value
			envchtfid = (int ****)malloc((npropch+1) * sizeof(int ***));		//channel property time function id (pointer)
			nenvchtf = (int *)malloc((npropch+1) * sizeof(int));				//number of channel property time functions
			nenvchtfpairs = (int **)malloc((npropch+1) * sizeof(int *));		//number of channel time functions pairs
			envchtf = (float ***)malloc((npropch+1) * sizeof(float *));			//channel time function value
			envchtftime = (float ***)malloc((npropch+1) * sizeof(float *));		//channel time function time break

			//loop over number of properties
			for(iprop=1; iprop<=npropch; iprop++)
			{
				//Record 22
				fscanf(inputfile_fp, "%s %d %s %f %s %f %s %d",	//read
					varname,				//dummy
					&pid,					//property identification number
					varname,				//dummy
					&convunits,				//units conversion factor
					varname,				//dummy
					&scale,					//scale factor
					varname,				//dummy
					&ntf);					//number of time functions for this property

				//Assign property id (used as an array index)
				pidch[iprop] = pid;

				//Assign number of time functions for this propery
				nenvchtf[iprop] = ntf;

				//Echo environmental properties to file
				fprintf(echofile_fp, "\n\n  Specification of Channel Environmental Property  ");
				fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
				fprintf(echofile_fp,   "\n  Property Name = %s", pname);
				fprintf(echofile_fp,   "\n  Property ID = %d", pidch[iprop]);
				fprintf(echofile_fp,   "\n  Units Conversion Factor = %f", convunits);
				fprintf(echofile_fp,   "\n  Scale Factor = %f", scale);
				fprintf(echofile_fp,   "\n  Number of Time Functions for Property = %d\n\n", nenvchtf[iprop]);

				//Allocate remaining memory for channel environmental properties node values...
				//
				//Allocate memory (links)
				envch[iprop] = (float ***)malloc((nlinks+1) * sizeof(float **));		//channel property node value
				envchtfid[iprop] = (int ***)malloc((nlinks+1) * sizeof(int **));		//channel property time function id (pointer)

				//loop over links
				for(i=1; i<=nlinks; i++)
				{
					//Allocate memory (cols)
					envch[iprop][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//channel property node value
					envchtfid[iprop][i] = (int **)malloc((nnodes[i]+1) * sizeof(int *));	//channel property time function id (pointer)

					//loop over nodes
					for(i=1; i<=nnodes[i]; i++)
					{
						//the number of layers is the maximum number of layers
						nlayers = maxstackch;

						//Allocate memory (layers)
						envch[iprop][i][j] = (float *)malloc((nlayers+1) * sizeof(float));	//channel property node value
						envchtfid[iprop][i][j] = (int *)malloc((nlayers+1) * sizeof(int));	//channel property time function id (pointer)

						//loop over layers
						for(k=0; k<=nlayers; k++)
						{
							//initialize node values
							envch[iprop][i][j][k] = 0.0;	//channel property node value
							envchtfid[iprop][i][j][k] = 0;	//channel property time function id (pointer)

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links

				//Allocate additional memory for channel environmental time functions
				nenvchtfpairs[iprop] = (int *)malloc((nenvchtf[iprop]+1) * sizeof(int));		//number of channel time functions pairs
				envchtf[iprop] = (float **)malloc((nenvchtf[iprop]+1) * sizeof(float *));		//channel time function value
				envchtftime[iprop] = (float **)malloc((nenvchtf[iprop]+1) * sizeof(float *));	//channel time function time break

				//Read channel environmental property file names and process...
				//
				//Note:  One file is read for the water column and one file is
				//       read for the sediment stack.
				//
				//Record 23 (Part 1)
				fscanf(inputfile_fp, "%s",	//read
					varname);				//dummy

				//Record 23 (Part 2)
				fgets(envpropfile, MAXNAMESIZE, inputfile_fp);	//Read name of channel environmental file

				//strip leading blanks, trailing blanks, and final carriage return from string
				envpropfile = StripString(envpropfile);

				//Echo channel environmental property file name to file
				fprintf(echofile_fp, "\n\nChannel Environmental Property File: %s\n",
					envpropfile);

				//Read environmental property condition file for water column and sediment
				ReadChannelEnvironmentFile(iprop, convunits, scale);

				//if time functions are specified
				if(nenvchtf[iprop] > 0)
				{
					//Loop over number of time functions
					for(itf=1; itf<=nenvchtf[iprop]; itf++)
					{
						//Record 24 (Part 1)
						fscanf(inputfile_fp, "%s %d",	//read
							varname,		//dummy
							&tfid);			//time function id

						//Record 24 (Part 2)
						fgets(tfname, MAXNAMESIZE, inputfile_fp);	//Read time function name/description

						//Echo function id and description to file
						fprintf(echofile_fp, "\n\n%s %d  %s %s\n",
							"Function: ", tfid,
							"Description: ", tfname);

						//check function id for error...
						//
						//if tfid does not equal the expected value (tfid != itf)
						if(tfid != itf)
						{
							//Write error message to file
							fprintf(echofile_fp, "\n\n\nChannel Env. Time Function Error:\n");
							fprintf(echofile_fp,       "  Function ID Read = %d\n", tfid);
							fprintf(echofile_fp,       "  Function ID Expected = %d\n", itf);

							//Write error message to screen
							printf(              "\n\n\nChannel Env. Time Function Error:\n");
							printf(                    "  Function ID Read = %d\n", tfid);
							printf(                    "  Function ID Expected = %d\n", itf);

							exit(EXIT_FAILURE);					//abort

						}	//end if tfid != itf

						//Record 25
						fscanf(inputfile_fp, "%s %f %s %f %s %f",	//read
							varname,				//dummy
							&convunits,				//units conversion factor
							varname,				//dummy
							&convtime,				//time conversion factor
							varname,				//dummy
							&scale);				//scale factor

						//Echo converson and scale factors to file
						fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
						fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
						fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

						//Record 26
						fscanf(inputfile_fp, "%s %d",		//read
							varname,						//dummy
							&nenvchtfpairs[iprop][itf]);	//number of pairs in time function series

						//Echo number of pairs to file
						fprintf(echofile_fp, "\n\n %s %d\n",
							"Number of Pairs: ", nenvchtfpairs[iprop][itf]);

						//Allocate remaining memory for channel environmental time functions
						envchtf[iprop][itf] = (float *)malloc((nenvchtfpairs[iprop][itf]+1) * sizeof(float));		//allocate memory
						envchtftime[iprop][itf] = (float *)malloc((nenvchtfpairs[iprop][itf]+1) * sizeof(float));	//allocate memory

						//Write label for function time series to file
						fprintf(echofile_fp, "  Function Value    ");	//time function units vary...
						fprintf(echofile_fp, "  Time (days)  \n");
						fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~  ");
						fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

						//Loop over number of pairs in current function
						for(k=1; k<=nenvchtfpairs[iprop][itf]; k++)
						{
							//Record 27
							fscanf(inputfile_fp, "%f %f",		//read
								&envchtf[iprop][itf][k],		//time function value
								&envchtftime[iprop][itf][k]);	//time function time break

							//Apply conversion and scale factors
							envchtf[iprop][itf][k] = envchtf[iprop][itf][k] * convunits * convtime * scale;

							//Echo function value and time break to file
							fprintf(echofile_fp, "%18.3f  %15.3f\n",
								envchtf[iprop][itf][k], envchtftime[iprop][itf][k]);

						}	//end loop over number of pairs

					}	//end loop over number of time functions

				}	//end if nenvchtf[iprop] > 0

			}	//end loop over channel properties

		}	//end if chanopt > 0

		//Read particle fraction organic carbon (fpoc)...
		//
		//Overland particle foc
		//
		//Allocate initial memory for overland particle properties
		fpocov = (float ****)malloc((nsolids+1) * sizeof(float ***));		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
		fpocovtfid = (int ****)malloc((nsolids+1) * sizeof(int ***));		//overland particle property time function id (pointer) [isolid][row][col][ilayer]
		nfpocovtf = (int *)malloc((nsolids+1) * sizeof(int));				//number of overland particle time functions [isolid]
		nfpocovtfpairs = (int **)malloc((nsolids+1) * sizeof(int *));		//number of time series pairs in overland particle time function [isolid][itf]
		fpocovtf = (float ***)malloc((nsolids+1) * sizeof(float **));		//overland particle time function value [isolid][itf][ipairs]
		fpocovtftime = (float ***)malloc((nsolids+1) * sizeof(float **));	//overland partcle time function time break [isolid][itf][ipairs]

		//Loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Allocate additional memory for overland particle properties
			fpocov[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
			fpocovtfid[isolid] = (int ***)malloc((nrows+1) * sizeof(int **));	//overland particle property time function id (pointer) [isolid][row][col][ilayer]

			//Loop over rows
			for(i=1; i<=nrows; i++)
			{
				//Allocate additional memory for overland particle properties
				fpocov[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
				fpocovtfid[isolid][i] = (int **)malloc((ncols+1) * sizeof(int *));		//overland particle property time function id (pointer) [isolid][row][col][ilayer]

				//Loop over columns
				for(j=1; j<=ncols; j++)
				{
					//Allocate remaining memory for overland particle properties
					fpocov[isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//overland paricle fraction organic carbon (dimensionless) [isolid][row][col][layer]
					fpocovtfid[isolid][i][j] = (int *)malloc((maxstackov+1) * sizeof(int));		//overland particle property time function id (pointer) [isolid][row][col][ilayer]

					//loop over layers (include 0 for water column)
					for(k=0; k<=maxstackov; k++)
					{
						//set default values to 1.0 for fpoc, 0 for tfid...
						fpocov[isolid][i][j][k] = 1.0;			//overland particle fraction organic carbon (dimensionless) [isolid][row][col][layer]
						fpocovtfid[isolid][i][j][k] = 0;		//overland particle property time function id (pointer) [isolid][row][col][ilayer]

					}	//end loop over layers

				}	//end loop over columns

			}	//end loop over rows

		}	//end loop over solids

		//Read overland particle properties
		//
		//Record 28
		fscanf(inputfile_fp, "%s %d",	//read
			varname,					//dummy
			&fpocovopt);				//overland particle foc option

		//Echo foc option to file
		fprintf(echofile_fp, "\nOverland particle foc option = %d\n", fpocovopt);

		//if fpocovopt > 0 (overland fpoc value are entered for each solids type)
		if(fpocovopt > 0)
		{
			//loop over number of solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Record 29
				fscanf(inputfile_fp, "%s %d %s %f %s %d",	//read
					pname,					//dummy
					&pid,					//particle (solids type) identification number
					varname,				//dummy
					&scale,					//scale factor
					varname,				//dummy
					&ntf);					//number of time functions for this particle type

				//check particle id for error...
				//
				//if pid does not equal the expected value (pid != isolid)
				if(pid != isolid)
				{
					//Write error message to file
					fprintf(echofile_fp, "\n\n\nOverland Particle Fpoc Error:\n");
					fprintf(echofile_fp,       "  Particle ID Read = %d\n", pid);
					fprintf(echofile_fp,       "  Particle ID Expected = %d\n", isolid);

					//Write error message to screen
					printf(              "\n\n\nOverland Particle Fpoc Error:\n");
					printf(                    "  Particle ID Read = %d\n", pid);
					printf(                    "  Particle ID Expected = %d\n", isolid);

					exit(EXIT_FAILURE);					//abort

				}	//end if pid != isolid

				//Assign number of time functions for this propery
				nfpocovtf[isolid] = ntf;

				//Echo environmental properties to file
				fprintf(echofile_fp, "\n\n  Specification of Overland Particle Fpoc Values  ");
				fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
				fprintf(echofile_fp,   "\n  Particle Descrption = %s", pname);
				fprintf(echofile_fp,   "\n  Particle ID = %d", isolid);
				fprintf(echofile_fp,   "\n  Scale Factor = %f", scale);
				fprintf(echofile_fp,   "\n  Number of Time Functions for Solid %d = %d\n\n",
					isolid, nfpocovtf[isolid]);

				//Allocate additional memory for overland particle properties
				nfpocovtfpairs[isolid] = (int *)malloc((nfpocovtf[isolid]+1) * sizeof(int));		//number of time series pairs in overland particle time function [isolid][itf]
				fpocovtf[isolid] = (float **)malloc((nfpocovtf[isolid]+1) * sizeof(float *));		//overland particle time function value [isolid][itf][ipairs]
				fpocovtftime[isolid] = (float **)malloc((nfpocovtf[isolid]+1) * sizeof(float *));	//overland partcle time function time break [isolid][itf][ipairs]

				//Read property grid file names and process...
				//
				//Note:  One property grid file is read for the water column
				//       and then one file is read for each layer in the soil
				//       stack.  The soil stack files are read in reverse order.
				//
				//Record 30 (Part 1)
				fscanf(inputfile_fp, "%s",	//read
					varname);				//dummy

				//Record 30 (Part 2)
				fgets(envpropfile, MAXNAMESIZE, inputfile_fp);	//Read name of property grid file

				//strip leading blanks, trailing blanks, and final carriage return from string
				envpropfile = StripString(envpropfile);

				//Echo overland environmental property file name to file (Layer 0)
				fprintf(echofile_fp, "\n\nLayer: %d  Property File: %s\n",
					0, envpropfile);

				//Read overland fpoc property condition file for water column (Layer 0)
				ReadOverlandFpocFile(isolid, 0, scale);

				//if time functions are specified
				if(nfpocovtf[isolid] > 0)
				{
					//Record 31 (Part 1)
					fscanf(inputfile_fp, "%s",	//read
						varname);				//dummy

					//Record 31 (Part 2)
					fgets(envtfpointerfile, MAXNAMESIZE, inputfile_fp);	//Read name of time function pointer grid file

					//strip leading blanks, trailing blanks, and final carriage return from string
					envtfpointerfile = StripString(envtfpointerfile);

					//Echo environmental time function pointer file name to file
					fprintf(echofile_fp, "\n\nTime function Pointer File: %s\n",
						envtfpointerfile);

					//Read overland fpoc time function pointer file for water column (Layer 0)
					ReadOverlandFpocTFPointerFile(isolid, 0);

				}	//end if nfpocovtf[iprop] > 0

				//Loop over layers in reverse order (!!) (top down) (bottom is Layer 1)
				for(ilayer=maxstackov; ilayer>=1; ilayer--)
				{
					//Record 32 (Part 1)
					fscanf(inputfile_fp, "%s",	//read
						varname);				//dummy

					//Record 32 (Part 2)
					fgets(envpropfile, MAXNAMESIZE, inputfile_fp);	//Read name of property grid file

					//strip leading blanks, trailing blanks, and final carriage return from string
					envpropfile = StripString(envpropfile);

					//Echo environmental property file name to file
					fprintf(echofile_fp, "\n\nLayer: %d  Property File: %s\n",
						ilayer, envpropfile);

					//Read overland fpoc file for this soil layer
					ReadOverlandFpocFile(isolid, ilayer, scale);

					//if time functions are specified
					if(nfpocovtf[isolid] > 0)
					{
						//Record 33 (Part 1)
						fscanf(inputfile_fp, "%s",	//read
							varname);				//dummy

						//Record 33 (Part 2)
						fgets(envtfpointerfile, MAXNAMESIZE, inputfile_fp);	//Read name of time function pointer grid file

						//strip leading blanks, trailing blanks, and final carriage return from string
						envtfpointerfile = StripString(envtfpointerfile);

						//Echo overland environmental time function pointer file name to file
						fprintf(echofile_fp, "\n\nTime function Pointer File: %s\n",
							envtfpointerfile);

						//Read overland fpoc time function pointer file for soil stack
						ReadOverlandFpocTFPointerFile(isolid, ilayer);

					}	//end if nfpocovtf[isolid] > 0

				}	//loop over layers (in reverse order)

				//if time functions are specified
				if(nfpocovtf[isolid] > 0)
				{
					//Loop over number of time functions
					for(itf=1; itf<=nfpocovtf[isolid]; itf++)
					{
						//Record 34 (Part 1)
						fscanf(inputfile_fp, "%s %d",	//read
							varname,		//dummy
							&tfid);			//time function id

						//Record 34 (Part 2)
						fgets(tfname, MAXNAMESIZE, inputfile_fp);	//Read time function name/description

						//Echo function id and description to file
						fprintf(echofile_fp, "\n\n%s %d  %s %s\n",
							"Function: ", tfid,
							"Description: ", tfname);

						//check function id for error...
						//
						//if tfid does not equal the expected value (tfid != itf)
						if(tfid != itf)
						{
							//Write error message to file
							fprintf(echofile_fp, "\n\n\nOverland Fpoc Time Function Error:\n");
							fprintf(echofile_fp,       "  Function ID Read = %d\n", tfid);
							fprintf(echofile_fp,       "  Function ID Expected = %d\n", itf);

							//Write error message to screen
							printf(              "\n\n\nOverland Fpoc Time Function Error:\n");
							printf(                    "  Function ID Read = %d\n", tfid);
							printf(                    "  Function ID Expected = %d\n", itf);

							exit(EXIT_FAILURE);					//abort

						}	//end if tfid != itf

						//Record 35
						fscanf(inputfile_fp, "%s %f %s %f %s %f",	//read
							varname,				//dummy
							&convunits,				//units conversion factor
							varname,				//dummy
							&convtime,				//time conversion factor
							varname,				//dummy
							&scale);				//scale factor

						//Echo converson and scale factors to file
						fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
						fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
						fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

						//Record 36
						fscanf(inputfile_fp, "%s %d",		//read
							varname,						//dummy
							&nfpocovtfpairs[isolid][itf]);	//number of pairs in time function series

						//Echo number of pairs to file
						fprintf(echofile_fp, "\n\n %s %d\n",
							"Number of Pairs: ", nfpocovtfpairs[isolid][itf]);

						//Allocate remaining memory for overland fpoc time functions
						fpocovtf[isolid][itf] = (float *)malloc((nfpocovtfpairs[isolid][itf]+1) * sizeof(float));	//allocate memory
						fpocovtftime[isolid][itf] = (float *)malloc((nfpocovtfpairs[isolid][itf]+1) * sizeof(float));	//allocate memory

						//Write label for function time series to file
						fprintf(echofile_fp, "  Function Value    ");	//time function units vary...
						fprintf(echofile_fp, "  Time (days)  \n");
						fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~  ");
						fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

						//Loop over number of pairs in current function for current solids type
						for(k=1; k<=nfpocovtfpairs[isolid][itf]; k++)
						{
							//Record 37
							fscanf(inputfile_fp, "%f %f",	//read
								&fpocovtf[isolid][itf][k],		//time function value
								&fpocovtftime[isolid][itf][k]);	//time function time break

							//Apply conversion and scale factors
							fpocovtf[isolid][itf][k] = fpocovtf[isolid][itf][k] * convunits * convtime * scale;

							//Echo function value and time break to file
							fprintf(echofile_fp, "%18.3f  %15.3f\n",
								fpocovtf[isolid][itf][k], fpocovtftime[isolid][itf][k]);

						}	//end loop over number of pairs

					}	//end loop over number of time functions

				}	//end if nfpocovtf[isolid] > 0

			}	//end loop over solids

		}	//end if fpocovopt > 0

		//Channel particle foc
		//
		//if channels are simulated (chnopt > 0)
		if(chnopt > 0)
		{
			//Allocate initial memory for channel particle properties
			fpocch = (float ****)malloc((nsolids+1) * sizeof(float ***));		//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
			fpocchtfid = (int ****)malloc((nsolids+1) * sizeof(int ***));		//channel particle property time function id (pointer) [isolid][row][col][ilayer]
			nfpocchtf = (int *)malloc((nsolids+1) * sizeof(int));				//number of channel particle time functions [isolid]
			nfpocchtfpairs = (int **)malloc((nsolids+1) * sizeof(int *));		//number of time series pairs in channel particle time function [isolid][itf]
			fpocchtf = (float ***)malloc((nsolids+1) * sizeof(float **));		//channel particle time function value [isolid][itf][ipairs]
			fpocchtftime = (float ***)malloc((nsolids+1) * sizeof(float **));	//channel partcle time function time break [isolid][itf][ipairs]


			//Loop over solids
			for(isolid=1; isolid<=nsolids; isolid++)
			{
				//Allocate additional memory for channel particle properties
				fpocch[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));		//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
				fpocchtfid[isolid] = (int ***)malloc((nlinks+1) * sizeof(int **));		//channel particle property time function id (pointer) [isolid][row][col][ilayer]

				//Loop over links
				for(i=1; i<=nlinks; i++)
				{
					//Allocate additional memory for channel particle properties
					fpocch[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));		//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
					fpocchtfid[isolid][i] = (int **)malloc((nnodes[i]+1) * sizeof(int *));		//channel particle property time function id (pointer) [isolid][row][col][ilayer]

					//Loop over nodes
					for(j=1; j<=nnodes[i]; j++)
					{
						//Allocate remaining memory for channel particle properties
						fpocch[isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));			//channel paricle fraction organic carbon (dimensionless) [isolid][link][node][layer]
						fpocchtfid[isolid][i][j] = (int *)malloc((maxstackch+1) * sizeof(int));			//channel particle property time function id (pointer) [isolid][row][col][ilayer]

						//loop over layers (include 0 for water column)
						for(k=0; k<=maxstackch; k++)
						{
							//set default values to 1.0 for fpoc and 0 for tfid...
							fpocch[isolid][i][j][k] = 1.0;			//channel particle fraction organic carbon (dimensionless) [isolid][link][node][layer]
							fpocchtfid[isolid][i][j][k] = 0;		//channel particle property time function id (pointer) [isolid][row][col][ilayer]

						}	//end loop over layers

					}	//end loop over nodes

				}	//end loop over links

			}	//end loop over solids

			//Read channel particle properties
			//
			//Record 38
			fscanf(inputfile_fp, "%s %d",	//read
				varname,					//dummy
				&fpocchopt);				//channel particle foc option

			//Echo foc option to file
			fprintf(echofile_fp, "\nChannel particle foc option = %d\n", fpocchopt);

			//if fpocchopt > 0 (channel particle fpoc is specified)
			if(fpocchopt > 0)
			{
				//loop over number of solids
				for(isolid=1; isolid<=nsolids; isolid++)
				{
					//Record 39
					fscanf(inputfile_fp, "%s %d %s %f %s %d",	//read
						pname,					//dummy
						&pid,					//particle (solids type) identification number
						varname,				//dummy
						&scale,					//scale factor
						varname,				//dummy
						&ntf);					//number of time functions for this particle type

					//check particle id for error...
					//
					//if pid does not equal the expected value (pid != isolid)
					if(pid != isolid)
					{
						//Write error message to file
						fprintf(echofile_fp, "\n\n\nChannel Particle Fpoc Error:\n");
						fprintf(echofile_fp,       "  Particle ID Read = %d\n", pid);
						fprintf(echofile_fp,       "  Particle ID Expected = %d\n", isolid);

						//Write error message to screen
						printf(              "\n\n\nChannel Particle Fpoc Error:\n");
						printf(                    "  Particle ID Read = %d\n", pid);
						printf(                    "  Particle ID Expected = %d\n", isolid);

						exit(EXIT_FAILURE);					//abort

					}	//end if pid != isolid

					//Assign number of time functions for this propery
					nfpocchtf[isolid] = ntf;

					//Echo environmental properties to file
					fprintf(echofile_fp, "\n\n  Specification of Channel Particle Fpoc Values  ");
					fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
					fprintf(echofile_fp,   "\n  Particle Descrption = %s", pname);
					fprintf(echofile_fp,   "\n  Particle ID = %d", isolid);
					fprintf(echofile_fp,   "\n  Scale Factor = %f", scale);
					fprintf(echofile_fp,   "\n  Number of Time Functions for Solid %d = %d\n\n",
						isolid, nfpocchtf[isolid]);

					//Allocate additional memory for overland particle properties
					nfpocchtfpairs[isolid] = (int *)malloc((nfpocchtf[isolid]+1) * sizeof(int *));		//number of time series pairs in channel particle time function [isolid][itf]
					fpocchtf[isolid] = (float **)malloc((nfpocchtf[isolid]+1) * sizeof(float *));		//channel particle time function value [isolid][itf][ipairs]
					fpocchtftime[isolid] = (float **)malloc((nfpocchtf[isolid]+1) * sizeof(float *));	//channel partcle time function time break [isolid][itf][ipairs]

					//Record 40 (Part 1)
					fscanf(inputfile_fp, "%s",	//read
						varname);				//dummy

					//Record 40 (Part 2)
					fgets(envpropfile, MAXNAMESIZE, inputfile_fp);	//Read name of channel fpoc file

					//strip leading blanks, trailing blanks, and final carriage return from string
					envpropfile = StripString(envpropfile);

					//Echo channel fpoc file name to file
					fprintf(echofile_fp, "\n\nSolid: %d  Property File: %s\n",
						isolid, envpropfile);

					//Read channel fpoc file (for water column and sediment)
					ReadChannelFpocFile(isolid, scale);

					//if time functions are specified
					if(nfpocchtf[isolid] > 0)
					{
						//Loop over number of time functions
						for(itf=1; itf<=nfpocchtf[isolid]; itf++)
						{
							//Record 41 (Part 1)
							fscanf(inputfile_fp, "%s %d",	//read
								varname,		//dummy
								&tfid);			//time function id

							//Record 41 (Part 2)
							fgets(tfname, MAXNAMESIZE, inputfile_fp);	//Read time function name/description

							//Echo function id and description to file
							fprintf(echofile_fp, "\n\n%s %d  %s %s\n",
								"Function: ", tfid,
								"Description: ", tfname);

							//check function id for error...
							//
							//if tfid does not equal the expected value (tfid != itf)
							if(tfid != itf)
							{
								//Write error message to file
								fprintf(echofile_fp, "\n\n\nChannel Fpoc Time Function Error:\n");
								fprintf(echofile_fp,       "  Function ID Read = %d\n", tfid);
								fprintf(echofile_fp,       "  Function ID Expected = %d\n", itf);

								//Write error message to screen
								printf(              "\n\n\nChannel Fpoc Time Function Error:\n");
								printf(                    "  Function ID Read = %d\n", tfid);
								printf(                    "  Function ID Expected = %d\n", itf);

								exit(EXIT_FAILURE);					//abort

							}	//end if tfid != itf

							//Record 42
							fscanf(inputfile_fp, "%s %f %s %f %s %f",	//read
								varname,				//dummy
								&convunits,				//units conversion factor
								varname,				//dummy
								&convtime,				//time conversion factor
								varname,				//dummy
								&scale);				//scale factor

							//Echo converson and scale factors to file
							fprintf(echofile_fp, "Units Conversion Factor = %f\n", convunits);
							fprintf(echofile_fp, "Time Conversion Factor = %f\n", convtime);
							fprintf(echofile_fp, "Scale Factor = %f\n\n", scale);

							//Record 43
							fscanf(inputfile_fp, "%s %d",		//read
								varname,						//dummy
								&nfpocchtfpairs[isolid][itf]);	//number of pairs in time function series

							//Echo number of pairs to file
							fprintf(echofile_fp, "\n\n %s %d\n",
								"Number of Pairs: ", nfpocchtfpairs[isolid][itf]);

							//Allocate remaining memory for channel fpoc time functions
							fpocchtf[isolid][itf] = (float *)malloc((nfpocchtfpairs[isolid][itf]+1) * sizeof(float));	//allocate memory
							fpocchtftime[isolid][itf] = (float *)malloc((nfpocchtfpairs[isolid][itf]+1) * sizeof(float));	//allocate memory

							//Write label for function time series to file
							fprintf(echofile_fp, "  Function Value    ");	//dimensionless...
							fprintf(echofile_fp, "  Time (days)  \n");
							fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~  ");
							fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

							//Loop over number of pairs in current function for current solids type
							for(k=1; k<=nfpocchtfpairs[isolid][itf]; k++)
							{
								//Record 44
								fscanf(inputfile_fp, "%f %f",	//read
									&fpocchtf[isolid][itf][k],		//time function value
									&fpocchtftime[isolid][itf][k]);	//time function time break

								//Apply conversion and scale factors
								fpocchtf[isolid][itf][k] = fpocovtf[isolid][itf][k] * convunits * convtime * scale;

								//Echo function value and time break to file
								fprintf(echofile_fp, "%18.3f  %15.3f\n",
									fpocchtf[isolid][itf][k], fpocchtftime[isolid][itf][k]);

							}	//end loop over number of pairs

						}	//end loop over number of time functions

					}	//end if nfpocchtf[isolid] > 0

				}	//end loop over solids

			}	//end if fpocchopt > 0

		}	//end if chanopt > 0

	}	//end if ksim > 2

	//Close the Echo File
	fclose(echofile_fp);

//End of function: Return to ReadInputFile
}
