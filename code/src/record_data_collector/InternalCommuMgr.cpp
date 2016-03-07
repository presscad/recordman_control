#include "InternalCommuMgr.h"


CInternalCommuMgr::CInternalCommuMgr(void)
{
	m_pConfigVariableObj = NULL;
	m_pInterRabbitCommuHandler = NULL;
}


CInternalCommuMgr::~CInternalCommuMgr(void)
{
}

void CInternalCommuMgr::SetConfigVariableHandle(CConfigVariableMgr* pObj)
{
	m_pConfigVariableObj = pObj;
}

bool CInternalCommuMgr::InitCommandMonitorHandler()
{
	try
	{
		if (NULL == m_pInterRabbitCommuHandler)
		{
			m_pInterRabbitCommuHandler = new CRabbitmqAccess;
		}

		if (NULL == m_pInterRabbitCommuHandler)
		{
			printf("new class CRabbitMqFactory failed£¡\n");
			return false;
		}

		m_pInterRabbitCommuHandler->SetRabbitAccessParam(&m_pConfigVariableObj->m_rabbit_mq_param);

		if (false == m_pInterRabbitCommuHandler->ConnectRabbitMqServer(1))
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
