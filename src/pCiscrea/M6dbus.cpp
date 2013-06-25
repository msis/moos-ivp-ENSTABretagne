/**
 * \file M6dbus.cpp
 * \brief Classe M6dbus
 * \author Team CISSAU - Veni Vidi Vici (ENSTA Bretagne)
 * \version 0.1
 * \date Jun 5th 2013
 *
 * Classe permettant la manipulation de l'AUV par lecture et écriture dans ses registres
 * Utilisation du protocole de communication Modbus
 *
 */

#include "M6dbus.h"
#include "ColorParse.h"
#include <cerrno>
#include <time.h>

M6dbus::M6dbus(string IP, int prt)
{
    on = false;
    //Modbus = modbus_new_tcp((char*)IP.c_str(),prt);
    Modbus = modbus_new_rtu("/dev/ttyUSB0", 38400, 'N', 8, 1);
    modbus_set_slave(Modbus, 16);
    modbus_set_debug(Modbus, TRUE);
    modbus_set_debug(Modbus, FALSE);
    modbus_set_error_recovery(Modbus,
        (modbus_error_recovery_mode)(MODBUS_ERROR_RECOVERY_LINK|MODBUS_ERROR_RECOVERY_PROTOCOL));

	struct timeval old_response_timeout;
	struct timeval response_timeout;

	/* Save original timeout */
	modbus_get_response_timeout(Modbus, &old_response_timeout);
	response_timeout.tv_sec = 0;
	response_timeout.tv_usec = 500000;
	modbus_set_response_timeout(Modbus, &response_timeout);
	cout << "Response timeout : " << old_response_timeout.tv_sec << "s - us : " << old_response_timeout.tv_usec << endl;

	struct timeval old_byte_timeout;
	struct timeval byte_timeout;

	/* Save original timeout */
	modbus_get_byte_timeout(Modbus, &old_byte_timeout);
	cout << "Byte timeout : " << old_byte_timeout.tv_sec << "s - us : " << old_byte_timeout.tv_usec << endl;

	/* Define a new and too short timeout! */
	byte_timeout.tv_sec = 0;
	byte_timeout.tv_usec = 500000;
	modbus_set_byte_timeout(Modbus, &byte_timeout);

    if(modbus_connect(Modbus)==-1)
    {
        cerr << "Connection failed!" << endl;
        cout << "Unable to connect to host. M6dbus is going down..." << endl;
        modbus_free(Modbus);
        return;
    }
    else
    {
        on = true;
        cout << "Connected to " << IP << "!" << endl;
        cout << "Reading registers..." << endl;
    }
    
    for(int i = 0 ; i < 10 ; i ++)
		modbus_write_bit(Modbus, 0, true);
	
    if(modbus_read_registers(Modbus,1,48,regTab)==-1)
    {
        cerr << "Unable to read the registers!" << endl;
        cout << "Please try to read them again" << endl;
    }
    
    else
    {
        cout << "Register table is up-to-date!" << endl;
    }
    
    // Initialisation des registres liés aux propulseurs
	m_RegPropFr = regTab[1];
	m_RegPropRe = regTab[2];
	m_RegPropVert = regTab[3];
}

M6dbus::~M6dbus()
{
    modbus_close(Modbus);
}

bool M6dbus::getOn()
{
    return on;
}

int M6dbus::updateRegTab(uint16_t* Tab)
{
	//usleep(40*1000);
    int resultat = modbus_read_registers(Modbus,1,48,Tab);
    
    /*if(resultat == -1)
		cout << termColor("red") << "Connection failed: " << modbus_strerror(errno) << endl << termColor();*/
		
	usleep(40*1000);
	return resultat;
}

int M6dbus::writeReg(int reg, int val)
{
    int r;
    modbus_flush(Modbus);
    r = modbus_write_register(Modbus,reg,val);
    modbus_flush(Modbus);
    updateAll();
    return r;
}

int M6dbus::getRegNum(int n)
{
    if (updateAll() == -1)
        return -1;
    return regTab[n+1];
}

int M6dbus::updateAll()
{
	int ret;
	ret = updateRegTab(regTab);
	waterInC = ((regTab[10] >> 7) && 0x1);
	depth = regTab[32];
	direction = regTab[33];
	U1 = regTab[34];
	U2 = regTab[35];
	I1 = regTab[36];
	I2 = regTab[37];
	tempMB = regTab[38];
	tempOut = regTab[39];
	tempEx1 = regTab[40];
	tempEx2 = regTab[41];
	tempEx3 = regTab[42];
	capBat1 = regTab[43];
	capBat2 = regTab[44];
	version = regTab[47];
	alarms = regTab[31];
	auvTime[0] = regTab[16];
	auvTime[1] = regTab[17];
	auvTime[2] = regTab[18];
	auvTime[3] = regTab[19];
	return ret;
}

