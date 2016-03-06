#if !defined(RECORD_COMMAND_MONITOR_HANDLER_INCLUDE)
#define RECORD_COMMAND_MONITOR_HANDLER_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "RabbitMqFactory.h"

class CCommandMonitorHandler
{
public:
	CCommandMonitorHandler(void);
	~CCommandMonitorHandler(void);

public:
	//设置配置类访问句柄
	void SetConfigVariableHandle(CConfigVariableMgr* pObj);

public:
	bool InitCommandMonitorHandler();


private:
/**	\brief 配置维护类对象*/
	CConfigVariableMgr* m_pConfigVariableObj;

	CRabbitMqFactory* m_pCommuHandle;
};

#endif
