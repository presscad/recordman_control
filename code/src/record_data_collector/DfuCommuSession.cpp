#include "DfuCommuSession.h"

//************************************
// Method:    DFU_OPERATION_THREAD_PROC
// FullName:  DFU_OPERATION_THREAD_PROC
// Access:    public 
// Returns:   THREAD_FUNC WINAPI
// Qualifier: 与dfu的业务处理线程
// Parameter: LPVOID param
//************************************
THREAD_FUNC WINAPI DFU_OPERATION_THREAD_PROC(LPVOID param)
{
	CDfuCommuSession* pDfuCommuSession = (CDfuCommuSession*)param;

	try
	{
		if (NULL == pDfuCommuSession)
		{
			return THREAD_RETURN;
		}

		pDfuCommuSession->DfuCommuOperationLoop();
	}
	catch (...)
	{
		printf("DFU_OPERATION_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}


CDfuCommuSession::CDfuCommuSession(void)
{
	m_bExitFlag = true;
	m_pCollectorSysParam = NULL;
	m_pNetSocket = NULL;
}


CDfuCommuSession::~CDfuCommuSession(void)
{
	if (NULL != m_pNetSocket)
	{
		m_pNetSocket->close();
		delete m_pNetSocket;
		m_pNetSocket = NULL;
	}
}

//************************************
// Method:    SetCollectorSysParam
// FullName:  CDfuCommuSession::SetCollectorSysParam
// Access:    public 
// Returns:   void
// Qualifier: 设置参数访问对象指针
// Parameter: COLLECTOR_DATA_SYS_PARAM * pParam 参数访问对象指针
//************************************
void CDfuCommuSession::SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam)
{
	m_pCollectorSysParam = pParam;
}

//************************************
// Method:    InitDfuCommuSession
// FullName:  CDfuCommuSession::InitDfuCommuSession
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 初始化
//************************************
bool CDfuCommuSession::InitDfuCommuSession()
{
	try
	{
		m_bExitFlag = true;

		if (NULL == m_pNetSocket)
		{
			m_pNetSocket = new CNet;
		}

		if (NULL == m_pNetSocket)
		{
			printf("new class CNet failed！\n");
			return false;
		}

		if (false == socketsInit())
		{
			printf("socket init failed！\n");
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
// Method:    StartCommuSession
// FullName:  CDfuCommuSession::StartCommuSession
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 启动
//************************************
bool CDfuCommuSession::StartCommuSession()
{
	bool bRet = false;

	try
	{
		m_DfuOperationThread.Stop();

		bRet = m_pNetSocket->ConnectServer(m_pCollectorSysParam->chDfuAddr, m_pCollectorSysParam->nDfuPort);
		if (false == bRet)
		{
			printf("connect dfu server：%s port：%d failed！\n", 
				m_pCollectorSysParam->chDfuAddr, m_pCollectorSysParam->nDfuPort);
			return false;
		}

		m_pNetSocket->SetOptions(SENDTIME, m_pCollectorSysParam->nSendTimeout, 0);
		m_pNetSocket->SetOptions(RECVTIME, m_pCollectorSysParam->nRecvTimeout, 0);

		m_bExitFlag = false;

		if (false == m_DfuOperationThread.Start(DFU_OPERATION_THREAD_PROC, this))
		{
			printf("start dfu operation thread failed！\n");
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
// Method:    StopCommuSession
// FullName:  CDfuCommuSession::StopCommuSession
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 停止
//************************************
bool CDfuCommuSession::StopCommuSession()
{
	m_bExitFlag = true;

	if (NULL != m_pNetSocket)
	{
		m_pNetSocket->close();
	}

	m_DfuOperationThread.Stop();

	return true;
}

//************************************
// Method:    DfuCommuOperationLoop
// FullName:  CDfuCommuSession::DfuCommuOperationLoop
// Access:    public 
// Returns:   int
// Qualifier:
//************************************
int CDfuCommuSession::DfuCommuOperationLoop()
{
	RECORD_DFU_MSG* pMsg = NULL;

	while (!m_bExitFlag)
	{
		try
		{
			if (m_bExitFlag)
			{
				break;
			}

			if (NULL == pMsg)
			{
				pMsg = new RECORD_DFU_MSG;
			}

			if (NULL == pMsg)
			{
				break;
			}
			bzero(pMsg, sizeof(RECORD_DFU_MSG));

			CDfuMsgParser msg_parser;
			msg_parser.Attach(pMsg);
			msg_parser.SetMsgStartMark();
			msg_parser.SetMsgTransMark(1);
			msg_parser.SetMsgProtocolMark(1);
			msg_parser.SetMsgLength(8);
			pMsg->DfuMsgHdr.byteCommandMask[0] = '0x01';
			pMsg->DfuMsgHdr.byteEndMask[0] = '0x5F';
			pMsg->DfuMsgHdr.byteEndMask[1] = '0x5F';
			msg_parser.SetMsgEndFlag(true);

			int nRet = m_pNetSocket->write(&pMsg->DfuMsgHdr, 20);

			ReceiveMsg(pMsg);

			MySleep(100);
		}
		catch (...)
		{
			printf("DfuCommuOperationLoop exit abormal！\n");
			return -1;
		}
	}

	return 0;
}

int CDfuCommuSession::ReceiveMsg(RECORD_DFU_MSG* pMsg)
{
	if(pMsg == NULL)
	{
		return -1;
	}

	int nRet(0);

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr, sizeof(RECORD_DFU_MSG_HEADER));
	if(nRet < 16 && nRet > 0)
	{
		return nRet;
	}
	else if(nRet <= 0)//超时
	{
		return nRet;
	}
	else
	{
		printf("recved msg！\n");
	}
	return -1;
}