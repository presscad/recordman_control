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
// Method:    StartRecordApciHandler
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
	vector<DFU_COMMU_MSG> veDfuMsg;
	vector<DFU_COMMU_MSG> veResultMsg;
	CJsonMsgParser jsonMsgparser;
	veDfuMsg.clear();
	veResultMsg.clear();

	jsonMsgparser.Attach(pJsonCommand);

	if (false == jsonMsgparser.JsonToRecordDfuMsg(veDfuMsg))
	{
		return false;
	}

	if (1 != SendAndRecvMessage(veDfuMsg, veResultMsg))
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
	vector<DFU_COMMU_MSG> result_msg;
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
				LaunchLinkTest();//send test msg
				time(&m_tLinkActive);
			}

			if ((tCur - m_tCheckFile) >= m_pDfuCommuParam->nCheckNewFileTime)
			{
				//LaunchQueryNewFile();
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

int CRecordAPCIHandler::SendAndRecvMessage(vector<DFU_COMMU_MSG>& veCommandMsg, vector<DFU_COMMU_MSG>& veResultMsg)
{
	CLockUp lockup(&m_LockApciMsgHandler);

	int nSendRet(0);
	int nRecvRet(0);

	vector<DFU_COMMU_MSG>::iterator it = veCommandMsg.begin();
	for (; it != veCommandMsg.end(); it++)
	{
		nSendRet = SendDfuMessage(*it);
		if (0 != nSendRet)
		{
			return (-2 == nSendRet)?-1:-2;
		}

		DFU_COMMU_MSG recvMsg;
		recvMsg.clear();

		nRecvRet = ReceiveMsg(recvMsg);
	}

	return 1;
}

int CRecordAPCIHandler::SendAndRecvMessage(DFU_COMMU_MSG& commandMsg, vector<DFU_COMMU_MSG>& veResultMsg)
{
	CLockUp lockup(&m_LockApciMsgHandler);

	DFU_COMMU_MSG recvMsg;
	CDFUMsgAttach sendMsgAttach;
	CDFUMsgAttach resultMsgAttach;
	int nSendRet(0);
	int nRecvRet(0);
	bool bEnd = false;
	sendMsgAttach.Attach(&commandMsg);

	nSendRet = SendDfuMessage(commandMsg);
	if (0 != nSendRet)
	{
		return (-2 == nSendRet)?-1:-2;
	}

	while (!bEnd)
	{
		recvMsg.clear();
		nRecvRet = ReceiveMsg(recvMsg);
		if (-1 == nRecvRet)
		{
			m_LogFile.FormatAdd(CLogFile::error, "[SendAndRecvMessage-s]recv msg error！");
			return -2;
		}
		else if (-2 == nRecvRet)
		{
			m_LogFile.FormatAdd(CLogFile::error, "[SendAndRecvMessage-s]recv msg timeout！");
		}
		else
		{
			resultMsgAttach.Attach(&recvMsg);
			if (resultMsgAttach.GetMsgdirection() == 0)//收到命令报文
			{
				DFU_COMMU_MSG test_msg;
				test_msg.clear();

				CDFUMsgAttach test_msg_parser;
				test_msg_parser.Attach(&test_msg);
				test_msg_parser.SetMsgStartMask();
				test_msg_parser.SetMsgTransMask(resultMsgAttach.GetMsgTransMask());
				test_msg_parser.SetMsgProtocolMask();
				test_msg_parser.SetMsgReserve();
				test_msg_parser.SetMsgFuncMask(0, 1, 0);
				test_msg_parser.SetMsgCommand(resultMsgAttach.GetMsgCommand());
				test_msg_parser.SetMsgEndFlag(true);
				test_msg_parser.SetMsgLength();
				test_msg_parser.SetEndMask();

				SendDfuMessage(test_msg);//响应测试报文

				continue;
			}

			if (sendMsgAttach.GetMsgTransMask() != resultMsgAttach.GetMsgTransMask())
			{
				m_LogFile.FormatAdd(CLogFile::error, "[SendAndRecvMessage-s]recv wrong transmark msg：%d", 
					sendMsgAttach.GetMsgTransMask());
				continue;
			}

			if (sendMsgAttach.GetMsgCommand() != resultMsgAttach.GetMsgCommand())
			{
				m_LogFile.FormatAdd(CLogFile::error, "[SendAndRecvMessage-s]recv wrong command result msg：%d", 
					sendMsgAttach.GetMsgCommand());
				continue;
			}

			bEnd = resultMsgAttach.GetMsgEndFlag();

			veResultMsg.push_back(recvMsg);
		}
	}

	return 0;
}

//************************************
// 函 数 名： ReceiveMsg
// 功能概要： recv msg from net
// 访问权限： private 
// 返 回 值： int 
// 参    数： RECORD_DFUMSG& pMsg 
//************************************
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

	LOG_BUFFER_HEAD recv_log_header;
	bzero(&recv_log_header, sizeof(LOG_BUFFER_HEAD));
	recv_log_header.nWay = MSG_RECV;
	m_pNetSocket->GetOpponentAddr(&recv_log_header.addr);
	WriteMsgLog(pMsg, recv_log_header);

	time(&m_tLinkActive);
	
	return nRet;
}

