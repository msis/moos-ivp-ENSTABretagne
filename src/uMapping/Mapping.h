/**
 * \file Mapping.h
 * \brief Classe Mapping
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de mapping
 *
 */

#ifndef Mapping_HEADER
#define Mapping_HEADER

#include <cv.h>
#include "highgui.h"
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace cv;
using namespace std;

class Mapping : public CMOOSApp
{
	public:
		Mapping();
		~Mapping();

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