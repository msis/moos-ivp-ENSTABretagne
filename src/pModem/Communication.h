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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../common/constantes.h"

#define NOMBRE_BITS_TYPE		2
#define NOMBRE_BITS_TOTAL		32
#define NOMBRE_BITS_REDONDANCE	NOMBRE_BITS_TOTAL / 2
#define NOMBRE_BITS_DATA		(NOMBRE_BITS_TOTAL / 2) - NOMBRE_BITS_TYPE
#define CODE_AUTRE_INFORMATION	0
#define CODE_POSITION_X			1
#define CODE_POSITION_Y			2
#define CODE_POSITION_Z			3

using namespace std;

class Communication
{
	protected:
		static bool encoderMessage(int type_message, int data, char* resultat);

	public :
		static long conversionEnBinaire(int nombre_decimal);
		static int conversionEnDecimal(long nombre_binaire);
		static bool encoderX(int data_x, char* resultat);
		static bool encoderY(int data_y, char* resultat);
		static bool encoderZ(int data_z, char* resultat);
		static bool decoderMessage(char* message, int* type_message, int* data);
		static bool messageValide(char* message);
};

#endif // COMMUNICATION_H
