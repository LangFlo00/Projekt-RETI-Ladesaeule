#ifndef __SMARTTAP_H__
#define __SMARTTAP_H__

#include "apptools.h"

extern bool WaitAppleRemoved;

bool ReadCardDataGoogleSmartTap(TGPayEncryption* GPayEnv, byte* PassData, int* PassDataByteCnt, int MaxPassDataByteCnt);
void ApplePayApp_WaitRemoved(void);

#endif
