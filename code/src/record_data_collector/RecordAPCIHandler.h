#if !defined(RECORD_APCI_HANDLER_INCLUDE)
#define RECORD_APCI_HANDLER_INCLUDE

#include "ConfigVariableMgr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRecordAPCIHandler
{
public:
	CRecordAPCIHandler(void);
	~CRecordAPCIHandler(void);

public:
	//设置参数访问对象指针
	void SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam);

	//初始化
	//true：成功 false：失败
	bool InitRecordApciHandler();

	//启动
	//true：成功 false：失败
	bool StartRecordApciHandler();

	//停止
	//true：成功 false：失败
	bool StopRecordApciHandler();

public:
	//与dfu通讯主线程函数
	//0：线程退出
	int DfuCommuOperationLoop();

private:
	//process command
	int ProcessCommand(RECORD_DFU_MSG* pCommandMsg, vector<RECORD_DFU_MSG*> veResultMsg);
	
	//read msg from socket
	//return readed msg bytes num
	int ReceiveMsg(RECORD_DFU_MSG* pMsg);

	//send msg
	int WriteRecordMsg(RECORD_DFU_MSG* pMsg);

	//create poling command
	void CreatePolingCommand(RECORD_DFU_MSG* pCommandMsg);

	//query new osc file from dfu
	void CreateQueryNewOscCommand(RECORD_DFU_MSG* pCommandMsg);

	void CreateGetOscFileCommand(RECORD_DFU_MSG* pCommandMsg, int nFileIndex);

private:
	//print msg
	void LogMessage(const RECORD_DFU_MSG* pMsg, const LOG_BUFFER_HEAD& pHead);

	//copy string
	void CopyMessageToString(BYTE bMsg, char*& pChar);

	//get msg trans mask
	UINT GetMsgTransMask();

private:
	/**	\brief 配置参数对象*/
	COLLECTOR_DATA_SYS_PARAM* m_pCollectorSysParam;

private:
	/**	\brief 退出标志*/
	bool m_bExitFlag;

	/**	\brief 与dfu业务处理线程*/
	CRecordmanThread m_DfuOperationThread;

	/**	\brief net对象*/
	CNet* m_pNetSocket;

private:
	//msg process lock
	CSafeLock m_LockApciMsgHandler;

	//link last active time
	time_t m_tLinkActive;

	//transmask
	UINT m_utTransMask;
};

#endif

