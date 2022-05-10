/*
 * configuration parameters 
 */

#ifndef CONFIG_H
#define CONFIG_H

// Sip parameters
const char *SipIP       = "IPOrHostnameOfTheSIPServer";        
const int   SipPORT     = 5060;                 
const char *SipUSER     = "InternoSIP";           
const char *SipPW      = "PasswordInternoSIP";
// SIP Registration parameters
const int SipEXPIRES   = 20;                   // registration expires in seconds; renew after SipEXPIRES/2

#define ledPin D4 
#define speakerPin D2
#define resetPin D5

#endif
