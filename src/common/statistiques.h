/**
 * \file Tests.h
 * \brief Classe contenant des fonctions de statistiques
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 26th 2013
 */

#ifndef STATISTIQUES
#define STATISTIQUES

#include <string>
#include <fstream>
#include <cmath>
#include <algorithm>
#include "ColorParse.h"
#include "constantes.h"

using namespace std;

class Statistiques
{
	public:
	
		/**
		 * \fn
		 * \brief Médiane sur les valeurs d'un vecteur d'entiers
		 */
		 
		static double mediane(vector<int> vec)
		{
			typedef vector<int>::size_type vec_sz;

			vec_sz size = vec.size();
			/*if (size == 0)
				throw domain_error("median of an empty vector");*/

			sort(vec.begin(), vec.end());
			vec_sz mid = size/2;

			return size % 2 == 0 ? (vec[mid] + vec[mid-1]) / 2 : vec[mid];
		}
	
		/**
		 * \fn
		 * \brief Médiane sur les valeurs d'un vecteur de float
		 */
		 
		static double mediane(vector<float> vec)
		{
			typedef vector<float>::size_type vec_sz;

			vec_sz size = vec.size();
			/*if (size == 0)
				throw domain_error("median of an empty vector");*/

			sort(vec.begin(), vec.end());
			vec_sz mid = size/2;

			return size % 2 == 0 ? (vec[mid] + vec[mid-1]) / 2 : vec[mid];
		}

		/**
		 * \fn
		 * \brief Médiane sur les valeurs d'un vecteur d'entiers
		 */
		 
		static double moyenne(vector<int> vec)
		{
			double sum = 0;
			
			for(int i = 0 ; i < (int)vec.size() ; i++)
				sum += vec[i];
				
			return sum / vec.size();
		}

		/**
		 * \fn
		 * \brief Écart type sur les valeurs d'un vecteur d'entiers
		 */
		 
		static double ecartType(vector<int> vec)
		{
			double E = 0;
			double m = moyenne(vec);
			
			for(int i = 0 ; i < (int)vec.size() ; i++)
				E += (vec[i] - m) * (vec[i] - m);
				
			return sqrt(1.0 / vec.size() * E);
		}
};

#endif 