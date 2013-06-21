/**
 * \file PipeFollowing.cpp
 * \brief Classe PipeFollowing
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 20th 2013
 *
 * Application MOOS de suivi de pipe
 *
 */

#include <iterator>
#include <iomanip>
#include <cmath>
#include "MBUtils.h"
#include "PipeFollowing.h"

using namespace std;

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
PipeFollowing::PipeFollowing()
{
	/*	------------------------------	
	 * 			TO DO
	 * Actuellement, l'application se lance directement sur une vidéo
	 * Lorsque le pipefollowing sera validé -> le lancer sur les données
	 * de la MOOSDB
	 * 
	 */
	 
	m_iterations = 0;
	m_timewarp   = 1;

	CvCapture *capture;
	IplImage *img;
	int key, i = 0;
	capture = cvCaptureFromAVI("pipeline.avi");
	cvNamedWindow("PipeFollowing", 1 );
	
	red = CV_RGB(158, 8, 0);
	blue = CV_RGB(19, 102, 143);
	white = CV_RGB(255, 255, 255);
	
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
		int largeur_pipe;
		double taux_reconnaissance_pipe = 0;
		LinearRegression* linreg = new LinearRegression(NULL, NULL, 0);
		getOrientationPipe(img, linreg, &largeur_pipe, &taux_reconnaissance_pipe);
		getJonctionsPipe(img, linreg, largeur_pipe, taux_reconnaissance_pipe);
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

/**
 * \fn
 * \brief Méthode retournant l'angle d'orientation du pipeline
 */

void PipeFollowing::getOrientationPipe(IplImage* img_original, LinearRegression* linreg, int* largeur_pipe, double* taux_reconnaissance_pipe)
{
	uchar *data_seuillage = NULL, *data_nb = NULL;
	seuillageTeinteJaune(img_original, VALEUR_SEUILLAGE, &data_seuillage, &data_nb);
	
	char texte_image[50];
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.4, 0.4, 0.0, 0.08, CV_AA);
	int w = cvGetSize(channelRed).width;
	int h = cvGetSize(channelRed).height;
	
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
			cvLine(img_original, pt3, pt3, red, DRAWING_THICKNESS / 1.5, DRAWING_CONNECTIVITY);
			nb_points_regression ++;
		}
		
		points_tranche.clear();
	}
	
	// On réalise une régression linéaire sur l'ensemble des centres récupérés
	linreg->Calculate();
	
	// Estimation de la largeur du pipe
	int largeur_pipe_visible;
	int nb_points_jaunes;
	int x_tranche, y_tranche;
	vector<int> mesures_largeur_pipe;
	for(int l = 0 ; l < h ; l++) // l tranches sont étudiées
	{
		nb_points_jaunes = 0;
		
		for(int k = -LARGEUR_MAX_PIPE / 2 ; k < LARGEUR_MAX_PIPE / 2 ; k ++)
		{
			x_tranche = linreg->estimateY(l) + k;
			y_tranche = l - linreg->getB() * k;
			
			if(data_seuillage[y_tranche * w + x_tranche] == 255)
				nb_points_jaunes ++;
		}
		
		mesures_largeur_pipe.push_back(nb_points_jaunes);
	}
	
	largeur_pipe_visible = median(mesures_largeur_pipe);
	*largeur_pipe = largeur_pipe_visible * (1 + PROPORTION_PIPE_NON_VISIBLE); // Une partie du pipe n'est pas détectée
	
	// Estimation du taux de reconnaissance du pipe
	nb_points_jaunes = 0;
	int nb_points_total = 0;
	for(int l = 0 ; l < h ; l++) // l tranches sont étudiées
	{
		for(int k = -largeur_pipe_visible / 2 ; k < largeur_pipe_visible / 2 ; k ++)
		{
			x_tranche = linreg->estimateY(l) + k;
			y_tranche = l - linreg->getB() * k;
			
			if(data_seuillage[y_tranche * w + x_tranche] == 255)
				nb_points_jaunes ++;
				
			nb_points_total ++;
		}
	}
	
	if(nb_points_total != 0)
		*taux_reconnaissance_pipe = nb_points_jaunes * 100.0 / nb_points_total;
	 
	// Visualisation de la direction du pipe
	CvScalar* couleur_contours_pipe;
	
	if(*taux_reconnaissance_pipe >= 80)
		couleur_contours_pipe = &blue;
	
	else
		couleur_contours_pipe = &red;
		
	cvLine(img_original, 
			cvPoint((*largeur_pipe / 2) + linreg->estimateY(0), 0), 
			cvPoint((*largeur_pipe / 2) + linreg->estimateY(w), w), 
			*couleur_contours_pipe, DRAWING_THICKNESS, DRAWING_CONNECTIVITY); 
	cvLine(img_original, 
			cvPoint(-(*largeur_pipe / 2) + linreg->estimateY(0), 0), 
			cvPoint(-(*largeur_pipe / 2) + linreg->estimateY(w), w), 
			*couleur_contours_pipe, DRAWING_THICKNESS, DRAWING_CONNECTIVITY);
	
	// Affichage d'informations sur l'image
	sprintf(texte_image, "Taux : %.1lf%%", *taux_reconnaissance_pipe);
	CvScalar* couleur_texte;
	
	if(*taux_reconnaissance_pipe >= 80)
		couleur_texte = &white;
	
	else
		couleur_texte = &red;
	
	cvPutText(img_original, texte_image, cvPoint(10, 20), &font, *couleur_texte);
}

