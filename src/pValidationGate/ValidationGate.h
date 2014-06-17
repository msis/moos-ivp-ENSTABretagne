/**
 * \file ValidationGate.h
 * \brief Classe ValidationGate
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jul 16th 2013
 *
 * Application MOOS de passage de gate
 *
 */

#ifndef ValidationGate_HEADER
#define ValidationGate_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class ValidationGate : public CMOOSApp
{
	public:
		ValidationGate();
		~ValidationGate();

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
		bool			m_lancer_mission;
};

#endif 