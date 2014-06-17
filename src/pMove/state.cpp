#include "state.h"
#include "stdio.h"
#include <iterator>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "MBUtils.h"

#define ang(x) atan2(sin(x),cos(x))
//#define FPATH "/home/jacques/"
#define FPATH "/home/ciscrea1/"

#define DEBUG

#ifdef DEBUG
	#define p printf
#else
	#define p 
#endif

#undef SIMU
#define DIRECT
#define MAJ

#define SPEED 10
#define TEMPS_ALIGNEMENT 5000
#define DISTANCE_ARRET 0.2
#define TEMPS_RECUL 3000

float reglage_cap_imu(float cap){
	return 180 - cap*180/M_PI;
}

void repere_sonar(float *left, float *right){
	*left = ang(*left + M_PI);
	*right = ang(*right + M_PI);
}

void reglage_cap_sonar(float *left, float *right){
    if(*left < 0){
      *left = *left * 180/M_PI + 360;
    }
    else{
      *left = *left * 180/M_PI;
    }
    if(*right < 0){
      *right = *right * 180/M_PI + 360;
    }
    else{
      *right = *right * 180/M_PI;
    }
}

float State::get_cap(){
	return cap_robot;
}

float State::get_x(){
	return x;
}

float State::get_y(){
	return y;
}

float State::get_bas(){
	return d_bas;
}

float State::get_haut(){
	return d_haut;
}

float State::get_gauche(){
	return d_gauche;
}

float State::get_droite(){
	return d_droite;
}

void State::set_bas(double a){
	printf("bas %f\n", a);
	d_bas = a;
}

void State::set_haut(double a){
	printf("haut %f, reach %f\n", a, 4 - y2reach);
	d_haut = a;
	char fhaut[256];
	*fhaut = '\0';
	sprintf(fhaut, "%shaut.txt",FPATH);
	FILE * f = fopen(fhaut, "a");
    if(f!=NULL){
    	fprintf(f, "%f\n", a);
        fclose(f);
    }
    else{
        printf("impossible d'ouvrir le fichier haut\n");
    }
}

void State::set_gauche(double a){
	printf("gauche %f\n", a);
	d_gauche = a;
}

void State::set_droite(double a){
	printf("droite %f, reach %f\n", a, 3 - x2reach);
	d_droite = a;
	char fdroite[256];
	*fdroite = '\0';
	sprintf(fdroite, "%sdroite.txt",FPATH);
	FILE * f = fopen(fdroite, "a");
    if(f!=NULL){
    	fprintf(f, "%f\n", a);
        fclose(f);
    }
    else{
        printf("impossible d'ouvrir le fichier droite\n");
    }
}

void State::set_cap(float a){
	cap_robot = a;
}

void State::set_x(double a){
	x = a;
	char fx[256];
	*fx = '\0';
	sprintf(fx, "%shaut.txt",FPATH);
	FILE * f = fopen(fx, "w");
    if(f!=NULL){
    	fwrite(&a, sizeof(double), 1, f);
        fclose(f);
    }
    else{
        printf("impossible d'ouvrir le fichier haut\n");
    }
}


void State::set_y(double a){
	y = a;
	char fy[256];
	*fy = '\0';
	sprintf(fy, "%shaut.txt",FPATH);
	FILE * f = fopen(fy, "w");
    if(f!=NULL){
    	double b = 4 - a;
    	fwrite(&b, sizeof(double), 1, f);
        fclose(f);
    }
    else{
        printf("impossible d'ouvrir le fichier haut\n");
    }
}

void State::set_x2reach(double a){
	x2reach = a;
}


void State::set_y2reach(double a){
	y2reach = a;
}

void State::set_app(void* lapp){
	app = (IMove*) lapp;
}

void State::set_objectif(unsigned int objectif){
	pos_objectif = objectif;
}

