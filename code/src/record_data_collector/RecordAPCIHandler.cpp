#include "RecordAPCIHandler.h"

//************************************
// Method:    DFU_BUINESS_PROCESS_THREAD_PROC
// FullName:  DFU_BUINESS_PROCESS_THREAD_PROC
// Access:    public 
// Returns:   THREAD_FUNC WINAPI
// Qualifier: 与dfu的业务处理线程
// Parameter: LPVOID param
//************************************
THREAD_FUNC WINAPI DFU_BUINESS_PROCESS_THREAD_PROC(LPVOID param)
{
	CRecordAPCIHandler* pRecordApciHandler = (CRecordAPCIHandler*)param;

	try
	{
		if (NULL == pRecordApciHandler)
		{
			return THREAD_RETURN;
		}

		pRecordApciHandler->DfuBuinessProcessLoop();
	}
	catch (...)
	{
		printf("DFU_APCI_OPERATION_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

//recv from dfu thread
THREAD_FUNC WINAPI DFU_MSG_RECV_THREAD_PROC(LPVOID param)
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
THREAD_FUNC WINAPI FILE_BUINESS_THREAD_PROC(LPVOID param)
{
	CRecordAPCIHandler* pRecordApciHandler = (CRecordAPCIHandler*)param;

	try
	{
		if (NULL == pRecordApciHandler)
		{
			return THREAD_RETURN;
		}

		pRecordApciHandler->FileBuinessProcessLoop();
	}
	catch (...)
	{
		printf("FILE_BUINESS_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

CRecordAPCIHandler::CRecordAPCIHandler(void):
m_LockSocketSend("LOCK_SOCKET_SEND"),
m_LockCommandList("LOCK_COMMAND_LIST"),
m_LockFileList("LOCK_FILE_LIST")
{
	m_bExitFlag = true;
	m_pConfigHandle = NULL;
	m_pNetSocket = NULL;
	m_pResultCallBackFunc = NULL;
	m_pResultProcessClassHandle = NULL;
	m_utTransMask = 0;
	time(&m_tLinkActive);
	time(&m_tCheckFile);

	m_command_msg_list.clear();
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

void CRecordAPCIHandler::SetConfigVariableMgrHandle(CConfigVariableMgr* pConfigHandle)
{
	m_pConfigHandle = pConfigHandle;
}

//result json fun
void CRecordAPCIHandler::RegisterResultCallBackFunc(PRESULTMSGCALLBACKFUNC pFunc, XJHANDLE pObj)
{
	m_pResultCallBackFunc = pFunc;
	m_pResultProcessClassHandle = pObj;
}

//init logfile
bool CRecordAPCIHandler::InitLogFile()
{
	m_LogFile.Close();

	m_LogFile.SetLogLevel(m_pConfigHandle->GetSysParam_LogLevel());
	m_LogFile.SetLogSaveDays(m_pConfigHandle->GetSysParam_LogDays());
	m_LogFile.SetLogPath(m_pConfigHandle->GetSysParam_LogPath());

	return (TRUE == m_LogFile.Open(m_pConfigHandle->GetDfuCommuParam_Addr(true)))?true:false;
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
	time(&it->tSendTime);
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

	m_LogFile.FormatAdd(CLogFile::error, 
		"[AddDfuResultMsg]can not find result belong struct，drop（trans_mask：%d，command_id：%d）！", 
		recv_msg_attach.GetMsgTransMask(), recv_msg_attach.GetMsgCommand());

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

//process client result msg
bool CRecordAPCIHandler::ProcessClientResultMsg(DFUMESSAGE& client_result_msg)
{
	CDfuMsgToJson dfu_to_json;
	cJSON* pJsonMsg = NULL;
	dfu_to_json.Attach(&client_result_msg);
	dfu_to_json.DfuMsgToJson(pJsonMsg);

	if (NULL != m_pResultCallBackFunc)
	{
		int nRet = m_pResultCallBackFunc(
			client_result_msg.nTransMask, 
			client_result_msg.nInternalCommandID, 
			pJsonMsg, 
			m_pResultProcessClassHandle);

		if (nRet != 1)
		{
			if (NULL != pJsonMsg)
			{
				cJSON_Delete(pJsonMsg);
				pJsonMsg = NULL;
			}
		}
	}

	return true;
}

//query file list msg
bool CRecordAPCIHandler::PorocessFListResultMsg(DFUMESSAGE& file_list_msg)
{
	if (file_list_msg.result_msg.size() <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[PorocessFListResultMsg]find new file list，but no result！");
		return false;
	}

	DFU_COMMU_MSG* pOneMsg = NULL;
	CDFUMsgAttach msgAttach;
	UINT uFileIndex(0);
	UINT uFileNum(0);
	int nOffset = 18;

	pOneMsg = &file_list_msg.result_msg.front();
	ConvertUint32BigedianToL(&(*pOneMsg)[nOffset], uFileIndex);
	ConvertUint32BigedianToL(&(*pOneMsg)[nOffset + 4], uFileNum);

	m_LogFile.FormatAdd(CLogFile::trace, 
		"[PorocessFListResultMsg]search new file complete，result（index：%d，num：%d）", 
		uFileIndex, uFileNum);

	if (uFileIndex > 0)
	{
		LaunchReadNewFile(uFileIndex);
	}
	else
	{
		LaunchManualFile();
	}

	return true;
}

//process file msg(analyze and save)
bool CRecordAPCIHandler::ProcessFileResultMsg(DFUMESSAGE& file_msg)
{
	if (file_msg.result_msg.size() <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[ProcessFileResultMsg]read new file finish，but no result！");
		return false;
	}

	DFU_COMMU_MSG* pOneMsg = NULL;
	CDFUMsgAttach msgAttach;
	uint16 uAnalogchannelNum(0);//模拟量通道数
	uint16 uDigitalchannelNum(0);//开关量通道数
	UINT uFilesecond(0);//录波时刻，秒
	UINT uFilenanosecond(0);//录波时刻，纳秒
	UINT uFaultsecond(0);//故障时刻，秒
	UINT uFaultnanosecond(0);//故障时刻，纳秒
	UINT uSamplesectionNum(0);//采样段数目
	UINT uDatablockNum(0);//数据块数目
	UINT uAiVal(0);//模拟通道值
	UINT uNo(0);
	UINT uDataNo(0);
	UINT uDival(0);
	int nMsgoffset = RECORD_DFU_MSG_HEADER_OFFSET;

	comtradeHead head;
	CreateComtrade com;
	list<aiChannel> aichs;//模拟量通道
	list<diChannel> dichs;//开关量通道
	list<sampleInfo> samples;//采样段
	list<list<short> > datas;//数据
	aichs.clear();
	dichs.clear();
	samples.clear();
	datas.clear();

	try
	{
		head.revYear = 1991;
		head.datType="ascii";
		head.timemult=1.0;

		GetOscInfo(head);//get osc info

		vector<DFU_COMMU_MSG>::iterator itFile = file_msg.result_msg.begin();
		for (; itFile != file_msg.result_msg.end(); itFile++)
		{
			pOneMsg = &*itFile;
			msgAttach.Attach(pOneMsg);

			ConvertUint16BigedianToL(&(*pOneMsg)[nMsgoffset], uAnalogchannelNum);//模拟通道数目
			head.aiCount += uAnalogchannelNum;
			nMsgoffset += 2;

			ConvertUint16BigedianToL(&(*pOneMsg)[nMsgoffset], uAnalogchannelNum);//开关通道数目
			head.diCount += uAnalogchannelNum;
			nMsgoffset += 2;

			ConvertFloat32BigedianToL(&(*pOneMsg)[nMsgoffset], head.primary);//模拟通道一次系数
			nMsgoffset += 4;

			ConvertFloat32BigedianToL(&(*pOneMsg)[nMsgoffset], head.secondary);//模拟通道二次系数
			nMsgoffset += 4;

			ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uFilesecond);//录波时刻（秒）
			nMsgoffset += 4;
			head.startTime = FormatDfuMsgTime(uFilesecond);

			ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uFilenanosecond);//录波时刻（纳秒）
			nMsgoffset += 4;

			ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uFaultsecond);//故障时间（秒）
			nMsgoffset += 4;
			head.faultTime = FormatDfuMsgTime(uFaultsecond);

			ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uFilenanosecond);//故障时刻（纳秒）
			nMsgoffset += 4;

			ConvertFloat32BigedianToL(&(*pOneMsg)[nMsgoffset], head.lineFreq);//线路频率
			nMsgoffset += 4;

			ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uSamplesectionNum);//采样段数目
			head.rateCount += uSamplesectionNum;
			nMsgoffset += 4;

			ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uDatablockNum);//数据块数目
			nMsgoffset += 4;

			for (uNo = 0; uNo < uSamplesectionNum; uNo++)
			{
				sampleInfo sa;

				ConvertFloat32BigedianToL(&(*pOneMsg)[nMsgoffset], sa.rate);//采样率
				nMsgoffset += 4;

				ConvertInt32BigedianToL(&(*pOneMsg)[nMsgoffset], sa.lastIndex);//此采用频率最后一个采样的序号
				nMsgoffset += 4;

				samples.push_back(sa);
			}

			for (uNo = 0; uNo < uDatablockNum; uNo++)//数据块
			{
				aiChannel ai;
				list<short> data_val;
				for (uDataNo = 0; uDataNo < uAnalogchannelNum; uDataNo++)//模拟量
				{
					ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], ai.index);//模拟通道编号
					nMsgoffset += 4;

					ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uDival);//模拟通道编号
					nMsgoffset += 4;

					aichs.push_back(ai);
					data_val.push_back(uDival);
				}

				ConvertUint32BigedianToL(&(*pOneMsg)[nMsgoffset], uDival);
				nMsgoffset += 4;//开关量
				for (uDataNo = 0; uDataNo < uAnalogchannelNum; uDataNo++)
				{
					diChannel di;

					dichs.push_back(di);
					data_val.push_back();
				}

				datas.push_back(data_val);
			}
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[ProcessFileResultMsg]find unknown exception!");
		return false;
	}

	com.setComtradeHead(head);//设置数据头
	com.setSampleInfos(samples);//采样段
	com.setAiChannels(aichs);//模拟量通道
	com.setDiChannels(dichs);//开关量通道

	com.create(m_pConfigHandle->GetDfuCommuParam_FileSavePath(), "test");

	return true;
}

