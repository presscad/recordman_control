#include "InternalCommuMgr.h"

//************************************
// 函 数 名： AmqpCommandRecvFun
// 功能概要： 从rabbitmq接收报文的函数
// 访问权限： public 
// 返 回 值： int 
// 参    数： amqp_envelope_t * pAmqp_envelope_t 
// 参    数： void * pReserved 
//************************************
int AmqpCommandRecvFun(amqp_envelope_t* pAmqp_envelope_t, XJHANDLE pReserved)
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

//json result msg recv fun
int CommandResultRecvFunc(int nTransMask, int nCommandID, cJSON* pJsonMsg, XJHANDLE pReserved)
{
	if (NULL == pJsonMsg)
	{
		return 0;
	}

	if (NULL == pReserved)
	{
		return 0;
	}

	CInternalCommuMgr* pInternalCommuMgr = (CInternalCommuMgr*)pReserved;
	bool bRet = pInternalCommuMgr->AddAmqpResultMsg(pJsonMsg, nTransMask, nCommandID);

	return (true == bRet)?1:0;
}

//process amqp msg thread
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

//json msg send thread
THREAD_FUNC WINAPI AmqpResultSendProc(LPVOID pParam)
{
	CInternalCommuMgr* pInternalCommuMgr = (CInternalCommuMgr*)pParam;
	if (NULL == pInternalCommuMgr)
	{
		return THREAD_RETURN;
	}

	try
	{
		pInternalCommuMgr->AmqpMsgSendLoop();
	}
	catch (...)
	{
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

CInternalCommuMgr::CInternalCommuMgr(void):
m_LockAmqpRecvMsg("LOCK_AMQP_RECV_LIST"),
m_LockResultMsg("LOCK_RESULT_MSG_LIST")
{
	m_bExit = true;
	m_pRabbitmqParm = NULL;
	m_pInterRabbitCommuHandler = NULL;
	m_pRecordApciHandler = NULL;
	m_pSystemParm = NULL;

	m_veAmqpCommand.clear();
	m_veJsonSendList.clear();
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
		InitLogFile();

		m_pRecordApciHandler->RegisterResultCallBackFunc(CommandResultRecvFunc, this);

		if (NULL == m_pInterRabbitCommuHandler)
		{
			m_pInterRabbitCommuHandler = new CRabbitmqAccess;
		}

		if (NULL == m_pInterRabbitCommuHandler)
		{
			m_LogFile.FormatAdd(CLogFile::error, "new class CRabbitMqFactory failed！");
			return false;
		}

		m_pInterRabbitCommuHandler->SetRabbitAccessParam(
			&m_pRabbitmqParm->rabbitmq_basick_param);

		if (false == m_pInterRabbitCommuHandler->ConnectRabbitMqServer())
		{
			m_LogFile.FormatAdd(CLogFile::error, "[InitCommandMonitorHandler]connect rabbitmq server failed！");
			return false;
		}

		m_LogFile.FormatAdd(CLogFile::trace, "InitCommandMonitorHandler succeed！");
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[InitCommandMonitorHandler]init command monitor handler find exception！");
		return false;
	}

	return true;
}

//start recv thread and msg process thread
bool CInternalCommuMgr::StartCommandMonitorHandler()
{
	RABBIT_RECV_PARAM rabbit_rcv_param;
	sprintf(rabbit_rcv_param.chQueueName, "%s", m_pRabbitmqParm->chCollectorRecvQueName);

	m_pInterRabbitCommuHandler->RegisterRecvHandler(AmqpCommandRecvFun, this);//注册接收回调函数
	if (false == m_pInterRabbitCommuHandler->StartAmqpRecv(rabbit_rcv_param))
	{
		m_LogFile.FormatAdd(CLogFile::error, "[StartCommandMonitorHandler]start amqp recv failed！");
		return false;
	}

	m_LogFile.FormatAdd(CLogFile::trace, "[StartCommandMonitorHandler]start amqp recv succeed！");

	m_IdlerMqCommandThread.Stop();
	m_AmqpMsgSendThread.Stop();

	m_bExit = false;
	if (false == m_IdlerMqCommandThread.Start(AmqpCommandOperationProc, this))
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[StartCommandMonitorHandler]start amqp command process thread failed！");
		return false;
	}
	m_LogFile.FormatAdd(CLogFile::trace, 
		"[StartCommandMonitorHandler]start amqp command process thread succeed！");

	if (false == m_AmqpMsgSendThread.Start(AmqpResultSendProc, this))
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[StartCommandMonitorHandler]start json result send thread failed！");
		return false;
	}
	m_LogFile.FormatAdd(CLogFile::trace, 
		"[StartCommandMonitorHandler]start json result send thread succeed！");

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
	m_AmqpMsgSendThread.Stop();

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

//json result msg send thread main loop
int CInternalCommuMgr::AmqpMsgSendLoop()
{
	while (!m_bExit)
	{
		if (true == m_bExit)
		{
			break;;
		}

		JSON_SENDMSG json_send_msg;
		if (false == GetAmqpResultMsg(json_send_msg))
		{
			MySleep(200);
			continue;
		}

		if (json_send_msg.pJsonMsg != NULL)
		{
			m_pInterRabbitCommuHandler->SendMsg(
				json_send_msg.pJsonMsg, 
				json_send_msg.sender_info, 
				json_send_msg.sender_channel);

			if (NULL != json_send_msg.pJsonMsg)
			{
				cJSON_Delete(json_send_msg.pJsonMsg);
				json_send_msg.pJsonMsg = NULL;
			}
		}
	}

	return 0;
}

