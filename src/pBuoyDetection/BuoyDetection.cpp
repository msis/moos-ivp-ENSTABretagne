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


#include "cv.h"
#include "highgui.h"
#include "math.h"
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <time.h>
#include <string>
#include <stdlib.h>

using namespace std;

IplImage* image1 = 0;
IplImage*imgYellowThresh=NULL;
int c,X0,Y0,um_inf,um_sup=255;
float X,Y;
double posX = 0,posY = 0;
int Hi,Hs,Si,Ss,Vi,Vs;
int PHSV[6]={0 , 108 ,  70,  255,  150 ,  245};
void teclado(int PHSV[6],int key);
int saturacion(int a1, int b1, int c1);
int indi;
CvCapture* g_capture = NULL;
CvFont font; 
	char Imp[100];


void teclado(int PHSV[6],int key)
{
	

		
	switch (key){
		
		case 49: indi=1; break;
		case 50: indi=2; break;
		case 51: indi=3; break;
		case 52: indi=4; break;
		case 53: indi=5; break;
		case 54: indi=6; break;
	
				}

			
	
if(indi==1){ switch (key){
		
		case  43:	PHSV[0]+=10; break;
		case  45: 	PHSV[0]-=10; break;
				}}

if(indi==4){	switch (key){
		
		case  43:	PHSV[1]+=12;  break;
		case  45:	PHSV[1]-=12;break;
					 }	}
if(indi==2){	switch (key){
		
		case  43:	PHSV[2]+=10;  break;
		case  45:	PHSV[2]-=10;break;
					 }	}
if(indi==5){	switch (key){
		
		case  43:	PHSV[3]+=10;  break;
		case  45:	PHSV[3]-=10;break;
					 }	}
if(indi==3){	switch (key){
		
		case  43:	PHSV[4]+=10;  break;
		case  45:	PHSV[4]-=10;break;
					 }	}
if(indi==6){	switch (key){
		
		case  43:	PHSV[5]+=10;  break;
		case  45:	PHSV[5]-=10;break;
					 }	}



	
      PHSV[0]=saturacion(0,PHSV[0],360);
	  PHSV[1]=saturacion(0,PHSV[1],360);
	  PHSV[2]=saturacion(0,PHSV[2],255);
	  PHSV[3]=saturacion(0,PHSV[3],255);
	  PHSV[4]=saturacion(0,PHSV[4],255);
	  PHSV[5]=saturacion(0,PHSV[5],255);


}


int saturacion(int a1, int b1, int c1)
{
	if(b1<a1){b1=a1;}
	if((b1>a1)&&(b1<c1)){b1=b1;}
	if(b1>c1){b1=c1;}
	return b1;
}
	

/**
 * \fn
 * \brief Constructeur de l'application MOOS
 */
 
BuoyDetection::BuoyDetection()
{
	m_iterations = 0;
	m_timewarp   = 1;
	
	cvNamedWindow("BuoyDetection", 1);
	
	image1 = cvCreateImage(cvSize(LARGEUR_IMAGE_CAMERA, HAUTEUR_IMAGE_CAMERA), 8, 3);
	
	g_capture = cvCreateFileCapture("pelicula.avi");
	IplImage* HSVimg = cvLoadImage( "HSV.jpg" );
	cvNamedWindow( "Original", 1 );
	cvNamedWindow( "FiltroRojo", 1 );
	cvInitFont(&font,CV_FONT_HERSHEY_SIMPLEX,0.5,0.5,0,1);

profondeur_auv = 0;	
}

/**
 * \fn
 * \brief Destructeur de l'instance de l'application
 */

BuoyDetection::~BuoyDetection()
{
	cvDestroyWindow( "Original" );
	cvDestroyWindow( "FiltroRojo" );
}

/**
 * \fn
 * \brief Méthode appelée lorsqu'une variable de la MOOSDB est mise à jour
 * N'est appelée que si l'application s'est liée à la variable en question
 */
 
