#if !defined(RECORD_APCI_HANDLER_INCLUDE)
#define RECORD_APCI_HANDLER_INCLUDE

#include "../../common/MongodbAccess.h"
#include "ConfigVariableMgr.h"
#include "JsonMsgParser.h"
#include "../../common/MessageLog.h"
#include "DfuMsgAttach.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef vector<DFU_COMMU_MSG> full_msg_list;

class CRecordAPCIHandler
{
public:
	CRecordAPCIHandler(void);
	~CRecordAPCIHandler(void);

public:
	//set access param
	void SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam, COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParam);

	//mongodb access param
	void SetMongoAccessParam(RECORD_MONGO_BASIC_PARAM* pMongoParam);

	//��ʼ��
	//true���ɹ� false��ʧ��
	bool InitRecordApciHandler();

	//����
	//true���ɹ� false��ʧ��
	bool StartRecordApciHandler();

	//ֹͣ
	//true���ɹ� false��ʧ��
	bool StopRecordApciHandler();

	//send command and recive result
	bool ProcessJsonCommand(cJSON* pJsonCommand, cJSON*& pJsonResult);

public:
	//��dfuͨѶ���̺߳���
	//0���߳��˳�
	int DfuCommuOperationLoop();

	//save new osc file
	int SaveFileOperationLoop();

private:
	//SendAndRecvMessage multi
	int SendAndRecvMessage(vector<DFU_COMMU_MSG>& veCommandMsg, vector<DFU_COMMU_MSG>& veResultMsg);
	
	//SendAndRecvMessage singel msg
	int SendAndRecvMessage(DFU_COMMU_MSG& commandMsg, vector<DFU_COMMU_MSG>& veResultMsg);

	//read msg from socket
	//return readed msg bytes num
	int ReceiveMsg(DFU_COMMU_MSG& pMsg);

	//send msg
	int SendDfuMessage(DFU_COMMU_MSG& record_msg);

private:
	//init logfile
	bool InitLogFile();

	//create link test command
	bool LaunchLinkTest();

	//query new osc file from dfu
	int LaunchQueryNewFile();

	//get osc file
	void ProcessGetOscFile(int nFileIndex);

private:
	//print msg
	void WriteMsgLog(const DFU_COMMU_MSG& pMsg, const LOG_BUFFER_HEAD& pHead);

	//get msg trans mask
	UINT GetMsgTransMask();

private:
	/**	\brief ���ò�������*/
	COLLECTOR_DATA_SYS_PARAM* m_pCollectorSysParam;

	//dfu commu param
	COLLECTOR_DFU_COMMU_PARAM* m_pDfuCommuParam;

	//mongo param
	RECORD_MONGO_BASIC_PARAM* m_pMongoParam;

	//log handler
	CMessageLog m_LogFile;

private:
	/**	\brief �˳���־*/
	bool m_bExitFlag;

	/**	\brief ��dfuҵ�����߳�*/
	CRecordmanThread m_DfuOperationThread;

	//save file thread
	CRecordmanThread m_DfuFileSaveTherad;

private:
	/**	\brief net����*/
	CNet* m_pNetSocket;

	//mongo access
	CMongodbAccess m_MongoAccessHandler;

	//msg process lock
	CSafeLock m_LockApciMsgHandler;

	//file msg list
	full_msg_list m_file_msg_list;

	//link last active time
	time_t m_tLinkActive;

	//check new file time
	time_t m_tCheckFile;

	//transmask
	UINT m_utTransMask;
};

#endif

