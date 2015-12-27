#if !defined(AFX_RECORD_DFU_MSG_PARSER__INCLUDED_)
#define AFX_RECORD_DFU_MSG_PARSER__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sys_define.h"

class CDfuMsgParser
{
private:
	RECORD_DFU_MSG* m_pMsg ;
	RECORD_DFU_MSG_HEADER * m_pMsgHdr ;

public:
	CDfuMsgParser() ;
	CDfuMsgParser(RECORD_DFU_MSG* pMsg);
	virtual ~CDfuMsgParser();

public:
	char* GetMsgHdr();
	void SetMsgHdr(char* pMsgHdr);
	char* GetMsgBody();
	void SetMsgBody(char* pMsgBody);
	void Attach(RECORD_DFU_MSG* pMsg);

public:	
	void SetMsgStartMark();

	UINT GetMsgTransMark();
	void SetMsgTransMark(UINT nTransMark);

	int GetMsgProtocolMark();
	void SetMsgProtocolMark(int nProtocol);

	void SetMsgReserve();

	UINT GetMsgLength();
	void SetMsgLength(UINT nLeng);

	void SetMsgFunMask(int nFunMask);

	bool GetMsgEndFlag();
	void SetMsgEndFlag(bool bEndFlag);

public:
	char*  Strrev(char* szT);
	char*  Itoa(int value, char* str, int radix);
	void   DoubleToArray(double fDouble,char* szByte,int iLength);
	void   IntToArray(UINT l,char* ch,int length);
	int    ArrayToInt(char* byte, int nCount);
	UINT   ArrayToHex(char* byte, int nCount);
	double ArrayToDouble(char* szByte,int iLength);
	UINT EndianConvertLToB(UINT InputNum);
	UINT EndianConvertBToL(UINT InputNum);
};

#endif // !defined(AFX_STTPMSGPARSER_H__D0F3545B_1380_4756_AE3B_3B009138B990__INCLUDED_)
