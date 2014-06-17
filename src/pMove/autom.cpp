#include "autom.h"
#include "state.h"
#include "sonar.h"
#include <stdlib.h>  
#include <string>
#include <math.h>

using namespace std;

#define distance_entre_mesure 2
#define ANGLE_SIN_MAX 0.15
#define ANGLE_COS_MAX 0.988

unsigned int first;


void reglage_venant_sonar(float *heading){
  *heading = *heading*M_PI/180 + M_PI;
}

float reglage_venant_imu(float heading){
  return -heading*M_PI/180 + M_PI;
}

State* Autom::get_State(){
	return state;
}

void Autom::getSize(std::string data, std::string *scanline, float *heading, float * dist, int *size){
  std::string b = "bearing=";
  std::string a = "ad_interval=";
  std::string s = "scanline=";

  size_t bs = data.find(b);
  size_t as = data.find(a);
  size_t ss = data.find(s);


  std::string bearing = data.substr(bs + b.size(), as - (bs + b.size())-1);
  std::string ad_interval = data.substr(as + a.size(), ss - (as + a.size())-1);
  scanline->assign(data.substr(ss + s.size(), data.size() - (ss + s.size()) -1));

  size_t xs = scanline->find("x");

  *size = atoi(scanline->substr(1,xs-1).c_str());

  size_t vs = scanline->find("{");

  scanline->assign(scanline->substr(vs + 1, scanline->size()));

  *heading = atof(bearing.c_str());
  reglage_venant_sonar(heading);
  *dist = atof(ad_interval.c_str())/2;
}

void Autom::getData(std::string scanline, unsigned int *raw){

  const char *deb = scanline.c_str();
  char val[] = "\0\0\0\0";
  int i = 0;
  int j = 0;
  while( *deb !='\0'){
    if(*deb !=',' & j < 4){
      val[j] = *deb;
      j++;
    }
    else{
      //printf("taille %d val %s\n",j,val);
      j = 0;
      raw[i] = atoi(val);
      val[0] = '\0';val[1] = '\0';val[2] = '\0';val[3] = '\0';
      i++;
    }
    deb++;
  }

}

Autom::Autom(){
	
	state = new Init;
  state->set_x(-1);
	state->set_y(-1);
  state->set_haut(100);
  state->set_bas(100);
  state->set_droite(100);
  state->set_gauche(100);
  state->set_objectif(0);
  first = 0;
}

void Autom::transition(){
  printf("on va transitioner\n");

	State * old_State = state;
	state = state->next_State(&wall);
	delete old_State;
}

void Autom::doing(){

	state->doing();

}

void Autom::onNewMes(CMOOSMsg msg){

	if(msg.GetKey() == "SONAR_RAW_DATA" && wall){
		// on recupère les données
		
    string data  = msg.GetString();
    float heading, dist;
    int taille_vec;
    std::string scanline;
    getSize(data, &scanline, &heading, &dist, &taille_vec);

    unsigned int raw[taille_vec];
    getData(scanline, raw);

    int ind;
    Sonar::getIndMax(raw, taille_vec, &ind);

    dist = ind * dist;
    
    // On met à jour les distances au mur seulement si elles ne sont pas trop éloigné de l'ancienne (l'algorithme peut faire des erreurs)

    if(fabs(sin(state->get_cap() + heading)) < ANGLE_SIN_MAX && cos(state->get_cap() + heading) > ANGLE_COS_MAX){
      float distance_found = dist * fabs(cos(state->get_cap() + heading));
      if(fabs(distance_found - state->get_droite()) < distance_entre_mesure){
      	state->set_droite(distance_found);
      }
      //printf("distance au mur droit %f, avec un angle de %f, sin %f, cos %f\n",dist, 180/M_PI*(state->get_cap() + heading), sin(state->get_cap() + heading), cos(state->get_cap() + heading));
    }
    else if(fabs(sin(state->get_cap() + heading + M_PI/2)) < ANGLE_SIN_MAX && cos(state->get_cap() + heading + M_PI/2) > ANGLE_COS_MAX){
    	float distance_found = dist * fabs(cos(state->get_cap() + heading + M_PI/2));
      if(fabs(distance_found - state->get_bas()) < distance_entre_mesure){
	if(4-distance_found < 0){
		distance_found = distance_found-4;
	}
        state->set_haut(4.0 - distance_found);
      } 
      //printf("distance au mur bas %f, avec un angle de %f, sin %f, cos %f\n",dist, 180/M_PI*(state->get_cap() + heading), sin(state->get_cap() + heading + M_PI/2), cos(state->get_cap() + heading + M_PI/2));
    }
    else if(fabs(sin(state->get_cap() + heading - M_PI/2)) < ANGLE_SIN_MAX && cos(state->get_cap() + heading- M_PI/2) > ANGLE_COS_MAX){
    	float distance_found = dist * fabs(cos(state->get_cap() + heading - M_PI/2));
      if(fabs(distance_found - state->get_haut()) < distance_entre_mesure){
        state->set_haut(distance_found);
      }
      //printf("distance au mur haut %f, avec un angle de %f, sin %f, cos %f\n",dist, 180/M_PI*(state->get_cap() + heading), sin(state->get_cap() + heading - M_PI/2), cos(state->get_cap() + heading - M_PI/2));
    }
    else if(fabs(sin(state->get_cap() + heading - M_PI)) < ANGLE_SIN_MAX && cos(state->get_cap() + heading- M_PI) > ANGLE_COS_MAX){
    	float distance_found = dist * fabs(cos(state->get_cap() + heading + M_PI));
      if(fabs(distance_found - state->get_gauche()) < distance_entre_mesure){
        state->set_droite(3.0 - distance_found);
      }
      //printf("distance au mur gauche %f, avec un angle de %f, sin %f, cos %f\n",dist, 180/M_PI*(state->get_cap() + heading), sin(state->get_cap() + heading + M_PI), cos(state->get_cap() + heading + M_PI));
    }

    if (state->arrived())
		{
		  this->transition();
		  this->doing();
		}
	}

	if(msg.GetKey() == "VVV_HEADING"){
		// recuperer le cap et le mettre à jour
		state->set_cap( reglage_venant_imu( msg.GetDouble() ) );
    state->maj_sonar();
	}

	if(msg.GetKey() == "LOCALISATIONX"){
		//mettre à jour la position*
    printf("loc x :%f\n",msg.GetDouble());

    state->set_x(msg.GetDouble());
      
    if(first){
      first = 0;
      state->set_gauche(state->get_x());
      state->set_droite(3 - state->get_x());
      state->set_haut(4 - state->get_y());
      state->set_bas(state->get_y());
      transition();
      doing();
      transition();
      doing();
    }
    else{
      first = 1;
    }
	}

	if(msg.GetKey() == "LOCALISATIONY"){
		//mettre à jour la position*
    printf("loc y :%f\n",msg.GetDouble());

    state->set_y(msg.GetDouble());

    if(first){
      first = 0;
      state->set_gauche(state->get_x());
      state->set_droite(3 - state->get_x());
      state->set_haut(4 - state->get_y());
      state->set_bas(state->get_y());
      transition();
      doing();
      transition();
      doing();
    }
    else{
      first = 1;
    }
	}

	if(msg.GetKey() == "X2REACH"){
		state->set_x2reach(msg.GetDouble());
	}

	if(msg.GetKey() == "Y2REACH"){
		state->set_y2reach(msg.GetDouble());
	}
}
