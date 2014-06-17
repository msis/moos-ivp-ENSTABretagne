#ifndef SONAR_H
#define SONAR_H

#include "localisation.h"

class Sonar{

	public :

		static void getIndMax(unsigned int *raw, int size, int *ind);
		static void localisation(float *rho, float *tt, int size, PARAM *param, POS *pos, float cap, int mode);
};

#endif // SONAR_H