#if !defined(RECORD_DFU_COMMU_SESSION_INCLUDE)
#define RECORD_DFU_COMMU_SESSION_INCLUDE

#include "ConfigVariableMgr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CDfuCommuSession
{
public:
	CDfuCommuSession(void);
	~CDfuCommuSession(void);

public:
	//���ò������ʶ���ָ��
	void SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam);

	//��ʼ��
	//true���ɹ� false��ʧ��
	bool InitDfuCommuSession();

	//����
	//true���ɹ� false��ʧ��
	bool StartCommuSession();

	//ֹͣ
	//true���ɹ� false��ʧ��
	bool StopCommuSession();

public:
	//��dfuͨѶ���̺߳���
	//0���߳��˳�
	int DfuCommuOperationLoop();

private:
	//��socket��ȡ����
	//��ȡ�����ֽ���
	int ReceiveMsg(RECORD_DFU_MSG* pMsg);

	//���ͱ���
	int WriteRecordMsg(RECORD_DFU_MSG* pMsg);

	//��ӡ���
	void LogMessage(const RECORD_DFU_MSG* pMsg, const LOG_BUFFER_HEAD& pHead);

	//���������ַ�
	void CopyMessageToString(BYTE bMsg, char*& pChar);

private:
/**	\brief ���ò�������*/
	COLLECTOR_DATA_SYS_PARAM* m_pCollectorSysParam;

private:
/**	\brief �˳���־*/
	bool m_bExitFlag;

/**	\brief ��dfuҵ�����߳�*/
	CRecordmanThread m_DfuOperationThread;

/**	\brief net����*/
	CNet* m_pNetSocket;
};

#endif