//************************************
// 函 数 名： SendDfuMessage
// 功能概要： send msg
// 访问权限： private 
// 返 回 值： int -1：failed other：send bytes length
// 参    数： RECORD_DFUMSG& record_msg 
//************************************
int CRecordAPCIHandler::SendDfuMessage(DFU_COMMU_MSG& record_msg)
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
		nWriteRet = m_pNetSocket->write((char*)&record_msg[0], nMsgSendLen);
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

		WriteMsgLog(record_msg, send_log_header);
	}

	time(&m_tLinkActive);

	return nRet;
}

//************************************
// 函 数 名： LogMessage
// 功能概要： print log to screen
// 访问权限： private 
// 返 回 值： void 
// 参    数： const RECORD_DFU_MSG& pMsg msg struct
// 参    数： const LOG_BUFFER_HEAD & pHead log header
//************************************
void CRecordAPCIHandler::WriteMsgLog(const DFU_COMMU_MSG& pMsg, const LOG_BUFFER_HEAD& pHead)
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
				Record_Itoa(pMsg[i], temp1, 16);
				temp[1] = temp1[0];
				temp[0] = '0';
				memset(temp1, 0, 1);
			}
			else
			{
				Record_Itoa(pMsg[i], temp, 16);
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

//get msg trans mask
UINT CRecordAPCIHandler::GetMsgTransMask()
{
	m_utTransMask++;
	m_utTransMask = (m_utTransMask < 65536)?m_utTransMask:(m_utTransMask/65535);
	return m_utTransMask;
}

bool CRecordAPCIHandler::LaunchLinkTest()
{
	DFU_COMMU_MSG dfu_msg;
	vector<DFU_COMMU_MSG> result_msg;
	dfu_msg.clear();
	result_msg.clear();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&dfu_msg);
	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(GetMsgTransMask());
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(0x01);
	msg_parser.SetMsgEndFlag(true);
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();

	if (0 != SendAndRecvMessage(dfu_msg, result_msg))
	{
		return false;
	}

	return true;
}

//process query new file
int CRecordAPCIHandler::LaunchQueryNewFile()
{
	int nFileIndex = -1;
	DFU_COMMU_MSG dfu_msg;
	vector<DFU_COMMU_MSG> result_msg;
	dfu_msg.clear();
	result_msg.clear();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&dfu_msg);

	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(GetMsgTransMask());
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(0x31);
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();
	msg_parser.SetMsgEndFlag(true);

	if (1 != SendAndRecvMessage(dfu_msg, result_msg))
	{
		return -1;
	}

	vector<DFU_COMMU_MSG>::iterator it = result_msg.begin();
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
	DFU_COMMU_MSG dfu_msg;
	CDFUMsgAttach msg_parser;
	dfu_msg.clear();

	msg_parser.Attach(&dfu_msg);
	//BYTE* pMsgBody = msg_parser.GetMsgBody();
	
	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgTransMask(GetMsgTransMask());
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(0x33);
	//pMsgBody[0] = 0x00;
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();
	msg_parser.SetMsgEndFlag(true);
}
