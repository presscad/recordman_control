#include "RabbitMqCommandMgr.h"

int rmq_msg_recv_callback(amqp_envelope_t* pAmqp_envelope_t, XJHANDLE pReserved)
{
	if (NULL == pAmqp_envelope_t)
	{
		return 0;
	}

	if (NULL == pReserved)
	{
		return 0;
	}

	CRabbitMqCommandMgr* pInternalCommuMgr = (CRabbitMqCommandMgr*)pReserved;
	pInternalCommuMgr->Add_rmq_command_list(pAmqp_envelope_t);

	return 1;
}

int command_result_recv_callback(int nTransMask, int nCommandID, cJSON* pJsonMsg, XJHANDLE pReserved)
{
	if (NULL == pJsonMsg)
	{
		return 0;
	}

	if (NULL == pReserved)
	{
		return 0;
	}

	CRabbitMqCommandMgr* pInternalCommuMgr = (CRabbitMqCommandMgr*)pReserved;
	bool bRet = pInternalCommuMgr->Add_rmq_result_list(pJsonMsg, nTransMask, nCommandID);

	return (true == bRet)?1:0;
}

THREAD_FUNC WINAPI rmq_command_operation_proc(LPVOID pParam)
{
	CRabbitMqCommandMgr* pInternalCommuMgr = (CRabbitMqCommandMgr*)pParam;
	if (NULL == pInternalCommuMgr)
	{
		return THREAD_RETURN;
	}

	try
	{
		pInternalCommuMgr->rmq_command_operation_loop();
	}
	catch (...)
	{
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

THREAD_FUNC WINAPI send_result_to_rmq_proc(LPVOID pParam)
{
	CRabbitMqCommandMgr* pInternalCommuMgr = (CRabbitMqCommandMgr*)pParam;
	if (NULL == pInternalCommuMgr)
	{
		return THREAD_RETURN;
	}

	try
	{
		pInternalCommuMgr->send_msg_to_rmq_loop();
	}
	catch (...)
	{
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

CRabbitMqCommandMgr::CRabbitMqCommandMgr(void):
m_LockAmqpRecvMsg("LOCK_AMQP_RECV_LIST"),
m_LockResultMsg("LOCK_RESULT_MSG_LIST")
{
	m_bExit = true;
	m_pConfigHandle = NULL;
	m_pInterRabbitCommuHandler = NULL;
	m_pMainFlow = NULL;

	m_veAmqpCommand.clear();
	m_veJsonSendList.clear();
}


CRabbitMqCommandMgr::~CRabbitMqCommandMgr(void)
{
	m_LogFile.Close();
}

void CRabbitMqCommandMgr::SetConfigVariableMgrHandle(CConfigVariableMgr* pConfigHandle)
{
	m_pConfigHandle = pConfigHandle;
}

void CRabbitMqCommandMgr::SetDfuMainFlowHandler(CDfuMainFlow* pMainFlowHandler)
{
	m_pMainFlow = pMainFlowHandler;
}

bool CRabbitMqCommandMgr::InitCommandMonitorHandler()
{
	try
	{
		InitLogFile();

		m_pMainFlow->RegisterCommandResultCallBack(command_result_recv_callback, this);

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
			m_pConfigHandle->GetRabmqBasicParamHandle());

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

bool CRabbitMqCommandMgr::StartCommandMonitorHandler()
{
	RABBIT_RECV_PARAM rabbit_rcv_param;
	sprintf(rabbit_rcv_param.chQueueName, "%s", m_pConfigHandle->GetRabmqadParam_RevQueName());

	m_pInterRabbitCommuHandler->RegisterRecvHandler(rmq_msg_recv_callback, this);//注册接收回调函数
	if (false == m_pInterRabbitCommuHandler->StartAmqpRecv(rabbit_rcv_param))
	{
		m_LogFile.FormatAdd(CLogFile::error, "[StartCommandMonitorHandler]start amqp recv failed！");
		return false;
	}

	m_LogFile.FormatAdd(CLogFile::trace, "[StartCommandMonitorHandler]start amqp recv succeed！");

	m_AmqpCommandProcessThread.Stop();
	m_AmqpMsgSendThread.Stop();

	m_bExit = false;
	if (false == m_AmqpCommandProcessThread.Start(rmq_command_operation_proc, this))
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[StartCommandMonitorHandler]start amqp command process thread failed！");
		return false;
	}
	m_LogFile.FormatAdd(CLogFile::trace, 
		"[StartCommandMonitorHandler]start amqp command process thread succeed！");

	if (false == m_AmqpMsgSendThread.Start(send_result_to_rmq_proc, this))
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[StartCommandMonitorHandler]start json result send thread failed！");
		return false;
	}
	m_LogFile.FormatAdd(CLogFile::trace, 
		"[StartCommandMonitorHandler]start json result send thread succeed！");

	return true;
}

bool CRabbitMqCommandMgr::StopCommandMonitorHandler()
{
	if (NULL == m_pInterRabbitCommuHandler)
	{
		return false;
	}

	m_bExit = true;

	m_pInterRabbitCommuHandler->StopAmqpRecv();
	m_AmqpCommandProcessThread.Stop();
	m_AmqpMsgSendThread.Stop();

	if (NULL != m_pInterRabbitCommuHandler)
	{
		delete m_pInterRabbitCommuHandler;
		m_pInterRabbitCommuHandler = NULL;
	}

	return true;
}

int CRabbitMqCommandMgr::rmq_command_operation_loop()
{
	amqp_envelope_t* pAmqpEnveLop = NULL;

	while (!m_bExit)
	{
		if (true == m_bExit)
		{
			break;;
		}

		if (false == Get_rmq_command_from_list(pAmqpEnveLop))
		{
			MySleep(300);
			continue;
		}

		Process_rmp_command(pAmqpEnveLop);
		m_pInterRabbitCommuHandler->FreeAmqpEnveloptObj(pAmqpEnveLop);
	}

	return 0;
}

int CRabbitMqCommandMgr::send_msg_to_rmq_loop()
{
	while (!m_bExit)
	{
		if (true == m_bExit)
		{
			break;;
		}

		JSON_SENDMSG json_send_msg;
		if (false == Get_command_result(json_send_msg))
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

bool CRabbitMqCommandMgr::Process_rmp_command(amqp_envelope_t* pAmqpComand)
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
		json_send_msg.nTransMask = m_pMainFlow->GetMsgTransMask();
		json_send_msg.sender_channel = pAmqpComand->channel;
		json_send_msg.sender_info = pAmqpComand->message.properties;
		Add_result_wait_msg(json_send_msg);//加入等待队列

		dfuMsg.nDfuCommandID = nDfuMsgID;
		dfuMsg.nInternalCommandID = jsonMsgparser.GetCommandID();
		dfuMsg.nCommandNum = dfuMsg.command_msg.size();
		dfuMsg.nMsgType = RECORD_DFU_MESSAGE_TYPE_JSON;
		dfuMsg.nTransMask = json_send_msg.nTransMask;
		dfuMsg.bRecvEnd = false;
		m_pMainFlow->PushCommandToDfuApci(dfuMsg);

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

bool CRabbitMqCommandMgr::Get_command_result(JSON_SENDMSG& json_send_msg)
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

void CRabbitMqCommandMgr::Add_result_wait_msg(JSON_SENDMSG& json_send_msg)
{
	CLockUp lockUp(&m_LockResultMsg);

	m_veJsonSendList.push_back(json_send_msg);
}

bool CRabbitMqCommandMgr::Add_rmq_result_list(cJSON* pResultMsg, int nTransMask, int nCommmandID)
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

void CRabbitMqCommandMgr::Add_rmq_command_list(amqp_envelope_t* pAmqpEnvelope)
{
	CLockUp lockUp(&m_LockAmqpRecvMsg);

	m_veAmqpCommand.push_back(pAmqpEnvelope);
}

bool CRabbitMqCommandMgr::Get_rmq_command_from_list(amqp_envelope_t*& pAmqpComand)
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

bool CRabbitMqCommandMgr::InitLogFile()
{
	m_LogFile.Close();

	m_LogFile.SetLogPath(m_pConfigHandle->GetSysParam_LogPath());
	m_LogFile.SetLogLevel(m_pConfigHandle->GetSysParam_LogLevel());
	m_LogFile.SetLogSaveDays(m_pConfigHandle->GetSysParam_LogDays());

	return (TRUE == m_LogFile.Open("internal_commu_log"))?true:false;
}
