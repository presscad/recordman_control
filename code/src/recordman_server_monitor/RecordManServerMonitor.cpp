#include "RecordManServerMonitor.h"


CRecordManServerMonitor::CRecordManServerMonitor(void)
{
	bzero(m_chLogpath, sizeof(m_chLogpath));
	sprintf(m_chLogpath, "%s", MANAGER_BOARD_DEFAULT_ROOT_LOG);
	m_nLogLevel = CLogFile::trace;
	m_nLogDays = MONITOR_LOG_SAVE_DEFAULT_DAYS;

	InitLogFile();
}


CRecordManServerMonitor::~CRecordManServerMonitor(void)
{
}

bool CRecordManServerMonitor::InitLogFile()
{
	m_Log.Close();

	m_Log.SetLogPath(m_chLogpath);
	m_Log.SetLogLevel(m_nLogLevel);
	m_Log.SetLogSaveDays(m_nLogDays);

	return (TRUE == m_Log.Open("server_monitor"))?true:false;
}

bool CRecordManServerMonitor::InitServerMonitor()
{
	return true;
}

bool CRecordManServerMonitor::StartServerMonitor()
{
	return true;
}

bool CRecordManServerMonitor::EndServerMonitor()
{
	return true;
}

bool CRecordManServerMonitor::ExitServerMonitor()
{
	return true;
}
