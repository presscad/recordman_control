#include "RecordDataCollector.h"


CRecordDataCollector::CRecordDataCollector(void)
{
	m_pConfigvarialemgr = NULL;
	m_pRecordApciHandler = NULL;
	m_pCommandHandlerMgr = NULL;
	m_pCommandMonitorHandler = NULL;
}


CRecordDataCollector::~CRecordDataCollector(void)
{
}

//************************************
// Method:    InitRecordDataColletor
// FullName:  CRecordDataCollector::InitRecordDataColletor
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 必要的初始化
//************************************
bool CRecordDataCollector::InitRecordDataColletor()
{
	try
	{
		if (false == InitSysConfigVariable())
		{
			return false;
		}

		if (false == InitApciHandler())
		{
			return false;
		}

		if (false == InitCommandMonitorHandler())
		{
			return false;
		}
	}
	catch (...)
	{
		printf("[InitRecordDataColletor]init record dataCollector find exception！\n");
		return false;
	}

	return true;
}

//************************************
// Method:    StartRecordDataColletor
// FullName:  CRecordDataCollector::StartRecordDataColletor
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 启动
//************************************
bool CRecordDataCollector::StartRecordDataColletor()
{
	try
	{
		if (false == m_pRecordApciHandler->StartRecordApciHandler())
		{
			return false;
		}
	}
	catch (...)
	{
		printf("[StartRecordDataColletor]start record dataCollector find exception！\n");
		return false;
	}
	return true;
}

//************************************
// Method:    EndRecordDataColletor
// FullName:  CRecordDataCollector::EndRecordDataColletor
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 停止
//************************************
bool CRecordDataCollector::EndRecordDataColletor()
{
	try
	{
		if (NULL != m_pRecordApciHandler)
		{
			m_pRecordApciHandler->StopRecordApciHandler();
		}
	}
	catch (...)
	{
		printf("[EndRecordDataColletor]end record dataCollector find exception！\n");
		return false;
	}
	return true;
}

//************************************
// Method:    ExitRecordDataColletor
// FullName:  CRecordDataCollector::ExitRecordDataColletor
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 退出
//************************************
bool CRecordDataCollector::ExitRecordDataColletor()
{
	try
	{
		if (NULL != m_pRecordApciHandler)
		{
			delete m_pRecordApciHandler;
			m_pRecordApciHandler = NULL;
		}

		if (NULL != m_pConfigvarialemgr)
		{
			delete m_pConfigvarialemgr;
			m_pConfigvarialemgr = NULL;
		}
	}
	catch (...)
	{
		printf("[ExitRecordDataColletor]exit record dataCollector find exception！\n");
		return false;
	}
	return true;
}

//************************************
// Method:    InitSysConfigVariable
// FullName:  CRecordDataCollector::InitSysConfigVariable
// Access:    private 
// Returns:   bool true：成功 false：失败
// Qualifier: 读取配置参数
//************************************
bool CRecordDataCollector::InitSysConfigVariable()
{
	try
	{
		if (NULL == m_pConfigvarialemgr)
		{
			m_pConfigvarialemgr = new CConfigVariableMgr;
		}

		if (NULL == m_pConfigvarialemgr)
		{
			printf("new class CConfigVariableMgr failed！\n");
			return false;
		}

		if (false == m_pConfigvarialemgr->InitCollectorSysparam())
		{
			return false;
		}

		if (false == m_pConfigvarialemgr->LoadCollectorSysParam())
		{
			return false;
		}
	}
	catch (...)
	{
		printf("[InitSysConfigVariable]init collector system param find exception！\n");
		return false;
	}

	return true;
}

//************************************
// Method:    InitApciHandler
// FullName:  CRecordDataCollector::InitApciHandler
// Access:    private 
// Returns:   bool
// Qualifier:
//************************************
bool CRecordDataCollector::InitApciHandler()
{
	try
	{
		if (NULL == m_pRecordApciHandler)
		{
			m_pRecordApciHandler = new CRecordAPCIHandler;
		}

		if (NULL == m_pRecordApciHandler)
		{
			printf("new class CRecordAPCIHandler failed！\n");
			return false;
		}

		m_pRecordApciHandler->SetCollectorSysParam(&m_pConfigvarialemgr->m_collector_sys_param);

		if (false == m_pRecordApciHandler->InitRecordApciHandler())
		{
			return false;
		}

		printf("InitDfuCommuSession succeed！\n");
	}
	catch (...)
	{
		printf("[InitApciHandler]init apci handler find exception！\n");
		return false;
	}
	
	return true;
}

bool CRecordDataCollector::InitCommandMonitorHandler()
{
	try
	{
		if (NULL == m_pCommandMonitorHandler)
		{
			m_pCommandMonitorHandler = new CCommandMonitorHandler;
		}

		if (NULL == m_pCommandMonitorHandler)
		{
			printf("new class CCommandMonitorHandler failed！\n");
			return false;
		}

		m_pCommandMonitorHandler->SetConfigVariableHandle(m_pConfigvarialemgr);

		if (false == m_pCommandMonitorHandler->InitCommandMonitorHandler())
		{
			return false;
		}

		printf("InitCommandMonitorHandler succeed！\n");
	}
	catch (...)
	{
		printf("[InitCommandMonitorHandler]init command monitor find exception！\n");
		return false;
	}

	return true;
}

//************************************
// Method:    InitCommandHandlerMgr
// FullName:  CRecordDataCollector::InitCommandHandlerMgr
// Access:    private 
// Returns:   bool true：success false：fail
// Qualifier: 
//************************************
bool CRecordDataCollector::InitCommandHandlerMgr()
{
	try
	{
		if (NULL == m_pCommandHandlerMgr)
		{
			m_pCommandHandlerMgr = new CCommandHandlerMgr;
		}

		if (NULL == m_pCommandHandlerMgr)
		{
			printf("new class CCommandHandlerMgr failed！\n");
			return false;
		}
	}
	catch (...)
	{
		printf("[InitCommandHandlerMgr]init command handler manager find exception！\n");
		return false;
	}
	
	return true;
}
