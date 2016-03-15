#include "InternalCommuMgr.h"

//************************************
// 函 数 名： AmqpCommandRecvFun
// 功能概要： 从rabbitmq接收报文的函数
// 访问权限： public 
// 返 回 值： int 
// 参    数： amqp_envelope_t * pAmqp_envelope_t 
// 参    数： void * pReserved 
//************************************
int AmqpCommandRecvFun(amqp_envelope_t* pAmqp_envelope_t, void* pReserved)
{
	if (NULL == pAmqp_envelope_t)
	{
		return 0;
	}

	if (NULL == pReserved)
	{
		return 0;
	}

	CInternalCommuMgr* pInternalCommuMgr = (CInternalCommuMgr*)pReserved;
	pInternalCommuMgr->AddAmqpCommand(pAmqp_envelope_t);

	return 1;
}

THREAD_FUNC WINAPI AmqpCommandOperationProc(LPVOID pParam)
{
	CInternalCommuMgr* pInternalCommuMgr = (CInternalCommuMgr*)pParam;
	if (NULL == pInternalCommuMgr)
	{
		return THREAD_RETURN;
	}

	try
	{
		pInternalCommuMgr->AmqpCommandOperationLoop();
	}
	catch (...)
	{
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

CInternalCommuMgr::CInternalCommuMgr(void):
m_LockAmqpRecvMsg("LOCK_AMQP_RECV_LIST")
{
	m_bExit = true;
	m_pConfigVariableObj = NULL;
	m_pInterRabbitCommuHandler = NULL;

	m_veAmqpCommand.clear();
}


CInternalCommuMgr::~CInternalCommuMgr(void)
{
}

void CInternalCommuMgr::SetConfigVariableHandle(CConfigVariableMgr* pObj)
{
	m_pConfigVariableObj = pObj;
}

void CInternalCommuMgr::AddAmqpCommand(amqp_envelope_t* pAmqpEnvelope)
{
	CLockUp lockUp(&m_LockAmqpRecvMsg);

	m_veAmqpCommand.push_back(pAmqpEnvelope);
}

int CInternalCommuMgr::AmqpCommandOperationLoop()
{
	amqp_envelope_t* pAmqpEnveLop = NULL;

	while (!m_bExit)
	{
		if (true == m_bExit)
		{
			break;;
		}

		if (false == GetAmqpCommand(pAmqpEnveLop))
		{
			MySleep(300);
			continue;
		}

		ProcessAmqpCommand(pAmqpEnveLop);
		m_pInterRabbitCommuHandler->FreeAmqpEnveloptObj(pAmqpEnveLop);
	}

	return 0;
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
			printf("new class CRabbitMqFactory failed！\n");
			return false;
		}

		m_pInterRabbitCommuHandler->SetRabbitAccessParam(&m_pConfigVariableObj->m_rabbit_mq_param);
		if (false == m_pInterRabbitCommuHandler->ConnectRabbitMqServer(DATA_COLLECTOR_COMMAND_CHANNEL))
		{
			return false;
		}

		printf("InitCommandMonitorHandler succeed！\n");
	}
	catch (...)
	{
		printf("[InitCommandMonitorHandler]init command monitor handler find exception！\n");
		return false;
	}

	return true;
}

bool CInternalCommuMgr::StartCommandMonitorHandler()
{
	m_IdlerMqCommandThread.Stop();
	m_bExit = false;
	if (false == m_IdlerMqCommandThread.Start(AmqpCommandOperationProc, this))
	{
		return false;
	}

	m_pInterRabbitCommuHandler->RegisterRecvHandler(AmqpCommandRecvFun, this);//注册接收回调函数
	if (false == m_pInterRabbitCommuHandler->StartAmqpRecv(DATA_COLLECTOR_RECV_QUEUE_NAME))
	{
		return false;
	}

	return true;
}

bool CInternalCommuMgr::StopCommandMonitorHandler()
{
	if (NULL == m_pInterRabbitCommuHandler)
	{
		return false;
	}

	m_bExit = true;

	m_pInterRabbitCommuHandler->StopAmqpRecv();
	m_IdlerMqCommandThread.Stop();

	if (NULL != m_pInterRabbitCommuHandler)
	{
		delete m_pInterRabbitCommuHandler;
		m_pInterRabbitCommuHandler = NULL;
	}

	return true;
}

bool CInternalCommuMgr::GetAmqpCommand(amqp_envelope_t*& pAmqpComand)
{
	CLockUp lockUp(&m_LockAmqpRecvMsg);

	if (0 >= m_veAmqpCommand.size())
	{
		return false;
	}

	vector<amqp_envelope_t*>::iterator it = m_veAmqpCommand.begin();
	pAmqpComand = *it;
	m_veAmqpCommand.erase(it);

	return true;
}

bool CInternalCommuMgr::ProcessAmqpCommand(amqp_envelope_t* pAmqpComand)
{
	int nCommandID = 0;
	cJSON* pRecvRootJson = NULL;
	CJsonMsgParser jsonMsgparser;
	char* messageBody = NULL;
	
	messageBody = new char[pAmqpComand->message.body.len];
	bzero(messageBody, pAmqpComand->message.body.len);

	memcpy(messageBody, pAmqpComand->message.body.bytes, pAmqpComand->message.body.len);

	pRecvRootJson = cJSON_Parse(messageBody);
	if (NULL == pRecvRootJson)
	{
		return false;
	}

	jsonMsgparser.Attach(pRecvRootJson);
	nCommandID = jsonMsgparser.GetCommandID();

	printf("recv msg is：%s，msgid is：%d \n", 
		cJSON_Print(pRecvRootJson), nCommandID);

	m_pInterRabbitCommuHandler->SendMsg(pRecvRootJson, pAmqpComand->message.properties);

	cJSON_Delete(pRecvRootJson);
	pRecvRootJson = NULL;

	free(messageBody);
}
