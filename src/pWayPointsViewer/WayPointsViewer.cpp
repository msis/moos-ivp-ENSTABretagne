/**
 * \file WayPointsViewer.cpp
 * \brief Classe WayPointsViewer
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de suivi de mur
 *
 */

#include <iterator>
#include <algorithm>
#include "MBUtils.h"
#include "WayPointsViewer.h"

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
WayPointsViewer::WayPointsViewer()
{
	m_iterations = 0;
	m_timewarp   = 1;
	m_ref_identifiants_points = 1;
	
	m_centre_ile_px.x = 245;
	m_centre_ile_px.y = 260;
	
	m_moosgeodesy.Initialise(48.303131, -4.537218);

	// Parcours large
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

	// Prcours proche de l'île
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
	
	m_position = make_point(-4.534590163338356,48.30323621201908);
	m_moosgeodesy.LatLong2LocalGrid(m_position.y, m_position.x, m_position.y, m_position.x);
	
	double nord, est;
	for(list<Point2D>::iterator it = m_waypoints.begin() ; it != m_waypoints.end() ; it ++)
		m_moosgeodesy.LatLong2LocalGrid(it->y, it->x, it->y, it->x);
	
	cvNamedWindow("Mapping", 1);
	m_map_background = imread("background_ile.png", CV_LOAD_IMAGE_COLOR);
	
	// Centre de l'île
	tracerPointeur(m_map_background, m_centre_ile_px.x, m_centre_ile_px.y, 20, 1, Scalar(208, 222, 223));
	
	// Echelle
	double a = metresEnPixels(50.);
	line(m_map_background, 
			Point(10, HAUTEUR_MAPPING - 10), 
			Point(10 + a, HAUTEUR_MAPPING - 10), 
			Scalar(255, 255, 255), 1, 8, 0);
	line(m_map_background, 
			Point(10, HAUTEUR_MAPPING - 10), 
			Point(10, HAUTEUR_MAPPING - 13), 
			Scalar(255, 255, 255), 1, 8, 0);
	line(m_map_background, 
			Point(10 + a, HAUTEUR_MAPPING - 10), 
			Point(10 + a, HAUTEUR_MAPPING - 13), 
			Scalar(255, 255, 255), 1, 8, 0);
	
	// Parcours à suivre
	for(list<Point2D>::iterator it = m_waypoints.begin() ; it != m_waypoints.end() ; it ++)
	{
		if(it->identifiant == 1)
			continue;
		
		char texte[10];
		int x, y, delta_x, delta_y;
		x = m_centre_ile_px.x + metresEnPixels(it->x);
		y = m_centre_ile_px.y - metresEnPixels(it->y);
		sprintf(texte, "%d", it->identifiant);
		
		if(x > m_centre_ile_px.x)
			delta_x = 4;
		
		else
			delta_x = -(10 * strlen(texte));
		
		if(y > m_centre_ile_px.y)
			delta_y = 10;
		
		else
			delta_y = -5;
		
		tracerPointeur(m_map_background, x, y, 5, 1, Scalar(243, 109, 77));
		putText(m_map_background, string(texte), 
					Point(x + delta_x, y + delta_y), 
					FONT_HERSHEY_SIMPLEX, 
					0.3, 
					Scalar(243, 109, 77));
	}
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

WayPointsViewer::~WayPointsViewer()
{
	Mat map = Mat::zeros(LARGEUR_MAPPING, HAUTEUR_MAPPING, CV_8UC3);
	cvDestroyWindow("Mapping");
}

/**
 * \fn
 * \brief Création d'un point
 */

Point2D WayPointsViewer::make_point(double x, double y)
{
	Point2D p;	p.x = x;	p.y = y;
	p.identifiant = m_ref_identifiants_points;
	m_ref_identifiants_points ++;
	return p;
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool WayPointsViewer::OnNewMail(MOOSMSG_LIST &NewMail)
{
	bool gps_update = false;
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		if(msg.GetKey() == "VVV_HEADING_DESIRED")
			m_heading = msg.GetDouble();
		
		else if(msg.GetKey() == "GPS_LAT")
		{
			m_gps_lat = msg.GetDouble();
			gps_update = true;
		}
		
		else if(msg.GetKey() == "GPS_LON")
		{
			m_gps_long = msg.GetDouble();
			gps_update = true;
		}
		
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
	
	if(gps_update)
		m_moosgeodesy.LatLong2LocalGrid(m_gps_lat, m_gps_long, m_position.y, m_position.x);

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool WayPointsViewer::OnConnectToServer()
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
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool WayPointsViewer::Iterate()
{
	m_iterations ++;
	Mat map = m_map_background.clone();
	
	// Position actuelle 
	tracerPointeur(m_map_background, 
					m_centre_ile_px.x + metresEnPixels(m_position.x), 
					m_centre_ile_px.y - metresEnPixels(m_position.y), 
					0, 1, Scalar(57, 221, 251)); // trace
	tracerPointeur(map, 
					m_centre_ile_px.x + metresEnPixels(m_position.x), 
					m_centre_ile_px.y - metresEnPixels(m_position.y), 
					8, 1, Scalar(57, 221, 251)); // curseur
					
	double heading_map = 180 - m_heading;
	double longueur_ligne = metresEnPixels(30.);
	line(map, 
			Point(m_centre_ile_px.x + metresEnPixels(m_position.x), 
					m_centre_ile_px.y - metresEnPixels(m_position.y)), 
			Point(m_centre_ile_px.x + metresEnPixels(m_position.x) + cos(MOOSDeg2Rad(heading_map))*longueur_ligne, 
					m_centre_ile_px.y - metresEnPixels(m_position.y) - sin(MOOSDeg2Rad(heading_map))*longueur_ligne), 
			Scalar(200, 200, 200), 1, 8, 0);
	
	imshow("Mapping", map);
	waitKey(1);
	
	return(true);
}

/**
 * \fn
 * \brief Remise à l'échelle d'une position sur la carte
 */

double WayPointsViewer::metresEnPixels(double c)
{
	return 255. * c / 306.;
}

/**
 * \fn
 * \brief Méthode affichant un pointeur sur la carte
 */
 
void WayPointsViewer::tracerPointeur(Mat m, int x, int y, int taille_pointeur, int epaisseur, Scalar couleur)
{
	line(m, Point(x, y - taille_pointeur), Point(x, y + taille_pointeur), couleur, epaisseur, 8, 0);
	line(m, Point(x - taille_pointeur, y), Point(x + taille_pointeur, y), couleur, epaisseur, 8, 0);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool WayPointsViewer::OnStartUp()
{
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

			if(param == "FOO")
			{
				//handled
			}
			
			else if(param == "BAR")
			{
				//handled
			}
		}
	}

	m_timewarp = GetMOOSTimeWarp();
	RegisterVariables();
	
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void WayPointsViewer::RegisterVariables()
{
	m_Comms.Register("VVV_HEADING_DESIRED", 0);
	m_Comms.Register("GPS_LON", 0);
	m_Comms.Register("GPS_LAT", 0);
	m_Comms.Register("VVV_HEADING", 0);
}
