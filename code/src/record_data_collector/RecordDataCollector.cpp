#include "RecordDataCollector.h"


CRecordDataCollector::CRecordDataCollector(void)
{
	m_pConfigvarialemgr = NULL;
	m_pDfuMainFlow = NULL;
	m_pCommandHandlerMgr = NULL;
	m_pInternalCommuMgr = NULL;
	m_pMongodbAccessHandler = NULL;

	bzero(m_chLogpath, sizeof(m_chLogpath));
	sprintf(m_chLogpath, "%s", MANAGER_BOARD_DEFAULT_ROOT_LOG);
	m_nLogLevel = CLogFile::trace;
	m_nLogDays = COLLECTOR_LOG_SAVE_DEFAULT_DAYS;

	InitLogFile();//init log
}


CRecordDataCollector::~CRecordDataCollector(void)
{
	m_Log.Close();
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

		bzero(m_chLogpath, sizeof(m_chLogpath));
		sprintf(m_chLogpath, "%s", m_pConfigvarialemgr->GetSysParam_LogPath());
		m_nLogLevel = m_pConfigvarialemgr->GetSysParam_LogLevel();
		m_nLogDays = m_pConfigvarialemgr->GetSysParam_LogDays();

		InitLogFile();

		if (false == InitMongoDbAccessHandler())
		{
			return false;
		}

		if (false == InitDfuMainFlowHandler())
		{
			return false;
		}

		if (false == InitInternalCommuMgr())
		{
			return false;
		}
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, 
			"[InitRecordDataColletor]init record dataCollector find exception！");
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
		if (false == m_pDfuMainFlow->StartMainFlow())
		{
			return false;
		}

		if (false == m_pInternalCommuMgr->StartCommandMonitorHandler())
		{
			return false;
		}
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, 
			"[StartRecordDataColletor]start record dataCollector find exception！");
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
		if (NULL != m_pDfuMainFlow)
		{
			m_pDfuMainFlow->StopMainFlow();
		}

		if (NULL != m_pInternalCommuMgr)
		{
			m_pInternalCommuMgr->StopCommandMonitorHandler();
		}
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, 
			"[EndRecordDataColletor]end record dataCollector find exception！");
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
		if (NULL != m_pDfuMainFlow)
		{
			delete m_pDfuMainFlow;
			m_pDfuMainFlow = NULL;
		}
		
		if (NULL != m_pInternalCommuMgr)
		{
			delete m_pInternalCommuMgr;
			m_pInternalCommuMgr = NULL;
		}

		if (NULL != m_pMongodbAccessHandler)
		{
			delete m_pMongodbAccessHandler;
			m_pMongodbAccessHandler = NULL;
		}

		if (NULL != m_pConfigvarialemgr)
		{
			delete m_pConfigvarialemgr;
			m_pConfigvarialemgr = NULL;
		}
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, 
			"[ExitRecordDataColletor]exit record dataCollector find exception！");
		return false;
	}
	return true;
}

//init log file
bool CRecordDataCollector::InitLogFile()
{
	m_Log.Close();

	m_Log.SetLogPath(m_chLogpath);
	m_Log.SetLogLevel(m_nLogLevel);
	m_Log.SetLogSaveDays(m_nLogDays);

	return (TRUE == m_Log.Open("data_collector"))?true:false;
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
			m_Log.FormatAdd(CLogFile::error, "new class CConfigVariableMgr failed！");
			return false;
		}

		m_pConfigvarialemgr->SetLogAccessHandler(&m_Log);
		if (false == m_pConfigvarialemgr->InitCollectorSysparam())
		{
			m_Log.FormatAdd(CLogFile::error, "[InitSysConfigVariable]InitCollectorSysparam failed！");
			return false;
		}

		if (false == m_pConfigvarialemgr->LoadCollectorSysParam())
		{
			m_Log.FormatAdd(CLogFile::error, "[InitSysConfigVariable]LoadCollectorSysParam failed！");
			return false;
		}

		m_Log.FormatAdd(CLogFile::trace, "[InitSysConfigVariable]init config succeed！");
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, 
			"[InitSysConfigVariable]init collector system param find exception！");
		return false;
	}

	return true;
}

