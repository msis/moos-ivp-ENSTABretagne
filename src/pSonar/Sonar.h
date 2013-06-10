/**
 * \file Sonar.h
 * \brief Classe Sonar
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données liées au sonar
 *
 */

#ifndef Sonar_HEADER
#define Sonar_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class Sonar : public CMOOSApp
{
	public:
		Sonar();
		~Sonar();

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