#include "RecordAPCIHandler.h"

THREAD_FUNC WINAPI send_dfu_msg_proc(LPVOID param)
{
	CRecordAPCIHandler* pRecordApciHandler = (CRecordAPCIHandler*)param;

	try
	{
		if (NULL == pRecordApciHandler)
		{
			return THREAD_RETURN;
		}

		pRecordApciHandler->Send_dfu_msg_operation_loop();
	}
	catch (...)
	{
		printf("DFU_MSG_SEND_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

THREAD_FUNC WINAPI recv_dfu_msg_proc(LPVOID param)
{
	CRecordAPCIHandler* pRecordApciHandler = (CRecordAPCIHandler*)param;

	try
	{
		if (NULL == pRecordApciHandler)
		{
			return THREAD_RETURN;
		}

		pRecordApciHandler->Recv_dfu_msg_operation_loop();
	}
	catch (...)
	{
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

CRecordAPCIHandler::CRecordAPCIHandler(void):
m_LockSocketSend("LOCK_SOCKET"),
m_LockCommandList("LOCK_COMMAND_BUF")
{
	m_bExitFlag = true;
	m_pDfuCommuParamHandler = NULL;
	m_pNetSocket = NULL;
	m_pDfuResultCallBackFunc = NULL;
	m_pResultProcessClassHandle = NULL;
	m_pSysParamHandler = NULL;

	m_utTransMask = 0;
	time(&m_tLinkActive);

	m_command_msg_buf.clear();
}


CRecordAPCIHandler::~CRecordAPCIHandler(void)
{
	if (NULL != m_pNetSocket)
	{
		m_pNetSocket->close();
		delete m_pNetSocket;
		m_pNetSocket = NULL;
	}

	m_LogFile.Close();
}

void CRecordAPCIHandler::SetDfuCommuParamHandler(COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParamHandler)
{
	m_pDfuCommuParamHandler = pDfuCommuParamHandler;
}

void CRecordAPCIHandler::SetSysParamHandler(COLLECTOR_DATA_SYS_PARAM* pSysParamHandler)
{
	m_pSysParamHandler = pSysParamHandler;
}

void CRecordAPCIHandler::RegisterDfuResultCallBackFunc(PRESULTDFUMSGCALLBACKFUNC pFunc, XJHANDLE pObj)
{
	m_pDfuResultCallBackFunc = pFunc;
	m_pResultProcessClassHandle = pObj;
}

bool CRecordAPCIHandler::InitLogFile()
{
	m_LogFile.Close();

	m_LogFile.SetLogLevel(m_pSysParamHandler->nLoglevel);
	m_LogFile.SetLogSaveDays(m_pSysParamHandler->nLogDays);
	m_LogFile.SetLogPath(m_pSysParamHandler->chLogpath);

	return (TRUE == m_LogFile.Open(m_pDfuCommuParamHandler->chDfuAddr))?true:false;
}

bool CRecordAPCIHandler::GetSendMsgFromList(DFU_COMMU_MSG& sendmsg)
{
	CLockUp lockup(&m_LockCommandList);

	if (m_command_msg_buf.size() <= 0)
	{
		return false;
	}

	DFUCOMMUMSG_BUF::iterator it = m_command_msg_buf.begin();
	sendmsg = *it;
	m_command_msg_buf.erase(it);

	return true;
}

bool CRecordAPCIHandler::InitRecordApciHandler()
{
	try
	{
		m_bExitFlag = true;

		InitLogFile();

		if (NULL == m_pNetSocket)
		{
			m_pNetSocket = new CNet;
		}

		if (NULL == m_pNetSocket)
		{
			m_LogFile.FormatAdd(CLogFile::error, "new class CNet failed！");
			return false;
		}

		if (false == socketsInit())
		{
			m_LogFile.FormatAdd(CLogFile::error, "socket init failed！");
			return false;
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "InitRecordApciHandler find exception！");
		return false;
	}

	return true;
}

bool CRecordAPCIHandler::StartRecordApciHandler()
{
	bool bRet = false;

	try
	{
		m_DfuBuinessThread.Stop();
		m_DfuRecvThread.Stop();

		bRet = m_pNetSocket->ConnectServer(
			m_pDfuCommuParamHandler->chDfuAddr, 
			m_pDfuCommuParamHandler->nDfuport);

		if (false == bRet)
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]connect dfu server：%s port：%d failed！", 
				m_pDfuCommuParamHandler->chDfuAddr, 
				m_pDfuCommuParamHandler->nDfuport);

			return false;
		}

		m_LogFile.FormatAdd(CLogFile::trace, 
			"[StartRecordApciHandler]connect dfu server：%s port：%d succeed！", 
			m_pDfuCommuParamHandler->chDfuAddr, 
			m_pDfuCommuParamHandler->nDfuport);

		m_pNetSocket->SetOptions(SENDTIME, (m_pSysParamHandler->nSendTimeout * 1000), 0);
		m_pNetSocket->SetOptions(RECVTIME, (m_pSysParamHandler->nRecvTimeout * 1000), 0);

		m_bExitFlag = false;

		if (false == m_DfuBuinessThread.Start(send_dfu_msg_proc, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]start dfu send thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartRecordApciHandler]start dfu send thread succeed！");

		if (false == m_DfuRecvThread.Start(recv_dfu_msg_proc, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]start dfu recv thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartRecordApciHandler]start dfu recv thread succeed！");
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "StartRecordApciHandler find exception！");
		return false;
	}

	return true;
}

bool CRecordAPCIHandler::StopRecordApciHandler()
{
	m_bExitFlag = true;

	if (NULL != m_pNetSocket)
	{
		m_pNetSocket->close();
	}

	m_DfuRecvThread.Stop();
	m_DfuBuinessThread.Stop();

	return true;
}

int CRecordAPCIHandler::Send_dfu_msg_operation_loop()
{
	time_t tCur;
	time(&tCur);
	DFU_COMMU_MSG sendMsg;
	bool bTestSend = false;
	bool bQueryNewFile = false;

	while (!m_bExitFlag)
	{
		try
		{
			if (true == m_bExitFlag)
			{
				break;
			}

			time(&tCur);//get cur time
			if ((tCur - m_tLinkActive) >= m_pDfuCommuParamHandler->nIdleTime)//test frame
			{
				if (bTestSend == false)
				{
					LaunchLinkTest();//send test msg
					bTestSend = true;
				}
				time(&m_tLinkActive);
			}

			sendMsg.clear();
			if (true == GetSendMsgFromList(sendMsg))
			{
				SendMessage(sendMsg);//send msg
			}
			else
			{
				MySleep(200);
			}
		}
		catch (...)
		{
			m_LogFile.FormatAdd(CLogFile::error, "DfuCommuOperationLoop exit exception！");
			return -1;
		}
	}

	return 0;
}

int CRecordAPCIHandler::Recv_dfu_msg_operation_loop()
{
	DFU_COMMU_MSG pRecvMsg;
	int nRecvRet(0);
	int nSendRet(0);
	CDFUMsgAttach resultMsgAttach;

	try
	{
		while (!m_bExitFlag)
		{
			if (true == m_bExitFlag)
			{
				break;
			}

			pRecvMsg.clear();
			nRecvRet = ReceiveMsg(pRecvMsg);//接收

			if (nRecvRet >= 20)
			{
				resultMsgAttach.Attach(&pRecvMsg);
				
				if ((resultMsgAttach.GetMsgdirection() == 0) && 
					(resultMsgAttach.GetMsgCommand() == 1))//测试报文
				{
					DFU_COMMU_MSG reply_msg;
					CreateTestReplyMsg(reply_msg, pRecvMsg);
					nSendRet = SendMessage(reply_msg);
				}
				else if (resultMsgAttach.GetMsgEndFlag() == false)//还有后续帧
				{
					
				}
				else
				{
					m_pDfuResultCallBackFunc(pRecvMsg, m_pResultProcessClassHandle);
				}
			}
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "DfuRecvOperationLoop exit abormal！");
		return -1;
	}
	
	return 0;
}

int CRecordAPCIHandler::PushMsgToDfu(DFU_COMMU_MSG& command)
{
	CLockUp lockup(&m_LockCommandList);

	m_command_msg_buf.push_back(command);
	
	return 1;
}

int CRecordAPCIHandler::ReceiveMsg(DFU_COMMU_MSG& pMsg)
{
	CDFUMsgAttach dfuMsgParser;
	dfuMsgParser.Attach(&pMsg);

	int nRet(0);
	int iTryTime(0);

	while(1)
	{
		pMsg.resize(2);
		nRet = m_pNetSocket->read((char*)&pMsg[0], 2);//收取启动码
		if (nRet <= 0)
		{
			m_LogFile.FormatAdd(CLogFile::error, "recv start code msg failed，return：%d！", 
				nRet);
			return (0 == nRet)?-1:nRet;
		}

		iTryTime++;

		if (false == dfuMsgParser.CheckStartMask())
		{
			m_LogFile.FormatAdd(CLogFile::error, "recv wrong start code msg，continue recv！");
			if (iTryTime > 3)
			{
				return -1;
			}
		}
		else
		{
			pMsg.resize(4);
			nRet = m_pNetSocket->read((char*)&pMsg[2], 2);//事务识别码
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv trans code msg failed！");
				return (0 == nRet)?-1:nRet;
			}

			pMsg.resize(6);
			nRet = m_pNetSocket->read((char*)&pMsg[4], 2);//协议码
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv protocol code msg failed！");
				return (0 == nRet)?-1:nRet;
			}

// 			if (false == dfuMsgParser.CheckMsgProtocolMask())
// 			{
// 				m_LogFile.FormatAdd(CLogFile::error, "recv wrong protocol code msg！");
// 			}

			pMsg.resize(8);
			nRet = m_pNetSocket->read((char*)&pMsg[6], 2);//备用
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv reserve code msg failed！");
				return (0 == nRet)?-1:nRet;
			}

			pMsg.resize(10);
			nRet = m_pNetSocket->read((char*)&pMsg[8], 2);//报文长度
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv msg length code failed！");
				return (0 == nRet)?-1:nRet;
			}

			int nMsgLen = dfuMsgParser.GetMsgLength();
			pMsg.resize(pMsg.size() + nMsgLen + 2);
			nRet = m_pNetSocket->read((char*)&pMsg[10], 2);//功能码
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv fun mask failed！");
				return (0 == nRet)?-1:nRet;
			}

			nRet = m_pNetSocket->read((char*)&pMsg[12], 2);//命令码
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv command mask failed！");
				return (0 == nRet)?-1:nRet;
			}

			nRet = m_pNetSocket->read((char*)&pMsg[14], 4);//帧序号
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv frame seq failed！");
				return (0 == nRet)?-1:nRet;
			}

			if (nMsgLen > 8)
			{
				nRet = m_pNetSocket->read((char*)&pMsg[18], (nMsgLen - 8));
				if (nRet <= 0)
				{
					m_LogFile.FormatAdd(CLogFile::error, "recv msg body failed！");
					return (0 == nRet)?-1:nRet;
				}
			}

			nRet = m_pNetSocket->read((char*)&pMsg[pMsg.size() - 2], 2);
			if (nRet <= 0)
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv end mask failed！");
				return (0 == nRet)?-1:nRet;
			}

			if (false == dfuMsgParser.CheckEndMask())
			{
				m_LogFile.FormatAdd(CLogFile::error, "recv wrong end mask msg！");
			}
			else
			{
				break;
			}
		}
	}

	nRet = pMsg.size();

	LOG_BUFFER_HEAD recv_log_header;
	bzero(&recv_log_header, sizeof(LOG_BUFFER_HEAD));
	recv_log_header.nWay = MSG_RECV;
	m_pNetSocket->GetOpponentAddr(&recv_log_header.addr);
	Write_message_log(pMsg, recv_log_header);

	time(&m_tLinkActive);
	
	return nRet;
}

