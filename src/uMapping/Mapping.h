/**
 * \file Mapping.h
 * \brief Classe Mapping
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 20th 2013
 *
 * Application MOOS de mapping
 *
 */

#ifndef Mapping_HEADER
#define Mapping_HEADER

#include <cv.h>
#include "highgui.h"
#include "../common/constantes.h"
#include "../common/statistiques.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

#define LARGEUR_MAPPING		500
#define HAUTEUR_MAPPING		500

using namespace std;
using namespace cv;

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
		void getDataSonar(int* data, int* nb_data, int* angle_data_sonar);

	private: // Configuration variables

	private: // State variables
		unsigned int		m_iterations;
		double				m_timewarp;
		Mat					m_map, m_img_ciscrea;
};

#endif 