bool BuoyDetection::OnNewMail(MOOSMSG_LIST &NewMail)
{
	MOOSMSG_LIST::iterator p;

	for(p = NewMail.begin() ; p != NewMail.end() ; p++)
	{
		CMOOSMsg &msg = *p;

		if(msg.GetKey() == "VVV_Z")
		{
			profondeur_auv = msg.GetDouble();
		}
		
		if(msg.GetKey() == m_nom_variable_image)
		{
			if((int)msg.GetString().size() == image1->imageSize)
				memcpy(image1->imageData, msg.GetString().data(), image1->imageSize);
			
			else
				cout << "Erreur : mauvaises dimensions dans la variable image \"" << m_nom_variable_image << "\" depuis la MOOSDB" << endl;
			
			
			//-------------------------------Punto principal------------------------------------------------------

		X0=image1->width/2;
		Y0=image1->height/2;
		cvCircle (image1, cvPoint(X0,Y0),3,CV_RGB(255,0,0),3, 8);
		

		//--------------------------------- FILTRO (HSV)----------------------------------------------------------
		
		// Convert the image into an HSV image
		IplImage* imgHSV = cvCreateImage(cvGetSize(image1), 8, 3);
		cvCvtColor(image1, imgHSV, CV_BGR2HSV);
		IplImage* imgThreshed = cvCreateImage(cvGetSize(image1), 8, 1);
				
		teclado(PHSV,c);

		Hi=PHSV[0];   Hs=PHSV[1];   Si=PHSV[2];  Ss=PHSV[3];   Vi=PHSV[4];  Vs=PHSV[5];
	
		cvInRangeS(imgHSV, cvScalar(Hi/2,Si,Vi), cvScalar(Hs/2,Ss, Vs), imgThreshed); 
		imgYellowThresh = imgThreshed; 

	
		

		////--------------------------PROCESO PARA LA OBTECION DEL CENTROIDE---------------------------------------------------------------

		// Calculate the moments to estimate the position of the ball
		CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
		cvMoments(imgYellowThresh, moments, 1);

		// The actual moment values
		double moment10 = cvGetSpatialMoment(moments, 1, 0);
		double moment01 = cvGetSpatialMoment(moments, 0, 1);
		double area = cvGetCentralMoment(moments, 0, 0);    //Numero de pixeles encontrados
	
		
		if (area<1500) //podria ser la posicion desead
		{
			posX=posX;    
			posY=posY;
			X=X;
			Y=Y;

		}
		if(area>1500)
		{
			posX = moment10/area;
			posY = moment01/area;


			//---------------------------Punto localizado---------------------------------------------------------

			
			cvCircle (image1, cvPoint(posX,posY),3,CV_RGB(0,255,0),3, 8);

			//Posicion respecto del punto principal
			X= (posX-(image1->width/2)) ;   
			Y=-((image1->height/2)-posY);   


			

		}

		sprintf(Imp, "X:%.0f ", X); cvPutText (image1,Imp,cvPoint(100,220), &font, cvScalar(0,0,0));
		sprintf(Imp, "Y:%.0f ", Y); cvPutText (image1,Imp,cvPoint(200,220), &font, cvScalar(0,0,0));
		
		m_Comms.Notify("VVV_X_BUOY", X);
		m_Comms.Notify("VVV_Y_BUOY", Y);
		m_Comms.Notify("VVV_Z_BUOY", profondeur_auv);
		
		cvLine( image1, cvPoint(image1->width/2,0), cvPoint(image1->width/2,image1->height), CV_RGB(255,0,0),1 );
		cvLine( image1, cvPoint(0,image1->height/2), cvPoint(image1->width,image1->height/2), CV_RGB(255,0,0),1 );
		
		//-------------------------------------------------------------------------------------------------------
		printf("%d  %d  %d  %d  %d  %d  %f\n",Hi,Hs,Si,Ss,Vi,Vs,area);
		cout<<area<<endl;
		
		cvFlip (imgYellowThresh,imgYellowThresh,0);
		cvShowImage( "FiltroRojo", imgYellowThresh );
		cvShowImage( "Original", image1 );
			
		
		c = cvWaitKey(1);
		if(c == 27) break;


		delete moments; 
		cvReleaseImage(&imgYellowThresh);
		cvReleaseImage(&imgHSV);
		
		
		
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
}
