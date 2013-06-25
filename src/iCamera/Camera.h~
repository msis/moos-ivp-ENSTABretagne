/**
 * \file Camera.h
 * \brief Classe Camera
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données de la caméra orientée vers l'avant
 *
 */

#ifndef Camera_HEADER
#define Camera_HEADER

#include <cv.h>
#include "highgui.h"
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

using namespace std;
using namespace cv;

class Camera : public CMOOSApp
{
	public:
		Camera();
		double getOrientationPipe(IplImage* img_original);
		double median(vector<int> vec);
		~Camera();

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
		bool 			m_server;
		VideoCapture 	m_vc;
		Mat 			m_capture_frame, m_bw_image, m_image;
		IplImage		*channelRed, *channelGreen, *channelBlue, *img_hsv, *img_nb;
};

#endif 