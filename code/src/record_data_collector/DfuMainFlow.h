#if !defined(RECORD_DFU_MAIN_FLOW_INCLUDE)
#define RECORD_DFU_MAIN_FLOW_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../common/MongodbAccess.h"
#include "JsonMsgParser.h"
#include "DfuMsgToJson.h"
#include "../../common/common_time.h"
#include "../../common/CreateComtrade.h"
#include "../../common/GECodeConvert.h"
#include "RecordAPCIHandler.h"

typedef int (*PRESULTMSGCALLBACKFUNC)(int nTransMask, int nCommandID, cJSON* pJsonMsg, XJHANDLE pReserved);

class CDfuMainFlow
{
public:
	CDfuMainFlow(void);
	~CDfuMainFlow(void);

public:
	void SetMainFlowParamHandler(COLLECTOR_DATA_SYS_PARAM* pSysParamHandler, COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParamHandler);

	void SetMongoDbAccessHandler(CMongodbAccess* pMongodbObj);

	void RegisterCommandResultCallBack(PRESULTMSGCALLBACKFUNC pCallBack, XJHANDLE pReserved);

public:
	bool InitMainFlow();

	bool StartMainFlow();

	bool StopMainFlow();

	bool PushCommandToDfuApci(DFUMESSAGE& command_msg);

	bool PushDfuResultMsg(DFU_COMMU_MSG result_msg);

	int GetMsgTransMask();

public:
	int MainBuinessLoop();

	int ResultBuinessLoop();

	int FileBuinessLoop();

private:
	//init logfile
	bool InitLogFile();

private:
	//check msg is over
	bool CheckCommandFinish(DFUMESSAGE& full_command_msg);

	//process client result msg
	bool ProcessClientResultMsg(DFUMESSAGE& client_result_msg);

	//query file list msg
	bool PorocessFListResultMsg(DFUMESSAGE& file_list_msg);

	//process file msg(analyze and save)
	bool ProcessFileResultMsg(DFUMESSAGE& file_msg);

	//osc info
	bool GetOscInfo(comtradeHead& head);

	//analyze msg header
	bool AnalyzeFileMsgHeader(comtradeHead& head, DFU_COMMU_MSG* pMsg, int& nOffset, UINT& uDatablockNum);

	//analyze msg samples
	bool AnalyzeFileMsgSamples(list<sampleInfo>& samples, float uSampleNum, DFU_COMMU_MSG* pMsg, int& nOffset);

	//analyze msg ai
	bool AnalyzeFileMsgAis(list<short>& data_vals, UINT uAiNum, DFU_COMMU_MSG* pMsg, int& nOffset);

	//analyze msg di
	bool AnalyzeFileMsgDis(list<short>& data_vals, UINT uDiNum, DFU_COMMU_MSG* pMsg, int& nOffset);

private:
	CRecordAPCIHandler* m_pApciHandler;

	COLLECTOR_DATA_SYS_PARAM* m_pSysParamHandler;

	COLLECTOR_DFU_COMMU_PARAM* m_pDfuCommuParamHandler;

	CMessageLog m_LogFile;

	//mongo access
	CMongodbAccess* m_pMongoAccessHandler;

	//command list lock
	CSafeLock m_LockCommandBuf;

	//File list lock
	CSafeLock m_LockFileBuf;

	PRESULTMSGCALLBACKFUNC m_pCommandResultCallBackFunc;

	XJHANDLE m_pCallBackCallObj;

private:
	DFUMESSAGE_BUF m_CommandMsgBuf;

	DFUMESSAGE_BUF m_FileMsgBuf;

	CRecordmanThread m_MainBuinessThread;

	CRecordmanThread m_ResultBuinessThread;

	CRecordmanThread m_FileBuinessThread;

private:
	bool m_bExitFlag;

	//check new file time
	time_t m_tCheckFile;
};

#endif

