#if !defined(RECORD_APCI_HANDLER_INCLUDE)
#define RECORD_APCI_HANDLER_INCLUDE

#include "../../common/MongodbAccess.h"
#include "ConfigVariableMgr.h"
#include "../../common/JsonMsgParser.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef vector<RECORD_DFU_MSG> full_msg_list;

class CRecordAPCIHandler
{
public:
	CRecordAPCIHandler(void);
	~CRecordAPCIHandler(void);

public:
	//set access param
	void SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam, COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParam);

	//mongodb access param
	void SetMongoAccessParam(RECORD_MONGO_BASIC_PARAM* pMongoParam);

	//初始化
	//true：成功 false：失败
	bool InitRecordApciHandler();

	//启动
	//true：成功 false：失败
	bool StartRecordApciHandler();

	//停止
	//true：成功 false：失败
	bool StopRecordApciHandler();

	//send command and recive result
	bool ProcessJsonCommand(cJSON* pJsonCommand, cJSON*& pJsonResult);

public:
	//与dfu通讯主线程函数
	//0：线程退出
	int DfuCommuOperationLoop();

	//save new osc file
	int SaveFileOperationLoop();

private:
	//process multi command
	int ProcessCommandMsg(vector<RECORD_DFU_MSG>& veCommandMsg, vector<RECORD_DFU_MSG>& veResultMsg);
	
	//process one command
	int ProcessCommandMsg(RECORD_DFU_MSG& commandMsg, vector<RECORD_DFU_MSG>& veResultMsg);

	//read msg from socket
	//return readed msg bytes num
	int ReceiveMsg(RECORD_DFU_MSG* pMsg);

	//send msg
	int WriteRecordMsg(RECORD_DFU_MSG* pMsg);

	//recv until end flag
	int RecvFlowMsg(vector<RECORD_DFU_MSG>& veResultMsg);

private:
	//create poling command
	bool ProcessPolingCommand();

	//query new osc file from dfu
	int ProcessQueryNewFile();

	//get osc file
	void ProcessGetOscFile(int nFileIndex);

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

	//dfu commu param
	COLLECTOR_DFU_COMMU_PARAM* m_pDfuCommuParam;

	//mongo param
	RECORD_MONGO_BASIC_PARAM* m_pMongoParam;

private:
	/**	\brief 退出标志*/
	bool m_bExitFlag;

	/**	\brief 与dfu业务处理线程*/
	CRecordmanThread m_DfuOperationThread;

	//save file thread
	CRecordmanThread m_DfuFileSaveTherad;

private:
	/**	\brief net对象*/
	CNet* m_pNetSocket;

	//mongo access
	CMongodbAccess m_MongoAccessHandler;

	//msg process lock
	CSafeLock m_LockApciMsgHandler;

	//file msg list
	full_msg_list m_file_msg_list;

	//link last active time
	time_t m_tLinkActive;

	//check new file time
	time_t m_tCheckFile;

	//transmask
	UINT m_utTransMask;
};

#endif

