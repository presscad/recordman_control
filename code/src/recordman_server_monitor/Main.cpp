/*!
 * \file Main.cpp
 * \date 2015/12/27 12:36
 *
 * \author pengl
 * Contact: 
 *
 * \brief 
 *
 * TODO: main
 *
 * \note
*/
#pragma warning(disable : 4275)
#pragma warning(disable : 4996)

#include "RecordManServerMonitor.h"

/***********************全局变量******************************/
/**	\brief 退出标志*/
bool bExitMain = false;

/**	\brief 是否是以服务方式运行*/
bool bRunAsServices = false;

/**	\brief 入口类实例*/
CRecordManServerMonitor g_server_monitor;

#ifdef OS_WINDOWS
	/**	\brief 服务运行状态*/
	SERVICE_STATUS servicestatus;

	/**	\brief 服务句柄*/
	SERVICE_STATUS_HANDLE servicestatushandle;
#endif

void WINAPI KServiceMain(DWORD argc, LPTSTR * argv);

void InstallService(const char * szServiceName);

bool Start();

BOOL IsInstalled();

BOOL Uninstall();

//************************************
// 函 数 名:  sigroutine
// 功能概要:  信号处理函数
// 访问权限:  public 
// 返 回 值:  void 
// 参    数:  int nsig 信号值
//************************************
void sigroutine(int nsig)
{
	switch(nsig)
	{
	case SIGINT:
	case SIGTERM:
		{
			g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
				"recv exit sinal（%d），start exit program normal！", nsig);
			bExitMain = true;
		}
		break;

	default:
		break;
	}
}

//************************************
// 函 数 名:  ServiceCtrlHandler
// 功能概要:  响应服务台命令
// 访问权限:  public 
// 返 回 值:  void WINAPI 
// 参    数:  DWORD dwControl 收到的命令
//************************************
void WINAPI ServiceCtrlHandler(DWORD dwControl)
{
#ifdef OS_WINDOWS
	switch (dwControl)
	{
	case SERVICE_CONTROL_PAUSE:
		break;

	case SERVICE_CONTROL_CONTINUE:
		break;

	case SERVICE_CONTROL_STOP:
		servicestatus.dwCurrentState = SERVICE_STOP_PENDING;
		bExitMain = true;
		break;

	case SERVICE_CONTROL_SHUTDOWN:
		break;

	case SERVICE_CONTROL_INTERROGATE:
		break;
	}
	SetServiceStatus(servicestatushandle, &servicestatus);
#endif
}

//************************************
// 函 数 名:  Start
// 功能概要:  开始
// 访问权限:  public 
// 返 回 值:  bool true：成功 false：失败
//************************************
bool Start()
{
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "begin init param！");
	if(false == g_server_monitor.InitServerMonitor())
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"init param fail，begin exit！");
		return false;
	}
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "init param succeed，begin start！");

	if(false == g_server_monitor.StartServerMonitor())
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, "start failed，begin to exit！");
		return false;
	}
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "start program succeed！");

	return true;
}

//************************************
// 函 数 名:  KServiceMain
// 功能概要:  开始主程序
// 访问权限:  public 
// 返 回 值:  void WINAPI 
// 参    数:  DWORD argc 参数
// 参    数:  LPTSTR * argv 
//************************************
void WINAPI KServiceMain(DWORD argc, LPTSTR * argv)
{
#ifdef OS_WINDOWS
	if (bRunAsServices)
	{
		servicestatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		servicestatus.dwCurrentState = SERVICE_START_PENDING;
		servicestatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
		servicestatus.dwWin32ExitCode = 0;
		servicestatus.dwServiceSpecificExitCode = 0;
		servicestatus.dwCheckPoint = 0;
		servicestatus.dwWaitHint = 0;
		servicestatushandle = ::RegisterServiceCtrlHandler("recordman_server_monitor", ServiceCtrlHandler);
		if (servicestatushandle == (SERVICE_STATUS_HANDLE)0)
		{
			return;
		}
		::SetServiceStatus(servicestatushandle, &servicestatus);
	}
#endif

	bool bInitialized = false;
	bExitMain = false;
	bInitialized = Start();//开始加载

#ifdef OS_LINUX
	signal(SIGTERM,sigroutine);//对SIGTERM处理
	signal(SIGINT,sigroutine);//对SIGINT处理
#else
	signal(SIGINT,sigroutine);//对SIGINT处理
#endif
	
#ifdef OS_WINDOWS
	bExitMain = !bInitialized;
	if (bRunAsServices)
	{
		if (bInitialized)
		{
			servicestatus.dwCurrentState = SERVICE_RUNNING;
			SetServiceStatus(servicestatushandle, &servicestatus);
		}
	}
#endif

	while(1)//业务执行中,如果收到中断信息则退出
	{
		if(bExitMain)
		{
			break;
		}

#ifdef OS_LINUX
		if (!bInitialized)//加载失败时
		{
			break;
		}
#endif

		try
		{
			MySleep(2000);//等待2秒
		}
		catch(...)
		{
			printf("sleep excption\n");
		}		
	}

	//停止业务
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "start end program！");
	g_server_monitor.EndServerMonitor();	
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "end program succeed！");

	//退出并释放资源
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "start exit program！");
	g_server_monitor.ExitServerMonitor();
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "exit program succeed！");
	
