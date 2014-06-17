#ifndef _COMMON_H
#define _COMMON_H


#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE 1
#endif // _CRT_SECURE_NO_DEPRECATE

#include <list>
#include "interval/box.h"

using namespace std;


#define PI 3.1415926535897932384626433832795
#define RadToDeg (180.0/PI)
#define PI_sur_180 (PI/180.0)


char* fgets2(FILE* f,char* line);


#endif // _COMMON_H
