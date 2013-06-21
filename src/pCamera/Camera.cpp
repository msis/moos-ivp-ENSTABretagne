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
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		if(p->IsName("Image"))
		{
			cerr << "bytes : " << p->GetBinaryDataSize() << "\tlatency : " <<
			setprecision(3) << (MOOSLocalTime() - p->GetTime()) * 1e3 << "ms\r";
			memcpy(m_image.data, p->GetBinaryData(), p->GetBinaryDataSize());
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
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool Camera::OnConnectToServer()
{
	// register for variables here
	// possibly look at the mission file?
	// m_MissionReader.GetConfigurationParam("Name", <string>);
	// m_Comms.Register("VARNAME", 0);

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
	m_vc >> m_capture_frame;
	cvtColor(m_capture_frame, m_bw_image, CV_BGR2GRAY);
	resize(m_bw_image, m_image, m_image.size(), 0, 0, INTER_NEAREST);
	Notify("Image", (void*)m_image.data, m_image.size().area(), MOOSLocalTime());
	
	imshow("display", m_image);
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
				identifiant_camera = atoi((char*)value.c_str());
		}
	}

	m_timewarp = GetMOOSTimeWarp();

	SetAppFreq(20, 400);
	SetIterateMode(COMMS_DRIVEN_ITERATE_AND_MAIL);
	m_image = Mat(378, 512, CV_8UC1);

	if(identifiant_camera == -1)
	{
		cout << "Aucun identifiant de caméra reconnu" << endl;
		return false;
	}
	
	if(!m_vc.open(identifiant_camera))
		return false;
		
	namedWindow("display", 1);
	
	RegisterVariables();
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void Camera::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}
