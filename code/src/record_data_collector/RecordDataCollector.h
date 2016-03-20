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
	//初始化必要的参数
	//true：成功 false：失败
	bool InitRecordDataColletor();

	//启动
	//true：成功 false：失败
	bool StartRecordDataColletor();

	//停止
	//true：成功 false：失败
	bool EndRecordDataColletor();

	//退出
	//true：成功 false：失败
	bool ExitRecordDataColletor();

private:
	//读取配置参数
	//true：成功 false：失败
	bool InitSysConfigVariable();
	
	//初始化dfu通讯类对象
	//true：成功 false：失败
	bool InitApciHandler();

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
};

#endif
