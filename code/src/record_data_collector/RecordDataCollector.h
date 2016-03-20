#if !defined(RECORD_RECORD_DATA_COLLECTOR_INCLUDE)
#define RECORD_RECORD_DATA_COLLECTOR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RecordAPCIHandler.h"
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
	//��ʼ����Ҫ�Ĳ���
	//true���ɹ� false��ʧ��
	bool InitRecordDataColletor();

	//����
	//true���ɹ� false��ʧ��
	bool StartRecordDataColletor();

	//ֹͣ
	//true���ɹ� false��ʧ��
	bool EndRecordDataColletor();

	//�˳�
	//true���ɹ� false��ʧ��
	bool ExitRecordDataColletor();

private:
	//��ȡ���ò���
	//true���ɹ� false��ʧ��
	bool InitSysConfigVariable();
	
	//��ʼ��dfuͨѶ�����
	//true���ɹ� false��ʧ��
	bool InitApciHandler();

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
	CRecordAPCIHandler* m_pRecordApciHandler;

/**	\brief ϵͳ�ڲ���Ϣ����*/
	CInternalCommuMgr* m_pInternalCommuMgr;
};

#endif
