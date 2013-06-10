/**
 * \file Hydrophone.h
 * \brief Classe Hydrophone
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données de l'hydrophone
 *
 */

#ifndef Hydrophone_HEADER
#define Hydrophone_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class Hydrophone : public CMOOSApp
{
	public:
		Hydrophone();
		~Hydrophone();

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