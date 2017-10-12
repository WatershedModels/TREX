/*----------------------------------------------------------------------
C-  Header:     trex_chemical_declarations.h
C-
C-	Purpose/	Global declarations header file for chemical transport
C-	Methods:	caculations.  Used with trex_chemical_definitions.h.
C-
C-	Created:	Mark Velleux
C-              Department of Civil Engineering
C-              Colorado State University
C-              Fort Collins, CO  80523
C-
C-				John F. England, Jr.
C-				Bureau of Reclamation
C-				Flood Hydrology Group, D-8530
C-				Bldg. 67, Denver Federal Center
C-              Denver, CO  80225
C-
C-	Date:		16-MAR-2004
C-
C-	Revised:
C-
C-	Date:
C-
C-	Revisions:
C-
C---------------------------------------------------------------------*/

//general order of header:
//	1. include files
//	2. global constant definitions
//	3. file declarations (organized by DataGroups A-E)
//	4. function prototypes (organized by order of appearance/DataGroups A-E)
//	5. variable declarations by state variables, general controls, then ksim
//		(1=water, 2=solids, 3=chemicals)

/*************************/
/*   FILE DECLARATIONS   */
/*************************/

//Data Group D
extern FILE *soilchemicfile_fp;			//soil chemical initial conditions file pointer
extern FILE *initialchemovfile_fp;		//initial chemical overland file pointer
extern FILE *sedimentchemicfile_fp;		//sediment chemical initial conditions file pointer
extern FILE *initialchemchfile_fp;		//initial chemical channel file pointer
extern FILE *cwdovareafile_fp;			//overland distributed chemical load area file pointer

//Data Group F
extern FILE *chemexpfile_fp;			//chemical export file pointer
extern FILE *chemconcwatergrid_fp;		//solids water column concentration grid file pointer
extern FILE *chemconcsurfgrid_fp;		//solids surface layer concentration grid file pointer


/***************************/
/*   FUNCTION PROTOTYPES   */
/***************************/

extern void ReadDataGroupD(void);						//Reads Data Group D of input file
extern void ReadSoilLayerChemicalFile(int, int);		//Reads grid files of soil layer chemical initial conditions
extern void ReadInitialChemicalOverlandFile(int);		//Reads grid files of initial chemical concentrations in the overland plane
extern void ReadSedimentChemicalFile(void);				//Reads sediment chemical initial condition file
extern void ReadInitialChemicalChannelFile(void);		//Reads chemical initial chemical concentrations for channel network
extern void ChemicalPropertyInit(void);					//Allocate memory and set default values for chemical properties
extern void ReadCWDovAreaFile(int, int);				//Reads grid files of area for overland distributed chemical loads

extern void InitializeChemical(void);					//Initializes chemical transport variables (sets initial values)
extern void ComputeInitialStateChemical(void);			//Computes initial states of chemical variables at beginning of simulation
extern void TimeFunctionInitChemical(void);				//Initializes piecewise linear time functions for chemical (Loads, BCs)
extern void UpdateTimeFunctionChemical(void);			//Updates piecewise linear time functions for chemicals (Loads, BCs)

extern void ChemicalTransport(void);					//Computes derivative terms for chemical transport processes
extern void OverlandChemicalAdvection(void);			//Computes advective flux of chemicals in overland plane
extern void OverlandChemicalDispersion(void);			//Computes dispersive flux of chemicals in overland plane
extern void OverlandChemicalErosion(void);				//Computes erosion flux of chemicals in overland plane
extern void OverlandChemicalDeposition(void);			//Computes deposition flux of chemicals in overland plane
extern void OverlandChemicalInfiltration(void);			//Compute overland chemical infiltration (to soils)
extern void OverlandChemicalKinetics(void);				//Computes kinetic reaction flux of chemicals in overland plane
extern void ChannelChemicalAdvection(void);				//Computes advective flux of chemicals in channels
extern void ChannelChemicalDispersion(void);			//Computes dispersive flux of chemicals in channels
extern void ChannelChemicalErosion(void);				//Computes erosion flux of chemicals in channels
extern void ChannelChemicalDeposition(void);			//Computes deposition flux of chemicals in channels
extern void ChannelChemicalTransmissionLoss(void);		//Computes channel chemical transmission loss (to sediments)
extern void ChannelChemicalKinetics(void);				//Computes kinetic reaction flux of chemicals in channels
extern void FloodplainChemicalTransfer(void);			//Transfers chemicals between overland plane and channels

extern void OverlandChemicalPartitioning(void);			//Compute overland chemical partitioning
extern void OverlandChemicalBiodegradation(void);		//Compute overland chemical biodegradation
extern void OverlandChemicalHydrolysis(void);			//Compute overland chemical hydrolysis
extern void OverlandChemicalOxidation(void);			//Compute overland chemical oxidation
extern void OverlandChemicalPhotolysis(void);			//Compute overland chemical photolysis
extern void OverlandChemicalRadioactive(void);			//Compute overland chemical radioactive decay
extern void OverlandChemicalVolatilization(void);		//Compute overland chemical volatilization
extern void OverlandChemicalUDReaction(void);			//Compute overland chemical user-defined reaction
extern void OverlandChemicalYield(void);				//Compute overland chemical yield (transformation between chemical types)
extern void OverlandChemicalDissolution(void);			//Computes dissolution flux of solids in overland plane

