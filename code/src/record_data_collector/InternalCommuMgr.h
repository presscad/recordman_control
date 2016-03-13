#if !defined(RECORD_INTERNAL_COMMU_MGR_INCLUDE)
#define RECORD_INTERNAL_COMMU_MGR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "../../common/RabbitmqAccess.h"

class CInternalCommuMgr
{
public:
	CInternalCommuMgr(void);
	~CInternalCommuMgr(void);

public:
	//������������ʾ��
	void SetConfigVariableHandle(CConfigVariableMgr* pObj);

public:
	void AddAmqpCommand(amqp_envelope_t* pAmqpEnvelope);

	int AmqpCommandOperationLoop();

public:
	bool InitCommandMonitorHandler();

	bool StartCommandMonitorHandler();

	bool StopCommandMonitorHandler();

private:
	bool GetAmqpCommand(amqp_envelope_t* pAmqpComand);

private:
/**	\brief ����ά�������*/
	CConfigVariableMgr* m_pConfigVariableObj;

	CRabbitmqAccess* m_pInterRabbitCommuHandler;

	CSafeLock m_LockAmqpRecvMsg;

	CRecordmanThread m_IdlerMqCommandThread;

private:
	bool m_bExit;

	vector<amqp_envelope_t*> m_veAmqpCommand;
};

#endif
