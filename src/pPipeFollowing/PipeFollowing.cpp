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

#include <clocale>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <string>
#include "MBUtils.h"
#include "PipeFollowing.h"
#include "ColorParse.h"

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
	green = CV_RGB(30, 153, 65);
	
	m_img = cvCreateImage(cvSize(LARGEUR_IMAGE_CAMERA, HAUTEUR_IMAGE_CAMERA), 8, 3);
	img_nb = cvCreateImage(cvGetSize(m_img), 8, 1);
	
	channelRed = cvCreateImage(cvGetSize(m_img), 8, 1);
	channelGreen = cvCreateImage(cvGetSize(m_img), 8, 1);
	channelBlue = cvCreateImage(cvGetSize(m_img), 8, 1);
	channelYellow = cvCreateImage(cvGetSize(m_img), 8, 1);
	
	m_linreg = new LinearRegression(NULL, NULL, 0);
}

/**
 * \fn
 * \brief Méthode calculant l'angle d'orientation du pipeline
 */

void PipeFollowing::updateOrientationPipe()
{
	CvPoint pt; // Pour le dessin des points sur l'image
	m_linreg->reset();
	uchar *data_seuillage = NULL, *data_nb = NULL;
	seuillageTeinteJaune(m_img, m_param_valeur_seuillage, &data_seuillage, &data_nb);
	
	char texte_image[50];
	CvFont font;
	cvInitFont(&font, CV_FONT_HERSHEY_COMPLEX, 0.4, 0.4, 0.0, 0.08, CV_AA);
	int w = cvGetSize(m_img).width;
	int h = cvGetSize(m_img).height;
	vector<int> points_tranche;
	
	// On récupère les centres de chaque tranche du pipe
	int nb_points_regression = 0, mediane_abscisses, ecart_type_abscisses, nb_points;
	for(int j = m_param_marge_image ; j < h - m_param_marge_image ; j++) // h tranches sont étudiées
	{
		nb_points = 0;
		
		for(int i = m_param_marge_image ; i < w - m_param_marge_image ; i ++)
		{
			if(data_seuillage[j * w + i] == 255)
			{
				pt = cvPoint(i, j);
				cvLine(m_img, pt, pt, red, 1, DRAWING_CONNECTIVITY);
				points_tranche.push_back(i);
				nb_points ++;
			}
		}
		
		if(nb_points != 0)
		{
			// Récupération de l'abscisse centrale de la tranche considérée
			mediane_abscisses = (int)Statistiques::mediane(points_tranche);
			ecart_type_abscisses = (int)Statistiques::ecartType(points_tranche);
			
			if(ecart_type_abscisses < m_param_ecart_type_maximal)
			{
				for(int i = m_param_marge_image ; i < w - m_param_marge_image ; i ++)
				{
					if(data_seuillage[j * w + i] == 255)
					{
						pt = cvPoint(i, j);
						cvLine(m_img, pt, pt, green, 1, DRAWING_CONNECTIVITY);
					}
				}
				
				m_linreg->addXY(j, mediane_abscisses, false);
				pt = cvPoint(mediane_abscisses, j);
				cvLine(m_img, pt, pt, red, DRAWING_THICKNESS / 1.5, DRAWING_CONNECTIVITY);
				nb_points_regression ++;
			}
		}
		
		points_tranche.clear();
	}
	
	// On réalise une régression linéaire sur l'ensemble des centres récupérés
	m_linreg->Calculate();
	
	// Estimation de la largeur du pipe
	int largeur_pipe_visible;
	int nb_points_jaunes;
	int x_tranche, y_tranche;
	vector<int> mesures_largeur_pipe;
	for(int l = m_param_marge_image ; l < h - m_param_marge_image ; l++) // l tranches sont étudiées
	{
		nb_points_jaunes = 0;
		
		for(int k = -m_param_largeur_max_pipe / 2 ; k < m_param_largeur_max_pipe / 2 ; k ++)
		{
			x_tranche = m_linreg->estimateY(l) + k;
			y_tranche = l - m_linreg->getB() * k;
			
			if(data_seuillage[y_tranche * w + x_tranche] == 255)
				nb_points_jaunes ++;
		}
		
		mesures_largeur_pipe.push_back(nb_points_jaunes);
	}
	
	largeur_pipe_visible = Statistiques::mediane(mesures_largeur_pipe);
	m_largeur_pipe = largeur_pipe_visible * (1 + m_param_proportion_pipe_non_visible); // Une partie du pipe n'est pas détectée
	
	// Estimation du taux de reconnaissance du pipe
	nb_points_jaunes = 0;
	int nb_points_total = 0;
	for(int l = m_param_marge_image ; l < h - m_param_marge_image ; l++) // l tranches sont étudiées
	{
		for(int k = -largeur_pipe_visible / 2 ; k < largeur_pipe_visible / 2 ; k ++)
		{
			x_tranche = m_linreg->estimateY(l) + k;
			y_tranche = l - m_linreg->getB() * k;
			
			if(data_seuillage[y_tranche * w + x_tranche] == 255)
				nb_points_jaunes ++;
				
			nb_points_total ++;
		}
	}
	
	if(nb_points_total != 0)
		m_taux_reconnaissance_pipe = nb_points_jaunes * 100.0 / nb_points_total;
	 
	// Visualisation de la direction du pipe
	CvScalar* couleur_contours_pipe;
	
	if(m_taux_reconnaissance_pipe >= 80)
		couleur_contours_pipe = &blue;
	
	else
		couleur_contours_pipe = &red;
		
	cvLine(m_img, 
			cvPoint((m_largeur_pipe / 2) + m_linreg->estimateY(0), 0), 
			cvPoint((m_largeur_pipe / 2) + m_linreg->estimateY(w), w), 
			*couleur_contours_pipe, 2, DRAWING_CONNECTIVITY); 
	cvLine(m_img, 
			cvPoint(-(m_largeur_pipe / 2) + m_linreg->estimateY(0), 0), 
			cvPoint(-(m_largeur_pipe / 2) + m_linreg->estimateY(w), w), 
			*couleur_contours_pipe, 2, DRAWING_CONNECTIVITY);
	
	// Affichage d'informations sur l'image
	sprintf(texte_image, "Taux : %.1lf%%", m_taux_reconnaissance_pipe);
	CvScalar* couleur_texte;
	
	if(m_taux_reconnaissance_pipe >= 80)
		couleur_texte = &white;
	
	else
		couleur_texte = &red;
	
	cvPutText(m_img, texte_image, cvPoint(10, 20), &font, *couleur_texte);
}

