/**
 * \file Mapping.cpp
 * \brief Classe Mapping
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 20th 2013
 *
 * Application MOOS de mapping
 *
 */

#include <clocale>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <string>
#include "MBUtils.h"
#include "Mapping.h"
#include "ColorParse.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Mapping::Mapping()
{
	m_iterations = 0;
	m_timewarp   = 1;
	
	cvNamedWindow("Mapping", 1);
	m_map = Mat::zeros(LARGEUR_MAPPING, HAUTEUR_MAPPING, CV_8UC3);
	m_map = Scalar(255, 255, 255);
	m_img_ciscrea = imread("img_ciscrea_small.png");
	
	int* data_sonar = NULL, nb_data_sonar, angle_data_sonar;
	getDataSonar(data_sonar, &nb_data_sonar, &angle_data_sonar);
	/*
	for(int i = 0 ; i < nb_data_sonar ; i ++)
	{
		line(m_map,
				Point(cos(angle_data_sonar * M_PI / 180.0) * i, sin(angle_data_sonar * M_PI / 180.0) * i),
				Point(cos(angle_data_sonar * M_PI / 180.0) * i, sin(angle_data_sonar * M_PI / 180.0) * i),
				Scalar(161, 149, 104), 1, 8);
	}*/
	
	
	int largeur_img = 37;
	resize(m_img_ciscrea, m_img_ciscrea, Size(largeur_img, largeur_img * m_img_ciscrea.size().height /  m_img_ciscrea.size().width), 0, 0, INTER_LINEAR);

	/*transform(m_img_ciscrea, 
				m_img_ciscrea, 
				getRotationMatrix2D(Point2f(m_img_ciscrea.size().width / 2, m_img_ciscrea.size().height / 2), 30.0, 1.0));*/
	
	Rect roi(Point((LARGEUR_MAPPING - m_img_ciscrea.size().width) / 2, (HAUTEUR_MAPPING - m_img_ciscrea.size().height) / 2), 
				Size(m_img_ciscrea.size().width, m_img_ciscrea.size().height));
	Mat destinationROI = m_map(roi);
	m_img_ciscrea.copyTo(destinationROI);
	
	line(m_map, Point(LARGEUR_MAPPING / 2, (HAUTEUR_MAPPING / 2) - 40), Point(LARGEUR_MAPPING / 2, (HAUTEUR_MAPPING / 2) - 120), Scalar(161, 149, 104), 1, 8, 0);
	line(m_map, Point(LARGEUR_MAPPING / 2, (HAUTEUR_MAPPING / 2) + 40), Point(LARGEUR_MAPPING / 2, (HAUTEUR_MAPPING / 2) + 120), Scalar(161, 149, 104), 1, 8, 0);
	line(m_map, Point((LARGEUR_MAPPING / 2) - 40, HAUTEUR_MAPPING / 2), Point((LARGEUR_MAPPING / 2) - 120, HAUTEUR_MAPPING / 2), Scalar(161, 149, 104), 1, 8, 0);
	line(m_map, Point((LARGEUR_MAPPING / 2) + 40, HAUTEUR_MAPPING / 2), Point((LARGEUR_MAPPING / 2) + 120, HAUTEUR_MAPPING / 2), Scalar(161, 149, 104), 1, 8, 0);
}

/**
 * \fn
 * \brief Récupération des données depuis l'interface du sonar
 */

void Mapping::getDataSonar(int* data, int* nb_data, int* angle_data_sonar)
{
	data[0] = 1;
	data[1] = 200;
	data[2] = 150;
	data[3] = 25;
	
	*nb_data = 4;
	*angle_data_sonar = 30;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Mapping::~Mapping()
{
	//cvReleaseImage(&img);
	Mat map = Mat::zeros(LARGEUR_MAPPING, HAUTEUR_MAPPING, CV_8UC3);
	cvDestroyWindow("Mapping");
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Mapping::OnNewMail(MOOSMSG_LIST &NewMail)
{
	bool nouveau_parametre = false;
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;
		
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
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Mapping::OnConnectToServer()
{
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool Mapping::Iterate()
{
	m_iterations++;
	imshow("Mapping", m_map);
	waitKey(10);
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Mapping::OnStartUp()
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
		}
	}
	
	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Mapping::RegisterVariables()
{
	
}
