#ifndef _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_WARNINGS
#endif
#include "GECodeConvert.h"
#include "locale.h"
#include "iostream"
#include "stdlib.h"
#include "wchar.h"
#include <string.h>
using namespace std;

#ifdef _MSC_VER	  
#include <Windows.h>
#define CSET_GBK    "936"
#define CSET_UTF8   "65001"	  
#define LC_NAME_zh_CN   "Chinese_People's Republic of China" 
#else
#define CSET_GBK    "GBK"
#define CSET_UTF8   "UTF-8"  
#define LC_NAME_zh_CN   "zh_CN"
#endif 
#define LC_NAME_zh_CN_GBK       LC_NAME_zh_CN "." CSET_GBK
#define LC_NAME_zh_CN_UTF8      LC_NAME_zh_CN "." CSET_UTF8
//#define LC_NAME_zh_CN_DEFAULT   LC_NAME_zh_CN_GBK



CGECodeConvert::CGECodeConvert(void)
{
}


CGECodeConvert::~CGECodeConvert(void)
{
}

long CGECodeConvert::Gbk2Unicode( const char* pGbk, long nGbkCount, wchar_t*& pUnicode )
{
	if(pGbk == 0)
		return -1;
	long wlength = -1;
#ifdef _MSC_VER	
	wlength = MultiByteToWideChar(936, 0, pGbk, nGbkCount, 0, 0);
	if(wlength != -1)
	{
		pUnicode = new wchar_t[wlength+1];
		memset(pUnicode, 0, (wlength+1)*sizeof(wchar_t));
		MultiByteToWideChar(936, 0, pGbk, nGbkCount, pUnicode, wlength);
	}
#else
	char* curlocale = setlocale(LC_ALL, 0);
	char* localeinfo = setlocale(LC_ALL, LC_NAME_zh_CN_GBK);
	if(localeinfo)
		cout<<"set locale to"<<localeinfo<<"\n";
	else
	{
		cout<<"set locale to gbk failed"<<"\n";
		return -1;
	}

	wlength = mbstowcs(0, pGbk, nGbkCount);
	if(wlength == -1)
	{
		 setlocale(LC_ALL, curlocale);
		return -1;
	}
	pUnicode = new wchar_t[wlength+1];
	memset(pUnicode, 0, (wlength+1)*sizeof(wchar_t));
	mbstowcs(pUnicode, pGbk, wlength+1);
	setlocale(LC_ALL, curlocale);
#endif
	return wlength;
}

long CGECodeConvert::Unicode2Gbk( const wchar_t* pUnicode, long nUniCount, char*& pGbk )
{
	if(pUnicode == 0)
		return -1;
	long nlength = -1;
#ifdef _MSC_VER	
	nlength = WideCharToMultiByte(936, 0, pUnicode, nUniCount, 0, 0, 0, 0);
	if(nlength != -1)
	{
		pGbk = new char[nlength+1];
		memset(pGbk, 0, nlength+1);
		WideCharToMultiByte(936, 0, pUnicode, nUniCount, pGbk, nlength, 0, 0);
	}
#else
	char* curlocale = setlocale(LC_ALL, 0);
	char* localeinfo = setlocale(LC_ALL, LC_NAME_zh_CN_GBK);
	if(localeinfo)
		cout<<"set locale to"<<localeinfo<<"\n";
	else
	{
		cout<<"set locale to gbk failed"<<"\n";
		return -1;
	}

	nlength = wcstombs(0, pUnicode, nUniCount);
	if(nlength == -1)
	{
		 setlocale(LC_ALL, curlocale);
		return -1;
	}
	pGbk = new char[nlength+1];
	memset(pGbk, 0, nlength+1);
	wcstombs(pGbk, pUnicode, nlength+1);
	setlocale(LC_ALL, curlocale);
#endif
	return nlength;
}

long CGECodeConvert::Default2Unicode( const char* pSrc, long nSrc, wchar_t*& pUnicode )
{
	if(pSrc == 0)
		return -1;
	char* curlocale = setlocale(LC_ALL, 0);
	char* localeinfo = setlocale(LC_ALL, "");
	if(localeinfo)
	{
		cout<<"set locale to"<<localeinfo<<"\n";
	}
	else
	{
		cout<<"set locale to default failed"<<"\n";
		return -1;
	}

	long wlength = mbstowcs(0, pSrc, nSrc);
	if(wlength == -1) 
	{
		setlocale(LC_ALL, curlocale);
		return -1;
	}
	pUnicode = new wchar_t[wlength+1];
	memset(pUnicode, 0, (wlength+1)*sizeof(wchar_t));
	mbstowcs(pUnicode, pSrc, wlength+1);
	setlocale(LC_ALL, curlocale);
	return wlength;
}

