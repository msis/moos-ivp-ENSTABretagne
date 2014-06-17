#include "sonar.h"
#include "localisation.h"

void Sonar :: getIndMax(unsigned int *raw, int size, int *ind){

	unsigned int max = 0;
	for(int i = 0; i < size; i++){
		if(max < raw[i]){
			max = raw[i];
			*ind = i;
		}
	}
}

void Sonar :: localisation(float *rho, float *tt, int size, PARAM *param, POS *pos, float cap, int mode){

	localisation( rho, tt, size, param, pos, cap, mode);
}