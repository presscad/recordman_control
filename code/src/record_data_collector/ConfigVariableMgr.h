#if !defined(RECORD_CONFIG_VARIABLE_MGR_INCLUDE)
#define RECORD_CONFIG_VARIABLE_MGR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "const_define.h"

class CConfigVariableMgr
{
public:
	CConfigVariableMgr(void);
	~CConfigVariableMgr(void);

public:
/**	\brief ϵͳ����*/
	COLLECTOR_DATA_SYS_PARAM m_collector_sys_param;

public:
	//��ʼ������
	//true���ɹ� false��ʧ��
	bool InitCollectorSysparam();

	//�ӿ��ж�ȡ����
	//true���ɹ� false��ʧ��
	bool LoadCollectorSysParam();
};

#endif
