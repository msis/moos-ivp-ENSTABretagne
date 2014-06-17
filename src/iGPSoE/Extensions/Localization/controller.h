#ifndef controllerH
#define controllerH     


#include "common.h"


//---------------------------------------------------------------------------
class controller
{
public:
	double compteur;
	int i_mission;
	double dt;
	int N_mission_points;
	vector <double> mission_1,mission_2,mission_3,mission_4;
	vector <char> mission_mode;
	controller();
	void clock(double& u1,double& u2,double& u3,box Xhat,double z);
};
//---------------------------------------------------------------------------

bool GoTo(double X,double Y,double Z,double radius,double& u1,double& u2,double& u3,box Xhat,double z);
void DepthRegulation(double Z,double& u1,double& u2,double& u3,double z);
void GoAhead(double Z,double& u1,double& u2,double& u3,double z);
void MoveBack(double Z,double& u1,double& u2,double& u3,double z);


#endif
