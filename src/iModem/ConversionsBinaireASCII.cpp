/**
 * \file ConversionsBinaireASCII.cpp
 * \brief Classe ConversionsBinaireASCII
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 13th 2013
 *
 * Classe de conversion de chaine ASCII en binaire et inversement
 *
 */
 
#include <iostream>
#include <bitset>
#include "ConversionsBinaireASCII.h"
#include "Communication.h"

/**
 * \fn
 * \brief Fonction de conversion d'une chaine ASCII en binaire
 */

void ConversionsBinaireASCII::asciiToBinary(char* input_ascii, char* output_binary)
{
	char* temp;
	for(int j = 0 ; j < NOMBRE_OCTETS_MESSAGE_FINAL ; j ++)
	{
		bitset<8> c(input_ascii[j]);
		temp = (char*)c.to_string().c_str();
		for(int k = 0 ; k < 8 ; k ++)
			output_binary[j*8 + k] = temp[k];
	}
	
	output_binary[NOMBRE_BITS_TOTAL] = '\0';
}

/**
 * \fn
 * \brief Fonction de conversion d'une chaine représentant un nombre binaire en ASCII
 */

void ConversionsBinaireASCII::binaryToAscii(char* input_binary, char* output_ascii)
{
	char* temp;
	bitset<32> b = bitset<32>(string(input_binary));
	unsigned long i = b.to_ulong();
	temp = (char*)&i;
	
	for(int j = 0 ; j < NOMBRE_OCTETS_MESSAGE_FINAL ; j ++)
		output_ascii[j] = temp[j];
	
	inverserChaine(output_ascii, NOMBRE_OCTETS_MESSAGE_FINAL);
}

/**
 * \fn
 * \brief Fonction inversant l'ordre des caractères dans une chaine
 */

void ConversionsBinaireASCII::inverserChaine(char chaine[], int longueur)
{
	char temp;
	for(int i = 0 ; i < longueur/2 ; i++)
	{
		temp = chaine[i];
		chaine[i] = chaine[longueur - i - 1];
		chaine[longueur - i - 1] = temp;
	}
}
