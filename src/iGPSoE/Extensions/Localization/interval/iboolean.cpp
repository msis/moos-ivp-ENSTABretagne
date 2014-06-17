#include "iboolean.h"

iboolean::iboolean ()
{
	value = iperhaps;
}

iboolean::iboolean (bool b)
{
	if (b) value=itrue; else value=ifalse;
}

iboolean::iboolean (IBOOLEAN tt)
{
	value = tt;
}

iboolean::iboolean (const iboolean& t)
{
	*this = t;
}

iboolean operator&& (iboolean x, iboolean y)
{
	if ( (x.value==ifalse) || (y.value==ifalse) ) return iboolean(ifalse);
	if ( (x.value==itrue) && (y.value==itrue) ) return iboolean(itrue);
	return iboolean(iperhaps);
}

iboolean operator|| (iboolean& x, iboolean& y)
{	
	if ( (x.value==itrue) || (y.value==itrue) ) return iboolean(itrue);
	if ( (x.value==iperhaps) || (y.value==iperhaps) ) return iboolean(iperhaps);
	return iboolean(ifalse);
}

bool operator== (iboolean x, iboolean y)
{	
	return (x.value==y.value);
}

bool operator!= (iboolean x, iboolean y)
{	
	return (x.value!=y.value);
}


iboolean operator! (iboolean& x)
{	
	if (x.value == itrue) return iboolean(ifalse);
	if (x.value == ifalse) return iboolean(itrue);
	return iboolean(iperhaps);
}

iboolean Inter(iboolean a, iboolean b)
{	
	if ((a.value == itrue)&&(b.value == ifalse)) return iboolean(empty);
	if ((a.value == itrue)||(b.value == itrue)) return iboolean(itrue);
	if ((a.value == ifalse)||(b.value == ifalse)) return iboolean(ifalse);
	return iboolean(iperhaps);
}




