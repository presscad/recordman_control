#if !defined(RECORD_RECORD_DATA_COLLECTOR_INCLUDE)
#define RECORD_RECORD_DATA_COLLECTOR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordAPCIHandler.h"
#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "CommandHandlerMgr.h"
#include "InternalCommuMgr.h"

class CRecordDataCollector
{
public:
	CRecordDataCollector(void);
	~CRecordDataCollector(void);

public:
	//init
	//true：success false：failed
	bool InitRecordDataColletor();

	//start
	//true：success false：failed
	bool StartRecordDataColletor();

	//stop
	//true：success false：failed
	bool EndRecordDataColletor();

	//exit
	//true：success false：failed
	bool ExitRecordDataColletor();

private:
	//init log file
	bool InitLogFile();

	//load config param
	//true：success false：failed
	bool InitSysConfigVariable();
	
	//初始化dfu通讯类对象
	//true：成功 false：失败
	bool InitApciHandler();

	//init internalcommu mgr
	bool InitInternalCommuMgr();

	//初始化命令处理会话管理类对象
	//true：成功 false：失败
	bool InitCommandHandlerMgr();

private:
/**	\brief 配置维护类对象*/
	CConfigVariableMgr* m_pConfigvarialemgr;

/**	\brief 命令处理管理类*/
	CCommandHandlerMgr* m_pCommandHandlerMgr;

/**	\brief 与dfu通信管理类*/
	CRecordAPCIHandler* m_pRecordApciHandler;

/**	\brief 系统内部消息管理*/
	CInternalCommuMgr* m_pInternalCommuMgr;

public:
/**	\brief logfile*/
	CMessageLog m_Log;

private:
	//log level
	int m_nLogLevel;

	//log days
	int m_nLogDays;

	//log path
	char m_chLogpath[MAX_LINE_LENGTH];
};

#endif
