/**
 * \file Communication.h
 * \brief Classe Communication
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 11th 2013
 *
 * Classe de manipulation de codes correcteurs pour communications bruitées
 *
 */
 
/*		TO DO :
 * 		Implémenter un code correcteur (Hamming ?)
 * 		Actuellement, le message est juste envoyé deux fois : pas de correction, juste détection
 */

/*	RÈGLES D'ENCODAGE DU MESSAGE :
 * 		4 bits pour le type du message
 * 		12 bits pour les données
 * 		16 bits de redondance (identiques aux 16 premiers)
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Communication.h"

/**
 * \fn
 * \brief Méthode encodant une donnée sur X
 */

bool Communication::encoderMesureExterneX(int data_x, char* resultat)
{
	return Communication::encoderMessage(TYPE_MESURE_EXTERNE_X, round(data_x / PRECISION_DONNEES_ENVOYEES), resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur Y
 */

bool Communication::encoderMesureExterneY(int data_y, char* resultat)
{
	return Communication::encoderMessage(TYPE_MESURE_EXTERNE_Y, round(data_y / PRECISION_DONNEES_ENVOYEES), resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur la profondeur
 */

bool Communication::encoderMesureExterneZ(int data_z, char* resultat)
{
	return Communication::encoderMessage(TYPE_MESURE_EXTERNE_Z, round(data_z / PRECISION_DONNEES_ENVOYEES), resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur la position X de l'anomalie
 */

bool Communication::encoderPositionAnomalieX(int data_x, char* resultat)
{
	return Communication::encoderMessage(TYPE_ANOMALIE_X, round(data_x / PRECISION_DONNEES_ENVOYEES), resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur la position Y de l'anomalie
 */

bool Communication::encoderPositionAnomalieY(int data_y, char* resultat)
{
	return Communication::encoderMessage(TYPE_ANOMALIE_Y, round(data_y / PRECISION_DONNEES_ENVOYEES), resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur la profondeur de l'anomalie
 */

bool Communication::encoderPositionAnomalieZ(int data_z, char* resultat)
{
	return Communication::encoderMessage(TYPE_ANOMALIE_Z, round(data_z/ PRECISION_DONNEES_ENVOYEES), resultat);
}

/**
 * \fn
 * \brief Méthode encodant l'information sur l'état de l'anomalie à trouver sur le mur
 * 			Il s'agit de l'état de la lumière se trouvant sur la bouée
 */

bool Communication::encoderEtatAnomalie(bool etat, char* resultat)
{
	return Communication::encoderMessage(TYPE_ETAT_ANOMALIE, (int)etat, resultat);
}

/**
 * \fn
 * \brief Méthode encodant une confirmation de bonne réception d'un message par ce moyen de communication
 */

bool Communication::encoderConfirmationReception(char* resultat, bool bonne_reception)
{
	if(bonne_reception)
		return Communication::encoderMessage(TYPE_CONFIRMATION_RECEPTION, 1, resultat);
	
	else
		return Communication::encoderMessage(TYPE_CONFIRMATION_RECEPTION, 0, resultat);
}

/**
 * \fn
 * \brief Méthode corrigeant un message lorsque celui-ci présente une erreur
 */

bool Communication::corrigerMessage(char* message)
{
	// TO DO
	return true;
}

/**
 * \fn
 * \brief Méthode détectant une erreur dans un message
 */

bool Communication::messageValide(char* message)
{
	for(int i = 0 ; i < NOMBRE_BITS_TOTAL / 2 ; i ++)
		if(message[i] != message[i + NOMBRE_BITS_TOTAL / 2])
			return false;
	
	return true;
}

/**
 * \fn
 * \brief Méthode retournant le type du message correspondant à la chaine passée en parametre
 */

int Communication::typeDeMessageCorrespondant(char* chaine_type)
{
	if(strcmp(chaine_type, CODE_TYPE_MESURE_EXTERNE_X) == 0)
		return TYPE_MESURE_EXTERNE_X;
		
	if(strcmp(chaine_type, CODE_TYPE_MESURE_EXTERNE_Y) == 0)
		return TYPE_MESURE_EXTERNE_Y;
		
	if(strcmp(chaine_type, CODE_TYPE_MESURE_EXTERNE_Z) == 0)
		return TYPE_MESURE_EXTERNE_Z;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_VX) == 0)
		return TYPE_CONSIGNE_VX;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_VY) == 0)	
		return TYPE_CONSIGNE_VY;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_VZ) == 0)
		return TYPE_CONSIGNE_VZ;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_VZ) == 0)
		return TYPE_CONSIGNE_VZ;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_RZ) == 0)	
		return TYPE_CONSIGNE_RZ;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_CAP) == 0)
		return TYPE_CONSIGNE_CAP;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_VITESSE) == 0)
		return TYPE_CONSIGNE_VITESSE;
		
	if(strcmp(chaine_type, CODE_TYPE_CONSIGNE_Z) == 0)	
		return TYPE_CONSIGNE_Z;
		
	if(strcmp(chaine_type, CODE_TYPE_CONFIRMATION_RECEPTION) == 0)
		return TYPE_CONFIRMATION_RECEPTION;
		
	if(strcmp(chaine_type, CODE_TYPE_ETAT_ANOMALIE) == 0)
		return TYPE_ETAT_ANOMALIE;
		
	if(strcmp(chaine_type, CODE_TYPE_ANOMALIE_X) == 0)
		return TYPE_ANOMALIE_X;
		
	if(strcmp(chaine_type, CODE_TYPE_ANOMALIE_Y) == 0)
		return TYPE_ANOMALIE_Y;
		
	if(strcmp(chaine_type, CODE_TYPE_ANOMALIE_Z) == 0)
		return TYPE_ANOMALIE_Z;
	
	return -1;
}

