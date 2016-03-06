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
	//������������ʾ��
	void SetConfigVariableHandle(CConfigVariableMgr* pObj);

public:
	bool InitCommandMonitorHandler();


private:
/**	\brief ����ά�������*/
	CConfigVariableMgr* m_pConfigVariableObj;

	CRabbitMqFactory* m_pCommuHandle;
};

#endif
