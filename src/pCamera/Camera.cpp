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
#include "regression_lineaire/linreg.h"


/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
Camera::Camera()
{
	m_iterations = 0;
	m_timewarp   = 1;

	CvCapture *capture;
	IplImage *img;
	int key, i = 0;
	capture = cvCaptureFromAVI("pipeline.avi");
	cvNamedWindow("PipeFollowing", 1 );
	
	img = cvQueryFrame(capture); 
	
	channelRed = cvCreateImage(cvGetSize(img), 8, 1);
	channelGreen = cvCreateImage(cvGetSize(img), 8, 1);
	channelBlue = cvCreateImage(cvGetSize(img), 8, 1);
	img_nb = cvCreateImage(cvGetSize(img), 8, 1);
	img_hsv = cvCreateImage(cvGetSize(img), 8, 3);

	while(key != 'q')
	{
		i ++;
		img = cvQueryFrame(capture); 
		
		// Début de perte du pipeline : 8000
		// Perte du pipeline : 9000
		// Fin du pipeline : 12700
		
		if(i < 4000)
			continue;
		
		//cout << "Frame " << i << endl;
		getOrientationPipe(img);
		key = cvWaitKey(10); 
		cvShowImage("PipeFollowing", img);
	}

	cvReleaseImage(&img_nb);
	cvReleaseImage(&img_hsv);
	cvReleaseImage(&channelRed);
	cvReleaseImage(&channelGreen);
	cvReleaseImage(&channelBlue);
	cvReleaseCapture(&capture);
	cvDestroyWindow("PipeFollowing");
}

double Camera::getOrientationPipe(IplImage* img_original)
{
	// Transposition en HSV : la teinte jaune devient rouge
	cvCvtColor(img_original, img_hsv, CV_BGR2HSV);
	cvSplit(img_hsv, channelBlue, channelGreen, channelRed, NULL);
	
	// Conversion en noir et blanc
	cvCvtColor(img_original, img_nb, CV_RGB2GRAY);
	
	// Soustraction des composantes RGB pour ne récupérer que la composante rouge représentant le pipe
	cvAdd(channelGreen, channelBlue, channelBlue);	// Les données GREEN et BLUE sont assemblées dans BLUE
	cvSub(channelRed, channelBlue, channelRed);		// Les données BLUE sont soustraites de RED
	cvThreshold(channelRed, channelRed, 100, 255, CV_THRESH_BINARY); // Seuillage
	uchar *data_seuillage = (uchar *)channelRed->imageData;
	uchar *data_nb = (uchar *)img_nb->imageData;

	char texte_image[50];
	int thickness = 2;
	int connectivity = 8;
	CvScalar red = CV_RGB(158, 8, 0);
	CvScalar blue = CV_RGB(19, 102, 143);
	CvScalar white = CV_RGB(255, 255, 255);
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.4, 0.4, 0.0, 0.08, CV_AA);
	int w = cvGetSize(channelRed).width;
	int h = cvGetSize(channelRed).height;
	
	double x[] = { };
	double y[] = { };
	
	LinearRegression* linreg = new LinearRegression(x, y, 0);
	vector<int> points_tranche;
	
	// On récupère les centres de chaque tranche du pipe
	int nb_points_regression = 0, mediane_abscisses, nb_points;
	for(int j = 0 ; j < h ; j++) // h tranches sont étudiées
	{
		nb_points = 0;
		
		for(int i = 0 ; i < w ; i ++)
		{
			if(data_seuillage[j * w + i] == 255)
			{
				points_tranche.push_back(i);
				nb_points ++;
			}
		}
		
		if(nb_points != 0)
		{
			// Récupération de l'abscisse centrale de la tranche considérée
			mediane_abscisses = (int)median(points_tranche);
			linreg->addXY(j, mediane_abscisses, false);
			CvPoint pt3 = cvPoint(mediane_abscisses, j);
			cvLine(img_original, pt3, pt3, red, thickness / 1.5, connectivity);
			nb_points_regression ++;
		}
		
		points_tranche.clear();
	}
	
	// On réalise une régression linéaire sur l'ensemble des centres récupérés
	linreg->Calculate();
	
	int marge_avant_calculs = 0;
	int min_intensite_ombre = 255;
	int rang_min_intensite_ombre = -1;
	for(int j = marge_avant_calculs ; j < h - marge_avant_calculs ; j++) // h tranches sont étudiées
	{
		if(data_nb[j * w + (int)linreg->estimateY(j)] < min_intensite_ombre)
		{
			min_intensite_ombre = data_nb[j * w + (int)linreg->estimateY(j)];
			rang_min_intensite_ombre = j;
		}
	}
	
	int centre, largeur_pipe = 150;
	
	// Visualisation de la direction du pipe
	cvLine(img_original, 
			cvPoint((largeur_pipe / 2) + linreg->estimateY(0), 0), 
			cvPoint((largeur_pipe / 2) + linreg->estimateY(w), w), 
			blue, thickness, connectivity); 
	cvLine(img_original, 
			cvPoint(-(largeur_pipe / 2) + linreg->estimateY(0), 0), 
			cvPoint(-(largeur_pipe / 2) + linreg->estimateY(w), w), 
			blue, thickness, connectivity);
	//cvLine(img_original, cvPoint(linreg->estimateY(0), 0), cvPoint(linreg->estimateY(w), w), blue, thickness, connectivity); 
	sprintf(texte_image, "Corr : %lf", linreg->getCoefCorrel());
	cvPutText(img_original, texte_image, cvPoint(10, 20), &font, white);

	return linreg->getB();
}

double Camera::median(vector<int> vec)
{
	typedef vector<int>::size_type vec_sz;

	vec_sz size = vec.size();
	if (size == 0)
		throw domain_error("median of an empty vector");

	sort(vec.begin(), vec.end());
	vec_sz mid = size/2;

	return size % 2 == 0 ? (vec[mid] + vec[mid-1]) / 2 : vec[mid];
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
	/*MOOSMSG_LIST::iterator p;

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
	}*/
	
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
	/*m_vc >> m_capture_frame;
	cvtColor(m_capture_frame, m_bw_image, CV_BGR2GRAY);
	resize(m_bw_image, m_image, m_image.size(), 0, 0, INTER_NEAREST);
	Notify("Image", (void*)m_image.data, m_image.size().area(), MOOSLocalTime());
	
	imshow("display", m_image);
	waitKey(10);*/

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
	
	/*if(!m_vc.open(identifiant_camera))
		return false;*/
		
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
