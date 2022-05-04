#ifndef ARDUINO_SIP_H
#define ARDUINO_SIP_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <WiFiUdp.h>
#include <Ticker.h>

class Sip
{
  public:
    Sip(char *pBuf, size_t lBuf);
    ~Sip();


    void        Init(const char *SipIp, int SipPort, const char *MyIp, int MyPort, const char *SipUser, const char *SipPassWd, int RegTimeout, int MaxDialSec = 10);
    bool        Dial(const char *DialNr, const char *DialDesc = "");
    void        Processing(char *pBuf, size_t lBuf);
    bool        IsBusy() {
      return iRingTime != 0;
    }
    void        Subscribe(int expire = 600);
    void        Register(const char *pIn = 0);
    void        loop(char *acSipIn, int len);   // what needs to be done in the main loop
    void        setCallCallback(void (*callCallback)(const char * from)); // what to do when we recevie a call
    void        setCancelCallback(void (*cancelCallback)(const char * from)); // what to do when we recevie a call

  private:
    char       *pbuf;
    size_t      lbuf;
    char        caRead[256];
    void        (*fcallCallback)(const char * from); // callback when a call is signaled
    void        (*fcancelCallback)(const char * from); // callback when a call is canceled

    const char *pSipIp;
    int         iSipPort;
    const char *pSipUser;
    const char *pSipPassWd;
    const char *pMyIp;
    int         iMyPort;
    const char *pDialNr;
    const char *pDialDesc;


    uint32_t    callid;
    uint32_t    tagid;
    uint32_t    branchid;

    int         iAuthCnt;
    uint32_t    iRingTime;
    uint32_t    iMaxTime;
    int         iDialRetries;
    int         iLastCSeq;
    int         iRegTime;

    Ticker reregister;
    // void callbackReregister();  // reregister Ticker callback
    int regExpire = 600;         // registration expires in seconds

    WiFiUDP   Udp;

    void        HandleUdpPacket(const char *p);
    void        AddSipLine(const char* constFormat , ... );
    bool        AddCopySipLine(const char *p, const char *psearch);
    bool        ParseParameter(char *dest, int destlen, const char *name, const char *line, char cq = '\"');
    bool        ParseReturnParams(const char *p);
    int         GrepInteger(const char *p, const char *psearch);
    void        Ack(const char *pIn);
    void        Cancel(int seqn);
    void        Bye(int cseq);
    void        Ok(const char *pIn);
    void        Ringing(const char *pIn);
    void        Invite(const char *pIn = 0);


    uint32_t    Millis();
    uint32_t    Random();
    int         SendUdp();
    void        MakeMd5Digest(char *pOutHex33, char *pIn);

};

#endif  // ARDUINO_SIP_H
