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
	//启动码
	void SetMsgStartMask();
	bool CheckStartMask();

	//事务标识
	UINT GetMsgTransMask();
	void SetMsgTransMask(UINT uTransMark);

	//协议标识
	int GetMsgProtocolMask();
	void SetMsgProtocolMask(int nProtocol = DEFINE_COMMU_PROTOCOL_RECORD_SELF);
	bool CheckMsgProtocolMask();

	//备用字段
	void SetMsgReserve();

	//报文长度
	UINT GetMsgLength();
	void SetMsgLength();

	//功能码
	void SetMsgFuncMask(int nErrorFlag = 0, int nDirect = 0, int nConfirmFlag = 0);
	//报文方向
	void SetMsgDirection(int nDircetion);
	int GetMsgdirection();

	//错误标识
	void SetMsgErrorFlag(int nErrorFlag);
	//get error flag,true:error false:no error
	bool GetMsgErrorFlag();

	//多帧确认标识
	void SetMsgMutiFrameConfirmFlag(int nFlag);
	int GetMsgMutiFrameConfirmFlag();

	//命令码
	//获得命令码
	int GetMsgCommand();
	//设置命令码
	void SetMsgCommand(int nMsgCommand);

	//结束码
	bool GetMsgEndFlag();
	void SetMsgEndFlag(bool bEndFlag);

	//结束码
	//根据规约设置结束码，目前支持录波器内部规约
	void SetEndMask(int nProtocol = DEFINE_COMMU_PROTOCOL_RECORD_SELF);
	//检查收到的报文中结束码是否正确
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
