#include "ConfigVariableMgr.h"


CConfigVariableMgr::CConfigVariableMgr(void)
{
}


CConfigVariableMgr::~CConfigVariableMgr(void)
{
}

//************************************
// Method:    InitCollectorSysparam
// FullName:  CConfigVariableMgr::InitCollectorSysparam
// Access:    public 
// Returns:   bool true���ɹ� false��ʧ��
// Qualifier: ��ʼ������
//************************************
bool CConfigVariableMgr::InitCollectorSysparam()
{
	try
	{
		m_collector_sys_param.nLoglevel = CLogFile::trace;
		m_collector_sys_param.nLogDays = COLLECTOR_LOG_SAVE_DEFAULT_DAYS;
		m_collector_sys_param.nDfuPort = COLLECTOR_DFU_LISTEN_PORT;
		m_collector_sys_param.nIdleCheckTime = COLLECTOR_IDLE_CHECEK_DEFAULT_TIME;
		m_collector_sys_param.nRecvTimeout = COLLECTOR_COMMU_RECV_TIMEOUT;
		m_collector_sys_param.nSendTimeout = COLLECTOR_COMMU_SEND_TIMEOUT;
		sprintf(m_collector_sys_param.chDfuAddr, "%s", "10.123.16.56");
		sprintf(m_collector_sys_param.chLogpath, "%s", "./data_collector_log/");
	}
	catch (...)
	{
		printf("[InitCollectorSysparam]init system param find unknown exception��\n");
		return false;
	}

	return true;
}

//************************************
// Method:    LoadCollectorSysParam
// FullName:  CConfigVariableMgr::LoadCollectorSysParam
// Access:    public 
// Returns:   bool true���ɹ� false��ʧ��
// Qualifier: �ӿ��ж�ȡ����
//************************************
bool CConfigVariableMgr::LoadCollectorSysParam()
{
	return true;
}
