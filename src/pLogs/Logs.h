/**
 * \file Logs.h
 * \brief Classe Logs
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS enregistrant les logs des missions de l'AUV
 *
 */

#ifndef Logs_HEADER
#define Logs_HEADER

#include <vector>
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class Logs : public CMOOSApp
{
	public:
		Logs();
		~Logs();
		bool creerFichierLogs();
		bool enregistrerPositionLogs();
		bool supprimerFichierLogs();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		string getCheminCompletDuFichier();

	private: // Configuration variables
		bool			m_generer_logs;
		bool 			m_auv_connecte;
		bool			m_fichier_logs_cree;
		vector<string>	m_listeVariablesSuivies;
		unsigned int	m_iterations;
		double			m_timewarp;
		string 			m_nom_fichier;
		int				m_position_auv_x;
		int				m_position_auv_y;
		int				m_position_auv_z;

	private: // State variables
};

#endif 