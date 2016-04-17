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
	CRecordAPCIHandler(void);
	~CRecordAPCIHandler(void);

public:
	void SetDfuCommuParamHandler(COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParamHandler);

	void SetSysParamHandler(COLLECTOR_DATA_SYS_PARAM* pSysParamHandler);

	//set result call back fun
	void RegisterDfuResultCallBackFunc(PRESULTDFUMSGCALLBACKFUNC pFunc, XJHANDLE pObj);

public:

	//初始化
	//true：成功 false：失败
	bool InitRecordApciHandler();

	//启动
	//true：成功 false：失败
	bool StartRecordApciHandler();

	//停止
	//true：成功 false：失败
	bool StopRecordApciHandler();

	//Post msg
	int PushMsgToDfu(DFU_COMMU_MSG& command);

	//get msg trans mask
	UINT CreateTransMask();

public:
	//send loop
	int DfuSendOperationLoop();

	//recv loop
	int DfuRecvOperationLoop();

private:
	//read msg from socket
	//return readed msg bytes num
	int ReceiveMsg(DFU_COMMU_MSG& pMsg);

	//send msg
	int SendDfuMessage(DFU_COMMU_MSG& record_msg);

	//get send msg from list
	bool GetDfuSendMsg(DFU_COMMU_MSG& sendmsg);

private:
	//init logfile
	bool InitLogFile();

	//create link test command
	bool LaunchLinkTest();

	//query new osc file from dfu
	bool LaunchQueryNewFile();

	//get new file
	bool LaunchReadNewFile(UINT& uIndex);

	//manual file
	bool LaunchManualFile();

	//create test_msg
	void CreateTestReplyMsg(DFU_COMMU_MSG& reply_msg, DFU_COMMU_MSG recv_msg);

	//print msg
	void WriteDfuMessageLog(const DFU_COMMU_MSG& pMsg, const LOG_BUFFER_HEAD& pHead);

private:
	COLLECTOR_DFU_COMMU_PARAM* m_pDfuCommuParamHandler;

	COLLECTOR_DATA_SYS_PARAM* m_pSysParamHandler;

	//log handler
	CMessageLog m_LogFile;

private:
	/**	\brief 退出标志*/
	bool m_bExitFlag;

	/**	\brief 与dfu业务处理线程*/
	CRecordmanThread m_DfuBuinessThread;

	//recv thread
	CRecordmanThread m_DfuRecvThread;

private:
	/**	\brief net对象*/
	CNet* m_pNetSocket;

	//msg process lock
	CSafeLock m_LockSocketSend;

	//command list lock
	CSafeLock m_LockCommandList;

private:
	//dfu msg list
	DFUCOMMUMSG_BUF m_command_msg_buf;

	//json result fun
	PRESULTDFUMSGCALLBACKFUNC m_pDfuResultCallBackFunc;

	//result class handle
	XJHANDLE m_pResultProcessClassHandle;

	//link last active time
	time_t m_tLinkActive;

	//transmask
	UINT m_utTransMask;
};

#endif

