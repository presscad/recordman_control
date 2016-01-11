#if !defined(RECORD_DFU_COMMU_SESSION_INCLUDE)
#define RECORD_DFU_COMMU_SESSION_INCLUDE

#include "ConfigVariableMgr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDfuCommuSession
{
public:
	CDfuCommuSession(void);
	~CDfuCommuSession(void);

public:
	//设置参数访问对象指针
	void SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam);

	//初始化
	//true：成功 false：失败
	bool InitDfuCommuSession();

	//启动
	//true：成功 false：失败
	bool StartCommuSession();

	//停止
	//true：成功 false：失败
	bool StopCommuSession();

public:
	//与dfu通讯主线程函数
	//0：线程退出
	int DfuCommuOperationLoop();

private:
	//从socket读取报文
	//读取到的字节数
	int ReceiveMsg(RECORD_DFU_MSG* pMsg);

	//发送报文
	int WriteRecordMsg(RECORD_DFU_MSG* pMsg);

	//打印输出
	void LogMessage(const RECORD_DFU_MSG* pMsg, const LOG_BUFFER_HEAD& pHead);

	//拷贝报文字符
	void CopyMessageToString(BYTE bMsg, char*& pChar);

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
};

#endif
