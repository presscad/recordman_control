#include "RecordDataCollector.h"


CRecordDataCollector::CRecordDataCollector(void)
{
	m_pConfigvarialemgr = NULL;
	m_pDfuCommuSession = NULL;
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

		if (false == InitDfuCommuSession())
		{
			return false;
		}
	}
	catch (...)
	{
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
		if (false == m_pDfuCommuSession->StartCommuSession())
		{
			return false;
		}
	}
	catch (...)
	{
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
		if (NULL != m_pDfuCommuSession)
		{
			m_pDfuCommuSession->StopCommuSession();
		}
	}
	catch (...)
	{
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
		if (NULL != m_pDfuCommuSession)
		{
			delete m_pDfuCommuSession;
			m_pDfuCommuSession = NULL;
		}

		if (NULL != m_pConfigvarialemgr)
		{
			delete m_pConfigvarialemgr;
			m_pConfigvarialemgr = NULL;
		}
	}
	catch (...)
	{
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
		return false;
	}

	return true;
}

//************************************
// Method:    InitDfuCommuSession
// FullName:  CRecordDataCollector::InitDfuCommuSession
// Access:    private 
// Returns:   bool
// Qualifier:
//************************************
bool CRecordDataCollector::InitDfuCommuSession()
{
	try
	{
		if (NULL == m_pDfuCommuSession)
		{
			m_pDfuCommuSession = new CDfuCommuSession;
		}

		if (NULL == m_pDfuCommuSession)
		{
			printf("new class CDfuCommuSession failed！\n");
			return false;
		}

		m_pDfuCommuSession->SetCollectorSysParam(&m_pConfigvarialemgr->m_collector_sys_param);

		if (false == m_pDfuCommuSession->InitDfuCommuSession())
		{
			return false;
		}

		printf("InitDfuCommuSession succeed！\n");
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}