extern void ChannelChemicalPartitioning(void);		//Compute channel chemical partitioning
extern void ChannelChemicalBiodegradation(void);	//Compute channel chemical biodegradation
extern void ChannelChemicalHydrolysis(void);		//Compute channel chemical hydrolysis
extern void ChannelChemicalOxidation(void);			//Compute channel chemical oxidation
extern void ChannelChemicalPhotolysis(void);		//Compute channel chemical photolysis
extern void ChannelChemicalRadioactive(void);		//Compute channel chemical radioactive decay
extern void ChannelChemicalVolatilization(void);	//Compute channel chemical volatilization
extern void ChannelChemicalUDReaction(void);		//Compute channel chemical user-defined reaction
extern void ChannelChemicalYield(void);				//Compute channel chemical yield (transformation between chemical types)
extern void ChannelChemicalDissolution(void);		//Computes dissolution flux of solids in channels

extern void ChemicalBalance(void);						//Computes mass balance for chemicals
extern void OverlandChemicalConcentration(void);		//Computes mass balance for chemical concentration in overland plane
extern void ChannelChemicalConcentration(void);			//Computes mass balance for chemical concentration in channels

extern void NewStateChemical(void);		//Set new state (conditions) for chemicals (concentrations) for next time step

extern void WriteTimeSeriesChemical(void);	//Writes time series (export) chemical files
extern void WriteGridsChemical(int);	//Writes chemical grid files
extern void WriteEndGridsChemical(int);	//Writes end of simulation chemical grid files
extern void ComputeChemicalGroupConcSums(float *, int, int, int, int);	//Compute sums of chemical group concentrations
extern void ComputeChemicalGroupPhaseAve(float *, int, int, int, int);	//Compute averages of chemical group phase distributions
extern void ComputeChemicalGroupTransportSums(float *, int, int, int);	//Compute sums of chemical group transport flux/mass

extern void ComputeFinalStateChemical(void);	//Computes final states of chemical variables at end of simulation
extern void WriteMassBalanceChemical(void);		//Writes end of simulation mass balance for water
extern void WriteSummaryChemical(void);			//Writes summary (chemical) statistics at end of simulation
extern void FreeMemoryChemical(void);			//Frees allocated memory for chemical variables at end of simulation

/************************************/
/*   GLOBAL VARIABLE DECLARATIONS   */
/************************************/

	/**********************/
	//  state variables
	/**********************/
extern float
	****cchemov,		//chemical concentration in overland plane grid cell (at current time t) (g/m3) [ichem][r][c][layer]
	****cchemovnew,		//new chemical concentration in overland plane grid cell (for next time t+dt) (g/m3) [ichem][r][c][layer]
	****cchemch,		//chemical concentration in channel node (at current time t) (g/m3) [ichem][r][c][layer]
	****cchemchnew;		//new chemical concentration in channel node (for next time t+dt) (g/m3) [ichem][r][c][layer]


	/********************************************************/
	//  chemicals (ksim = 3) controls, support, reporting
	/********************************************************/

