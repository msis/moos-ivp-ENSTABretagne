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
#include <set>
#include "MBUtils.h"
#include "VideoCodec.h"

using namespace std;
using namespace cv;

namespace {
  // Maximum number of images to process 
  const size_t nb_images_max = 8;
}


/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
VideoCodec::VideoCodec()
{
	m_iterations = 0;
	m_timewarp   = 1;
	
	m_nb_img = 0;
	m_nb_buf = 0;
	
	// Initialize raw image and JPEG data buffers (for OnNewMail loop)
	m_img.assign(nb_images_max, ImageData());
	m_buf.assign(nb_images_max, JpegData());
	for (unsigned int i=0; i<nb_images_max; ++i)
	{
	  // Create image buffer
	  m_img[i].img.create(HAUTEUR_IMAGE_CAMERA, LARGEUR_IMAGE_CAMERA,  CV_8UC3);
	  // reserve 200kB for compressed data
	  m_buf[i].buf.reserve(200000);
	}
	// Initialize raw image and JPEG data buffers (for iterate codec loop)
	imgRaw.create(HAUTEUR_IMAGE_CAMERA, LARGEUR_IMAGE_CAMERA,  CV_8UC3);
	bufComp.reserve(200000);
	
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
 * \brief Retourne l'index du buffer d'image disponible pour la variable passée en paramètre
 */
  unsigned int VideoCodec::getImageDataIndexForVar(const std::string& varName)
  {
    // Index of buffer to use
    unsigned int buf_id = nb_images_max;
    for (unsigned int i=0; i<m_nb_img; i++)
    {
      if (m_img[i].name == varName) {
	buf_id = i;
	break;
      }
    }
    // If no buffer is tied to the designed variable name
    if (buf_id == nb_images_max)
    {
      // Use next available buffer for this variable
      if (m_nb_img < nb_images_max)
      {
	buf_id = m_nb_img++;
      }
      // Overwrite the first buffer if no other is available
      else
      {
	buf_id = 0;
      }
    }
    return buf_id;
  }
  
/**
 * \fn
 * \brief Retourne l'index du buffer de données JPEG disponible pour la variable passée en paramètre
 */
  unsigned int VideoCodec::getJpegDataIndexForVar(const std::string& varName)
  {
    // Index of buffer to use
    unsigned int buf_id = nb_images_max;
    for (unsigned int i=0; i<m_nb_buf; i++)
    {
      if (m_img[i].name == varName) {
	buf_id = i;
	break;
      }
    }
    // If no buffer is tied to the designed variable name
    if (buf_id == nb_images_max)
    {
      // Use next available buffer for this variable
      if (m_nb_buf < nb_images_max)
      {
	buf_id = m_nb_buf++;
      }
      // Overwrite the first buffer if no other is available
      else
      {
	buf_id = 0;
      }
    }
    return buf_id;
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
		  // Index of buffer to use
		  unsigned int buf_id = getImageDataIndexForVar(msg.GetName());
		  
		  m_img[buf_id].name = msg.GetName();
		  m_img[buf_id].time = msg.GetTime();
		  
		  //cout << "msg name " << msg.GetName() << endl;
		  memcpy(m_img[buf_id].img.data, msg.GetBinaryData(), msg.GetBinaryDataSize());
		  
		  /*
		  // Set image compression parameters
		  vector<int> encode_params;
		  encode_params.push_back(CV_IMWRITE_JPEG_QUALITY);
		  encode_params.push_back(60);
		  // Compress image to JPEG with OpenCV
		  cv::imencode(".jpg",imgRaw,bufComp,encode_params);
		  // Prefix the variable name and publish compressed image
		  string pubName = "VCODEC_JPEG_" + msg.GetName();
		  Notify(pubName, bufComp, msg.GetTime());
		  
		  // Optional display of the compressed image (may be used for quality check)
		  if (m_affichage_image) {
		    Mat imdec = cv::imdecode(bufComp, CV_LOAD_IMAGE_COLOR);
		    imshow(m_display_name, imdec);
		    waitKey(10);
		  }
		  */
		}
		
		if (MOOSWildCmp("VCODEC_JPEG_*", msg.GetName())
		  && m_decode)
		{
		  // Index of buffer to use
		  unsigned int buf_id = getJpegDataIndexForVar(msg.GetName());
		  m_buf[buf_id].name = msg.GetName();
		  m_buf[buf_id].time = msg.GetTime();
		  
		  //cout << "msg name " << msg.GetName() << endl;
		  msg.GetBinaryData(m_buf[buf_id].buf);
		  
		  /*
		  // Decode JPEG image data with OpenCV
		  Mat imdec = cv::imdecode(bufComp, CV_LOAD_IMAGE_COLOR);
		  // Remove the "VCODEC_JPEG_" prefix from variable name
		  string pubName = msg.GetName().substr(12);
		  // Publish uncompressed image
		  Notify(pubName, imdec, msg.GetTime());
		  
		  // Optional display of the uncompressed image
		  if (m_affichage_image) {
		    imshow(m_display_name, imdec);
		    waitKey(10);
		  }
		  */
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
  // Set image compression parameters
  vector<int> encode_params;
  encode_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  encode_params.push_back(60);
  
  // Compress buffered images to JPEG
  for (unsigned int buf_id = 0; buf_id < m_nb_img; buf_id++)
  {
    const string &name = m_img[buf_id].name;
    const double &time = m_img[buf_id].time;
    const cv::Mat &img = m_img[buf_id].img;

    // Compress image to JPEG with OpenCV
    cv::imencode(".jpg",img,bufComp,encode_params);
    // Prefix the variable name and publish compressed image
    string pubName = "VCODEC_JPEG_" + name;
    Notify(pubName, bufComp, time);
    
    // Optional display of the compressed image (may be used for quality check)
    if (m_affichage_image && buf_id == m_nb_img-1) {
      Mat imdec = cv::imdecode(bufComp, CV_LOAD_IMAGE_COLOR);
      imshow(m_display_name, imdec);
      waitKey(10);
    }
  }
  // Reset image buffer
  m_nb_img = 0;
  
  // Decode buffered JPEG data
  for (unsigned int buf_id = 0; buf_id < m_nb_img; buf_id++)
  {
    const string &name = m_buf[buf_id].name;
    const double &time = m_buf[buf_id].time;
    const vector<uchar> &buf = m_buf[buf_id].buf;
    
    // Decode JPEG image data with OpenCV
    Mat imdec = cv::imdecode(buf, CV_LOAD_IMAGE_COLOR);
    // Remove the "VCODEC_JPEG_" prefix from variable name
    string pubName = name.substr(12);
    // Publish uncompressed image
    Notify(pubName, imdec, time);
    
    // Optional display of the uncompressed image
    if (m_affichage_image  && buf_id == m_nb_buf-1) {
      imshow(m_display_name, imdec);
      waitKey(10);
    }
  }
  // Reset jpeg data buffer
  m_nb_buf = 0;
  
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

//	SetAppFreq(20, 400);
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
