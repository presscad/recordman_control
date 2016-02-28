#include "CommandMonitorHandler.h"


CCommandMonitorHandler::CCommandMonitorHandler(void)
{
	m_pConfigVariableObj = NULL;
}


CCommandMonitorHandler::~CCommandMonitorHandler(void)
{
}

void CCommandMonitorHandler::SetConfigVariableHandle(CConfigVariableMgr* pObj)
{
	m_pConfigVariableObj = pObj;
}
