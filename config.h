/*
 * configuration parameters 
 */

#ifndef CONFIG_H
#define CONFIG_H

//
// WiFi parameters
//
// WiFi SSID'S and passwords
// the strongest WiFi station will be used
const char* ssid = "EnterSSIDHere";
const char* password =  "EnterPasswordHere";

// Sip parameters
const char *SipIP       = "IPOrHostnameOfTheSIPServer";        
const int   SipPORT     = 5060;                 
const char *SipUSER     = "InternoSIP";           
const char *SipPW      = "PasswordInternoSIP";
// SIP Registration parameters
const int SipEXPIRES   = 20;                   // registration expires in seconds; renew after SipEXPIRES/2

#define ledPin D2 
#define speakerPin D5

#endif
