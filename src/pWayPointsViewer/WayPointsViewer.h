/**
 * \file WayPointsViewer.h
 * \brief Classe WayPointsViewer
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de suivi de mur
 *
 */

#ifndef WayPointsViewer_HEADER
#define WayPointsViewer_HEADER

#include <map>
#include <list>
#include <utility>
#include <math.h>
#include <cv.h>
#include "highgui.h"
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "MOOS/libMOOSGeodesy/MOOSGeodesy.h"

#define LARGEUR_MAPPING		500
#define HAUTEUR_MAPPING		500

using namespace cv;
using namespace std;

struct Point2D
{
	double x, y;
	int identifiant;
};

class WayPointsViewer : public CMOOSApp
{
	public:
		WayPointsViewer();
		~WayPointsViewer();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		void tracerPointeur(Mat m, int x, int y, int taille_pointeur, int epaisseur, Scalar couleur);
		double metresEnPixels(double c);
		Point2D make_point(double x, double y);

	private: // Configuration variables

	private: // State variables
		unsigned int	m_iterations;
		double			m_timewarp;
		Mat				m_map_background;
		double			m_heading;
		double			m_gps_lat, m_gps_long;
		int				m_ref_identifiants_points;
		Point2D			m_position;
		Point2D			m_centre_ile_px;
		list<Point2D>	m_waypoints;
		CMOOSGeodesy	m_moosgeodesy;
};

#endif 