State * Init::next_State(unsigned char *wall){

	*wall = 1;

	State * state = new OBJECTIF;
	p("on recupère un nouvelle objectif l'objectif\n");
	state->set_droite(d_droite);
	state->set_gauche(d_gauche);
	state->set_haut(d_haut);
	state->set_bas(d_bas);
	state->set_x(x);
	state->set_y(y);
	state->set_x2reach(x2reach);
	state->set_y2reach(y2reach);
	state->set_cap(cap_robot);
	state->set_app(app);
	state->set_objectif(pos_objectif);

	return state;
}

State * OBJECTIF::next_State(unsigned char *wall){

	*wall = 1;

#ifdef DIRECT

	State * state = new STRAIGHT;
	p("on va reguler en tous droit vers l'objectif\n");
	state->set_droite(d_droite);
	state->set_gauche(d_gauche);
	state->set_haut(d_haut);
	state->set_bas(d_bas);
	state->set_x(x);
	state->set_y(y);
	state->set_x2reach(x2reach);
	state->set_y2reach(y2reach);
	state->set_cap(cap_robot);
	state->set_app(app);
	state->set_objectif(pos_objectif);
	return state;
#else
	if(fabs(x-x2reach) > fabs(y-y2reach)){
		p("on va reguler en X\n");
		State * state = new REGULX;
		state->set_droite(d_droite);
		state->set_gauche(d_gauche);
		state->set_haut(d_haut);
		state->set_bas(d_bas);
		state->set_x(x);
		state->set_y(y);
		state->set_x2reach(x2reach);
		state->set_y2reach(y2reach);
		state->set_cap(cap_robot);
		state->set_app(app);
		return state;
	}
	else{
		State * state = new REGULY;
		p("on va reguler en Y\n");
		state->set_droite(d_droite);
		state->set_gauche(d_gauche);
		state->set_haut(d_haut);
		state->set_bas(d_bas);
		state->set_x(x);
		state->set_y(y);
		state->set_x2reach(x2reach);
		state->set_y2reach(y2reach);
		state->set_cap(cap_robot);
		state->set_app(app);
		return state;
	}
#endif
}

State * REGULX::next_State(unsigned char *wall){
	// check somethings
	if(fabs(d_bas-y2reach) < 0.1 || fabs(4 - d_haut - y2reach) < 0.1){
		*wall = 1;
		State * state;
		state = new LOC;
		p("on va reguler en Y\n");
		state->set_droite(d_droite);
		state->set_gauche(d_gauche);
		state->set_haut(d_haut);
		state->set_bas(d_bas);
		state->set_x(x);
		state->set_y(y);
		state->set_x2reach(x2reach);
		state->set_y2reach(y2reach);
		state->set_cap(cap_robot);
		state->set_app(app);
		state->set_objectif(pos_objectif);
		return state;
	}
	else{
		*wall = 0;
		State *state;
		state = new REGULY;
		p("on va reguler en Y\n");
		state->set_droite(d_droite);
		state->set_gauche(d_gauche);
		state->set_haut(d_haut);
		state->set_bas(d_bas);
		state->set_x(x);
		state->set_y(y);
		state->set_x2reach(x2reach);
		state->set_y2reach(y2reach);
		state->set_cap(cap_robot);
		state->set_app(app);
		state->set_objectif(pos_objectif);
		return state;
	}
}

State * REGULY::next_State(unsigned char *wall){
	// check somethings
	if(fabs(d_gauche-x2reach) < 0.1 || fabs(3 - d_droite - x2reach) < 0.1){
		printf("gauche %f droite %f\n",d_gauche, d_droite);
		*wall = 0;
		State * state;
		state = new LOC;
		state->set_droite(d_droite);
		state->set_gauche(d_gauche);
		state->set_haut(d_haut);
		state->set_bas(d_bas);
		state->set_x(x);
		state->set_y(y);
		state->set_x2reach(x2reach);
		state->set_y2reach(y2reach);
		state->set_cap(cap_robot);
		state->set_app(app);
		state->set_objectif(pos_objectif);
		return state;
	}
	else{
		*wall = 1;
		State *state;
		state = new REGULX;
		p("on va reguler en X\n");
		state->set_droite(d_droite);
		state->set_gauche(d_gauche);
		state->set_haut(d_haut);
		state->set_bas(d_bas);
		state->set_x(x);
		state->set_y(y);
		state->set_x2reach(x2reach);
		state->set_y2reach(y2reach);
		state->set_cap(cap_robot);
		state->set_app(app);
		state->set_objectif(pos_objectif);
		return state;
	}
}