//add file msg
bool CRecordAPCIHandler::AddFileResultMsg(DFUMESSAGE& file_result_msg)
{
	CLockUp lockUp(&m_LockFileList);

	m_file_msg_list.push_back(file_result_msg);

	m_LogFile.FormatAdd(CLogFile::trace, "[AddFileResultMsg]add a file msg to list，list size：%d", 
		m_file_msg_list.size());

	return true;
}

//get file save
bool CRecordAPCIHandler::GetFileResultMsg(DFUMESSAGE& file_msg)
{
	CLockUp lockUp(&m_LockFileList);

	if (m_file_msg_list.size() <= 0)
	{
		return false;
	}

	dfumsg_list::iterator it = m_file_msg_list.begin();
	file_msg = *it;
	m_file_msg_list.erase(it);

	return true;
}

//get osc info
bool CRecordAPCIHandler::GetOscInfo(comtradeHead& head)
{
	long lLen = 0;
	auto_ptr<mongo::DBClientCursor> result;
	m_MongoAccessHandler.QueryData("runstatus_info", result);
	string strFieldval = "";
	CGECodeConvert code_convert;
	char* pVal = NULL;

	while (result->more())
	{
		mongo::BSONObj obj = result->next();
		BSONForEach(e, obj)
		{
			if (strcmp(e.fieldName(), "devname") == 0)
			{
				strFieldval = e.String();
				lLen = code_convert.Utf82Gbk(strFieldval.c_str(), strFieldval.size(), pVal);
				if (lLen != -1)
				{
					head.devName = pVal;
				}
			}
			else if (strcmp(e.fieldName(), "station") == 0)
			{
				strFieldval = e.String();
				lLen = code_convert.Utf82Gbk(strFieldval.c_str(), strFieldval.size(), pVal);
				if (lLen != -1)
				{
					head.stationName = pVal;
				}
			}

			if (NULL != pVal)
			{
				delete[] pVal;
				pVal = NULL;
			}
		}
	}

	return true;
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
		m_MongoAccessHandler.SetMongDbAccessparam(m_pConfigHandle->GetMongoParamHandle());
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
		m_DfuBuinessThread.Stop();
		m_DfuRecvThread.Stop();

		bRet = m_pNetSocket->ConnectServer(
			m_pConfigHandle->GetDfuCommuParam_Addr(true), 
			m_pConfigHandle->GetDfuCommuParam_Port(true));

		if (false == bRet)
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]connect dfu server：%s port：%d failed！", 
				m_pConfigHandle->GetDfuCommuParam_Addr(true), 
				m_pConfigHandle->GetDfuCommuParam_Port(true));

			return false;
		}

		m_LogFile.FormatAdd(CLogFile::trace, 
			"[StartRecordApciHandler]connect dfu server：%s port：%d succeed！", 
			m_pConfigHandle->GetDfuCommuParam_Addr(true), 
			m_pConfigHandle->GetDfuCommuParam_Port(true));

		m_pNetSocket->SetOptions(SENDTIME, (m_pConfigHandle->GetSysParam_SendTimeOut() * 1000), 0);
		m_pNetSocket->SetOptions(RECVTIME, (m_pConfigHandle->GetSysParam_RecTimeOut() * 1000), 0);

		m_bExitFlag = false;

		if (false == m_DfuBuinessThread.Start(DFU_BUINESS_PROCESS_THREAD_PROC, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]start dfu operation thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartRecordApciHandler]start dfu operation thread succeed！");

		if (false == m_DfuRecvThread.Start(DFU_MSG_RECV_THREAD_PROC, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]start dfu recv thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartRecordApciHandler]start dfu recv thread succeed！");

		if (false == m_DfuRecvThread.Start(FILE_BUINESS_THREAD_PROC, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartRecordApciHandler]start file buiness thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartRecordApciHandler]start file buiness thread succeed！");
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
	m_DfuBuinessThread.Stop();
	m_FileBuinessThread.Stop();

	return true;
}

