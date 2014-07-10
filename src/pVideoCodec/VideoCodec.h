/**
 * \file VideoCodec.h
 * \brief Classe VideoCodec
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données de la caméra orientée vers l'avant
 *
 */

#ifndef VideoCodec_HEADER
#define VideoCodec_HEADER

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

class VideoCodec : public CMOOSApp
{
	public:
		VideoCodec();
		~VideoCodec();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		std::string			m_image_name, m_display_name;
		unsigned int	m_iterations;
		double			m_timewarp;
		bool			m_decode;
		
		bool			m_affichage_image;
		cv::Mat imgRaw;
		std::vector<uchar> bufComp;
};

#endif 
