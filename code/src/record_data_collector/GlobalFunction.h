#ifndef RECORD_DATA_COLLECTOR_GLOBAL_FUNCTION_H
#define RECORD_DATA_COLLECTOR_GLOBAL_FUNCTION_H

#include "const_define.h"

//
char* Record_Strrev(char* szT);

//×Ö·û×ª»»
char* Record_Itoa(int value, char* str, int radix);

string GetDfuErrorMsgString(int nErrorNum);

void ConvertInt32BigedianToL(XJHANDLE pChar, int& nVal);

void ConvertUint32BigedianToL(XJHANDLE pChar, UINT& nVal);

void ConvertFloat32BigedianToL(XJHANDLE pChar, float& fVal);

string GetInt32VersionInfo(int nVersion);

#endif