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
	bool InitCommandMonitorHandler();


private:
/**	\brief ����ά�������*/
	CConfigVariableMgr* m_pConfigVariableObj;

	CRabbitmqAccess* m_pInterRabbitCommuHandler;
};

#endif
