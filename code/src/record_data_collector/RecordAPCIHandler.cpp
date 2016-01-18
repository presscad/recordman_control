#include "RecordAPCIHandler.h"

//************************************
// Method:    DFU_APCI_OPERATION_THREAD_PROC
// FullName:  DFU_APCI_OPERATION_THREAD_PROC
// Access:    public 
// Returns:   THREAD_FUNC WINAPI
// Qualifier: ��dfu��ҵ�����߳�
// Parameter: LPVOID param
//************************************
THREAD_FUNC WINAPI DFU_APCI_OPERATION_THREAD_PROC(LPVOID param)
{
	CRecordAPCIHandler* pRecordApciHandler = (CRecordAPCIHandler*)param;

	try
	{
		if (NULL == pRecordApciHandler)
		{
			return THREAD_RETURN;
		}

		pRecordApciHandler->DfuCommuOperationLoop();
	}
	catch (...)
	{
		printf("DFU_APCI_OPERATION_THREAD_PROC thread exit abnormal��\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

CRecordAPCIHandler::CRecordAPCIHandler(void)
{
	m_bExitFlag = true;
	m_pCollectorSysParam = NULL;
	m_pNetSocket = NULL;
}


CRecordAPCIHandler::~CRecordAPCIHandler(void)
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
// Qualifier: ���ò������ʶ���ָ��
// Parameter: COLLECTOR_DATA_SYS_PARAM * pParam �������ʶ���ָ��
//************************************
void CRecordAPCIHandler::SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam)
{
	m_pCollectorSysParam = pParam;
}

//************************************
// Method:    InitDfuCommuSession
// FullName:  CDfuCommuSession::InitDfuCommuSession
// Access:    public 
// Returns:   bool true���ɹ� false��ʧ��
// Qualifier: ��ʼ��
//************************************
bool CRecordAPCIHandler::InitRecordApciHandler()
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
			printf("new class CNet failed��\n");
			return false;
		}

		if (false == socketsInit())
		{
			printf("socket init failed��\n");
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
// Returns:   bool true���ɹ� false��ʧ��
// Qualifier: ����
//************************************
bool CRecordAPCIHandler::StartRecordApciHandler()
{
	bool bRet = false;

	try
	{
		m_DfuOperationThread.Stop();

		bRet = m_pNetSocket->ConnectServer(m_pCollectorSysParam->chDfuAddr, m_pCollectorSysParam->nDfuPort);
		if (false == bRet)
		{
			printf("connect dfu server��%s port��%d failed��\n", 
				m_pCollectorSysParam->chDfuAddr, m_pCollectorSysParam->nDfuPort);
			return false;
		}

		m_pNetSocket->SetOptions(SENDTIME, m_pCollectorSysParam->nSendTimeout, 0);
		m_pNetSocket->SetOptions(RECVTIME, m_pCollectorSysParam->nRecvTimeout, 0);

		m_bExitFlag = false;

		if (false == m_DfuOperationThread.Start(DFU_APCI_OPERATION_THREAD_PROC, this))
		{
			printf("start dfu operation thread failed��\n");
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
// Returns:   bool true���ɹ� false��ʧ��
// Qualifier: ֹͣ
//************************************
bool CRecordAPCIHandler::StopRecordApciHandler()
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
int CRecordAPCIHandler::DfuCommuOperationLoop()
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
			msg_parser.SetMsgStartMask();
			msg_parser.SetMsgTransMask(1);
			msg_parser.SetMsgProtocolMask();
			msg_parser.SetMsgReserve();
			msg_parser.SetMsgLength(8);
			msg_parser.SetMsgCommand(1);
			msg_parser.SetEndMask();
			msg_parser.SetMsgEndFlag(true);

			int nRet = WriteRecordMsg(pMsg);

			bzero(pMsg, sizeof(RECORD_DFU_MSG));
			ReceiveMsg(pMsg);

			MySleep(100);
		}
		catch (...)
		{
			printf("DfuCommuOperationLoop exit abormal��\n");
			return -1;
		}
	}

	return 0;
}

//************************************
// �� �� ���� ReceiveMsg
// ���ܸ�Ҫ�� recv msg from net
// ����Ȩ�ޣ� private 
// �� �� ֵ�� int 
// ��    ���� RECORD_DFU_MSG * pMsg 
//************************************
int CRecordAPCIHandler::ReceiveMsg(RECORD_DFU_MSG* pMsg)
{
	if(pMsg == NULL)
	{
		return -1;
	}

	CDfuMsgParser dfuMsgParser;
	dfuMsgParser.Attach(pMsg);

	int nRet(0);
	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteStartMask[0], 2);//��ȡ������
	if (nRet <= 0)
	{
		printf("recv start code msg failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	if (false == dfuMsgParser.CheckStartMask())
	{
		printf("recv wrong start code msg��\n");
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteTransMark[0], 2);//����ʶ����
	if (nRet <= 0)
	{
		printf("recv trans code msg failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteProtocolMark[0], 2);//Э����
	if (nRet <= 0)
	{
		printf("recv protocol code msg failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	if (false == dfuMsgParser.CheckMsgProtocolMask())
	{
		printf("recv wrong protocol code msg��\n");
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteReserve[0], 2);//����
	if (nRet <= 0)
	{
		printf("recv reserve code msg failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteMsgLen[0], 2);//���ĳ���
	if (nRet <= 0)
	{
		printf("recv msg length code failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	int nMsgLen = dfuMsgParser.GetMsgLength();

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteFunMask[0], 2);//������
	if (nRet <= 0)
	{
		printf("recv fun mask failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteCommandMask[0], 2);//������
	if (nRet <= 0)
	{
		printf("recv command mask failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteFrameSeq[0], 4);//֡���
	if (nRet <= 0)
	{
		printf("recv frame seq failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	if (nMsgLen > 8)
	{
		nRet = m_pNetSocket->read((char*)&pMsg->MsgBody[0], (nMsgLen - 8));
		if (nRet <= 0)
		{
			printf("recv msg body failed��\n");
			return (0 == nRet)?-1:nRet;
		}
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteEndMask[0], 2);
	if (nRet <= 0)
	{
		printf("recv end mask failed��\n");
		return (0 == nRet)?-1:nRet;
	}

	if (false == dfuMsgParser.CheckEndMask())
	{
		printf("recv wrong end mask msg��\n");
	}

	LOG_BUFFER_HEAD recv_log_header;
	bzero(&recv_log_header, sizeof(LOG_BUFFER_HEAD));
	recv_log_header.nWay = MSG_RECV;
	m_pNetSocket->GetOpponentAddr(&recv_log_header.addr);
	LogMessage(pMsg, recv_log_header);
	
	return -1;
}

//************************************
// �� �� ���� WriteRecordMsg
// ���ܸ�Ҫ�� send msg
// ����Ȩ�ޣ� private 
// �� �� ֵ�� int -1��failed other��send bytes length
// ��    ���� RECORD_DFU_MSG * pMsg 
//************************************
int CRecordAPCIHandler::WriteRecordMsg(RECORD_DFU_MSG* pMsg)
{
	if (NULL == pMsg)
	{
		return -1;
	}

	int nRet(0);
	int nWriteRet(0);
	int nMsgSendLen = 0;
	CDfuMsgParser msg_parser;
	msg_parser.Attach(pMsg);
	vector<BYTE> send_msg;
	send_msg.clear();

	send_msg.push_back(pMsg->DfuMsgHdr.byteStartMask[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteStartMask[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteTransMark[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteTransMark[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteProtocolMark[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteProtocolMark[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteReserve[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteReserve[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteMsgLen[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteMsgLen[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteFunMask[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteFunMask[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteCommandMask[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteCommandMask[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteFrameSeq[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteFrameSeq[1]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteFrameSeq[2]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteFrameSeq[3]);

	for (int i = 0; i < (msg_parser.GetMsgLength() - 8); i++)
	{
		send_msg.push_back(pMsg->MsgBody[i]);
	}

	send_msg.push_back(pMsg->DfuMsgHdr.byteEndMask[0]);
	send_msg.push_back(pMsg->DfuMsgHdr.byteEndMask[1]);

	nMsgSendLen = send_msg.size();

	try
	{
		nWriteRet = m_pNetSocket->write(&send_msg[0], nMsgSendLen);
	}
	catch (...)
	{
		
	}

	if (nWriteRet < nMsgSendLen)
	{
		printf("[WriteRecordMsg]send msg failed��return��%d", 
			nWriteRet);
		nRet = -1;
	}

	if (nWriteRet > 0)
	{
		LOG_BUFFER_HEAD send_log_header;
		bzero(&send_log_header, sizeof(LOG_BUFFER_HEAD));
		send_log_header.nWay = MSG_SEND;
		m_pNetSocket->GetOpponentAddr(&send_log_header.addr);

		LogMessage(pMsg, send_log_header);
	}

	return 0;
}

//************************************
// �� �� ���� LogMessage
// ���ܸ�Ҫ�� print log to screen
// ����Ȩ�ޣ� private 
// �� �� ֵ�� void 
// ��    ���� const RECORD_DFU_MSG * pMsg msg struct
// ��    ���� const LOG_BUFFER_HEAD & pHead log header
//************************************
void CRecordAPCIHandler::LogMessage(const RECORD_DFU_MSG* pMsg, const LOG_BUFFER_HEAD& pHead)
{
	if(NULL == pMsg)
		return ;
	
	string strLog = "";
	CDfuMsgParser msg_parser;
	char chtemp[10]="";
	int nHeadLen  = 0;
	
	msg_parser.Attach((RECORD_DFU_MSG*)pMsg);
	strLog.resize((msg_parser.GetMsgLength() + 10)*3 + sizeof(LOG_BUFFER_HEAD) + 30);
	
	char * pchar = (char *)&strLog[0];
	
	// print log head
	switch(pHead.nWay)
	{
	case MSG_RECV:
		sprintf(pchar,"recv from %s(%d) \r\n", 
			pHead.addr.IpAddr, pHead.addr.nPort);
		
		nHeadLen += strlen(pchar);
		pchar    += strlen(pchar);
		break;

	case MSG_SEND:
		sprintf(pchar,"send to %s(%d) \r\n",
			pHead.addr.IpAddr,pHead.addr.nPort);
		
		nHeadLen += strlen(pchar);
		pchar    += strlen(pchar);
		
		break;

	default:
		break;
	}
	
    
	// printf log body
	try
	{
		CopyMessageToString(pMsg->DfuMsgHdr.byteStartMask[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteStartMask[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteTransMark[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteTransMark[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteProtocolMark[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteProtocolMark[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteReserve[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteReserve[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteMsgLen[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteMsgLen[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteFunMask[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteFunMask[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteCommandMask[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteCommandMask[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteFrameSeq[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteFrameSeq[1], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteFrameSeq[2], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteFrameSeq[3], pchar);

		for (int i = 0; i < (msg_parser.GetMsgLength() - 8); i++)
		{
			CopyMessageToString(pMsg->MsgBody[i], pchar);
		}

		CopyMessageToString(pMsg->DfuMsgHdr.byteEndMask[0], pchar);
		CopyMessageToString(pMsg->DfuMsgHdr.byteEndMask[1], pchar);
		
		// add enter
		memcpy(pchar, "\r\n", 2);
		
		printf("%s\n", strLog.c_str());
	}
	catch(...)
	{
		printf("LogMessage exception��\n");
		return;
	}		
}

//************************************
// �� �� ���� CopyMessageToString
// ���ܸ�Ҫ�� copy message to string buffer
// ����Ȩ�ޣ� private 
// �� �� ֵ�� void 
// ��    ���� BYTE bMsg byte msg
// ��    ���� char * & pChar string buffer
//************************************
void CRecordAPCIHandler::CopyMessageToString(BYTE bMsg, char*& pChar)
{
	char temp[3] = "";
	char temp1[2] = "";

	if (bMsg < 16)
	{
		Record_Itoa(bMsg, temp1, 16);
		temp[1] = temp1[0];
		temp[0] = '0';
		memset(temp1, 0, 1);
	}
	else
	{
		Record_Itoa(bMsg, temp, 16);
	}


	memcpy(pChar, temp, 2);
	pChar += 2;
	memcpy(pChar, " ", 1);
	pChar += 1;
}
