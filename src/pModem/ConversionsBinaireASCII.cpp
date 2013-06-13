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

#include "ConversionsBinaireASCII.h"

/**
 * \fn
 * \brief Fonction de conversion d'une chaine ASCII en binaire
 */

void ConversionsBinaireASCII::asciiToBinary(char* input_ascii, char* output_binary)
{
	int ascii; // used to store the ASCII number of a character
	int length = strlen(input_ascii); // find the length of the user's input
	
	sprintf(output_binary, "");
	
	for(int x = 0; x < length; x++) // repeat until user's input have all been read
	// x < length because the last character is "\0"
	{
		ascii = input_ascii[x]; // store a character in its ASCII number

		/* Refer to http://www.wikihow.com/Convert-from-Decimal-to-Binary for conversion method used 
		* in this program*/		

		char* binary_reverse = new char [9]; // dynamic memory allocation
		char* binary = new char [9];

		int y = 0; // counter used for arrays

		while(ascii != 1) // continue until ascii == 1
		{
			if(ascii % 2 == 0) // if ascii is divisible by 2
				binary_reverse[y] = '0'; // then put a zero
			
			else if(ascii % 2 == 1) // if it isnt divisible by 2
				binary_reverse[y] = '1'; // then put a 1
			
			ascii /= 2; // find the quotient of ascii / 2
			y++; // add 1 to y for next loop
		}

		if(ascii == 1) // when ascii is 1, we have to add 1 to the beginning
		{
			binary_reverse[y] = '1';
			y++;
		}

		if(y < 8) // add zeros to the end of string if not 8 characters (1 byte)
			for(; y < 8; y++) // add until binary_reverse[7] (8th element)
				binary_reverse[y] = '0';

		sprintf(binary, "");
		for(int z = 0; z < 8; z++) // our array is reversed. put the numbers in the rigth order (last comes first)
			sprintf(binary, "%s%c", binary, binary_reverse[7 - z]);
		
		sprintf(output_binary, "%s%s", output_binary, binary); // display the 8 digit binary number

		delete [] binary_reverse; // free the memory created by dynamic mem. allocation
		delete [] binary;
	}
}

/**
 * \fn
 * \brief Fonction de conversion d'une binaire en ASCII
 */

void ConversionsBinaireASCII::binaryToAscii(char* input_binary, char* output_ascii)
{
	int length = strlen(input_binary); // get length of string
	int binary[8]; // array used to store 1 byte of binary number (1 character)
	int asciiNum = 0; // the ascii number after conversion from binary
	char ascii; // the ascii character itself

	sprintf(output_ascii, "");
	int z = 0; // counter used

	for(int x = 0; x < length / 8; x++) // reading in bytes. total characters = length / 8
	{
		for(int a = 0; a < 8; a++) // store info into binary[0] through binary[7]
		{
			binary[a] = (int) input_binary[z] - 48; // z never resets
			z++;
		}

		int power[8]; // will set powers of 2 in an array
		int counter = 7; // power starts at 2^0, ends at 2^7
		for(int x = 0; x < 8; x++)
		{
			power[x] = counter; // power[] = {7, 6, 5, ..... 1, 0}
			counter--; // decrement counter each time
		}

		for(int y = 0; y < 8; y++) // will compute asciiNum
		{
			double a = binary[y]; // store the element from binary[] as "a"
			double b = power[y]; // store the lement from power[] as "b"

			asciiNum += a* pow(2, b); // asciiNum = sum of a * 2^power where 0 <= power <= 7, power is int
		}

		ascii = asciiNum; // assign the asciiNum value to ascii, to change it into an actual character
		asciiNum = 0; // reset asciiNum for next loop
		sprintf(output_ascii, "%s%c", output_ascii, ascii); // display the ascii character
	}
}