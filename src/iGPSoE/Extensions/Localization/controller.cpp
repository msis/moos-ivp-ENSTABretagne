#include "controller.h"


//---------------------------------------------------------------------------
controller::controller()	{   

	compteur=0;
	i_mission=0;
	FILE *f;
	char line[256];
	f = fopen("controller.txt","r");
	fgets2(f,line); sscanf(line,"%lf",&dt);
	fgets2(f,line); sscanf(line,"%d",&N_mission_points);
	for (int i=0;i<N_mission_points;i++)	{ 
		char mode;
		double m1,m2,m3,m4;
		fgets2(f,line); sscanf(line,"%c%lf%lf%lf%lf",&mode,&m1,&m2,&m3,&m4);
		mission_mode.push_back(mode);
		mission_1.push_back(m1); mission_2.push_back(m2); mission_3.push_back(m3); mission_4.push_back(m4);
	}
	fclose(f);
}
//---------------------------------------------------------------------------
void controller::clock(double& u1,double& u2,double& u3,box Xhat,double z)	{    

	if (mission_mode[i_mission] == 'u')	{  // Commande en boucle ouverte avec Chrono
		compteur = compteur+dt;
		u1 = mission_1[i_mission];
		u2 = mission_2[i_mission];
		u3 = 0;
		if (z < mission_3[i_mission]-0.1) u3 = 0;   // On remonte tout seul
		if (z > mission_3[i_mission]+0.1) u3 = -1;  // On descend
		if (compteur > mission_4[i_mission])	{
			compteur = 0;
			i_mission++;
		}
		return;
	}
	if (GoTo(mission_1[i_mission],mission_2[i_mission],mission_3[i_mission],mission_4[i_mission],u1,u2,u3,Xhat,z))	{
		i_mission++;
	}
}
//---------------------------------------------------------------------------
bool GoTo(double X,double Y,double Z,double radius,double& u1,double& u2,double& u3,box Xhat,double z)	{    

	double xhat,yhat,thetahat,ux,uy,ex,ey,norm_e,omega,a;

	// Régulation en profondeur
	u3 = 0;
	if (z < Z-0.1) u3 = 0;   // On remonte tout seul
	if (z > Z+0.1) u3 = -1;  // On descend

	xhat = Center(Xhat[1]);
	yhat = Center(Xhat[2]);
	thetahat = Center(Xhat[3]);
	ux = cos(thetahat);	
	uy = sin(thetahat);
	ex = X-xhat;		
	ey = Y-yhat;

	norm_e = sqrt(ex*ex+ey*ey)+0.001;  // 0.001 pour éviter les divisions par 0
	if (norm_e < radius)  { // On a atteint notre objectif.
		u1 = 0;
		u2 = 0;
		return true;
	}

	// ux*ex+uy*ey est le produit scalaire entre le cap de l'AUV et la direction désirée.
	// Si il est négatif, on ne va pas dans la bonne direction.
	if ((ux*ex+uy*ey)/norm_e < 0.5) omega = 1;  // Si l'AUV n'est pas dans le bon sens : rotation maximale
	else omega = fabs((ux*ey-ex*uy)/norm_e); // Sinon, la rotation est liée à l'erreur d'angle
	// La quantité (ux*ey-ex*uy)/norm_e  ci-dessus correspond au sinus de l'angle
	if (ux*ey-ex*uy < 0) omega = -omega; // Signe de la rotation, suivant que la cible est à droite ou à gauche

	if (fabs(omega) == 1)	{ // Rotation maximale
		u1 = -omega;
		u2 = +omega;
	}
	else	{
		// Dans ce qui suit, on a deux situations : si la norme de l'erreur
		// entre la cible et l'AUV est grande (supérieur à 3m), on propulse
		// normalement pour avoir une vitesse de croisière. Sinon, on propulse moins.
		// En gros, on a une band-bang proportionnelle.
		a = min(3.0, norm_e);
		// La poussée tangentielle est proportionnelle à la distance
		// norm_e, sauf si cette dernière est trop grande auquel cas,
		// on la sature à 3 mètre.
		u1 = 0.5*(0.33*a-1.0*omega);
		u2 = 0.5*(0.33*a+1.0*omega);
	}

	return false;
}
//---------------------------------------------------------------------------
void DepthRegulation(double Z,double& u1,double& u2,double& u3,double z)	{    

	u1 = 0;
	u2 = 0;
	u3 = 0;
	if (z < Z-0.1) u3 = 0;   // On remonte tout seul
	if (z > Z+0.1) u3 = -1;  // On descend
}
//---------------------------------------------------------------------------
void GoAhead(double Z,double& u1,double& u2,double& u3,double z)	{    

	u1 = 1;
	u2 = 1;
	u3 = 0;
	if (z < Z-0.1) u3 = 0;   // On remonte tout seul
	if (z > Z+0.1) u3 = -1;  // On descend
}
//---------------------------------------------------------------------------
void MoveBack(double Z,double& u1,double& u2,double& u3,double z)	{    

	// Recule et tourne un peu tout en restant à la même profondeur
	u1 = 0;
	u2 = -1;
	u3 = 0;
	if (z < Z-0.1) u3 = 0;   // On remonte tout seul
	if (z > Z+0.1) u3 = -1;  // On descend
}
//---------------------------------------------------------------------------

