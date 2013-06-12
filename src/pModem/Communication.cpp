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
	return Communication::encoderMessage(CODE_POSITION_X, data_x, resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur Y
 */

bool Communication::encoderY(int data_y, char* resultat)
{
	return Communication::encoderMessage(CODE_POSITION_Y, data_y, resultat);
}

/**
 * \fn
 * \brief Méthode encodant une donnée sur la profondeur
 */

bool Communication::encoderZ(int data_z, char* resultat)
{
	return Communication::encoderMessage(CODE_POSITION_Z, data_z, resultat);
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
 * \brief Méthode décodant un message donné
 */

bool Communication::decoderMessage(char* message, int* type_message, int* data)
{
	if(!Communication::messageValide(message))
		return false;
	
	*type_message = Communication::conversionEnDecimal(atol(strndup(message, NOMBRE_BITS_TYPE)));
	*data = Communication::conversionEnDecimal(atol(strndup(message + NOMBRE_BITS_TYPE, NOMBRE_BITS_DATA)));
	return true;
}

/**
 * \fn
 * \brief Méthode encodant un message à envoyer
 */

bool Communication::encoderMessage(int type_message, int data, char* resultat)
{
	/*	RÈGLES D'ENCODAGE DU MESSAGE :
	 * 		2 bits pour le type du message : posX, posY, posZ ou autre
	 * 		14 bits pour les données
	 * 		16 bits de redondance (identiques aux 16 premiers)
	 */
	if(data > pow(2, NOMBRE_BITS_DATA)) // Le codage ne passe plus sur 14 bits
	{
		cout << "Encodage du message impossible !" << endl;
		return false;
	}
	
	if(type_message == CODE_POSITION_X || type_message == CODE_POSITION_Y || type_message == CODE_POSITION_Z)
	{
		int i, j;
		char temp[NOMBRE_BITS_DATA];
		sprintf(resultat, "");
		
		sprintf(temp, "%ld", Communication::conversionEnBinaire(type_message)); // Type du message
		
		for(j = 0 ; j < NOMBRE_BITS_TYPE - (int)strlen(temp) ; j ++)
			sprintf(resultat, "%s0", resultat);
		for(i = 0 ; i < (int)strlen(temp) ; i ++)
			sprintf(resultat, "%s%c", resultat, temp[i]);
		
		sprintf(temp, "%ld", Communication::conversionEnBinaire(data)); // Contenu du message
		
		for(j = 0 ; j < NOMBRE_BITS_DATA - (int)strlen(temp) ; j ++)
			sprintf(resultat, "%s0", resultat);
		for(i = 0 ; i < (int)strlen(temp) ; i ++)
			sprintf(resultat, "%s%c", resultat, temp[i]);
		
		sprintf(temp, "%ld", Communication::conversionEnBinaire(type_message)); // Type du message
		
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
	
	else
		sprintf(resultat, "");
	
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
