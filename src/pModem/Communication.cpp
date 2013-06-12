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
 * 		2 bits pour le type du message : posX, posY, posZ ou autre
 * 			si autre, 2 bits supplémentaires pour repréciser le type
 * 		14 bits pour les données (12 si autre)
 * 		16 bits de redondance (identiques aux 16 premiers)
 * 
 *		11.. : data Z
 *		10.. : data Y
 *		01.. : data X
 *		0001 : état anomalie
 *		0000 : confirmation réception
 *		0010 : -
 *		0011 : -
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

bool Communication::encoderX(int data_x, char* resultat)
{
	return Communication::encoderMessage(TYPE_POSITION_X, data_x, resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur Y
 */

bool Communication::encoderY(int data_y, char* resultat)
{
	return Communication::encoderMessage(TYPE_POSITION_Y, data_y, resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur la profondeur
 */

bool Communication::encoderZ(int data_z, char* resultat)
{
	return Communication::encoderMessage(TYPE_POSITION_Z, data_z, resultat);
}

/**
 * \fn
 * \brief Méthode encodant l'information sur l'état de l'anomalie à trouver sur le mur
 * 			Il s'agit de l'état de la lumière se trouvant sur la bouée
 */

bool Communication::encoderEtatAnomalie(bool etat, char* resultat)
{
	return Communication::encoderMessage(TYPE_AUTRE_ETAT_ANOMALIE, (int)etat, resultat);
}

/**
 * \fn
 * \brief Méthode encodant une confirmation de bonne réception d'un message par ce moyen de communication
 */

bool Communication::encoderConfirmationReception(char* resultat)
{
	return Communication::encoderMessage(TYPE_AUTRE_CONFIRMATION_RECEPTION, 0, resultat);
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
	return strcmp(strndup(message, NOMBRE_BITS_REDONDANCE), strndup(message + NOMBRE_BITS_REDONDANCE, NOMBRE_BITS_REDONDANCE)) == 0;
}

/**
 * \fn
 * \brief Méthode retournant le type du message correspondant à la chaine passée en parametre
 */

int Communication::typeDeMessageCorrespondant(char* chaine_type)
{
	if(strlen(chaine_type) == 4) // Cas d'un type "autre"
	{
		if(strcmp(chaine_type, CODE_TYPE_AUTRE_CONFIRMATION_RECEPTION) == 0)
			return TYPE_AUTRE_CONFIRMATION_RECEPTION;
		
		if(strcmp(chaine_type, CODE_TYPE_AUTRE_ETAT_ANOMALIE) == 0)
			return TYPE_AUTRE_ETAT_ANOMALIE;
	}
	
	else
	{
		if(strcmp(chaine_type, CODE_TYPE_POSITION_X) == 0)
			return TYPE_POSITION_X;
		
		if(strcmp(chaine_type, CODE_TYPE_POSITION_Y) == 0)
			return TYPE_POSITION_Y;
		
		if(strcmp(chaine_type, CODE_TYPE_POSITION_Z) == 0)
			return TYPE_POSITION_Z;
	}
	
	return -1;
}

/**
 * \fn
 * \brief Méthode décodant un message donné
 */

bool Communication::decoderMessage(char* message, int* type_message, int* data)
{
	if(!Communication::messageValide(message))
		if(!Communication::corrigerMessage(message))
			return false;
	
	char type_primaire[2];
	sprintf(type_primaire, "%s", strndup(message, NOMBRE_BITS_TYPE));
	
	if(strcmp(type_primaire, CODE_TYPE_AUTRE) == 0)
	{
		*type_message = Communication::typeDeMessageCorrespondant(strndup(message, NOMBRE_BITS_TYPE * 2));
		*data = Communication::conversionEnDecimal(atol(strndup(message + NOMBRE_BITS_TYPE * 2, NOMBRE_BITS_DATA - NOMBRE_BITS_TYPE)));
	}
	
	else
	{
		*type_message = Communication::typeDeMessageCorrespondant(strndup(message, NOMBRE_BITS_TYPE));
		*data = Communication::conversionEnDecimal(atol(strndup(message + NOMBRE_BITS_TYPE, NOMBRE_BITS_DATA)));
	}
	
	return true;
}

/**
 * \fn
 * \brief Méthode encodant un message à envoyer
 */

bool Communication::encoderMessage(int type_message, int data, char* resultat)
{
	if(type_message == TYPE_POSITION_X || type_message == TYPE_POSITION_Y || type_message == TYPE_POSITION_Z)
	{
		char code_type_message[2];
		switch(type_message)
		{
			case TYPE_POSITION_X:
				sprintf(code_type_message, "%s", CODE_TYPE_POSITION_X);
				break;
			
			case TYPE_POSITION_Y:
				sprintf(code_type_message, "%s", CODE_TYPE_POSITION_Y);
				break;
			
			case TYPE_POSITION_Z:
				sprintf(code_type_message, "%s", CODE_TYPE_POSITION_Z);
				break;
		}
		
		if(data > pow(2, NOMBRE_BITS_DATA)) // Le codage ne passe plus sur 14 bits
		{
			cout << "Encodage du message impossible !" << endl;
			return false;
		}
		
		int i, j;
		char temp[NOMBRE_BITS_DATA];
		sprintf(resultat, "");
		
		sprintf(temp, "%s", code_type_message); // Type du message
		
		for(j = 0 ; j < NOMBRE_BITS_TYPE - (int)strlen(temp) ; j ++)
			sprintf(resultat, "%s0", resultat);
		for(i = 0 ; i < (int)strlen(temp) ; i ++)
			sprintf(resultat, "%s%c", resultat, temp[i]);
		
		sprintf(temp, "%ld", Communication::conversionEnBinaire(data)); // Contenu du message
		
		for(j = 0 ; j < NOMBRE_BITS_DATA - (int)strlen(temp) ; j ++)
			sprintf(resultat, "%s0", resultat);
		for(i = 0 ; i < (int)strlen(temp) ; i ++)
			sprintf(resultat, "%s%c", resultat, temp[i]);
	}
	
	else if(type_message == TYPE_AUTRE_ETAT_ANOMALIE || type_message == TYPE_AUTRE_CONFIRMATION_RECEPTION)
	{
		char char_data;
		char code_type_message[4];
		switch(type_message)
		{
			case TYPE_AUTRE_ETAT_ANOMALIE:
				char_data = (char)(((int)'0') + data);
				sprintf(code_type_message, "%s", CODE_TYPE_AUTRE_ETAT_ANOMALIE);
				break;
			
			case TYPE_AUTRE_CONFIRMATION_RECEPTION:
				char_data = '0';
				sprintf(code_type_message, "%s", CODE_TYPE_AUTRE_CONFIRMATION_RECEPTION);
				break;
		}
		
		sprintf(resultat, "%s", code_type_message);
		for(int i = 0 ; i < NOMBRE_BITS_DATA - NOMBRE_BITS_TYPE ; i ++)
			sprintf(resultat, "%s%c", resultat, char_data);
	}
	
	else
		sprintf(resultat, "");
	
	sprintf(resultat, "%s%s", resultat, resultat); // Redondance
	
	return true;
}

/**
 * \fn
 * \brief Méthode de conversion d'un nombre décimal en nombre binaire
 */

long Communication::conversionEnBinaire(int nombre_decimal)
{
	char resultat[32];
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
	char resultat[32];
	sprintf(resultat, "%ld", nombre_binaire);
	return strtol(resultat, NULL, 2);
}
