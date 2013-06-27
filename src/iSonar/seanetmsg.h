#ifndef SEANETMSG_H
#define SEANETMSG_H

#include <string>
#include <stdint.h>


class SeaNetMsg
{    
protected:
    std::string m_data;
    
public:
    enum MessageReadError {
        mrNotAMessage = -1,
        mrNotEnoughData = -2,
    };
    
    enum MessageTypeID {
        mteNoMessageType = -1,
        mtNull = 0,
        mtVersionData,
        mtHeadData,
        mtAlive = 4,
        mtBBUserData = 6,
        mtReBoot = 16,
        mtHeadCommand = 19,
        mtSendVersion = 23,
        mtSendBBuser,
        mtSendData
    };

protected:
    uint8_t read_uchar_at(int offset) const {return *(uint8_t *)(m_data.data()+offset);}
    uint16_t read_ushort_at(int offset) const {return *(uint16_t *)(m_data.data()+offset);}
    uint32_t read_ulong_at(int offset) const {return *(uint32_t *)(m_data.data()+offset);}
    
    void set_uchar_at(int offset, uint8_t value) {m_data[offset] = value;}
    void set_ushort_at(int offset, uint16_t value) {m_data[offset] = value & 0xFF; m_data[offset+1] = (value & 0xFF00) >> 8;}
    void set_ulong_at(int offset, uint32_t value) {m_data[offset] = value & 0xFF; m_data[offset+1] = (value & 0xFF00) >> 8;  m_data[offset+2] = (value & 0xFF0000) >> 16; m_data[offset+3] = (value & 0xFF000000) >> 24; }
    
public:
    SeaNetMsg();
    SeaNetMsg(const std::string &data);
    virtual ~SeaNetMsg();
    
    const std::string & data() const {return m_data;};
    MessageTypeID messageType() const {return (MessageTypeID) read_uchar_at(10);}
    void setMessageType(MessageTypeID msg_type) {set_uchar_at(10, (uint8_t)msg_type);}
    
    void print_hex(int max_bytes = 50) const;
    
public: // Static member functions
    static bool startsWithHeader(const std::string &data);
    static int readMsgLength(const std::string &data);
    static MessageTypeID detectMessageType(const std::string &data);
    static int numberBytesMissing(const std::string &data, int &full_message_length);
    static SeaNetMsg * fromData(const std::string &data);
};

class SeaNetMsg_SendVersion : public SeaNetMsg
{
public:
    SeaNetMsg_SendVersion() : SeaNetMsg() {
        const char msg[] = {0x40,0x30,0x30,0x30,0x38,0x08,0x00,0xFF,0x02,0x03,0x17,0x80,0x02,0x0A};
        m_data.assign(msg, sizeof(msg));
    }
};

class SeaNetMsg_SendBBUser : public SeaNetMsg
{
public:
    SeaNetMsg_SendBBUser() : SeaNetMsg() {
        const char msg[] = {0x40,0x30,0x30,0x30,0x38,0x08,0x00,0xFF,0x02,0x03,0x18,0x80,0x02,0x0A};
        m_data.assign(msg, sizeof(msg));
    }
};

class SeaNetMsg_ReBoot : public SeaNetMsg
{
public:
    SeaNetMsg_ReBoot() : SeaNetMsg() {
        const char msg[] = {0x40,0x30,0x30,0x30,0x38,0x08,0x00,0xFF,0x02,0x03,0x10,0x80,0x02,0x0A};
        m_data.assign(msg, sizeof(msg));
    }
};

class SeaNetMsg_SendData : public SeaNetMsg
{
public:
    SeaNetMsg_SendData() : SeaNetMsg() {
        const char msg[] = {0x40,0x30,0x30,0x30,0x43,0x0C,0x00,0xFF,0x02,0x07,0x19,0x80,0x02,0xCA,0x64,0xB0,0x03,0x0A}; //18
        m_data.assign(msg, sizeof(msg));
    }
    
    void setTime(double time_in_sec) { uint32_t t_ms = time_in_sec*1000.0; set_ulong_at(13,t_ms);}
    
};

class SeaNetMsg_HeadCommand : public SeaNetMsg
{
public:
    SeaNetMsg_HeadCommand(); /*: SeaNetMsg() {
        const char msg[] = {0x40,0x30,0x30,0x30,0x43,0x0C,0x00,0xFF,0x02,0x07,0x19,0x80,0x02,0xCA,0x64,0xB0,0x03,0x0A}; //18
        m_data.assign(msg, sizeof(msg));
    }*/    
};

class SeaNetMsg_HeadData : public SeaNetMsg
{
public:
    SeaNetMsg_HeadData() : SeaNetMsg() {};
    
    double bearing() const { uint16_t bearing_grad = read_ushort_at(40); return 360./6400.*bearing_grad;}
    double ADInterval_s() const { uint16_t ad_int_640ns = read_ushort_at(33); return 640.e-9*ad_int_640ns;}
    double ADInterval_m() const { return 1500.*ADInterval_s();}
    int nBins() const { return read_ushort_at(42); }
    
    const uint8_t * scanlineData() const { return reinterpret_cast<const uint8_t *>(m_data.data()+44); }
    uint8_t bin(int i) const { return m_data[44+i]; }
    
    double firstObstacleDist( uint8_t threshold, const double& min_dist, const double& max_dist) const;
    
};

#endif // SEANETMSG_H
