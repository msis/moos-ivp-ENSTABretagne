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

#include <cv.h>
#include "highgui.h"
#include "../common/constantes.h"
#include "../common/statistiques.h"
#include "MOOS/libMOOS/App/MOOSApp.h"

#define	DRAWING_THICKNESS					1
#define DRAWING_CONNECTIVITY				8

using namespace std;
using namespace cv;

class BuoyDetection : public CMOOSApp
{
	public:
		BuoyDetection();
		~BuoyDetection();

	protected:
		double updatePositionBouee();
		void seuillageTeinteOrange(IplImage* img, int seuil, uchar **data_seuillage, uchar **data_nb);
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		string			m_nom_variable_image;
		IplImage 		*m_img;
		unsigned int	m_iterations;
		double			m_timewarp;
		double			position_x_bouee_dans_image, position_y_bouee_dans_image;
		IplImage		*channelRed, *channelGreen, *channelBlue, *channelOrange, *img_nb;
		CvScalar 		red, blue, white, green;
		double 			m_intervalle_mise_a_jour;
		double			m_heading_actuel;
		
		// Parametres
		double m_param_valeur_seuillage;
		double m_param_ecart_type_maximal;
		double m_param_marge_image;
};

#endif 