extern char
	*soilchemicfile,		//soil chemical initial condition file
	*initialchemovfile,     //initial chemical concentration overland file
	*sedimentchemicfile,	//sediment chemical initial condition file
	*initialchemchfile,     //initial chemical concentration channel file
	*chemexprootfile,		//chemical export root file name
	*chemextension,			//chemical export file name extension
	**chemexpfile,			//chemical export file
	**cgroupname,			//name of solids reporting group [nsgroups]
	**chemname,				//chemical name[nchems]

	*cwdovareafile,			//overland chemical distributed load area file name

	*totchemconcwatergridroot,	//total chemical water column concentration grid root name
	*dischemconcwatergridroot,	//dissolved chemical water column concentration grid root name
	*bndchemconcwatergridroot,	//bound chemical water column concentration grid root name
	*prtchemconcwatergridroot,	//particulate chemical water column concentration grid root name
	*srbchemconcwatergridroot,	//sorbed chemical water column concentration grid root name
	**totchemconcwatergrid,		//total chemical water column concentration grid file name [ncgroups]
	**dischemconcwatergrid,		//dissolved chemical water column concentration grid file name [ncgroups]
	**bndchemconcwatergrid,		//bound chemical water column concentration grid file name [ncgroups]
	**prtchemconcwatergrid,		//particulate chemical water column concentration grid file name [ncgroups]
	**srbchemconcwatergrid,		//sorbed chemical water column concentration grid file name [ncgroups]

	*totchemconcsurfgridroot,	//total chemical surface layer concentration grid root name
	*dischemconcsurfgridroot,	//dissolved chemical surface layer concentration grid root name
	*bndchemconcsurfgridroot,	//bound chemical surface layer concentration grid root name
	*prtchemconcsurfgridroot,	//particulate chemical surface layer concentration grid root name
	*srbchemconcsurfgridroot,	//sorbed chemical surface layer concentration grid root name
	**totchemconcsurfgrid,		//total chemical surface layer concentration grid file name [ncgroups]
	**dischemconcsurfgrid,		//dissolved chemical surface layer concentration grid file name [ncgroups]
	**bndchemconcsurfgrid,		//bound chemical surface layer concentration grid file name [ncgroups]
	**prtchemconcsurfgrid,		//particulate chemical surface layer concentration grid file name [ncgroups]
	**srbchemconcsurfgrid,		//sorbed chemical surface layer concentration grid file name [ncgroups]

	*totchemconcsubgridroot,	//total chemical subsurface layer concentration grid root name
	*dischemconcsubgridroot,	//dissolved chemical subsurface layer concentration grid root name
	*bndchemconcsubgridroot,	//bound chemical subsurface layer concentration grid root name
	*prtchemconcsubgridroot,	//particulate chemical subsurface layer concentration grid root name
	*srbchemconcsubgridroot,	//sorbed chemical subsurface layer concentration grid root name
	**totchemconcsubgrid,		//total chemical subsurface layer concentration grid file name [ncgroups]
	**dischemconcsubgrid,		//dissolved chemical subsurface layer concentration grid file name [ncgroups]
	**bndchemconcsubgrid,		//bound chemical subsurface layer concentration grid file name [ncgroups]
	**prtchemconcsubgrid,		//particulate chemical subsurface layer concentration grid file name [ncgroups]
	**srbchemconcsubgrid,		//sorbed chemical subsurface layer concentration grid file name [ncgroups]

	*dischemfracwatergridroot,	//chemical water column dissolved fraction grid root name
	*bndchemfracwatergridroot,	//chemical water column bound fraction grid root name
	*mblchemfracwatergridroot,	//chemical water column mobile fraction grid root name
	*prtchemfracwatergridroot,	//chemical water column particulate fraction grid root name
	**dischemfracwatergrid,		//chemical water column dissolved fraction grid file name [ncgroups]
	**bndchemfracwatergrid,		//chemical water column bound fraction grid file name [ncgroups]
	**mblchemfracwatergrid,		//chemical water column mobile fraction grid file name [ncgroups]
	**prtchemfracwatergrid,		//chemical water column particulate fraction grid file name [ncgroups]

	*dischemfracsurfgridroot,	//chemical surface layer dissolved fraction grid root name
	*bndchemfracsurfgridroot,	//chemical surface layer bound fraction grid root name
	*mblchemfracsurfgridroot,	//chemical surface layer mobile fraction grid root name
	*prtchemfracsurfgridroot,	//chemical surface layer particulate fraction grid root name
	**dischemfracsurfgrid,		//chemical surface layer dissolved fraction grid file name [ncgroups]
	**bndchemfracsurfgrid,		//chemical surface layer bound fraction grid file name [ncgroups]
	**mblchemfracsurfgrid,		//chemical surface layer mobile fraction grid file name [ncgroups]
	**prtchemfracsurfgrid,		//chemical surface layer particulate fraction grid file name [ncgroups]

	*infchemfluxgridroot,		//chemical infiltration flux grid root name
	**infchemfluxgrid,			//chemical infiltration flux grid file name [ncgroups]

	*chemerosiongridroot,		//chemical gross erosion grid root name
	**chemerosiongrid,			//chemical gross erosion grid file name [nsgroups]
	*chemdepositiongridroot,	//chemical gross deposition grid root name
	**chemdepositiongrid,		//chemical gross deposition grid file name [nsgroups]
	*chemnetaccumgridroot,		//chemical gross netaccum grid root name
	**chemnetaccumgrid;			//chemical gross netaccum grid file name [nsgroups]

