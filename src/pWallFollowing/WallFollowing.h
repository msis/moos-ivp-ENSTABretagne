/**
 * \file WallFollowing.h
 * \brief Classe WallFollowing
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de suivi de mur
 *
 */

#ifndef WallFollowing_HEADER
#define WallFollowing_HEADER

#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;

class WallFollowing : public CMOOSApp
{
	public:
		WallFollowing();
		~WallFollowing();

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