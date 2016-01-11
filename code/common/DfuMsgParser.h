#if !defined(AFX_RECORD_DFU_MSG_PARSER__INCLUDED_)
#define AFX_RECORD_DFU_MSG_PARSER__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sys_define.h"

class CDfuMsgParser
{
private:
	RECORD_DFU_MSG* m_pMsg;
	RECORD_DFU_MSG_HEADER * m_pMsgHdr;

public:
	CDfuMsgParser() ;
	CDfuMsgParser(RECORD_DFU_MSG* pMsg);
	virtual ~CDfuMsgParser();

public:
	void Attach(RECORD_DFU_MSG* pMsg);

	RECORD_DFU_MSG_HEADER* GetMsgHdr();

	BYTE* GetMsgBody();
	void SetMsgBody(BYTE* pMsgBody);

public:	
	void SetMsgStartMask();
	bool CheckStartMask();

	UINT GetMsgTransMask();
	void SetMsgTransMask(UINT nTransMark);

	int GetMsgProtocolMask();
	void SetMsgProtocolMask(int nProtocol = DEFINE_COMMU_PROTOCOL_RECORD_SELF);
	bool CheckMsgProtocolMask();

	void SetMsgReserve();

	UINT GetMsgLength();
	void SetMsgLength(UINT nLeng);

	void SetMsgFunMask(int nFunMask);

	bool GetMsgEndFlag();
	void SetMsgEndFlag(bool bEndFlag);

	//���������
	int GetMsgCommand();
	//����������
	void SetMsgCommand(int nMsgCommand);

	//���ݹ�Լ���ý����룬Ŀǰ֧��¼�����ڲ���Լ
	void SetEndMask(int nProtocol = DEFINE_COMMU_PROTOCOL_RECORD_SELF);
	//����յ��ı����н������Ƿ���ȷ
	bool CheckEndMask();

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
