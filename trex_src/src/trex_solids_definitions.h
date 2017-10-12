/*----------------------------------------------------------------------
C-  Header:     trex_solids_definitions.h
C-
C-	Purpose/	Global definitions header file for sediment transport
C-	Methods:	calculations.  Used with trex_solids_declarations.h.
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

/*************************/
/*   FILE DECLARATIONS   */
/*************************/

//Data Group C
FILE *soilstackfile_fp;				//soil stack file pointer
FILE *soillayerthicknessfile_fp;	//soil layer thickness file pointer
FILE *soillayersolidsgsdfile_fp;	//soil layer solids grain size distribution file pointer
FILE *initialssovfile_fp;			//initial suspended solids overland file pointer
FILE *sedimentpropertiesfile_fp;	//sediment properties file pointer
FILE *initialsschfile_fp;			//initial suspended solids channel file pointer
FILE *swdovareafile_fp;				//overland distributed solids load area file pointer

//Data Group F
FILE *sedexpfile_fp;				//sediment export file pointer
FILE *solidsconcwatergrid_fp;		//solids water column concentration grid file pointer
FILE *solidsconcsurfgrid_fp;		//solids surface layer concentration grid file pointer

/************************************/
/*   GLOBAL VARIABLE DEFINITIONS    */
/************************************/


	/**********************/
	//  state variables
	/**********************/
float
	****csedov,			//solids concentration in overland plane grid cell (at current time t) (g/m3) [isolid][r][c][layer]
	****csedovnew,		//new solids concentration in overland plane grid cell (for next time t+dt) (g/m3) [isolid][r][c][layer]
	****csedch,			//solids concentration in channel node (at current time t) (g/m3) [isolid][r][c][layer]
	****csedchnew;		//new solids concentration in channel node (for next time t+dt) (g/m3) [isolid][r][c][layer]


	/*************************************************************/
	//  solids/sediment (ksim = 2) controls, support, reporting
	/*************************************************************/
char
	loadname[MAXNAMESIZE],		//load name
	bcname[MAXNAMESIZE],		//boundary condition name
	*soilstackfile,				//soil stack file name
	*soillayerthicknessfile,	//soil layer thickness file name
	*soillayersolidsgsdfile,	//soil layer solids grain size distribution file name
	*initialssovfile,			//initial suspended solids overland file name
	*sedimentpropertiesfile,	//sediment properties file
	*initialsschfile,			//initial suspended solids channel file name
	*sedexpfileroot,			//sediment export root file name
	*sedextension,				//sediment export file name extension
	**sedexpfile,				//sediment export file [nsgroups]
	**sgroupname,				//name of solids reporting group [nsgroups]
	**particlename,				//Particle name [nsolids]

	*swdovareafile,				//overland solids distributed load area file name

	*solidsconcwatergridroot,	//solids water column concentration grid root name
	**solidsconcwatergrid,		//solids water column concentration grid file name [nsgroups]
	*solidsconcsurfgridroot,	//solids surface layer concentration grid root name
	**solidsconcsurfgrid,		//solids surface layer concentration grid file name [nsgroups]

	*netelevationgrid,			//solids net elevation grid name (total solids)

	*solidserosiongridroot,		//solids gross erosion grid root name
	**solidserosiongrid,		//solids gross erosion grid file name [nsgroups]
	*solidsdepositiongridroot,	//solids gross deposition grid root name
	**solidsdepositiongrid,		//solids gross deposition grid file name [nsgroups]
	*solidsnetaccumgridroot,	//solids gross netaccum grid root name
	**solidsnetaccumgrid;		//solids gross netaccum grid file name [nsgroups]

