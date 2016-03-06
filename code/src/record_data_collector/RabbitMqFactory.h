#if !defined(RECORD_RABBIT_MQ_FACTORY_INCLUDE)
#define RECORD_RABBIT_MQ_FACTORY_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "../../common_open_source/rabbitmq/include/amqp.h"
#include "../../common_open_source/rabbitmq/include/amqp_framing.h"

class CRabbitMqFactory
{
public:
	CRabbitMqFactory(void);
	~CRabbitMqFactory(void);

public:
	//设置配置类访问句柄
	void SetConfigVariableHandle(CConfigVariableMgr* pObj);

public:
	bool ConnectRabbitMqServer(int nchannelid);

private:

	bool LoginAndOpenChannel(int nchannelid);

	bool find_operation_server_error(int nErrorNo, const char* pContext);

	bool find_amqp_error(amqp_rpc_reply_t replay, const char* pContext);

private:
	/**	\brief 配置维护类对象*/
	CConfigVariableMgr* m_pConfigVariableObj;

private:
	/**	\brief socket handle*/
	int m_nSockfd;

	/**	\brief channel id*/
	int m_nChannelid;

	/**	\brief 连接句柄*/
	amqp_connection_state_t m_pRabbitMqConn;
};

#endif
