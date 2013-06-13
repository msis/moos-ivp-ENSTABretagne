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
#include "ConversionsBinaireASCII.h"

// Structure du message
#define NOMBRE_OCTETS_MESSAGE_FINAL			4
#define NOMBRE_BITS_TYPE					4
#define NOMBRE_BITS_TOTAL					32
#define NOMBRE_BITS_REDONDANCE				NOMBRE_BITS_TOTAL / 2
#define NOMBRE_BITS_DATA					(NOMBRE_BITS_TOTAL / 2) - NOMBRE_BITS_TYPE

// Types du message
#define TYPE_MESURE_EXTERNE_X				0
#define CODE_TYPE_MESURE_EXTERNE_X			"0000"
#define TYPE_MESURE_EXTERNE_Y				1
#define CODE_TYPE_MESURE_EXTERNE_Y			"0001"
#define TYPE_MESURE_EXTERNE_Z				2
#define CODE_TYPE_MESURE_EXTERNE_Z			"0010"
#define TYPE_CONSIGNE_VX					3
#define CODE_TYPE_CONSIGNE_VX				"0011"
#define TYPE_CONSIGNE_VY					4
#define CODE_TYPE_CONSIGNE_VY				"0100"
#define TYPE_CONSIGNE_VZ					5
#define CODE_TYPE_CONSIGNE_VZ				"0101"
#define TYPE_CONSIGNE_RZ					6
#define CODE_TYPE_CONSIGNE_RZ				"0110"
#define TYPE_CONSIGNE_CAP					7
#define CODE_TYPE_CONSIGNE_CAP				"0111"
#define TYPE_CONSIGNE_VITESSE				8
#define CODE_TYPE_CONSIGNE_VITESSE			"1000"
#define TYPE_CONSIGNE_Z						9
#define CODE_TYPE_CONSIGNE_Z				"1001"
#define TYPE_CONFIRMATION_RECEPTION			10
#define CODE_TYPE_CONFIRMATION_RECEPTION	"1010"
#define TYPE_ETAT_ANOMALIE					11
#define CODE_TYPE_ETAT_ANOMALIE				"1011"
#define TYPE_ANOMALIE_X						12
#define CODE_TYPE_ANOMALIE_X				"1100"
#define TYPE_ANOMALIE_Y						13
#define CODE_TYPE_ANOMALIE_Y				"1101"
#define TYPE_ANOMALIE_Z						14
#define CODE_TYPE_ANOMALIE_Z				"1110"

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
		
		static bool encoderMesureExterneX(int data_x, char* resultat);
		static bool encoderMesureExterneY(int data_y, char* resultat);
		static bool encoderMesureExterneZ(int data_z, char* resultat);
		
		static bool encoderPositionAnomalieX(int data_x, char* resultat);
		static bool encoderPositionAnomalieY(int data_y, char* resultat);
		static bool encoderPositionAnomalieZ(int data_z, char* resultat);
		
		static bool encoderEtatAnomalie(bool etat, char* resultat);
		static bool encoderConfirmationReception(char* resultat);
		
		static bool decoderMessage(char* message, int* type_message, int* data);
		static bool messageValide(char* message);
};

#endif // COMMUNICATION_H