int CRecordAPCIHandler::SendMessage(DFU_COMMU_MSG& record_msg)
{
	int nRet(0);
	int nWriteRet(0);
	int nMsgSendLen = 0;
	nMsgSendLen = record_msg.size();

	if (nMsgSendLen <=0 )
	{
		m_LogFile.FormatAdd(CLogFile::error, "[SendDfuMessage]send msg length is wrong（%d）", 
			nMsgSendLen);
		return -2;
	}

	try
	{
		CLockUp lockup(&m_LockSocketSend);
		{
			nWriteRet = m_pNetSocket->write((char*)&record_msg[0], nMsgSendLen);
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[SendDfuMessage]write msg find exception！");
		nRet = -1;
		return nRet;
	}

	if (nWriteRet < nMsgSendLen)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[SendDfuMessage]send msg failed，return：%d", 
			nWriteRet);
		nRet = -1;
	}

	if (nWriteRet > 0)
	{
		LOG_BUFFER_HEAD send_log_header;
		bzero(&send_log_header, sizeof(LOG_BUFFER_HEAD));
		send_log_header.nWay = MSG_SEND;
		m_pNetSocket->GetOpponentAddr(&send_log_header.addr);

		Write_message_log(record_msg, send_log_header);
	}

	time(&m_tLinkActive);

	return nRet;
}

