#include "RabbitmqAccess.h"

THREAD_FUNC WINAPI AmqpMsgRecvProc(LPVOID pParam)
{
	CRabbitmqAccess* pRabbitmqAccess = (CRabbitmqAccess*)pParam;
	if (NULL == pParam)
	{
		return THREAD_RETURN;
	}

	try
	{
		pRabbitmqAccess->RecvAmqpMsgLoop();
	}
	catch (...)
	{
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}


CRabbitmqAccess::CRabbitmqAccess(void)
{
	m_pRabbitAccessParam = NULL;
	m_pRabbitMqConn = NULL;
	m_pRabbitMqSocketHandler = NULL;
	m_pAmqpRecvFun = NULL;
	m_pAmqpRecvReserved = NULL;
	m_nSockfd = -1;
	m_nChannelid = -1;
	m_bExit = true;
}


CRabbitmqAccess::~CRabbitmqAccess(void)
{
	CloseRabbitMqConn();
}

void CRabbitmqAccess::SetRabbitAccessParam(RABBIT_MQ_ACCESS_PARAM* pObj)
{
	m_pRabbitAccessParam = pObj;
}

void CRabbitmqAccess::RegisterRecvHandler(PAMQPRECVFUNCALLBACK pRecvFun, void* pReserved)
{
	m_pAmqpRecvFun = pRecvFun;
	m_pAmqpRecvReserved = pReserved;
}

//************************************
// 函 数 名： ConnectRabbitMqServer
// 功能概要： connnct rabbitmq server
// 访问权限： public 
// 返 回 值： bool true:succeed false:failed
//************************************
bool CRabbitmqAccess::ConnectRabbitMqServer(int nchannelid)
{
	bool bRet = true;

	try
	{
		m_nChannelid = nchannelid;

		m_pRabbitMqConn = amqp_new_connection();
		if (NULL == m_pRabbitMqConn)
		{
			return false;
		}

		m_pRabbitMqSocketHandler = amqp_tcp_socket_new(m_pRabbitMqConn);
		if (NULL == m_pRabbitMqSocketHandler)
		{
			return false;
		}

		m_nSockfd = amqp_socket_open(
			m_pRabbitMqSocketHandler, 
			m_pRabbitAccessParam->chhostname, 
			m_pRabbitAccessParam->nserver_port);

		if (true == find_operation_server_error(m_nSockfd, "Opening socket"))
		{
			bRet = false;
		}
		else
		{
			bRet = LoginAndOpenChannel(nchannelid);
		}
	}
	catch (...)
	{
		bRet = false;
	}

	if (false == bRet)
	{
		CloseRabbitMqConn();
	}

	return bRet;
}

bool CRabbitmqAccess::CloseRabbitMqConn()
{
	if (NULL == m_pRabbitMqConn)
	{
		return true;
	}

	find_amqp_error(amqp_channel_close(m_pRabbitMqConn, m_nChannelid, AMQP_REPLY_SUCCESS), "Closing channel");
	find_amqp_error(amqp_connection_close(m_pRabbitMqConn, AMQP_REPLY_SUCCESS), "Closing connection");
	find_operation_server_error(amqp_destroy_connection(m_pRabbitMqConn), "Ending connection");

	m_pRabbitMqConn = NULL;

	return true;
}

bool CRabbitmqAccess::SetAmqpQos(short nPrefetchCount)
{
	if (NULL == m_pRabbitMqConn)
	{
		return false;
	}

	amqp_basic_qos(m_pRabbitMqConn, m_nChannelid, 0, nPrefetchCount, false);

	return true;
}

bool CRabbitmqAccess::StartAmqpRecv(string strQueue /*= "queue.test"*/)
{
	SetAmqpQos(1000);//set qos
	amqp_basic_consume(
		m_pRabbitMqConn, m_nChannelid, 
		amqp_cstring_bytes(strQueue.c_str()), amqp_empty_bytes, 
		0, false, 0, amqp_empty_table);
	amqp_get_rpc_reply(m_pRabbitMqConn);

	m_bExit = false;

	m_MsgRecvThread.Stop();
	if (!m_MsgRecvThread.Start(AmqpMsgRecvProc, this))
	{
		return false;
	}

	return true;
}

bool CRabbitmqAccess::StopAmqpRecv()
{
	m_bExit = true;
	m_MsgRecvThread.Stop();

	return true;
}

bool CRabbitmqAccess::SendMsg(cJSON* pSendJsonContent, amqp_basic_properties_t& send_basic_properties)
{
	if (NULL == pSendJsonContent)
	{
		return false;
	}

	char* pSendMsg = cJSON_Print(pSendJsonContent);
	if (NULL == pSendMsg)
	{
		return false;
	}

	amqp_basic_properties_t props;
	props = send_basic_properties;

	amqp_basic_publish(
		m_pRabbitMqConn, 
		m_nChannelid, 
		amqp_cstring_bytes(""), 
		amqp_cstring_bytes("WEB_RECEIVE_RESULT_QUEUE"), 
		0, 0, &props, amqp_cstring_bytes(pSendMsg));

	return true;
}

bool CRabbitmqAccess::FreeAmqpEnveloptObj(amqp_envelope_t* pAmqpEnveloptObj)
{
	if (NULL == pAmqpEnveloptObj)
	{
		return false;
	}

	amqp_destroy_envelope(pAmqpEnveloptObj);

	if (NULL != pAmqpEnveloptObj)
	{
		delete pAmqpEnveloptObj;
		pAmqpEnveloptObj = NULL;
	}

	return true;
}

bool CRabbitmqAccess::LoginAndOpenChannel(int nchannelid)
{
	if (NULL == m_pRabbitMqConn)
	{
		return false;
	}

	bool bRet = true;

	try
	{
		amqp_rpc_reply_t login_replay;
		login_replay = amqp_login(
			m_pRabbitMqConn, 
			RABBIT_MQ_DEFAULT_V_HOST, 
			RABBIT_MQ_DEFAULT_CHANNEL_MAX, 
			RABBIT_MQ_DEFAULT_FRAME_MAX, 
			0, 
			AMQP_SASL_METHOD_PLAIN, 
			m_pRabbitAccessParam->chusrname, 
			m_pRabbitAccessParam->chpassword);

		if (true == find_amqp_error(login_replay, "Logging in"))
		{
			return false;
		}

		amqp_rpc_reply_t open_channel_replay;
		amqp_channel_open(m_pRabbitMqConn, m_nChannelid);
		open_channel_replay = amqp_get_rpc_reply(m_pRabbitMqConn);
		if (true == find_amqp_error(open_channel_replay, "Opening channel"))
		{
			bRet = false;
		}
	}
	catch (...)
	{
		bRet = false;
	}
	
	return bRet;
}

int CRabbitmqAccess::RecvAmqpMsgLoop()
{
	amqp_envelope_t* envelope = NULL;

	while(!m_bExit)
	{
		amqp_rpc_reply_t ret;
		amqp_maybe_release_buffers(m_pRabbitMqConn);
		envelope = new amqp_envelope_t;

		ret = amqp_consume_message(m_pRabbitMqConn, envelope, NULL, 0);
		if(ret.reply_type == AMQP_RESPONSE_NORMAL)
		{
			if (NULL != m_pAmqpRecvFun)
			{
				m_pAmqpRecvFun(envelope, m_pAmqpRecvReserved);
			}
			
			if(amqp_basic_ack(m_pRabbitMqConn, m_nChannelid, envelope->delivery_tag, false) > 0)
			{
				printf("failing to send the ack to the broker...\n");
			}
		}
		else if(ret.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
		{
			printf("amqp_consume_message faild!\n");
		}
	}

	return 0;
}

bool CRabbitmqAccess::find_operation_server_error(int nErrorNo, const char* pContext)
{
	if (nErrorNo < 0)
	{
		const char* pErrorMsg = amqp_error_string2(-nErrorNo);
		if (NULL != pErrorMsg)
		{
			printf("%s：%s.\n", 
				pContext, pErrorMsg);
		}

		return true;
	}

	return false;
}

bool CRabbitmqAccess::find_amqp_error(amqp_rpc_reply_t replay, const char* pContext)
{
	switch (replay.reply_type) {
	case AMQP_RESPONSE_NORMAL:
		return false;

	case AMQP_RESPONSE_NONE:
		printf("%s: missing RPC reply type!\n", pContext);
		break;

	case AMQP_RESPONSE_LIBRARY_EXCEPTION:
		printf("%s: %s\n", pContext, amqp_error_string2(replay.library_error));
		break;

	case AMQP_RESPONSE_SERVER_EXCEPTION:
		switch (replay.reply.id) {
		case AMQP_CONNECTION_CLOSE_METHOD: {
			amqp_connection_close_t *m = (amqp_connection_close_t *) replay.reply.decoded;
			printf("%s: server connection error %d, message: %.*s\n", 
				pContext, m->reply_code, (int)m->reply_text.len, (char *)m->reply_text.bytes);
			break;
										   }
		case AMQP_CHANNEL_CLOSE_METHOD: {
			amqp_channel_close_t *m = (amqp_channel_close_t *)replay.reply.decoded;
			printf("%s: server channel error %d, message: %.*s\n", 
				pContext, m->reply_code, (int)m->reply_text.len, (char *)m->reply_text.bytes);
			break;
										}
		default:
			printf("%s: unknown server error, method id 0x%08X\n", pContext, replay.reply.id);
			break;
		}
		break;
	}

	return true;
}