//commu operation thread main loop
int CRecordAPCIHandler::DfuBuinessProcessLoop()
{
	time_t tCur;
	time(&tCur);
	DFU_COMMU_MSG sendMsg;
	DFUMESSAGE over_msg;
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

			if ((tCur - m_tCheckFile) >= 
				m_pConfigHandle->GetDfuCommuParam_ChecknewfileTime(true))//query file
			{
				if (bQueryNewFile == false)
				{
					LaunchQueryNewFile();
					bQueryNewFile = true;
				}
				time(&m_tCheckFile);
			}

// 			if ((tCur - m_tLinkActive) >= m_pDfuCommuParam->nIdleTime)//test frame
// 			{
// 				if (bTestSend == false)
// 				{
// 					LaunchLinkTest();//send test msg
// 					bTestSend = true;
// 				}
// 				time(&m_tLinkActive);
// 			}

			sendMsg.clear();
			if (true == GetDfuSendMsg(sendMsg))
			{
				SendDfuMessage(sendMsg);//send msg
			}

			if (false == CheckCommandOver(over_msg))//find process over msg
			{
				MySleep(200);
				continue;
			}

			switch (over_msg.nMsgType)
			{
			case RECORD_DFU_MESSAGE_TYPE_JSON: //client result msg
				{
					ProcessClientResultMsg(over_msg);
				}
				break;

			case RECORD_DFU_MESSAGE_TYPE_OSC_LIST: //query osc list result
				{
					PorocessFListResultMsg(over_msg);
					bQueryNewFile = false;
				}
				break;

			case RECORD_DFU_MESSAGE_TYPE_OSC_FILE: //file msg
				{
					AddFileResultMsg(over_msg);
				}
				break;

			case RECORD_DFU_MESSAGE_TYPE_TEST: //test msg
				{
					bTestSend = false;
				}
				break;
			}

			MySleep(200);
		}
		catch (...)
		{
			m_LogFile.FormatAdd(CLogFile::error, "DfuCommuOperationLoop exit exception！");
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
int CRecordAPCIHandler::FileBuinessProcessLoop()
{
	bool bRet(false);

	try
	{
		while (!m_bExitFlag)
		{
			if (true == m_bExitFlag)
			{
				break;
			}

			DFUMESSAGE file_msg;
			if (GetFileResultMsg(file_msg) == false)
			{
				MySleep(200);
				continue;
			}

			bRet = ProcessFileResultMsg(file_msg);
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[FileBuinessProcessLoop]file buiness loop exception！");
		return -1;
	}

	m_LogFile.FormatAdd(CLogFile::trace, "[FileBuinessProcessLoop]file buiness exit normal！");

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
	WriteDfuMessageLog(pMsg, recv_log_header);

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

		WriteDfuMessageLog(record_msg, send_log_header);
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
void CRecordAPCIHandler::WriteDfuMessageLog(const DFU_COMMU_MSG& pMsg, const LOG_BUFFER_HEAD& pHead)
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
UINT CRecordAPCIHandler::CreateTransMask()
{
	m_utTransMask++;
	m_utTransMask = (m_utTransMask < 65536)?m_utTransMask:(m_utTransMask/65535);
	return m_utTransMask;
}

//start test
bool CRecordAPCIHandler::LaunchLinkTest()
{
	DFUMESSAGE test_msg_list;
	DFU_COMMU_MSG test_msg;
	int nTransMask = CreateTransMask();

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

	test_msg_list.command_msg.push_back(test_msg);

	test_msg_list.bProcessed = false;
	test_msg_list.bRecvEnd = false;
	test_msg_list.nMsgType = RECORD_DFU_MESSAGE_TYPE_TEST;
	//test_msg_list.nMsgType = RECORD_DFU_MESSAGE_TYPE_OSC_LIST;
	//test_msg_list.nMsgType = RECORD_DFU_MESSAGE_TYPE_JSON;
	test_msg_list.nDfuCommandID = msg_parser.GetMsgCommand();
	test_msg_list.nCommandNum = test_msg_list.command_msg.size();
	test_msg_list.nTransMask = nTransMask;
	PostDfuMsg(test_msg_list);

	return true;
}

//process query new file
bool CRecordAPCIHandler::LaunchQueryNewFile()
{
	DFUMESSAGE newfile_msg_list;
	DFU_COMMU_MSG newfile_msg;
	int nTransMask = CreateTransMask();

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

	newfile_msg_list.command_msg.push_back(newfile_msg);

	newfile_msg_list.bProcessed = false;
	newfile_msg_list.bRecvEnd = false;
	newfile_msg_list.nMsgType = RECORD_DFU_MESSAGE_TYPE_OSC_LIST;
	newfile_msg_list.nDfuCommandID = msg_parser.GetMsgCommand();
	newfile_msg_list.nCommandNum = newfile_msg_list.command_msg.size();
	newfile_msg_list.nTransMask = nTransMask;
	PostDfuMsg(newfile_msg_list);

	return true;
}

//get new file
bool CRecordAPCIHandler::LaunchReadNewFile(UINT& uIndex)
{
	DFUMESSAGE newfile_msg_list;
	DFU_COMMU_MSG newfile_msg;
	int nTransMask = CreateTransMask();

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

	newfile_msg_list.command_msg.push_back(newfile_msg);

	newfile_msg_list.bProcessed = false;
	newfile_msg_list.bRecvEnd = false;
	newfile_msg_list.nMsgType = RECORD_DFU_MESSAGE_TYPE_OSC_FILE;
	newfile_msg_list.nDfuCommandID = msg_parser.GetMsgCommand();
	newfile_msg_list.nCommandNum = newfile_msg_list.command_msg.size();
	newfile_msg_list.nTransMask = nTransMask;

	PostDfuMsg(newfile_msg_list);

	return true;
}

bool CRecordAPCIHandler::LaunchManualFile()
{
	DFUMESSAGE manual_file_msg_list;
	DFU_COMMU_MSG manual_file_msg;
	int nTransMask = CreateTransMask();

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

	manual_file_msg_list.command_msg.push_back(manual_file_msg);

	manual_file_msg_list.bProcessed = false;
	manual_file_msg_list.bRecvEnd = false;
	manual_file_msg_list.nMsgType = RECORD_DFU_MESSAGE_TYPE_JSON;
	manual_file_msg_list.nDfuCommandID = msg_parser.GetMsgCommand();
	manual_file_msg_list.nCommandNum = manual_file_msg_list.command_msg.size();
	manual_file_msg_list.nTransMask = nTransMask;
	PostDfuMsg(manual_file_msg_list);

	return true;
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
