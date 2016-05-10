#if !defined(RECORD_DFU_MAIN_FLOW_INCLUDE)
#define RECORD_DFU_MAIN_FLOW_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../common/MongodbAccess.h"
#include "JsonMsgParser.h"
#include "DfuMsgToJson.h"
#include "RecordAPCIHandler.h"
#include "ConvertMsgToComtradeFile.h"

typedef int (*PRESULTMSGCALLBACKFUNC)(int nTransMask, int nCommandID, cJSON* pJsonMsg, XJHANDLE pReserved);

class CDfuMainFlow
{
public:
	CDfuMainFlow(
		COLLECTOR_DATA_SYS_PARAM* pSysParamHandler, 
		COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParamHandler, 
		CMongodbAccess* pMongodbObj);

	~CDfuMainFlow(void);

public:
	void RegisterCommandResultCallBack(PRESULTMSGCALLBACKFUNC pCallBack, XJHANDLE pReserved);

	bool InitMainFlow();

	bool StartMainFlow();

	bool StopMainFlow();

	bool PushCommandToDfuApci(DFUMESSAGE& command_msg);

	bool PushDfuResultMsg(DFU_COMMU_MSG& result_msg);

	int GetMsgTransMask();

public:
	int MainBuinessLoop();

	int ResultBuinessLoop();

	int FileBuinessLoop();

private:
	bool InitLogFile();

	bool PullCommandToDfuApci(DFU_COMMU_MSG& command_msg);

	bool CheckCommandFinish(DFUMESSAGE& full_command_msg);

	bool PushFileResultBuf(DFUMESSAGE& file_result);

	bool PullFileFromList(DFUMESSAGE& file_result);

	bool ProcessClientResultMsg(DFUMESSAGE& client_result_msg);

	bool PorocessFListResultMsg(DFUMESSAGE& file_list_msg);

	bool LaunchQueryNewFile();

	bool LaunchReadNewFile(UINT& uIndex);

	bool LaunchManualFile();

private:
	CRecordAPCIHandler* m_pApciHandler;

	COLLECTOR_DATA_SYS_PARAM* m_pSysParamHandler;

	COLLECTOR_DFU_COMMU_PARAM* m_pDfuCommuParamHandler;

	CMongodbAccess* m_pMongoAccessHandler;

	CConvertMsgToComtradeFile* m_pComtradeFileConverter;

private:
	DFUMESSAGE_BUF m_CommandMsgBuf;

	DFUMESSAGE_BUF m_FileMsgBuf;

	CRecordmanThread m_MainBuinessThread;

	CRecordmanThread m_ResultBuinessThread;

	CRecordmanThread m_FileBuinessThread;

	CSafeLock m_LockCommandBuf;

	CSafeLock m_LockFileBuf;

private:
	CMessageLog m_LogFile;

	bool m_bExitFlag;

	bool m_bQueryNewFile;

	time_t m_tCheckFile;

private:
	PRESULTMSGCALLBACKFUNC m_pCommandResultCallBackFunc;

	XJHANDLE m_pCallBackCallObj;
};

#endif