//init mongodb access handler
bool CRecordDataCollector::InitMongoDbAccessHandler()
{
	string strError = "";

	try
	{
		mongo::client::GlobalInstance mongo_instance;//init mongo
		if (!mongo_instance.initialized())
		{
			m_Log.FormatAdd(CLogFile::error, "failed to initialize the client driver，reason：%s", 
				mongo_instance.status().codeString().c_str());
			return false;
		}

		if (NULL == m_pMongodbAccessHandler)
		{
			m_pMongodbAccessHandler = new CMongodbAccess;
		}

		if (NULL == m_pMongodbAccessHandler)
		{
			m_Log.FormatAdd(CLogFile::error, "new class CMongodbAccess failed！");
			return false;
		}

		m_pMongodbAccessHandler->SetMongDbAccessparam(m_pConfigvarialemgr->GetMongoParamHandle());
		if (false == m_pMongodbAccessHandler->ConnectMongoServer(strError))
		{
			m_Log.FormatAdd(CLogFile::error, 
				"connect mongodb failed，reason：%s", strError.c_str());
			return false;
		}
		m_Log.FormatAdd(CLogFile::trace, "connect mongodb succeed！");
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, 
			"[InitSysConfigVariable]init collector system param find exception！");
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
bool CRecordDataCollector::InitDfuMainFlowHandler()
{
	try
	{
		if (NULL == m_pDfuMainFlow)
		{
			m_pDfuMainFlow = new CDfuMainFlow;
		}

		if (NULL == m_pDfuMainFlow)
		{
			m_Log.FormatAdd(CLogFile::error, "[InitDfuMainFlowHandler]new class CDfuMainFlow failed！");
			return false;
		}

		m_pDfuMainFlow->SetMainFlowParamHandler(
			m_pConfigvarialemgr->GetSysParamHandler(), m_pConfigvarialemgr->GetDfuCommuParamHandler(true));
		m_pDfuMainFlow->SetMongoDbAccessHandler(m_pMongodbAccessHandler);
		if (false == m_pDfuMainFlow->InitMainFlow())
		{
			m_Log.FormatAdd(CLogFile::error, "[InitDfuMainFlowHandler]InitMainFlow failed！");
			return false;
		}

		m_Log.FormatAdd(CLogFile::trace, "[InitDfuMainFlowHandler]InitMainFlow succeed！");
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, "[InitDfuMainFlowHandler]InitMainFlow find exception！");
		return false;
	}
	
	return true;
}

bool CRecordDataCollector::InitInternalCommuMgr()
{
	try
	{
		if (NULL == m_pInternalCommuMgr)
		{
			m_pInternalCommuMgr = new CInternalCommuMgr;
		}

		if (NULL == m_pInternalCommuMgr)
		{
			m_Log.FormatAdd(CLogFile::error, "[InitInternalCommuMgr]new class CInternalCommuMgr failed！");
			return false;
		}

		m_pInternalCommuMgr->SetConfigVariableMgrHandle(m_pConfigvarialemgr);
		m_pInternalCommuMgr->SetDfuMainFlowHandler(m_pDfuMainFlow);
		if (false == m_pInternalCommuMgr->InitCommandMonitorHandler())
		{
			m_Log.FormatAdd(CLogFile::error, "[InitInternalCommuMgr]InitCommandMonitorHandler failed！");
			return false;
		}

		m_Log.FormatAdd(CLogFile::trace, "[InitInternalCommuMgr]InitCommandMonitorHandler succeed！");
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, "[InitInternalCommuMgr]init command monitor find exception！");
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
			m_Log.FormatAdd(CLogFile::error, "new class CCommandHandlerMgr failed！");
			return false;
		}
	}
	catch (...)
	{
		m_Log.FormatAdd(CLogFile::error, "[InitCommandHandlerMgr]init command handler manager find exception！");
		return false;
	}
	
	return true;
}
