#ifndef __APPTOOLS_H__
#define __APPTOOLS_H__

#include "fio.h"
#include "bletools.h"

// ******************************************************************
// ****** Timer Functions *******************************************
// ******************************************************************

void StartTimer(unsigned long Duration);
void StopTimer(void);
bool TestTimer(void);

// ******************************************************************
// ****** Delay Function ********************************************
// ******************************************************************

void Delay(unsigned long Duration);

// ******************************************************************
// ****** Tools *****************************************************
// ******************************************************************

char Nibble2HexChar(byte Nibble);

// ******************************************************************
// ****** Beep Functions ********************************************
// ******************************************************************

void SetVolume(int NewVolume);
int GetVolume(void);
void BeepLow(void);
void BeepHigh(void);

// ******************************************************************
// ****** Compatibility to TWN3 *************************************
// ******************************************************************

int ConvertTagTypeToTWN3(int TagTypeTWN4);

// ******************************************************************
// ****** LEGIC Mini-API ********************************************
// ******************************************************************

bool LEGIC_Idle(void);
bool LEGIC_SearchTXP(int TagType,byte *ID,int *IDLength);
bool LEGIC_Select(int TagType);
bool LEGIC_ReadTagTypeFamily(int TagType,byte *Type,byte *Family);
bool LEGIC_SearchSegmentByStamp(const byte* Stamp,int StampLength);
bool LEGIC_SearchSegmentByIndex(int Index,byte *Stamp,int *StampLength,int *SegmentLength);
bool LEGIC_ReadSegment(int Offset,int ByteCnt,byte *Data);

// ******************************************************************
// ****** NFC *******************************************************
// ******************************************************************

typedef struct
{
    byte* Header;
    byte TypeLength;
    int PayloadLength;
    byte IDLength;
    byte* Type;
    byte* ID;
    byte* Payload;
    byte* NextRecord;
} TNDEFRecord;

#define NDEF_IsMBSet(Header)    (Header & 0x80)
#define NDEF_IsMESet(Header)    (Header & 0x40)
#define NDEF_IsCFSet(Header)    (Header & 0x20)
#define NDEF_IsSRSet(Header)    (Header & 0x10)
#define NDEF_IsILSet(Header)    (Header & 0x08)
#define NDEF_GetTNF(Header)        (Header & 0x07)

bool NDEF_WaitConnect(void);
bool NDEF_ReceiveRecord(byte *RecordType,int *RecordTypeLength,int MaxRecordTypeLength,
                        byte *RecordData,int *RecordDataLength,int MaxRecordDataLength);

// ******************************************************************
// ****** BLE *******************************************************
// ******************************************************************

#define LEN_MOBILE_ID        16

const char* BLEGetMessageString(int DecodedMessageType);
void BLECalcWalletKey(const byte *Source,byte *Dest);

// ******************************************************************
// ****** LED Compatibility Functions *******************************
// ******************************************************************

void CompLEDInit(int LEDs);
void CompLEDOn(int LEDs);
void CompLEDOff(int LEDs);
void CompLEDToggle(int LEDs);
void CompLEDBlink(int LEDs,int TimeOn,int TimeOff);

// ******************************************************************
// ****** Host Functions ********************************************
// ******************************************************************

#ifndef __FIO_H__

bool HostTestByte(void);
byte HostReadByte(void);
bool HostTestChar(void);
char HostReadChar(void);

void HostWriteByte(byte Byte);
void HostWriteChar(char Char);
void HostWriteString(const char *String);
void HostWriteRadix(const byte *ID,int BitCnt,int DigitCnt,int Radix);
void HostWriteBin(const byte *ID,int BitCnt,int DigitCnt);
void HostWriteDec(const byte *ID,int BitCnt,int DigitCnt);
void HostWriteHex(const byte *ID,int BitCnt,int DigitCnt);
void HostWriteVersion(void);

#endif

// ******************************************************************
// ****** ApplePay Helpers ******************************************
// ******************************************************************

void ApplePayApp_WaitRemoved(void);

// ******************************************************************
// ****** AppleVAS **************************************************
// ******************************************************************

bool AppleVAS_ReadCardData(byte *CardData,int *CardDataLen,int MaxCardDataLen);

// ******************************************************************
// ****** Transact **************************************************
// ******************************************************************
#define TRANSACTREADAPPLE           0x01
#define TRANSACTREADGPAY            0x02
#define TRANSACTREADDESFIRE         0x04
#define TRANSACTREADMIFARE          0x08
#define TRANSACTREADMIFARE4K        0x10
#define TRANSACTPHYSICALDATASIZE    48
#define TRANSACTMOBILEDATASIZE      28

#define TRANSACTAPPLECIDFILE        2
#define TRANSACTAPPLESISFILE        3
#define TRANSACTMF2GOCIDFILE        1
#define TRANSACTMF2GOSISFILE        2

void Transact_SetMobileFiles(int AppleFileNumber, int MF2GoFileNumber);
void Transact_SetReadTypes(int ReadTypes);
void Transact_MobileReadSIS(bool ReadSIS);
bool Transact_ReadCardData(const TTransactKeys *Keys,byte *CardData,int *CardDataLen,int MaxCardDataLen);

// ******************************************************************
// ****** KleverKey *************************************************
// ******************************************************************

void InitKleverKey(void);
bool SearchKleverKey(int *TagType,int *IDBitCount,byte *ID,int MaxIDBytes);

// ******************************************************************
// ****** Safetrust *************************************************
// ******************************************************************

void InitSafetrust(void);
bool SearchSafetrust(int *TagType,int *IDBitCount,byte *ID,int MaxIDBytes);

// ******************************************************************
// ****** Google SmartTap *******************************************
// ******************************************************************

//bool ReadCardDataGoogleSmartTap(byte* PassID, int* PassIDByteCnt, int MaxPassIDByteCnt);

// ******************************************************************
// ****** Elatec Access *********************************************
// ******************************************************************

bool ElatecAccess_ReadCardData(const byte* EncryptedKeyPackage, int EncryptedKeyPackageByteCnt, byte *CardData, int *CardDataLen, int MaxCardDataLen);

/**
 * @brief Holds Wavelynx mobile credential metadata
 *
 */
typedef struct __attribute__((__packed__))
{
	struct TWavelynxECPSettings
	{
		byte TCI[3];
		byte ecpFormat;
		byte ecpTerminalMode;
		byte ecpTerminalType;
		byte ecpTerminalSubType;
		byte ecpBitCount;
		byte ecpAppOptions;
	} ECPSettings;
	struct TWavelynxMF2GoSettings
	{
		byte AID[3];
		byte DFNameLength;
		byte DFName[16];
	} MF2GoSettings;
} TWavelynxCustomSettings;

bool WLiOSReadData(int cryptoEnvironment, TWavelynxCustomSettings* CustomSettings, byte* CardData, int CardDataMaxBytes, int* BytesCopied, int bytesToRead);
bool WLGetCustomSettings(TWavelynxCustomSettings* CustomSettings);
bool WLMF2GoReadData(int cryptoEnvironment, TWavelynxCustomSettings* CustomSettings, byte* CardData, int CardDataMaxBytes, int* BytesCopied, int bytesToRead);

void LegicECPDemo_Debug(bool Mode);
bool LegicECPDemo_Init(void);
bool LegicECPDemo_Read(byte *Data,int Offset,int Length);

bool SiemensBuildingXAccess_ReadData(const byte *VASUPA,int VASUPALen,const byte* Key1, const byte* Key2, int AID, byte* Data, int ByteCnt);

#endif
