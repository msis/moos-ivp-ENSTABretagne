/**
 * \file BottomCamera.h
 * \brief Classe BottomCamera
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 7th 2013
 *
 * Application MOOS récupérant les données de la caméra orientée vers le fond
 *
 */

#ifndef BottomCamera_HEADER
#define BottomCamera_HEADER

#include "MOOS/libMOOS/App/MOOSApp.h"
#include "../common/constantes.h"

using namespace std;

class BottomCamera : public CMOOSApp
{
	public:
		BottomCamera();
		~BottomCamera();

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