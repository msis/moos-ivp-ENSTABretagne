/**
 * \file VideoCodec.cpp
 * \brief Classe VideoCodec
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
#include "VideoCodec.h"

using namespace std;
using namespace cv;


/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
VideoCodec::VideoCodec()
{
	m_iterations = 0;
	m_timewarp   = 1;
	imgRaw.create(HAUTEUR_IMAGE_CAMERA, LARGEUR_IMAGE_CAMERA,  CV_8UC3);
	bufComp.reserve(200000); // reserve 200kB for compressed data
	m_affichage_image = false;
	m_decode = false;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

VideoCodec::~VideoCodec()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool VideoCodec::OnNewMail(MOOSMSG_LIST &NewMail)
{
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
		
		
		if (MOOSWildCmp("VVV_IMAGE_*", msg.GetName())
		  && !m_decode)
		{
		  //cout << "msg name " << msg.GetName() << endl;
		  memcpy(imgRaw.data, msg.GetBinaryData(), msg.GetBinaryDataSize());
		  vector<int> encode_params;
		  encode_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		  encode_params.push_back(60);
		  cv::imencode(".jpg",imgRaw,bufComp,encode_params);
		  string pubName = "VCODEC_JPEG_" + msg.GetName();
		  Notify(pubName, bufComp, msg.GetTime());
		  
		  if (m_affichage_image) {
		    Mat imdec = cv::imdecode(bufComp, CV_LOAD_IMAGE_COLOR);
		    imshow(m_display_name, imdec);
		  }
		}
		
		if (MOOSWildCmp("VCODEC_JPEG_*", msg.GetName())
		  && m_decode)
		{
		  //cout << "msg name " << msg.GetName() << endl;
		  msg.GetBinaryData(bufComp);
		  Mat imdec = cv::imdecode(bufComp, CV_LOAD_IMAGE_COLOR);
		  string pubName = msg.GetName().substr(12);
		  Notify(pubName, imdec, msg.GetTime());
		  
		  if (m_affichage_image) {
		    imshow(m_display_name, imdec);
		  }
		}
		
	}
	
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool VideoCodec::OnConnectToServer()
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
 
bool VideoCodec::Iterate()
{
  /*
	Mat	m_capture_frame(LARGEUR_IMAGE_CAMERA, HAUTEUR_IMAGE_CAMERA, CV_64FC(3));
	
	if(m_vc_v4l2.read(m_capture_frame))
	{
		if(m_inverser_image)
			flip(m_capture_frame, m_capture_frame, -1);
		
		Notify((char*)(m_image_name).c_str(), (void*)m_capture_frame.data, 3 * LARGEUR_IMAGE_CAMERA * HAUTEUR_IMAGE_CAMERA, MOOSLocalTime());
		imwrite("test.jpeg", m_capture_frame);
		
		if(m_affichage_image)
			imshow(m_display_name, m_capture_frame);
	}
	
	else
		cout << "No frame grabbed." << endl;
		
	waitKey(10);
	*/
        waitKey(10);
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool VideoCodec::OnStartUp()
{
	setlocale(LC_ALL, "C");
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
				
			if(param == "DISPLAY_IMAGE")
				m_affichage_image = (value == "true");
				
			if(param == "DECODE")
				m_decode = (value == "true");
		}
	}

	m_timewarp = GetMOOSTimeWarp();

	SetAppFreq(20, 400);
	SetIterateMode(REGULAR_ITERATE_AND_COMMS_DRIVEN_MAIL);

/*
	if(!m_vc_v4l2.open(device_name, LARGEUR_IMAGE_CAMERA, HAUTEUR_IMAGE_CAMERA))
		return false;
*/
	if(m_affichage_image)
		namedWindow(m_display_name, CV_WINDOW_NORMAL);
		
	RegisterVariables();
	setlocale(LC_ALL, "");
	return(true);
}

/**
 * \fn
 * \brief Inscription de l'application à l'évolution de certaines variables de la MOOSDB
 */
 
void VideoCodec::RegisterVariables()
{
  if (!m_decode)
	Register("VVV_IMAGE_*", "iCamera*", 0);
  else
	Register("VCODEC_JPEG_*", "*", 0);
}
