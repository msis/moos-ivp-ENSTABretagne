/**
 * \file Ciscrea.h
 * \brief Classe Ciscrea
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS controlant le Ciscrea :
 * récupération des informations sur l'état de l'AUV et fonctions de commande
 *
 */

#ifndef Ciscrea_HEADER
#define Ciscrea_HEADER

#include <vector>
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "../common/constantes.h"
#include "AUV.h"

using namespace std;

class Ciscrea : public CMOOSApp
{
	public:
		Ciscrea(int identifiant_auv = 0);
		~Ciscrea();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		void instancierAUV();

	private: // Configuration variables
		vector<string>	m_listeVariablesSuivies;
		unsigned int	m_iterations;
		double			m_timewarp;

	private: // State variables
		double			m_Vx, m_Vy, m_Vz, m_Rz;
		AUV* 			m_auv_ciscrea;
		int				m_identifiant_auv_a_instancier;
};

#endif 
