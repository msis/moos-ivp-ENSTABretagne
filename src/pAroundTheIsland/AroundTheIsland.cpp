/**
 * \file AroundTheIsland.cpp
 * \brief Classe AroundTheIsland
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jul 17th 2013
 *
 * Application MOOS de mission autour d'une île
 *
 */

#include <iterator>
#include <algorithm>
#include "MBUtils.h"
#include "AroundTheIsland.h"

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
AroundTheIsland::AroundTheIsland()
{
	m_iterations = 0;
	m_timewarp   = 1;
	m_auv_initialise = false;
	m_mission_lancee = false;
	m_simulation_obstacle = false;
	m_en_pause = false;
	m_distance_mur = 1000;
	
	// Initialisation des paramètres
	VITESSE_CONVERGENCE			= 0.001;
	ANGLE_EVITEMENT				= 60;
	ANGLE_CORRECTIF				= 45;
	DUREE_EVITEMENT				= 5;
	CAP_INITIAL					= 90;
	VITESSE_CONSTANTE			= 15;
	NOMBRE_MIN_OBSTACLES 		= 5;
	DISTANCE_LIMITE_OBSTACLES 	= 5;
	PROFONDEUR				 	= 2;
	SEUIL_DETECTION_SONAR		= 100;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

AroundTheIsland::~AroundTheIsland()
{
	
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool AroundTheIsland::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;
		if(msg.GetSource() == GetAppName())
			continue;

		if(msg.GetKey() == "GO_ISLAND" && msg.GetDouble() == 1.0)
		{
			m_mission_lancee = true;
			m_date_debut_mission = MOOSTime();
			cout << endl << getReferenceTemps() << "Lancement de la mission" << endl;
			m_Comms.Notify("GO_ISLAND", 0.0);
		}

		if(msg.GetKey() == "STOP_ISLAND" && msg.GetDouble() == 1.0)
		{
			m_mission_lancee = false;
			cout << endl << getReferenceTemps() << "Arret de la mission" << endl;
			m_Comms.Notify("VVV_Z_DESIRED", 0.0);
			m_Comms.Notify("VVV_VX_DESIRED", 0.0);
			m_Comms.Notify("STOP_ISLAND", 0.0);
		}

		if(msg.GetKey() == "SET_OBSTACLE" && msg.GetDouble() == 1.0)
		{
			m_simulation_obstacle = true;
			m_Comms.Notify("SET_OBSTACLE", 0.0);
		}

		if(msg.GetKey() == "DIST_MUR")
		{
			m_distance_mur = msg.GetDouble();
		}

		if(msg.GetKey() == "SONAR_RAW_DATA")
		{
			float angle = 0;
			int nRows, nCols;
			float ad_interval = 0.25056;
			vector<unsigned int> scanline;
			MOOSValFromString(angle, msg.GetString(), "bearing");
			MOOSValFromString(ad_interval, msg.GetString(), "ad_interval");
			MOOSValFromString(scanline, nRows, nCols, msg.GetString(), "scanline");
			
			// Enregistrement de la position de l'obstacle, pour l'angle donné :
			for(int i = 0 ; i < (int)scanline.size() ; i ++)
			{
				float dist = (ad_interval / 2.0) * i;
				if(scanline[i] > SEUIL_DETECTION_SONAR && dist > 0.5) // Détection du premier dépassement du seuil
				{
					m_obstacles[angle] = dist;
					break;
				}
			}
		}
		
		if(msg.GetKey() == "ATI_V_CONV")
			VITESSE_CONVERGENCE = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_ANG_EVIT")
			ANGLE_EVITEMENT = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_DUR_EVIT")
			DUREE_EVITEMENT = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_ANG_CORR")
			ANGLE_CORRECTIF = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_INIT_CAP")
			CAP_INITIAL = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_V_CONST")
		{
			VITESSE_CONSTANTE = msg.GetDouble();
			m_Comms.Notify("VVV_VX_DESIRED", VITESSE_CONSTANTE);
		}
		
		if(msg.GetKey() == "ATI_NB_MIN_OBST")
			NOMBRE_MIN_OBSTACLES = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_DIST_LIM_OBST")
			DISTANCE_LIMITE_OBSTACLES = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_SEUIL_SONAR")
			SEUIL_DETECTION_SONAR = msg.GetDouble();
		
		if(msg.GetKey() == "ATI_Z")
		{
			PROFONDEUR = msg.GetDouble();
			m_Comms.Notify("VVV_Z_DESIRED", PROFONDEUR);
		}
		
		#if 0 // Keep these around just for template
			string key   = msg.GetKey();
			string comm  = msg.GetCommunity();
			double dval  = msg.GetDouble();
			string sval  = msg.GetString(); 
			string msrc  = msg.GetSource();
			double mtime = msg.GetTime();
			bool   mdbl  = msg.IsDouble();
			bool   mstr  = msg.IsString();
		#endif
	}

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool AroundTheIsland::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool AroundTheIsland::Iterate()
{
	if(m_en_pause)
	{
		if(MOOSTime() - m_date_pause < m_duree_pause)
			return (true);
	}
	
	if(m_mission_lancee)
	{
		if(!m_auv_initialise)
		{
			m_heading = CAP_INITIAL;
			m_Comms.Notify("VVV_Z_DESIRED", PROFONDEUR);
			m_Comms.Notify("VVV_HEADING_DESIRED", m_heading);
			
			if(!m_en_pause)
				return pause(5);
			
			else
				m_en_pause = false;
			
			m_Comms.Notify("VVV_VX_DESIRED", VITESSE_CONSTANTE);
			cout << getReferenceTemps() << "Fin de l'initialisation" << endl << endl;
			m_date_iterate_precedent = MOOSTime();
			m_auv_initialise = true;
		}
		
		else
		{
			m_heading += (MOOSTime() - m_date_iterate_precedent) * VITESSE_CONVERGENCE;
			m_Comms.Notify("VVV_HEADING_DESIRED", m_heading);
			
			if(obstacleDetecte())
			{
				// Éloignement dans une autre direction
				m_Comms.Notify("VVV_HEADING_DESIRED", m_heading - ANGLE_EVITEMENT);
				cout << getReferenceTemps() << "Obstacle détecté" << endl;
				cout << getReferenceTemps() << "\tÉloignement..." << endl;
				return pause(DUREE_EVITEMENT);
			}
			
			if(m_en_pause) // Reprise de cap si plus d'obstacle (la pause vient de se finir)
			{
				m_heading -= ANGLE_CORRECTIF;
				m_Comms.Notify("VVV_HEADING_DESIRED", m_heading);
				cout << getReferenceTemps() << "\tReprise de cap..." << endl << endl;
				m_en_pause = false;
			}
		}
	}
	
	m_date_iterate_precedent = MOOSTime();
	return(true);
}

/**
 * \fn
 * \brief Méthode de détection d'obstacle
 */
 
bool AroundTheIsland::obstacleDetecte()
{
	if(m_simulation_obstacle)
	{
		m_simulation_obstacle = false;
		return true;
	}
	
	else
	{
		double angle, distance;
		int nb_points_proches = 0;
		
		for(map<float, float>::iterator it = m_obstacles.begin() ; it != m_obstacles.end() ; it ++)
		{
			angle = it->first; 		// clef
			distance = it->second; 	// valeur
			
			if(distance < DISTANCE_LIMITE_OBSTACLES)
				nb_points_proches ++;
		}
			
		return nb_points_proches > NOMBRE_MIN_OBSTACLES;
		
		/*
		// Avec l'algo de WallFollowing en regression :
		
		if(m_distance_mur < DISTANCE_LIMITE_OBSTACLES)
		{
			cout << "Obstacle détecté à " << m_distance_mur << "m" << endl;
			return true;
		}
		
		else
			return false;*/
	}
}

/**
 * \fn
 * \brief Méthode de détection d'obstacle
 */
 
bool AroundTheIsland::pause(double duree_pause)
{
	m_en_pause = true;
	m_duree_pause = duree_pause;
	m_date_pause = MOOSTime();
	return m_en_pause;
}

/**
 * \fn
 * \brief Méthode retournant une chaine indiquant la durée depuis le lancement de la mission
 */
 
string AroundTheIsland::getReferenceTemps()
{
	char chaine[100];
	int secondes = MOOSTime() - m_date_debut_mission;
	sprintf(chaine, " t = %02d:%02d \t", secondes / 60, secondes % 60);
	return string(chaine);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool AroundTheIsland::OnStartUp()
{
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
		list<string>::iterator p;
		for(p = sParams.begin() ; p != sParams.end() ; p++)
		{
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);

			if(param == "VITESSE_CONVERGENCE")
			{
				VITESSE_CONVERGENCE = atof(value.c_str());
				m_Comms.Notify("ATI_V_CONV", VITESSE_CONVERGENCE);
			}

			else if(param == "ANGLE_EVITEMENT")
			{
				ANGLE_EVITEMENT = atof(value.c_str());
				m_Comms.Notify("ATI_ANG_EVIT", ANGLE_EVITEMENT);
			}

			else if(param == "ANGLE_CORRECTIF")
			{
				ANGLE_CORRECTIF = atof(value.c_str());
				m_Comms.Notify("ATI_ANG_CORR", ANGLE_CORRECTIF);
			}

			else if(param == "DUREE_EVITEMENT")
			{
				DUREE_EVITEMENT = atof(value.c_str());
				m_Comms.Notify("ATI_DUR_EVIT", DUREE_EVITEMENT);
			}

			else if(param == "CAP_INITIAL")
			{
				CAP_INITIAL = atof(value.c_str());
				m_Comms.Notify("ATI_INIT_CAP", CAP_INITIAL);
			}

			else if(param == "VITESSE_CONSTANTE")
			{
				VITESSE_CONSTANTE = atof(value.c_str());
				m_Comms.Notify("ATI_V_CONST", VITESSE_CONSTANTE);
			}

			else if(param == "NOMBRE_MIN_OBSTACLES")
			{
				NOMBRE_MIN_OBSTACLES = atof(value.c_str());
				m_Comms.Notify("ATI_NB_MIN_OBST", NOMBRE_MIN_OBSTACLES);
			}

			else if(param == "DISTANCE_LIMITE_OBSTACLES")
			{
				DISTANCE_LIMITE_OBSTACLES = atof(value.c_str());
				m_Comms.Notify("ATI_DIST_LIM_OBST", DISTANCE_LIMITE_OBSTACLES);
			}

			else if(param == "PROFONDEUR")
			{
				PROFONDEUR = atof(value.c_str());
				m_Comms.Notify("ATI_Z", PROFONDEUR);
			}

			else if(param == "SEUIL_DETECTION_SONAR")
			{
				SEUIL_DETECTION_SONAR = atof(value.c_str());
				m_Comms.Notify("ATI_SEUIL_SONAR", SEUIL_DETECTION_SONAR);
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();
	RegisterVariables();
	
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void AroundTheIsland::RegisterVariables()
{
	m_Comms.Register("GO_ISLAND", 0);
	m_Comms.Register("STOP_ISLAND", 0);
	m_Comms.Register("SONAR_RAW_DATA", 0);
	m_Comms.Register("SET_OBSTACLE", 0);
	m_Comms.Register("DIST_MUR", 0);
	
	// Paramètres
	m_Comms.Register("ATI_V_CONV", 0);
	m_Comms.Register("ATI_ANG_EVIT", 0);
	m_Comms.Register("ATI_DUR_EVIT", 0);
	m_Comms.Register("ATI_ANG_CORR", 0);
	m_Comms.Register("ATI_INIT_CAP", 0);
	m_Comms.Register("ATI_V_CONST", 0);
	m_Comms.Register("ATI_Z", 0);
	m_Comms.Register("ATI_NB_MIN_OBST", 0);
	m_Comms.Register("ATI_DIST_LIM_OBST", 0);
	m_Comms.Register("ATI_SEUIL_SONAR", 0);
}
