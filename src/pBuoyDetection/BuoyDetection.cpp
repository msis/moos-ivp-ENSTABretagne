/**
 * \file BuoyDetection.cpp
 * \brief Classe BuoyDetection
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 25th 2013
 *
 * Application MOOS de recherche de la bouée
 *
 */

#include <clocale>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <string>
#include "MBUtils.h"
#include "BuoyDetection.h"
#include "ColorParse.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
BuoyDetection::BuoyDetection()
{
	m_iterations = 0;
	m_timewarp   = 1;
	
	cvNamedWindow("BuoyDetection", 1);
	
	red = CV_RGB(158, 8, 0);
	blue = CV_RGB(19, 102, 143);
	white = CV_RGB(255, 255, 255);
	green = CV_RGB(30, 153, 65);
	
	m_img = cvCreateImage(cvSize(LARGEUR_IMAGE_CAMERA, HAUTEUR_IMAGE_CAMERA), 8, 3);
	img_nb = cvCreateImage(cvGetSize(m_img), 8, 1);
	
	channelRed = cvCreateImage(cvGetSize(m_img), 8, 1);
	channelGreen = cvCreateImage(cvGetSize(m_img), 8, 1);
	channelBlue = cvCreateImage(cvGetSize(m_img), 8, 1);
	channelOrange = cvCreateImage(cvGetSize(m_img), 8, 1);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

BuoyDetection::~BuoyDetection()
{
	cvReleaseImage(&img_nb);
	cvReleaseImage(&channelRed);
	cvReleaseImage(&channelGreen);
	cvReleaseImage(&channelBlue);
	cvReleaseImage(&channelOrange);
	cvDestroyWindow("BuoyDetection");
}

/**
 * \fn
 * \brief Méthode calculant la position de la bouée dans l'image
 */

void BuoyDetection::updatePositionBouee()
{
	CvPoint pt; // Pour le dessin des points sur l'image
	uchar *data_seuillage = NULL, *data_nb = NULL;
	seuillageTeinteOrange(m_img, m_param_valeur_seuillage, &data_seuillage, &data_nb);
	
	int w = cvGetSize(m_img).width;
	int h = cvGetSize(m_img).height;
	
	position_x_bouee_dans_image = -1;
	position_y_bouee_dans_image = -1;
	
	vector<int> points_tranches, medianes_tranches;
	int nb_points, nb_medianes, mediane_des_points, mediane_des_medianes;
	
	// Recherche de l'abscisse
	nb_medianes = 0;
	for(int j = m_param_marge_image ; j < h - m_param_marge_image ; j++) // h tranches sont étudiées
	{
		nb_points = 0;
		
		for(int i = m_param_marge_image ; i < w - m_param_marge_image ; i ++)
		{
			if(data_seuillage[j * w + i] == 255)
			{
				pt = cvPoint(i, j);
				cvLine(m_img, pt, pt, red, 1, DRAWING_CONNECTIVITY);
				points_tranches.push_back(i);
				nb_points ++;
			}
		}
		
		if(nb_points != 0 && Statistiques::ecartType(points_tranches) < m_param_ecart_type_maximal)
		{
			mediane_des_points = Statistiques::mediane(points_tranches);
			medianes_tranches.push_back(mediane_des_points);
			points_tranches.clear();
			nb_medianes ++;
		}
	}
	
	if(nb_medianes != 0 && Statistiques::ecartType(medianes_tranches))
	{
		mediane_des_medianes = Statistiques::mediane(medianes_tranches);
		position_x_bouee_dans_image = mediane_des_medianes;
		medianes_tranches.clear();
	}
	
	// Recherche de l'ordonnée
	nb_medianes = 0;
	for(int j = m_param_marge_image ; j < w - m_param_marge_image ; j++) // h tranches sont étudiées
	{
		nb_points = 0;
		
		for(int i = m_param_marge_image ; i < h - m_param_marge_image ; i ++)
		{
			if(data_seuillage[i * w + j] == 255)
			{
				pt = cvPoint(j, i);
				cvLine(m_img, pt, pt, red, 1, DRAWING_CONNECTIVITY);
				points_tranches.push_back(i);
				nb_points ++;
			}
		}
		
		if(nb_points != 0 && Statistiques::ecartType(points_tranches) < m_param_ecart_type_maximal)
		{
			mediane_des_points = Statistiques::mediane(points_tranches);
			medianes_tranches.push_back(mediane_des_points);
			points_tranches.clear();
			nb_medianes ++;
		}
	}
	
	if(nb_medianes != 0 && Statistiques::ecartType(medianes_tranches))
	{
		mediane_des_medianes = Statistiques::mediane(medianes_tranches);
		position_y_bouee_dans_image = mediane_des_medianes;
		medianes_tranches.clear();
	}
	
	// La croix pointant sur le centre de la bouée :
	cvLine(m_img, 
		cvPoint(position_x_bouee_dans_image, 0),
		cvPoint(position_x_bouee_dans_image, h),
		green, 2, DRAWING_CONNECTIVITY);
		
	cvLine(m_img, 
		cvPoint(0, position_y_bouee_dans_image),
		cvPoint(w, position_y_bouee_dans_image),
		green, 2, DRAWING_CONNECTIVITY);
}

/**
 * \fn
 * \brief Méthode effectuant un seuillage sur les composantes oranges de l'image
 */
 
void BuoyDetection::seuillageTeinteOrange(IplImage* img, int seuil, uchar **data_seuillage, uchar **data_nb)
{
	IplImage* imgHSV = cvCreateImage(cvGetSize(img), 8, 3);
	cvCvtColor(img, imgHSV, CV_BGR2HSV);
	cvInRangeS(imgHSV, cvScalar(5, 70, 20), cvScalar(30, 255, 255), channelOrange);
	*data_seuillage = (uchar *)channelOrange->imageData;
	*data_nb = (uchar *)img_nb->imageData;
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool BuoyDetection::OnNewMail(MOOSMSG_LIST &NewMail)
{
	bool nouveau_parametre = false;
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		if(msg.GetKey() == m_nom_variable_image)
		{
			if((int)msg.GetString().size() == m_img->imageSize)
				memcpy(m_img->imageData, msg.GetString().data(), m_img->imageSize);
			
			else
				cout << "Erreur : mauvaises dimensions dans la variable image \"" << m_nom_variable_image << "\" depuis la MOOSDB" << endl;
			
			updatePositionBouee();
			cvShowImage("BuoyDetection", m_img);
			waitKey(10);
		}

		if(msg.GetKey() == "BUOYDETECTION__VALEUR_SEUILLAGE")
		{
			m_param_valeur_seuillage = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "BUOYDETECTION__ECART_TYPE_MAXIMAL")
		{
			m_param_ecart_type_maximal = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "BUOYDETECTION__MARGE_IMAGE")
		{
			m_param_marge_image = msg.GetDouble();
			nouveau_parametre = true;
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
	
	if(nouveau_parametre)
	{
		cout << endl << "Paramètres du traitement :" << endl;
		cout << "\tBUOYDETECTION__VALEUR_SEUILLAGE : \t\t\t" << m_param_valeur_seuillage << endl;
		cout << "\tBUOYDETECTION__ECART_TYPE_MAXIMAL : \t\t\t" << m_param_ecart_type_maximal << endl;
		cout << "\tBUOYDETECTION__MARGE_IMAGE : \t\t\t\t" << m_param_marge_image << endl;
	}
		
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool BuoyDetection::OnConnectToServer()
{
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool BuoyDetection::Iterate()
{
	m_iterations++;
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool BuoyDetection::OnStartUp()
{
	setlocale(LC_ALL, "C");
	m_intervalle_mise_a_jour = 0;
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
			
			// Paramètres généraux sur l'application
			if(param == "VARIABLE_IMAGE_NAME")
				m_nom_variable_image = value;

			if(param == "TIME_INTERVAL")
				m_intervalle_mise_a_jour = atof(value.c_str());
			
			// Paramètres du traitement d'image
			if(param == "BUOYDETECTION__VALEUR_SEUILLAGE")
				m_param_valeur_seuillage = atof(value.c_str());

			if(param == "BUOYDETECTION__ECART_TYPE_MAXIMAL")
				m_param_ecart_type_maximal = atof(value.c_str());

			if(param == "BUOYDETECTION__MARGE_IMAGE")
				m_param_marge_image = atof(value.c_str());
		}
	}
	
	cout << termColor("blue") << "Analyse de \"" << m_nom_variable_image << "\" ; interv = " << m_intervalle_mise_a_jour << termColor() << endl;
	m_timewarp = GetMOOSTimeWarp();

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void BuoyDetection::RegisterVariables()
{
	m_Comms.Register(m_nom_variable_image, m_intervalle_mise_a_jour);
	m_Comms.Register("BUOYDETECTION__VALEUR_SEUILLAGE", 0);
	m_Comms.Register("BUOYDETECTION__ECART_TYPE_MAXIMAL", 0);
	m_Comms.Register("BUOYDETECTION__MARGE_IMAGE", 0);
}
