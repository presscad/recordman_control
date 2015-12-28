#if !defined(RECORD_RECORD_DATA_COLLECTOR_INCLUDE)
#define RECORD_RECORD_DATA_COLLECTOR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "DfuCommuSession.h"

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
	bool InitDfuCommuSession();

private:
/**	\brief 配置维护类对象*/
	CConfigVariableMgr* m_pConfigvarialemgr;

/**	\brief dfu通讯类对象*/
	CDfuCommuSession* m_pDfuCommuSession;
};

#endif