int
	nsolids,		//Number of solid (particle) types
	nsgroups,		//Number of Solids Reporting Groups
	nsyields,		//Number of solids reaction yields
	*syldfrom,		//Reacting solids number
	*syldto,		//Product solids number
	*syldprocess,	//Process number for solids reaction
	nsedreports,	//Number of Sediment reports

	advovopt,		//overland advection option
	dspovopt,		//overland dispersion option
	depovopt,		//overland deposition option
	ersovopt,		//overland erosion option
	elevovopt,		//overland elevation update option
	tnsovopt,		//overland solids transformation (reaction) option

	advchopt,		//channel advection option
	dspchopt,		//channel dispersion option
	depchopt,		//channel deposition option
	erschopt,		//channel erosion option
	elevchopt,		//channel elevation update option
	tnschopt,		//channel solids transformation (reaction) option

	stkovopt,		//stack collapse option for the overland soil stack
	stkchopt,		//stack collapse option for the channel sediment stack

	//Note:  Declarations/definitions for maxstackov and maxstackch were moved to
	//       trex_water_declarations/definitions to allow initialization of
	//       environmental properties when ksim = 1
	//
	//maxstackov,		//maximum number of layers in an overland soil stack column
	//maxstackch,		//maximum number of layers in a channel sediment stack column

	*sgroupnumber,		//Solids reporting group number [isolid]
	*cncopt,			//cohesive/non-cohesive option [isolid]

	*sedreprow,			//Row number for sediment report [ireport]
	*sedrepcol,			//Column number for sediment report [ireport]
	*sedunitsopt,		//Units option for Sediment report [ireport]

	**nstackov0,		//initial number of layers in the overland soil stack [row][col]
//mlv	**nstackov,			//present number of layers in the overland soil stack [row][col]

	**nstackch0,		//initial number of layers in the channel sediment stack [link][node]
//mlv	**nstackch,			//present number of layers in the channel sediment stack [link][node]

	*nswpov,			//Number of overland point source solids loads [isolid]
	**swpovopt,			//Overland point source solids load option (0=input as kg/day, 1=input a g/m3) [isolid]
	**swpovrow,			//Overland solids load row number [isolid][iswpov]
	**swpovcol,			//Overland solids load column number [isolid][iswpov]
	**nswpovpairs,		//Number of pairs (load and time) in a overland point source solids load time series [isolid][iswpov]
	**swpovpoint,		//pointer to current position for an overland point source solids load function [isolid][iswpov]

	*nswdov,			//Number of overland distributed solids loads [isolid]
	**nswdovcells,		//Number of grid cells with an overland distributed solids load [isolid][iswdov]
	***swdovrow,		//Overland distributed solids load row number [isolid][iswdov][iswdovcells[][]]
	***swdovcol,		//Overland distributed solids load column number [isolid][iswdov][iswdovcells[][]]
	**nswdovpairs,		//Number of pairs (load and time) in a overland distributed solids load time series [isolid][iswdov]
	**swdovpoint,		//pointer to current position for an overland distributed solids load function [isolid][iswdov]

	*nswch,				//Number of channel point source solids loads [isolid]
	**swchopt,			//Channel point source solids load option (0=input as kg/day, 1=input a g/m3) [isolid]
	**swchlink,			//Channel link for point source solids loads [isolid][iswch]
	**swchnode,			//Channel node for point source solids loads [isolid][iswch]
	**nswchpairs,		//Number of pairs of channel point source solids loads and time [isolid][iswch]
	**swchpoint,		//pointer to current position for a channel point source solids load function

	**nsbcpairs,		//Number of solids boundary condition pairs [ioutlet][isolid]
	**sbcpoint,			//pointer to current position for a solids boundary condition function [ioutlet][isolid]

	popovflag,			//flag to signal if any location in the overland soil stack needs upward re-indexing (unburial)
	pushovflag,			//flag to signal if any location in the overland soil stack needs downward re-indexing (burial)

	popchflag,			//flag to signal if any location in the channel sediment stack needs upward re-indexing (unburial)
	pushchflag;			//flag to signal if any location in the channel sediment stack needs downward re-indexing (burial)

int
	*abropt,			//solids abrasion option
	*mnropt;			//solids mineralization option

/*
//mlv: place solids reaction float parameters here...

float
*/

