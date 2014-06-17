#include "GPSoEUtils.h"

#define SZDEV "192.168.0.9:4001"

#ifdef USE_GPSOEMGR
int main()
{
	HGPSOEMGR hGPSoEMgr = INVALID_HGPSOEMGR_VALUE;
	double utc = 0;
	double latitude = 0;
	double longitude = 0;
	int i = 0;

	INIT_DEBUG;

	InitGPSoEMgr(&hGPSoEMgr, (char*)SZDEV, 100, DATA_POLLING_MODE_GPSOE, THREAD_PRIORITY_NORMAL, TRUE);

	for (i = 0; i < 1000; i++)
	{
		if (GetDataGPSoEMgr(hGPSoEMgr, &utc, &latitude, &longitude) == EXIT_SUCCESS)
		{
			printf("UTC : %f, Latitude : %f, Longitude : %f\n", utc, latitude, longitude);
		}
		mSleep(200);
	}
	
	ReleaseGPSoEMgr(&hGPSoEMgr);

	printf("Press ENTER to continue . . .");(void)getchar();

	return EXIT_SUCCESS;
}
#else
int main()
{
	HGPSOE hGPSoE = INVALID_HGPSOE_VALUE;
	double utc = 0;
	double latitude = 0;
	double longitude = 0;

	INIT_DEBUG;

	OpenGPSoE(&hGPSoE, SZDEV);
	GetDataGPSoE(hGPSoE, &utc, &latitude, &longitude);
	printf("UTC : %f, Latitude : %f, Longitude : %f\n", utc, latitude, longitude);
	CloseGPSoE(&hGPSoE);

	printf("Press ENTER to continue . . .");(void)getchar();

	return EXIT_SUCCESS;
}
#endif // USE_GPSOEMGR
