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

/***********************ȫ�ֱ���******************************/
/**	\brief �˳���־*/
bool bExitMain = false;

/**	\brief �Ƿ����Է���ʽ����*/
bool bRunAsServices = false;

/**	\brief �����ʵ��*/
CRecordManServerMonitor g_server_monitor;

#ifdef OS_WINDOWS
	/**	\brief ��������״̬*/
	SERVICE_STATUS servicestatus;

	/**	\brief ������*/
	SERVICE_STATUS_HANDLE servicestatushandle;
#endif

void WINAPI KServiceMain(DWORD argc, LPTSTR * argv);

void InstallService(const char * szServiceName);

bool Start();

BOOL IsInstalled();

BOOL Uninstall();

//************************************
// �� �� ��:  sigroutine
// ���ܸ�Ҫ:  �źŴ�����
// ����Ȩ��:  public 
// �� �� ֵ:  void 
// ��    ��:  int nsig �ź�ֵ
//************************************
void sigroutine(int nsig)
{
	switch(nsig)
	{
	case SIGINT:
	case SIGTERM:
		{
			g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
				"recv exit sinal��%d����start exit program normal��", nsig);
			bExitMain = true;
		}
		break;

	default:
		break;
	}
}

//************************************
// �� �� ��:  ServiceCtrlHandler
// ���ܸ�Ҫ:  ��Ӧ����̨����
// ����Ȩ��:  public 
// �� �� ֵ:  void WINAPI 
// ��    ��:  DWORD dwControl �յ�������
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
// �� �� ��:  Start
// ���ܸ�Ҫ:  ��ʼ
// ����Ȩ��:  public 
// �� �� ֵ:  bool true���ɹ� false��ʧ��
//************************************
bool Start()
{
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "begin init param��");
	if(false == g_server_monitor.InitServerMonitor())
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"init param fail��begin exit��");
		return false;
	}
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "init param succeed��begin start��");

	if(false == g_server_monitor.StartServerMonitor())
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, "start failed��begin to exit��");
		return false;
	}
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "start program succeed��");

	return true;
}

//************************************
// �� �� ��:  KServiceMain
// ���ܸ�Ҫ:  ��ʼ������
// ����Ȩ��:  public 
// �� �� ֵ:  void WINAPI 
// ��    ��:  DWORD argc ����
// ��    ��:  LPTSTR * argv 
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
	bInitialized = Start();//��ʼ����

#ifdef OS_LINUX
	signal(SIGTERM,sigroutine);//��SIGTERM����
	signal(SIGINT,sigroutine);//��SIGINT����
#else
	signal(SIGINT,sigroutine);//��SIGINT����
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

	while(1)//ҵ��ִ����,����յ��ж���Ϣ���˳�
	{
		if(bExitMain)
		{
			break;
		}

#ifdef OS_LINUX
		if (!bInitialized)//����ʧ��ʱ
		{
			break;
		}
#endif

		try
		{
			MySleep(2000);//�ȴ�2��
		}
		catch(...)
		{
			printf("sleep excption\n");
		}		
	}

	//ֹͣҵ��
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "start end program��");
	g_server_monitor.EndServerMonitor();	
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "end program succeed��");

	//�˳����ͷ���Դ
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "start exit program��");
	g_server_monitor.ExitServerMonitor();
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, "exit program succeed��");
	
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
// �� �� ��:  InstallService
// ���ܸ�Ҫ:  ��װ����
// ����Ȩ��:  public 
// �� �� ֵ:  void 
// ��    ��:  const char * szServiceName ������
//************************************
void InstallService(const char * szServiceName)
{
#ifdef OS_WINDOWS
	SC_HANDLE handle = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (handle == NULL)
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"Couldn't open service manager��");
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
			"can not register service��CreateService failed��");
        return ;
    }
	::CloseServiceHandle(hService);
	::CloseServiceHandle(handle);
	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
		"register succeed��please use command(Net Start recordman_server_monitor) to start service��");
#endif
}

//************************************
// �� �� ��:  IsInstalled
// ���ܸ�Ҫ:  �����Ƿ��Ѿ���װ��
// ����Ȩ��:  public 
// �� �� ֵ:  BOOL TRUE:�Ѿ���װ FALSE:δ��װ
//************************************
BOOL IsInstalled()
{
#ifdef OS_WINDOWS
    BOOL bResult = FALSE;
    SC_HANDLE hSCM = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);//�򿪷�����ƹ�����
	
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
			"[IsInstalled]open windows sc manager failed��");
	}
    return bResult;
#endif
}

//************************************
// �� �� ��:  Uninstall
// ���ܸ�Ҫ:  ����װ����
// ����Ȩ��:  public 
// �� �� ֵ:  BOOL TRUE:�ɹ� FALSE:ʧ��
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
			"[Uninstall] can not open service manager��");
        return FALSE;
    }
	
    SC_HANDLE hService = ::OpenService(hSCM, "recordman_server_monitor", SERVICE_STOP | DELETE);
	
    if (hService == NULL)
    {
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"[Uninstall] can not open service��");
        ::CloseServiceHandle(hSCM);
        return FALSE;
    }
    SERVICE_STATUS status;
    ::ControlService(hService, SERVICE_CONTROL_STOP, &status);

    BOOL bDelete = ::DeleteService(hService);//ɾ������
    ::CloseServiceHandle(hService);
    ::CloseServiceHandle(hSCM);
	
    if (bDelete)
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
			"[Uninstall]delete recordman_server_monitor service succeed��");
        return TRUE;
	}
	else
	{
		g_server_monitor.m_Log.FormatAdd(CLogFile::error, 
			"[Uninstall]delete recordman_server_monitor service failed��");
	}
#endif
    return FALSE;
}

//************************************
// �� �� ��:  main
// ���ܸ�Ҫ:  c/c++ �������
// ����Ȩ��:  public 
// �� �� ֵ:  int 
// ��    ��:  int argc ��������(Ӧ�ó�������Ϊ��һ������,��������)
// ��    ��:  char * argv[] ��Ÿ���������ָ������
//************************************
int main(int argc, char *argv[])
{
	BOOL Result(TRUE);

#ifdef OS_WINDOWS
	char exeFullPath[MAX_LINE_LENGTH];
	::GetModuleFileName(NULL, exeFullPath, MAX_LINE_LENGTH);
	string strProgramFullPath = GetFilePathFromFullFileName(exeFullPath);

	g_server_monitor.m_Log.FormatAdd(CLogFile::trace, 
		"program run path��%s", strProgramFullPath.c_str());

	SetCurrentDirectory(strProgramFullPath.c_str());//����·��

	//����ʽ
	if ((argc==2) && (strcmp(argv[1], "install")==0))//��װ
	{
		InstallService("recordman_server_monitor");
		return 0;
	}
	else if ((argc==2) && stricmp(argv[1], "uninstall") == 0)//ж��
	{
		Uninstall();
		return 0;
	}
	else if (
		(argc == 2) && 
		(stricmp(argv[1], "-s") == 0 || stricmp(argv[1], "-S") == 0))//�Է���ʽ����
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
			printf("\n���� %s install ���а�װ���� \n", argv[0]);
			printf("���� %s uninstall ���з���װ���� \n", argv[0]);
			return 0;
		}
		else
		{
			printf("\n������ע��,��ʹ��\"Net Start recordman_server_monitor\"�������� \n");
			return 0;
		}
	}
	else
	{
		KServiceMain(0,NULL);
	}
#endif

/* LINUX���������� */
#ifdef OS_LINUX
	KServiceMain(0,NULL);
#endif
	return (Result)?0:-1;
}