float
	advovscale,		//overland advection scale factor
	dspovscale,		//overland dispersion scale factor
	depovscale,		//overland deposition scale factor
	ersovscale,		//overland erosion scale factor
	tnsovscale,		//overland solids transformations scale factor

	advchscale,		//channel advection scale factor
	dspchscale,		//channel dispersion scale factor
	depchscale,		//channel deposition scale factor
	erschscale,		//channel erosion scale factor
	tnschscale,		//channel solids transformations scale factor

	*ds,				//particle mean diameter (m) [isolid]
	*ws,				//particle setling velocity (omega) (m/s) [isolid]
	*spgravity,			//particle specific gravity (dimensionless) [isolid]
	*dstar,				//particle dimensionless diameter (dimensionless) [isolid]

	*syield,			//solids reaction yield (g/g) [isolid]

	*kusle,				//K factor in USLE (Universal Soil Loss Equation) [isoil]
	*cusle,				//C factor in USLE (Universal Soil Loss Equation) [iland]
	*pusle,				//P factor in USLE (Universal Soil Loss Equation) [iland]
	*vcov,				//critical velocity for erosion overland (m/s) [isoil]

	*porosityov,		//overland soil porosity (dimensionless) [isoil]

	*tcdov,				//critical shear stress for deposition overland (pdep) (N/m2) [isolid]
	*vcch,				//critical velocity for erosion in channels (m/s) [isolid]
	*tceov,				//critical shear stress for erosion overland (excess shear) (N/m2) [isolid]
	*zageov,			//overland sediment aging factor (excess shear)  [isolid]
	*mexpov,			//overland sediment erosion intensity exponent (excess shear) (dimensionless) [isoil]
	*ayov,				//overland sediment erosion yield coefficient (excess shear) (g/cm2) [iland]

	*tcdch,				//critical shear stress for deposition in channels (pdep) (N/m2) [isolid]
	*tcech,				//critical shear stress for erosion in channels (excess shear) (N/m2) [isolid]
	*zagech,			//channel sediment aging factor (excess shear)  [isolid]
	**mexpch,			//channel sediment erosion intensity exponent (excess shear) (dimensionless) [link][node]
	**aych,				//channel sediment erosion yield coefficient (excess shear) (g/cm2) [link][node]

	***porositych,		//channel sediment porosity (dimensionless) [link][node][ilayer]

	//soil and sediment stack control parameters
	minvolov,		//minimum volume limit factor for layers in the overland soil stack
	maxvolov,		//maximum volume limit factor for layers in the overland soil stack

	minvolch,		//minimum volume limit factor for layers in the channel sediment stack
	maxvolch,		//maximum volume limit factor for layers in the channel sediment stack

	//reporting stations
	*sedarea,			//Area of a watershed basin at a reporting station for sediment report (m3) [nsedreports]
	**sedreportov,		//Overland sediment discharge at a sediment reporting station (m3/s) [nsolids][nsedreports]
	**sedreportch,		//Channel sediment discharge at a sediment reporting station (m3/s) [nsolids][nsedreports]
	*sedconvert,		//Sediment export time series reporting conversion factor [nsedreports]

	//forcing functions and boundary conditions
	***swpov,			//Value for an overland point source solids load [isolid][iload][ipair]
	***swpovtime,		//Time break for an overland point source solids load [isolid][iload][ipair]

	****swdovarea,		//Value for an overland distributed solids load area [isolid][iload][row][col] (m2)
	***swdov,			//Value for an overland distributed solids load (g/m2/mm) [isolid][iload][ipair]
	***swdovtime,		//Time break for an overland distributed solids load [isolid][iload][ipair]

	***swch,			//Value for a channel point source solids load [isolid][iload][ipair]
	***swchtime,		//Time break for a channel point source solid load [isolid][iload][ipair]

	***sbc,				//Value for a solids boundary condition [ioutlet][isolid][ipair]
	***sbctime,			//Time break for a solids boundary condition [ioutlet][isolid][ipair]

	*ntswpov,		//next time to update overland point source solids loads [isolid]
	*ntswdov,		//next time to update overland distributed solids loads [isolid]
	*ntswch,		//next time to update channel solids loads [isolid]
	*ntsbc,			//next time to update solids boundary conditions [isolid]

	**nswpovt,		//next (upper) interval time for overland point source solids load time series [isolid][iload]
	**pswpovt,		//prior (lower) interval time for overland point source solids load time series [isolid][iload]
	**bswpov,		//intercept for overland point source solids load interpolation [isolid][iload]
	**mswpov,		//slope for overland point source solids load interpolation [isolid][iload]
	**swpovinterp,	//temporally interpolated overland point source solids load [isolid][iload]

	**nswdovt,		//next (upper) interval time for overland distributed solids load time series [isolid][iload]
	**pswdovt,		//prior (lower) interval time for overland distributed solids load time series [isolid][iload]
	**bswdov,		//intercept for overland distributed solids load interpolation [isolid][iload]
	**mswdov,		//slope for overland distributed solids load interpolation [isolid][iload]
	**swdovinterp,	//temporally interpolated overland distributed solids load [isolid][iload]

	**nswcht,		//next (upper) interval time for channel sediment load time series [isolid][iload]
	**pswcht,		//prior (lower) interval time for channel sediment load time series [isolid][iload]
	**bswch,		//intercept for channel sediment load interpolation [isolid][iload]
	**mswch,		//slope for channel sediment load interpolation [isolid][iload]
	**swchinterp,	//temporally interpolated channel sediment load [isolid][iload]

	**nsbct,		//next (upper) interval time for sediment BC time series [ioutlet][isolid]
	**psbct,		//prior (lower) interval time for sediment BC time series
	**bsbc,			//intercept for sediment BC interpolation
	**msbc,			//slope for sediment BC interpolation
	**sbcinterp,	//temporally interpolated sediment BC

	//stack parameters
	***hlayerov0,		//initial thickness of an overland soil layer (m) [row][col][layer]
	***hlayerov,		//present thickness of an overland soil layer (m) [row][col][layer]

	***hlayerch0,		//initial thickness of a channel sediment layer (m) [link][node][layer]
	***hlayerch,		//present thickness of a channel sediment layer (m) [link][node][layer]

	***bwlayerch0,		//initial bottom width of a channel sediment layer (m) [link][node][layer]
	***bwlayerch,		//present bottom width of a channel sediment layer (m) [link][node][layer]

	**aovground,		//ground surface area of an overland soil layer (m2) (constant for all layers) [row][col]

	***achbed0,			//initial bed surface area of a channel sediment layer (m2) [link][node][layer]
	***achbed,			//present bed surface area of a channel sediment layer (m2) [link][node][layer]

	***minvlayerov,		//minimum volume of an overland soil layer (m3) [row][col][layer]
	***maxvlayerov,		//maximum volume of an overland soil layer (m3) [row][col][layer]

	***minvlayerch,		//minimum volume of a channel sediment layer (m) [link][node][layer]
	***maxvlayerch,		//maximum volume of a channel sediment layer (m) [link][node][layer]

	***elevlayerov,		//elevation of an overland soil layer (m) [row][col][layer]

	***elevlayerch,		//elevation of a channel sediment layer (m) [link][node][layer]

	**gsdov,			//Overland Grain Size Distribution [isoil][isolid]
	****gsdch,			//Channel Grain Size Distribution [isolid][link][node][ilayer]

	//transport process flow terms
	***advinflowov,		//inflow by advection in the overland plane (m3/s) [row][col][direction]
	***advoutflowov,	//outflow by advection in the overland plane (m3/s) [row][col][direction]
	***advinflowch,		//inflow by advection in the channel network (m3/s) [link][node][direction]
	***advoutflowch,	//outflow by advection in the channel network (m3/s) [link][node][direction]

	***dspflowov,	//flow by dispersion in the overland plane (m3/s) [row][col][direction]
	***dspflowch,	//flow by dispersion in the channel network (m3/s) [link][node][direction]

	***depflowov,	//flow by deposition in the overland plane (m3/s) [isolid][row][col]
	***depflowch,	//flow by deposition in the channel network (m3/s) [isolid][link][node]

	***ersflowov,	//flow by erosion in the overland plane (m3/s) [isolid][row][col]
	***ersflowch,	//flow by erosion in the channel network (m3/s) [isolid][link][node]

    ***transrateov,	//overland solids transport rate [isolid][row][col]
    ***transratech,		//channel solids transport rate [isolid][link][node]

    ***transcapov,	//overland solids transport capacity [isolid][row][col]
    ***transcapch,		//channel solids transport capacity [isolid][link][node]

	**taumaxov,			//maximum shear stress overland (shear stress memory, lasts for up to 30 days) (N/m2) [link][node]
	**taumaxtimeov,		//time of maximum shear stress overland (hours) [link][node]

	**taumaxch,			//maximum shear stress in channels (shear stress memory, lasts for up to 30 days) (N/m2) [link][node]
	**taumaxtimech,		//time of maximum shear stress in channels (hours) [link][node]

	//advective flux terms
	****advsedovinflux,		//advected influx of sediment in overland plane [isolid][row][col][source]
	****advsedovoutflux,	//advected outflux of sediment in overland plane [isolid][row][col][source]

	****advsedchinflux,		//advected influx of sediment in channels [isolid][link][node][source]
	****advsedchoutflux,	//advected outflux of sediment in channels [isolid][link][node][source]

	//dispersive flux terms
	****dspsedovinflux,		//dispersed influx of sediment in overland plane [isolid][row][col][layer][source]
	****dspsedovoutflux,	//dispersed outflux of sediment in overland plane [isolid][row][col][layer][source]

	****dspsedchinflux,		//dispersed influx of sediment in channels [isolid][link][node][source]
	****dspsedchoutflux,	//dispersed outflux of sediment in channels [isolid][link][node][source]

	//deposition (settling) flux terms
	****depsedovinflux,		//deposited influx of sediment in overland plane [isolid][row][col][layer]
	****depsedovoutflux,	//deposited outflux of sediment in overland plane [isolid][row][col][layer]

	****depsedchinflux,		//deposited influx of sediment in channels [isolid][link][node][layer]
	****depsedchoutflux,	//deposited outflux of sediment in channels [isolid][link][node][layer]

	//erosion (resuspension) flux terms
	****erssedovinflux,		//eroded influx of sediment in the overland plane [isolid][row][col][layer]
	****erssedovoutflux,	//eroded outflux of sediment in the overland plane [isolid][row][col][layer]

	****erssedchinflux,		//eroded influx of sediment in channels [isolid][link][node][layer]
	****erssedchoutflux,	//eroded outflux of sediment in channels [isolid][link][node][layer]

	//dissolution flux terms
	****dslsedovoutflux,	//dissoluted outflux of solids in the overland plane [isolid][row][col][layer]
	****dslsedchoutflux,	//dissoluted outflux of solids in channels [isolid][link][node][layer]

	**sedflowpeakov,		//Peak overland solids discharge at an outlet (kg/s) [solid][outlet]
	**sedtimepeakov,		//Time of peak overland solids discharge at an outlet (hrs) [solid][outlet]
	**sedflowpeakch,		//Peak channel solids discharge at an outlet (kg/s) [solid][outlet]
	**sedtimepeakch;		//Time of peak channel solids discharge at an outlet (hrs) [solid][outlet]

