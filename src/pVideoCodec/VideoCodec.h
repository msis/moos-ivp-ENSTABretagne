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
	  
private:
  struct NamedAndTimedData {
    std::string name;
    double time;
  };
  struct ImageData : public NamedAndTimedData {
    cv::Mat img;
  };
  struct JpegData : public NamedAndTimedData {
    std::vector<uchar> buf;
  };
  
  unsigned int getImageDataIndexForVar(const std::string& varName);
  unsigned int getJpegDataIndexForVar(const std::string& varName);

	private: // State variables
		std::string			m_display_name;
		unsigned int	m_iterations;
		double			m_timewarp;
		bool			m_decode;
		
		bool			m_affichage_image;
		cv::Mat imgRaw;
		std::vector<uchar> bufComp;
		std::vector< ImageData > m_img;
		std::vector< JpegData > m_buf;
		unsigned int m_nb_img, m_nb_buf;
};

#endif 
