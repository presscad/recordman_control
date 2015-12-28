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
/**	\brief 系统参数*/
	COLLECTOR_DATA_SYS_PARAM m_collector_sys_param;

public:
	//初始化参数
	//true：成功 false：失败
	bool InitCollectorSysparam();

	//从库中读取参数
	//true：成功 false：失败
	bool LoadCollectorSysParam();
};

#endif
