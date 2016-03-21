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

	int AmqpCommandOperationLoop();

public:
	bool InitCommandMonitorHandler();

	bool StartCommandMonitorHandler();

	bool StopCommandMonitorHandler();

private:
	bool InitLogFile();

	bool GetAmqpCommand(amqp_envelope_t*& pAmqpComand);

	bool ProcessAmqpCommand(amqp_envelope_t* pAmqpComand);

private:
/**	\brief 配置维护类对象*/
	COLLECTOR_ADVANCE_RABBITMQ_PARAM* m_pRabbitmqParm;

	CRabbitmqAccess* m_pInterRabbitCommuHandler;

	CRecordAPCIHandler* m_pRecordApciHandler;

	COLLECTOR_DATA_SYS_PARAM* m_pSystemParm;

	CSafeLock m_LockAmqpRecvMsg;

	CRecordmanThread m_IdlerMqCommandThread;

	CMessageLog m_LogFile;

private:
	bool m_bExit;

	vector<amqp_envelope_t*> m_veAmqpCommand;
};

#endif
