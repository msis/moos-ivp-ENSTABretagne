//Code by Jan SLIWKA in 2010

//#define REALSONAR

//#include "simulator.h"
//#include "underwater_robot.h"
#include "SonarDF.h"

using namespace std;

//------------------------------------------------------------------------------
SonarDF::SonarDF(){
	char chemin[50];
	sprintf(chemin, "config/SonarDF.txt");
	init(chemin);
}
//------------------------------------------------------------------------------
SonarDF::SonarDF(char* filename){
	init(filename);
}
//------------------------------------------------------------------------------
SonarDF::~SonarDF(){
	ReleaseMiniKingMgr(&hMiniKingMgr);
}
//------------------------------------------------------------------------------
void SonarDF::init(char* filename){//"Sensors/sonar/SonarDF.txt"
	//initialize the structure
	hMiniKingMgr = INVALID_HMINIKINGMGR_VALUE;
	//here create and configure the measurement thread
	char szDevice[100];
	int nstep;
	range = 0;nstep = 0;threshold = 0;PMinDist = 0;PMaxDist = 0;distance_error = 0;//just in case
	//open filename
	FILE *f;
	char line[256];
	f = fopen(filename,"r");
	fgets2(f,line,256); sscanf(line,"%s",szDevice);
	fgets2(f,line,256); sscanf(line,"%d",&range);
	fgets2(f,line,256); sscanf(line,"%d",&nstep);
	fgets2(f,line,256); sscanf(line,"%d",&threshold);
	fgets2(f,line,256); sscanf(line,"%lf",&PMinDist);
	fgets2(f,line,256); sscanf(line,"%lf",&PMaxDist);
	fgets2(f,line,256); sscanf(line,"%lf",&distance_error);

	//configure
	printf("Initialisation du sonar :\n\tDevice : %s\n\tRange : %d\n\tNstep : %d\n", szDevice, range, nstep);
	
	if(configure(szDevice,range,nstep))
		printf("Initialisation effectuée !\n");
		
	//
	data_1_or_2 = true;
}
//------------------------------------------------------------------------------
int SonarDF::configure(char* szDevice,int range,int nsteps){
	if(hMiniKingMgr != INVALID_HMINIKINGMGR_VALUE)
		ReleaseMiniKingMgr(&hMiniKingMgr);
		
	double scan_direction = 0;
	double scan_width = 360;
	int NBins = 200;
	int gain = 35;
	return InitMiniKingMgr(&hMiniKingMgr,szDevice,NBins,nsteps,range,scan_direction,scan_width,gain,SCANLINES_POLLING_MODE_MINIKING,THREAD_PRIORITY_NORMAL,TRUE);
}
//------------------------------------------------------------------------------
int SonarDF::configure(char* szDevice,int range,int nsteps,double scan_direction,double scan_width){

	if(hMiniKingMgr != INVALID_HMINIKINGMGR_VALUE) ReleaseMiniKingMgr(&hMiniKingMgr);
	int NBins = 200;
	int gain = 35;
	return InitMiniKingMgr(&hMiniKingMgr,szDevice,NBins,nsteps,range,scan_direction,scan_width,gain,SCANLINES_POLLING_MODE_MINIKING,THREAD_PRIORITY_NORMAL,TRUE);
}
//------------------------------------------------------------------------------
void SonarDF::SonarDF_lock(){

	while(!hMiniKingMgr->bConnected)usleep(1000*100);

}
//------------------------------------------------------------------------------
bool SonarDF::isConnected(){

	return hMiniKingMgr->bConnected?true:false;

}
//------------------------------------------------------------------------------
void SonarDF::get_sonar_raw_data(double &sonar_angle_param,vector<int> &scanline){

	double angle1 = 0,angle2 = 0;
	uint8* scanline1 = new uint8[hMiniKingMgr->NBins];
	uint8* scanline2 = new uint8[hMiniKingMgr->NBins];
	GetScanlineMiniKingMgr(hMiniKingMgr,scanline1,&angle1,scanline2,&angle2);
	//yield results
	if(data_1_or_2){
		for(int i=0;i<hMiniKingMgr->NBins;i++){
			scanline.push_back(*(scanline2+i));
		}

		sonar_angle_param = - angle2*M_PI/180;
	}
	else{
		for(int i=0;i<hMiniKingMgr->NBins;i++){
			scanline.push_back(*(scanline1+i));
		}

		sonar_angle_param = - angle1*M_PI/180;	
	}
	delete [] scanline1;
	delete [] scanline2;

	data_1_or_2 = !data_1_or_2;

}
//------------------------------------------------------------------------------
void SonarDF::get_sonar_raw_data(vector<double> &Valpha,vector<vector<int> > &Vscanlines){
	Valpha.clear();Vscanlines.clear();
	uint8* scanlines = new uint8[hMiniKingMgr->NSteps*hMiniKingMgr->NBins];
	double* angles = new double[hMiniKingMgr->NSteps];
	GetScanlinesMiniKingMgr(hMiniKingMgr,scanlines,angles);
	//foreach measure
	for(int i=0;i<hMiniKingMgr->NSteps;i++){
		Valpha.push_back(-angles[i]*M_PI/180);
		vector<int> scanline;
		for(int j=0;j<hMiniKingMgr->NBins;j++){
			scanline.push_back(scanlines[i*hMiniKingMgr->NBins+j]);
		}
		Vscanlines.push_back(scanline);
	}
	delete [] scanlines;
	delete [] angles;
}
//------------------------------------------------------------------------------
//data pre-processing for distance detection (threshold....)
//------------------------------------------------------------------------------

