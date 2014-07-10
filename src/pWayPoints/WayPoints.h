/**
 * \file WayPoints.h
 * \brief Classe WayPoints
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jul 17th 2013
 *
 * Application MOOS de mission autour d'une île
 *
 */

#ifndef WayPoints_HEADER
#define WayPoints_HEADER

#include <map>
#include <list>
#include <utility>
#include <math.h>
#include "../common/constantes.h"
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "MOOSGeodesy.h"

using namespace std;

struct Point
{
	double x, y;
	int identifiant;
};

struct Vecteur
{
	double x, y;
};

class WayPoints : public CMOOSApp
{
	public:
		WayPoints();
		~WayPoints();

	protected:
		bool OnNewMail(MOOSMSG_LIST &NewMail);
		bool Iterate();
		bool OnConnectToServer();
		bool OnStartUp();
		void RegisterVariables();
		bool waypointDepasse();
		Point make_point(double x, double y);
		Vecteur make_vecteur(Point p1, Point p2);
		void normaliserVecteur(Vecteur &vecteur);
		double produitScalaire(Vecteur v1, Vecteur v2);
		double determinant(Vecteur u, Vecteur v);

	private: // Configuration variables

	private: // State variables
		unsigned int				m_iterations;
		double						m_timewarp;
		int							m_ref_identifiants_points;
		double						m_gps_lat, m_gps_long;
		Point						m_position;
		Point						m_waypoint_precedent;
		list<Point>					m_waypoints;
		CMOOSGeodesy				m_moosgeodesy;
};

#endif 