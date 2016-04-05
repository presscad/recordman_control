#if !defined(RECORDMAN_SERVER_MONITOR_INCLUDE)
#define RECORDMAN_SERVER_MONITOR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "const_define.h"

class CRecordManServerMonitor
{
public:
	CRecordManServerMonitor(void);
	~CRecordManServerMonitor(void);

private:
	//init log file
	bool InitLogFile();

public:
	//init
	//true��success false��failed
	bool InitServerMonitor();

	//start
	//true��success false��failed
	bool StartServerMonitor();

	//stop
	//true��success false��failed
	bool EndServerMonitor();

	//exit
	//true��success false��failed
	bool ExitServerMonitor();

public:
	/**	\brief logfile*/
	CMessageLog m_Log;

private:
	//log level
	int m_nLogLevel;

	//log days
	int m_nLogDays;

	//log path
	char m_chLogpath[MAX_LINE_LENGTH];
};

#endif