void CRecordAPCIHandler::Write_message_log(const DFU_COMMU_MSG& pMsg, const LOG_BUFFER_HEAD& pHead)
{	
	string strLog = "";
	CDFUMsgAttach msg_parser;
	char chtemp[10]="";
	int    nHeadLen  = 0;
	int nMsgLen = pMsg.size();

	strLog.resize(nMsgLen*3 + sizeof(LOG_BUFFER_HEAD) + 30);
	
	char * pchar = (char *)&strLog[0];
	
	// print log head
	switch(pHead.nWay)
	{
	case MSG_RECV:
		{
			sprintf(pchar,"recv from %s(%d) \r\n", 
				pHead.addr.IpAddr, pHead.addr.nPort);

			nHeadLen += strlen(pchar);
			pchar    += strlen(pchar);
		}
		break;

	case MSG_SEND:
		{
			sprintf(pchar,"send to %s(%d) \r\n", 
				pHead.addr.IpAddr,pHead.addr.nPort);

			nHeadLen += strlen(pchar);
			pchar    += strlen(pchar);
		}
		break;

	default:
		break;
	}
	
    
	// printf log body
	try
	{
		char temp[3] = "" ,temp1[2]="";
		for (int i = 0; i < nMsgLen; ++i)
		{
			if (pMsg[i] < 16)
			{
				recordman_itoa(pMsg[i], temp1, 16);
				temp[1] = temp1[0];
				temp[0] = '0';
				memset(temp1, 0, 1);
			}
			else
			{
				recordman_itoa(pMsg[i], temp, 16);
			}

			memcpy(pchar,temp,2);
			pchar += 2;
			memcpy(pchar," ",1);
			pchar += 1;
		}
		// add enter
		memcpy(pchar, "\r\n", 2);
		
		m_LogFile.FormatAdd(CLogFile::trace, "%s", strLog.c_str());
	}
	catch(...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "LogMessage exception！");
		return;
	}		
}

