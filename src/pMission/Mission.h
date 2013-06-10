/**
 * \file Mission.h
 * \brief Classe Mission
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 10th 2013
 * 
 * Ordonnancement de la mission que l'AUV doit réaliser
 * (se base sur la lecture d'un fichier de mission)
 */

#ifndef Mission_HEADER
#define Mission_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class Mission : public CMOOSApp
{
	public:
		Mission(string nomFichierMission);
		~Mission();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		unsigned int	m_iterations;
		double			m_timewarp;
};

#endif 