#if !defined(RECORD_RABBIT_MQ_ACCESS_INCLUDE)
#define RECORD_RABBIT_MQ_ACCESS_INCLUDE

#include "sys_define.h"
#include "../common_open_source/rabbitmq/include/amqp.h"
#include "../common_open_source/rabbitmq/include/amqp_framing.h"
#include "../common_open_source/rabbitmq/include/amqp_tcp_socket.h"
#include "../common_open_source/rabbitmq/include/amqp_ssl_socket.h"
#include "RecordmanThread.h"
#include "../common_open_source/cJSON/cJSON.h"
#include "SafeLock.h"

typedef int (*PAMQPRECVFUNCALLBACK)(amqp_envelope_t* pAmqp_envelope_t, XJHANDLE pReserved);

class CRabbitmqAccess
{
public:
	CRabbitmqAccess(void);
	~CRabbitmqAccess(void);

public:
	//设置配置类访问句柄
	void SetRabbitAccessParam(RABBIT_MQ_BASIC_ACCESS_PARAM* pObj);

	void RegisterRecvHandler(PAMQPRECVFUNCALLBACK pRecvFun, XJHANDLE pReserved);

public:
	bool ConnectRabbitMqServer();

	bool CloseRabbitMqConn();

	bool StartAmqpRecv(RABBIT_RECV_PARAM& rabbitmq_recv_param);

	bool StopAmqpRecv();

	bool SendMsg(cJSON* pSendJsonContent, amqp_basic_properties_t& send_basic_properties, amqp_channel_t ampq_channel);

	bool FreeAmqpEnveloptObj(amqp_envelope_t* pAmqpEnveloptObj);

public:
	int RecvAmqpMsgLoop();

private:
	bool LoginAndOpenChannel();

	bool SetAmqpQos(short nPrefetchCount);

	bool find_operation_server_error(int nErrorNo, const char* pContext);

	bool find_amqp_error(amqp_rpc_reply_t replay, const char* pContext);

private:
	/**	\brief rabbitmq 基本访问参数*/
	RABBIT_MQ_BASIC_ACCESS_PARAM* m_pRabbitBasicParam;

private:
	/**	\brief socket handle*/
	int m_nSockfd;

	bool m_bExit;

	CSafeLock m_LockRabbitmqSend;

	/**	\brief 连接句柄*/
	amqp_connection_state_t m_pRabbitMqConn;

	amqp_socket_t* m_pRabbitMqSocketHandler;

	CRecordmanThread m_MsgRecvThread;

private:
	PAMQPRECVFUNCALLBACK m_pAmqpRecvFun;
	
	XJHANDLE m_pAmqpRecvReserved;
};

#endif