long CGECodeConvert::Unicode2Default( const wchar_t* pUnicode, long nUniCount, char*& pDest )
{
	if(pUnicode == 0)
		return -1;
	char* curlocale = setlocale(LC_ALL, 0);
	char* localeinfo = setlocale(LC_ALL, "");
	if(localeinfo)
	{
		cout<<"set locale to"<<localeinfo<<"\n";
	}
	else
	{
		cout<<"set locale to default failed"<<"\n";
		return -1;
	}

	long nlength = wcstombs(0, pUnicode, nUniCount);
	if(nlength == -1)
	{
		setlocale(LC_ALL, curlocale);
		return -1;
	}
	pDest = new char[nlength+1];
	memset(pDest, 0, nlength+1);
	wcstombs(pDest, pUnicode, nlength+1);
	setlocale(LC_ALL, curlocale);
	return nlength;
}

long CGECodeConvert::Utf82Unicode( const char* pUtf8, long nUtf8Count, wchar_t*& pUnicode )
{
	if(pUtf8 == 0)
		return -1;
	long wlength = -1;
#ifdef _MSC_VER	
	wlength = MultiByteToWideChar(CP_UTF8, 0, pUtf8, nUtf8Count, 0, 0);
	if(wlength != -1)
	{
		pUnicode = new wchar_t[wlength+1];
		memset(pUnicode, 0, (wlength+1)*sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, pUtf8, nUtf8Count, pUnicode, wlength);
	}
#else
	char* curlocale = setlocale(LC_ALL, 0);
	char* localeinfo = setlocale(LC_ALL, LC_NAME_zh_CN_UTF8);
	if(localeinfo)
		cout<<"set locale to "<<localeinfo<<"\n";
	else
	{
		cout<<"set locale to utf8 failed"<<"\n";
		return -1;
	}

	wlength = mbstowcs(0, pUtf8, nUtf8Count);
	if(wlength != -1)
	{
		pUnicode = new wchar_t[wlength+1];
		memset(pUnicode, 0, (wlength+1)*sizeof(wchar_t));
		mbstowcs(pUnicode, pUtf8, wlength+1);
	}
	setlocale(LC_ALL, curlocale);
#endif
	return wlength;
}

long CGECodeConvert::Unicode2Utf8( const wchar_t* pUnicode, long nUniCount, char*& pUtf8 )
{
	if(pUnicode == 0)
		return -1;
	long nlength = -1;
#ifdef _MSC_VER	
	nlength = WideCharToMultiByte(CP_UTF8, 0, pUnicode, nUniCount, 0, 0, 0, 0);
	if(nlength != -1)
	{
		pUtf8 = new char[nlength+1];
		memset(pUtf8, 0, nlength+1);
		WideCharToMultiByte(CP_UTF8, 0, pUnicode, nUniCount, pUtf8, nlength, 0, 0);
	}
#else
	char* curlocale = setlocale(LC_ALL, 0);
	char* localeinfo = setlocale(LC_ALL, LC_NAME_zh_CN_UTF8);
	if(localeinfo)
		cout<<"set locale to"<<localeinfo<<"\n";
	else
	{
		cout<<"set locale to utf8 failed"<<"\n";
		return -1;
	}

	nlength = wcstombs(0, pUnicode, nUniCount);
	if(nlength != -1)
	{
		pUtf8 = new char[nlength+1];
		memset(pUtf8, 0, nlength+1);
		wcstombs(pUtf8, pUnicode, nlength+1);
	}
	setlocale(LC_ALL, curlocale);
#endif
	return nlength;
}

