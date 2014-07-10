/**
 * \file WayPoints.cpp
 * \brief Classe WayPoints
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jul 17th 2013
 *
 * Application MOOS de mission autour d'une île
 *
 */

#include <iterator>
#include <algorithm>
#include "MBUtils.h"
#include "WayPoints.h"
#include "ColorParse.h"

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
WayPoints::WayPoints()
{
	m_iterations = 0;
	m_timewarp   = 1;
	m_ref_identifiants_points = 1;
	
	m_moosgeodesy.Initialise(48.303131, -4.537218);

	// Parcours large autour de l'île
	/*m_waypoints.push_back(make_point(-4.534274881126787,48.30328123684544));
	m_waypoints.push_back(make_point(-4.534354338163467,48.3028926627652));
	m_waypoints.push_back(make_point(-4.534987862732912,48.30265745068134));
	m_waypoints.push_back(make_point(-4.534912811622956,48.30172799501912));
	m_waypoints.push_back(make_point(-4.535541337401179,48.30144781440489));
	m_waypoints.push_back(make_point(-4.536623557565463,48.30171121108806));
	m_waypoints.push_back(make_point(-4.538187294636643,48.3015653100484));
	m_waypoints.push_back(make_point(-4.539082024594746,48.30173802893376));
	m_waypoints.push_back(make_point(-4.539520934204093,48.302301986646));
	m_waypoints.push_back(make_point(-4.539370088840833,48.30346048022233));
	m_waypoints.push_back(make_point(-4.538118344474873,48.30466090900167));
	m_waypoints.push_back(make_point(-4.536681443170951,48.30492727178057));
	m_waypoints.push_back(make_point(-4.535945459593399,48.30479686179294));
	m_waypoints.push_back(make_point(-4.535589168541877,48.30434275663863));
	m_waypoints.push_back(make_point(-4.535439601902204,48.3037517878406));
	m_waypoints.push_back(make_point(-4.534638383926079,48.30352082998913));
	m_waypoints.push_back(make_point(-4.534274881126787,48.30328123684544));*/
	
	// Parcours proche de l'île
	m_waypoints.push_back(make_point(-4.534590163338356,48.30323621201908));
	m_waypoints.push_back(make_point(-4.53478762216008,48.30334651838246));
	m_waypoints.push_back(make_point(-4.535338202882623,48.30326872403582));
	m_waypoints.push_back(make_point(-4.535709627194199,48.30335301044348));
	m_waypoints.push_back(make_point(-4.535923558290338,48.3038853028886));
	m_waypoints.push_back(make_point(-4.535892651305976,48.30424405590262));
	m_waypoints.push_back(make_point(-4.536055327369376,48.30454539251438));
	m_waypoints.push_back(make_point(-4.536493267983829,48.30473876429819));
	m_waypoints.push_back(make_point(-4.537175041209843,48.30462322599557));
	m_waypoints.push_back(make_point(-4.537865910908123,48.30450112394015));
	m_waypoints.push_back(make_point(-4.538193579521336,48.30431361797088));
	m_waypoints.push_back(make_point(-4.539054088257123,48.30339799457749));
	m_waypoints.push_back(make_point(-4.539203448805972,48.30307094254877));
	m_waypoints.push_back(make_point(-4.539270962811838,48.30218588442434));
	m_waypoints.push_back(make_point(-4.538953658010474,48.30191794661709));
	m_waypoints.push_back(make_point(-4.538187539710014,48.30174497426677));
	m_waypoints.push_back(make_point(-4.536933709104124,48.30201349870482));
	m_waypoints.push_back(make_point(-4.535811260639849,48.30167755409671));
	m_waypoints.push_back(make_point(-4.535348754982527,48.30173218255946));
	m_waypoints.push_back(make_point(-4.535488634255342,48.30278271885528));
	m_waypoints.push_back(make_point(-4.535270044927863,48.30293684926692));
	m_waypoints.push_back(make_point(-4.534597992379511,48.30303867953135));
	m_waypoints.push_back(make_point(-4.534590163338356,48.30323621201908));
	
	double nord, est;
	for(list<Point>::iterator it = m_waypoints.begin() ; it != m_waypoints.end() ; it ++)
		m_moosgeodesy.LatLong2LocalUTM(it->y, it->x, it->y, it->x);
	
	// Tests
	/*m_waypoints.push_back(make_point(11,3));
	m_waypoints.push_back(make_point(3,3));*/
	
	m_waypoint_precedent = m_waypoints.front();
	m_position = m_waypoint_precedent;
	m_waypoints.pop_front();
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

WayPoints::~WayPoints()
{
	
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool WayPoints::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;
		if(msg.GetSource() == GetAppName())
			continue;
		
		if(msg.GetKey() == "GPS_LAT")
			m_gps_lat = msg.GetDouble();
		
		if(msg.GetKey() == "GPS_LON")
			m_gps_long = msg.GetDouble();
		
		#if 0 // Keep these around just for template
			string key   = msg.GetKey();
			string comm  = msg.GetCommunity();
			double dval  = msg.GetDouble();
			string sval  = msg.GetString(); 
			string msrc  = msg.GetSource();
			double mtime = msg.GetTime();
			bool   mdbl  = msg.IsDouble();
			bool   mstr  = msg.IsString();
		#endif
	}
	
	m_moosgeodesy.LatLong2LocalUTM(m_gps_lat, m_gps_long, m_position.y, m_position.x);
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool WayPoints::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Création d'un point
 */

Point WayPoints::make_point(double x, double y)
{
	Point p;	p.x = x;	p.y = y;
	p.identifiant = m_ref_identifiants_points;
	m_ref_identifiants_points ++;
	return p;
}

/**
 * \fn
 * \brief Création d'un vecteur
 */

Vecteur WayPoints::make_vecteur(Point p1, Point p2)
{
	Vecteur v;	v.x = p2.x - p1.x;	v.y = p2.y - p1.y;
	return v;
}

/**
 * \fn
 * \brief Méthode normalisant un vecteur
 */
 
void WayPoints::normaliserVecteur(Vecteur &v)
{
	double denom = sqrt(v.x*v.x + v.y*v.y);
	v.x /= denom;
	v.y /= denom;
}

/**
 * \fn
 * \brief Méthode calculant un produit scalaire
 */
 
double WayPoints::produitScalaire(Vecteur v1, Vecteur v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}

/**
 * \fn
 * \brief Méthode calculant un déterminant
 */
 
double WayPoints::determinant(Vecteur u, Vecteur v)
{
	return u.x*v.y - v.x*u.y;
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool WayPoints::Iterate()
{
	if(!m_waypoints.empty())
	{
		// Mise à jour des waypoints :
		
		while(!m_waypoints.empty() && waypointDepasse())
		{
			m_waypoint_precedent = m_waypoints.front();
			cout << termColor("red") << "Waypoint " << m_waypoint_precedent.identifiant << " (" << m_waypoint_precedent.x << "," << m_waypoint_precedent.y << ") dépassé" << termColor() << endl;
			m_waypoints.pop_front();
		}
		
		if(m_waypoints.empty())
		{
			m_Comms.Notify("VVV_Z_DESIRED", 0.);
			m_Comms.Notify("VVV_VX_DESIRED", 0.);
			cout << endl << "Fin de la mission" << endl << endl;
		}
		
		else
		{
			// Suivi de ligne :
			
			// 1.
			Vecteur vAB = make_vecteur(m_waypoint_precedent, m_waypoints.front());
			Vecteur vMB = make_vecteur(m_position, m_waypoints.front());
			Vecteur vAM = make_vecteur(m_waypoint_precedent, m_position);
			normaliserVecteur(vAB);
			double e = determinant(vAB, vAM);
				// si e > 0, robot à gauche de la ligne
				// si e < 0, robot à droite de la ligne
			
			// 3.
			double phi = atan2(vAB.y, vAB.x);
			
			// 4.
			double r = 5.; // Précision GPS, pour rejoindre la ligne
			double gamma_inf = M_PI/4.;
			double theta = phi - (2.*gamma_inf/M_PI) * atan(e/r);
			
			m_Comms.Notify("VVV_HEADING_DESIRED", 180. - MOOSRad2Deg(theta));
				// theta = 0 		--> direction Est
				// theta = PI/2 	--> direction Nord
				
			cout << endl << m_waypoints.front().identifiant << endl;
			cout << "Distance de la ligne : \t" << e << endl;
			cout << "Distance du waypoint : \t" << sqrt(vMB.x*vMB.x + vMB.y*vMB.y) << endl << endl;
		}
	}
	
	return(true);
}

/**
 * \fn
 * \brief Méthode testant le dépassement d'un waypoint
 */
 
bool WayPoints::waypointDepasse()
{
	Vecteur vBA = make_vecteur(m_waypoints.front(), m_waypoint_precedent);
	normaliserVecteur(vBA);
	Vecteur vBM = make_vecteur(m_waypoints.front(), m_position);
	normaliserVecteur(vBM);
	
	return produitScalaire(vBA, vBM) < 0;
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool WayPoints::OnStartUp()
{
	double profondeur = 2., vx = 20.;
	setlocale(LC_ALL, "C");
	list<string> sParams;
	m_MissionReader.EnableVerbatimQuoting(false);
	if(m_MissionReader.GetConfiguration(GetAppName(), sParams))
	{
		list<string>::iterator p;
		for(p = sParams.begin() ; p != sParams.end() ; p++)
		{
			string original_line = *p;
			string param = stripBlankEnds(toupper(biteString(*p, '=')));
			string value = stripBlankEnds(*p);
			
			if(param == "Z")
				profondeur = atof(value.c_str());
			
			if(param == "VX")
				vx = atof(value.c_str());
		}
	}

	m_timewarp = GetMOOSTimeWarp();
	RegisterVariables();
	
	// Commandes constantes
	m_Comms.Notify("VVV_Z_DESIRED", profondeur);
	m_Comms.Notify("VVV_VX_DESIRED", vx);
	cout << termColor("blue") << "Lancement du parcours...\n\tZ  = \t" << profondeur << "\n\tVX = \t" << vx << termColor() << endl << endl;
	
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void WayPoints::RegisterVariables()
{
	m_Comms.Register("GPS_LAT", 0);
	m_Comms.Register("GPS_LON", 0);
}
