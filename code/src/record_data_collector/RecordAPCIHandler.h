#if !defined(RECORD_APCI_HANDLER_INCLUDE)
#define RECORD_APCI_HANDLER_INCLUDE

#include "../../common/MongodbAccess.h"
#include "ConfigVariableMgr.h"
#include "JsonMsgParser.h"
#include "../../common/MessageLog.h"
#include "DfuMsgAttach.h"
#include "DfuMsgToJson.h"

typedef int (*PJSONRESULTFUNCALLBACK)(int nTransMask, int nCommandID, cJSON* pJsonMsg, XJHANDLE pReserved);

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef vector<DFUMESSAGE> dfumsg_list;

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

	//set result json fun
	void RegisterJsonResultFunc(PJSONRESULTFUNCALLBACK pFunc, XJHANDLE pObj);

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
	int PostDfuMsg(DFUMESSAGE& command);

	//get msg trans mask
	UINT GetMsgTransMask();

public:
	//与dfu通讯主线程函数
	//0：线程退出
	int DfuCommuOperationLoop();

	//recv loop
	int DfuRecvOperationLoop();

	//save new osc file
	int SaveFileOperationLoop();

private:
	//read msg from socket
	//return readed msg bytes num
	int ReceiveMsg(DFU_COMMU_MSG& pMsg);

	//send msg
	int SendDfuMessage(DFU_COMMU_MSG& record_msg);

private:
	//init logfile
	bool InitLogFile();

	//get send msg from list
	bool GetDfuSendMsg(DFU_COMMU_MSG& sendmsg);

	//add result msg to list
	bool AddDfuResultMsg(DFU_COMMU_MSG& recv_msg);

	//check msg is over
	bool CheckCommandOver(DFUMESSAGE& full_command_msg);

	//get follow up msg
	bool GetFollowUpMsg(int nMsgTrans, int nCommandID, DFU_COMMU_MSG& follow_up_msg);

	//create link test command
	bool LaunchLinkTest();

	//query new osc file from dfu
	int LaunchQueryNewFile();

	//get osc file
	void ProcessGetOscFile(int nFileIndex);

	//create test_msg
	void CreateTestReplyMsg(DFU_COMMU_MSG& reply_msg, DFU_COMMU_MSG recv_msg);

private:
	//print msg
	void WriteMsgLog(const DFU_COMMU_MSG& pMsg, const LOG_BUFFER_HEAD& pHead);

private:
	/**	\brief 配置参数对象*/
	COLLECTOR_DATA_SYS_PARAM* m_pCollectorSysParam;

	//dfu commu param
	COLLECTOR_DFU_COMMU_PARAM* m_pDfuCommuParam;

	//mongo param
	RECORD_MONGO_BASIC_PARAM* m_pMongoParam;

	//log handler
	CMessageLog m_LogFile;

private:
	/**	\brief 退出标志*/
	bool m_bExitFlag;

	/**	\brief 与dfu业务处理线程*/
	CRecordmanThread m_DfuOperationThread;

	//recv thread
	CRecordmanThread m_DfuRecvThread;

	//save file thread
	CRecordmanThread m_DfuFileSaveTherad;

private:
	/**	\brief net对象*/
	CNet* m_pNetSocket;

	//mongo access
	CMongodbAccess m_MongoAccessHandler;

	//msg process lock
	CSafeLock m_LockSocketSend;

	//command list lock
	CSafeLock m_LockCommandList;

private:
	//dfu msg list
	dfumsg_list m_command_msg_list;

	//json result fun
	PJSONRESULTFUNCALLBACK m_pJsonResultFun;

	XJHANDLE m_pJsonResultHandle;

	//link last active time
	time_t m_tLinkActive;

	//check new file time
	time_t m_tCheckFile;

	//transmask
	UINT m_utTransMask;
};

#endif

