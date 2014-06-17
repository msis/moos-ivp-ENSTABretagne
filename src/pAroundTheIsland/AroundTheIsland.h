/**
 * \file AroundTheIsland.h
 * \brief Classe AroundTheIsland
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jul 17th 2013
 *
 * Application MOOS de mission autour d'une île
 *
 */

#ifndef AroundTheIsland_HEADER
#define AroundTheIsland_HEADER

#include <map>
#include <math.h>
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class AroundTheIsland : public CMOOSApp
{
	public:
		AroundTheIsland();
		~AroundTheIsland();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		bool obstacleDetecte();
		bool pause(double duree_pause);
		string getReferenceTemps();

	private: // Configuration variables

	private: // State variables
		unsigned int		m_iterations;
		double				m_timewarp;
		map<float, float> 	m_obstacles;
		bool				m_auv_initialise, m_mission_lancee;
		double 				m_heading;
		bool 				m_en_pause;
		bool				m_simulation_obstacle;
		double 				m_duree_pause;
		double 				m_date_pause, m_date_debut_mission, m_date_iterate_precedent;
		
		double 				m_distance_mur;
		
		// Paramètres
		double VITESSE_CONVERGENCE;
		double ANGLE_EVITEMENT;
		double ANGLE_CORRECTIF;
		double DUREE_EVITEMENT;
		double CAP_INITIAL;
		double VITESSE_CONSTANTE;
		double NOMBRE_MIN_OBSTACLES;
		double DISTANCE_LIMITE_OBSTACLES;
		double PROFONDEUR;
		double SEUIL_DETECTION_SONAR;
};

#endif 