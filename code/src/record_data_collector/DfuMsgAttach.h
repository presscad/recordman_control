#if !defined(AFX_RECORD_DFU_MSG_ATTACH__INCLUDED_)
#define AFX_RECORD_DFU_MSG_ATTACH__INCLUDED_

#include "const_define.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDFUMsgAttach
{
public:
	CDFUMsgAttach();
	CDFUMsgAttach(WORD pLengthMask);
	virtual ~CDFUMsgAttach();

public:
	void Attach(DFU_COMMU_MSG* pMsg);

public:
	//������
	void SetMsgStartMask();
	bool CheckStartMask();

	//�����ʶ
	UINT GetMsgTransMask();
	void SetMsgTransMask(UINT uTransMark);

	//Э���ʶ
	int GetMsgProtocolMask();
	void SetMsgProtocolMask(int nProtocol = DEFINE_COMMU_PROTOCOL_RECORD_SELF);
	bool CheckMsgProtocolMask();

	//�����ֶ�
	void SetMsgReserve();

	//���ĳ���
	UINT GetMsgLength();
	void SetMsgLength();

	//������
	void SetMsgFuncMask(int nErrorFlag = 0, int nDirect = 0, int nConfirmFlag = 0);
	//���ķ���
	void SetMsgDirection(int nDircetion);
	int GetMsgdirection();

	//�����ʶ
	void SetMsgErrorFlag(int nErrorFlag);
	//get error flag,true:error false:no error
	bool GetMsgErrorFlag();

	//��֡ȷ�ϱ�ʶ
	void SetMsgMutiFrameConfirmFlag(int nFlag);
	int GetMsgMutiFrameConfirmFlag();

	//������
	//���������
	int GetMsgCommand();
	//����������
	void SetMsgCommand(int nMsgCommand);

	//������
	bool GetMsgEndFlag();
	void SetMsgEndFlag(bool bEndFlag);

	//������
	//���ݹ�Լ���ý����룬Ŀǰ֧��¼�����ڲ���Լ
	void SetEndMask(int nProtocol = DEFINE_COMMU_PROTOCOL_RECORD_SELF);
	//����յ��ı����н������Ƿ���ȷ
	bool CheckEndMask();

public:
	void SetMsgSettingZone(int nZone);
	int GetMsgSettingZone();

	void SetMsgSettingGroup(int nGroup);

	void SetMsgSettingGroupIndex(int nIndex);

	void SetMsgSubModuleNo(int nModuleNo);

	void SetMsgCurSecond(UINT nCurSecond);

	void SetMsgCurNanoSecond(UINT nCurNanoSecond);

	void SetMsgCurTimeZone(int nTimeZone);

	int GetErrorNum();

	int GetSettingNum();

	int GetOldZone();

	int GetNewZone();

	int GetZoneNum();

	int GetFileNum();

private:
	WORD m_wLengthMask;

	DFU_COMMU_MSG* m_pMsg;
};
#endif // !defined(AFX_RECORD_DFU_MSG_ATTACH__INCLUDED_)