extern int
	nchems,				//Number of shemicals
	ncgroups,			//Number of chemical Reporting Groups
	*cgroupnumber,		//chemical reporting group number [ichem]
	ncyields,			//Number of chemical reaction yields
	*cyldfrom,			//Reacting chemical number
	*cyldto,			//Product chemical number
	*cyldprocess,		//Process number for chemical reaction
	nchemreports,		//Number of chemical reports (reporting stations)

	*ncwpov,			//Number of overland point source chemical loads
	**cwpovopt,			//Overland point source chemical load option (0=input as kg/day, 1=input a g/m3) [ichem]
	**cwpovrow,			//Overland point source chemical load row number
	**cwpovcol,			//Overland point source chemical load column number
	**ncwpovpairs,		//Number of pairs (load and tme) in a overland point source chemical load time series
	**cwpovpoint,		//pointer to current position for an overland point source chemical load function

	*ncwdov,			//Number of overland distributed chemical loads
	**ncwdovcells,		//Number of grid cells with an overland distributed chemical load [ichem][iswdov]
	***cwdovrow,		//Overland distributed chemical load row number [ichem][icwdov][icwdovcells[][]]
	***cwdovcol,		//Overland distributed chemical load column number [ichem][icwdov][icwdovcells[][]]
	**ncwdovpairs,		//Number of pairs (load and tme) in a overland distributed chemical load time series
	**cwdovpoint,		//pointer to current position for an overland distributed chemical load function

	*ncwch,				//Number of channel chemical loads
	**cwchopt,			//Channel chemical load option (0=input as kg/day, 1=input a g/m3) [ichem]
	**cwchlink,		//Channel chemical load link number
	**cwchnode,		//Channel chemical load node number
	**ncwchpairs,	//Number of pairs (load and tme) in a channel chemical load time series
	**cwchpoint,	//pointer to current position for a channel chemical load function

	**ncbcpairs,	//Number of solids chemicals condition pairs
	**cbcpoint,		//pointer to current position for a chemical boundary condition function

	*chemreprow,		//Row number for chemical report
	*chemrepcol,		//Column number for chemical report
	*chemunitsopt,		//Units option for chemical report

	ssropt;			//subsurface reporting option (0 = point concentration, 1= depth-averaged concentration)

extern int
	*partopt,		//chemical partitioning option [chem]
	*bioopt,		//chemical biodegradation option [chem]
	*hydopt,		//chemical hydrolysis option [chem]
	*oxiopt,		//chemical oxidation option [chem]
	*phtopt,		//chemical photolysis option [chem]
	*vltopt,		//chemical volatilization option [chem]
	*radopt,		//chemical radioactive decay option [chem]
	*udropt,		//chemical user-defined reaction option [chem]
	*dslopt;		//solid dissolution option [ichem]

extern float
	*kb,		//chemical doc binding coefficient [ichem]
	*kp,		//chemical solids partitioning coefficient [ichem]
	*koc,		//chemical organic carbon partitioning coefficient [ichem]
	*nux,		//particle interaction parameter for solids-dependent partitioning [ichem]
	*kbiowov,	//chemical first-order decay coefficient in overland water [ichem] (1/day)
	*kbiowch,	//chemical first-order decay coefficient in channel water [ichem] (1/day)
    *kbiosov,	//chemical first-order decay coefficient in overland soil [ichem] (1/day)
	*kbiosch,	//chemical first-order decay coefficient in channel bed sediment [ichem] (1/day)
	*kdslw,			//solid dissolution rate in water [ichem] (cm/s)
	*kdsls,			//solid dissolution rate in soil/sediment [ichem] (cm/s)
	*csolubility;	//solid solubility [ichem] (g/m3)

extern float
	*cyield,			//Chemical reaction yield (g/g) of product chemical from reacting chemical
	*chemarea,			//Area of watershed basin at a chemical reporting station (m3)
	**totchemreportov,	//Overland total chemical discharge at a chemical reporting station (g/s)
	**dischemreportov,	//Overland dissolved chemical discharge at a chemical reporting station (g/s)
	**bndchemreportov,	//Overland bound chemical discharge at a chemical reporting station (g/s)
	**prtchemreportov,	//Overland particulate chemical discharge at a chemical reporting station (g/s)
	**totchemreportch,	//Channel total chemical discharge at a chemical reporting station (g/s)
	**dischemreportch,	//Channel dissolved chemical discharge at a chemical reporting station (g/s)
	**bndchemreportch,	//Channel bound chemical discharge at a chemical reporting station (g/s)
	**prtchemreportch,	//Channel particulate chemical discharge at a chemical reporting station (g/s)
	*chemconvert,		//Chemical export time series reporting conversion factor

	horizon,			//horizon (depth below grade) for subsurface chemical concentration reporting (m)

	***cwpov,			//Value of an overland point source chemical load (kg/day) [ichem][iload][ipair]
	***cwpovtime,		//Time break for an overland point source chemical load (hours) [ichem][iload][ipair]

	****cwdovarea,		//Value for an overland distributed chemical load area [ichem][iload][row][col] (m2)
	***cwdov,			//Value of an overland distributed chemical load (g/m2/mm) [ichem][iload][ipair]
	***cwdovtime,		//Time break for an overland distributed chemical load (hours) [ichem][iload][ipair]

	***cwch,			//Value of a channel chemical load (kg/day) [ichem][iload][ipair]
	***cwchtime,		//Time break for a channel chemical load (hours) [ichem][iload][ipair]

	***cbc,				//Value of a channel chemical boundary condition (g/m3) [ioutlet][ichem][ipair]
	***cbctime,			//Time break for channel chemical boundary condition (hours) [ioutlet][ichem][ipair]

	*ntcwpov,		//next time to update overland point source chemical loads (hours) [ichem]
	*ntcwdov,		//next time to update overland distributed chemical loads (hours) [ichem]
	*ntcwch,		//next time to update channel chemical loads (hours) [ichem]
	*ntcbc,			//next time to update chemical boundary conditions (hours) [ichem]

	**ncwpovt,		//next (upper) interval time for overland point source chemical load time series
	**pcwpovt,		//prior (lower) interval time for overland point source chemical load time series
	**bcwpov,		//intercept for overland point source chemical load interpolation
	**mcwpov,		//slope for overland point source chemical load interpolation
	**cwpovinterp,	//temporally interpolated overland point source chemical load

	**ncwdovt,		//next (upper) interval time for overland distributed chemical load time series
	**pcwdovt,		//prior (lower) interval time for overland distributed chemical load time series
	**bcwdov,		//intercept for overland distributed chemical load interpolation
	**mcwdov,		//slope for overland distributed chemical load interpolation
	**cwdovinterp,	//temporally interpolated overland distributed chemical load

	**ncwcht,		//next (upper) interval time for channel chemical load time series
	**pcwcht,		//prior (lower) interval time for channel chemical load time series
	**bcwch,		//intercept for channel chemical load interpolation
	**mcwch,		//slope for channel chemical load interpolation
	**cwchinterp,	//temporally interpolated channel chemical load

	**ncbct,		//next (upper) interval time for chemical BC time series
	**pcbct,		//prior (lower) interval time for chemical BC time series
	**bcbc,			//intercept for chemical BC interpolation
	**mcbc,			//slope for chemical BC interpolation
	**cbcinterp;	//temporally interpolated chemical BC