State * LOC::next_State(unsigned char *wall){

	*wall = 0;
	State * state = new OBJECTIF;
	state->set_droite(d_droite);
	state->set_gauche(d_gauche);
	state->set_haut(d_haut);
	state->set_bas(d_bas);
	state->set_x(x);
	state->set_y(y);
	state->set_x2reach(x2reach);
	state->set_y2reach(y2reach);
	state->set_cap(cap_robot);
	state->set_app(app);
	state->set_objectif(pos_objectif);
	return state;
}

State * STRAIGHT::next_State(unsigned char *wall){

	*wall = 0;
	State * state = new LOC;
	state->set_droite(d_droite);
	state->set_gauche(d_gauche);
	state->set_haut(d_haut);
	state->set_bas(d_bas);
	state->set_x(x);
	state->set_y(y);
	state->set_x2reach(x2reach);
	state->set_y2reach(y2reach);
	state->set_cap(cap_robot);
	state->set_app(app);
	state->set_objectif(pos_objectif);
	return state;
}

State * State::next_State(unsigned char *wall){
	printf("ERROR, INTERFACE METHOD IS CALLED !!!\n");
}

void Init::doing(){
	// TO DO
	p("Doing init\n");

	// on dit au sonar de ne rgarder les données au de la de 5 mètres
	app->myNotify("SONAR_PARAMS", "Range=5");

	// on récupère l'objectif
	app->myNotify("DO_LOC", 1.0);
	// met à jour le cap et les distances
}

void OBJECTIF::doing(){
	// TO DO
	p("finding new objectif\n");
	std::ifstream file;
    std::string filename = "/home/ciscrea1/Course.txt";
    file.open(filename.c_str());
    for(unsigned int j = 0 ;j < pos_objectif ;j++){
    	std::string t;
        getline(file, t);
    }
    std::string sx, sy;
    getline(file, sx);
    if(!file.eof()){
    	x2reach = atof(sx.c_str());
    }
    else{
    	pos_objectif = 0;
    	file.close();
    	doing();
    }

    getline(file, sy);
    if(!file.eof())
    	y2reach = atof(sy.c_str());
    else
    	sleep(100);
    pos_objectif = pos_objectif + 2;
    file.close();
}

void REGULX::doing(){
	// TO DO
	// met à jour la régulation en cap
	p("doing RGULX\n");
	double cap_a_prendre;
	if(x-x2reach < 0){
		cap_a_prendre = 0;
		printf("cap à prendre %f\n", cap_a_prendre);
	}
	else{
		cap_a_prendre = -M_PI;
		printf("cap à prendre %f\n", cap_a_prendre);
	}
	app->myNotify("VVV_HEADING_DESIRED",reglage_cap_imu(cap_a_prendre));

#ifdef SIMU
	printf("je dois changer ttmin et ttmax\n");
	app->myNotify("TTMIN", 0 - M_PI/20);
	app->myNotify("TTMAX", 0 + M_PI/20);
#else
	// std::ostringstream param_sonar_stream;
	// param_sonar_stream << "Continuous=false,LeftLimit=" << (-M_PI/20) << "RightLimit=" << (M_PI/20);
	// std::string param_sonar = param_sonar_stream.str();
	// app->myNotify("SONAR_PARAMS", param_sonar);
#endif

	// met à jour les valeurs des moteurs
	app->myNotify("VVV_VX_DESIRED", SPEED);

}

