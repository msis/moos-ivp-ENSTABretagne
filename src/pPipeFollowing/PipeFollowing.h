/**
 * \file PipeFollowing.h
 * \brief Classe PipeFollowing
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 20th 2013
 *
 * Application MOOS de suivi de pipe
 *
 */

#ifndef PipeFollowing_HEADER
#define PipeFollowing_HEADER

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#include "../common/constantes.h"
#include "../common/statistiques.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "regression_lineaire/linreg.h"

#define	DRAWING_THICKNESS					1
#define DRAWING_CONNECTIVITY				8

using namespace std;
using namespace cv;

class PipeFollowing : public CMOOSApp
{
	public:
		PipeFollowing();
		~PipeFollowing();

	protected:
		void updateOrientationPipe();
		double mediane(vector<int> vec);
		double moyenne(vector<int> vec);
		double ecartType(vector<int> vec);
		void rotationImage(IplImage* src, IplImage* dst, double angle);
		void getJonctionsPipe(IplImage* m_img, LinearRegression* linreg, int largeur_pipe, double taux_reconnaissance_pipe); 
		void seuillageTeinteJaune(IplImage* img, int seuil, uchar **data_seuillage, uchar **data_nb);
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();

	private: // Configuration variables

	private: // State variables
		string				m_nom_variable_image;
		IplImage 			*m_img;
		int 				m_largeur_pipe;
		double 				m_taux_reconnaissance_pipe;
		LinearRegression	*m_linreg;
		unsigned int		m_iterations;
		double				m_timewarp;
		IplImage			*channelRed, *channelGreen, *channelBlue, *channelYellow, *img_nb;
		Mat 				m_mat_dst;
		CvScalar 			red, blue, white, green;
		double 				m_intervalle_mise_a_jour;
		
		// Parametres
		double m_param_largeur_max_pipe;
		double m_param_proportion_pipe_non_visible;
		double m_param_corr_coeff_min_detection;
		double m_param_valeur_seuillage;
		double m_param_proportion_points_jonction_pipe;
		double m_param_ecart_type_maximal;
		double m_param_marge_image;
};

#endif 
