#include "RecordAPCIHandler.h"

//************************************
// Method:    DFU_APCI_OPERATION_THREAD_PROC
// FullName:  DFU_APCI_OPERATION_THREAD_PROC
// Access:    public 
// Returns:   THREAD_FUNC WINAPI
// Qualifier: 与dfu的业务处理线程
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
		printf("DFU_APCI_OPERATION_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

//save new file thread
THREAD_FUNC WINAPI DFU_NEWFILE_SAVE_THREAD_PROC(LPVOID param)
{
	CRecordAPCIHandler* pRecordApciHandler = (CRecordAPCIHandler*)param;

	try
	{
		if (NULL == pRecordApciHandler)
		{
			return THREAD_RETURN;
		}

		pRecordApciHandler->SaveFileOperationLoop();
	}
	catch (...)
	{
		printf("DFU_NEWFILE_SAVE_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

CRecordAPCIHandler::CRecordAPCIHandler(void):
m_LockApciMsgHandler("LOCK_MSG_PROCESS")
{
	m_bExitFlag = true;
	m_pCollectorSysParam = NULL;
	m_pDfuCommuParam = NULL;
	m_pNetSocket = NULL;
	m_pMongoParam = NULL;
	m_utTransMask = 0;
	time(&m_tLinkActive);
	time(&m_tCheckFile);
	m_file_msg_list.clear();
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

void CRecordAPCIHandler::SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam, COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParam)
{
	m_pCollectorSysParam = pParam;
	m_pDfuCommuParam = pDfuCommuParam;
}

void CRecordAPCIHandler::SetMongoAccessParam(RECORD_MONGO_BASIC_PARAM* pMongoParam)
{
	m_pMongoParam = pMongoParam;
}

//init logfile
bool CRecordAPCIHandler::InitLogFile()
{
	m_LogFile.Close();

	m_LogFile.SetLogLevel(m_pCollectorSysParam->nLoglevel);
	m_LogFile.SetLogSaveDays(m_pCollectorSysParam->nLogDays);
	m_LogFile.SetLogPath(m_pCollectorSysParam->chLogpath);

	return (TRUE == m_LogFile.Open(m_pDfuCommuParam->chDfuAddr))?true:false;
}

//************************************
// Method:    InitDfuCommuSession
// FullName:  CDfuCommuSession::InitDfuCommuSession
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 初始化
//************************************
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

		string strError = "";
		m_MongoAccessHandler.SetMongDbAccessparam(m_pMongoParam);
		if (false == m_MongoAccessHandler.ConnectMongoServer(strError))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[InitRecordApciHandler]connect mongodb failed，reason：%s", 
				strError.c_str());
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[InitRecordApciHandler]connect mongodb succeed！");
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "InitRecordApciHandler find exception！");
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
bool CRecordAPCIHandler::StartRecordApciHandler()
{
	bool bRet = false;

	try
	{
		m_DfuOperationThread.Stop();

		bRet = m_pNetSocket->ConnectServer(
			m_pDfuCommuParam->chDfuAddr, 
			m_pDfuCommuParam->nDfuport);

		if (false == bRet)
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]connect dfu server：%s port：%d failed！", 
				m_pDfuCommuParam->chDfuAddr, 
				m_pDfuCommuParam->nDfuport);

			return false;
		}

		m_LogFile.FormatAdd(CLogFile::trace, 
			"[StartRecordApciHandler]connect dfu server：%s port：%d succeed！", 
			m_pDfuCommuParam->chDfuAddr, 
			m_pDfuCommuParam->nDfuport);

		m_pNetSocket->SetOptions(SENDTIME, m_pCollectorSysParam->nSendTimeout*1000, 0);
		m_pNetSocket->SetOptions(RECVTIME, m_pCollectorSysParam->nRecvTimeout*1000, 0);

		m_bExitFlag = false;

		if (false == m_DfuOperationThread.Start(DFU_APCI_OPERATION_THREAD_PROC, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]start dfu operation thread failed！");
			return false;
		}

		m_LogFile.FormatAdd(CLogFile::trace, "[StartRecordApciHandler]start dfu operation thread succeed！");
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "StartRecordApciHandler find exception！");
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

