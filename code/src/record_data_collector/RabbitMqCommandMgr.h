#if !defined(RECORD_INTERNAL_COMMU_MGR_INCLUDE)
#define RECORD_INTERNAL_COMMU_MGR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DfuMainFlow.h"
#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "../../common/RabbitmqAccess.h"
#include "../../common/MessageLog.h"

//send msg struct
typedef struct _JSON_SENDMSG
{
	bool bEnd;

	int nTransMask;

	int nCommandID;

	cJSON* pJsonMsg;

	amqp_basic_properties_t sender_info;

	amqp_channel_t sender_channel;

	_JSON_SENDMSG()
	{
		bEnd = false;
		nTransMask = -1;
		nCommandID = -1;
		pJsonMsg = NULL;
	}

}JSON_SENDMSG;

class CRabbitMqCommandMgr
{
public:
	CRabbitMqCommandMgr(void);
	~CRabbitMqCommandMgr(void);

public:
	void SetConfigVariableMgrHandle(CConfigVariableMgr* pConfigHandle);

	void SetDfuMainFlowHandler(CDfuMainFlow* pMainFlowHandler);

public:
	void Add_rmq_command_list(amqp_envelope_t* pAmqpEnvelope);

	void Add_result_wait_msg(JSON_SENDMSG& json_send_msg);

	bool Add_rmq_result_list(cJSON* pResultMsg, int nTransMask, int nCommmandID);

	int rmq_command_operation_loop();

	int send_msg_to_rmq_loop();

public:
	bool InitCommandMonitorHandler();

	bool StartCommandMonitorHandler();

	bool StopCommandMonitorHandler();

private:
	bool InitLogFile();

	bool Get_rmq_command_from_list(amqp_envelope_t*& pAmqpComand);

	bool Process_rmp_command(amqp_envelope_t* pAmqpComand);

	bool Get_command_result(JSON_SENDMSG& json_send_msg);

private:
	CConfigVariableMgr* m_pConfigHandle;

	CRabbitmqAccess* m_pInterRabbitCommuHandler;

	CDfuMainFlow* m_pMainFlow;

	CSafeLock m_LockAmqpRecvMsg;

	CSafeLock m_LockResultMsg;

	CRecordmanThread m_AmqpCommandProcessThread;

	CRecordmanThread m_AmqpMsgSendThread;

	CMessageLog m_LogFile;

private:
	bool m_bExit;

	vector<amqp_envelope_t*> m_veAmqpCommand;

	vector<JSON_SENDMSG> m_veJsonSendList;
};

#endif