/**
 * \fn
 * \brief Méthode effectuant un seuillage sur les composantes jaunes de l'image
 */
 
void PipeFollowing::seuillageTeinteJaune(IplImage* img, int seuil, uchar **data_seuillage, uchar **data_nb)
{
	// Récupération des composantes RGB
	cvSplit(img, channelBlue, channelGreen, channelRed, NULL);
	
	// Conversion en noir et blanc
	cvCvtColor(img, img_nb, CV_RGB2GRAY);
	
	// Soustraction des composantes RGB pour ne récupérer que la composante qui nous intéresse
	cvAdd(channelRed, channelGreen, channelYellow);			// Les données GREEN et RED sont assemblées dans YELLOW
	cvSub(channelYellow, channelBlue, channelYellow);		// Les données BLUE sont soustraites de YELLOW
	cvThreshold(channelYellow, channelYellow, m_param_valeur_seuillage, 255, CV_THRESH_BINARY); // Seuillage
	*data_seuillage = (uchar *)channelYellow->imageData;
	*data_nb = (uchar *)img_nb->imageData;
}

/**
 * \fn
 * \brief Méthode retournant l'angle d'orientation du pipeline
 */
 
void PipeFollowing::getJonctionsPipe(IplImage* m_img, LinearRegression* linreg, int largeur_pipe, double taux_reconnaissance_pipe)
{
	/*	----------------------------
	 * 			   BONUS
	 *  ---------------------------		*/
	
	// Détection des jonctions entre les pipes
	uchar *data_seuillage = NULL, *data_nb = NULL;
	seuillageTeinteJaune(m_img, m_param_valeur_seuillage, &data_seuillage, &data_nb);
	
	int nb_points_jaunes;
	int x_tranche, y_tranche;
	int w = cvGetSize(m_img).width;
	int h = cvGetSize(m_img).height;
	int niveau_gris_centre_tranche;
	int niveau_gris_median_centre_tranche;
	vector<int> mesures_niveaux_gris_centraux;
	int x_centre_tranche_flottant, y_centre_tranche_flottant;
	for(int l = 0 ; l < h ; l++) // l tranches sont étudiées
	{
		nb_points_jaunes = 0;
		
		for(int k = -m_param_largeur_max_pipe / 2 ; k < m_param_largeur_max_pipe / 2 ; k ++)
		{
			x_tranche = linreg->estimateY(l) + k;
			y_tranche = l - linreg->getB() * k;
			
			if(data_seuillage[y_tranche * w + x_tranche] == 255)
				nb_points_jaunes ++;
			
			if(k == 0 && l % 2 == 0)
				mesures_niveaux_gris_centraux.push_back(data_nb[y_tranche * w + x_tranche]);
		}
		
		niveau_gris_median_centre_tranche = Statistiques::mediane(mesures_niveaux_gris_centraux);
		
		if(taux_reconnaissance_pipe >= 90) // On ne réalise l'étude que si le pipe est correctement détecté
		{
			if(nb_points_jaunes * 1.0 / m_param_largeur_max_pipe > m_param_proportion_points_jonction_pipe)
			{
				// Une tranche plus large a été détectée :
				// on couple cette information à une recherche d'ombre au centre du pipe
				
				for(int m = -20 ; m < 20 ; m ++)
				{
					y_centre_tranche_flottant = l + m;
					x_centre_tranche_flottant = linreg->estimateY(y_centre_tranche_flottant);
					
					if(data_nb[y_centre_tranche_flottant * w + x_centre_tranche_flottant] < niveau_gris_median_centre_tranche)
					{
						cvLine(m_img, 
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
 * \brief Destructeur de l'instance de l'application
 */

PipeFollowing::~PipeFollowing()
{
	cvReleaseImage(&img_nb);
	cvReleaseImage(&channelRed);
	cvReleaseImage(&channelGreen);
	cvReleaseImage(&channelBlue);
	cvReleaseImage(&channelYellow);
	cvDestroyWindow("PipeFollowing");
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool PipeFollowing::OnNewMail(MOOSMSG_LIST &NewMail)
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

			updateOrientationPipe();
			cvShowImage("PipeFollowing", m_img);
			waitKey(10);
		}
	
		if(msg.GetKey() == "PIPEFOLLOWING__LARGEUR_MAX_PIPE")
		{
			m_param_largeur_max_pipe = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "PIPEFOLLOWING__PROPORTION_PIPE_NON_VISIBLE")
		{
			m_param_proportion_pipe_non_visible = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "PIPEFOLLOWING__CORR_COEFF_MIN_DETECTION")
		{
			m_param_corr_coeff_min_detection = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "PIPEFOLLOWING__VALEUR_SEUILLAGE")
		{
			m_param_valeur_seuillage = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "PIPEFOLLOWING__PROPORTION_POINTS_JONCTION_PIPE")
		{
			m_param_proportion_points_jonction_pipe = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "PIPEFOLLOWING__ECART_TYPE_MAXIMAL")
		{
			m_param_ecart_type_maximal = msg.GetDouble();
			nouveau_parametre = true;
		}

		if(msg.GetKey() == "PIPEFOLLOWING__MARGE_IMAGE")
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
		cout << "\tPIPEFOLLOWING__LARGEUR_MAX_PIPE : \t\t\t" << m_param_largeur_max_pipe << endl;
		cout << "\tPIPEFOLLOWING__PROPORTION_PIPE_NON_VISIBLE :\t\t" << m_param_proportion_pipe_non_visible << endl;
		cout << "\tPIPEFOLLOWING__CORR_COEFF_MIN_DETECTION : \t\t" << m_param_corr_coeff_min_detection << endl;
		cout << "\tPIPEFOLLOWING__VALEUR_SEUILLAGE : \t\t\t" << m_param_valeur_seuillage << endl;
		cout << "\tPIPEFOLLOWING__PROPORTION_POINTS_JONCTION_PIPE : \t" << m_param_proportion_points_jonction_pipe << endl;
		cout << "\tPIPEFOLLOWING__ECART_TYPE_MAXIMAL : \t\t\t" << m_param_ecart_type_maximal << endl;
		cout << "\tPIPEFOLLOWING__MARGE_IMAGE : \t\t\t\t" << m_param_marge_image << endl;
	}
		
	return(true);
}

/**
 * \fn
 * \brief Méthode appelée dès que le contact avec la MOOSDB est effectué
 */
 
bool PipeFollowing::OnConnectToServer()
{
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
			if(param == "PIPEFOLLOWING__LARGEUR_MAX_PIPE")
				m_param_largeur_max_pipe = atof(value.c_str());

			if(param == "PIPEFOLLOWING__PROPORTION_PIPE_NON_VISIBLE")
				m_param_proportion_pipe_non_visible = atof(value.c_str());

			if(param == "PIPEFOLLOWING__CORR_COEFF_MIN_DETECTION")
				m_param_corr_coeff_min_detection = atof(value.c_str());

			if(param == "PIPEFOLLOWING__VALEUR_SEUILLAGE")
				m_param_valeur_seuillage = atof(value.c_str());

			if(param == "PIPEFOLLOWING__PROPORTION_POINTS_JONCTION_PIPE")
				m_param_proportion_points_jonction_pipe = atof(value.c_str());

			if(param == "PIPEFOLLOWING__ECART_TYPE_MAXIMAL")
				m_param_ecart_type_maximal = atof(value.c_str());

			if(param == "PIPEFOLLOWING__MARGE_IMAGE")
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
 
void PipeFollowing::RegisterVariables()
{
	m_Comms.Register(m_nom_variable_image, m_intervalle_mise_a_jour);
	m_Comms.Register("PIPEFOLLOWING__LARGEUR_MAX_PIPE", 0);
	m_Comms.Register("PIPEFOLLOWING__PROPORTION_PIPE_NON_VISIBLE", 0);
	m_Comms.Register("PIPEFOLLOWING__CORR_COEFF_MIN_DETECTION", 0);
	m_Comms.Register("PIPEFOLLOWING__VALEUR_SEUILLAGE", 0);
	m_Comms.Register("PIPEFOLLOWING__PROPORTION_POINTS_JONCTION_PIPE", 0);
	m_Comms.Register("PIPEFOLLOWING__ECART_TYPE_MAXIMAL", 0);
	m_Comms.Register("PIPEFOLLOWING__MARGE_IMAGE", 0);
}