//send command and recive result
bool CRecordAPCIHandler::ProcessJsonCommand(cJSON* pJsonCommand, cJSON*& pJsonResult)
{
	vector<RECORD_DFU_MSG> veDfuMsg;
	vector<RECORD_DFU_MSG> veResultMsg;
	CJsonMsgParser jsonMsgparser;
	veDfuMsg.clear();
	veResultMsg.clear();

	jsonMsgparser.Attach(pJsonCommand);

	if (false == jsonMsgparser.JsonToRecordDfuMsg(veDfuMsg))
	{
		return false;
	}

	if (1 != ProcessCommandMsg(veDfuMsg, veResultMsg))
	{
		return false;
	}

	pJsonResult = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonResult, "command_id", 20061);
	cJSON_AddNumberToObject(pJsonResult, "result", 1);

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
	vector<RECORD_DFU_MSG> result_msg;
	time_t tCur;
	time(&tCur);

	while (!m_bExitFlag)
	{
		try
		{
			if (m_bExitFlag)
			{
				break;
			}

			time(&tCur);
			if ((tCur - m_tLinkActive) >= m_pDfuCommuParam->nIdleTime)
			{
				ProcessPolingCommand();//send test msg
				time(&m_tLinkActive);
			}

			if ((tCur - m_tCheckFile) >= m_pDfuCommuParam->nCheckNewFileTime)
			{
				ProcessQueryNewFile();
				time(&m_tCheckFile);
			}

			MySleep(500);
		}
		catch (...)
		{
			m_LogFile.FormatAdd(CLogFile::error, "DfuCommuOperationLoop exit abormal！");
			return -1;
		}
	}

	return 0;
}

//save new osc file
int CRecordAPCIHandler::SaveFileOperationLoop()
{
	while (!m_bExitFlag)
	{
		if (true == m_bExitFlag)
		{
			break;
		}
	}

	return 0;
}

int CRecordAPCIHandler::ProcessCommandMsg(vector<RECORD_DFU_MSG>& veCommandMsg, vector<RECORD_DFU_MSG>& veResultMsg)
{
	CLockUp lockup(&m_LockApciMsgHandler);

	vector<RECORD_DFU_MSG>::iterator it = veCommandMsg.begin();
	for (; it != veCommandMsg.end(); it++)
	{
		if (0 == WriteRecordMsg(&*it))
		{
			RecvFlowMsg(veResultMsg);
		}
	}

	return 1;
}

int CRecordAPCIHandler::ProcessCommandMsg(RECORD_DFU_MSG& commandMsg, vector<RECORD_DFU_MSG>& veResultMsg)
{
	CLockUp lockup(&m_LockApciMsgHandler);

	if (0 == WriteRecordMsg(&commandMsg))
	{
		RecvFlowMsg(veResultMsg);
	}

	return 1;
}

//************************************
// 函 数 名： ReceiveMsg
// 功能概要： recv msg from net
// 访问权限： private 
// 返 回 值： int 
// 参    数： RECORD_DFU_MSG * pMsg 
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
	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteStartMask[0], 2);//收取启动码
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv start code msg failed！");
		return (0 == nRet)?-1:nRet;
	}

	if (false == dfuMsgParser.CheckStartMask())
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv wrong start code msg！");
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteTransMark[0], 2);//事务识别码
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv trans code msg failed！");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteProtocolMark[0], 2);//协议码
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv protocol code msg failed！");
		return (0 == nRet)?-1:nRet;
	}

	if (false == dfuMsgParser.CheckMsgProtocolMask())
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv wrong protocol code msg！");
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteReserve[0], 2);//备用
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv reserve code msg failed！");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteMsgLen[0], 2);//报文长度
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv msg length code failed！");
		return (0 == nRet)?-1:nRet;
	}

	int nMsgLen = dfuMsgParser.GetMsgLength();

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteFunMask[0], 2);//功能码
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv fun mask failed！");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteCommandMask[0], 2);//命令码
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv command mask failed！");
		return (0 == nRet)?-1:nRet;
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteFrameSeq[0], 4);//帧序号
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv frame seq failed！");
		return (0 == nRet)?-1:nRet;
	}

	if (nMsgLen > 8)
	{
		nRet = m_pNetSocket->read((char*)&pMsg->MsgBody[0], (nMsgLen - 8));
		if (nRet <= 0)
		{
			m_LogFile.FormatAdd(CLogFile::error, "recv msg body failed！");
			return (0 == nRet)?-1:nRet;
		}
	}

	nRet = m_pNetSocket->read((char*)&pMsg->DfuMsgHdr.byteEndMask[0], 2);
	if (nRet <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv end mask failed！");
		return (0 == nRet)?-1:nRet;
	}

	if (false == dfuMsgParser.CheckEndMask())
	{
		m_LogFile.FormatAdd(CLogFile::error, "recv wrong end mask msg！");
	}

	LOG_BUFFER_HEAD recv_log_header;
	bzero(&recv_log_header, sizeof(LOG_BUFFER_HEAD));
	recv_log_header.nWay = MSG_RECV;
	m_pNetSocket->GetOpponentAddr(&recv_log_header.addr);
	LogMessage(pMsg, recv_log_header);

	time(&m_tLinkActive);
	
	return 1;
}