/**
 * \fn
 * \brief Méthode effectuant un seuillage sur les composantes jaunes de l'image
 */
 
void PipeFollowing::seuillageTeinteJaune(IplImage* img_original, int seuil, uchar **data_seuillage, uchar **data_nb)
{
	// Transposition en HSV : la teinte jaune devient rouge
	cvCvtColor(img_original, img_hsv, CV_BGR2HSV);
	cvSplit(img_hsv, channelBlue, channelGreen, channelRed, NULL);
	
	// Conversion en noir et blanc
	cvCvtColor(img_original, img_nb, CV_RGB2GRAY);
	
	// Soustraction des composantes RGB pour ne récupérer que la composante rouge représentant le pipe
	cvAdd(channelGreen, channelBlue, channelBlue);	// Les données GREEN et BLUE sont assemblées dans BLUE
	cvSub(channelRed, channelBlue, channelRed);		// Les données BLUE sont soustraites de RED
	cvThreshold(channelRed, channelRed, VALEUR_SEUILLAGE, 255, CV_THRESH_BINARY); // Seuillage
	*data_seuillage = (uchar *)channelRed->imageData;
	*data_nb = (uchar *)img_nb->imageData;
}

/**
 * \fn
 * \brief Méthode retournant l'angle d'orientation du pipeline
 */
 
void PipeFollowing::getJonctionsPipe(IplImage* img_original, LinearRegression* linreg, int largeur_pipe, double taux_reconnaissance_pipe)
{
	/*	----------------------------
	 * 			   BONUS
	 *  ---------------------------		*/
	
	// Détection des jonctions entre les pipes
	uchar *data_seuillage = NULL, *data_nb = NULL;
	seuillageTeinteJaune(img_original, VALEUR_SEUILLAGE, &data_seuillage, &data_nb);
	
	int nb_points_jaunes;
	int x_tranche, y_tranche;
	int w = cvGetSize(channelRed).width;
	int h = cvGetSize(channelRed).height;
	int niveau_gris_centre_tranche;
	int niveau_gris_median_centre_tranche;
	vector<int> mesures_niveaux_gris_centraux;
	int x_centre_tranche_flottant, y_centre_tranche_flottant;
	for(int l = 0 ; l < h ; l++) // l tranches sont étudiées
	{
		nb_points_jaunes = 0;
		
		for(int k = -LARGEUR_MAX_PIPE / 2 ; k < LARGEUR_MAX_PIPE / 2 ; k ++)
		{
			x_tranche = linreg->estimateY(l) + k;
			y_tranche = l - linreg->getB() * k;
			
			if(data_seuillage[y_tranche * w + x_tranche] == 255)
				nb_points_jaunes ++;
			
			if(k == 0 && l % 2 == 0)
				mesures_niveaux_gris_centraux.push_back(data_nb[y_tranche * w + x_tranche]);
		}
		
		niveau_gris_median_centre_tranche = median(mesures_niveaux_gris_centraux);
		
		if(taux_reconnaissance_pipe >= 90) // On ne réalise l'étude que si le pipe est correctement détecté
		{
			if(nb_points_jaunes * 1.0 / LARGEUR_MAX_PIPE > PROPORTION_POINTS_JONCTION_PIPE)
			{
				// Une tranche plus large a été détectée :
				// on couple cette information à une recherche d'ombre au centre du pipe
				
				for(int m = -20 ; m < 20 ; m ++)
				{
					y_centre_tranche_flottant = l + m;
					x_centre_tranche_flottant = linreg->estimateY(y_centre_tranche_flottant);
					
					if(data_nb[y_centre_tranche_flottant * w + x_centre_tranche_flottant] < niveau_gris_median_centre_tranche)
					{
						cvLine(img_original, 
							cvPoint(linreg->estimateY(l) - (largeur_pipe / 2), l - linreg->getB() * (-largeur_pipe / 2)),
							cvPoint(linreg->estimateY(l) + (largeur_pipe / 2), l - linreg->getB() * (largeur_pipe / 2)),
							white, DRAWING_THICKNESS, DRAWING_CONNECTIVITY);
						break;
					}
				}
			}
		}
	}
}

/**
 * \fn
 * \brief Rotation d'un angle 'angle' sur une image 'src' (retourne 'dst')
 */
 
void PipeFollowing::rotationImage(IplImage* src, IplImage* dst, double angle)
{
	Mat imgMat(src); 
	Point2f src_center(imgMat.cols/2.0F, imgMat.rows/2.0F);
	Mat rot_mat = getRotationMatrix2D(src_center, angle, 1.0);
	warpAffine(imgMat, m_mat_dst, rot_mat, imgMat.size());
	dst->imageData = (char *) m_mat_dst.data;
}

/**
 * \fn
 * \brief Médiane sur les valeurs d'un vecteur d'entiers
 */
 
double PipeFollowing::median(vector<int> vec)
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

PipeFollowing::~PipeFollowing()
{
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool PipeFollowing::OnNewMail(MOOSMSG_LIST &NewMail)
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
	}

	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool PipeFollowing::OnConnectToServer()
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
 
bool PipeFollowing::Iterate()
{
	m_iterations++;
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée au lancement de l'application
 */
 
bool PipeFollowing::OnStartUp()
{
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
 
void PipeFollowing::RegisterVariables()
{
	// m_Comms.Register("FOOBAR", 0);
}
