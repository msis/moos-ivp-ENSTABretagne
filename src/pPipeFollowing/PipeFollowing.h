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

#include <cv.h>
#include "highgui.h"
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "regression_lineaire/linreg.h"

#define LARGEUR_MAX_PIPE				250
#define PROPORTION_PIPE_NON_VISIBLE		0.2
#define CORR_COEFF_MIN_DETECTION		0.4
#define VALEUR_SEUILLAGE				100
#define	DRAWING_THICKNESS				1
#define DRAWING_CONNECTIVITY			8
#define PROPORTION_POINTS_JONCTION_PIPE	0.6

using namespace std;
using namespace cv;

class PipeFollowing : public CMOOSApp
{
	public:
		PipeFollowing();
		void getOrientationPipe(IplImage* img_original, LinearRegression* linreg, int* largeur_pipe, double* taux_reconnaissance_pipe);
		double median(vector<int> vec);
		void rotationImage(IplImage* src, IplImage* dst, double angle);
		void getJonctionsPipe(IplImage* img_original, LinearRegression* linreg, int largeur_pipe, double taux_reconnaissance_pipe); 
		void seuillageTeinteJaune(IplImage* img_original, int seuil, uchar **data_seuillage, uchar **data_nb);
		~PipeFollowing();

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
		IplImage		*channelRed, *channelGreen, *channelBlue, *img_hsv, *img_nb;
		Mat 			m_mat_dst;
		CvScalar 		red, blue, white;
};

#endif 