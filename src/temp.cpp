/**
 * \file Camera.cpp
 * \brief Classe Camera
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Application MOOS récupérant les données de la caméra orientée vers l'avant
 *
 */

#include <iterator>
#include <iostream>
#include <iomanip>
#include "MBUtils.h"
#include "Camera.h"


/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Camera::Camera()
{
	m_iterations = 0;
	m_timewarp   = 1;
	channelRed = cvCreateImage(cvSize(320, 240), 8, 1);
	channelGreen = cvCreateImage(cvSize(320, 240), 8, 1);
	channelBlue = cvCreateImage(cvSize(320, 240), 8, 1);
	img_nb = cvCreateImage(cvSize(320, 240), 8, 1);
	img_hsv = cvCreateImage(cvSize(320, 240), 8, 3);
	namedWindow((char*)m_image_name.c_str(), CV_WINDOW_AUTOSIZE);
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

Camera::~Camera()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool Camera::OnNewMail(MOOSMSG_LIST &NewMail)
{
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Camera::OnConnectToServer()
{
	RegisterVariables();
	Register("Image", 0.0);
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée automatiquement périodiquement
 * Implémentation du comportement de l'application
 */
 
bool Camera::Iterate()
{
	if(m_vc_v4l2.read(m_capture_frame))
	{
		//Notify((char*)(m_image_name).c_str(), (void*)m_image.data, m_image.rows()*m_image.step(), MOOSLocalTime());
		IplImage ipl_img = m_capture_frame;
		Notify((char*)(m_image_name).c_str(), (void*)ipl_img.imageData, ipl_img.imageSize, MOOSLocalTime());
		imshow((char*)m_image_name.c_str(), m_capture_frame);
	}
	
	else
		cout << "No frame grabbed." << endl;
		
	waitKey(10);
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool Camera::OnStartUp()
{
	int identifiant_camera = -1;
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

			if(param == "IDENTIFIANT_CV_CAMERA")
				identifiant_camera = atoi(value.c_str());

			if(param == "VARIABLE_IMAGE_NAME")
				m_image_name = value;
		}
	}

	m_timewarp = GetMOOSTimeWarp();
	SetAppFreq(20, 400);
	SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);
	string device_name = "/dev/video" + identifiant_camera;

	if(identifiant_camera == -1)
	{
		cout << "Aucun identifiant de caméra reconnu" << endl;
		return false;
	}

	if(!m_vc_v4l2.open(device_name, 320, 240))
		return false;

	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Camera::RegisterVariables()
{
	
}
