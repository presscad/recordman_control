#include "DfuMainFlow.h"

THREAD_FUNC WINAPI MAIN_BUINESS_THREAD_PROC(LPVOID param)
{
	CDfuMainFlow* pMainFlow = (CDfuMainFlow*)param;

	try
	{
		if (NULL == pMainFlow)
		{
			return THREAD_RETURN;
		}

		pMainFlow->MainBuinessLoop();
	}
	catch (...)
	{
		printf("MAIN_BUINESS_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

THREAD_FUNC WINAPI RESULT_BUINESS_THREAD_PROC(LPVOID param)
{
	CDfuMainFlow* pMainFlow = (CDfuMainFlow*)param;

	try
	{
		if (NULL == pMainFlow)
		{
			return THREAD_RETURN;
		}

		pMainFlow->ResultBuinessLoop();
	}
	catch (...)
	{
		printf("RESULT_BUINESS_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

THREAD_FUNC WINAPI FILE_BUINESS_THREAD_PROC(LPVOID param)
{
	CDfuMainFlow* pMainFlow = (CDfuMainFlow*)param;

	try
	{
		if (NULL == pMainFlow)
		{
			return THREAD_RETURN;
		}

		pMainFlow->FileBuinessLoop();
	}
	catch (...)
	{
		printf("FILE_BUINESS_THREAD_PROC thread exit abnormal！\n");
		return THREAD_RETURN;
	}

	return THREAD_RETURN;
}

int push_dfu_result_msg_callback(DFU_COMMU_MSG& result_msg, XJHANDLE pReserved)
{
	CDfuMainFlow* pDfumaiflow = (CDfuMainFlow*)pReserved;

	pDfumaiflow->PushDfuResultMsg(result_msg);

	return 0;
}

CDfuMainFlow::CDfuMainFlow(
	COLLECTOR_DATA_SYS_PARAM* pSysParamHandler, 
	COLLECTOR_DFU_COMMU_PARAM* pDfuCommuParamHandler, 
	CMongodbAccess* pMongodbObj):
m_LockFileBuf("LOCK_FILE_BUF"),
m_LockCommandBuf("LOCK_COMMAND_BUF")
{
	m_pSysParamHandler = pSysParamHandler;
	m_pDfuCommuParamHandler = pDfuCommuParamHandler;
	m_pMongoAccessHandler = pMongodbObj;
	
	m_bExitFlag = true;
	m_bQueryNewFile = false;

	m_pApciHandler = NULL;
	m_pCommandResultCallBackFunc = NULL;
	m_pCallBackCallObj = NULL;
	m_pComtradeFileConverter = NULL;

	m_CommandMsgBuf.clear();
}


CDfuMainFlow::~CDfuMainFlow(void)
{
	m_LogFile.Close();
}

void CDfuMainFlow::RegisterCommandResultCallBack(PRESULTMSGCALLBACKFUNC pCallBack, XJHANDLE pReserved)
{
	m_pCommandResultCallBackFunc = pCallBack;
	m_pCallBackCallObj = pReserved;
}

bool CDfuMainFlow::InitLogFile()
{
	m_LogFile.Close();

	m_LogFile.SetLogLevel(m_pSysParamHandler->nLoglevel);
	m_LogFile.SetLogSaveDays(m_pSysParamHandler->nLogDays);
	m_LogFile.SetLogPath(m_pSysParamHandler->chLogpath);

	return (TRUE == m_LogFile.Open("main_flow_log"))?true:false;
}

bool CDfuMainFlow::InitMainFlow()
{
	try
	{
		if (NULL == m_pApciHandler)
		{
			m_pApciHandler = new CRecordAPCIHandler(m_pDfuCommuParamHandler, m_pSysParamHandler);
		}

		if (NULL == m_pApciHandler)
		{
			m_LogFile.FormatAdd(CLogFile::error, "[InitMainFlow]malloc memory for CRecordAPCIHandler failed！");
			return false;
		}

		if (NULL == m_pComtradeFileConverter)
		{
			m_pComtradeFileConverter = new CConvertMsgToComtradeFile(m_pMongoAccessHandler);
		}

		if (NULL == m_pComtradeFileConverter)
		{
			m_LogFile.FormatAdd(CLogFile::error, "[InitMainFlow]malloc memory for CConvertMsgToComtradeFile failed！");
			return false;
		}

		m_pApciHandler->RegisterDfuResultCallBackFunc(push_dfu_result_msg_callback, this);
		m_pApciHandler->InitRecordApciHandler();
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "");
		return false;
	}

	return true;
}

bool CDfuMainFlow::StartMainFlow()
{
	try
	{
		m_bExitFlag = false;

		if (false == m_MainBuinessThread.Start(MAIN_BUINESS_THREAD_PROC, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartMainFlow]start main buiness thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartMainFlow]start main buiness thread succeed！");

		if (false == m_ResultBuinessThread.Start(RESULT_BUINESS_THREAD_PROC, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartMainFlow]start result buiness thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartMainFlow]start result buiness thread succeed！");

		if (false == m_FileBuinessThread.Start(FILE_BUINESS_THREAD_PROC, this))
		{
			m_LogFile.FormatAdd(CLogFile::error, 
				"[StartMainFlow]start file buiness thread failed！");
			return false;
		}
		m_LogFile.FormatAdd(CLogFile::trace, "[StartMainFlow]start file buiness thread succeed！");

		m_pApciHandler->StartRecordApciHandler();
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}

bool CDfuMainFlow::StopMainFlow()
{
	try
	{
		m_bExitFlag = true;

		m_MainBuinessThread.Stop();
		m_ResultBuinessThread.Stop();
		m_FileBuinessThread.Stop();
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}

bool CDfuMainFlow::PullCommandToDfuApci(DFU_COMMU_MSG& command_msg)
{
	CLockUp lockup(&m_LockCommandBuf);

	if (m_CommandMsgBuf.size() <= 0)
	{
		return false;
	}

	DFUMESSAGE_BUF::iterator it = m_CommandMsgBuf.begin();
	for (; it != m_CommandMsgBuf.end(); it++)
	{
		if (true == it->bProcessed)
		{
			return false;
		}
		else
		{
			vector<DFU_COMMU_MSG>::iterator itCommand = it->command_msg.begin();
			command_msg = *itCommand;
			it->bProcessed = true;
			break;
		}
	}

	return true;
}

bool CDfuMainFlow::PushCommandToDfuApci(DFUMESSAGE& command_msg)
{
	CLockUp lockup(&m_LockCommandBuf);

	CDFUMsgAttach command_attach;

	vector<DFU_COMMU_MSG>::iterator it = command_msg.command_msg.begin();
	for (; it != command_msg.command_msg.end(); it++)
	{
		command_attach.Attach(&*it);
		command_attach.SetMsgTransMask(command_msg.nTransMask);
	}

	m_CommandMsgBuf.push_back(command_msg);

	m_LogFile.FormatAdd(CLogFile::trace, "[PushCommandToDfuApci]收到命令加入待发送队列，当前队列大小：%d", 
		m_CommandMsgBuf.size());

	return 1;
}

bool CDfuMainFlow::PushDfuResultMsg(DFU_COMMU_MSG& result_msg)
{
	CLockUp lockup(&m_LockCommandBuf);

	CDFUMsgAttach recv_msg_attach;
	recv_msg_attach.Attach(&result_msg);

	DFUMESSAGE_BUF::iterator it = m_CommandMsgBuf.begin();
	for (; it != m_CommandMsgBuf.end(); it++)
	{
		if ((it->nTransMask == recv_msg_attach.GetMsgTransMask()) && 
			(it->nDfuCommandID == recv_msg_attach.GetMsgCommand()))
		{
			it->bRecvEnd = recv_msg_attach.GetMsgEndFlag();
			it->result_msg.push_back(result_msg);
			it->nCommandProcessResult = recv_msg_attach.GetMsgErrorFlag();

			return true;
		}
	}

	m_LogFile.FormatAdd(CLogFile::error, 
		"[PushDfuResultMsg]can not find result belong struct，drop（trans_mask：%d，command_id：%d）！", 
		recv_msg_attach.GetMsgTransMask(), recv_msg_attach.GetMsgCommand());

	return false;
}

int CDfuMainFlow::GetMsgTransMask()
{
	return m_pApciHandler->Create_link_transmask();
}

bool CDfuMainFlow::CheckCommandFinish(DFUMESSAGE& full_command_msg)
{
	CLockUp lockup(&m_LockCommandBuf);

	DFUMESSAGE_BUF::iterator it = m_CommandMsgBuf.begin();
	for (; it != m_CommandMsgBuf.end(); it++)
	{
		if (it->bRecvEnd == true)
		{
			full_command_msg = *it;
			m_CommandMsgBuf.erase(it);
			return true;
		}
	}

	return false;
}

bool CDfuMainFlow::PushFileResultBuf(DFUMESSAGE& file_result)
{
	CLockUp lockup(&m_LockFileBuf);

	m_FileMsgBuf.push_back(file_result);

	return true;
}

bool CDfuMainFlow::PullFileFromList(DFUMESSAGE& file_result)
{
	CLockUp lockup(&m_LockFileBuf);

	if (m_FileMsgBuf.size() <= 0)
	{
		return false;
	}

	DFUMESSAGE_BUF::iterator it = m_FileMsgBuf.begin();
	file_result = *it;
	m_FileMsgBuf.erase(it);

	return true;
}

int CDfuMainFlow::MainBuinessLoop()
{
	time_t tCur;
	time(&tCur);
	time(&m_tCheckFile);
	DFU_COMMU_MSG command_msg;

	try
	{
		while (!m_bExitFlag)
		{
			if (true == m_bExitFlag)
			{
				break;
			}

			command_msg.clear();
			if (true == PullCommandToDfuApci(command_msg))//pull a command
			{
				m_pApciHandler->PushMsgToDfu(command_msg);
				MySleep(30);
				continue;
			}

			time(&tCur);
			if ((tCur - m_tCheckFile) >= m_pDfuCommuParamHandler->nCheckNewFileTime)//query file
			{
				if (m_bQueryNewFile == false)
				{
					LaunchQueryNewFile();
					m_bQueryNewFile = true;
				}
				time(&m_tCheckFile);
				continue;
			}

		}
	}
	catch (...)
	{
		return 1;
	}
	return 0;
}

int CDfuMainFlow::ResultBuinessLoop()
{
	DFUMESSAGE finish_command_msg;

	try
	{
		while (!m_bExitFlag)
		{
			if (true == m_bExitFlag)
			{
				break;
			}

			if (false == CheckCommandFinish(finish_command_msg))//find process over msg
			{
				MySleep(200);
				continue;
			}

			switch (finish_command_msg.nMsgType)
			{
			case RECORD_DFU_MESSAGE_TYPE_JSON: //client result msg
				{
					ProcessClientResultMsg(finish_command_msg);
				}
				break;

			case RECORD_DFU_MESSAGE_TYPE_OSC_LIST: //query osc list result
				{
					PorocessFListResultMsg(finish_command_msg);
				}
				break;

			case RECORD_DFU_MESSAGE_TYPE_OSC_FILE: //file msg
				{
					PushFileResultBuf(finish_command_msg);
					m_bQueryNewFile = false;
				}
				break;

			case RECORD_DFU_MESSAGE_TYPE_MANUAL_FILE:
				{
					m_bQueryNewFile = false;
					m_LogFile.FormatAdd(CLogFile::trace, "manual file %s", 
						(0 == finish_command_msg.nCommandProcessResult)?"succeed":"failed");
				}
				break;
			}
		}
	}
	catch (...)
	{
		
	}
	return 0;
}

int CDfuMainFlow::FileBuinessLoop()
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
			if (PullFileFromList(file_msg) == false)
			{
				MySleep(200);
				continue;
			}

			m_pComtradeFileConverter->AttachDfuMsg(&file_msg);
			bRet = m_pComtradeFileConverter->ConvertMsgToFile(m_pDfuCommuParamHandler->chFileSavePath);
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

bool CDfuMainFlow::ProcessClientResultMsg(DFUMESSAGE& client_result_msg)
{
	CDfuMsgToJson dfu_to_json;
	cJSON* pJsonMsg = NULL;
	dfu_to_json.Attach(&client_result_msg);
	dfu_to_json.DfuMsgToJson(pJsonMsg);

	if (NULL != m_pCommandResultCallBackFunc)
	{
		int nRet = m_pCommandResultCallBackFunc(
			client_result_msg.nTransMask, 
			client_result_msg.nInternalCommandID, 
			pJsonMsg, 
			m_pCallBackCallObj);

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

bool CDfuMainFlow::PorocessFListResultMsg(DFUMESSAGE& file_list_msg)
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
	int nOffset = RECORD_DFU_MSG_HEADER_OFFSET;

	pOneMsg = &file_list_msg.result_msg.front();
	convert_btol_uint32(&(*pOneMsg)[nOffset], uFileIndex);
	convert_btol_uint32(&(*pOneMsg)[nOffset + 4], uFileNum);

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

bool CDfuMainFlow::LaunchQueryNewFile()
{
	DFUMESSAGE new_file_msg_buf;
	DFU_COMMU_MSG newfile_msg;
	int nTransMask = m_pApciHandler->Create_link_transmask();

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

	new_file_msg_buf.nDfuCommandID = msg_parser.GetMsgCommand();
	new_file_msg_buf.nInternalCommandID = -1;
	new_file_msg_buf.nCommandNum = 1;
	new_file_msg_buf.nMsgType = RECORD_DFU_MESSAGE_TYPE_OSC_LIST;
	new_file_msg_buf.nTransMask = nTransMask;
	new_file_msg_buf.bRecvEnd = false;
	new_file_msg_buf.command_msg.push_back(newfile_msg);
	new_file_msg_buf.bProcessed = false;
	new_file_msg_buf.result_msg.clear();

	PushCommandToDfuApci(new_file_msg_buf);

	return true;
}

bool CDfuMainFlow::LaunchReadNewFile(UINT& uIndex)
{
	DFUMESSAGE file_msg_buf;
	DFU_COMMU_MSG file_msg;
	int nTransMask = m_pApciHandler->Create_link_transmask();

	CDFUMsgAttach msg_parser;
	msg_parser.Attach(&file_msg);

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

	file_msg_buf.nDfuCommandID = msg_parser.GetMsgCommand();
	file_msg_buf.nInternalCommandID = -1;
	file_msg_buf.nCommandNum = 1;
	file_msg_buf.nMsgType = RECORD_DFU_MESSAGE_TYPE_OSC_FILE;
	file_msg_buf.nTransMask = nTransMask;
	file_msg_buf.bRecvEnd = false;
	file_msg_buf.command_msg.push_back(file_msg);
	file_msg_buf.bProcessed = false;
	file_msg_buf.result_msg.clear();

	PushCommandToDfuApci(file_msg_buf);

	return true;
}

bool CDfuMainFlow::LaunchManualFile()
{
	DFUMESSAGE manual_file_buf;
	DFU_COMMU_MSG manual_file_msg;
	int nTransMask = m_pApciHandler->Create_link_transmask();

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

	manual_file_buf.nDfuCommandID = msg_parser.GetMsgCommand();
	manual_file_buf.nInternalCommandID = -1;
	manual_file_buf.nCommandNum = 1;
	manual_file_buf.nMsgType = RECORD_DFU_MESSAGE_TYPE_MANUAL_FILE;
	manual_file_buf.nTransMask = nTransMask;
	manual_file_buf.bRecvEnd = false;
	manual_file_buf.command_msg.push_back(manual_file_msg);
	manual_file_buf.bProcessed = false;
	manual_file_buf.result_msg.clear();

	PushCommandToDfuApci(manual_file_buf);

	return true;
}
