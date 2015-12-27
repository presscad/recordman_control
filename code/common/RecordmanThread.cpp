#pragma warning(disable : 4275)

#include "RecordmanThread.h"

CRecordmanThread::CRecordmanThread() 
{
	m_hHande = INVALID_THREAD;
	m_hId    = 0;
}

CRecordmanThread::~CRecordmanThread() 
{
	Stop();
}

//************************************
// Method:    Start
// FullName:  CRecordmanThread::Start
// Access:    public 
// Returns:   bool
// Qualifier: //
// Parameter: RECORD_MAN_THREAD_ROUTINE threadfun
// Parameter: LPVOID lpParameter
//************************************
bool CRecordmanThread::Start(RECORD_MAN_THREAD_ROUTINE threadfun, LPVOID lpParameter)//
{
	int nRet =0;
	
	try
	{
		// begine create sttp listen thread
		nRet = xj_thread_create(
			&m_hHande, &m_hId, (RECORD_MAN_THREAD_ROUTINE) threadfun, 
			lpParameter, 0);

		if(0 != nRet)
		{
			return false;
		}
	}
	catch(...)
	{
		printf("create thread error \n");
		return false;
	}

	return true;
}


//************************************
// Method:    Stop
// FullName:  CRecordmanThread::Stop
// Access:    public 
// Returns:   bool
// Qualifier:
//************************************
bool CRecordmanThread::Stop()
{
	if((m_hId == 0) || (m_hHande == INVALID_THREAD))
	{
		return false;
	}

	int nRet = 0;

#ifdef OS_LINUX
	nRet = xj_thread_join(m_hId,NULL);
	m_hId = 0;
#endif

#ifdef OS_WINDOWS	
	nRet = xj_thread_detach(m_hHande);
	m_hHande = INVALID_THREAD;
#endif

	if(nRet == 0)
		return true;
	else
		return false;
}

//************************************
// Method:    IsValid
// FullName:  CRecordmanThread::IsValid
// Access:    public 
// Returns:   bool
// Qualifier:
//************************************
bool CRecordmanThread::IsValid()
{
	if(m_hHande == INVALID_THREAD)
	{
		return false;
	}
	else
	{
		return true;
	}
}

//************************************
// Method:    GetThreadHandle
// FullName:  CRecordmanThread::GetThreadHandle
// Access:    public 
// Returns:   THREAD_HANDLE
// Qualifier:
//************************************
THREAD_HANDLE CRecordmanThread::GetThreadHandle()
{
	return m_hHande;
}

//************************************
// Method:    GetThreadId
// FullName:  CRecordmanThread::GetThreadId
// Access:    public 
// Returns:   THREAD_ID
// Qualifier:
//************************************
THREAD_ID CRecordmanThread::GetThreadId()
{
	return m_hId;
}