UINT CRecordAPCIHandler::Create_link_transmask()
{
	m_utTransMask++;
	m_utTransMask = (m_utTransMask < 65536)?m_utTransMask:(m_utTransMask/65535);
	return m_utTransMask;
}

bool CRecordAPCIHandler::LaunchLinkTest()
{
	DFU_COMMU_MSG test_msg;
	int nTransMask = Create_link_transmask();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&test_msg);

	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(nTransMask);
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(RECORD_COMMAND_CHAR_PATROL_VAR);
	msg_parser.SetMsgEndFlag(true);
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();

	PushMsgToDfu(test_msg);

	return true;
}

bool CRecordAPCIHandler::LaunchQueryNewFile()
{
	DFU_COMMU_MSG newfile_msg;
	int nTransMask = Create_link_transmask();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&newfile_msg);

	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(nTransMask);
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(RECORD_COMMAND_CHAR_NEW_OSC_QUERY_VAR);
	msg_parser.SetMsgEndFlag(true);
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();

	PushMsgToDfu(newfile_msg);

	return true;
}

bool CRecordAPCIHandler::LaunchReadNewFile(UINT& uIndex)
{
	DFU_COMMU_MSG newfile_msg;
	int nTransMask = Create_link_transmask();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&newfile_msg);

	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(nTransMask);
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(RECORD_COMMAND_CHAR_OSC_FILE_READ_VAR);
	msg_parser.SetMsgEndFlag(true);
	msg_parser.SetFileIndex(uIndex);
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();

	PushMsgToDfu(newfile_msg);

	return true;
}

bool CRecordAPCIHandler::LaunchManualFile()
{
	DFU_COMMU_MSG manual_file_msg;
	int nTransMask = Create_link_transmask();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&manual_file_msg);

	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(nTransMask);
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(RECORD_COMMAND_CHAR_MANUAL_OSC_VAR);
	msg_parser.SetMsgEndFlag(true);
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();

	PushMsgToDfu(manual_file_msg);

	return true;
}

void CRecordAPCIHandler::CreateTestReplyMsg(DFU_COMMU_MSG& reply_msg, DFU_COMMU_MSG recv_msg)
{
	CDFUMsgAttach resultMsgAttach;
	CDFUMsgAttach test_msg_parser;
	reply_msg.clear();

	test_msg_parser.Attach(&reply_msg);
	resultMsgAttach.Attach(&recv_msg);

	test_msg_parser.SetMsgStartMask();
	test_msg_parser.SetMsgTransMask(resultMsgAttach.GetMsgTransMask());
	test_msg_parser.SetMsgProtocolMask();
	test_msg_parser.SetMsgReserve();
	test_msg_parser.SetMsgFuncMask(0, 1, 0);
	test_msg_parser.SetMsgCommand(resultMsgAttach.GetMsgCommand());
	test_msg_parser.SetMsgEndFlag(true);
	test_msg_parser.SetMsgLength();
	test_msg_parser.SetEndMask();
}
