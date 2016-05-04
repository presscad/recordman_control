#if !defined(RECORD_APCI_HANDLER_INCLUDE)
#define RECORD_APCI_HANDLER_INCLUDE

#include "ConfigVariableMgr.h"
#include "../../common/MessageLog.h"
#include "DfuMsgAttach.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRecordAPCIHandler
{
public:
	CRecordAPCIHandler(
		COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParamHandler, 
		COLLECTOR_DATA_SYS_PARAM* pSysParamHandler);
	~CRecordAPCIHandler(void);

public:
	void RegisterDfuResultCallBackFunc(PRESULTDFUMSGCALLBACKFUNC pFunc, XJHANDLE pObj);

	bool InitRecordApciHandler();

	bool StartRecordApciHandler();

	bool StopRecordApciHandler();

	int PushMsgToDfu(DFU_COMMU_MSG& command);

	UINT Create_link_transmask();

public:
	int Send_dfu_msg_operation_loop();

	int Recv_dfu_msg_operation_loop();

private:
	int ReceiveMsg(DFU_COMMU_MSG& pMsg);

	int SendMessage(DFU_COMMU_MSG& record_msg);

	bool GetSendMsgFromList(DFU_COMMU_MSG& sendmsg);

private:
	bool InitLogFile();

	bool LaunchLinkTest();

	bool LaunchQueryNewFile();

	bool LaunchReadNewFile(UINT& uIndex);

	bool LaunchManualFile();

	void CreateTestReplyMsg(DFU_COMMU_MSG& reply_msg, DFU_COMMU_MSG recv_msg);

	void Write_message_log(const DFU_COMMU_MSG& pMsg, const LOG_BUFFER_HEAD& pHead);

private:
	COLLECTOR_DFU_COMMU_PARAM* m_pDfuCommuParamHandler;

	COLLECTOR_DATA_SYS_PARAM* m_pSysParamHandler;

	CRecordmanThread m_DfuBuinessThread;

	CRecordmanThread m_DfuRecvThread;

private:
	DFUCOMMUMSG_BUF m_command_msg_buf;

	CSafeLock m_LockSocketSend;

	CSafeLock m_LockCommandList;

private:
	CNet* m_pNetSocket;

	PRESULTDFUMSGCALLBACKFUNC m_pDfuResultCallBackFunc;

	XJHANDLE m_pResultProcessClassHandle;

	//link last active time
	time_t m_tLinkActive;

	//transmask
	UINT m_utTransMask;

	bool m_bExitFlag;

	CMessageLog m_LogFile;
};

#endif

