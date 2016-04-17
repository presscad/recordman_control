#if !defined(RECORD_RECORD_DATA_COLLECTOR_INCLUDE)
#define RECORD_RECORD_DATA_COLLECTOR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../common/MongodbAccess.h"
#include "DfuMainFlow.h"
#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "CommandHandlerMgr.h"
#include "InternalCommuMgr.h"

class CRecordDataCollector
{
public:
	CRecordDataCollector(void);
	~CRecordDataCollector(void);

public:
	//init
	//true��success false��failed
	bool InitRecordDataColletor();

	//start
	//true��success false��failed
	bool StartRecordDataColletor();

	//stop
	//true��success false��failed
	bool EndRecordDataColletor();

	//exit
	//true��success false��failed
	bool ExitRecordDataColletor();

private:
	//init log file
	bool InitLogFile();

	//load config param
	//true��success false��failed
	bool InitSysConfigVariable();

	//init mongodb access handler
	//true:success false:failed
	bool InitMongoDbAccessHandler();
	
	//��ʼ��dfuͨѶ�����
	//true���ɹ� false��ʧ��
	bool InitDfuMainFlowHandler();

	//init internalcommu mgr
	bool InitInternalCommuMgr();

	//��ʼ�������Ự���������
	//true���ɹ� false��ʧ��
	bool InitCommandHandlerMgr();

private:
/**	\brief ����ά�������*/
	CConfigVariableMgr* m_pConfigvarialemgr;

/**	\brief ����������*/
	CCommandHandlerMgr* m_pCommandHandlerMgr;

/**	\brief ��dfuͨ�Ź�����*/
	CDfuMainFlow* m_pDfuMainFlow;

/**	\brief ϵͳ�ڲ���Ϣ����*/
	CInternalCommuMgr* m_pInternalCommuMgr;

	CMongodbAccess* m_pMongodbAccessHandler;

public:
/**	\brief logfile*/
	CMessageLog m_Log;

private:
	//log level
	int m_nLogLevel;

	//log days
	int m_nLogDays;

	//log path
	char m_chLogpath[MAX_LINE_LENGTH];
};

#endif
