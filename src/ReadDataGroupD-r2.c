/*---------------------------------------------------------------------
C-  Function:   ReadDataGroupD
C-
C-  Purpose/    ReadDataGroupD is called at the start of the simulation
C-  Methods:    to read Data Group D (chemical transport simulation
C-              parameters) from the model input file.
C-
C-
C-  Inputs:     None
C
C-  Outputs:    None
C-
C-  Controls:   chnopt
C-
C-  Calls:      ReadSoilLayerChemicalFile,
C-              ReadInitialChemicalOverlandFile,
C-              ReadSedimentChemicalFile,
C-              ReadInitialChemicalChannelFile,
C-              ChemicalProperyInit
C-
C-  Called by:  ReadInputFile
C-
C-  Created:    Mark Velleux, CSU
C-              Aditya Maroo, CSU
C-              John England, USBR
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

//trex global variable declarations for chemical transport
#include "trex_chemical_declarations.h"

void ReadDataGroupD()
{
	//local variable declarations/definitions
	int
		ichem,		//loop counter for chemical number
		ilayer,		//layer in the soil/sediment stack
		nfields,	//number of fields (data groupings) of chemical reaction constants
		nconstants,	//number of constants in a field identifying chemical reactions
		cid,		//number to identify a chemical reaction constant
		iload;		//load time function identifier

	float
		convunits,	//conversion factors for length, area, volume or mass units
		convtime,	//conversion factor for time units
		scale,		//scale factor
		cvalue;		//value of a chemical constant

	float
		lkp,		//temporary variable to store log kp partitioning values
		lkb,		//temporary variable to store log kb binding values
		lkoc;		//temporary variable to store log koc partitioning values

	char
		fieldname[MAXNAMESIZE],		//name of field for chemical properties
		cname[MAXNAMESIZE];			//name of chemical property/constant

	//open the echo file in append mode (append to existing file)
	echofile_fp = fopen(echofile, "a");

	//write message to screen
	printf("\n\n***************************\n");
	printf(	   "*                         *\n");
	printf(	   "*   Reading Data Group D  *\n");
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
	fprintf(echofile_fp, "\n\n\n%s\n\n", header);

	//Record 2
	fscanf(inputfile_fp, "%s %d %s %d",	//read
		varname,						//dummy
		&nchems,						//Number of Chemicals
		varname,						//dummy
		&ncgroups);						//Number of Chemical Reporting Groups

	//Echo number of chemicals to file
	fprintf(echofile_fp, "\nNumber of Chemical Types = %d\n", nchems);

	//if the number of chemicals types < 1, abort...
	if(nchems < 1)
	{
		//Write error message to file
		fprintf(echofile_fp, "\n\n\nData Group Error:\n");
		fprintf(echofile_fp,       "  Number of chemicals must be >= 1\n");
		fprintf(echofile_fp,       "  User selected nchems = %d\n", nchems);
		fprintf(echofile_fp,       "  Select nchems >= 1");

		//Write error message to screen
		printf(              "\n\n\nData Group Error:\n");
		printf(                   "  Number of chemcals must be >= 1\n");
		printf(                   "  User selected nchems = %d\n", nchems);
		printf(                   "  Select nchems >= 1");

		exit(EXIT_FAILURE);					//abort

	}	//end if nchems < 1

	//Echo number of chemical reporting groups to file
	fprintf(echofile_fp, "\nNumber of Chemical Reporting Groups = %d\n", ncgroups);

	//Housekeeping: skip extra carriage return
	fgets(header, MAXHEADERSIZE, inputfile_fp);

	//Record 3
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Write label for chemical reporting group names to file
	fprintf(echofile_fp, "\n\n  Chemical Reporting Groups  ");
	fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
	fprintf(echofile_fp,   "\n  Group Number    ");
	fprintf(echofile_fp,     "          Group Name          ");
	fprintf(echofile_fp,   "\n~~~~~~~~~~~~~~~~  ");
	fprintf(echofile_fp,     "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for chemical reporting group names
	cgroupname = (char **)malloc((ncgroups+1) * sizeof(char *));	//allocate memory

	//Loop over number of chemical reporting groups
	for(i=1; i<=ncgroups; i++)
	{
		//Allocate remaining memory for chemical reporting group names
		cgroupname[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 4 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 4 (Part 2)
		fgets(cgroupname[i], MAXNAMESIZE, inputfile_fp);	//Read name of chemical group

		//strip leading blanks, trailing blanks, and final carriage return from string
		cgroupname[i] = StripString(cgroupname[i]);

		//Echo chemical reporting group names to file
		fprintf(echofile_fp, "%16d  %s\n",
			i, cgroupname[i]);

	}	//end loop over ncgroups

	//Record 5
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Allocate memory for chemical identification arrays
	cgroupnumber = (int *)malloc((nchems+1) * sizeof(int));		//chemical reporting group number
	chemname = (char **)malloc((nchems+1) * sizeof(char *));	//name of chemical

	//Allocate memory and set default values for chemical properties
	ChemicalPropertyInit();

	//Loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//Allocate remaining memory for chemical name array
		chemname[i] = (char *)malloc(MAXNAMESIZE * sizeof(char));

		//Record 6 (Part 1)
		fscanf(inputfile_fp, "%s %d %s %d %s %d",	//read
			varname,								//dummy
			&ichem,									//chemical number
			varname,								//dummy
			&nfields,								//number of property fields
			varname,								//dummy
			&cgroupnumber[i]);						//chemical reporting group number

		//Record 6 (Part 2)
		fgets(chemname[i], MAXNAMESIZE, inputfile_fp);	//read chemical name/description

		//strip leading blanks, trailing blanks, and final carriage return from string
		chemname[i] = StripString(chemname[i]);

		//Echo chemical number and name to file
		fprintf(echofile_fp, "\n\nChemical Number: %d", i);
		fprintf(echofile_fp,   "\nDescription: %s\n", chemname[i]);
		fprintf(echofile_fp,   "\nNumber of Fields (Data Groupings): %d\n", nfields);
		fprintf(echofile_fp,   "\nReporting Group: %d\n", cgroupnumber[i]);

		//Check that chemical input is chemical expected...
		//
		//if ichem does not equal i
		if(ichem != i)
		{
			//Write error message to file
			fprintf(echofile_fp, "\n\n\nChemical specification Error:\n");
			fprintf(echofile_fp,       "  Chemical specified = %d", ichem);
			fprintf(echofile_fp,       "  Chemical expected = %d", i);

			//Write error message to screen
			printf("\n\n\nChemical specification Error:\n");
			printf(      "  Chemical specified = %d", ichem);
			printf(      "  Chemical expected = %d", i);

			exit(EXIT_FAILURE);					//abort

		}	//end check of chemical number (ichem != i)

		//Loop over number of fields for current chemical
		for(j=1; j<=nfields; j++)
		{
			//Record 7 (Part 1)
			fscanf(inputfile_fp, "%d",	//read
				&nconstants);					//number of constants for current field

			//Record 7 (Part 2)
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
				//Record 8
				fscanf(inputfile_fp, "%s %d %f",	//read
					cname,							//name of constant
					&cid,							//constant identifier
					&cvalue);						//value of contant

				//Echo constant information to file
				fprintf(echofile_fp, "%11d  %17.4f  %s\n", cid, cvalue, cname);

				//Assign constants...
				//
				//Process options: if option = 0, no computations for the process
				if(cid == 100)
				{
					partopt[i] = (int)(cvalue);		//partitioning option

					//set flag for process control (if any chemical partitions)
					partopt[0] = partopt[0] + partopt[i];
				}
				else if(cid == 200)
				{
					bioopt[i] = (int)(cvalue);		//biodegredation option

					//set flag for process control (if any chemical biodegrades)
					bioopt[0] = bioopt[0] + bioopt[i];
				}
				else if(cid == 300)
				{
					dslopt[i] = (int)(cvalue);		//dissolution option

					//set flag for process control (if any chemical dissolves)
					dslopt[0] = dslopt[0] + dslopt[i];
				}
				else if(cid == 400)
				{
					hydopt[i] = (int)(cvalue);		//hydrolysis option

					//set flag for process control (if any chemical hydrolyzes)
					hydopt[0] = hydopt[0] + hydopt[i];
				}
				else if(cid == 500)
				{
					oxiopt[i] = (int)(cvalue);		//oxidation option

					//set flag for process control (if any chemical oxidizes)
					oxiopt[0] = oxiopt[0] + oxiopt[i];
				}
				else if(cid == 600)
				{
					phtopt[i] = (int)(cvalue);		//photolysis option

					//set flag for process control (if any chemical photolyzes)
					phtopt[0] = phtopt[0] + phtopt[i];
				}
				else if(cid == 700)
				{
					radopt[i] = (int)(cvalue);		//radioactive decay option

					//set flag for process control (if any chemical decays)
					radopt[0] = radopt[0] + radopt[i];
				}
				else if(cid == 800)
				{
					vltopt[i] = (int)(cvalue);		//volatilization option

					//set flag for process control (if any chemical volatilizes)
					vltopt[0] = vltopt[0] + vltopt[i];
				}
				else if(cid == 900)
				{
					udropt[i] = (int)(cvalue);		//user-defined reaction option

					//set flag for process control (if any chemical has an alternate reaction)
					udropt[0] = udropt[0] + udropt[i];
				}
				//Partitioning parameters
				else if(cid == 110)
				{
					lkp = cvalue;	//log partition or distribution coefficient

					//partition coefficient (L/kg)
					kp[i] = (float)(pow(10.0,lkp));

					//convert partition coefficient units from (L/kg) to (m3/g)
					kp[i] = (float)(kp[i] * 1.0e-6);
				}
				else if(cid == 120)
				{
					lkb = cvalue;	//log binding coefficient

					//binding coefficient (L/kg)
					kb[i] = (float)(pow(10.0,lkb));

					//convert binding coefficient units from (L/kg) to (m3/g)
					kb[i] = (float)(kb[i] * 1.0e-6);
				}
				else if(cid == 130)
				{
					lkoc = cvalue;	//log organic carbon (OC) partition coefficient

					//organic carbon partition coefficient (L/kg)
					koc[i] = (float)(pow(10.0,lkoc));

					//convert OC partition coefficient units from (L/kg) to (m3/g)
					koc[i] = (float)(koc[i] * 1.0e-6);
				}
				else if(cid == 140)
				{
					nux[i] = cvalue;	//particle interaction parameter
				}
				//Biodegradation parameters
				else if(cid == 210)
				{
					kbiowov[i] = cvalue;	//first-order biodegradation rate for overland water (input as 1/day)

					//convert biodegradation rate units from (1/day) to (1/sec)
					kbiowov[i] = (float)(kbiowov[i] / 86400.0);
				}
				else if(cid == 215)
				{
					kbiowov[i] = cvalue;	//second-order biodegradation rate for overland water (mL/cells/day)

					//convert biodegradation rate units from (mL/cells/day) to (m3/cells/sec)
					kbiowov[i] = (float)(kbiowov[i] * 10e6 / 86400.0);
				}
				else if(cid == 220)
				{
					kbiosov[i] = cvalue;	//first-order biodegradation rate for overland soil (input as 1/day)

					//convert biodegradation rate units from (1/day) to (1/sec)
					kbiosov[i] = (float)(kbiosov[i] / 86400.0);
				}
				else if(cid == 225)
				{
					kbiosov[i] = cvalue;	//second-order biodegradation rate for overland soil (mL/cells/day)

					//convert biodegradation rate units from (mL/cells/day) to (m3/cells/sec)
					kbiosov[i] = (float)(kbiosov[i] * 10e6 / 86400.0);
				}
				else if(cid == 230)
				{
					kbiowch[i] = cvalue;	//first-order biodegradation rate for channel water (input as 1/day)

					//convert biodegradation rate units from (1/day) to (1/sec)
					kbiowch[i] = (float)(kbiowch[i] / 86400.0);
				}
				else if(cid == 235)
				{
					kbiowch[i] = cvalue;	//second-order biodegradation rate for channel water (mL/cells/day)

					//convert biodegradation rate units from (mL/cells/day) to (m3/cells/sec)
					kbiowch[i] = (float)(kbiowch[i] * 10e6 / 86400.0);
				}
				else if(cid == 240)
				{
					kbiosch[i] = cvalue;	//first-order biodegradation rate for channel sediment (input as 1/day)

					//convert biodegradation rate units from (1/day) to (1/sec)
					kbiosch[i] = (float)(kbiosch[i] / 86400.0);
				}
				else if(cid == 245)
				{
					kbiosch[i] = cvalue;	//second-order biodegradation rate for channel sediment (mL/cells/day)

					//convert biodegradation rate units from (mL/cells/day) to (m3/cells/sec)
					kbiosch[i] = (float)(kbiosch[i] * 10e6 / 86400.0);
				}
				//Dissolution parameters

				//Hydrolysis parameters

				//Oxidation parameters

				//Photolysis parameters

				//Radioactive decay parameters

				//Volatilization parameters

				//Extra (user-defined) reaction parameters

				//else the constant is undefined
				else
				{
					//Write error message to file
					fprintf(echofile_fp, "\n\n\nData Group Error:\n");
					fprintf(echofile_fp,       "  Undefined chemical constant\n");
					fprintf(echofile_fp,       "  User selected cid = %d\n", cid);
					fprintf(echofile_fp,       "  No constant defined for this identifier");

					//Write error message to screen
					printf(              "\n\n\nData Group Error:\n");
					printf(                    "  Undefined chemical constant\n");
					printf(                    "  User selected cid = %d\n", cid);
					printf(                    "  No constant defined for this identifier");

					exit(EXIT_FAILURE);					//abort

				}	//end if cid == a value...

/*
				//constants for simple reactions.
				else if(cid == 140)
					kv[i] = cvalue;		//volatilization
				else if(cid == 141)
					kbw[i] = cvalue;	//water column biodegredation
				else if(cid == 142)
					kbs[i] = cvalue;	//benthic biodegredation
				else if(cid == 181)
					khoh[i] = cvalue;	//alkaline hydrolysis
				else if(cid == 182)
					khn[i] = cvalue;	//neutral hydrolysis
				else if(cid == 183)
					khh[i] = cvalue;	//acid hydrolysis
				else if(cid == 256)
					ko[i] = cvalue;		//oxidation
				else if(cid == 287)
					kf[i] = cvalue;		//photolysis
				else if(cid == 571)
					ke[i] = cvalue;		//user-defined reaction
				else if(cid == 143)
					thbw[i] = cvalue;	//water column biodegredation
				else if(cid == 144)
					thbs[i] = cvalue;	//benthic biodegredation
				else if(cid == 252)
					thhoh[i] = cvalue;	//alkaline hydrolysis
				else if(cid == 253)
					thhn[i] = cvalue;	//neutral hydrolysis
				else if(cid == 254)
					thh[i] = cvalue;	//acid hydrolysis
				else if(cid == 257)
					tho[i] = cvalue;	//oxidation
				else if(cid == 289)
					thf[i] = cvalue;	//photolysis
				else if(cid == 572)
					the[i] = cvalue;	//user-defined reaction

				//general chemical constants.
				else if(cid == 9)
					tdint[i] = cvalue;	//time interval for rxn. rates
				else if(cid == 81)
					molwt[i] = cvalue;	//molecular weight
				else if(cid == 82)
					solg[i] = cvalue;	//solubility
				else if(cid == 83)
					vaprg[i] = cvalue;	//vapor pressure
				else if(cid == 84)
					lkow[i] = cvalue;	//log octanol-water partition cf.

				//ionization constants.
				else if(cid >= 85 && cid <= 88)
					spflg[i] = int(cvalue);	//flag indicating presence
											//of an ionic species of
											//a chemical

				else if(cid >= 91 && cid <= 94)
					pka[i] = cvalue;	//constant in arrhenius eqn.
				else if(cid >= 95 && cid <= 98)
					epka[i] = cvalue;	//activation energy of dissociation

				//Volatilization constants.
				else if(cid == 136)
					xvl[i] = int(cvalue);	//volitalization option
				else if(cid == 137)
					hlc25[i] = cvalue;	//henry's law constant @25 c
				else if(cid == 138)
					klt[i] = cvalue;		//volat. correction factor
				else if(cid == 139)
					kvog[i] = cvalue;		//volat./reaeration rates
				else if(cid == 600)                           
					hlca0[i] = cvalue;		//henry's law temp factor a0
				else if(cid == 601)                           
					hlca1[i] = cvalue;		//henry's law temp factor a1
				else if(cid == 1)                           
					xvh[i] = int(cvalue);	//henry's law constant option
				else if(cid == 2)
					xvg[i] = int(cvalue);	 //volat. option 4 gas transfer
				else if(cid == 3)                            
					vb[i] = cvalue;			//lebas molar volume.
				else if(cid == 4)                            
					t_melt[i] = cvalue;		//melting temperature (c)
				else if(cid == 5)
					airtmp = cvalue;		//multiplier for air temp (c)
				else if(cid == 8)
					atmos[i] = cvalue;	//atmospheric concentration 

				else if((cid >= 146) && (cid <= 160))
				{
					//Get the second-order 20-degree C biodegredation constants for 
					//(1) dissolved chemicals, (2) DOC-bound chemical, and (3) particulate 
					//chemicals.  These are at IC=146,151,156.
					itemp = cid - 145;
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						if((itemp / (isolid * 5)) <= 1)
						{
							kbio20(ichem,isolid) = cvalue;  
						}
					}
				}

				//Temperature correction for dissolved chemical biodegradation.
				else if((cid >= 161) && (cid <= 165))
					q10dis[i] = cvalue;

				//Temperature correction for DOC-bound chemical biodegradation.
				else if((cid >= 166) && (cid <= 170))
					q10doc[i] = cvalue;

				//Temperature correction for particulate chemical biodegradation.
				else if((cid == 171) && (cid <= 175))
					q10par[i] = cvalue;

				//Reference temperature for hydrolysis rates (c).
				else if(cid == 184)
					trefh[i] = cvalue;

				//Get the second-order 20-degree C alkaline hydrolysis rate constant
				//for (1) dissolved chemical, (2) DOC-bound chemical, and (3) particulate
				//chemical.  These are for IC=186, 191, and 196.
				else if((cid >= 186) && (cid <= 200))
				{
					itemp = cid - 185;
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						if((itemp / (isolid * 5)) <= 1)
						{
							kh20(ichem,1,isolid) = cvalue;
						}

					}
				}

				//Activation energy for alkaline hydrolysis.  (?? Range of indices ??)
				else if((cid >= 231) && (cid <= 235))
					ehoh[i] = cvalue;

				//Get the neutral hydrolysis rate constants at 20-degrees C for 
				//(1) dissolved chemical, (2) DOC-bound chemical, and (3) particulate
				//chemical.  These are at IC=201, 206, and 211.
				else if((cid >= 201) && (cid <= 215))
				{
					itemp = cid - 200;
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						if((itemp / (isolid * 5)) <= 1)
						{
							kh20(ichem,2,isolid) = cvalue;
						}
					}
				}
				//Activation energy for neutral hydrolysis.  (?? Range of indices ??)
				else if((cid >= 236) && (cid <= 240))
					ehn[i] = cvalue;

				//Get the second-order 20-degrees C acid hydrolysis rate constants for
				//(1) dissolved chemical, (2) DOC-bound chemical, and (3) particulate
				//chemical.  These are at IC=216, 221, and 226.  
				else if((cid >= 216) && (cid <= 230))
				{
					itemp = cid - 215;
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						if((itemp / (isolid * 5)) <= 1)
						{
							kh20(ichem,3,isolid) = cvalue;
						}
					}
				}
				//Activation energy for acid hydrolysis.
				else if((cid >= 241) && (cid <= 245))
					ehh[i] = cvalue;

				//Reference temperature for oxidation rates (c).
				else if(cid == 258)
					trefo[i] = cvalue;

				//Get the second-order 20-degrees C oxidation rate constant for
				//(1) dissolved chemical, (2) DOC-bound chemical, and (3) particulate
				//chemical.  These are at IC=261, 266, and 271.
				else if((cid >= 261) && (cid <= 275))
				{
					itemp = cid - 260;
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						if((itemp / (isolid * 5)) <= 1)
						{
							kox20(ichem,isolid) = cvalue;
						}
					}
				}
				//Activation energy for oxidation.
				else if((cid >= 276) && (cid <= 280))
					eox[i] = cvalue;

				//Toxi4 photolysis constants.
				else if(cid == 286)
					xphoto[i] = int(cvalue);

				//Depth integrated photolysis rates.
				else if((cid >= 291) && (cid <= 295))
					kdpg[i] = cvalue;

				//Get the quantum fraction yields for (1) dissolved chemical, (2) DOC-bound
				//chemical, and (3) particulate chemical. These are at IC=551,556,561.
				else if((cid >= 551) && (cid <= 565))
				{
					itemp = cid - 550;
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						if((itemp / (isolid * 5)) <= 1)
						{
							quantg(ichem,isolid) = cvalue;
						}
					}
				}

				//Reference temperature for the user-defined reaction (c).
				else if(cid == 573)
					trefe[i] = cvalue;

				//Get the second-order user-defined reaction rate constants for
				//(1) dissolved chemical, (2) DOC-bound chemical, and (3) particulate
				//chemical.  These are at IC=576, 581, and 586.
				else if((cid >= 576) && (cid <= 590))
				{
					itemp = cid - 575;
					for(isolid=1; isolid<=nsolids; isolid++)
					{
						if((itemp / (isolid * 5)) <= 1)
						{
							ke20(ichem,isolid) = cvalue;
						}
					}
				}
				//Activation energy for the user-defined reaction.
				else if((cid >= 591) && (cid <= 595))
					eex[i] = cvalue;

				else	//else cid is undefined
				{
					//Write Error message to file
					fprintf(echofile_fp, "Error! Undefined constant:");
					fprintf(echofile_fp, "  cid = %3d  Name: %s",
						cid,
						cname[i]);

					//Write Error message to screen
					printf("Error! Undefined constant:");
					printf("  cid = %3d  Name: %s",
						cid,
						cname[i]);

					//Abort
					exit(EXIT_FAILURE);

				} //end if cid = a value
*/

			}	//end loop over constants

			//Start a new line for the next row of data in the echo file 
			fprintf(echofile_fp, "\n");

		}	//end loop over fields

	}	//end loop over chemicals

	//Record 9
	fscanf(inputfile_fp, "%s %d",	//read
		varname,					//dummy
		&ncyields);					//number of chemical reaction yields

	//Echo number of yeilds to file
	fprintf(echofile_fp, "\nNumber of Yields: %d\n", ncyields);

	//if the number of chemical yields is greater than zero
	if(ncyields > 0)
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

		//Allocate memory for chemical reaction yield parameters
		cyldfrom = (int *)malloc((ncyields+1) * sizeof(int));		//reacting chemical
		cyldto = (int *)malloc((ncyields+1) * sizeof(int));		//product chemical
		cyldprocess = (int *)malloc((ncyields+1) * sizeof(int));	//chemical reaction process pathway number
		cyield = (float *)malloc((ncyields+1) * sizeof(float));	//chemical reaction yield

		//Loop over number of chemical yields
		for(i=1; i<=ncyields; i++)
		{
			//Record 10
			fscanf(inputfile_fp, "%s %d %s %d %s %d %s %f",	//read
				varname,									//dummy
				&cyldfrom[i],								//reacting chemical
				varname,									//dummy
				&cyldto[i],									//product chemical
				varname,									//dummy
				&cyldprocess[i],							//reaction process pathway number
				varname,									//dummy
				&cyield[i]);								//reaction yield (g/g)

			//Echo yield parameters to file
			fprintf(echofile_fp, "%6d  %4d  %9d  %22.4f\n",
				cyldfrom[i], cyldto[i], cyldprocess[i], cyield[i]);

		}	//end loop over number of chemical yields

	}	//end if ncyields > 0

	//Housekeeping: skip extra carriage return
	fgets(header, MAXHEADERSIZE, inputfile_fp);

	//Record 11
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Allocate memory for soil chemical initial condition file name
	soilchemicfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Allocate memory for global overland chemical concentration
	cchemov = (float ****)malloc((nchems+1) * sizeof(float ***));

	//loop over chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Allocate additional memory for overland chemical concentration
		cchemov[ichem] = (float ***)malloc((nrows+1) * sizeof(float **));

		//loop over rows
		for(i=1; i<=nrows; i++)
		{
			//Allocate additional memory for overland chemical concentration
			cchemov[ichem][i] = (float **)malloc((ncols+1) * sizeof(float *));

			//loop over columns
			for(j=1; j<=ncols; j++)
			{
				//if the cell is in the domain
				if(imask[i][j] > 0)
				{
					//Allocate remaining memory for overland chemical concentration
					cchemov[ichem][i][j] = (float *)malloc((maxstackov+1) * sizeof(float));

				}	//end if imask[][] > 0

			}	//end loop over columns

		}	//end loop over rows

	}	//end loop over chemicals

	//Read soil chemical initial conditions
	//
	//Loop over layers in reverse order (!!) (top down) (bottom is Layer 1)
	for(ilayer=maxstackov; ilayer>=1; ilayer--)
	{
		//Loop over number of chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{	
			//Record 12 (Part 1)
			fscanf(inputfile_fp, "%s",	//read
			varname);					//dummy

			//Record 12 (Part 2)
			fgets(soilchemicfile, MAXNAMESIZE, inputfile_fp);	//read soil chemical IC file name

			//strip leading blanks, trailing blanks, and final carriage return from string
			soilchemicfile = StripString(soilchemicfile);

			//Echo soil chemical initial condition file name to file
			fprintf(echofile_fp, "\n\nLayer: %d  Chemical: %d\n",
				ilayer, ichem);
			fprintf(echofile_fp, "\nSoil Initial Condition File: %s\n",
				soilchemicfile);

			//Read chemical initial condition file for this layer
			ReadSoilLayerChemicalFile(ilayer, ichem);

		}	//end loop over chemicals

	}	//loop over layers (in reverse order)

	//Record 13
	fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

	//Echo header to file
	fprintf(echofile_fp, "\n\n%s\n", header);

	//Allocate memory for overland initial chemical water column concentration file name
	initialchemovfile = (char *)malloc(MAXNAMESIZE * sizeof(char));

	//Loop over number of chemicals
	for(ichem=1; ichem<=nchems; ichem++)
	{	
		//Record 14 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
		varname);					//dummy

		//Record 14 (Part 2)
		fgets(initialchemovfile, MAXNAMESIZE, inputfile_fp);	//read overland water chemical IC file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		initialchemovfile = StripString(soilchemicfile);

		//Echo soil chemical initial condition file name to file
		fprintf(echofile_fp, "\n\nChemical: %5d  Overland Initial Condition File: %s\n",
			ichem, initialchemovfile);

		//Read initial chemical overland file
		ReadInitialChemicalOverlandFile(ichem);

	}	//end loop over chemicals

	//if channels are simulated)
	if(chnopt > 0)
	{
		//Record 15
		fgets(header, MAXHEADERSIZE, inputfile_fp);	//Read header

		//Echo header to file
		fprintf(echofile_fp, "\n\n%s\n", header);

		//Allocate memory for sediment chemical initial condition file name
		sedimentchemicfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Allocate memory for global channel chemical concentration
		cchemch = (float ****)malloc((nchems+1) * sizeof(float ***));

		//loop over chemicals
		for(ichem=1; ichem<=nchems; ichem++)
		{
			//Allocate additional memory for channel chemical concentration
			cchemch[ichem] = (float ***)malloc((nlinks+1) * sizeof(float **));

			//loop over links
			for(i=1; i<=nlinks; i++)
			{
				//Allocate additional memory for channel chemical concentration
				cchemch[ichem][i] = (float **)malloc((nnodes[i]+1) * sizeof(float *));

				//loop over nodes
				for(j=1; j<=nnodes[i]; j++)
				{
					//Allocate remaining memory for channel chemical concentration
					cchemch[ichem][i][j] = (float *)malloc((maxstackch+1) * sizeof(float));

					//Note:  Because the sediment bed properties file only specified
					//       chemical concentration values for occupied layers of the
					//       stack, cchemch values for all layers (including the water
					//       column (Layer 0) are initialized to zero here rather than
					//       in InitializeChemical to simplify the model set-up process.
					//       If the initialization was done in InitializeChemical, the
					//       logic needed would be more complicated to prevent wiping
					//       out the initial condition read in ReadDataGroupD...
					//
					//loop over all layers (surface water and all possible bed layers)
					for(k=0;k<=maxstackch;k++)
					{
						//set the channel chemical concentration to zero
						cchemch[ichem][i][j][k] = 0.0;

					}	//end loop over all layers

				}	//end loop over nodes

			}	//end loop over links

		}	//end loop over chemicals

		//Record 16 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 16 (Part 2)
		fgets(sedimentchemicfile, MAXNAMESIZE, inputfile_fp);	//read sediment chemical IC file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		sedimentchemicfile = StripString(sedimentchemicfile);

		//Echo sediment chemical initial condition file name to file
		fprintf(echofile_fp, "\n\nSediment Chemical Initial Condition File: %s\n",
			sedimentchemicfile);

		//Read sediment chemical initial condition file
		ReadSedimentChemicalFile();

		//Allocate memory for initial chemical channel file name
		initialchemchfile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

		//Record 17 (Part 1)
		fscanf(inputfile_fp, "%s",	//read
			varname);				//dummy

		//Record 17 (Part 2)
		fgets(initialchemchfile, MAXNAMESIZE, inputfile_fp);	//read initial channel chemical file name

		//strip leading blanks, trailing blanks, and final carriage return from string
		initialchemchfile = StripString(initialchemchfile);

		//Echo initial channel chemical file name to file
		fprintf(echofile_fp, "\n\nSediment Chemical Initial Condition File: %s\n",
			initialchemchfile);

		//Read initial chemical channel file
		ReadInitialChemicalChannelFile();

	}	//end if chnopt > 0

	//Write label for overland chemical point source loads/forcing functions
	fprintf(echofile_fp, "\n\n\n  Overland Point Source Chemical Loads/Forcing Functions  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for overland chemical point source loads/forcing functions (w)
	ncwpov = (int *)malloc((nchems+1) * sizeof(int));				//allocate memory
	cwpovrow = (int **)malloc((nchems+1) * sizeof(int *));			//allocate memory
	cwpovcol = (int **)malloc((nchems+1) * sizeof(int *));			//allocate memory
	ncwpovpairs = (int **)malloc((nchems+1) * sizeof(int *));		//allocate memory
	cwpovopt = (int **)malloc((nchems+1) * sizeof(int *));			//allocate memory
	cwpov = (float ***)malloc((nchems+1) * sizeof(float **));		//allocate memory
	cwpovtime = (float ***)malloc((nchems+1) * sizeof(float **));	//allocate memory

	//loop over number of chemicals
	for(i=1; i<=nchems; i++)
	{
		//Record 18
		fscanf(inputfile_fp, "%s %d",	//read
			loadname,					//general description of chemical loads
			&ncwpov[i]);				//number of overland point source chemical loads for current chemical type

		//Echo number of loads and description to file
		fprintf(echofile_fp, "\n\n %s %d  %s %d  %s %s\n",
			"Chemical Type: ", i, "Number of Loads: ", ncwpov[i],
			"Description: ", loadname);

		//if the number of overland point source chemical loads > 0
		if(ncwpov[i] > 0)
		{
			//Record 19
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

			//Allocate additional memory for overland point source chemical loads/forcing functions
			cwpovrow[i] = (int *)malloc((ncwpov[i]+1) * sizeof(int));			//allocate memory
			cwpovcol[i] = (int *)malloc((ncwpov[i]+1) * sizeof(int));			//allocate memory
			ncwpovpairs[i] = (int *)malloc((ncwpov[i]+1) * sizeof(int));		//allocate memory
			cwpovopt[i] = (int *)malloc((ncwpov[i]+1) * sizeof(int));			//allocate memory
			cwpov[i] = (float **)malloc((ncwpov[i]+1) * sizeof(float *));		//allocate memory
			cwpovtime[i] = (float **)malloc((ncwpov[i]+1) * sizeof(float *));	//allocate memory

			//Loop over number of overland point source loads for current chemical type
			for(j=1; j<=ncwpov[i]; j++)
			{
				//Record 20 (Part 1)
				fscanf(inputfile_fp, "%d %d %d %d",	//read
					&cwpovrow[i][j],				//overland point source chemical load row
					&cwpovcol[i][j],				//overland point source chemical load column
					&ncwpovpairs[i][j],				//number of pairs (time breaks) in function
					&cwpovopt[i][j]);				//overland point source chemical load option

				//Record 20 (Part 2)
				fgets(loadname, MAXNAMESIZE, inputfile_fp);		//Read loadname

				//Echo number, row, column, and number of pairs for current load to file
				fprintf(echofile_fp, "\n\n %s %5d %s %5d %s %5d %s %5d %s %5d %s %s\n\n",
					"  Load Number: ", j,
					"  Row Number: ", cwpovrow[i][j],
					"  Column Number: ", cwpovcol[i][j],
					"  Number of Time Breaks: ", ncwpovpairs[i][j],
					"  Load Option: ", cwpovopt[i][j],
					"  Description: ", loadname);

				//Allocate remaining memory for point source chemical loads/forcing functions
				cwpov[i][j] = (float *)malloc((ncwpovpairs[i][j]+1) * sizeof(float));		//allocate memory
				cwpovtime[i][j] = (float *)malloc((ncwpovpairs[i][j]+1) * sizeof(float));	//allocate memory

				//Write label for load time series to file
				//
				//if the load option equals 1
				if(cwpovopt[i][j] == 1)
				{
					fprintf(echofile_fp, "  Concentration (g/m3)     ");	//load in g/m3 (concentration) when cwpovopt = 1
					fprintf(echofile_fp, "  Time (days)  \n");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~~~~~~~   ");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");
				}
				else	//else cwpovopt[][] != 1
				{
					fprintf(echofile_fp, "  Load (kg/day)     ");	//load in kg/day when cwpovopt = 0
					fprintf(echofile_fp, "  Time (days)  \n");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~   ");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

				}	//end if cwpovopt == 1

				//Loop over number of pairs in current load for current chemical type
				for(k=1; k <= ncwpovpairs[i][j]; k++)
				{
					//Record 21
					fscanf(inputfile_fp, "%f %f",	//read
						&cwpov[i][j][k],			//chemical load value
						&cwpovtime[i][j][k]);		//chemical load time break

					//Apply conversion and scale factors
					cwpov[i][j][k] = cwpov[i][j][k] * convunits * convtime * scale;

					//Echo load value and time break to file
					fprintf(echofile_fp, "%16.3f %19.3f\n",
						cwpov[i][j][k], cwpovtime[i][j][k]);

				}	//end loop over number of pairs

			}	//end loop over number of overland loads

		}	//end if ncwpov > 0

	}	//end loop over number of chemical types

	//Write label for overland disrtibuted chemical loads/forcing functions
	fprintf(echofile_fp, "\n\n\n  Overland Distributed Chemical Loads/Forcing Functions  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for overland distributed chemical loads/forcing functions (wdov)
	ncwdov = (int *)malloc((nchems+1) * sizeof(int));				//allocate memory
	cwdovarea = (float ****)malloc((nchems+1) * sizeof(float ***));	//allocate memory
	ncwdovcells = (int **)malloc((nchems+1) * sizeof(int *));		//allocate memory
	cwdovrow = (int ***)malloc((nchems+1) * sizeof(int **));		//allocate memory
	cwdovcol = (int ***)malloc((nchems+1) * sizeof(int **));		//allocate memory
	ncwdovpairs = (int **)malloc((nchems+1) * sizeof(int *));		//allocate memory
	cwdov = (float ***)malloc((nchems+1) * sizeof(float **));		//allocate memory
	cwdovtime = (float ***)malloc((nchems+1) * sizeof(float **));	//allocate memory

	//Allocate memory for overland distributed load area file name
	cwdovareafile = (char *)malloc(MAXNAMESIZE * sizeof(char));	//allocate memory

	//Note:  In the following loop, ichem is used the loop index rather than
	//       "i" because i (and j) are global variables and are used in the
	//       ReadCWDovAreaFile as loop indices for row and column.  Use of
	//       ichem prevents "losing" the proper value of the index when area
	//       data are read by ReadCWDovAreaFile.
	//
	//loop over number of chemical
	for(ichem=1; ichem<=nchems; ichem++)
	{
		//Record 22
		fscanf(inputfile_fp, "%s %d",	//read
			loadname,					//general description of chemical loads
			&ncwdov[ichem]);				//number of overland disributed chemical loads for current chemical type

		//Echo number of loads and description to file
		fprintf(echofile_fp, "\n\n %s %d  %s %d  %s %s\n",
			"Chemical Type: ", ichem, "Number of Loads: ", ncwdov[ichem], "Description: ", loadname);

		//Allocate additional memory for overland distributed chemical loads/forcing functions
		cwdovarea[ichem] = (float ***)malloc((ncwdov[ichem]+1) * sizeof(float **));	//allocate memory
		ncwdovcells[ichem] = (int *)malloc((ncwdov[ichem]+1) * sizeof(int));		//allocate memory
		cwdovrow[ichem] = (int **)malloc((ncwdov[ichem]+1) * sizeof(int *));		//allocate memory
		cwdovcol[ichem] = (int **)malloc((ncwdov[ichem]+1) * sizeof(int *));		//allocate memory
		ncwdovpairs[ichem] = (int *)malloc((ncwdov[ichem]+1) * sizeof(int));		//allocate memory
		cwdov[ichem] = (float **)malloc((ncwdov[ichem]+1) * sizeof(float *));		//allocate memory
		cwdovtime[ichem] = (float **)malloc((ncwdov[ichem]+1) * sizeof(float *));	//allocate memory

		//if the number of overland chemical loads > 0
		if(ncwdov[ichem] > 0)
		{
			//Note:  In the following loop, iload is used as the loop index rather than
			//       "j" because i (and j) are global variables and are used in the
			//       ReadCWDovAreaFile as loop indices for row and column.  Use of
			//       iload prevents "losing" the proper value of the index when area
			//       data are read by ReadCWDovAreaFile.
			//
			//loop over number of overland distributed source loads for current chemical type
			for(iload=1; iload<=ncwdov[ichem]; iload++)
			{
				//Record 23 (Part 1)
				fscanf(inputfile_fp, "%s",	//read
					varname);				//dummy

				//Record 23 (Part 2)
				fgets(cwdovareafile, MAXNAMESIZE, inputfile_fp);	//read overland distributed load area file name

				//strip leading blanks, trailing blanks, and final carriage return from string
				cwdovareafile = StripString(cwdovareafile);

				//Echo channel chemical initial condition file name to file
				fprintf(echofile_fp, "\n  Overland Distributed Load Area File: %s\n",
					cwdovareafile);

				//Read overland distributed chemical load area file
				ReadCWDovAreaFile(ichem, iload);

				//Record 24
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

				//Record 25 (Part 1)
				fscanf(inputfile_fp, "%d",	//read
					&ncwdovpairs[ichem][iload]);	//number of pairs (time breaks) in function

				//Record 25 (Part 2)
				fgets(loadname, MAXNAMESIZE, inputfile_fp);		//Read loadname

				//Echo number, and number of pairs for current load to file
				fprintf(echofile_fp, "\n\n %s %5d %s %5d %s %s\n\n",
					"  Load Number: ", iload,
					"  Number of Time Breaks: ", ncwdovpairs[ichem][iload],
					"  Description: ", loadname);

				//Allocate remaining memory for chemical loads/forcing functions
				cwdov[ichem][iload] = (float *)malloc((ncwdovpairs[ichem][iload]+1) * sizeof(float));		//allocate memory
				cwdovtime[ichem][iload] = (float *)malloc((ncwdovpairs[ichem][iload]+1) * sizeof(float));	//allocate memory

				//Write label for load time series to file
				fprintf(echofile_fp, "  Load (g/m2/mm rain)     ");	//load in g/m2/mm rain
				fprintf(echofile_fp, "  Time (hours)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~~~~~~~   ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~~\n");

				//Loop over number of pairs in current load for current chemical type
				for(k=1; k<=ncwdovpairs[ichem][iload]; k++)
				{
					//Record 26
					fscanf(inputfile_fp, "%f %f",	//read
						&cwdov[ichem][iload][k],			//chemical load value
						&cwdovtime[ichem][iload][k]);		//chemical load time break

					//Apply conversion and scale factors
					cwdov[ichem][iload][k] = cwdov[ichem][iload][k] * convunits * convtime * scale;

					//Echo load value and time break to file
					fprintf(echofile_fp, "%16.3e %20.3f\n",
						cwdov[ichem][iload][k], cwdovtime[ichem][iload][k]);

				}	//end loop over number of pairs

			}	//end loop over number of overland distributed loads

		}	//end if ncwdov > 0

	}	//end loop over number of chemical types

	//if channels are simulated)
	if(chnopt > 0)
	{
		//Write label for channel chemical loads/forcing functions
		fprintf(echofile_fp, "\n\n\n  Chemical Loads/Forcing Functions (Channels)  \n");
		fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

		//Allocate memory for channel chemical loads/forcing functions (w)
		ncwch = (int *)malloc((nchems+1) * sizeof(int));				//allocate memory
		cwchlink = (int **)malloc((nchems+1) * sizeof(int *));			//allocate memory
		cwchnode = (int **)malloc((nchems+1) * sizeof(int *));			//allocate memory
		ncwchpairs = (int **)malloc((nchems+1) * sizeof(int *));		//allocate memory
		cwchopt = (int **)malloc((nchems+1) * sizeof(int *));			//allocate memory
		cwch = (float ***)malloc((nchems+1) * sizeof(float **));		//allocate memory
		cwchtime = (float ***)malloc((nchems+1) * sizeof(float **));	//allocate memory

		//loop over number of chemicals
		for(i=1; i<=nchems; i++)
		{
			//Record 27
			fscanf(inputfile_fp, "%s %d",	//read
				loadname,					//general description of chemical loads
				&ncwch[i]);					//number of channel chemical loads for current chemical type

			//Echo number of loads and description to file
			fprintf(echofile_fp, "\n\n %s %d  %s %d  %s %s\n",
				"Chemical Type:", i, "Number of Loads:", ncwch[i], "Description:", loadname);

			//if the number of channel chemical loads > 0
			if(ncwch[i] > 0)
			{
				//Record 28
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

				//Allocate additional memory for chemical loads/forcing functions
				cwchlink[i] = (int *)malloc((ncwch[i]+1) * sizeof(int));		//allocate memory
				cwchnode[i] = (int *)malloc((ncwch[i]+1) * sizeof(int));		//allocate memory
				ncwchpairs[i] = (int *)malloc((ncwch[i]+1) * sizeof(int));		//allocate memory
				cwchopt[i] = (int *)malloc((ncwch[i]+1) * sizeof(int));			//allocate memory
				cwch[i] = (float **)malloc((ncwch[i]+1) * sizeof(float *));		//allocate memory
				cwchtime[i] = (float **)malloc((ncwch[i]+1) * sizeof(float *));	//allocate memory

				//Loop over number of channel loads for current chemical type
				for(j=1; j <= ncwch[i]; j++)
				{
					//Record 29 (Part 1)
					fscanf(inputfile_fp, "%d %d %d %d",	//read
						&cwchlink[i][j],				//chemical load Link
						&cwchnode[i][j],				//chemical load Node
						&ncwchpairs[i][j],				//number of pairs (time breaks) in function
						&cwchopt[i][j]);				//chemcal load option

					//Record 29 (Part 2)
					fgets(loadname, MAXNAMESIZE, inputfile_fp);		//Read loadname

					//Echo number, link, node, and number of pairs for current load to file
					fprintf(echofile_fp, "\n\n %s %5d %s %5d %s %5d %s %5d %s %5d %s %s\n\n",
						"  Load Number: ", j,
						"  Link Number: ", cwchlink[i][j],
						"  Node Number: ", cwchnode[i][j],
						"  Number of Time Breaks: ", ncwchpairs[i][j],
						"  Load Option: ", cwchopt[i][j],
						"  Description: ", loadname);

					//Allocate remaining memory for chemical loads/forcing functions
					cwch[i][j] = (float *)malloc((ncwchpairs[i][j]+1) * sizeof(float));		//allocate memory
					cwchtime[i][j] = (float *)malloc((ncwchpairs[i][j]+1) * sizeof(float));	//allocate memory

					//Write label for load time series to file
					fprintf(echofile_fp, "  Load (kg/day)     ");	//need to adopt consistent units
					fprintf(echofile_fp, "  Time (days)  \n");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~~~   ");
					fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

					//Loop over number of pairs in current load for current chemical type
					for(k=1; k<=ncwchpairs[i][j]; k++)
					{
						//Record 30
						fscanf(inputfile_fp, "%f %f",	//read
							&cwch[i][j][k],				//chemical load value
							&cwchtime[i][j][k]);		//chemical load time break

						//Apply conversion and scale factors
						cwch[i][j][k] = cwch[i][j][k] * convunits * convtime * scale;

						//Echo load value and time break to file
						fprintf(echofile_fp, "%16.3f %19.3f\n",
							cwch[i][j][k], cwchtime[i][j][k]);

					}	//end loop over numner of pairs

				}	//end loop over number of channel loads

			}	//end if ncwch > 0

		}	//end loop over number of chemical types

	}	//end if chnopt > 0

	//Housekeeping: skip extra carriage return
	fgets(header, MAXHEADERSIZE, inputfile_fp);

	//Write label for chemical boundary condition (bc) functions
	fprintf(echofile_fp, "\n\n\n  Chemical Boundary Condition Functions (Outlets)  \n");
	fprintf(echofile_fp,       "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");

	//Allocate memory for chemical boundary conditions
	ncbcpairs = (int **)malloc((noutlets+1) * sizeof(int *));		//allocate memory
	cbc = (float ***)malloc((noutlets+1) * sizeof(float **));		//allocate memory
	cbctime = (float ***)malloc((noutlets+1) * sizeof(float **));	//allocate memory

	//Loop over number of outlets
	for(i=1; i<=noutlets; i++)
	{
		//Record 31
		fgets(header, MAXHEADERSIZE, inputfile_fp);	//read header from file

		//Echo header to file
		fprintf(echofile_fp, "\n\n%s\n", header);

		//if the domain boundary condition option for this outlet > 0
		if(dbcopt[i] > 0)
		{
			//Record 32
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

			//Allocate additional memory for chemical boundary conditions
			ncbcpairs[i] = (int *)malloc((nchems+1) * sizeof(int));		//allocate memory
			cbc[i] = (float **)malloc((nchems+1) * sizeof(float *));		//allocate memory
			cbctime[i] = (float **)malloc((nchems+1) * sizeof(float *));	//allocate memory

			//Loop over number of chemicals
			for(j=1; j<=nchems; j++)
			{
				//Record 33 (Part 1)
				fscanf(inputfile_fp, "%d",
					&ncbcpairs[i][j]);	//Number of pairs for this chemical boundary conditions

				//Record 33 (Part 2)
				fgets(bcname, MAXNAMESIZE, inputfile_fp);	//Reads boundary condition name from file

				//Echo outlet, row, column, chemical type, and number of pairs for current bc to file
				fprintf(echofile_fp, "\n\n%s %5d %s %5d %s %5d %s %5d %s %5d %s %s\n\n",
					"  Outlet: ", i,
					"  Row Number: ", iout[i],
					"  Column Number: ", jout[i],
					"  Chemical Type: ", j,
					"  Number of Time Breaks: ", ncbcpairs[i][j],
					"  Description: ", bcname);

				//Allocate remaining memory for chemical boundary conditions
				cbc[i][j] = (float *)malloc((ncbcpairs[i][j]+1) * sizeof(float));		//allocate memory
				cbctime[i][j] = (float *)malloc((ncbcpairs[i][j]+1) * sizeof(float));	//allocate memory

				//Write label for boundary condition time series to file
				fprintf(echofile_fp, "  BC (g/m3)     ");	//need to adopt consistent units
				fprintf(echofile_fp, "  Time (days)  \n");
				fprintf(echofile_fp, "~~~~~~~~~~~~~   ");
				fprintf(echofile_fp, "~~~~~~~~~~~~~~~\n");

				//loop over number of pairs (time breaks)
				for(k=1; k<=ncbcpairs[i][j]; k++)
				{
					//Record 34
					fscanf(inputfile_fp, "%f %f",	//read
						&cbc[i][j][k],				//Boundary Condition value
						&cbctime[i][j][k]);			//Boundary Condition time break

					//Apply conversion and scale factors
					cbc[i][j][k] = cbc[i][j][k] * convunits * convtime * scale;

					//Echo boundary condition value and time break to file
					fprintf(echofile_fp, "%13.3f %18.3f\n",
						cbc[i][j][k], cbctime[i][j][k]);

				}	//end loop over number of pairs

			}	//end loop over number of chemicals

		}	//end if dbcopt[i] > 0

	}	//end loop over number of outlets

	//Record 35
	fscanf(inputfile_fp, "%s %d",	//read
		varname,					//dummy
		&nchemreports);				//number of chemical reporting stations

	//Echo Number of Chemical Reporting Stations to file
	fprintf(echofile_fp, "\n\n\nNumber of Chemical Reporting Stations = %d\n", nchemreports);

	//Allocate memory for chemical reporting stations
	chemreprow = (int *)malloc((nchemreports+1) * sizeof(int));
	chemrepcol = (int *)malloc((nchemreports+1) * sizeof(int));
	chemarea = (float *)malloc((nchemreports+1) * sizeof(float));
	chemunitsopt = (int *)malloc((nchemreports+1) * sizeof(int));
	
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

	//Loop over number of chemical reporting stations
	for(i=1; i<=nchemreports; i++)
	{
		//Record 36
		fscanf(inputfile_fp, "%d %d %f %d %s",	//read
			&chemreprow[i],					//reporting station cell row
			&chemrepcol[i],					//reporting station cell column
			&chemarea[i],					//reporting station cell (upstream) drainage area
			&chemunitsopt[i],				//flow report units option
			varname);						//station identifier

		//Echo Station (loop counter), Row, Column, Area, and Units Option to file
		fprintf(echofile_fp, "%11d   %12d   %15d   %23.3f   %23d %s\n",
			i, chemreprow[i], chemrepcol[i], chemarea[i], chemunitsopt[i], varname);

	}	//end loop over number of chemical reports

	//Close the Echo File
	fclose(echofile_fp);

//End of function: Return to ReadInputFile
}