//:::::configure:::::::
void SonarDF::configure_distance_processing(int threshold,double PMinDist,double PMaxDist){
	this->threshold = threshold;
	this->PMinDist = PMinDist;
	this->PMaxDist = PMaxDist;
}

//:::::process the last measure:::::::

double SonarDF::get_sonar_distance(){//!! little wrong if used with the other get
	double distance = 0;
	double angle = 0;
	get_sonar_data(angle,distance);
	return distance;
}
//------------------------------------------------------------------------------
double SonarDF::get_sonar_angle(){//!! little wrong if used with the other get
	double distance = 0;
	double angle = 0;
	get_sonar_data(angle,distance);
	return angle;
}
//------------------------------------------------------------------------------
void SonarDF::get_sonar_data(double &sonar_angle_param,double &sonar_distance_param){

	//to implement begin
	//double minDist = PMinDist*hMiniKingMgr->RangeScale;double maxDist = PMaxDist*hMiniKingMgr->RangeScale;
	//ComputeFirstObstacleDistFromScanlineMiniKingMgr(hMiniKingMgr,scanline2,threshold,minDist,maxDist,&distance2);
	//end
	vector<int> scanline;
	get_sonar_raw_data(sonar_angle_param,scanline);//angle ok in radian
	//param
	double minDist = PMinDist*hMiniKingMgr->RangeScale;double maxDist = PMaxDist*hMiniKingMgr->RangeScale;
	if(isConnected()) GetFirstObstacleDistance(hMiniKingMgr->RangeScale, minDist, maxDist, threshold, scanline, sonar_distance_param);
	else sonar_distance_param = 100;
}
//

//:::::get the last sector scan::::
//------------------------------------------------------------------------------

//private://------------------------------------------------------------------------------

int SonarDF::GetFirstObstacleDistance(int RangeScale, double minDist, double maxDist, int threshold, vector<int> scanline, double &dist){

	if(scanline.size() == 0) return EXIT_FAILURE;

	int minBin = (int)((minDist/RangeScale)*scanline.size());
	int maxBin = (int)((maxDist/RangeScale)*scanline.size());

	maxBin=(maxBin>((int)scanline.size()-1))?((int)scanline.size()-1):maxBin;
	minBin=(minBin<0)?0:minBin;

	int i = minBin;

	for(;;){

		if (scanline[i] > threshold ) {
			if (i == maxBin-1 || scanline[i] > scanline[i+1]) break;
		}

		i++;

		if (i >= maxBin ){//we diminish the threshold until we find something
			i = minBin;
			threshold = (int)(7*threshold/8);
		}

		if(threshold < 2) {//security
			i = -1;
			break; 
		}

	}

	dist = (double)((double)i*(double)RangeScale/(double)scanline.size()); // Converts in m	

	return EXIT_SUCCESS;
}
//------------------------------------------------------------------------------
//-create interval vector corresponding to all bins above threshold

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//old functions : first obstacle distance 
void SonarDF::get_sonar_data(vector<double> &Valpha,vector<double> &Vdistance){

	vector<vector<int> > Vscanlines;
	get_sonar_raw_data(Valpha,Vscanlines);//Valpha OK
	//Compute distance foreach scanline
	for(int i=0;i<(int)Vscanlines.size();i++){
		double distance = 0;
		double minDist = PMinDist*hMiniKingMgr->RangeScale;double maxDist = PMaxDist*hMiniKingMgr->RangeScale;
		GetFirstObstacleDistance(hMiniKingMgr->RangeScale, minDist, maxDist, threshold, Vscanlines[i], distance);	
		Vdistance.push_back(distance);
	}
}