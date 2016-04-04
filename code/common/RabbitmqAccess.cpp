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


CRabbitmqAccess::CRabbitmqAccess(void):
m_LockRabbitmqSend("LOCK_RABBITMQ_SEND")
{
	m_pRabbitBasicParam = NULL;
	m_pRabbitMqConn = NULL;
	m_pRabbitMqSocketHandler = NULL;
	m_pAmqpRecvFun = NULL;
	m_pAmqpRecvReserved = NULL;
	m_nSockfd = -1;
	m_bExit = true;
}


CRabbitmqAccess::~CRabbitmqAccess(void)
{
	CloseRabbitMqConn();
}

void CRabbitmqAccess::SetRabbitAccessParam(RABBIT_MQ_BASIC_ACCESS_PARAM* pObj)
{
	m_pRabbitBasicParam = pObj;
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
bool CRabbitmqAccess::ConnectRabbitMqServer()
{
	bool bRet = true;

	try
	{
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
			m_pRabbitBasicParam->chhostname, 
			m_pRabbitBasicParam->nserver_port);

		if (true == find_operation_server_error(m_nSockfd, "Opening socket"))
		{
			bRet = false;
		}
		else
		{
			bRet = LoginAndOpenChannel();
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

	find_amqp_error(
		amqp_channel_close(m_pRabbitMqConn, m_pRabbitBasicParam->nChannelID, AMQP_REPLY_SUCCESS), "Closing channel");
	find_amqp_error(
		amqp_connection_close(m_pRabbitMqConn, AMQP_REPLY_SUCCESS), "Closing connection");
	find_operation_server_error(
		amqp_destroy_connection(m_pRabbitMqConn), "Ending connection");

	m_pRabbitMqConn = NULL;

	return true;
}

bool CRabbitmqAccess::SetAmqpQos(short nPrefetchCount)
{
	if (NULL == m_pRabbitMqConn)
	{
		return false;
	}

	amqp_basic_qos(
		m_pRabbitMqConn, m_pRabbitBasicParam->nChannelID, 0, nPrefetchCount, false);

	return true;
}

bool CRabbitmqAccess::StartAmqpRecv(RABBIT_RECV_PARAM& rabbitmq_recv_param)
{

	amqp_queue_declare(
		m_pRabbitMqConn, 
		rabbitmq_recv_param.channelid, 
		amqp_cstring_bytes(rabbitmq_recv_param.chQueueName), 
		rabbitmq_recv_param.passive, 
		rabbitmq_recv_param.durable, 
		rabbitmq_recv_param.exclusive, 
		rabbitmq_recv_param.autoDelete, 
		amqp_empty_table
		);

	if (true == find_amqp_error(amqp_get_rpc_reply(m_pRabbitMqConn), "Declaring queue"))
	{
		return false;
	}

	/*
	amqp_queue_bind(
		m_pRabbitMqConn, 
		m_nChannelid, 
		amqp_cstring_bytes(strQueue.c_str()), 
		amqp_cstring_bytes(""), 
		amqp_cstring_bytes(""), 
		amqp_empty_table);

	if (true == find_amqp_error(amqp_get_rpc_reply(m_pRabbitMqConn)))
	{
		return true;
	}
	*/

	SetAmqpQos(rabbitmq_recv_param.perfetchcount);//set qos

	amqp_basic_consume(
		m_pRabbitMqConn, 
		rabbitmq_recv_param.channelid, 
		amqp_cstring_bytes(rabbitmq_recv_param.chQueueName), 
		amqp_empty_bytes, 
		0, 
		rabbitmq_recv_param.no_ack, 
		0, 
		amqp_empty_table);
	
	amqp_get_rpc_reply(m_pRabbitMqConn);

	m_bExit = false;

	m_MsgRecvThread.Stop();
	if (!m_MsgRecvThread.Start(AmqpMsgRecvProc, this))
	{
		return false;
	}

	return true;
}

//stop
bool CRabbitmqAccess::StopAmqpRecv()
{
	m_bExit = true;
	//CloseRabbitMqConn();

	m_MsgRecvThread.Stop();

	CloseRabbitMqConn();

	return true;
}

bool CRabbitmqAccess::SendMsg(cJSON* pSendJsonContent, amqp_basic_properties_t& send_basic_properties, amqp_channel_t ampq_channel)
{
	CLockUp lockUp(&m_LockRabbitmqSend);

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

	char* preplay_to = new char[send_basic_properties.reply_to.len + 1];
	bzero(preplay_to, (send_basic_properties.reply_to.len + 1));
	memcpy(preplay_to, send_basic_properties.reply_to.bytes, send_basic_properties.reply_to.len);

	amqp_basic_publish(
		m_pRabbitMqConn, 
		ampq_channel, 
		amqp_cstring_bytes(""), 
		amqp_cstring_bytes(preplay_to), 
		0, 0, &props, amqp_cstring_bytes(pSendMsg));

	if (NULL != preplay_to)
	{
		delete[] preplay_to;
		preplay_to = NULL;
	}

	if (NULL != pSendMsg)
	{
		free(pSendMsg);
		pSendMsg = NULL;
	}

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

bool CRabbitmqAccess::LoginAndOpenChannel()
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
			m_pRabbitBasicParam->chVhost, 
			m_pRabbitBasicParam->nChannelMax, 
			m_pRabbitBasicParam->nFrameMax, 
			m_pRabbitBasicParam->nHeartbeatTime, 
			AMQP_SASL_METHOD_PLAIN, 
			m_pRabbitBasicParam->chusrname, 
			m_pRabbitBasicParam->chpassword);

		if (true == find_amqp_error(login_replay, "Logging in"))
		{
			return false;
		}

		amqp_channel_open(m_pRabbitMqConn, m_pRabbitBasicParam->nChannelID);
		if (true == find_amqp_error(amqp_get_rpc_reply(m_pRabbitMqConn), "Opening channel"))
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
// 	struct timeval tv;
// 	tv.tv_sec = 10;
// 	tv.tv_usec = 0;

	try
	{
		while(!m_bExit)
		{
			amqp_rpc_reply_t ret;
			amqp_maybe_release_buffers(m_pRabbitMqConn);
			envelope = new amqp_envelope_t;
			bzero(envelope, sizeof(amqp_envelope_t));

			ret = amqp_consume_message(m_pRabbitMqConn, envelope, NULL, 0);
			if(ret.reply_type == AMQP_RESPONSE_NORMAL)
			{
				if (NULL != m_pAmqpRecvFun)
				{
					m_pAmqpRecvFun(envelope, m_pAmqpRecvReserved);
				}

				if(amqp_basic_ack(m_pRabbitMqConn, envelope->channel, envelope->delivery_tag, false) > 0)
				{
					printf("failing to send the ack to the broker...\n");
				}
			}
			else if(ret.reply_type == AMQP_RESPONSE_LIBRARY_EXCEPTION)
			{
				FreeAmqpEnveloptObj(envelope);
				printf("amqp_consume_message faild!\n");
			}
		}
	}
	catch (...)
	{
		printf("[RecvAmqpMsgLoop]find unknown exception！\n");
		return -1;
	}

	printf("[RecvAmqpMsgLoop]recv amqp loop exit normal！\n");

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