int M6dbus::turnLightOn(int intensity)
{
    int readErr,writeErr;

    int tempReg1,tempReg5,newReg1,newReg5;

    //turnLightOff();

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    tempReg5=regTab[4];
    newReg1 = tempReg1 | 0xF;       //turning on lights
    newReg5 = intensity & 0xFF;     //using the LSB only
    newReg5 = tempReg5 | newReg5;   //putting the intensity value in the register

    //TODO: check who comes first?
    writeErr=writeReg(5,newReg5);
    if (writeErr==-1)
        return writeErr;
    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::turnLightOff()
{
    int readErr,writeErr;
    int tempReg1,newReg1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    newReg1=tempReg1&0xFFF0;    //clearing the first byte

    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::turnLightOn(int numL, int intensity)
{
    int readErr,writeErr;

    int tempReg1,tempReg5,newReg1,newReg5;

    if (numL > 4 && numL <= 0)
        return -1;

    //turnLightOff();

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    tempReg5=regTab[4];
    newReg1 = tempReg1 | (0x1 << numL); //setting the corresponding bit
    newReg5 = intensity & 0xFF;     //using the LSB only
    newReg5 = tempReg5 | newReg5;   //putting the intensity value in the register

    //TODO: check who comes first?
    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    writeErr=writeReg(5,newReg5);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::turnLightOff(int numL)
{
    int readErr,writeErr;
    int tempReg1,newReg1;

    if (numL > 4 && numL <= 0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    switch(numL) //clearing the correspending bit
    {
    case 1:
        newReg1=tempReg1&0xFFFE;
        break;
    case 2:
        newReg1=tempReg1&0xFFFD;
        break;
    case 3:
        newReg1=tempReg1&0xFFFB;
        break;
    case 4:
        newReg1=tempReg1&0xFFF7;
        break;
    default :
        return -1;
    }

    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::turnLaserOn(int numL)
{
    int readErr,writeErr;

    int tempReg1,newReg1;

    if (numL > 2 && numL <= 0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    newReg1 = tempReg1 | (0x1 << (numL+4)); //setting the corresponding bit

    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::turnLaserOff(int numL)
{
    int readErr,writeErr;
    int tempReg1,newReg1;

    if (numL > 2 && numL <= 0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    switch(numL) //clearing the correspending bit
    {
    case 1:
        newReg1=tempReg1&0xFFEF;
        break;
    case 2:
        newReg1=tempReg1&0xFFDF;
        break;
    default :
        return -1;
    }

    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setModeLogic()
{
    int readErr,writeErr;

    int tempReg1,newReg1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    newReg1 = tempReg1 | 0x8000;

    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setModePower()
{
    int readErr,writeErr;

    int tempReg1,newReg1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempReg1=regTab[0];
    newReg1 = tempReg1 | 0x4000;

    writeErr=writeReg(1,newReg1);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCamera(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam | 0x1;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCamLights(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam | 0x2;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::clrCamLights(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam & 0xFFFD;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCameraBSE(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam | 0x4;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCameraBDO(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam | 0x8;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCameraBUP(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam | 0x10;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCameraBRI(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam | 0x20;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCameraBLE(int cam)
{
    int readErr,writeErr;

    int tempRegCam,newRegCam;

    if (cam > 4 && cam <=0)
        return -1;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegCam=regTab[9+cam];
    newRegCam = tempRegCam | 0x40;

    writeErr=writeReg(10+cam,newRegCam);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::setCamTilt(int val)
{
    int readErr,writeErr;

    int tempRegTilt,newRegTilt,tiltVal;

    if (val > 100)
        val = 100;
    if (val < -100)
        val = -100;
    if (val == 0)
        tiltVal = 0x7D;
    if (val > 0)
        tiltVal = 0x7D + ((val * 0x47) / 100) + Offcet;
    if (val < 0)
        tiltVal = 0x7D + ((val * 0x47) / 100) - Offcet;



    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegTilt = regTab[5];
    newRegTilt = (tempRegTilt & 0x00FF) | (tiltVal << 8);

    writeErr=writeReg(6,newRegTilt);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::updatePropulsors()
{
    if (writeReg(2,m_RegPropFr)==-1 || writeReg(3,m_RegPropRe)==-1 || writeReg(4,m_RegPropVert)==-1)
        return -1;
        
    return 1;
}

int M6dbus::setPropFrLe(int val)
{
    int readErr,writeErr;

    int tempRegPropFr,newRegPropFr,propVal;

    if (val > 100)
        val = 100;
    if (val < -100)
        val = -100;
    if (val == 0)
        propVal = 0x7D;
    if (val > 0)
        propVal = 0x7D + ((val * 0x47) / 100) + Offcet;
    if (val < 0)
        propVal = 0x7D + ((val * 0x47) / 100) - Offcet;

    /*readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegPropFr = regTab[1];*/
    m_RegPropFr = (m_RegPropFr & 0xFF00) | (propVal);
	
    /*writeErr=writeReg(2,m_RegPropFr);
    if (writeErr==-1)
        return writeErr;*/
    return 1;
}

int M6dbus::setPropFrRi(int val)
{
    int readErr,writeErr;

    int tempRegPropFr,newRegPropFr,propVal;

    if (val > 100)
        val = 100;
    if (val < -100)
        val = -100;
    if (val == 0)
        propVal = 0x7D;
    if (val > 0)
        propVal = 0x7D + ((val * 0x47) / 100) + Offcet;
    if (val < 0)
        propVal = 0x7D + ((val * 0x47) / 100) - Offcet;

    /*readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegPropFr = regTab[1];*/
    m_RegPropFr = (m_RegPropFr & 0x00FF) | (propVal << 8);

    /*writeErr=writeReg(2,m_RegPropFr);
    if (writeErr==-1)
        return writeErr;*/
    return 1;
}

int M6dbus::setPropReLe(int val)
{
    int readErr,writeErr;

    int tempRegPropFr,newRegPropFr,propVal;

    if (val > 100)
        val = 100;
    if (val < -100)
        val = -100;
    if (val == 0)
        propVal = 0x7D;
    if (val > 0)
        propVal = 0x7D + ((val * 0x47) / 100) + Offcet;
    if (val < 0)
        propVal = 0x7D + ((val * 0x47) / 100) - Offcet;

    /*readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;
        
    tempRegPropFr = regTab[2];*/
    m_RegPropRe = (m_RegPropRe & 0xFF00) | (propVal);

    /*writeErr=writeReg(3,m_RegPropRe);
    if (writeErr==-1)
        return writeErr;*/
    return 1;
}

int M6dbus::setPropReRi(int val)
{
    int readErr,writeErr;

    int tempRegPropFr,newRegPropFr,propVal;

    if (val > 100)
        val = 100;
    if (val < -100)
        val = -100;
    if (val == 0)
        propVal = 0x7D;
    if (val > 0)
        propVal = 0x7D + ((val * 0x47) / 100) + Offcet;
    if (val < 0)
        propVal = 0x7D + ((val * 0x47) / 100) - Offcet;

    /*readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegPropFr = regTab[2];*/
    m_RegPropRe = (m_RegPropRe & 0x00FF) | (propVal << 8);

    /*writeErr=writeReg(3,m_RegPropRe);
    if (writeErr==-1)
        return writeErr;*/
    return 1;
}

int M6dbus::setPropVert(int val)
{
    int readErr,writeErr;

    int tempRegPropFr,newRegPropFr,propVal;

    if (val > 100)
        val = 100;
    if (val < -100)
        val = -100;
    if (val == 0)
        propVal = 0x7D;
    if (val > 0)
        propVal = 0x7D + ((val * 0x47) / 100) + Offcet;
    if (val < 0)
        propVal = 0x7D + ((val * 0x47) / 100) - Offcet;

    /*readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegPropFr = regTab[3];*/
    m_RegPropVert = (m_RegPropVert & 0x00FF) | (propVal << 8);

    /*writeErr=writeReg(4,m_RegPropVert);
    if (writeErr==-1)
        return writeErr;*/
    return 1;
}

int M6dbus::getClockR4Rd()
{
    int readErr,writeErr;

    int tempRegAux,newRegAux;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegAux = regTab[15];
    newRegAux = tempRegAux | 0x1;

    writeErr=writeReg(16,newRegAux);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::getClockR4Wr()
{
    int readErr,writeErr;

    int tempRegAux,newRegAux;

    readErr = updateRegTab(regTab);
    if (readErr==-1)
        return readErr;

    tempRegAux = regTab[15];
    newRegAux = tempRegAux | 0x2;

    writeErr=writeReg(16,newRegAux);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int* M6dbus::getAuvTime()
{
    if (getClockR4Rd() == -1)
        return NULL;
    if (updateAll() == -1)
        return NULL;
    return auvTime;
}

//TODO: check grammar
int M6dbus::setAuvTime(int* time)
{
    int writeErr;

    int newRegTime[4];

    if (getClockR4Wr() == -1)
        return -1;
    if (updateAll() == -1)
        return -1;

    *newRegTime = *time;

    writeErr=writeReg(17,newRegTime[0]);
    if (writeErr==-1)
        return writeErr;
    writeErr=writeReg(18,newRegTime[1]);
    if (writeErr==-1)
        return writeErr;
    writeErr=writeReg(19,newRegTime[2]);
    if (writeErr==-1)
        return writeErr;
    writeErr=writeReg(20,newRegTime[3]);
    if (writeErr==-1)
        return writeErr;
    return 1;
}

int M6dbus::getAlarms()
{
    if (updateAll()==-1)
        return -1;
    waterIn1 = ((alarms & 0x1) && 0x1);
    waterIn2 = ((alarms & 0x2) && 0x2) ;
    waterIn3 = ((alarms & 0x4) && 0x4) ;
    hiTempIn = ((alarms & 0x8) && 0x8) ;
    lowU1 = ((alarms & 0x20) && 0x20) ;
    lowU2 = ((alarms & 0x40) && 0x40) ;
    highI1 = ((alarms & 0x80) && 0x80) ;
    highI2 = ((alarms & 0x100) && 0x100) ;
    maxDepth = ((alarms & 0x200) && 0x200);
    errRConf = ((alarms & 0x1000) && 0x1000);
    errSPI = ((alarms & 0x2000) && 0x2000);
    errI2C = ((alarms & 0x4000) && 0x4000);
    errLink = ((alarms & 0x8000) && 0x8000);
    return alarms;
}

bool M6dbus::getWaterIn1()
{
    if (getAlarms() == -1)
        return -1;

    return waterIn1;
}

bool M6dbus::getWaterIn2()
{
    if (getAlarms() == -1)
        return -1;

    return waterIn2;
}

bool M6dbus::getWaterIn3()
{
    if (getAlarms() == -1)
        return -1;

    return waterIn3;
}

bool M6dbus::getWaterInCam()
{
    if (getAlarms() == -1)
        return -1;

    return waterInC;
}

bool M6dbus::getHiTempIn()
{
    if (getAlarms() == -1)
        return -1;

    return hiTempIn;
}

bool M6dbus::getLowU1()
{
    if (getAlarms() == -1)
        return -1;

    return lowU1;
}

bool M6dbus::getLowU2()
{
    if (getAlarms() == -1)
        return -1;

    return lowU2;
}

bool M6dbus::getHighI1()
{
    if (getAlarms() == -1)
        return -1;

    return highI1;
}

bool M6dbus::getHighI2()
{
    if (getAlarms() == -1)
        return -1;

    return highI2;
}

bool M6dbus::getMaxDepth()
{
    if (getAlarms() == -1)
        return -1;

    return maxDepth;
}

bool M6dbus::getErrRConf()
{
    if (getAlarms() == -1)
        return -1;

    return errRConf;
}

bool M6dbus::getErrI2C()
{
    if (getAlarms() == -1)
        return -1;

    return errI2C;
}

bool M6dbus::getErrSPI()
{
    if (getAlarms() == -1)
        return -1;

    return errSPI;
}

bool M6dbus::getErrLink()
{
    if (getAlarms() == -1)
        return -1;

    return errLink;
}

int M6dbus::getDepth()
{
    if (updateAll() == -1)
        return -1;

    return depth;
}

int M6dbus::getDirec()
{
    if (updateAll() == -1)
        return -1;

    return direction;
}

int M6dbus::getU1()
{
    if (updateAll() == -1)
        return -1;

    return U1;
}

int M6dbus::getU2()
{
    if (updateAll() == -1)
        return -1;

    return U2;
}

int M6dbus::getI1()
{
    if (updateAll() == -1)
        return -1;

    return I1;
}

int M6dbus::getI2()
{
    if (updateAll() == -1)
        return -1;

    return I2;
}

int M6dbus::getTempMB()
{
    if (updateAll() == -1)
        return -1;

    return tempMB;
}

int M6dbus::getTempOut()
{
    if (updateAll() == -1)
        return -1;

    return tempOut;
}

int M6dbus::getTempEx1()
{
    if (updateAll() == -1)
        return -1;

    return tempEx1;
}

int M6dbus::getTempEx2()
{
    if (updateAll() == -1)
        return -1;

    return tempEx2;
}

int M6dbus::getTempEx3()
{
    if (updateAll() == -1)
        return -1;

    return tempEx3;
}

int M6dbus::getCapBat1()
{
    if (updateAll() == -1)
        return -1;

    return capBat1;
}

int M6dbus::getCapBat2()
{
    if (updateAll() == -1)
        return -1;

    return capBat2;
}

int M6dbus::getVersion()
{
    if (updateAll() == -1)
        return -1;

    return version;
}
