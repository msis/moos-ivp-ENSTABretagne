/**
 * \file Matrice.h
 * \brief Classe Matrice
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe de manipulation simples de matrices
 *
 */

#ifndef MATRICE_H
#define MATRICE_H

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <math.h>

#include "../common/constantes.h"

#define EPSILON		0.00000001 // Précision réel machine

class Matrice
{
	protected:
		int colonnes;
		int lignes;
		double **tableau;	// Le double tableau représentatif de la matrice
		std::string nom_matrice;	// Le nom de la matrice, tel qu'il sera affiché dans la console via la méthode afficher()

	public :
		Matrice(int nombre_lignes, int nombre_colonnes, std::string nom);
		void initialisation();
		double * & operator[](int rang);
		void changerNom(std::string nouveau_nom);
		void inverser();
		Matrice operator * (const Matrice &X);
		Matrice operator + (const Matrice &X);
		void operator = (const Matrice &X);
		int nombreLignes();
		int nombreColonnes();
		void afficher();
		~Matrice();
};

#endif // MATRICE_H