extern float
	//advective flux terms
	****advchemovinflux,	//advected influx of chemical in overland plane [ichem][row][col][source]
	****advchemovoutflux,	//advected outflux of chemical in overland plane [ichem][row][col][source]

	****advchemchinflux,	//advected influx of chemical in channels [ichem][link][node][source]
	****advchemchoutflux,	//advected outflux of chemical in channels [ichem][link][node][source]

	//dispersive flux terms
	****dspchemovinflux,	//dispersed influx of chemical in overland plane [ichem][row][col][source]
	****dspchemovoutflux,	//dispersed outflux of chemical in overland plane [ichem][row][col][source]

	****dspchemchinflux,	//dispersed influx of chemical in channels [ichem][link][node][source]
	****dspchemchoutflux,	//dispersed outflux of chemical in channels [ichem][link][node][source]

	//deposition (settling) flux terms
	****depchemovinflux,	//deposited influx of chemical in overland plane [ichem][row][col][layer]
	****depchemovoutflux,	//deposited outflux of chemical in overland plane [ichem][row][col][layer]

	****depchemchinflux,	//deposited influx of chemical in channels [ichem][link][node][layer]
	****depchemchoutflux,	//deposited outflux of chemical in channels [ichem][link][node][layer]

	//erosion (resuspension) flux terms
	****erschemovinflux,	//eroded influx of chemical in overland plane [ichem][row][col][layer]
	****erschemovoutflux,	//eroded outflux of chemical in overland plane [ichem][row][col][layer]

	****erschemchinflux,	//eroded influx of chemical in channels [ichem][link][node][layer]
	****erschemchoutflux,	//eroded outflux of chemical in channels [ichem][link][node][layer]

	//porewater release flux terms (in porewater volume corresponding to sediment volume eroded)
	****pwrchemovinflux,	//porewater release influx of chemical in overland plane [ichem][row][col][layer]
	****pwrchemovoutflux,	//porewater release outflux of chemical in overland plane [ichem][row][col][layer]

	****pwrchemchinflux,	//porewater release influx of chemical in channels [ichem][link][node][layer]
	****pwrchemchoutflux,	//porewater release outflux of chemical in channels [ichem][link][node][layer]

	//chemical infiltration flux terms
	****infchemovinflux,	//infiltration influx of chemical in overland plane [ichem][row][col][layer]
	****infchemovoutflux,	//infiltration outflux of chemical in overland plane [ichem][row][col][layer]

	****infchemchinflux,	//infiltration influx of chemical in channels [ichem][link][node][layer]
	****infchemchoutflux,	//infiltration outflux of chemical in channels [ichem][link][node][layer]

	//chemical biodegredation flux terms
	****biochemovinflux,	//biodegredation influx of chemicals in overland plane [ichem][row][col][layer]
	****biochemovoutflux,	//biodegredation outflux of chemicals in overland plane [ichem][row][col][layer]

	****biochemchinflux,	//biodegredation influx of chemicals in channels [ichem][link][node][layer]
	****biochemchoutflux,	//biodegredation outflux of chemicals in channels [ichem][link][node][layer]

	//chemical hydrolysis flux terms
	****hydchemovinflux,	//hydrolysis influx of chemicals in overland plane [ichem][row][col][layer]
	****hydchemovoutflux,	//hydrolysis outflux of chemicals in overland plane [ichem][row][col][layer]

	****hydchemchinflux,	//hydrolysis influx of chemicals in channels [ichem][link][node][layer]
	****hydchemchoutflux,	//hydrolysis outflux of chemicals in channels [ichem][link][node][layer]

	//chemical oxidation flux terms
	****oxichemovinflux,	//oxidation influx of chemicals in overland plane [ichem][row][col][layer]
	****oxichemovoutflux,	//oxidation outflux of chemicals in overland plane [ichem][row][col][layer]

	****oxichemchinflux,	//oxidation influx of chemicals in channels [ichem][link][node][layer]
	****oxichemchoutflux,	//oxidation outflux of chemicals in channels [ichem][link][node][layer]

	//chemical photolysis flux terms
	****phtchemovinflux,	//photolysis influx of chemicals in overland plane [ichem][row][col][layer]
	****phtchemovoutflux,	//photolysis outflux of chemicals in overland plane [ichem][row][col][layer]

	****phtchemchinflux,	//photolysis influx of chemicals in channels [ichem][link][node][layer]
	****phtchemchoutflux,	//photolysis outflux of chemicals in channels [ichem][link][node][layer]

	//chemical radioactive decay flux terms
	****radchemovinflux,	//radioactive decay influx of chemicals in overland plane [ichem][row][col][layer]
	****radchemovoutflux,	//radioactive decay outflux of chemicals in overland plane [ichem][row][col][layer]

	****radchemchinflux,	//radioactive decay influx of chemicals in channels [ichem][link][node][layer]
	****radchemchoutflux,	//radioactive decay outflux of chemicals in channels [ichem][link][node][layer]

	//chemical volatilization flux terms
	****vltchemovinflux,	//volatilization influx of chemicals in overland plane [ichem][row][col][layer]
	****vltchemovoutflux,	//volatilization outflux of chemicals in overland plane [ichem][row][col][layer]

	****vltchemchinflux,	//volatilization influx of chemicals in channels [ichem][link][node][layer]
	****vltchemchoutflux,	//volatilization outflux of chemicals in channels [ichem][link][node][layer]

	//chemical user-defined reaction flux terms
	****udrchemovinflux,	//user-defined reaction influx of chemicals in overland plane [ichem][row][col][layer]
	****udrchemovoutflux,	//user-defined reaction outflux of chemicals in overland plane [ichem][row][col][layer]

	****udrchemchinflux,	//user-defined reaction influx of chemicals in channels [ichem][link][node][layer]
	****udrchemchoutflux,	//user-defined reaction outflux of chemicals in channels [ichem][link][node][layer]

	//dissolution flux terms
	****dslchemovinflux,	//dissoluted influx of solids in the overland plane [ichem][row][col][layer]
	****dslchemchinflux,	//dissoluted influx of solids in channels [ichem][link][node][layer]

	//chemical peak discharge terms
	**chemflowpeakov,		//Peak overland chemical discharge at an outlet (kg/s) [solid][outlet]
	**chemtimepeakov,		//Time of peak overland chemical discharge at an outlet (hrs) [solid][outlet]
	**chemflowpeakch,		//Peak channel chemical discharge at an outlet (kg/s) [solid][outlet]
	**chemtimepeakch;		//Time of peak channel chemical discharge at an outlet (hrs) [solid][outlet]

