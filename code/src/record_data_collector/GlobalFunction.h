#ifndef RECORD_DATA_COLLECTOR_GLOBAL_FUNCTION_H
#define RECORD_DATA_COLLECTOR_GLOBAL_FUNCTION_H

#include "const_define.h"
#include "../../common/common_time.h"

char* recordman_strrev(char* szT);

char* recordman_itoa(int value, char* str, int radix);

string get_dfu_error_msg_string(int nErrorNum);

void convert_btol_int32(XJHANDLE pChar, int& nVal);

void convert_btol_uint32(XJHANDLE pChar, UINT& nVal);

void convert_btol_float32(XJHANDLE pChar, float& fVal);

void convert_btol_uint16(XJHANDLE pChar, uint16& uVal);

string get_int32_version(XJHANDLE pChar);

string get_dfu_crc(XJHANDLE pChar);

string format_dfu_msg_time(UINT uMsgtime, int nTimetype = CCommonTime::COMTRADE26Time);

#endif