void REGULY::doing(){
	// met à jour la régulation en cap
	p("Doing regulY\n");
	double cap_a_prendre;
	if(y-y2reach < 0){
		cap_a_prendre = M_PI/2;
		printf("cap à prendre %f\n", cap_a_prendre);
	}
	else{
		cap_a_prendre = -M_PI/2;
		printf("cap à prendre %f\n", cap_a_prendre);
	}
	app->myNotify("VVV_HEADING_DESIRED",reglage_cap_imu(cap_a_prendre));

#ifdef SIMU
	printf("je dois changer ttmin et ttmax\n");
	app->myNotify("TTMIN", 0 - M_PI/20);
	app->myNotify("TTMAX", 0 + M_PI/20);
#else
	// std::ostringstream param_sonar_stream;
	// param_sonar_stream << "LeftLimit=" << (-M_PI/20) << "RightLimit=" << (M_PI/20);
	// std::string param_sonar = param_sonar_stream.str();
	// app->myNotify("SONAR_PARAMS", param_sonar);
#endif

	// met à jour les valeurs des moteurs
	app->myNotify("VVV_VX_DESIRED", SPEED);

}

void LOC::doing(){
	// on lance la localisation
	app->myNotify("DO_LOC",0.0);
	p("Doing loc\n");
}

void STRAIGHT::doing(){
	// met à jour la régulation en cap
	p("Doing straight forward\n");
	double cap_a_prendre;
	cap_a_prendre = atan2(y2reach - y, x2reach - x);

	printf("_____ y2r %f y %f x2r %f x %f\n", y2reach, y, x2reach, x);
	printf("_____ y2r %f x2r %f\n", y2reach - y, x2reach - x);
	p("cap à prendre : %f\n", cap_a_prendre*180/M_PI);

	app->myNotify("VVV_HEADING_DESIRED",reglage_cap_imu(cap_a_prendre));
	printf("pause...\n");
	MOOSPause(TEMPS_ALIGNEMENT);
	printf("...finie\n");

	if (fabs(x2reach - x) > fabs(y2reach - y))
	{
		lookx = 1;
		if(x2reach > x){
			sens = 1;
		}
		else{
			sens = 0;
		}
		printf("on regarde à droite\n");
#ifdef SIMU
		printf("je dois changer ttmin et ttmax\n");
		printf("on Notify %f et %f \n", ang(-cap_a_prendre - M_PI/20)*180/M_PI, ang(-cap_a_prendre + M_PI/20)*180/M_PI);
		printf("on Notify %f et %f \n", ang(-cap_a_prendre - M_PI/20), ang(-cap_a_prendre + M_PI/20));
		app->myNotify("TTMIN", ang(-cap_a_prendre - M_PI/20));
		app->myNotify("TTMAX", ang(-cap_a_prendre + M_PI/20));
#else
	#ifdef MAJ

		float left = ang(-cap_a_prendre - M_PI/20);
		float right = ang(-cap_a_prendre + M_PI/20);
		if(cos(right) < 0 && cos(left) < 0){
			left = ang(left + M_PI);
			right = ang(right + M_PI);
		}
		// le sonar regar derrière lorsqu'il est à 0 degrée, il faut donc lui ajouter 180 degré
		repere_sonar(&left, &right);
		// le sonar ne comprend que les valeur entre 0 et 360 exclut
		reglage_cap_sonar(&left, &right);

		std::ostringstream param_sonar_stream;
		param_sonar_stream << "LeftLimit=" << left << ",RightLimit=" << right;
		std::string param_sonar = param_sonar_stream.str();
		app->myNotify("SONAR_PARAMS", param_sonar);
	#endif
#endif
	}
	else{
		lookx = 0;
		if(y2reach > y){
			sens = 1;
		}
		else{
			sens = 0;
		}
		printf("on regarde en haut\n");
#ifdef SIMU
		printf("je dois changer ttmin et ttmax\n");
		printf("on Notify %f et %f \n", ang(M_PI/2 - cap_a_prendre - M_PI/20)*180/M_PI, ang(M_PI/2 - cap_a_prendre + M_PI/20)*180/M_PI);
		printf("on Notify %f et %f \n", ang(M_PI/2 - cap_a_prendre - M_PI/20), ang(M_PI/2 - cap_a_prendre + M_PI/20));
		app->myNotify("TTMIN", ang(M_PI/2 - cap_a_prendre - M_PI/20));
		app->myNotify("TTMAX", ang(M_PI/2 - cap_a_prendre + M_PI/20));
#else
	#ifdef MAJ

		float left = ang(M_PI/2 - cap_a_prendre - M_PI/20);
		float right = ang(M_PI/2 - cap_a_prendre + M_PI/20);
		if(cos(right) < 0 && cos(left) < 0){
			left = ang(left + M_PI);
			right = ang(right + M_PI);
		}
		// le sonar regar derrière lorsqu'il est à 0 degrée, il faut donc lui ajouter 180 degré
		repere_sonar(&left, &right);
		reglage_cap_sonar(&left, &right);

		std::ostringstream param_sonar_stream;
		param_sonar_stream << "LeftLimit=" << left << ",RightLimit=" << right;
		std::string param_sonar = param_sonar_stream.str();
		app->myNotify("SONAR_PARAMS", param_sonar);
	#endif
#endif
	}



	// met à jour les valeurs des moteurs
	app->myNotify("VVV_VX_DESIRED", SPEED);

}