//process msg function
bool CInternalCommuMgr::ProcessAmqpCommand(amqp_envelope_t* pAmqpComand)
{
	cJSON* pRecvRootJson = NULL;
	cJSON* pResultJson = NULL;
	CJsonMsgParser jsonMsgparser;
	char* messageBody = NULL;
	DFUMESSAGE dfuMsg;
	int nDfuMsgID = -1;
	
	messageBody = new char[pAmqpComand->message.body.len];
	bzero(messageBody, pAmqpComand->message.body.len);

	memcpy(messageBody, pAmqpComand->message.body.bytes, pAmqpComand->message.body.len);

	pRecvRootJson = cJSON_Parse(messageBody);
	if (NULL == pRecvRootJson)
	{
		return false;
	}

	jsonMsgparser.Attach(pRecvRootJson);
	if (false == jsonMsgparser.JsonToRecordDfuMsg(dfuMsg.command_msg, nDfuMsgID))//转换失败
	{
		m_LogFile.FormatAdd(CLogFile::error, "[ProcessAmqpCommand]recv commmand：%d，transform dfu msg failed！", 
			jsonMsgparser.GetCommandID());

		pResultJson = cJSON_CreateObject();
		cJSON_AddNumberToObject(pResultJson, "command_id", (jsonMsgparser.GetCommandID() + 1));
		cJSON_AddNumberToObject(pResultJson, "result", RECORD_COMMAND_RESULT_FAILED);
		m_pInterRabbitCommuHandler->SendMsg(
			pResultJson, pAmqpComand->message.properties, pAmqpComand->channel);//send rabbitmq
	}
	else
	{
		JSON_SENDMSG json_send_msg;
		json_send_msg.bEnd = false;
		json_send_msg.nCommandID = jsonMsgparser.GetCommandID();
		json_send_msg.nTransMask = m_pRecordApciHandler->CreateTransMask();
		json_send_msg.sender_channel = pAmqpComand->channel;
		json_send_msg.sender_info = pAmqpComand->message.properties;
		AddResultWaitMsg(json_send_msg);//加入等待队列

		dfuMsg.nDfuCommandID = nDfuMsgID;
		dfuMsg.nInternalCommandID = jsonMsgparser.GetCommandID();
		dfuMsg.nCommandNum = dfuMsg.command_msg.size();
		dfuMsg.nMsgType = RECORD_DFU_MESSAGE_TYPE_JSON;
		dfuMsg.nTransMask = json_send_msg.nTransMask;
		dfuMsg.bRecvEnd = false;
		m_pRecordApciHandler->PostDfuMsg(dfuMsg);//发送给dfu

		m_LogFile.FormatAdd(CLogFile::error, "[ProcessAmqpCommand]post commmand：%d to dfu！", 
			jsonMsgparser.GetCommandID());
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

//get a send json msg
bool CInternalCommuMgr::GetAmqpResultMsg(JSON_SENDMSG& json_send_msg)
{
	CLockUp lockUp(&m_LockResultMsg);

	vector<JSON_SENDMSG>::iterator it = m_veJsonSendList.begin();
	for (; it != m_veJsonSendList.end(); it++)
	{
		if (it->bEnd == true)
		{
			json_send_msg = *it;
			m_veJsonSendList.erase(it);
			return true;
		}
	}

	return false;
}

//add a wait result command
void CInternalCommuMgr::AddResultWaitMsg(JSON_SENDMSG& json_send_msg)
{
	CLockUp lockUp(&m_LockResultMsg);

	m_veJsonSendList.push_back(json_send_msg);
}

//add a result msg
bool CInternalCommuMgr::AddAmqpResultMsg(cJSON* pResultMsg, int nTransMask, int nCommmandID)
{
	CLockUp lockUp(&m_LockResultMsg);

	vector<JSON_SENDMSG>::iterator it = m_veJsonSendList.begin();
	for (; it != m_veJsonSendList.end(); it++)
	{
		if ((it->nTransMask == nTransMask) && (it->nCommandID == nCommmandID))
		{
			it->pJsonMsg = pResultMsg;
			it->bEnd = true;
			return true;
		}
	}

	return false;
}

//add msg from rabbitmq to msg queue
void CInternalCommuMgr::AddAmqpCommand(amqp_envelope_t* pAmqpEnvelope)
{
	CLockUp lockUp(&m_LockAmqpRecvMsg);

	m_veAmqpCommand.push_back(pAmqpEnvelope);
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

//init logfile
bool CInternalCommuMgr::InitLogFile()
{
	m_LogFile.Close();

	m_LogFile.SetLogPath(m_pSystemParm->chLogpath);
	m_LogFile.SetLogLevel(m_pSystemParm->nLoglevel);
	m_LogFile.SetLogSaveDays(m_pSystemParm->nLogDays);

	return (TRUE == m_LogFile.Open("internal_commu_log"))?true:false;
}
