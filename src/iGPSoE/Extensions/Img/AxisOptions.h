/***************************************************************************************************************:')

AxisOptions.h

Options of the axis drawn in a window (axis position, axis step...).

Fabrice Le Bars

Created : 2011-04-03

Version status : To test and validate

***************************************************************************************************************:)*/

#ifndef AXISOPTIONS_H
#define AXISOPTIONS_H

#include "OSCore.h"

/*
Structure that defines the options of axis.
*/
struct _AXISOPTIONS
{
	double xAxisPos;
	double xAxisStep;
	double yAxisPos;
	double yAxisStep;
};
typedef struct _AXISOPTIONS AXISOPTIONS;

#endif // AXISOPTIONS_H