#ifdef OS_WINDOWS
	if (bRunAsServices)
	{
		servicestatus.dwCheckPoint = 0;
		servicestatus.dwWaitHint = 0;
		servicestatus.dwCurrentState = SERVICE_STOPPED;
		if (!bInitialized)
		{
			servicestatus.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
			servicestatus.dwServiceSpecificExitCode = 99;
		}
		else
		{
			servicestatus.dwWin32ExitCode = NO_ERROR;
			servicestatus.dwServiceSpecificExitCode = 0;
		}
		SetServiceStatus(servicestatushandle, &servicestatus);
	}
#endif
	return;
}

//************************************
// 函 数 名:  InstallService
// 功能概要:  安装服务
// 访问权限:  public 
// 返 回 值:  void 
// 参    数:  const char * szServiceName 服务名
//************************************
void InstallService(const char * szServiceName)
{
#ifdef OS_WINDOWS
	SC_HANDLE handle = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (handle == NULL)
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"Couldn't open service manager！");
		return ;
	}

	char szFilename[256];
	::GetModuleFileName(NULL, szFilename, 255);
	string strFilename = szFilename;
	strFilename += " -s";
	SC_HANDLE hService = ::CreateService(handle, szServiceName, 
		szServiceName, SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, 
		SERVICE_AUTO_START, SERVICE_ERROR_NORMAL, strFilename.c_str(), NULL, 
		NULL, NULL, NULL, NULL);
	if (hService == NULL)
    {
        ::CloseServiceHandle(handle);
        g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"can not register service，CreateService failed！");
        return ;
    }
	::CloseServiceHandle(hService);
	::CloseServiceHandle(handle);
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
		"register succeed，please use command(Net Start recordman_server_monitor) to start service！");
#endif
}

//************************************
// 函 数 名:  IsInstalled
// 功能概要:  服务是否已经安装务
// 访问权限:  public 
// 返 回 值:  BOOL TRUE:已经安装 FALSE:未安装
//************************************
BOOL IsInstalled()
{
#ifdef OS_WINDOWS
    BOOL bResult = FALSE;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);//打开服务控制管理器
	
    if (hSCM != NULL)
    {
        SC_HANDLE hService = ::OpenService(hSCM, "recordman_server_monitor", SERVICE_QUERY_CONFIG);
        if (hService != NULL)
        {
            bResult = TRUE;
            ::CloseServiceHandle(hService);
        }
        ::CloseServiceHandle(hSCM);
    }
	else
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"[IsInstalled]open windows sc manager failed！");
	}
    return bResult;
#endif
}

//************************************
// 函 数 名:  Uninstall
// 功能概要:  反安装服务
// 访问权限:  public 
// 返 回 值:  BOOL TRUE:成功 FALSE:失败
//************************************
BOOL Uninstall()
{
#ifdef OS_WINDOWS
    if (!IsInstalled())
        return TRUE;
	
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	
    if (hSCM == NULL)
    {
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"[Uninstall] can not open service manager！");
        return FALSE;
    }
	
    SC_HANDLE hService = ::OpenService(hSCM, "recordman_server_monitor", SERVICE_STOP | DELETE);
	
    if (hService == NULL)
    {
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"[Uninstall] can not open service！");
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    BOOL bDelete = ::DeleteService(hService);//删除服务
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
	
    if (bDelete)
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
			"[Uninstall]delete recordman_server_monitor service succeed！");
        return TRUE;
	}
	else
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"[Uninstall]delete recordman_server_monitor service failed！");
	}
#endif
    return FALSE;
}

//************************************
// 函 数 名:  main
// 功能概要:  c/c++ 程序入口
// 访问权限:  public 
// 返 回 值:  int 
// 参    数:  int argc 参数个数(应用程序名作为第一个参数,依次类推)
// 参    数:  char * argv[] 存放各个参数的指针数组
//************************************
int main(int argc, char *argv[])
{
	BOOL Result(TRUE);

#ifdef OS_WINDOWS
	char exeFullPath[MAX_LINE_LENGTH];
	::GetModuleFileName(NULL, exeFullPath, MAX_LINE_LENGTH);
	string strProgramFullPath = GetFilePathFromFullFileName(exeFullPath);

	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
		"program run path：%s", strProgramFullPath.c_str());

	SetCurrentDirectory(strProgramFullPath.c_str());//设置路径

	//服务方式
	if ((argc==2) && (strcmp(argv[1], "install")==0))//安装
	{
		InstallService("recordman_server_monitor");
		return 0;
	}
	else if ((argc==2) && stricmp(argv[1], "uninstall") == 0)//卸载
	{
		Uninstall();
		return 0;
	}
	else if (
		(argc == 2) && 
		(stricmp(argv[1], "-s") == 0 || stricmp(argv[1], "-S") == 0))//以服务方式启动
	{
		bRunAsServices = true;
		SERVICE_TABLE_ENTRY   service_table_entry[] =
 		{
 			{ "recordman_server_monitor", KServiceMain },
 			{ NULL, NULL }
 		};
 		Result = ::StartServiceCtrlDispatcher(service_table_entry);
	}
	else if ((argc == 2) && 
		(stricmp(argv[1], "-h") == 0 || stricmp(argv[1], "-H") == 0))
	{
		if (!IsInstalled())
		{
			printf("\n输入 %s install 进行安装服务 \n", argv[0]);
			printf("输入 %s uninstall 进行反安装服务 \n", argv[0]);
			return 0;
		}
		else
		{
			printf("\n服务已注册,请使用\"Net Start recordman_server_monitor\"启动服务 \n");
			return 0;
		}
	}
	else
	{
		KServiceMain(0,NULL);
	}
#endif

/* LINUX下正常启动 */
#ifdef OS_LINUX
	KServiceMain(0,NULL);
#endif
	return (Result)?0:-1;
}