//输入和输出都为大端
long CGECodeConvert::Utf16ToUtf32_BE( const uint16_t* pUtf16, long nUtf16Count, uint32_t*& pUtf32 )
{
	if(pUtf16 == 0)
		return -1;
	int wlength = 0;
	for(int i = 0; i < nUtf16Count; i++)
	{
		uint16_t temp = pUtf16[i];
		if(IsLittleEndian())
			swap16(temp);
		if(temp < 0xDC00 || temp >0xDFFF)
			wlength++; //0xD800到0XDFFF的值应该是成对出现的，所以只计数一次
	}
	pUtf32 = new uint32_t[wlength+1];
	memset(pUtf32, 0, (wlength+1)*sizeof(uint32_t));
	int nIndex = 0;
	for(int k = 0; k < nUtf16Count; k++)
	{
		uint16_t temp = pUtf16[k];
		if(IsLittleEndian())
			swap16(temp);
		if(temp < 0xD800 || temp >0xDFFF)
		{
			if(IsLittleEndian())
			{
				pUtf32[nIndex] = temp;
				swap32(pUtf32[nIndex]);
			}
			else
				pUtf32[nIndex] = pUtf16[k];
			nIndex++;
		}
		else if(temp >= 0xD800 && temp <= 0xDBFF)
		{
			//前导代理
			uint16_t vh = pUtf16[k] & 0x03FF; //留下后10位
			if(IsLittleEndian())
				vh = temp&0x03FF;
			pUtf32[nIndex] = vh;
			pUtf32[nIndex] = pUtf32[nIndex] << 10;
		}
		else if(temp >= 0xDC00 && temp <= 0xDFFF)
		{
			//后导代理
			uint16_t vl = pUtf16[k] & 0x03FF; //留下后10位
			if(IsLittleEndian())
				vl = temp & 0x03FF;
			pUtf32[nIndex] = pUtf32[nIndex] | vl;
			pUtf32[nIndex] += 0x10000;
			if(IsLittleEndian())
				swap32(pUtf32[nIndex]);
			nIndex++;
		}
	}
	return wlength;
}

//输入和输出都为大端
long CGECodeConvert::Utf32ToUtf16_BE( const uint32_t* pUtf32, long nUtf32Count, uint16_t*& pUtf16 )
{
	if(pUtf32 == 0)
		return -1;
	int wlength = 0;
	for(int i = 0; i < nUtf32Count; i++)
	{
		uint32_t temp = pUtf32[i];
		if(IsLittleEndian())
			swap32(temp);
		if(temp <= 0xFFFF)
			wlength++;
		else
			wlength += 2;
	}
	pUtf16 = new uint16_t[wlength+1];
	memset(pUtf16, 0, (wlength+1)*sizeof(uint16_t));
	int nIndex = 0;
	for(int k = 0; k < nUtf32Count; k++)
	{
		uint32_t temp = pUtf32[k];
		if(IsLittleEndian())
			swap32(temp);
		if(temp <= 0xFFFF)
		{
			if(IsLittleEndian())
			{
				pUtf16[nIndex] = temp;
				swap16(pUtf16[nIndex]);
			}
			else
				pUtf16[nIndex] = pUtf32[k];
			nIndex++;
		}
		else
		{
			uint32_t vx = pUtf32[k] - 0x10000;
			if(IsLittleEndian())
			{
				vx = temp - 0x10000;
			}
			uint16_t vh =(uint16_t) (vx>>10);
			uint16_t vl = (uint16_t) (vx&0x03FF);
			vh = vh | 0xD800;
			vl = vl | 0xDC00;
			pUtf16[nIndex] = vh;
			if(IsLittleEndian())
			{
				swap16(pUtf16[nIndex]);
			}
			nIndex++;
			pUtf16[nIndex] = vl;
			if(IsLittleEndian())
			{
				swap16(pUtf16[nIndex]);
			}
			nIndex++;
		}
	}
	return wlength;
}

