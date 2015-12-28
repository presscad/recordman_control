#if !defined(RECORD_RECORD_DATA_COLLECTOR_INCLUDE)
#define RECORD_RECORD_DATA_COLLECTOR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "const_define.h"
#include "ConfigVariableMgr.h"
#include "DfuCommuSession.h"

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
	bool InitDfuCommuSession();

private:
/**	\brief ����ά�������*/
	CConfigVariableMgr* m_pConfigvarialemgr;

/**	\brief dfuͨѶ�����*/
	CDfuCommuSession* m_pDfuCommuSession;
};

#endif