void State::doing(){
	printf("ERROR, INTERFACE METHOD IS CALLED\n");
}


int REGULX::arrived(){
	if(fabs(d_gauche - x2reach) < 0.1 || fabs(3 - d_droite - x2reach) < 0.1 ){
		printf("on a à droite %f, à gauche %f et à atteindre %f\n",d_droite, d_gauche, x2reach);
		app->myNotify("VVV_VX_DESIRED", 0.0);
		return 1;
	}
	else return 0;
}

int REGULY::arrived(){
	if(fabs(d_bas - y2reach) < 0.1 || fabs(4 - d_haut - y2reach) < 0.1 ){
		printf("on a en haut %f, en bas %f et y à atteindre %f\n",d_haut, d_bas, y2reach);
		app->myNotify("VVV_VX_DESIRED", 0.0);
		return 1;
	}
	else return 0;
}

int STRAIGHT::arrived(){

	if(lookx){
		if(( sens && (d_droite <= 3 - x2reach + DISTANCE_ARRET) ) || ((!sens) && (3 - d_droite <= x2reach + DISTANCE_ARRET) )){
			printf("on a à gauche %f, à droite bas %f et x à atteindre %f\n",d_gauche, d_droite, x2reach);
			app->myNotify("VVV_VX_DESIRED", -SPEED);
			MOOSPause(TEMPS_RECUL);
			app->myNotify("VVV_VX_DESIRED", 0.0);
			return 1;
		}
		else return 0;	
	}
	else{
		if(( sens && (d_haut <= 4 - y2reach + DISTANCE_ARRET) ) || ((!sens) && (4 - d_haut <= y2reach + DISTANCE_ARRET) )){
			printf("on a en haut %f, en bas %f et y à atteindre %f\n",d_haut, d_bas, y2reach);
			app->myNotify("VVV_VX_DESIRED", -SPEED);
			MOOSPause(TEMPS_RECUL);
			app->myNotify("VVV_VX_DESIRED", 0.0);
			return 1;
		}
		else return 0;	
	}
}

int Init::arrived(){
	//printf("erreur : appel de la focntion arrived dans l'etat INIT\n");
	return 0;
}

