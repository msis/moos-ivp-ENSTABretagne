/***************************************************************************************************************:')

CoordSystemChg.h

Coordinate system handling in images.

Fabrice Le Bars

Created : 2009-01-24

Version status : To test and validate

***************************************************************************************************************:)*/

#ifndef COORDSYSTEMCHG_H
#define COORDSYSTEMCHG_H

#include "CoordSystem.h"

#define INVERT_X_COORDSYSTEMCHG 0x00000010
#define INVERT_Y_COORDSYSTEMCHG 0x00000020
#define SWAP_XY_COORDSYSTEMCHG 0x00000040

/*
Structure.
*/
struct _COORDSYSTEMCHG
{
	COORDSYSTEM cs1;
	COORDSYSTEM cs2;
	double X1X2Ratio;
	double Y1Y2Ratio;
	double X2X1Ratio;
	double Y2Y1Ratio;
};
typedef struct _COORDSYSTEMCHG COORDSYSTEMCHG;

/*
Initialize a structure.
See the description of the COORDSYSTEMCHG structure for more information.

COORDSYSTEMCHG* pCSChg : (INOUT) Valid pointer to the structure.
COORDSYSTEM* pCS1 : (IN).
COORDSYSTEM* pCS2 : (IN).

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitCSChg(COORDSYSTEMCHG* pCSChg, COORDSYSTEM* pCS1, COORDSYSTEM* pCS2)
{
	pCSChg->cs1 = *pCS1;
	pCSChg->cs2 = *pCS2;

	pCSChg->X1X2Ratio = (pCSChg->cs1.xMax - pCSChg->cs1.xMin) / (pCSChg->cs2.xMax - pCSChg->cs2.xMin);
	pCSChg->Y1Y2Ratio = (pCSChg->cs1.yMax - pCSChg->cs1.yMin) / (pCSChg->cs2.yMax - pCSChg->cs2.yMin);

	pCSChg->X2X1Ratio = 1.0/pCSChg->X1X2Ratio;
	pCSChg->Y2Y1Ratio = 1.0/pCSChg->Y1Y2Ratio;

	return EXIT_SUCCESS;
}

/*
Initialize a structure.
See the description of the COORDSYSTEMCHG structure for more information.

COORDSYSTEMCHG* pCSChg : (INOUT) Valid pointer to the structure.
COORDSYSTEM* pCS1 : (IN).
COORDSYSTEM* pCS2 : (IN).
int flags : (IN) A combination of INVERT_X, INVERT_Y, SWAP_XY.

Return : EXIT_SUCCESS or EXIT_FAILURE if there is an error.
*/
inline int InitCSChgEx(COORDSYSTEMCHG* pCSChg, COORDSYSTEM* pCS1, COORDSYSTEM* pCS2, int flags)
{
	pCSChg->cs1 = *pCS1;

	switch (flags)
	{
	case INVERT_X_COORDSYSTEMCHG:
		pCSChg->cs1 = *pCS1;
		pCSChg->cs2.xMin = pCS2->xMax;
		pCSChg->cs2.xMax = pCS2->xMin;
		pCSChg->cs2.yMin = pCS2->yMin;
		pCSChg->cs2.yMax = pCS2->yMax;
		break;
	case INVERT_Y_COORDSYSTEMCHG:
		pCSChg->cs2.xMin = pCS2->xMin;
		pCSChg->cs2.xMax = pCS2->xMax;
		pCSChg->cs2.yMin = pCS2->yMax;
		pCSChg->cs2.yMax = pCS2->yMin;
		break;
	case SWAP_XY_COORDSYSTEMCHG:
		pCSChg->cs2.xMin = pCS2->yMin;
		pCSChg->cs2.xMax = pCS2->yMax;
		pCSChg->cs2.yMin = pCS2->xMin;
		pCSChg->cs2.yMax = pCS2->xMax;
		break;
	case INVERT_X_COORDSYSTEMCHG|INVERT_Y_COORDSYSTEMCHG:
		pCSChg->cs2.xMin = pCS2->xMax;
		pCSChg->cs2.xMax = pCS2->xMin;
		pCSChg->cs2.yMin = pCS2->yMax;
		pCSChg->cs2.yMax = pCS2->yMin;
	case INVERT_X_COORDSYSTEMCHG|SWAP_XY_COORDSYSTEMCHG:
		pCSChg->cs2.xMin = pCS2->yMin;
		pCSChg->cs2.xMax = pCS2->yMax;
		pCSChg->cs2.yMin = pCS2->xMax;
		pCSChg->cs2.yMax = pCS2->xMin;
		break;
	case INVERT_Y_COORDSYSTEMCHG|SWAP_XY_COORDSYSTEMCHG:
		pCSChg->cs2.xMin = pCS2->yMax;
		pCSChg->cs2.xMax = pCS2->yMin;
		pCSChg->cs2.yMin = pCS2->xMin;
		pCSChg->cs2.yMax = pCS2->xMax;
		break;
	case INVERT_X_COORDSYSTEMCHG|INVERT_Y_COORDSYSTEMCHG|SWAP_XY_COORDSYSTEMCHG:
		pCSChg->cs2.xMin = pCS2->yMax;
		pCSChg->cs2.xMax = pCS2->yMin;
		pCSChg->cs2.yMin = pCS2->xMax;
		pCSChg->cs2.yMax = pCS2->xMin;
		break;
	default:
		pCSChg->cs2 = *pCS2;
		break;
	}

	pCSChg->X1X2Ratio = (pCSChg->cs1.xMax - pCSChg->cs1.xMin) / (pCSChg->cs2.xMax - pCSChg->cs2.xMin);
	pCSChg->Y1Y2Ratio = (pCSChg->cs1.yMax - pCSChg->cs1.yMin) / (pCSChg->cs2.yMax - pCSChg->cs2.yMin);

	pCSChg->X2X1Ratio = 1.0/pCSChg->X1X2Ratio;
	pCSChg->Y2Y1Ratio = 1.0/pCSChg->Y1Y2Ratio;

	return EXIT_SUCCESS;
}

inline int CS1ToCS2Chg(COORDSYSTEMCHG* pCSChg, double x1, double y1, double* pX2, double* pY2)
{
	*pX2 = (x1 - pCSChg->cs1.xMin) * pCSChg->X2X1Ratio + pCSChg->cs2.xMin;
	*pY2 = (y1 - pCSChg->cs1.yMin) * pCSChg->Y2Y1Ratio + pCSChg->cs2.yMin;

	return EXIT_SUCCESS;
}

inline int CS2ToCS1Chg(COORDSYSTEMCHG* pCSChg, double* pX1, double* pY1, double x2, double y2)
{
	*pX1 = (x2 - pCSChg->cs2.xMin) * pCSChg->X1X2Ratio + pCSChg->cs1.xMin;
	*pY1 = (y2 - pCSChg->cs2.yMin) * pCSChg->Y1Y2Ratio + pCSChg->cs1.yMin;

	return EXIT_SUCCESS;
}

#endif // COORDSYSTEMCHG_H
