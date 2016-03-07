#if !defined(RECORD_RABBIT_MQ_ACCESS_INCLUDE)
#define RECORD_RABBIT_MQ_ACCESS_INCLUDE

#include "sys_define.h"
#include "../common_open_source/rabbitmq/include/amqp.h"
#include "../common_open_source/rabbitmq/include/amqp_framing.h"
#include "../common_open_source/rabbitmq/include/amqp_tcp_socket.h"
#include "../common_open_source/rabbitmq/include/amqp_ssl_socket.h"

class CRabbitmqAccess
{
public:
	CRabbitmqAccess(void);
	~CRabbitmqAccess(void);

public:
	//设置配置类访问句柄
	void SetRabbitAccessParam(RABBIT_MQ_ACCESS_PARAM* pObj);

public:
	bool ConnectRabbitMqServer(int nchannelid);

	bool CloseRabbitMqConn();

private:

	bool LoginAndOpenChannel(int nchannelid);

	bool find_operation_server_error(int nErrorNo, const char* pContext);

	bool find_amqp_error(amqp_rpc_reply_t replay, const char* pContext);

private:
	/**	\brief rabbit访问参数*/
	RABBIT_MQ_ACCESS_PARAM* m_pRabbitAccessParam;

private:
	/**	\brief socket handle*/
	int m_nSockfd;

	/**	\brief channel id*/
	int m_nChannelid;

	/**	\brief 连接句柄*/
	amqp_connection_state_t m_pRabbitMqConn;

	amqp_socket_t* m_pRabbitMqSocketHandler;
};

#endif
