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
	m_iterations = 0;
	m_timewarp   = 1;
	cvNamedWindow("PipeFollowing", 1);

	/*	Pour de la capture depuis un fichier vidéo :
			CvCapture *capture;
			IplImage *img;
			int key, i = 0;
			capture = cvCaptureFromAVI("pipeline.avi");
			img = cvQueryFrame(capture);
			* 
			cvReleaseCapture(&capture);
	*/
	
	red = CV_RGB(158, 8, 0);
	blue = CV_RGB(19, 102, 143);
	white = CV_RGB(255, 255, 255);
	 
	m_img = cvCreateImage(cvSize(LARGEUR_IMAGE_CAMERA, HAUTEUR_IMAGE_CAMERA), 8, 1);
	img_nb = cvCreateImage(cvGetSize(m_img), 8, 1);
	img_hsv = cvCreateImage(cvGetSize(m_img), 8, 3);
}

/**
 * \fn
 * \brief Méthode retournant l'angle d'orientation du pipeline
 */

void PipeFollowing::getOrientationPipe(IplImage* img_original, LinearRegression* linreg, int* largeur_pipe, double* taux_reconnaissance_pipe)
{
	CvPoint pt; // Pour le dessin des points sur l'image
	int marge = 5; // Pour ne pas étudier les bords de l'image (erreurs possibles)
	linreg->reset();
	uchar *data_seuillage = NULL, *data_nb = NULL;
	seuillageTeinteJaune(img_original, VALEUR_SEUILLAGE, &data_seuillage, &data_nb);
	
	char texte_image[50];
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.4, 0.4, 0.0, 0.08, CV_AA);
	int w = cvGetSize(img_original).width;
	int h = cvGetSize(img_original).height;
	
	vector<int> points_tranche;
	
	// On récupère les centres de chaque tranche du pipe
	int nb_points_regression = 0, mediane_abscisses, nb_points;
	for(int j = marge ; j < h - marge ; j++) // h tranches sont étudiées
	{
		nb_points = 0;
		
		for(int i = marge ; i < w - marge ; i ++)
		{
			if(data_seuillage[j * w + i] == 255)
			{
				pt = cvPoint(i, j);
				cvLine(img_original, pt, pt, red, 1, DRAWING_CONNECTIVITY);
				points_tranche.push_back(i);
				nb_points ++;
			}
		}
		
		if(nb_points != 0)
		{
			// Récupération de l'abscisse centrale de la tranche considérée
			mediane_abscisses = (int)mediane(points_tranche);
			linreg->addXY(j, mediane_abscisses, false);
			pt = cvPoint(mediane_abscisses, j);
			cvLine(img_original, pt, pt, red, DRAWING_THICKNESS / 1.5, DRAWING_CONNECTIVITY);
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
	for(int l = marge ; l < h - marge ; l++) // l tranches sont étudiées
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
	
	largeur_pipe_visible = mediane(mesures_largeur_pipe);
	*largeur_pipe = largeur_pipe_visible * (1 + PROPORTION_PIPE_NON_VISIBLE); // Une partie du pipe n'est pas détectée
	
	// Estimation du taux de reconnaissance du pipe
	nb_points_jaunes = 0;
	int nb_points_total = 0;
	for(int l = marge ; l < h - marge ; l++) // l tranches sont étudiées
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
	IplImage *channelRed = cvCreateImage(cvGetSize(img_original), 8, 1);
	IplImage *channelGreen = cvCreateImage(cvGetSize(img_original), 8, 1);
	IplImage *channelBlue = cvCreateImage(cvGetSize(img_original), 8, 1);
	IplImage *channelYellow = cvCreateImage(cvGetSize(img_original), 8, 1);
	
	// Récupération des composantes RGB
	cvSplit(img_original, channelBlue, channelGreen, channelRed, NULL);
	
	// Conversion en noir et blanc
	cvCvtColor(img_original, img_nb, CV_RGB2GRAY);
	
	// Soustraction des composantes RGB pour ne récupérer que la composante rouge représentant le pipe
	cvAdd(channelRed, channelGreen, channelYellow);			// Les données GREEN et RED sont assemblées dans YELLOW
	cvSub(channelYellow, channelBlue, channelYellow);		// Les données BLUE sont soustraites de YELLOW
	cvThreshold(channelYellow, channelYellow, VALEUR_SEUILLAGE, 255, CV_THRESH_BINARY); // Seuillage
	*data_seuillage = (uchar *)channelYellow->imageData;
	*data_nb = (uchar *)img_nb->imageData;
	
	cvReleaseImage(&channelRed);
	cvReleaseImage(&channelGreen);
	cvReleaseImage(&channelBlue);
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
	int w = cvGetSize(img_original).width;
	int h = cvGetSize(img_original).height;
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
		
		niveau_gris_median_centre_tranche = mediane(mesures_niveaux_gris_centraux);
		
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
 
double PipeFollowing::mediane(vector<int> vec)
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
 * \brief Médiane sur les valeurs d'un vecteur d'entiers
 */
 
double PipeFollowing::moyenne(vector<int> vec)
{
	return 1.0;
}

/**
 * \fn
 * \brief Médiane sur les valeurs d'un vecteur d'entiers
 */
 
double PipeFollowing::ecartType(vector<int> vec)
{
	return 1.0;
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

PipeFollowing::~PipeFollowing()
{
	cvReleaseImage(&img_nb);
	cvReleaseImage(&img_hsv);
	cvDestroyWindow("PipeFollowing");
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

		if(msg.GetKey() == m_nom_variable_image)
		{
			/*m_img->imageData = (char*)msg.GetString().data();
			cvShowImage("PipeFollowing", m_img);
			//getOrientationPipe(m_img, m_linreg, &m_largeur_pipe, &m_taux_reconnaissance_pipe);
			
			cout << msg.GetString() << endl;
			
			
			cvShowImage("PipeFollowing", m_img);
			
			
			*/
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
 
bool PipeFollowing::OnConnectToServer()
{
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

			if(param == "VARIABLE_IMAGE_NAME")
				m_nom_variable_image = value;
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
	m_Comms.Register(m_nom_variable_image, 0);
}
