#include "CommandMonitorHandler.h"


CCommandMonitorHandler::CCommandMonitorHandler(void)
{
	m_pConfigVariableObj = NULL;
	m_pCommuHandle = NULL;
}


CCommandMonitorHandler::~CCommandMonitorHandler(void)
{
}

void CCommandMonitorHandler::SetConfigVariableHandle(CConfigVariableMgr* pObj)
{
	m_pConfigVariableObj = pObj;
}

bool CCommandMonitorHandler::InitCommandMonitorHandler()
{
	try
	{
		if (NULL == m_pCommuHandle)
		{
			m_pCommuHandle = new CRabbitMqFactory;
		}

		if (NULL == m_pCommuHandle)
		{
			printf("new class CRabbitMqFactory failed£¡\n");
			return false;
		}

		m_pCommuHandle->SetConfigVariableHandle(m_pConfigVariableObj);

		if (false == m_pCommuHandle->ConnectRabbitMqServer(1))
		{
			return false;
		}

		printf("InitCommandMonitorHandler succeed£¡\n");
	}
	catch (...)
	{
		printf("[InitCommandMonitorHandler]init command monitor handler find exception£¡\n");
		return false;
	}

	return true;
}