int LOC::arrived(){
	//printf("erreur : appel de la focntion arrived dans l'etat LOC\n");
	return 0;
}
int OBJECTIF::arrived(){
	//printf("erreur : appel de la focntion arrived dans l'etat OBJECTIF\n");
	return 0;
}

int State::arrived(){
	printf("erreur : appel de la focntion arrived dans la classe mère\n");
}

void STRAIGHT::maj_sonar(){
#ifdef MAJ
#else
	if(lookx){
		float left = ang(-cap_robot - M_PI/20);
		float right = ang(-cap_robot + M_PI/20);
		if(cos(right) < 0 && cos(left) < 0){
			left = ang(left + M_PI);
			right = ang(right + M_PI);
		}
		//p("Dans notre repère: %f %f\n", left*180/M_PI, right*180/M_PI);
		// le sonar regar derrière lorsqu'il est à 0 degrée, il faut donc lui ajouter 180 degré
		repere_sonar(&left, &right);
		// le sonar ne comprend que les valeur entre 0 et 360 exclut
		reglage_cap_sonar(&left, &right);

		std::ostringstream param_sonar_stream;
		param_sonar_stream << "LeftLimit=" << left << ",RightLimit=" << right;
		std::string param_sonar = param_sonar_stream.str();
		app->myNotify("SONAR_PARAMS", param_sonar);
	}
	else{
		float left = ang(M_PI/2 - cap_robot - M_PI/20);
		float right = ang(M_PI/2 - cap_robot + M_PI/20);
		if(cos(right) < 0 && cos(left) < 0){
			left = ang(left + M_PI);
			right = ang(right + M_PI);
		}
		//p("Dans notre repère: %f %f\n", left*180/M_PI, right*180/M_PI);
		// le sonar regar derrière lorsqu'il est à 0 degrée, il faut donc lui ajouter 180 degré
		repere_sonar(&left, &right);
		// le sonar ne comprend que les valeur entre 0 et 360 exclut
		reglage_cap_sonar(&left, &right);

		std::ostringstream param_sonar_stream;
		param_sonar_stream << "LeftLimit=" << left << ",RightLimit=" << right;
		std::string param_sonar = param_sonar_stream.str();
		app->myNotify("SONAR_PARAMS", param_sonar);
	}
#endif
}

void LOC::maj_sonar(){
}

void Init::maj_sonar(){
}

void REGULY::maj_sonar(){
	float left = - M_PI/20;
	float right = M_PI/20;
	if(cos(right) < 0 && cos(left) < 0){
		left = ang(left + M_PI);
		right = ang(right + M_PI);
	}
	// le sonar regar derrière lorsqu'il est à 0 degrée, il faut donc lui ajouter 180 degré
	repere_sonar(&left, &right);
	// le sonar ne comprend que les valeur entre 0 et 360 exclut
	reglage_cap_sonar(&left, &right);

	std::ostringstream param_sonar_stream;
	param_sonar_stream << "LeftLimit=" << left << ",RightLimit=" << right;
	std::string param_sonar = param_sonar_stream.str();
	app->myNotify("SONAR_PARAMS", param_sonar);
}

void REGULX::maj_sonar(){
	float left = - M_PI/20;
	float right = M_PI/20;
	if(cos(right) < 0 && cos(left) < 0){
		left = ang(left + M_PI);
		right = ang(right + M_PI);
	}
	// le sonar regar derrière lorsqu'il est à 0 degrée, il faut donc lui ajouter 180 degré
	repere_sonar(&left, &right);
	// le sonar ne comprend que les valeur entre 0 et 360 exclut
	reglage_cap_sonar(&left, &right);

	std::ostringstream param_sonar_stream;
	param_sonar_stream << "LeftLimit=" << left << ",RightLimit=" << right;
	std::string param_sonar = param_sonar_stream.str();
	app->myNotify("SONAR_PARAMS", param_sonar);
}

void OBJECTIF::maj_sonar(){
}

void State::maj_sonar(){
}
