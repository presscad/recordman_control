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
	//设置配置类访问句柄
	void SetConfigVariableHandle(CConfigVariableMgr* pObj);

public:
	bool InitCommandMonitorHandler();


private:
/**	\brief 配置维护类对象*/
	CConfigVariableMgr* m_pConfigVariableObj;

	CRabbitmqAccess* m_pInterRabbitCommuHandler;
};

#endif
