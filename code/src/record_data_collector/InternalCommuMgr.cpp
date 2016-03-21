#include "InternalCommuMgr.h"

//************************************
// �� �� ���� AmqpCommandRecvFun
// ���ܸ�Ҫ�� ��rabbitmq���ձ��ĵĺ���
// ����Ȩ�ޣ� public 
// �� �� ֵ�� int 
// ��    ���� amqp_envelope_t * pAmqp_envelope_t 
// ��    ���� void * pReserved 
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
	m_pRabbitmqParm = NULL;
	m_pInterRabbitCommuHandler = NULL;
	m_pRecordApciHandler = NULL;
	m_pSystemParm = NULL;

	m_veAmqpCommand.clear();
}


CInternalCommuMgr::~CInternalCommuMgr(void)
{
	m_LogFile.Close();
}

//set rabbitmq access basic param
void CInternalCommuMgr::SetRabbitmqAccessParam(COLLECTOR_ADVANCE_RABBITMQ_PARAM* pObj)
{
	m_pRabbitmqParm = pObj;
}

//set system config param
void CInternalCommuMgr::SetSystemParam(COLLECTOR_DATA_SYS_PARAM* pSystemParam)
{
	m_pSystemParm = pSystemParam;
}

void CInternalCommuMgr::SetRecordApciHandler(CRecordAPCIHandler* pApciHandler)
{
	m_pRecordApciHandler = pApciHandler;
}

//init class member or other class
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
			m_LogFile.FormatAdd(CLogFile::error, "new class CRabbitMqFactory failed��");
			return false;
		}

		m_pInterRabbitCommuHandler->SetRabbitAccessParam(
			&m_pRabbitmqParm->rabbitmq_basick_param);

		if (false == m_pInterRabbitCommuHandler->ConnectRabbitMqServer())
		{
			m_LogFile.FormatAdd(CLogFile::error, "[InitCommandMonitorHandler]connect rabbitmq server failed��");
			return false;
		}

		m_LogFile.FormatAdd(CLogFile::trace, "InitCommandMonitorHandler succeed��");
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[InitCommandMonitorHandler]init command monitor handler find exception��");
		return false;
	}

	return true;
}

//start recv thread and msg process thread
bool CInternalCommuMgr::StartCommandMonitorHandler()
{
	RABBIT_RECV_PARAM rabbit_rcv_param;
	sprintf(rabbit_rcv_param.chQueueName, "%s", m_pRabbitmqParm->chCollectorRecvQueName);

	m_pInterRabbitCommuHandler->RegisterRecvHandler(AmqpCommandRecvFun, this);//ע����ջص�����
	if (false == m_pInterRabbitCommuHandler->StartAmqpRecv(rabbit_rcv_param))
	{
		return false;
	}

	m_IdlerMqCommandThread.Stop();
	m_bExit = false;
	if (false == m_IdlerMqCommandThread.Start(AmqpCommandOperationProc, this))
	{
		return false;
	}

	return true;
}

//stop monitor
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

//process msg from rabbitmq
//get a msg from queue one by one
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

//process msg function
bool CInternalCommuMgr::ProcessAmqpCommand(amqp_envelope_t* pAmqpComand)
{
	int nCommandID = 0;
	cJSON* pRecvRootJson = NULL;
	cJSON* pResultJson = NULL;
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

	if (true == m_pRecordApciHandler->ProcessJsonCommand(pRecvRootJson, pResultJson))
	{
		m_pInterRabbitCommuHandler->SendMsg(pResultJson, pAmqpComand->message.properties, pAmqpComand->channel);
	}

	if (NULL != pRecvRootJson)
	{
		cJSON_Delete(pRecvRootJson);
		pRecvRootJson = NULL;
	}

	if (NULL != pResultJson)
	{
		cJSON_Delete(pResultJson);
		pResultJson = NULL;
	}

	if (NULL != messageBody)
	{
		free(messageBody);
		messageBody = NULL;
	}

	return true;
}

//add msg from rabbitmq to msg queue
void CInternalCommuMgr::AddAmqpCommand(amqp_envelope_t* pAmqpEnvelope)
{
	CLockUp lockUp(&m_LockAmqpRecvMsg);

	m_veAmqpCommand.push_back(pAmqpEnvelope);
}

//init logfile
bool CInternalCommuMgr::InitLogFile()
{
	m_LogFile.Close();

	m_LogFile.SetLogPath(m_pSystemParm->chLogpath);
	m_LogFile.SetLogLevel(m_pSystemParm->nLoglevel);
	m_LogFile.SetLogSaveDays(m_pSystemParm->nLogDays);

	return (TRUE == m_LogFile.Open("internal_commu_log"))?true:false;
}

//get msg fron queue
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