//************************************
// 函 数 名： WriteRecordMsg
// 功能概要： send msg
// 访问权限： private 
// 返 回 值： int -1：failed other：send bytes length
// 参    数： RECORD_DFU_MSG * pMsg 
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
		m_LogFile.FormatAdd(CLogFile::error, "[WriteRecordMsg]write msg find exception！");
	}

	if (nWriteRet < nMsgSendLen)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[WriteRecordMsg]send msg failed，return：%d", 
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

	time(&m_tLinkActive);

	return 0;
}

//flow msg
int CRecordAPCIHandler::RecvFlowMsg(vector<RECORD_DFU_MSG>& veResultMsg)
{
	CLockUp lockup(&m_LockApciMsgHandler);

	bool bEnd = false;
	RECORD_DFU_MSG resultMsg;
	CDfuMsgParser dfu_parser;

	while (!bEnd)
	{
		bzero(&resultMsg, sizeof(RECORD_DFU_MSG));

		ReceiveMsg(&resultMsg);
		dfu_parser.Attach(&resultMsg);

		if (dfu_parser.GetMsgErrorFlag() == false)
		{
			if (dfu_parser.GetMsgdirection() != 1)
			{
				dfu_parser.SetMsgDirection(1);
				WriteRecordMsg(&resultMsg);
				continue;
			}
		}

		bEnd = dfu_parser.GetMsgEndFlag();
		dfu_parser.GetMsgErrorFlag();

		veResultMsg.push_back(resultMsg);
	}

	return 1;
}

//************************************
// 函 数 名： LogMessage
// 功能概要： print log to screen
// 访问权限： private 
// 返 回 值： void 
// 参    数： const RECORD_DFU_MSG * pMsg msg struct
// 参    数： const LOG_BUFFER_HEAD & pHead log header
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
		
		m_LogFile.FormatAdd(CLogFile::trace, "%s", strLog.c_str());
	}
	catch(...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "LogMessage exception！");
		return;
	}		
}

//************************************
// 函 数 名： CopyMessageToString
// 功能概要： copy message to string buffer
// 访问权限： private 
// 返 回 值： void 
// 参    数： BYTE bMsg byte msg
// 参    数： char * & pChar string buffer
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

//get msg trans mask
UINT CRecordAPCIHandler::GetMsgTransMask()
{
	m_utTransMask = (m_utTransMask < 99)?m_utTransMask:(m_utTransMask/99);
	return m_utTransMask;
}

bool CRecordAPCIHandler::ProcessPolingCommand()
{
	RECORD_DFU_MSG dfu_msg;
	vector<RECORD_DFU_MSG> result_msg;
	bzero(&dfu_msg, sizeof(RECORD_DFU_MSG));
	result_msg.clear();

	CDfuMsgParser msg_parser;
	msg_parser.Attach(&dfu_msg);
	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(GetMsgTransMask());
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgLength(8);
	msg_parser.SetMsgCommand(0x01);
	msg_parser.SetEndMask();
	msg_parser.SetMsgEndFlag(true);

	if (1 != ProcessCommandMsg(dfu_msg, result_msg))
	{
		return false;
	}

	return true;
}

//process query new file
int CRecordAPCIHandler::ProcessQueryNewFile()
{
	int nFileIndex = -1;
	RECORD_DFU_MSG dfu_msg;
	vector<RECORD_DFU_MSG> result_msg;
	bzero(&dfu_msg, sizeof(RECORD_DFU_MSG));
	result_msg.clear();

	CDfuMsgParser msg_parser;
	msg_parser.Attach(&dfu_msg);

	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(GetMsgTransMask());
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgLength(8);
	msg_parser.SetMsgCommand(0x31);
	msg_parser.SetEndMask();
	msg_parser.SetMsgEndFlag(true);

	if (1 != ProcessCommandMsg(dfu_msg, result_msg))
	{
		return -1;
	}

	vector<RECORD_DFU_MSG>::iterator it = result_msg.begin();
	for (; it != result_msg.end(); it++)
	{
		msg_parser.Attach(&*it);
		if (msg_parser.GetFileNum() <= 0)
		{
			nFileIndex = -1;
		}
		else
		{
			nFileIndex = 1;
		}
	}

	return nFileIndex;
}

//get osc file
void CRecordAPCIHandler::ProcessGetOscFile(int nFileIndex)
{
	RECORD_DFU_MSG dfu_msg;
	CDfuMsgParser msg_parser;
	bzero(&dfu_msg, sizeof(RECORD_DFU_MSG));

	msg_parser.Attach(&dfu_msg);
	BYTE* pMsgBody = msg_parser.GetMsgBody();
	
	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(GetMsgTransMask());
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgLength(9);
	msg_parser.SetMsgCommand(0x33);
	pMsgBody[0] = 0x00;
	msg_parser.SetEndMask();
	msg_parser.SetMsgEndFlag(true);
}
