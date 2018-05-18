/*---------------------------------------------------------------------
C-  Function:   ReadDataGroupC.c
C-
C-  Purpose/    ReadDataGroupC is called at the start of the simulation
C-  Methods:    to read Data Group C (sediment transport simulation
C-              parameters) from the model input file.
C-
C-
C-  Inputs:     None
C-
C-  Outputs:    None
C-
C-  Controls:   ksim, chnopt, depovopt, ersovopt, depchopt, erschopt,
C-              tnsopt, elevopt, cncopt, infopt, dbcopt, nsyield
C-
C-  Calls:      SolidsPropertyInit,
C-              ReadSoilTypeFile,
C-              ReadSoilStackFile,
C-              ReadLandUseFile,
C-              ReadInitialSolidsOverlandFile
C-              ReadSedimentPropertiesFile,
C-              ReadInitialSolidsChannelFile,
C-              StripString
C-
C-  Called by:  ReadInputFile
C-
C-  Created:    Mark Velleux
C-              John England
C-              Aditya Maroo
C-
C-  Date:       15-JUL-2003
C-
C-  Revised:    Mark Velleux
C-              HydroQual, Inc.
C-              1200 MacArthur Boulevard
C-              Mahwah, NJ  07430
C-
C-  Date:       29-APR-2008
C-
C-  Revisions:  Added overland distributed loads
C-
C-  Revised:    Mark Velleux
C-              HDR Engineering, Inc.
C-              1 International Boulevard
C-              10 Floor, Suite 1000
C-              Mahwah, NJ  07495
C-
C-  Date:       28-SEP-2017
C-
C-  Revisions:  Added user-defined inputs for overland soil transport
C-              capacity calculations: ktcov, betatcov, gammatcov,
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

void ReadDataGroupC()
{
	//local variable declarations/definitions
	int
		isolid,			//loop index for solids type
		ilayer,			//layer in the soil/sediment stack
		nfields,		//number of fields (data groupings) of solids reaction constants
		nconstants,		//number of constants in a field identifying solids reactions
		sid,			//number to identify a solids reaction constant
		iload;			//load time function identifier

	float
		convunits,		//conversion factors for length, area, volume or mass units
		convtime,		//conversion factor for time units
		scale,			//scale factor
		svalue;			//value of a solids reaction constant

	float
		gsdovtot;		//Overland grain size distribution total

	char
		fieldname[MAXNAMESIZE],		//name of field for chemical properties
		sname[MAXNAMESIZE];			//name of chemical property/constant

	//open the echo file in append mode (append to existing file)
	echofile_fp = fopen(echofile, "a");

	//write message to screen
	printf("\n\n***************************\n");
	printf(	   "*                         *\n");
	printf(	   "*   Reading Data Group C  *\n");
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
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header from file

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s\n\n", header);

	//Record 2
	fscanf(inputfile_fp, "%s %d %s %d",	//read
		varname,						//dummy
		&nsolids,						//Number of Solids Classes
		varname,						//dummy
		&nsgroups);						//Number of Solids Reporting Groups

	//Echo number of solids classes to file
	fprintf(echofile_fp, "\nNumber of Solids Classes = %d\n", nsolids);

	//if the number of solids types < 1, abort...
	if(nsolids < 1)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  Number of solids classes must be >= 1\n");
		fprintf(echofile_fp,       "  User selected nsolids = %d\n", nsolids);
		fprintf(echofile_fp,       "  Select nsolids >= 1");

		//Write error message to screen
		printf(              "\n\n\nData Group Error:\n");
		printf(                   "  Number of solids classes must be >= 1\n");
		printf(                   "  User selected nsolids = %d\n", nsolids);
		printf(                   "  Select nsolids >= 1");

		exit(EXIT_FAILURE);					//abort

	}	//end if nsolids < 1

	//Echo number of solids reporting groups to file
	fprintf(echofile_fp, "\nNumber of Solids Reporting Groups = %d\n", nsgroups);

	//if the number of solids reporting groups < 1 or > nsolids, abort...
	if(nsgroups < 1 || nsgroups > nsolids)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  Number of solids reporting groups must be >= 1\n");
		fprintf(echofile_fp,       "    and <= nsolids\n");
		fprintf(echofile_fp,       "  User selected nsolids = %d\n", nsolids);
		fprintf(echofile_fp,       "  User selected nsgroups = %d\n", nsgroups);

		//Write error message to screen
		printf(              "\n\n\nData Group Error:\n");
		printf(                   "   Number of solids reporting groups must be >= 1\n");
		printf(                   "    and <= nsolids\n");
		printf(                   "  User selected nsolids = %d\n", nsolids);
		printf(                   "  User selected nsgroups = %d\n", nsgroups);

		exit(EXIT_FAILURE);					//abort

	}	//end if nsgroups < 1 or nsgroups > nsolids

	//Record 3
	fscanf(inputfile_fp, "%s %d %s %f %s %d %s %f %s %d %s %f %s %d %s %f %s %d %s %f %s %d",	//read
		varname,						//dummy
		&advovopt,						//advection overland option
		varname,						//dummy
		&advovscale,					//advection overland scale factor
		varname,						//dummy
		&dspovopt,						//dispersion overland option
		varname,						//dummy
		&dspovscale,					//dispersion overland scale factor
		varname,						//dummy
		&depovopt,						//deposition overland option
		varname,						//dummy
		&depovscale,					//deposition overland scale factor
		varname,						//dummy
		&ersovopt,						//erosion overland option
		varname,						//dummy
		&ersovscale,					//erosion overland scale factor
		varname,						//dummy
		&tnsovopt,						//solids kinetics (transformations) overland option
		varname,						//dummy
		&tnsovscale,					//solids kinetics (transformations) overland scale factor
		varname,						//dummy
		&elevovopt);					//overland elevation update option

	//Echo label for solids overland transport bypass and process options to file
	fprintf(echofile_fp, "\n\n  Solids Transport Bypass and Process Options\n");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Echo overland advective transport bypass and process option to file
	fprintf(echofile_fp, "\nOverland Advection Option = %d   Scale Factor = %6.3f\n",
		advovopt, advovscale);

	//Echo overland dispersive transport bypass and process option to file
	fprintf(echofile_fp, "\nOverland Dispersion Option = %d   Scale Factor = %6.3f\n",
		dspovopt, dspovscale);

	//Echo overland deposition (settling) transport bypass and process option to file
	fprintf(echofile_fp, "\nOverland Deposition Option = %d   Scale Factor = %6.3f\n",
		depovopt, depovscale);

	//Echo overland erosion (resuspension) and process option to file
	fprintf(echofile_fp, "\nOverland Erosion Option = %d   Scale Factor = %6.3f\n",
		ersovopt, ersovscale);

	//Echo overland solids transformations bypass and process option to file
	fprintf(echofile_fp, "\nOverland Kinteics Option = %d   Scale Factor = %6.3f\n",
		tnsovopt, tnsovscale);

	//Echo overland elevation update option to file
	fprintf(echofile_fp, "\nOverland Elevation Update Option = %d\n",
		elevovopt);

	//if channels are simulated
	if(chnopt > 0)
	{
		//Record 4
		fscanf(inputfile_fp, "%s %d %s %f %s %d %s %f %s %d %s %f %s %d %s %f %s %d %s %f %s %d",	//read
			varname,						//dummy
			&advchopt,						//advection channel option
			varname,						//dummy
			&advchscale,					//advection channel scale factor
			varname,						//dummy
			&dspchopt,						//dispersion channel option
			varname,						//dummy
			&dspchscale,					//dispersion channel scale factor
			varname,						//dummy
			&depchopt,						//deposition channel option
			varname,						//dummy
			&depchscale,					//deposition channel scale factor
			varname,						//dummy
			&erschopt,						//erosion channel option
			varname,						//dummy
			&erschscale,					//erosion channel scale factor
			varname,						//dummy
			&tnschopt,						//solids kinetics (transformations) channel option
			varname,						//dummy
			&tnschscale,					//solids kinetics (transformations) channel scale factor
			varname,						//dummy
			&elevchopt);					//channel elevation update option

		//Echo channel advective transport bypass and process option to file
		fprintf(echofile_fp, "\nChannel Advection Option = %d   Scale Factor = %6.3f\n",
			advchopt, advchscale);

		//Echo channel dispersive transport bypass and process option to file
		fprintf(echofile_fp, "\nChannel Dispersion Option = %d   Scale Factor = %6.3f\n",
			dspchopt, dspchscale);

		//Echo channel deposition (settling) transport bypass and process option to file
		fprintf(echofile_fp, "\nChannel Deposition Option = %d   Scale Factor = %6.3f\n",
			depchopt, depchscale);

		//Echo channel erosion (resuspension) and process option to file
		fprintf(echofile_fp, "\nChannel Erosion Option = %d   Scale Factor = %6.3f\n",
			erschopt, erschscale);

		//Echo channel solids transformations bypass and process option to file
		fprintf(echofile_fp, "\nChannel Kinteics Option = %d   Scale Factor = %6.3f\n",
			tnschopt, tnschscale);

		//Echo channel elevation update option to file
		fprintf(echofile_fp, "\nChannel Elevation Update Option = %d\n",
			elevchopt);

	}	//end if chnopt > 0

	//Housekeeping: skip extra carriage return
	fgets(header, MAXHEADERSIZE, inputfile_fp);

	//Record 5
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Write label for solids reporting group names to file
	fprintf(echofile_fp, "\n\n  Solids Reporting Groups  ");
	fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	fprintf(echofile_fp,   "\n  Group Number    ");
	fprintf(echofile_fp,     "          Group Name          ");
	fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for solids reporting group names
	sgroupname = (char **)malloc((nsgroups+1) * sizeof(char *));	//allocate memory

	//Loop over number of solids reporting groups
	for(i=1; i<=nsgroups; i++)
	{
		//Allocate remaining memory for solids reporting group names
		sgroupname[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 6 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 6 (Part 2)
		fgets(sgroupname[i], MAXNAMESIZE, inputfile_fp);	//Read name of solids (particle) group

		//strip leading blanks, trailing blanks, and final carriage return from string
		sgroupname[i] = StripString(sgroupname[i]);

		//Echo solids reporting group names to file
		fprintf(echofile_fp, "%16d  %s\n",
			i, sgroupname[i]);

	}	//end loop over nsgroups

	//Record 7
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Write label for particle characteristics to file
	fprintf(echofile_fp, "\n\nSolids Class");
	fprintf(echofile_fp,     "  Mean Diameter (m)");
	fprintf(echofile_fp,     "  Specific Gravity");
	fprintf(echofile_fp,     "  Settling Velocity (m/s)");

	//if any of the overland or channel deposition options > 1 or 2 erosion options > 2
	if(depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2)
	{
		//Write label for cohesive/nocohesive option
		fprintf(echofile_fp, "  CNCOPT");

	}	//end if depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2

	//if the overland deposition option > 1
	if(depovopt > 1)
	{
		//Write label for overland deposition parameters
		fprintf(echofile_fp, "  TauCDepOv (N/m2)");

	}	//end if depovopt > 1

	//if the overland erosion option > 2
	if(ersovopt > 2)
	{
		//Write label for overland erosion parameters
		fprintf(echofile_fp, "  TauCErsOv (N/m2)");

		//Write label for overland erosion parameters
		fprintf(echofile_fp, "  ZOv (dimensionless)");

	}	//end if ersovopt > 2

	//if channels are simulated
	if(chnopt > 0)
	{
		//if the channel deposition option > 1
		if(depchopt > 1)
		{
			//Write label for channel deposition parameters
			fprintf(echofile_fp, "  TauCDepCh (N/m2)");

		}	//end if depchopt > 1

		//if the channel erosion option <= 2
		if(erschopt <= 2)
		{
			//Write label for channel erosion parameters
			fprintf(echofile_fp, "  VcritCh (m/s)");
		}
		//else if the channel erosion option > 2
		else if(erschopt > 2)
		{
			//Write label for channel erosion parameters
			fprintf(echofile_fp, "  TauCErsCh (N/m2)");

			//Write label for channel erosion parameters
			fprintf(echofile_fp, "  ZCh (dimensionless)");

		}	//end if erschopt <= 2

	}	//end if chnopt > 0

	fprintf(echofile_fp, "  Reporting Group");
	fprintf(echofile_fp, "  Particle Type Description\n");

	fprintf(echofile_fp,   "------------");
	fprintf(echofile_fp, "  -----------------");
	fprintf(echofile_fp, "  ----------------");
	fprintf(echofile_fp, "  -----------------------");

	//if any of the overland or channel deposition options > 1 or 2 erosion options > 2
	if(depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2)
	{
		//Write label for cohesive/nocohesive option
		fprintf(echofile_fp, "  ------");

	}	//end if depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2

	//if the overland deposition option > 1
	if(depovopt > 1)
	{
		//Write label for overland deposition parameters
		fprintf(echofile_fp, "  ----------------");

	}	//end if depovopt > 1

	//if the overland erosion option > 2
	if(ersovopt > 2)
	{
		//Write label for overland erosion parameters
		fprintf(echofile_fp, "  ----------------");

		//Write label for overland erosion parameters
		fprintf(echofile_fp, "  -------------------");

	}	//end if ersovopt > 2

	//if channels are simulated
	if(chnopt > 0)
	{
		//if the channel deposition option > 1
		if(depchopt > 1)
		{
			//Write label for channel deposition parameters
			fprintf(echofile_fp, "  ----------------");

		}	//end if depchopt > 1

		//if the channel erosion option <= 2
		if(erschopt <= 2)
		{
			//Write label for channel erosion parameters
			fprintf(echofile_fp, "  -------------");
		}
		//else if the channel erosion option > 2
		else if(erschopt > 2)
		{
			//Write label for channel erosion parameters
			fprintf(echofile_fp, "  ----------------");

			//Write label for channel erosion parameters
			fprintf(echofile_fp, "  -------------------");

		}	//end if erschopt <= 2

	}	//end if chnopt > 0

	fprintf(echofile_fp, "  ---------------");
	fprintf(echofile_fp, "  -------------------------\n");

	//Allocate memory for particle characteristics
	ds = (float *)malloc((nsolids+1) * sizeof(float));				//particle diameter (m)
	spgravity = (float *)malloc((nsolids+1) * sizeof(float));		//particle specific gravity (dimensionless)
	ws = (float *)malloc((nsolids+1) * sizeof(float));				//particle settling (fall) velocity (m/s)

	//if any of the overland or channel deposition options > 1 or 2 erosion options > 2
	if(depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2)
	{
		cncopt = (int *)malloc((nsolids+1) * sizeof(int));		//cohesive/non-cohesive option

	}	//end if depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2

	//if the overland deposition option > 1
	if(depovopt > 1)
	{
		tcdov = (float *)malloc((nsolids+1) * sizeof(float));	//tau critical for deposition overland (N/m2)

	}	//end if depovopt > 1

	//if the overland erosion option > 2
	if(ersovopt > 2)
	{
		tceov = (float *)malloc((nsolids+1) * sizeof(float));	//tau critical for erosion overland (N/m2)
		zageov = (float *)malloc((nsolids+1) * sizeof(float));		//sediment aging factor overland (dimensionless)

	}	//end if ersovopt > 2

	//if channels are simulated
	if(chnopt > 0)
	{
		//if the channel deposition option > 1
		if(depchopt > 1)
		{
			tcdch = (float *)malloc((nsolids+1) * sizeof(float));	//tau critical for deposition in channels (N/m2)

		}	//end if depchopt > 1

		//if the channel erosion option <= 2
		if(erschopt <= 2)
		{
			vcch = (float *)malloc((nsolids+1) * sizeof(float));	//critical velocity for erosion in channels (m/s)
		}
		//else if the channel erosion option > 2
		else if(erschopt > 2)
		{
			tcech = (float *)malloc((nsolids+1) * sizeof(float));	//tau critical for erosion in channels (N/m2)
			zagech = (float *)malloc((nsolids+1) * sizeof(float));	//sediment aging factor in channels (dimensionless)

		}	//end if erschopt <= 2

	}	//end if chnopt > 0

	//Allocate memory for reporting group and particle name
	sgroupnumber = (int *)malloc((nsolids+1) * sizeof(int));		//solids reporting group number
	particlename = (char **)malloc((nsolids+1) * sizeof(char *));	//name of particle type

	//Loop over number of solids
	for(i=1; i<=nsolids; i++)
	{
		//Record 8a
		fscanf(inputfile_fp, "%f %f %f",	//read
			&ds[i],							//particle mean diameter (m)
			&spgravity[i],					//particle specific gravity (dimensionless)
			&ws[i]);						//particle setling velocity (omega) (m/s)

		//Echo particle characteristics to file
		fprintf(echofile_fp, "%11d  %17.8f  %17.8f  %23.4e",
			i, ds[i], spgravity[i], ws[i]);

		//if any of the overland or channel deposition options > 1 or 2 erosion options > 2
		if(depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2)
		{
			//Record 8b
			fscanf(inputfile_fp, "%d",	//read
				&cncopt[i]);			//particle cohesive/non-cohesive option

			//Echo particle characteristics to file
			fprintf(echofile_fp, "  %6d", cncopt[i]);

			//if the overland deposition option > 1
			if(depovopt > 1)
			{
				//Record 8c
				fscanf(inputfile_fp, "%f",	//read
					&tcdov[i]);				//particle critical shear stress for deposition overland (N/m2)

				//Echo particle characteristics to file
				fprintf(echofile_fp, "  %16.8f", tcdov[i]);

				//Error check...
				//
				//if tcdov[i] <= 0.0
				if(tcdov[i] <= 0.0)
				{
					//Write error message to file
					fprintf(echofile_fp, "\n\n\nParticle Propery Error:\n");
					fprintf(echofile_fp,       "  Tau Critical for Overland Deposition must be > 0.0\n");
					fprintf(echofile_fp,       "  User selected tcdov = %f\n", tcdov[i]);
					fprintf(echofile_fp,       "  Particle type = %d\n", i);

					//Write error message to screen
					printf(              "\n\n\nParticle Propery Error:\n");
					printf(                    "  Tau Critical for Overland Deposition must be > 0.0\n");
					printf(                    "  User selected tcdov = %f\n", tcdov[i]);
					printf(                    "  Particle type = %d\n", i);

					exit(EXIT_FAILURE);					//abort

				}	//end if tcdov <= 0.0

			}	//end if depovopt > 1

			//if the overland erosion option > 2
			if(ersovopt > 2)
			{
				//Record 8d
				fscanf(inputfile_fp, "%f %f",	//read
					&tcdov[i],				//particle critical shear stress for erosion overland (N/m2)
					&zageov[i]);			//particle aging factor overland (dimensionless)

				//Echo particle characteristics to file
				fprintf(echofile_fp, "  %16.4f  %19.4f",
					tceov[i], zageov[i]);

			}	//end if ersovopt > 2

			//if channels are simulated
			if(chnopt > 0)
			{
				//if the channel deposition option > 1
				if(depchopt > 1)
				{
					//Record 8e
					fscanf(inputfile_fp, "%f",	//read
						&tcdch[i]);				//particle critical shear stress for deposition in channels (N/m2)

					//Echo particle characteristics to file
					fprintf(echofile_fp, "  %16.8f", tcdch[i]);

					//Error check...
					//
					//if tcdch[i] <= 0.0
					if(tcdch[i] <= 0.0)
					{
						//Write error message to file
						fprintf(echofile_fp, "\n\n\nParticle Propery Error:\n");
						fprintf(echofile_fp,       "  Tau Critical for Channel Deposition must be > 0.0\n");
						fprintf(echofile_fp,       "  User selected tcdch = %f\n", tcdch[i]);
						fprintf(echofile_fp,       "  Particle type = %d\n", i);

						//Write error message to screen
						printf(              "\n\n\nParticle Propery Error:\n");
						printf(                    "  Tau Critical for Channel Deposition must be > 0.0\n");
						printf(                    "  User selected tcdch = %f\n", tcdch[i]);
						printf(                    "  Particle type = %d\n", i);

						exit(EXIT_FAILURE);					//abort

					}	//end if tcdch <= 0.0

				}	//end if depchopt > 1

			}	//end if chnopt > 0

		}	//end if depovopt > 1 || depchopt > 1 || ersovopt > 2 || erschopt > 2

		//if channels are simulated
		if(chnopt > 0)
		{
			//if the channel erosion option <= 2 (transport capacity limited erosion)
			if(erschopt <= 2)
			{
				//Record 8f
				fscanf(inputfile_fp, "%f",	//read
					&vcch[i]);			//particle critical velocity (incipient motion threshold) (m/s)

				//Echo particle characteristics to file
				fprintf(echofile_fp, "  %13.8f",
					vcch[i]);
			}
			//else if the channel erosion option > 2 (erosion rate equation)
			else if(erschopt > 2)
			{
				//Record 8g
				fscanf(inputfile_fp, "%f %f",	//read
					&tcech[i],				//particle critical shear stress for erosion in channels (N/m2)
					&zagech[i]);			//particle aging factor in channels (dimensionless)

				//Echo particle characteristics to file
				fprintf(echofile_fp, "  %16.4f  %19.4f",
					tcech[i], zagech[i]);

			}	//end if erschopt <= 2

		}	//end if chnopt > 0

		//Allocate remaining memory for particle name
		particlename[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 8h (Part 1)
		fscanf(inputfile_fp, "%d",	//read
			&sgroupnumber[i]);		//solids reporting group number

		//Record 8h (Part 2)
		fgets(particlename[i], MAXNAMESIZE, inputfile_fp);		//read particle name

		//strip leading blanks, trailing blanks, and final carriage return from string
		particlename[i] = StripString(particlename[i]);

		//Echo particle characteristics to file
		fprintf(echofile_fp, "  %15d  %s",
			sgroupnumber[i], particlename[i]);

		//Write final carriage return to echofile to end this line
		fprintf(echofile_fp, "\n");

	}	//end loop over number of solids

	//if solids reactions/transformations occur in the overland plane or channel
	if(tnsovopt > 0 || tnschopt > 0)
	{
		//Record 9
		fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

		//Echo header to file
		fprintf(echofile_fp, "\n\n%s\n", header);

		//Allocate memory and set default values for solids properties
		SolidsPropertyInit();

		//Loop over number of solids
		for(i=1; i<=nsolids; i++)
		{
			//Record 10
			fscanf(inputfile_fp, "%s %d %s %d",	//read
				varname,						//dummy
				&isolid,						//solids number
				varname,						//dummy
				&nfields);						//number of property fields

			//Echo chemical number and name to file
			fprintf(echofile_fp, "\n\nSolid Number: %d", i);
			fprintf(echofile_fp,   "\nNumber of Fields (Data Groupings): %d\n", nfields);

			//Check that solid input is solid expected...
			//
			//if isolid does not equal i
			if(isolid != i)
			{
				//Write error message to file
				fprintf(echofile_fp, "\n\n\nSolids reaction specification error:\n");
				fprintf(echofile_fp,       "  Solid specified = %d", isolid);
				fprintf(echofile_fp,       "  Solid expected = %d", i);

				//Write error message to screen
				printf("\n\n\nSolids reaction specification error:\n");
				printf(      "  Solid specified = %d", isolid);
				printf(      "  Solid expected = %d", i);

				exit(EXIT_FAILURE);					//abort

			}	//end check of solids number (isolid != i)

			//Loop over number of fields for current solid
			for(j=1; j<=nfields; j++)
			{
				//Record 11 (Part 1)
				fscanf(inputfile_fp, "%d",	//read
					&nconstants);			//number of constants for current field

				//Record 11 (Part 2)
				fgets(fieldname, MAXNAMESIZE, inputfile_fp);	//read name of field

				//Echo field number and name to file
				fprintf(echofile_fp, "\n  Field Number: %d  Description: %s",
					j, fieldname);

				//Write label for output to file
				fprintf(echofile_fp, "\nConstant ID  ");
				fprintf(echofile_fp,   "Value of Constant  ");
				fprintf(echofile_fp,   "Constant Name\n");
				fprintf(echofile_fp,   "-----------  ");
				fprintf(echofile_fp,   "-----------------  ");
				fprintf(echofile_fp,   "-------------\n");

				//Loop over number of constants for this field
				for(k=1; k<=nconstants; k++)
				{
					//Record 12
					fscanf(inputfile_fp, "%s %d %f",	//read
						sname,							//name of solids reaction constant
						&sid,							//solids reaction constant identifier
						&svalue);						//value of solids reaction contant

					//Echo constant information to file
					fprintf(echofile_fp, "%11d  %17.4f  %s\n", sid, svalue, sname);

					//Assign constants...
					//
					//Process options: if option = 0, no computations for the process
					if(sid == 100)
					{
						abropt[i] = (int)(svalue);		//abrasion option

						//set flag for process control (if any solid abrades)
						abropt[0] = abropt[0] + abropt[i];
					}
					else if(sid == 200)
					{
						mnropt[i] = (int)(svalue);		//mineralization option

						//set flag for process control (if any solid mineralizes)
						mnropt[0] = mnropt[0] + mnropt[i];
					}
/*
					//Abrasion parameters
					else if(sid == 110)
					{

					}
					else if(sid == 120)
					{

					}
					else if(sid == 130)
					{

					}
					else if(sid == 140)
					{

					}
					//Mineralization parameters
					else if(sid == 210)
					{

					}
					else if(cid == 220)
					{

					}
					else if(cid == 230)
					{

					}
					else if(cid == 240)
					{

					}
					//Other process parameters here...
*/				
					//else the constant is undefined
					else
					{
						//Write error message to file
						fprintf(echofile_fp, "\n\n\nData Group Error:\n");
						fprintf(echofile_fp,       "  Undefined solids reaction constant\n");
						fprintf(echofile_fp,       "  User selected sid = %d\n", sid);
						fprintf(echofile_fp,       "  No constant defined for this identifier");

						//Write error message to screen
						printf(              "\n\n\nData Group Error:\n");
						printf(                    "  Undefined solids reaction constant\n");
						printf(                    "  User selected sid = %d\n", sid);
						printf(                    "  No constant defined for this identifier");

						exit(EXIT_FAILURE);					//abort

					}	//end if sid == a value...

				}	//end loop over constants

				//Start a new line for the next row of data in the echo file 
				fprintf(echofile_fp, "\n");

			}	//end loop over fields

		}	//end loop over chemicals

		//Record 13
		fscanf(inputfile_fp, "%s %d",	//read
			varname,					//dummy
			&nsyields);					//number of chemical reaction yields

		//Echo number of yeilds to file
		fprintf(echofile_fp, "\nNumber of Yields: %d\n", nsyields);

		//if the number of solids yields is greater than zero
		if(nsyields > 0)
		{
			//Write label for output to file
			fprintf(echofile_fp, "\n From   ");
			fprintf(echofile_fp,   " To   ");
			fprintf(echofile_fp,   " Process   ");
			fprintf(echofile_fp,   " Reaction Yield (g/g) \n");
			fprintf(echofile_fp,   "------  ");
			fprintf(echofile_fp,   "----  ");
			fprintf(echofile_fp,   "---------  ");
			fprintf(echofile_fp,   "----------------------\n");

			//Allocate memory for solids reaction yield parameters
			syldfrom = (int *)malloc((nsyields+1) * sizeof(int));		//reacting solid type
			syldto = (int *)malloc((nsyields+1) * sizeof(int));			//product solid type
			syldprocess = (int *)malloc((nsyields+1) * sizeof(int));	//solids reaction process pathway number
			syield = (float *)malloc((nsyields+1) * sizeof(float));		//solids reaction yield

			//Loop over number of solids yields
			for(i=1; i<=nsyields; i++)
			{
				//Record 14
				fscanf(inputfile_fp, "%s %d %s %d %s %d %s %f",	//read
					varname,									//dummy
					&syldfrom[i],								//reacting solid
					varname,									//dummy
					&syldto[i],									//product solid
					varname,									//dummy
					&syldprocess[i],							//reaction process pathway number
					varname,									//dummy
					&syield[i]);								//reaction yield (g/g)

				//Echo yield parameters to file
				fprintf(echofile_fp, "%6d  %4d  %9d  %22.4f\n",
					syldfrom[i], syldto[i], syldprocess[i], syield[i]);

			}	//end loop over number of chemical yields

		}	//end if ncyields > 0

	}	//end if tnsovopt > 0 or tnschopt > 0

	//Record 15 (Soil Type and Grain Size Distribution Header)
	fgets(header, MAXHEADERSIZE, inputfile_fp);		//read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s", header);

	//Record 16
	fscanf(inputfile_fp, "%s %d",	//read
		varname,					//dummy
		&nsoils);					//Number of soil types

	//Echo Number of soil types to file
	fprintf(echofile_fp, "\n\nNumber of Soil types = %d\n", nsoils);

	//Allocate memory for infiltration parameters
	//
	//if infiltration is simulated 
	if(infopt == 1)
	{
		//Allocate memory for infiltration parameters not previously allocated in Data Group B
		khsoil = (float *)malloc((nsoils+1) * sizeof(float));		//soil hydraulic conductivity
		capshsoil = (float *)malloc((nsoils+1) * sizeof(float));	//soil capillary suction head
		soilmd = (float *)malloc((nsoils+1) * sizeof(float));		//soil moisture deficit

	}	//end check of infopt

	//Allocate memory for soil erosion parameters
	//
	//if the erosion option <= 2
	if(ersovopt <= 2)
	{
		//Allocate memory
		kusle = (float *)malloc((nsoils+1) * sizeof(float));		//soil erodibility (K factor in USLE)
		vcov = (float *)malloc((nsoils+1) * sizeof(float));			//soil critical velocity for erosion
	}
	else	//else ersovopt > 2
	{
		//Allocate memory
		mexpov = (float *)malloc((nsoils+1) * sizeof(float));		//soil erosion exponent for excess shear

	}	//end if ersovopt <= 2

	//Allocate memory for parameters common to all options
	porosityov = (float *)malloc((nsoils+1) * sizeof(float));	//porosity of soil in the overland plane
	soilname = (char **)malloc((nsoils+1) * sizeof(char *));	//descriptive name of soil

	//Allocate memory for soil grain size distribution (gsd)
	gsdov = (float **)malloc((nsoils+1) * sizeof(float *));		//soil gsd (dimensionless) [isoil][isolid]

	//Loop over number of soil types and read soil erosion properties
	for(i=1; i<=nsoils; i++)
	{
		//Allocate remaining memory for soil names
		soilname[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//if there is no infiltration (infopt = 0)
		if(infopt == 0)
		{
			//if the overland erosion option <= 2
			if(ersovopt <= 2)
			{
				//Write label for soil characteristics to file (no infiltration)
				fprintf(echofile_fp, "\n\nSoil");
				fprintf(echofile_fp,   "  Kusle (tons/acre)");
				fprintf(echofile_fp,   "  Vcritical (m/s)");
				fprintf(echofile_fp,   "  Porosity (dimensionless)");
				fprintf(echofile_fp,   "  Soil Description\n");
				fprintf(echofile_fp,     "~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~\n");

				//Record 17 (Part 1)
				fscanf(inputfile_fp, "%f %f",	//read
					&kusle[i],					//K factor (soil erodibility) in usle
					&vcov[i]);					//critical velocity for erosion (m/s)

				//Echo erosion parameters to file
				fprintf(echofile_fp, "%4d  %17.5f  %15.7f", i, kusle[i], vcov[i]);
			}
			else	//else ersovopt > 2
			{
				//Write label for soil characteristics to file (no infiltration)
				fprintf(echofile_fp, "\n\nSoil");
				fprintf(echofile_fp,   "  mexpov (dimensionless)");
				fprintf(echofile_fp,   "  Porosity (dimensionless)");
				fprintf(echofile_fp,   "  Soil Description\n");
				fprintf(echofile_fp,     "~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~\n");

				//Record 18 (Part 1)
				fscanf(inputfile_fp, "%f",	//read
					&mexpov[i]);			//soil erosion exponent for excess shear

				//Echo erosion parameters to file
				fprintf(echofile_fp, "%4d  %22.5f", i, mexpov[i]);

			}	//end if ersovopt <= 2

			//Record 17/18 (Part 2)
			fscanf(inputfile_fp, "%f",	//read
				&porosityov[i]);		//overland soil porosity (dimensionless)

			//Record 17/18 (Part 3)
			fgets(soilname[i], MAXNAMESIZE, inputfile_fp);	//read soil name

			//strip leading blanks, trailing blanks, and final carriage return from string
			soilname[i] = StripString(soilname[i]);

			//Echo erosion parameters to file
			fprintf(echofile_fp, "  %24.5f  %s\n", porosityov[i], soilname[i]);
		}
		else	//if there is infiltration (may need elseif when more options are added)
		{
			//if the overland erosion option <= 2
			if(ersovopt <= 2)
			{
				//Write label for soil characteristics to file (infiltration)
				fprintf(echofile_fp, "\n\nSoil");
				fprintf(echofile_fp,   "  Kusle (tons/acre)");
				fprintf(echofile_fp,   "  Vcritical (m/s)");
				fprintf(echofile_fp,   "  Porosity (dimensionless)");
				fprintf(echofile_fp,   "  Kh (m/s)");
				fprintf(echofile_fp,   "  Capilary Suction Head (m)");
				fprintf(echofile_fp,   "  Soil Moisture Deficit (-)");
				fprintf(echofile_fp,   "  Soil Description\n");
				fprintf(echofile_fp,     "~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~\n");

				//Record 19 (Part 1)
				fscanf(inputfile_fp, "%f %f",	//read
					&kusle[i],					//K factor (soil erodibility) in usle
					&vcov[i]);					//critical velocity for erosion (m/s)

				//Echo erosion parameters to file
				fprintf(echofile_fp, "%4d  %17.5f  %15.7f", i, kusle[i], vcov[i]);
			}
			else	//else ersovopt > 2
			{
				//Write label for soil characteristics to file (infiltration)
				fprintf(echofile_fp, "\n\nSoil");
				fprintf(echofile_fp,   "  mexpov (dimensionless)");
				fprintf(echofile_fp,   "  Porosity (dimensionless)");
				fprintf(echofile_fp,   "  Kh (m/s)");
				fprintf(echofile_fp,   "  Capilary Suction Head (m)");
				fprintf(echofile_fp,   "  Soil Moisture Deficit (-)");
				fprintf(echofile_fp,   "  Soil Description\n");
				fprintf(echofile_fp,     "~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~~~~~~~~~~");
				fprintf(echofile_fp,   "  ~~~~~~~~~~~~~~~~\n");

				//Record 20 (Part 1)
				fscanf(inputfile_fp, "%f",	//read
					&mexpov[i]);			//soil erosion exponent for excess shear

				//Echo erosion parameters to file
				fprintf(echofile_fp, "%4d  %21.5f", i, mexpov[i]);

			}	//end if ersovopt <= 2

			//Record 19/20 (Part 2)
			fscanf(inputfile_fp, "%f %f %f %f",	//read
				&porosityov[i],					//overland soil porosity (dimensionless)
				&khsoil[i],						//Soil Hydraulic Conductivity (m/s)
				&capshsoil[i],					//Soil Capillary Suction Head (m)
				&soilmd[i]);					//Soil Moisture Deficit

			//Record 19/20 (Part 3)
			fgets(soilname[i], MAXNAMESIZE, inputfile_fp);	//read soil name

			//Echo erosion parameters to file
			fprintf(echofile_fp, "  %24.5f  %8.6f  %25.5f  %25.5f  %s\n",
				porosityov[i], khsoil[i], capshsoil[i], soilmd[i], soilname[i]);

		}	//end check of infopt

		//write labels (skip space too)

		//Record 21 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Allocate additional memory for soil gsd
		gsdov[i] = (float *)malloc((nsolids+1) * sizeof(float));	//soil gsd (dimensionless) [isoil][isolid]

		//initialize the sum of grain size distribution fractions
		gsdovtot = 0.0;

		//Write label for soil grain size distribution to file...
		//
		//loop over number of solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Write label for soil grain size distribution to file
			fprintf(echofile_fp, "  GSD Solid: %3d", isolid);

		}	//end loop over solids

		//Write label for soil grain size distribution sum to file
		fprintf(echofile_fp, "  GSD Sum\n");

		//loop over number of solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Write label for soil grain size distribution to file
			fprintf(echofile_fp, "  --------------");

		}	//end loop over solids

		//Write label for soil grain size distribution sum to file
		fprintf(echofile_fp, "  ---------\n");

		//Read soil grain size distribution...
		//
		//loop over solids
		for(isolid=1; isolid<=nsolids; isolid++)
		{
			//Record 21 (Part 2)
			fscanf(inputfile_fp, "%f",	//read
				&gsdov[i][isolid]);		//soil gsd (dimensionless)

			//Echo soil grain size distribution component to file
			fprintf(echofile_fp, "  %14.5f", gsdov[i][isolid]);

			//compute sum of grain size distribution fractions
			gsdovtot = gsdovtot + gsdov[i][isolid];

		}	//end loop over solids

		//Echo soil grain size distribution sum to file
		fprintf(echofile_fp, "  %9.5f\n", gsdovtot);

		//Check total value of the grain size distribution
		if((gsdovtot < 0.99999) || (gsdovtot > 1.00001))
		{
			//Write error message to file
			fprintf(echofile_fp, "Error:  Grain size distribution does not sum to 1.0!\n");

			//Write error message to screen
			printf("Error:  Grain size distribution does not sum to 1.0!\n");

			exit(EXIT_FAILURE);		//exit the program

		}	//end check summed value of the grain size distribution

	}	//end loop over number of soils

	//Housekeeping: skip extra carriage return
	fgets(header, MAXHEADERSIZE, inputfile_fp);

	//Record 22 (Land Use Clasification Header)
	fgets(header, MAXHEADERSIZE, inputfile_fp);		//Read header from file

	//Echo header to file
	fprintf(echofile_fp, "\n\n\n%s", header);

	//Record 23
	fscanf(inputfile_fp, "%s %d",	//read
		varname,					//dummy
		&nlands);					//Number of Land Use Classes

	//Echo Number of Land Use Classes to file
	fprintf(echofile_fp, "\n\nNumber of Land Use Classes = %d\n", nlands);

	//Allocate memory for land use class parameters
	nmanningov = (float *)malloc((nlands+1) * sizeof(float));			//Manning n (overland plane)
	interceptionclass = (float *)malloc((nlands+1) * sizeof(float));	//Interception depth for land class

	//if the overland erosion option <= 2
	if(ersovopt <= 2)
	{
		//Allocate memory for land use class parameters
		cusle = (float *)malloc((nlands+1) * sizeof(float));	//C (cropping) factor for usle
		pusle = (float *)malloc((nlands+1) * sizeof(float));	//P (practice) factor for usle

		//Allocate memory for generalized overland transport capacity parameters
		ktranscapov = (float *)malloc((nlands+1) * sizeof(float));	//coefficient for overland transport capacity: qs = k Q^betas Sf^gammas
		betasov = (float *)malloc((nlands+1) * sizeof(float));		//exponent for unit flow in generalized overland transport capacity
		gammasov = (float *)malloc((nlands+1) * sizeof(float));		//exponent for friction slope in generalized overland transport capacity
		barefracov = (float *)malloc((nlands+1) * sizeof(float));	//bare soil fraction for this land use class (for shear stress partitioning)
		tcwexpov = (float *)malloc((nlands+1) * sizeof(float));		//weighting exponent to apportion transport capacity by grain size (dstar^tcwexpov)
	}
	else	//else ersovopt > 2
	{
		//Allocate memory for land use class parameters
		ayov = (float *)malloc((nlands+1) * sizeof(float));		//overland sediment erosion yield (excess shear) (g/m2)

	}	//end if ersovopt <= 2

	//Allocate memory for land use class parameters
	landname = (char **)malloc((nlands+1) * sizeof(char *));	//descriptive land use name

	//Loop over number of land use classes to finish memory allocation
	for(i=1; i<=nlands; i++)
	{
		//Allocate remaining memory for land use names
		landname[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	}	//end loop over number of land uses classes

	//if the overland erosion option <= 2
	if(ersovopt <= 2)
	{
		//Write label for land use parameters to file
		fprintf(echofile_fp, "\nManning n");
		fprintf(echofile_fp, "  Interception Depth (mm)");
		fprintf(echofile_fp, "  Cusle");
		fprintf(echofile_fp, "  Pusle");

		//if the overland erosion option = 2 (generalized overland transport capacity)
		if(ersovopt == 2)
		{
			fprintf(echofile_fp, "  ktranscap (kg/m/s)");
			fprintf(echofile_fp, "  Betas (-)");
			fprintf(echofile_fp, "  Gammas (-)");
			fprintf(echofile_fp, "  Fraction Bare Soil (-)");
			fprintf(echofile_fp, "  Transport Exponent (-)");

		}	//end if ersovopt = 2

		fprintf(echofile_fp, "  Land Use Classification\n");
		fprintf(echofile_fp,   "~~~~~~~~~");
		fprintf(echofile_fp, "  ~~~~~~~~~~~~~~~~~~~~~~~");
		fprintf(echofile_fp, "  ~~~~~");
		fprintf(echofile_fp, "  ~~~~~");

		//if the overland erosion option = 2 (generalized overland transport capacity)
		if(ersovopt == 2)
		{
			fprintf(echofile_fp, "  ~~~~~~~~~~~~~~~~~~");
			fprintf(echofile_fp, "  ~~~~~~~~~");
			fprintf(echofile_fp, "  ~~~~~~~~~~");
			fprintf(echofile_fp, "  ~~~~~~~~~~~~~~~~~~~~~~");
			fprintf(echofile_fp, "  ~~~~~~~~~~~~~~~~~~~~~~");

		}	//end if ersovopt = 2

		fprintf(echofile_fp, "  ~~~~~~~~~~~~~~~~~~~~~~~\n");

		//Loop over number of land use classes to read land use parameters
		for(i=1; i<=nlands; i++)
		{
			//Record 24 (Part 1)
			fscanf(inputfile_fp, "%f %f %f %f",		//read
				&nmanningov[i],			//Manning n for this land use class
				&interceptionclass[i],	//interception depth (mm) for this land use class
				&cusle[i],				//C factor (in USLE) for this land use class
				&pusle[i]);				//P factor (in USLE) for this land use class

			//if the overland erosion option = 2 (generalized overland transport capacity)
			if(ersovopt == 2)
			{
				//Record 24 (Part 2)
				fscanf(inputfile_fp, "%f %f %f %f %f",		//read
					&ktranscapov[i],	//coefficient for generalized overland transport capacity: qs = k Q^betas Sf^gammas
					&betasov[i],		//exponent for unit flow in generalized overland transport capacity
					&gammasov[i],		//exponent for friction slope in generalized overland transport capacity
					&barefracov[i],		//bare soil fraction for this land use class (for shear stress partitioning)
					&tcwexpov[i]);		//weighting exponent to apportion transport capcity by grain size (dstar ^ tcwexpov)

			}	//end if ersovopt = 2

			//Record 24 (Part 3)
			fgets(landname[i], MAXNAMESIZE, inputfile_fp);	//Read Land Use Class Name

			//Echo land use parameters for the current land use class to file
			fprintf(echofile_fp, "%9.5f  %23.3f  %5.3f  %5.3f",
				nmanningov[i],
				interceptionclass[i],
				cusle[i],
				pusle[i]);

			//if the overland erosion option = 2 (generalized overland transport capacity)
			if(ersovopt == 2)
			{
				//Echo transport capacity parameters for the current land use class to file
				fprintf(echofile_fp, "  %18.4f  %9.4f  %9.4f  %22.4f  %22.4f",
					ktranscapov[i],
					betasov[i],
					gammasov[i],
					barefracov[i],
					tcwexpov[i]);

			}	//end if ersovopt = 2

			//Echo name for this land use class to file
			fprintf(echofile_fp, "%s",
				landname[i]);

		}	//End loop over land use classes
	}
	else	//else ersovopt > 2
	{
		//Write label for land use parameters to file
		fprintf(echofile_fp, "\nManning n");
		fprintf(echofile_fp, "  Interception Depth (mm)");
		fprintf(echofile_fp, "  ayov (g/m2)");
		fprintf(echofile_fp, "  Land Use Classification\n");
		fprintf(echofile_fp,   "~~~~~~~~~");
		fprintf(echofile_fp, "  ~~~~~~~~~~~~~~~~~~~~~~~");
		fprintf(echofile_fp, "  ~~~~~~~~~~~");
		fprintf(echofile_fp, "  ~~~~~~~~~~~~~~~~~~~~~~~\n");

		//Loop over number of land use classes to read land use parameters
		for(i=1; i<=nlands; i++)
		{
			//Record 25 (Part 1)
			fscanf(inputfile_fp, "%f %f %f",		//read
				&nmanningov[i],			//Manning n for this land use class
				&interceptionclass[i],	//interception depth (mm) for this land use class
				&ayov[i]);				//erosion yield for this land use class

			//Record 25 (Part 2)
			fgets(landname[i], MAXNAMESIZE, inputfile_fp);	//Read Land Use Class Name

			//Echo land use parameters for the current land use class to file
			fprintf(echofile_fp, "%9.5f  %22.3f  %11.3f  %s",
				nmanningov[i],
				interceptionclass[i],
				ayov[i],
				landname[i]);

		}	//End loop over land use classes

	}	//end if ersovopt <= 2

	//Allocate memory for land use classification file name
	landusefile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 26 (Part 1)
	fscanf(inputfile_fp, "%s",	//read
		varname);				//dummy

	//Record 26 (Part 2)
	fgets(landusefile, MAXNAMESIZE, inputfile_fp);	//read land use file name

	//strip leading blanks, trailing blanks, and final carriage return from string
	landusefile = StripString(landusefile);

	//Echo land use classification file name to file
	fprintf(echofile_fp, "\n\nLand Use Classification File: %s\n\n", landusefile);

	//Read Land Use Classification File
	ReadLandUseFile();

	//Write label for soil stack characteristics to file
	fprintf(echofile_fp, "\n\nSoil Stack Characteristics");
	fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 27
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header from file

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Write label for soil stack controls
	fprintf(echofile_fp, "\n\nMaxstackov  Minvolov  Maxvolov  Stkovopt");
	fprintf(echofile_fp,   "\n~~~~~~~~~~  ~~~~~~~~  ~~~~~~~~  ~~~~~~~~\n");

	//Record 28
	fscanf(inputfile_fp, "%s %d %s %f %s %f %s %d",	//read
		varname,					//dummy
		&maxstackov,				//maximum number of elements in the overland soil stack
		varname,					//dummy
		&minvolov,					//maximum volume multiplier for an overland stack element
		varname,					//dummy
		&maxvolov,					//minimum volume multiplier for an overland stack element
		varname,					//dummy
		&stkovopt);					//stack collapse option for the overland plane

	//Echo Soil Stack Characteristics to file
	fprintf(echofile_fp, "\n%10d  %8.3f  %8.3f  %8d\n",
		maxstackov, minvolov, maxvolov, stkovopt);

	//Check value of maxstackov...
	//
	//if maxstackov <= 2, abort...
	if(maxstackov <= 2)
	{
		//write error message to echo file
		fprintf(echofile_fp, "\n\n\nSoil Stack Specification Error:\n\n");
		fprintf(echofile_fp,     "  Minimum value for maxstackov must be > 2\n");
		fprintf(echofile_fp,     "    Value entered for maxstackov = %d \n\n",
			maxstackov);

		//Write message to screen
		printf(              "\n\n\nSoil Stack Specification Error:\n\n");
		printf(                 "  Minimum value for maxstackov must be > 2\n");
		printf(                 "    Value entered for maxstackov = %d \n\n",
			maxstackov);

		//Close the echo file
		fclose(echofile_fp);

		exit(EXIT_FAILURE);					//abort

	}	//end if maxstackov <= 2

	//Allocate memory for soil stack file name
	soilstackfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Record 29 (Part 1)
	fscanf(inputfile_fp, "%s",		//read
		varname);					//dummy

	//Record 29 (Part 2)
	fgets(soilstackfile, MAXNAMESIZE, inputfile_fp);	//read soil stack file name

	//strip leading blanks, trailing blanks, and final carriage return from string
	soilstackfile = StripString(soilstackfile);

	//Echo soil stack file name to file
	fprintf(echofile_fp, "\n\nSoil Stack File: %s\n",
		soilstackfile);

	//Read soil stack file
	ReadSoilStackFile();

	//Read soil element properties (thickness, soil type)
	//
	//Allocate memory for soil layer property file names
	soillayerthicknessfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//soil layer thickness file

	//Allocate memory for soil file name
	soiltypefile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Allocate memory for soil layer thickness
	hlayerov0 = (float ***)malloc((nrows+1) * sizeof(float **));	//initial soil layer thickness (m)

	//Allocate memory for soil types
	soiltype = (int ***)malloc((nrows+1) * sizeof(int **));		//soil type by layer

	//loop over rows
	for(i=1; i<=nrows; i++)
	{
		//Allocate additional memory for soil layer thickness
		hlayerov0[i] = (float **)malloc((ncols+1) * sizeof(float *));		//initial soil layer thickness (m)

		//Allocate additional memory for soil types
		soiltype[i] = (int **)malloc((ncols+1) * sizeof(int *));	//soil type by layer

		//loop over columns
		for(j=1; j<=ncols; j++)
		{
			//Allocate additional memory for soil layer thickness
			hlayerov0[i][j] = (float *)malloc((maxstackov+1) * sizeof(float));		//initial soil layer thickness (m)

			//Allocate remaining memory for soil types
			soiltype[i][j] = (int *)malloc((maxstackov+1) * sizeof(int));	//soil type by layer

		}	//end loop over columns

	}	//end loop over rows

	//loop over layers in reverse order (!!) (top down) (bottom is Layer 1)
	for(ilayer=maxstackov; ilayer>=1; ilayer--)
	{
		//Record 30 (Part 1)
		fscanf(inputfile_fp, "%s",		//read
			varname);					//dummy

		//Record 30 (Part 2)
		fgets(soillayerthicknessfile, MAXNAMESIZE, inputfile_fp);	//read layer thickness file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		soillayerthicknessfile = StripString(soillayerthicknessfile);

		//Echo layer thickness file name to file
		fprintf(echofile_fp, "\n\nSoil Thickness File For Layer %d: %s\n",
			ilayer, soillayerthicknessfile);

		//Read Soil Layer Thickness file
		ReadSoilLayerThicknessFile(ilayer);

		//Record 31 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 31 (Part 2)
		fgets(soiltypefile, MAXNAMESIZE, inputfile_fp);	//read soil file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		soiltypefile = StripString(soiltypefile);

		//Echo Soil Type Classification File name (for this layer) to file
		fprintf(echofile_fp, "\n\nSoil Type Classification File For Layer %d: %s\n",
			ilayer, soiltypefile);

		//Read soil type classification file (for this layer)
		ReadSoilTypeFile(ilayer);

	}	//end loop over layers

	//Write label for initial suspended solids concentrations to file
	fprintf(echofile_fp, "\n\n  Overland Plane Initial Suspended Solids Concentrations\n");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

	//Record 32
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header from file

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Allocate memory for overland initial suspended solids concentration file name
	initialssovfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Allocate memory for global overland solids concentrations (csedov)...
	//
	//Allocate first portion of memory
	csedov = (float ****)malloc((nsolids+1) * sizeof(float ***));

	//loop over solids (include zero for sum of all solids)
	for(isolid=0; isolid<=nsolids; isolid++)
	{
		//Allocation additional memory for overland solids concentration
		csedov[isolid] = (float ***)malloc((nrows+1) * sizeof(float **));

		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//Allocation additional memory for overland solids concentration
			csedov[isolid][i] = (float **)malloc((ncols+1) * sizeof(float *));

			//loop over columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] > 0)
				{
					//Allocation remaining memory for overland solids concentration
					csedov[isolid][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));

				}	//end if imask[][] > 0

			}	//end loop over columns

		}	//end loop over rows

	}	//end loop over solids

	//loop over solids
	for(isolid=1; isolid<=nsolids; isolid++)
	{
		//Record 33 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 33 (Part 2)
		fgets(initialssovfile, MAXNAMESIZE, inputfile_fp);	//read initial suspended solids file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		initialssovfile = StripString(initialssovfile);

		//Echo overland solids initial condition file name to file
		fprintf(echofile_fp,
			"\n  Solids Type: %d  Overland Solids Initial Condition File: %s\n",
			isolid, initialssovfile);

		//Read Overland Initial Suspended Solids File
		ReadInitialSolidsOverlandFile(isolid);

	}	//end loop over solids

	//if channels are simulated (chnopt > 0)
	if(chnopt > 0)
	{
		//Record 34
		fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header from file

		//Echo header to file
		fprintf(echofile_fp, "\n\n%s\n", header);

		//Write label for sediment stack characteristics to file
		fprintf(echofile_fp, "\n\nSediment Stack Characteristics");
		fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");

		fprintf(echofile_fp, "\n\nMaxstackch  Minvolch  Maxvolch  Stkchopt");
		fprintf(echofile_fp,   "\n~~~~~~~~~~  ~~~~~~~~  ~~~~~~~~  ~~~~~~~~\n");

		//Record 35
		fscanf(inputfile_fp, "%s %d %s %f %s %f %s %d",	//read
			varname,					//dummy
			&maxstackch,				//maximum number of elements in the channel sediment stack
			varname,					//dummy
			&minvolch,					//maximum volume multiplier for a channel stack element
			varname,					//dummy
			&maxvolch,					//minimum volume multiplier for a channel stack element
			varname,					//dummy
			&stkchopt);					//stack collapse option for the channel network

		//Echo Sediment Stack Characteristics to file
		fprintf(echofile_fp, "%10d  %8f  %8f  %8d\n",
			maxstackch, minvolch, maxvolch, stkchopt);

		//Check value of maxstackch...
		//
		//if maxstackch <= 2, abort...
		if(maxstackch <= 2)
		{
			//write error message to echo file
			fprintf(echofile_fp, "\n\n\nSediment Stack Specification Error:\n\n");
			fprintf(echofile_fp,     "  Minimum value for maxstackch must be > 2\n");
			fprintf(echofile_fp,     "    Value entered for maxstackch = %d \n\n",
				maxstackch);

			//Write message to screen
			printf(              "\n\n\nSediment Stack Specification Error:\n\n");
			printf(                 "  Minimum value for maxstackch must be > 2\n");
			printf(                 "    Value entered for maxstackch = %d \n\n",
				maxstackch);

			//Close the echo file
			fclose(echofile_fp);

			exit(EXIT_FAILURE);					//abort

		}	//end if maxstackch <= 2

		//Allocate memory for sediment properties file name
		sedimentpropertiesfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Allocate memory for global channel concentration array
		//
		//Allocate initial memory
		csedch = (float ****)malloc((nsolids+1) * sizeof(float ***));	//channel solids concentration (g/m3)

		//loop over number of solids (include zero for sum of all solids)
		for(isolid=0; isolid<=nsolids; isolid++)
		{
			//Allocate additional memory
			csedch[isolid] = (float ***)malloc((nlinks+1) * sizeof(float **));	//channel solids concentration (g/m3)

			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate additional memory
				csedch[isolid][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));	//channel solids concentration (g/m3)

				//loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Allocate remaining memory
					csedch[isolid][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));	//channel solids concentration (g/m3)

				}	//end loop over nodes

			}	//end loop over links

		}	//end loop over solids

		//Record 36 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 36 (Part 2)
		fgets(sedimentpropertiesfile, MAXNAMESIZE, inputfile_fp);	//read sediment properties file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		sedimentpropertiesfile = StripString(sedimentpropertiesfile);

		//Echo sediment properties file name to file
		fprintf(echofile_fp, "\n\nSediment Properties File: %s\n\n", sedimentpropertiesfile);

		//Read Sediment Properties File
		ReadSedimentPropertiesFile();

		//Allocate memory for channel initial suspended solids concentration file name
		initialsschfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 37 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 37 (Part 2)
		fgets(initialsschfile, MAXNAMESIZE, inputfile_fp);	//read initial suspended solids file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		initialsschfile = StripString(initialsschfile);

		//Echo channel solids initial condition file name to file
		fprintf(echofile_fp, "\n  Channel Suspended Solids Initial Condition File: %s\n",
			initialsschfile);

		//Read Channel Initial Suspended Solids File
		ReadInitialSolidsChannelFile();

	}	//end if chnopt = 1

	//Write label for overland point source solids loads/forcing functions
	fprintf(echofile_fp, "\n\n\n  Overland Solids Point Source Loads/Forcing Functions  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for overland point source solids loads/forcing functions (w)
	nswpov = (int *)malloc((nsolids+1) * sizeof(int));				//allocate memory
	swpovrow = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
	swpovcol = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
	nswpovpairs = (int **)malloc((nsolids+1) * sizeof(int *));		//allocate memory
	swpovopt = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
	swpov = (float ***)malloc((nsolids+1) * sizeof(float **));		//allocate memory
	swpovtime = (float ***)malloc((nsolids+1) * sizeof(float **));	//allocate memory

	//loop over number of solids
	for(i=1; i<=nsolids; i++)
	{
		//Record 38
		fscanf(inputfile_fp, "%s %d",	//read
			loadname,					//general description of point source solids loads
			&nswpov[i]);					//number of overland point source solid loads for current solids type

		//Echo number of point source loads and description to file
		fprintf(echofile_fp, "\n\n %s %d  %s %d  %s %s\n",
			"Solids Type: ", i, "Number of Loads: ", nswpov[i], "Description: ", loadname);

		//if the number of overland point source solids loads > 0
		if(nswpov[i] > 0)
		{
			//Record 39
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

			//Allocate additional memory for overland point source solids loads/forcing functions
			swpovrow[i] = (int *)malloc((nswpov[i]+1) * sizeof(int));			//allocate memory
			swpovcol[i] = (int *)malloc((nswpov[i]+1) * sizeof(int));			//allocate memory
			nswpovpairs[i] = (int *)malloc((nswpov[i]+1) * sizeof(int));		//allocate memory
			swpovopt[i] = (int *)malloc((nswpov[i]+1) * sizeof(int));			//allocate memory
			swpov[i] = (float **)malloc((nswpov[i]+1) * sizeof(float *));		//allocate memory
			swpovtime[i] = (float **)malloc((nswpov[i]+1) * sizeof(float *));	//allocate memory

			//Loop over number of overland point source loads for current solids type
			for(j=1; j<=nswpov[i]; j++)
			{
				//Record 40 (Part 1)
				fscanf(inputfile_fp, "%d %d %d %d",	//read
					&swpovrow[i][j],					//overland point source solids load row
					&swpovcol[i][j],					//overland point source solids load column
					&nswpovpairs[i][j],				//number of pairs (time breaks) in function
					&swpovopt[i][j]);				//overland point source solids load option

				//Record 40 (Part 2)
				fgets(loadname, MAXNAMESIZE, inputfile_fp);		//Read loadname

				//Echo number, row, column, and number of pairs for current load to file
				fprintf(echofile_fp, "\n\n %s %5d %s %5d %s %5d %s %5d %s %5d %s %s\n\n",
					"  Load Number: ", j,
					"  Row Number: ", swpovrow[i][j],
					"  Column Number: ", swpovcol[i][j],
					"  Number of Time Breaks: ", nswpovpairs[i][j],
					"  Load Option: ", swpovopt[i][j],
					"  Description: ", loadname);

				//Allocate remaining memory for point source solids loads/forcing functions
				swpov[i][j] = (float *)malloc((nswpovpairs[i][j]+1) * sizeof(float));		//allocate memory
				swpovtime[i][j] = (float *)malloc((nswpovpairs[i][j]+1) * sizeof(float));	//allocate memory

				//Write label for load time series to file
				//
				//if the load option equals 1
				if(swpovopt[i][j] == 1)
				{
					fprintf(echofile_fp, "  Concentration (g/m3)     ");	//load in g/m3 (concentration) when swpovopt = 1
					fprintf(echofile_fp, "  Time (days)  \n");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~   ");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");
				}
				else
				{
					fprintf(echofile_fp, "  Load (kg/day)     ");	//load in kg/day when swpovopt = 0
					fprintf(echofile_fp, "  Time (days)  \n");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~   ");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");
				}

				//Loop over number of pairs in current load for current solids type
				for(k=1; k<=nswpovpairs[i][j]; k++)
				{
					//Record 41
					fscanf(inputfile_fp, "%f %f",	//read
						&swpov[i][j][k],				//Solid Load value
						&swpovtime[i][j][k]);		//Solid load time break

					//Apply conversion and scale factors
					swpov[i][j][k] = swpov[i][j][k] * convunits * convtime * scale;

					//Echo load value and time break to file
					fprintf(echofile_fp, "%16.3f %19.3f\n",
						swpov[i][j][k], swpovtime[i][j][k]);

				}	//end loop over number of pairs

			}	//end loop over number of overland point source loads

		}	//end if nswpov > 0

	}	//end loop over number of solids types

	//Write label for overland disrtibuted solids loads/forcing functions
	fprintf(echofile_fp, "\n\n\n  Overland Distributed Solids Loads/Forcing Functions  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for overland distributed solids loads/forcing functions (wdov)
	nswdov = (int *)malloc((nsolids+1) * sizeof(int));					//allocate memory
	swdovarea = (float ****)malloc((nsolids+1) * sizeof(float ***));	//allocate memory
	nswdovcells = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
	swdovrow = (int ***)malloc((nsolids+1) * sizeof(int **));			//allocate memory
	swdovcol = (int ***)malloc((nsolids+1) * sizeof(int **));			//allocate memory
	nswdovpairs = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
	swdov = (float ***)malloc((nsolids+1) * sizeof(float **));			//allocate memory
	swdovtime = (float ***)malloc((nsolids+1) * sizeof(float **));		//allocate memory

	//Allocate memory for overland distributed load area file name
	swdovareafile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Note:  In the following loop, isolid is used the loop index rather than
	//       "i" because i (and j) is a global variable and is used in the
	//       ReadSWDovAreaFile as loop indices for row and column.  Use of
	//       isolid prevents "losing" the proper value of the index when area
	//       data are read by ReadSWDovAreaFile.
	//
	//loop over number of solids
	for(isolid=1; isolid<=nsolids; isolid++)
	{
		//Record 42
		fscanf(inputfile_fp, "%s %d",	//read
			loadname,					//general description of solids loads
			&nswdov[isolid]);				//number of overland disributed solid loads for current solids type

		//Echo number of loads and description to file
		fprintf(echofile_fp, "\n\n %s %d  %s %d  %s %s\n",
			"Solids Type: ", isolid, "Number of Loads: ", nswdov[isolid], "Description: ", loadname);

		//Allocate additional memory for overland distributed solids loads/forcing functions
		swdovarea[isolid] = (float ***)malloc((nswdov[isolid]+1) * sizeof(float **));	//allocate memory
		nswdovcells[isolid] = (int *)malloc((nswdov[isolid]+1) * sizeof(int));		//allocate memory
		swdovrow[isolid] = (int **)malloc((nswdov[isolid]+1) * sizeof(int *));		//allocate memory
		swdovcol[isolid] = (int **)malloc((nswdov[isolid]+1) * sizeof(int *));		//allocate memory
		nswdovpairs[isolid] = (int *)malloc((nswdov[isolid]+1) * sizeof(int));		//allocate memory
		swdov[isolid] = (float **)malloc((nswdov[isolid]+1) * sizeof(float *));		//allocate memory
		swdovtime[isolid] = (float **)malloc((nswdov[isolid]+1) * sizeof(float *));	//allocate memory

		//if the number of overland solids loads > 0
		if(nswdov[isolid] > 0)
		{
			//Note:  In the following loop, iload is used as the loop index rather than
			//       "j" because i (and j) are global variables and are used in the
			//       ReadSWDovAreaFile as loop indices for row and column.  Use of
			//       iload prevents "losing" the proper value of the index when area
			//       data are read by ReadSWDovAreaFile.
			//
			//loop over number of overland distributed source loads for current solids type
			for(iload=1; iload<=nswdov[isolid]; iload++)
			{
				//Record 43 (Part 1)
				fscanf(inputfile_fp, "%s",	//read
					varname);				//dummy

				//Record 43 (Part 2)
				fgets(swdovareafile, MAXNAMESIZE, inputfile_fp);	//read overland distributed load area file name

				//strip leading blanks, trailing blanks, and final carriage return from string
				swdovareafile = StripString(swdovareafile);

				//Echo channel solids initial condition file name to file
				fprintf(echofile_fp, "\n  Overland Distributed Load Area File: %s\n",
					swdovareafile);

				//Read overland distributed solids load area file
				ReadSWDovAreaFile(isolid, iload);

				//Record 44
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

				//Record 45 (Part 1)
				fscanf(inputfile_fp, "%d",	//read
					&nswdovpairs[isolid][iload]);	//number of pairs (time breaks) in function

				//Record 45 (Part 2)
				fgets(loadname, MAXNAMESIZE, inputfile_fp);		//Read loadname

				//Echo number, and number of pairs for current load to file
				fprintf(echofile_fp, "\n\n %s %5d %s %5d %s %s\n\n",
					"  Load Number: ", iload,
					"  Number of Time Breaks: ", nswdovpairs[isolid][iload],
					"  Description: ", loadname);

				//Allocate remaining memory for solids loads/forcing functions
				swdov[isolid][iload] = (float *)malloc((nswdovpairs[isolid][iload]+1) * sizeof(float));		//allocate memory
				swdovtime[isolid][iload] = (float *)malloc((nswdovpairs[isolid][iload]+1) * sizeof(float));	//allocate memory

				//Write label for load time series to file
				fprintf(echofile_fp, "  Load (g/m2/mm rain)     ");	//load in g/m2/mm rain
				fprintf(echofile_fp, "  Time (hours)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~~~~~~   ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

				//Loop over number of pairs in current load for current solids type
				for(k=1; k<=nswdovpairs[isolid][iload]; k++)
				{
					//Record 46
					fscanf(inputfile_fp, "%f %f",	//read
						&swdov[isolid][iload][k],			//Solid Load value
						&swdovtime[isolid][iload][k]);		//Solid load time break

					//Apply conversion and scale factors
					swdov[isolid][iload][k] = swdov[isolid][iload][k] * convunits * convtime * scale;

					//Echo load value and time break to file
					fprintf(echofile_fp, "%16.3f %20.3f\n",
						swdov[isolid][iload][k], swdovtime[isolid][iload][k]);

				}	//end loop over number of pairs

			}	//end loop over number of overland distributed loads

		}	//end if nswdov > 0

	}	//end loop over number of solids types

	//if channels are simulated)
	if(chnopt > 0)
	{
		//Write label for channel solids loads/forcing functions
		fprintf(echofile_fp, "\n\n\n  Solids Loads/Forcing Functions (Channels)  \n");
		fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

		//Allocate memory for channel solids loads/forcing functions (w)
		nswch = (int *)malloc((nsolids+1) * sizeof(int));				//allocate memory
		swchlink = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
		swchnode = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
		nswchpairs = (int **)malloc((nsolids+1) * sizeof(int *));		//allocate memory
		swchopt = (int **)malloc((nsolids+1) * sizeof(int *));			//allocate memory
		swch = (float ***)malloc((nsolids+1) * sizeof(float **));		//allocate memory
		swchtime = (float ***)malloc((nsolids+1) * sizeof(float **));	//allocate memory

		//loop over number of solids
		for(i=1; i<=nsolids; i++)
		{
			//Record 47
			fscanf(inputfile_fp, "%s %d",	//read
				loadname,					//general description of solids loads
				&nswch[i]);					//number of channel solid loads for current solids type

			//Echo number of loads and description to file
			fprintf(echofile_fp, "\n\n %s %d  %s %d  %s %s\n",
				"Solids Type:", i, "Number of Loads:", nswch[i], "Description:", loadname);

			//if the number of channel solids loads > 0
			if(nswch[i] > 0)
			{
				//Record 48
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

				//Allocate additional memory for solids loads/forcing functions
				swchlink[i] = (int *)malloc((nswch[i]+1) * sizeof(int));		//allocate memory
				swchnode[i] = (int *)malloc((nswch[i]+1) * sizeof(int));		//allocate memory
				nswchpairs[i] = (int *)malloc((nswch[i]+1) * sizeof(int));		//allocate memory
				swchopt[i] = (int *)malloc((nswch[i]+1) * sizeof(int));			//allocate memory
				swch[i] = (float **)malloc((nswch[i]+1) * sizeof(float *));		//allocate memory
				swchtime[i] = (float **)malloc((nswch[i]+1) * sizeof(float *));	//allocate memory

				//Loop over number of channel loads for current solids type
				for(j=1; j <= nswch[i]; j++)
				{
					//Record 49 (Part 1)
					fscanf(inputfile_fp, "%d %d %d %d",	//read
						&swchlink[i][j],				//Solid Load Link
						&swchnode[i][j],				//Solid Load Node
						&nswchpairs[i][j],				//Number of pairs (time breaks) in function
						&swchopt[i][j]);				//Solids Load option

					//Record 49 (Part 2)
					fgets(loadname, MAXNAMESIZE, inputfile_fp);		//Read loadname

					//Echo number, link, node, and number of pairs for current load to file
					fprintf(echofile_fp, "\n\n %s %5d %s %5d %s %5d %s %5d %s %5d %s %s\n\n",
						"  Load Number: ", j,
						"  Link Number: ", swchlink[i][j],
						"  Node Number: ", swchnode[i][j],
						"  Number of Time Breaks: ", nswchpairs[i][j],
						"  Load Option: ", swchopt[i][j],
						"  Description: ", loadname);

					//Allocate remaining memory for solids loads/forcing functions
					swch[i][j] = (float *)malloc((nswchpairs[i][j]+1) * sizeof(float));		//allocate memory
					swchtime[i][j] = (float *)malloc((nswchpairs[i][j]+1) * sizeof(float));	//allocate memory

					//Write label for load time series to file
					fprintf(echofile_fp, "  Load (kg/day)     ");	//need to adopt consistent units
					fprintf(echofile_fp, "  Time (days)  \n");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~   ");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

					//Loop over number of pairs in current load for current solids type
					for(k=1; k<=nswchpairs[i][j]; k++)
					{
						//Record 50
						fscanf(inputfile_fp, "%f %f",	//read
							&swch[i][j][k],				//Solid Load value
							&swchtime[i][j][k]);		//Solid load time break

						//Apply conversion and scale factors
						swch[i][j][k] = swch[i][j][k] * convunits * convtime * scale;

						//Echo load value and time break to file
						fprintf(echofile_fp, "%16.3f %19.3f\n",
							swch[i][j][k], swchtime[i][j][k]);

					}	//end loop over numner of pairs

				}	//end loop over number of channel loads

			}	//end if nswch > 0

		}	//end loop over number of solids types

	}	//end if(chnopt == 1)

	//Housekeeping: skip extra carriage return
	fgets(header, MAXHEADERSIZE, inputfile_fp);

	//Write label for solids boundary condition (bc) functions
	fprintf(echofile_fp, "\n\n\n  Solids Boundary Condition Functions (Outlets)  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for solids boundary conditions
	nsbcpairs = (int **)malloc((noutlets+1) * sizeof(int *));		//allocate memory
	sbc = (float ***)malloc((noutlets+1) * sizeof(float **));		//allocate memory
	sbctime = (float ***)malloc((noutlets+1) * sizeof(float **));	//allocate memory

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//Record 51
		fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header from file

		//Echo header to file
		fprintf(echofile_fp, "\n\n%s\n", header);

		//if the domain boundary condition option for this outlet > 0
		if(dbcopt[i] > 0)
		{
			//Record 52
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

			//Allocate additional memory for solids boundary conditions
			nsbcpairs[i] = (int *)malloc((nsolids+1) * sizeof(int));		//allocate memory
			sbc[i] = (float **)malloc((nsolids+1) * sizeof(float *));		//allocate memory
			sbctime[i] = (float **)malloc((nsolids+1) * sizeof(float *));	//allocate memory

			//Loop over number of solids
			for(j=1; j<=nsolids; j++)
			{
				//Record 53 (Part 1)
				fscanf(inputfile_fp, "%d",
					&nsbcpairs[i][j]);	//Number of pairs for this solids boundary conditions

				//Record 53 (Part 2)
				fgets(bcname, MAXNAMESIZE, inputfile_fp);	//Reads boundary condition name from file

				//Echo outlet, row, column, solids type, and number of pairs for current bc to file
				fprintf(echofile_fp, "\n\n%s %5d %s %5d %s %5d %s %5d %s %5d %s %s\n\n",
					"  Outlet: ", i,
					"  Row Number: ", iout[i],
					"  Column Number: ", jout[i],
					"  Solids Type: ", j,
					"  Number of Time Breaks: ", nsbcpairs[i][j],
					"  Description: ", bcname);

				//Allocate remaining memory for solids boundary conditions
				sbc[i][j] = (float *)malloc((nsbcpairs[i][j]+1) * sizeof(float));		//allocate memory
				sbctime[i][j] = (float *)malloc((nsbcpairs[i][j]+1) * sizeof(float));	//allocate memory

				//Write label for boundary condition time series to file
				fprintf(echofile_fp, "  BC (g/m3)     ");	//need to adopt consistent units
				fprintf(echofile_fp, "  Time (days)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~   ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

				//loop over number of pairs (time breaks)
				for(k=1; k<=nsbcpairs[i][j]; k++)
				{
					//Record 54
					fscanf(inputfile_fp, "%f %f",	//read
						&sbc[i][j][k],				//Boundary Condition value
						&sbctime[i][j][k]);			//Boundary Condition time break

					//Apply conversion and scale factors
					sbc[i][j][k] = sbc[i][j][k] * convunits * convtime * scale;

					//Echo boundary condition value and time break to file
					fprintf(echofile_fp, "%13.3f %18.3f\n",
						sbc[i][j][k], sbctime[i][j][k]);

				}	//end loop over number of pairs

			}	//end loop over number of solids

		}	//end if dbcopt[i] > 0

	}	//end loop over number of outlets

	//Record 55
	fscanf(inputfile_fp, "%s %d",	//read
		varname,					//dummy
		&nsedreports);				//Number of Sediment Reporting Stations

	//Echo Number of Sediment Reporting Stations to file
	fprintf(echofile_fp, "\n\n\nNumber of Sediment Reporting Stations = %d\n", nsedreports);

	//Allocate memory for sediment reporting stations
	sedreprow = (int *)malloc((nsedreports+1) * sizeof(int));		//reporting station cell row
	sedrepcol = (int *)malloc((nsedreports+1) * sizeof(int));		//reporting station cell column
	sedarea = (float *)malloc((nsedreports+1) * sizeof(float));		//reporting station cell (upstream) drainage area
	sedunitsopt = (int *)malloc((nsedreports+1) * sizeof(int));		//flow report units option

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

	//Loop over number of sediment reporting stations
	for(i=1; i<=nsedreports; i++)
	{
		//Record 56
		fscanf(inputfile_fp, "%d %d %f %d %s",	//read
			&sedreprow[i],					//reporting station cell row
			&sedrepcol[i],					//reporting station cell column
			&sedarea[i],					//reporting station cell (upstream) drainage area
			&sedunitsopt[i],				//sediment report units option
			varname);						//station identifier

		//Echo Station (loop counter), Row, Column, Area, and Units Option to file
		fprintf(echofile_fp, "%11d   %12d   %15d   %23.3f   %23d %s\n",
			i, sedreprow[i], sedrepcol[i], sedarea[i], sedunitsopt[i], varname);

	}	//end loop over number of sediment reporting stations

	//Close the Echo File
	fclose(echofile_fp);

//End of function: Return to ReadInputFile
}
