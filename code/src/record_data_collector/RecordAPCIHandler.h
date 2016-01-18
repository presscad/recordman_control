#if !defined(RECORD_APCI_HANDLER_INCLUDE)
#define RECORD_APCI_HANDLER_INCLUDE

#include "ConfigVariableMgr.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRecordAPCIHandler
{
public:
	CRecordAPCIHandler(void);
	~CRecordAPCIHandler(void);

public:
	//���ò������ʶ���ָ��
	void SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam);

	//��ʼ��
	//true���ɹ� false��ʧ��
	bool InitRecordApciHandler();

	//����
	//true���ɹ� false��ʧ��
	bool StartRecordApciHandler();

	//ֹͣ
	//true���ɹ� false��ʧ��
	bool StopRecordApciHandler();

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

