/**
 * \file ConversionsBinaireASCII.h
 * \brief Classe ConversionsBinaireASCII
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 13th 2013
 *
 * Classe de conversion de chaine ASCII en binaire et inversement
 *
 */

#ifndef CONVERSION_BINAIRE_ASCII_H
#define CONVERSION_BINAIRE_ASCII_H

#include <iostream>
#include <fstream>
#include <cmath>
#include "string.h"
#include "../common/constantes.h"

using namespace std;

class ConversionsBinaireASCII
{
	public:
		static void asciiToBinary(char* input_ascii, char* output_binary);
		static void binaryToAscii(char* input_binary, char* output_ascii);
};

#endif // CONVERSION_BINAIRE_ASCII_H
