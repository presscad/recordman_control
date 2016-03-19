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
	//process command
	int ProcessCommand(RECORD_DFU_MSG* pCommandMsg, vector<RECORD_DFU_MSG*> veResultMsg);
	
	//read msg from socket
	//return readed msg bytes num
	int ReceiveMsg(RECORD_DFU_MSG* pMsg);

	//send msg
	int WriteRecordMsg(RECORD_DFU_MSG* pMsg);

	//create poling command
	void CreatePolingCommand(RECORD_DFU_MSG* pCommandMsg);

	//query new osc file from dfu
	void CreateQueryNewOscCommand(RECORD_DFU_MSG* pCommandMsg);

	void CreateGetOscFileCommand(RECORD_DFU_MSG* pCommandMsg, int nFileIndex);

private:
	//print msg
	void LogMessage(const RECORD_DFU_MSG* pMsg, const LOG_BUFFER_HEAD& pHead);

	//copy string
	void CopyMessageToString(BYTE bMsg, char*& pChar);

	//get msg trans mask
	UINT GetMsgTransMask();

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

private:
	//msg process lock
	CSafeLock m_LockApciMsgHandler;

	//link last active time
	time_t m_tLinkActive;

	//transmask
	UINT m_utTransMask;
};

#endif