extern double
	//advective mass terms
	****advchemovinmass,	//advected inflowing mass of chemical in overland plane [ichem][row][col][source]
	****advchemovoutmass,	//advected outflowing mass of chemical in overland plane [ichem][row][col][source]

	****advchemchinmass,	//advected inflowing mass of chemical in channels [ichem][link][node][source]
	****advchemchoutmass,	//advected outflowing mass of chemical in channels [ichem][link][node][source]

	//dispersive mass terms
	****dspchemovinmass,	//dispersed inflowing mass of chemical in overland plane [ichem][row][col][source]
	****dspchemovoutmass,	//dispersed outflowing mass of chemical in overland plane [ichem][row][col][source]

	****dspchemchinmass,	//dispersed inflowing mass of chemical in channels [ichem][link][node][source]
	****dspchemchoutmass,	//dispersed outflowing mass of chemical in channels [ichem][link][node][source]

	//deposition (settling) mass terms
	****depchemovinmass,	//deposited inflowing mass of chemical in overland plane [ichem][row][col][layer]
	****depchemovoutmass,	//deposited outflowing mass of chemical in overland plane [ichem][row][col][layer]

	****depchemchinmass,	//deposited inflowing mass of chemical in channels [ichem][link][node][layer]
	****depchemchoutmass,	//deposited outflowing mass of chemical in channels [ichem][link][node][layer]

	//erosion (resuspension) mass terms
	****erschemovinmass,	//eroded inflowing mass of chemical in overland plane [ichem][row][col][layer]
	****erschemovoutmass,	//eroded outflowing mass of chemical in overland plane [ichem][row][col][layer]

	****erschemchinmass,	//eroded inflowing mass of chemical in channels [ichem][link][node][layer]
	****erschemchoutmass,	//eroded outflowing mass of chemical in channels [ichem][link][node][layer]

	//porewater release mass terms (in porewater volume corresponding to sediment volume eroded)
	****pwrchemovinmass,	//porewater release inflowing mass of chemical in overland plane [ichem][row][col][layer]
	****pwrchemovoutmass,	//porewater release outflowing mass of chemical in overland plane [ichem][row][col][layer]

	****pwrchemchinmass,	//porewater release inflowing mass of chemical in channels [ichem][link][node][layer]
	****pwrchemchoutmass,	//porewater release outflowing mass of chemical in channels [ichem][link][node][layer]

	//burial mass terms
	****burchemovinmass,	//buried inflowing mass of chemicals in the overland plane soil stack [ichem][row][col][layer]
	****burchemovoutmass,	//buried outflowing mass of chemicals in the overland plane soil stack [ichem][row][col][layer]

	****burchemchinmass,	//buried inflowing mass of chemicals in the channel sediment stack [ichem][row][col][layer]
	****burchemchoutmass,	//buried outflowing mass of chemicals in the channel sediment stack [ichem][row][col][layer]

	//scour (unburial) mass terms
	****scrchemovinmass,	//scoured inflowing mass of chemicals in the overland plane soil stack [ichem][row][col][layer]
	****scrchemovoutmass,	//scoured outflowing mass of chemicals in the overland plane soil stack [ichem][row][col][layer]

	****scrchemchinmass,	//scoured inflowing mass of chemicals in the channel sediment stack [ichem][link][node][layer]
	****scrchemchoutmass,	//scoured outflowing mass of chemicals in the channel sediment stack [ichem][link][node][layer]

	//chemical infiltration mass terms
	****infchemovinmass,	//infiltrated inflowing mass of chemical in overland plane [ichem][row][col][layer]
	****infchemovoutmass,	//infiltrated outflowing mass of chemical in overland plane [ichem][row][col][layer]

	****infchemchinmass,	//infiltrated inflowing mass of chemical in channels [ichem][link][node][layer]
	****infchemchoutmass,	//infiltrated inflowing mass of chemical in channels [ichem][link][node][layer]

	//chemical biodegredation flux terms
	****biochemovinmass,	//biodegredation inflowing mass of chemicals in overland plane [ichem][row][col][layer]
	****biochemovoutmass,	//biodegredation outflowing mass of chemicals in overland plane [ichem][row][col][layer]

	****biochemchinmass,	//biodegredation inflowing mass of chemicals in channels [ichem][link][node][layer]
	****biochemchoutmass,	//biodegredation outflowing mass of chemicals in channels [ichem][link][node][layer]

	//chemical hydrolysis mass terms
	****hydchemovinmass,	//hydrolysis inflowing mass of chemicals in overland plane [ichem][row][col][layer]
	****hydchemovoutmass,	//hydrolysis outflowing mass of chemicals in overland plane [ichem][row][col][layer]

	****hydchemchinmass,	//hydrolysis inflowing mass of chemicals in channels [ichem][link][node][layer]
	****hydchemchoutmass,	//hydrolysis outflowing mass of chemicals in channels [ichem][link][node][layer]

	//chemical oxidation mass terms
	****oxichemovinmass,	//oxidation inflowing mass of chemicals in overland plane [ichem][row][col][layer]
	****oxichemovoutmass,	//oxidation outflowing mass of chemicals in overland plane [ichem][row][col][layer]

	****oxichemchinmass,	//oxidation inflowing mass of chemicals in channels [ichem][link][node][layer]
	****oxichemchoutmass,	//oxidation outflowing mass of chemicals in channels [ichem][link][node][layer]

	//chemical photolysis mass terms
	****phtchemovinmass,	//photolysis inflowing mass of chemicals in overland plane [ichem][row][col][layer]
	****phtchemovoutmass,	//photolysis outflowing mass of chemicals in overland plane [ichem][row][col][layer]

	****phtchemchinmass,	//photolysis inflowing mass of chemicals in channels [ichem][link][node][layer]
	****phtchemchoutmass,	//photolysis outflowing mass of chemicals in channels [ichem][link][node][layer]

	//chemical radioactive decay mass terms
	****radchemovinmass,	//radioactive decay inflowing mass of chemicals in overland plane [ichem][row][col][layer]
	****radchemovoutmass,	//radioactive decay outflowing mass of chemicals in overland plane [ichem][row][col][layer]

	****radchemchinmass,	//radioactive decay inflowing mass of chemicals in channels [ichem][link][node][layer]
	****radchemchoutmass,	//radioactive decay outflowing mass of chemicals in channels [ichem][link][node][layer]

	//chemical volatilization mass terms
	****vltchemovinmass,	//volatilization inflowing mass of chemicals in overland plane [ichem][row][col][layer]
	****vltchemovoutmass,	//volatilization outflowing mass of chemicals in overland plane [ichem][row][col][layer]

	****vltchemchinmass,	//volatilization inflowing mass of chemicals in channels [ichem][link][node][layer]
	****vltchemchoutmass,	//volatilization outflowing mass of chemicals in channels [ichem][link][node][layer]

	//chemical user-defined reaction mass terms
	****udrchemovinmass,	//user-defined reaction inflowing mass of chemicals in overland plane [ichem][row][col][layer]
	****udrchemovoutmass,	//user-defined reaction outflowing mass of chemicals in overland plane [ichem][row][col][layer]

	****udrchemchinmass,	//user-defined reaction inflowing mass of chemicals in channels [ichem][link][node][layer]
	****udrchemchoutmass,	//user-defined reaction outflowing mass of chemicals in channels [ichem][link][node][layer]

	//dissolution mass terms
	****dslchemovinmass,	//dissoluted inflowing mass of solids in the overland plane [ichem][row][col][layer]
	****dslchemchinmass,	//dissoluted inflowing mass of solids in channels [ichem][link][node][layer]

	//mass balance parameters
	****initialchemov,		//initial chemical mass in a overland plane cell (kg)
	****initialchemch,		//initial chemical mass in a channel node (kg)

	**initialchemmassov,	//cumulative initial chemical mass in the overland plane (kg) [ichem][layer]
	**initialchemmassch,	//cumulative initial chemical mass in the channel network (kg) [ichem][layer]

	****finalchemov,		//final chemical mass in a overland plane cell (kg) [ichem][row][col][layer]
	****finalchemch,		//final chemical mass in a channel node (kg) [ichem][link][node][layer]

	**finalchemmassov,		//cumulative final chemical mass in the overland plane (kg) [ichem][layer]
	**finalchemmassch,		//cumulative final chemical mass in the channel network (kg) [ichem][layer]

	**totaladvcheminov,		//cumulative sum of chemical mass entering water column by overland advection (kg) [ichem][outlet]
	**totaldspcheminov,		//cumulative sum of chemical mass entering water column by overland dispersion (kg) [ichem][outlet]
	*totalerscheminov,		//cumulative sum of chemical mass entering water column by overland erosion (kg) [ichem]
	*totalpwrcheminov,		//cumulative sum of chemical mass entering water column by overland porewater release (kg) [ichem]
	*totaltnscheminov,		//cumulative sum of chemical mass entering water column by overland kinetics (kg) [ichem]

	**totaladvchemoutov,	//cumulative sum of chemical mass exiting water column by overland advection (kg) [ichem][outlet]
	**totaldspchemoutov,	//cumulative sum of chemical mass exiting water column by overland dispersion (kg) [ichem][outlet]
	*totaldepchemoutov,		//cumulative sum of chemical mass exiting water column by overland deposition (kg) [ichem]
	**totalinfchemoutov,	//cumulative sum of chemical mass exiting water column or surface soil by overland infiltration (kg) [ichem][0,1]
	*totaltnschemoutov,		//cumulative sum of chemical mass exiting water column by overland chemical kinetics (kg) [ichem]

	**totaladvcheminch,		//cumulative sum of chemical mass entering water column by channel advection (kg)
	**totaldspcheminch,		//cumulative sum of chemical mass entering water column by channel dispersion (kg)
	*totalerscheminch,		//cumulative sum of chemical mass entering water column by channel erosion (kg)
	*totalpwrcheminch,		//cumulative sum of chemical mass entering water column by channel porewater release (kg)
	*totaltnscheminch,		//cumulative sum of chemical mass entering water column by channel chemical kinetics (kg)

	**totaladvchemoutch,	//cumulative sum of chemical mass exiting water column by channel advection (kg)
	**totaldspchemoutch,	//cumulative sum of chemical mass exiting water column by channel dispersion (kg)
	*totaldepchemoutch,		//cumulative sum of chemical mass exiting water column by channel deposition (kg)
	**totalinfchemoutch,	//cumulative sum of chemical mass exiting water column or surface bed by channel infiltration (transmission loss) (kg) [ichem][0,1]
	*totaltnschemoutch,		//cumulative sum of chemical mass exiting water column by channel chemical kinetics (kg)

	*totalcwov,				//cumulative sum of external chemical loads entering overland plane (m3)
	*totalcwch;				//cumulative sum of external chemical loads entering channel network (m3)

extern float
	****fdissolvedov,		//fraction of total chemical in dissolved phase overland
	****fboundov,			//fraction of total chemical bound to doc overland
	*****fparticulateov,	//fraction of total chemical sorbed to particulate phase overland

	****fdissolvedch,		//fraction of total chemical in dissolved phase in channels
	****fboundch,			//fraction of total chemical bound to doc in channels
	*****fparticulatech;	//fraction of total chemical sorbed to particulate phase in channels

extern double
	*maxcchemov0,	//maximum chemical concentration in water column of overland plane
	*mincchemov0,	//minimum chemical concentration in water column of overland plane
	*maxcchemov1,	//maximum chemical concentration in surface sediment of overland plane
	*mincchemov1,	//minimum chemical concentration in surface sediment of overland plane

	*maxcchemch0,	//maximum chemical concentration in water column of channel network
	*mincchemch0,	//minimum chemical concentration in water column of channel network
	*maxcchemch1,	//maximum chemical concentration in surface sediment of channel network
	*mincchemch1;	//minimum chemical concentration in surface sediment of channel network
