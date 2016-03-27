#if !defined(RECORD_INTERNAL_COMMU_MGR_INCLUDE)
#define RECORD_INTERNAL_COMMU_MGR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordAPCIHandler.h"
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

class CInternalCommuMgr
{
public:
	CInternalCommuMgr(void);
	~CInternalCommuMgr(void);

public:
	//set rabbit access basic param
	void SetRabbitmqAccessParam(COLLECTOR_ADVANCE_RABBITMQ_PARAM* pObj);

	void SetSystemParam(COLLECTOR_DATA_SYS_PARAM* pSystemParam);

	void SetRecordApciHandler(CRecordAPCIHandler* pApciHandler);

public:
	void AddAmqpCommand(amqp_envelope_t* pAmqpEnvelope);

	void AddJsonWaitResultMsg(JSON_SENDMSG& json_send_msg);

	bool AddJsonResultMsg(cJSON* pResultMsg, int nTransMask, int nCommmandID);

	int AmqpCommandOperationLoop();

	int JsonResultSendLoop();

public:
	bool InitCommandMonitorHandler();

	bool StartCommandMonitorHandler();

	bool StopCommandMonitorHandler();

private:
	bool InitLogFile();

	bool GetAmqpCommand(amqp_envelope_t*& pAmqpComand);

	bool ProcessAmqpCommand(amqp_envelope_t* pAmqpComand);

	bool GetJsonResultMsg(JSON_SENDMSG& json_send_msg);

private:
/**	\brief 配置维护类对象*/
	COLLECTOR_ADVANCE_RABBITMQ_PARAM* m_pRabbitmqParm;

	CRabbitmqAccess* m_pInterRabbitCommuHandler;

	CRecordAPCIHandler* m_pRecordApciHandler;

	COLLECTOR_DATA_SYS_PARAM* m_pSystemParm;

	CSafeLock m_LockAmqpRecvMsg;

	CSafeLock m_LockResultMsg;

	CRecordmanThread m_IdlerMqCommandThread;

	CRecordmanThread m_SendJsonResultThread;

	CMessageLog m_LogFile;

private:
	bool m_bExit;

	vector<amqp_envelope_t*> m_veAmqpCommand;

	vector<JSON_SENDMSG> m_veJsonSendList;
};

#endif
