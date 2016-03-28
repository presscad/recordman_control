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

//recv from dfu thread
THREAD_FUNC WINAPI DFU_APCI_RECV_THREAD_PROC(LPVOID param)
{
	CRecordAPCIHandler* pRecordApciHandler = (CRecordAPCIHandler*)param;

	try
	{
		if (NULL == pRecordApciHandler)
		{
			return THREAD_RETURN;
		}

		pRecordApciHandler->DfuRecvOperationLoop();
	}
	catch (...)
	{
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
m_LockSocketSend("LOCK_SOCKET_SEND"),
m_LockCommandList("LOCK_COMMAND_LIST")
{
	m_bExitFlag = true;
	m_pCollectorSysParam = NULL;
	m_pDfuCommuParam = NULL;
	m_pNetSocket = NULL;
	m_pMongoParam = NULL;
	m_pJsonResultFun = NULL;
	m_pJsonResultHandle = NULL;
	m_utTransMask = 0;
	time(&m_tLinkActive);
	time(&m_tCheckFile);
	m_command_msg_list.clear();
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

//set system param
void CRecordAPCIHandler::SetCollectorSysParam(COLLECTOR_DATA_SYS_PARAM* pParam, COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParam)
{
	m_pCollectorSysParam = pParam;
	m_pDfuCommuParam = pDfuCommuParam;
}

//set mongodb access param
void CRecordAPCIHandler::SetMongoAccessParam(RECORD_MONGO_BASIC_PARAM* pMongoParam)
{
	m_pMongoParam = pMongoParam;
}

//result json fun
void CRecordAPCIHandler::RegisterJsonResultFunc(PJSONRESULTFUNCALLBACK pFunc, XJHANDLE pObj)
{
	m_pJsonResultFun = pFunc;
	m_pJsonResultHandle = pObj;
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

//get send msg
bool CRecordAPCIHandler::GetDfuSendMsg(DFU_COMMU_MSG& sendmsg)
{
	CLockUp lockup(&m_LockCommandList);

	if (m_command_msg_list.size() <= 0)
	{
		return false;
	}

	dfumsg_list::iterator it = m_command_msg_list.begin();
	for (; it != m_command_msg_list.end(); it++)
	{
		if (it->bProcessed == true)
		{
			return false;
		}
	}

	it = m_command_msg_list.begin();
	it->bProcessed = true;
	sendmsg = it->command_msg.front();

	return true;
}

//add result msg
bool CRecordAPCIHandler::AddDfuResultMsg(DFU_COMMU_MSG& recv_msg)
{
	CLockUp lockup(&m_LockCommandList);

	CDFUMsgAttach recv_msg_attach;
	recv_msg_attach.Attach(&recv_msg);

	dfumsg_list::iterator it = m_command_msg_list.begin();
	for (; it != m_command_msg_list.end(); it++)
	{
		if ((it->nTransMask == recv_msg_attach.GetMsgTransMask()) && 
			(it->nDfuCommandID == recv_msg_attach.GetMsgCommand()))
		{
			it->bRecvEnd = recv_msg_attach.GetMsgEndFlag();
			it->result_msg.push_back(recv_msg);
			it->nCommandProcessResult = recv_msg_attach.GetMsgErrorFlag();
			return true;
		}
	}

	return false;
}

//check command is process over
bool CRecordAPCIHandler::CheckCommandOver(DFUMESSAGE& full_command_msg)
{
	CLockUp lockup(&m_LockCommandList);

	dfumsg_list::iterator it = m_command_msg_list.begin();
	for (; it != m_command_msg_list.end(); it++)
	{
		if (it->bRecvEnd == true)
		{
			full_command_msg = *it;
			m_command_msg_list.erase(it);
			return true;
		}
	}

	return false;
}

//get follow-up msg
bool CRecordAPCIHandler::GetFollowUpMsg(int nMsgTrans, int nCommandID, DFU_COMMU_MSG& follow_up_msg)
{
	CLockUp lockup(&m_LockCommandList);

	dfumsg_list::iterator it = m_command_msg_list.begin();
	for (; it != m_command_msg_list.end(); it++)
	{
		if ((it->nTransMask == nMsgTrans) && 
			(it->nDfuCommandID == nCommandID))
		{
			if (it->command_msg.size() > 0)
			{
				follow_up_msg = it->command_msg.front();
				it->command_msg.erase(it->command_msg.begin());
				return true;
			}
		}
	}

	return false;
}

//init
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

//start 
bool CRecordAPCIHandler::StartRecordApciHandler()
{
	bool bRet = false;

	try
	{
		m_DfuOperationThread.Stop();
		m_DfuRecvThread.Stop();

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

		if (false == m_DfuRecvThread.Start(DFU_APCI_RECV_THREAD_PROC, this))
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

//stop
bool CRecordAPCIHandler::StopRecordApciHandler()
{
	m_bExitFlag = true;

	if (NULL != m_pNetSocket)
	{
		m_pNetSocket->close();
	}

	m_DfuRecvThread.Stop();
	m_DfuOperationThread.Stop();

	return true;
}

//commu operation thread main loop
int CRecordAPCIHandler::DfuCommuOperationLoop()
{
	time_t tCur;
	time(&tCur);
	DFU_COMMU_MSG sendMsg;
	DFUMESSAGE over_msg;

	while (!m_bExitFlag)
	{
		try
		{
			if (true == m_bExitFlag)
			{
				break;
			}

			time(&tCur);
			if ((tCur - m_tLinkActive) >= m_pDfuCommuParam->nIdleTime)
			{
				LaunchLinkTest();//send test msg
				time(&m_tLinkActive);
			}

			sendMsg.clear();
			if (true == GetDfuSendMsg(sendMsg))
			{
				SendDfuMessage(sendMsg);
			}

			if (true == CheckCommandOver(over_msg))
			{
				if (over_msg.nMsgType == RECORD_DFU_MESSAGE_TYPE_JSON)
				{
					CDfuMsgToJson dfu_to_json;
					cJSON* pJsonMsg = NULL;
					dfu_to_json.Attach(&over_msg);
					dfu_to_json.DfuMsgToJson(pJsonMsg);

					if (NULL != m_pJsonResultFun)
					{
						int nRet = m_pJsonResultFun(
							over_msg.nTransMask, over_msg.nInternalCommandID, pJsonMsg, m_pJsonResultHandle);
						if (nRet != 1)
						{
							if (NULL != pJsonMsg)
							{
								cJSON_Delete(pJsonMsg);
								pJsonMsg = NULL;
							}
						}
					}
				}
				else if (over_msg.nMsgType == RECORD_DFU_MESSAGE_TYPE_OSC_LIST)
				{
				}
				else if (over_msg.nMsgType == RECORD_DFU_MESSAGE_TYPE_OSC_FILE)
				{
				}
			}

			MySleep(200);
		}
		catch (...)
		{
			m_LogFile.FormatAdd(CLogFile::error, "DfuCommuOperationLoop exit abormal！");
			return -1;
		}
	}

	return 0;
}

//recv loop
int CRecordAPCIHandler::DfuRecvOperationLoop()
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
				if (resultMsgAttach.GetMsgErrorFlag() == true)
				{
					m_LogFile.FormatAdd(CLogFile::error, 
						"msg（transmask：%d，command_id：%d） execute failed，error num：%d，error msg：%s", 
						resultMsgAttach.GetMsgTransMask(), resultMsgAttach.GetMsgCommand(), 
						resultMsgAttach.GetErrorNum(), 
						GetDfuErrorMsgString(resultMsgAttach.GetErrorNum()).c_str());
					
					AddDfuResultMsg(pRecvMsg);//错误
				}
				else if ((resultMsgAttach.GetMsgdirection() == 0) && 
					(resultMsgAttach.GetMsgCommand() == 1))//测试报文
				{
					DFU_COMMU_MSG reply_msg;
					CreateTestReplyMsg(reply_msg, pRecvMsg);
					nSendRet = SendDfuMessage(reply_msg);
				}
				else if (resultMsgAttach.GetMsgEndFlag() == false)//还有后续帧
				{
					
				}
				else if (resultMsgAttach.GetMsgMutiFrameConfirmFlag() == 1)//确认帧
				{
					DFU_COMMU_MSG follow_up_msg;
					follow_up_msg.clear();
					if (true == GetFollowUpMsg(
						resultMsgAttach.GetMsgTransMask(), resultMsgAttach.GetMsgCommand(), follow_up_msg))
					{
						nSendRet = SendDfuMessage(follow_up_msg);
					}
					else
					{
						m_LogFile.FormatAdd(CLogFile::error, 
							"[DfuRecvOperationLoop]not found msg（transmask：%d，commandid：%d） follow-up msg", 
							resultMsgAttach.GetMsgTransMask(), resultMsgAttach.GetMsgCommand());
					}
				}
				else
				{
					AddDfuResultMsg(pRecvMsg);//正常结果报文
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

//post msg
int CRecordAPCIHandler::PostDfuMsg(DFUMESSAGE& command)
{
	CLockUp lockup(&m_LockCommandList);

	CDFUMsgAttach command_attach;

	vector<DFU_COMMU_MSG>::iterator it = command.command_msg.begin();
	for (; it != command.command_msg.end(); it++)
	{
		command_attach.Attach(&*it);
		command_attach.SetMsgTransMask(command.nTransMask);
	}

	m_command_msg_list.push_back(command);

	m_LogFile.FormatAdd(CLogFile::trace, "[PostDfuMsg]收到命令加入待发送队列，当前队列大小：%d", 
		m_command_msg_list.size());
	
	return 1;
}

//recv msg from socket
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

//start test
bool CRecordAPCIHandler::LaunchLinkTest()
{
	DFUMESSAGE test_dfu_message;
	DFU_COMMU_MSG test_msg;
	int nTransMask = GetMsgTransMask();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&test_msg);
// 	msg_parser.SetMsgStartMask();
// 	msg_parser.SetMsgTransMask(nTransMask);
// 	msg_parser.SetMsgProtocolMask();
// 	msg_parser.SetMsgReserve();
// 	msg_parser.SetMsgFuncMask();
// 	msg_parser.SetMsgCommand(RECORD_COMMAND_CHAR_PATROL_VAR);
// 	msg_parser.SetMsgEndFlag(true);
// 	msg_parser.SetMsgLength();
// 	msg_parser.SetEndMask();

	msg_parser.SetMsgStartMask();
	msg_parser.SetMsgProtocolMask();
	msg_parser.SetMsgReserve();
	msg_parser.SetMsgFuncMask();
	msg_parser.SetMsgCommand(RECORD_COMMAND_CHAR_SETTING_READ_VAR);
	msg_parser.SetMsgEndFlag(true);
	msg_parser.SetMsgSettingZone(1);
	msg_parser.SetMsgSettingGroup(1);
	msg_parser.SetMsgSettingGroupIndex(0);
	msg_parser.SetMsgLength();
	msg_parser.SetEndMask();

	test_dfu_message.command_msg.push_back(test_msg);

	test_dfu_message.bProcessed = false;
	test_dfu_message.bRecvEnd = false;
	//test_dfu_message.nMsgType = RECORD_DFU_MESSAGE_TYPE_TEST;
	test_dfu_message.nMsgType = RECORD_DFU_MESSAGE_TYPE_JSON;
	test_dfu_message.nDfuCommandID = msg_parser.GetMsgCommand();
	test_dfu_message.nCommandNum = test_dfu_message.command_msg.size();
	test_dfu_message.nTransMask = nTransMask;
	PostDfuMsg(test_dfu_message);

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

// 	if (1 != SendAndRecvMessage(dfu_msg, result_msg))
// 	{
// 		return -1;
// 	}

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

//create test reply msg
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
