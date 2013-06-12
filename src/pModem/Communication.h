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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "../common/constantes.h"

// Structure du message
#define NOMBRE_BITS_TYPE			2
#define NOMBRE_BITS_TOTAL			32
#define NOMBRE_BITS_REDONDANCE		NOMBRE_BITS_TOTAL / 2
#define NOMBRE_BITS_DATA			(NOMBRE_BITS_TOTAL / 2) - NOMBRE_BITS_TYPE

// Types du message
#define TYPE_AUTRE								0
#define CODE_TYPE_AUTRE							"00"
#define TYPE_POSITION_X							1
#define CODE_TYPE_POSITION_X					"01"
#define TYPE_POSITION_Y							2
#define CODE_TYPE_POSITION_Y					"10"
#define TYPE_POSITION_Z							3
#define CODE_TYPE_POSITION_Z					"11"
#define TYPE_AUTRE_CONFIRMATION_RECEPTION		4
#define CODE_TYPE_AUTRE_CONFIRMATION_RECEPTION	"0000"
#define TYPE_AUTRE_ETAT_ANOMALIE				5
#define CODE_TYPE_AUTRE_ETAT_ANOMALIE			"0001"

using namespace std;

class Communication
{
	protected:
		static bool encoderMessage(int type_message, int data, char* resultat);
		static int typeDeMessageCorrespondant(char* chaine_type);
		static bool corrigerMessage(char* message_initial); // TO DO

	public :
		static long conversionEnBinaire(int nombre_decimal);
		static int conversionEnDecimal(long nombre_binaire);
		static bool encoderX(int data_x, char* resultat);
		static bool encoderY(int data_y, char* resultat);
		static bool encoderZ(int data_z, char* resultat);
		static bool encoderEtatAnomalie(bool etat, char* resultat);
		static bool encoderConfirmationReception(char* resultat);
		static bool decoderMessage(char* message, int* type_message, int* data);
		static bool messageValide(char* message);
};

#endif // COMMUNICATION_H
