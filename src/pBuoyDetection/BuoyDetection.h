/**
 * \file BuoyDetection.h
 * \brief Classe BuoyDetection
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de recherche de la bouée
 *
 */

#ifndef BuoyDetection_HEADER
#define BuoyDetection_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class BuoyDetection : public CMOOSApp
{
	public:
		BuoyDetection();
		~BuoyDetection();

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