double
	//advective mass terms
	****advsedovinmass,		//advected inflowing mass of sediment in overland plane [isolid][row][col][source]
	****advsedovoutmass,	//advected outflowing mass of sediment in overland plane [isolid][row][col][source]

	****advsedchinmass,		//advected inflowing mass of sediment in channels [isolid][link][node][source]
	****advsedchoutmass,	//advected outflowing mass of sediment in channels [isolid][link][node][source]

	//dispersive mass terms
	****dspsedovinmass,		//dispersed inflowing mass of sediment in overland plane [isolid][row][col][layer][source]
	****dspsedovoutmass,	//dispersed outflowing mass of sediment in overland plane [isolid][row][col][layer][source]

	****dspsedchinmass,		//dispersed inflowing mass of sediment in channels [isolid][link][node][source]
	****dspsedchoutmass,	//dispersed outflowing mass of sediment in channels [isolid][link][node][source]

	//deposition (settling) mass terms
	****depsedovinmass,		//deposited inflowing mass of sediment in overland plane [isolid][row][col][layer]
	****depsedovoutmass,	//deposited outflowing mass of sediment in overland plane [isolid][row][col][layer]

	****depsedchinmass,		//deposited inflowing mass of sediment in channels [isolid][link][node][layer]
	****depsedchoutmass,	//deposited outflowing mass of sediment in channels [isolid][link][node][layer]

	//erosion (resuspension) mass terms
	****erssedovinmass,		//eroded inflowing mass of sediment in the overland plane [isolid][row][col][layer]
	****erssedovoutmass,	//eroded outflowing mass of sediment in the overland plane [isolid][row][col][layer]

	****erssedchinmass,		//eroded inflowing mass of sediment in channels [isolid][link][node][layer]
	****erssedchoutmass,	//eroded outflowing mass of sediment in channels [isolid][link][node][layer]

	//dissolution mass terms
	****dslsedovoutmass,	//dissoluted outflowing mass of solids in the overland plane [isolid][row][col][layer]
	****dslsedchoutmass,	//dissoluted outflowing mass of solids in channels [isolid][link][node][layer]

	//burial mass terms
	****bursedovinmass,		//buried inflowing mass of solids in the overland plane soil stack [isolid][row][col][layer]
	****bursedovoutmass,	//buried outflowing mass of solids in the overland plane soil stack [isolid][row][col][layer]

	****bursedchinmass,		//buried inflowing mass of solids in the channel sediment stack [isolid][link][node][layer]
	****bursedchoutmass,	//buried outflowing mass of solids in the channel sediment stack [isolid][link][node][layer]

	//scour (unburial) mass terms
	****scrsedovinmass,		//scoured inflowing mass of solids in the overland plane soil stack [isolid][row][col][layer]
	****scrsedovoutmass,	//scoured outflowing mass of solids in the overland plane soil stack [isolid][row][col][layer]

	****scrsedchinmass,		//scoured inflowing mass of solids in the channel sediment stack [isolid][link][node][layer]
	****scrsedchoutmass,	//scoured outflowing mass of solids in the channel sediment stack [isolid][link][node][layer]

	//mass balance parameters
	****initialsolidsov,	//initial solids mass in a overland plane cell (kg)
	****initialsolidsch,	//initial solids mass in a channel node (kg)
	
	**initialsolidsmassov,	//cumulative initial solids mass in the overland plane (kg) [solid][layer]
	**initialsolidsmassch,	//cumulative initial solids mass in the channel network (kg) [solid][layer]
	
	****finalsolidsov,		//final solids mass in a overland plane cell (kg) [solid][row][col][layer]
	****finalsolidsch,		//final solids mass in a channel node (kg) [solid][link][node][layer]

	**finalsolidsmassov,	//cumulative final solids mass in the overland plane (kg) [solid][layer]
	**finalsolidsmassch,	//cumulative final solids mass in the channel network (kg) [solid][layer]
	
	**totaladvsedinov,		//cumulative sum of solids mass entering water column by overland advection (kg) [solid][outlet]
	**totaldspsedinov,		//cumulative sum of solids mass entering water column by overland dispersion (kg) [solid][outlet]
	*totalerssedinov,		//cumulative sum of solids mass entering water column by overland erosion (kg) [solid]

	**totaladvsedoutov,		//cumulative sum of solids mass exiting water column by overland advection (kg) [solid][outlet]
	**totaldspsedoutov,		//cumulative sum of solids mass exiting water column by overland dispersion (kg) [solid][outlet]
	*totaldepsedoutov,		//cumulative sum of solids mass exiting water column by overland deposition (kg) [solid]

	**totaladvsedinch,		//cumulative sum of solids mass entering water column by channel advection (kg)
	**totaldspsedinch,		//cumulative sum of solids mass entering water column by channel dispersion (kg)
	*totalerssedinch,		//cumulative sum of solids mass entering water column by channel erosion (kg)

	**totaladvsedoutch,		//cumulative sum of solids mass exiting water column by channel advection (kg)
	**totaldspsedoutch,		//cumulative sum of solids mass exiting water column by channel dispersion (kg)
	*totaldepsedoutch,		//cumulative sum of solids mass exiting water column by channel deposition (kg)

	*totalswov,				//cumulative sum of external solids loads entering overland plane (m3)
	*totalswch;				//cumulative sum of external solids loads entering channel network (m3)

double
	***vlayerov0,		//initial volume of an overland soil layer (m3) [row][col][layer]
	***vlayerov,		//present volume of an overland soil layer (m3) [row][col][layer]
	***vlayerovnew,		//volume of an overland soil layer for next time t + dt (m3) [row][col][layer]

	***vlayerch0,		//initial volume of a channel sediment layer (m) [link][node][layer]
	***vlayerch,		//present volume of a channel sediment layer (m) [link][node][layer]
	***vlayerchnew;		//volume of a channel sediment layer for next time t + dt (m) [link][node][layer]

double
	*maxcsedov0,	//maximum solids concentration in water column of overland plane
	*mincsedov0,	//minimum solids concentration in water column of overland plane
	*maxcsedov1,	//maximum solids concentration in surface sediment of overland plane
	*mincsedov1,	//minimum solids concentration in surface sediment of overland plane

	*maxcsedch0,	//maximum solids concentration in water column of channel network
	*mincsedch0,	//minimum solids concentration in water column of channel network
	*maxcsedch1,	//maximum solids concentration in surface sediment of channel network
	*mincsedch1;	//minimum solids concentration in surface sediment of channel network
