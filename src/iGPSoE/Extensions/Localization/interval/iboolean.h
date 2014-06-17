#ifndef __IBOOLEAN__
#define __IBOOLEAN__
// IBOOLEAN is an interval Boolean. Used for a trivalued logic
// if x=[0,0]=ifalse  : certainly false
// if x=[0,1]=iperhaps: don't know
// if x=[1,1]=itrue  : certainly true
// Otherwise x=empty


#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


enum IBOOLEAN {itrue, ifalse, iperhaps, empty};

class iboolean
{
public:
	IBOOLEAN value;
public:
	iboolean ();
	iboolean (bool);
	iboolean (IBOOLEAN);
	iboolean (const iboolean&);
	friend iboolean operator&& (iboolean, iboolean);
	friend iboolean operator|| (iboolean&, iboolean&);
	friend iboolean operator!  (iboolean&);
	friend bool		 operator== (iboolean, iboolean);
	friend bool		 operator!= (iboolean, iboolean);
	friend iboolean Inter(iboolean,iboolean);
};

#endif
