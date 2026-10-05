// ******************************************************************
//
//    File: App_STD205_Standard.c
//    Date: 2025-03-31
// Version: 2.05
//
// Purpose:
//
// This is the standard app for TWN4 readers, which is installed
// as default app on TWN4 readers. This script can be run on any
// type of TWN4 reader without modification.
//
// Feel free to modify this program for your specific purposes!
//
// V1:
// ------
// - Initial release
//
// V2:
// ---
// - Support configuration via AppBlaster
//
// V1.03:
// ------
// - Changed version numbering scheme
//
// V1.04:
// ------
// - Configure tags being searched for only if LFTAGTYPES or
//   HFTAGTYPES requests a specific type. Otherwise, default setup
//   by firmware is used.
// - Marked as template for AppBlaster
// - Support ICLASS option
// - Change style how options are evaluated more convenient
//
// V1.05:
// ------
// - NFCP2P Demo Support
//
// V1.06:
// ------
// - Adaption to reworked NFC SNEP service
//
// V1.07:
// ------
// - Adaption to new naming scheme of functions for host communication
// - iCLASS: Read UID by default instead of PAC
//
// V1.08:
// ------
// - Optional support for remote wakeup
//
// V2.00:
// ------
// - Adaption to new architecture of AppBlaster V2
//
// V2.01:
// ------
// - Support configuration cards
//
// V2.02:
// ------
// - Support BLE GATT
//
// V2.03:
// ------
// - Support BLE GATT
//
// V2.04:
// ------
// - Support OnCardDone()
//
// V2.05:
// ------
// - Refined setup of active transponders
//
// ******************************************************************

#include "twn4.sys.h"
#include "apptools.h"

// ******************************************************************
// ****** Configuration Area ****************************************
// ******************************************************************

#if APPEXTCONFIG

#include "appconfig.h"

#else

// The comments below are showing a setup of custom set of active transponders
// Uncomment both definitions, and adjust them to your specific use case or application
//#define LFTAGTYPES      (TAGMASK(LFTAG_EM4102) | TAGMASK(LFTAG_HITAG1S))
//#define HFTAGTYPES      (TAGMASK(HFTAG_MIFARE))

#ifndef CONFIGENABLED
  #define CONFIGENABLED         SUPPORT_CONFIGCARD_OFF
#endif  

#define CARDTIMEOUT				2000UL	// Timeout in milliseconds
#define MAXCARDIDLEN            32		// Length in bytes
#define MAXCARDSTRINGLEN		128   	// Length W/O null-termination

bool ReadCardData(int TagType,const byte* ID,int IDBitCnt,char *CardString,int MaxCardStringLen)
{
	// Select data from card (take any ID from any transponder)
	byte CardData[32];
	int CardDataBitCnt;
    CardDataBitCnt = MIN(IDBitCnt,sizeof(CardData)*8);
    CopyBits(CardData,0,ID,0,CardDataBitCnt);

	// Modify card data (do not modify, just copy)

  	// Convert data to ASCII
    ConvertBinaryToString(CardData,0,CardDataBitCnt,CardString,16,(CardDataBitCnt+7)/8*2,MaxCardStringLen);
    return true;
}

// ****** Event Handler *********************************************

void OnStartup(void)
{
    LEDInit(REDLED | GREENLED);
    LEDOn(GREENLED);
    LEDOff(REDLED);
    
    SetVolume(30);
    BeepLow();
    BeepHigh();
}

void OnNewCardFound(const char *CardString)
{
	// Send card string including prefix (actually no prefix) and suffix ("\r")
    HostWriteString(CardString);
    HostWriteString("\r");

    LEDOff(GREENLED);
    LEDOn(REDLED);
    LEDBlink(REDLED,500,500);

    SetVolume(100);
    BeepHigh();
}

void OnCardTimeout(const char *CardString)
{
    LEDOn(GREENLED);
    LEDOff(REDLED);
}

void OnCardDone(void)
{
}

#endif

// ******************************************************************
// ****** Main Program Loop *****************************************
// ******************************************************************

int main(void)
{
	OnStartup();    	

	const byte Params[] = { SUPPORT_CONFIGCARD, 1, CONFIGENABLED, TLV_END };
	SetParameters(Params,sizeof(Params));

#if defined(LFTAGTYPES) && defined(HFTAGTYPES)
	SetTagTypes(LFTAGTYPES,HFTAGTYPES);
#endif    

	char OldCardString[MAXCARDSTRINGLEN+1]; 
    OldCardString[0] = 0;

    while (true)
    {
		int TagType;
		int IDBitCnt;
		byte ID[32];

		// Search a transponder
	    if (SearchTag(&TagType,&IDBitCnt,ID,sizeof(ID)))
	    {
			// A transponder was found. Read data from transponder and convert
			// it into an ASCII string according to configuration
			char NewCardString[MAXCARDSTRINGLEN+1];
			if (ReadCardData(TagType,ID,IDBitCnt,NewCardString,sizeof(NewCardString)-1))
			{
				// Is this a newly found transponder?
				if (strcmp(NewCardString,OldCardString) != 0)
				{
					// Yes. Save new card string
					strcpy(OldCardString,NewCardString);
					OnNewCardFound(NewCardString);
				}
				// (Re-)start timeout
			   	StartTimer(CARDTIMEOUT);
			}
			OnCardDone();
	    }
    	
        if (TestTimer())
        {
		    OnCardTimeout(OldCardString);
		    OldCardString[0] = 0;
        }
    }
}
