#ifndef __BLETOOLS_H__
#define __BLETOOLS_H__

// ******************************************************************
// ****** BLE *******************************************************
// ******************************************************************

#define LEN_MOBILE_ID		16

const char* BLEGetMessageString(int DecodedMessageType);
void BLECalcWalletKey(const byte *Source,byte *Dest);

#endif