/**
 * \fn
 * \brief Méthode décodant un message donné
 */

bool Communication::decoderMessage(char* message, int* type_message, int* data)
{
	char temp[NOMBRE_BITS_TYPE];
	
	if(!Communication::messageValide(message))
		if(!Communication::corrigerMessage(message))
			return false;
	
	char type_primaire[4];
	sprintf(type_primaire, "%s", strndup(message, NOMBRE_BITS_TYPE));
	
	for(int i = 0 ; i < NOMBRE_BITS_TYPE ; i ++)
		temp[i] = message[i];
	temp[NOMBRE_BITS_TYPE] = '\0';
		
	*type_message = Communication::typeDeMessageCorrespondant(temp);
		
	for(int i = 0 ; i < NOMBRE_BITS_DATA ; i ++)
		temp[i] = message[NOMBRE_BITS_TYPE + i];
	temp[NOMBRE_BITS_DATA] = '\0';
		
	*data = Communication::conversionEnDecimal(atol(temp));
	
	// S'il s'agit d'une distance
	if(*type_message == TYPE_MESURE_EXTERNE_X ||
		*type_message == TYPE_MESURE_EXTERNE_Y ||
		*type_message == TYPE_MESURE_EXTERNE_Z ||
		*type_message == TYPE_ANOMALIE_X ||
		*type_message == TYPE_ANOMALIE_Y ||
		*type_message == TYPE_ANOMALIE_Z)
		*data *= PRECISION_DONNEES_ENVOYEES;
		
	return *type_message != -1;
}

/**
 * \fn
 * \brief Méthode encodant un message à envoyer
 */