//输入和输出都为小端
long CGECodeConvert::Utf16ToUtf32_LE( const uint16_t* pUtf16, long nUtf16Count, uint32_t*& pUtf32 )
{
	if(pUtf16 == 0)
		return -1;
	int wlength = 0;
	for(int i = 0; i < nUtf16Count; i++)
	{
		uint16_t temp = pUtf16[i];
		if(!IsLittleEndian())
			swap16(temp);
		if(temp < 0xDC00 || temp >0xDFFF)
			wlength++; //0xD800到0XDFFF的值应该是成对出现的，所以只计数一次
	}
	pUtf32 = new uint32_t[wlength+1];
	memset(pUtf32, 0, (wlength+1)*sizeof(uint32_t));
	int nIndex = 0;
	for(int k = 0; k < nUtf16Count; k++)
	{
		uint16_t temp = pUtf16[k];
		if(!IsLittleEndian())
			swap16(temp);
		if(temp < 0xD800 || temp >0xDFFF)
		{
			if(!IsLittleEndian())
			{
				pUtf32[nIndex] = temp;
				swap32(pUtf32[nIndex]);
			}
			else
				pUtf32[nIndex] = pUtf16[k];
			nIndex++;
		}
		else if( temp >= 0xD800 && temp <= 0xDBFF)
		{
			//前导代理
			uint16_t vh = pUtf16[k] & 0x03FF; //留下后10位
			if(!IsLittleEndian())
				vh = temp & 0x03FF;
			pUtf32[nIndex] = vh;
			pUtf32[nIndex] = pUtf32[nIndex] << 10;
		}
		else if(temp >= 0xDC00 && temp <= 0xDFFF)
		{
			//后导代理
			uint16_t vl = pUtf16[k] & 0x03FF; //留下后10位
			if(!IsLittleEndian())
				vl = temp & 0x03FF;
			pUtf32[nIndex] = pUtf32[nIndex] | vl;
			pUtf32[nIndex] += 0x10000;
			if(!IsLittleEndian())
				swap32(pUtf32[nIndex]);
			nIndex++;
		}
	}
	return wlength;
}

//输入和输出都为小端
long CGECodeConvert::Utf32ToUtf16_LE( const uint32_t* pUtf32, long nUtf32Count, uint16_t*& pUtf16 )
{
	if(pUtf32 == 0)
		return -1;
	int wlength = 0;
	for(int i = 0; i < nUtf32Count; i++)
	{
		uint32_t temp = pUtf32[i];
		if(!IsLittleEndian())
			swap32(temp);
		if(temp <= 0xFFFF)
			wlength++;
		else
			wlength += 2;
	}
	pUtf16 = new uint16_t[wlength+1];
	memset(pUtf16, 0, (wlength+1)*sizeof(uint16_t));
	int nIndex = 0;
	for(int k = 0; k < nUtf32Count; k++)
	{
		uint32_t temp = pUtf32[k];
		if(!IsLittleEndian())
			swap32(temp);
		if(temp <= 0xFFFF)
		{
			if(!IsLittleEndian())
			{
				pUtf16[nIndex] = temp;
				swap16(pUtf16[nIndex]);
			}
			else
				pUtf16[nIndex] = pUtf32[k];
			nIndex++;
		}
		else
		{
			uint32_t vx = pUtf32[k] - 0x10000;
			if(!IsLittleEndian())
				vx = temp - 0x10000;
			uint16_t vh =(uint16_t) (vx>>10);
			uint16_t vl = (uint16_t) (vx&0x03FF);
			vh = vh | 0xD800;
			vl = vl | 0xDC00;
			pUtf16[nIndex] = vh;
			if(!IsLittleEndian())
				swap16(pUtf16[nIndex]);
			nIndex++;
			pUtf16[nIndex] = vl;
			if(!IsLittleEndian())
				swap16(pUtf16[nIndex]);
			nIndex++;
		}
	}
	return wlength;
}

long CGECodeConvert::Utf82Gbk(const char* pUtf8, long nUtf8Count, char*& pGbk)
{
	long length = -1;
	wchar_t* pUnicode = 0;
	long wlength = Utf82Unicode( pUtf8, strlen(pUtf8), pUnicode);
	if( wlength >= 0 && pUnicode != 0 ){
		length = Unicode2Gbk(pUnicode, wlength, pGbk);
		delete[] pUnicode;
		pUnicode = 0;
	}
	return length;
}

long CGECodeConvert::Gbk2Utf8(const char* pGbk, long nGbkCount, char*& pUtf8)
{
	long length = -1;
	wchar_t* pUnicode = 0;
	long wlength = Gbk2Unicode( pGbk, strlen(pGbk), pUnicode);
	if( wlength >= 0 && pUnicode != 0 ){
		length = Unicode2Utf8(pUnicode, wlength, pUtf8);
		delete[] pUnicode;
		pUnicode = 0;
	}
	return length;
}