bool Communication::encoderMessage(int type_message, int data, char* resultat)
{
	char code_type_message[2];
	switch(type_message)
	{
		case TYPE_MESURE_EXTERNE_X: 		sprintf(code_type_message, "%s", CODE_TYPE_MESURE_EXTERNE_X); break;
		case TYPE_MESURE_EXTERNE_Y: 		sprintf(code_type_message, "%s", CODE_TYPE_MESURE_EXTERNE_Y); break;
		case TYPE_MESURE_EXTERNE_Z: 		sprintf(code_type_message, "%s", CODE_TYPE_MESURE_EXTERNE_Z); break;
		case TYPE_CONSIGNE_VX: 				sprintf(code_type_message, "%s", CODE_TYPE_CONSIGNE_VX); break;
		case TYPE_CONSIGNE_VY: 				sprintf(code_type_message, "%s", CODE_TYPE_CONSIGNE_VY); break;
		case TYPE_CONSIGNE_VZ: 				sprintf(code_type_message, "%s", CODE_TYPE_CONSIGNE_VZ); break;
		case TYPE_CONSIGNE_RZ: 				sprintf(code_type_message, "%s", CODE_TYPE_CONSIGNE_RZ); break;
		case TYPE_CONSIGNE_CAP: 			sprintf(code_type_message, "%s", CODE_TYPE_CONSIGNE_CAP); break;
		case TYPE_CONSIGNE_VITESSE: 		sprintf(code_type_message, "%s", CODE_TYPE_CONSIGNE_VITESSE); break;
		case TYPE_CONSIGNE_Z: 				sprintf(code_type_message, "%s", CODE_TYPE_CONSIGNE_Z); break;
		case TYPE_CONFIRMATION_RECEPTION: 	sprintf(code_type_message, "%s", CODE_TYPE_CONFIRMATION_RECEPTION); break;
		case TYPE_ETAT_ANOMALIE: 			sprintf(code_type_message, "%s", CODE_TYPE_ETAT_ANOMALIE); break;
		case TYPE_ANOMALIE_X: 				sprintf(code_type_message, "%s", CODE_TYPE_ANOMALIE_X); break;
		case TYPE_ANOMALIE_Y: 				sprintf(code_type_message, "%s", CODE_TYPE_ANOMALIE_Y); break;
		case TYPE_ANOMALIE_Z: 				sprintf(code_type_message, "%s", CODE_TYPE_ANOMALIE_Z); break;
		default: return false;
	}
	
	if(data > pow(2, NOMBRE_BITS_DATA)) // Le codage ne passe plus sur 14 bits
	{
		cout << "Encodage du message impossible !" << endl;
		return false;
	}
	
	int i, j;
	char temp[NOMBRE_BITS_DATA];
	sprintf(resultat, "%s", code_type_message); // Type du message
	sprintf(temp, "%ld", Communication::conversionEnBinaire(data)); // Contenu du message
	
	for(i = 0 ; i < NOMBRE_BITS_DATA - (int)strlen(temp) ; i ++)
		sprintf(resultat, "%s0", resultat);
	
	for(i = 0 ; i < (int)strlen(temp) ; i ++)
		sprintf(resultat, "%s%c", resultat, temp[i]);
	
	sprintf(resultat, "%s%s", resultat, resultat); // Redondance
	return true;
}

/**
 * \fn
 * \brief Méthode de conversion d'un nombre décimal en nombre binaire
 */

long Communication::conversionEnBinaire(int nombre_decimal)
{
	char resultat[NOMBRE_BITS_TOTAL];
	int i = 0, t[40], j = 0;
	
	while(nombre_decimal > 0)
	{
		t[i] = nombre_decimal % 2;
		nombre_decimal = nombre_decimal / 2;
		i ++;
	}
	
	sprintf(resultat, "");
	
	for(j = (i - 1) ; j >= 0 ; j--)
		sprintf(resultat, "%s%d", resultat, t[j]);
		
	return strtol(resultat, NULL, 0);
}

/**
 * \fn
 * \brief Méthode de conversion d'un nombre binaire en nombre décimal
 */

int Communication::conversionEnDecimal(long nombre_binaire)
{
	char resultat[NOMBRE_BITS_TOTAL];
	sprintf(resultat, "%ld", nombre_binaire);
	return strtol(resultat, NULL, 2);
}
