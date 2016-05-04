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
	m_pApciHandler = NULL;
	m_pCommandResultCallBackFunc = NULL;
	m_pCallBackCallObj = NULL;

	m_CommandMsgBuf.clear();
}


CDfuMainFlow::~CDfuMainFlow(void)
{
	m_LogFile.Close();
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

bool CDfuMainFlow::PushDfuResultMsg(DFU_COMMU_MSG result_msg)
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

int CDfuMainFlow::MainBuinessLoop()
{
	time_t tCur;
	time(&tCur);
	time(&m_tCheckFile);

	try
	{
		while (!m_bExitFlag)
		{
			if (true == m_bExitFlag)
			{
				break;
			}

			if ((tCur - m_tCheckFile) >= m_pDfuCommuParamHandler->nCheckNewFileTime)//query file
			{
// 				if (bQueryNewFile == false)
// 				{
// 					LaunchQueryNewFile();
// 					bQueryNewFile = true;
// 				}
				time(&m_tCheckFile);
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
					//AddFileResultMsg(over_msg);
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

// 			DFUMESSAGE file_msg;
// 			if (GetFileResultMsg(file_msg) == false)
// 			{
// 				MySleep(200);
// 				continue;
// 			}
// 
// 			bRet = ProcessFileResultMsg(file_msg);
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
	int nOffset = 18;

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

bool CDfuMainFlow::ProcessFileResultMsg(DFUMESSAGE& file_msg)
{
	if (file_msg.result_msg.size() <= 0)
	{
		m_LogFile.FormatAdd(CLogFile::error, 
			"[ProcessFileResultMsg]read new file finish，but no result！");
		return false;
	}

	DFU_COMMU_MSG* pOneMsg = NULL;
	CDFUMsgAttach msgAttach;
	UINT uDatablockNum(0);//数据块数目
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

			AnalyzeFileMsgHeader(head, pOneMsg, nMsgoffset, uDatablockNum);//报文头
			AnalyzeFileMsgSamples(samples, head.rateCount, pOneMsg, nMsgoffset);//采样段

			for (UINT uNo = 0; uNo < uDatablockNum; uNo++)//数据块
			{
				list<short> data_val;
				data_val.clear();

				AnalyzeFileMsgAis(data_val, head.aiCount, pOneMsg, nMsgoffset);//模拟量
				AnalyzeFileMsgDis(data_val, head.diCount, pOneMsg, nMsgoffset);//开关量

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
	com.setDatas(datas);//通道的值

	com.create(m_pDfuCommuParamHandler->chFileSavePath, "test");

	return true;
}

bool CDfuMainFlow::GetOscInfo(comtradeHead& head)
{
	long lLen = 0;
	auto_ptr<mongo::DBClientCursor> result;
	m_pMongoAccessHandler->QueryData("runstatus_info", result);
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

bool CDfuMainFlow::AnalyzeFileMsgHeader(comtradeHead& head, DFU_COMMU_MSG* pMsg, int& nOffset, UINT& uDatablockNum)
{
	uint16 uAnalogchannelNum(0);//模拟量通道数
	uint16 uDigitalchannelNum(0);//开关量通道数
	UINT uFilesecond(0);//录波时刻，秒
	UINT uFilenanosecond(0);//录波时刻，纳秒
	UINT uFaultsecond(0);//故障时刻，秒
	UINT uFaultnanosecond(0);//故障时刻，纳秒
	UINT uSamplesectionNum(0);//采样段数目

	try
	{
		convert_btol_uint16(&(*pMsg)[nOffset], uAnalogchannelNum);//模拟通道数目
		head.aiCount += uAnalogchannelNum;
		nOffset += 2;

		convert_btol_uint16(&(*pMsg)[nOffset], uAnalogchannelNum);//开关通道数目
		head.diCount += uAnalogchannelNum;
		nOffset += 2;

		convert_btol_float32(&(*pMsg)[nOffset], head.primary);//模拟通道一次系数
		nOffset += 4;

		convert_btol_float32(&(*pMsg)[nOffset], head.secondary);//模拟通道二次系数
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uFilesecond);//录波时刻（秒）
		nOffset += 4;
		head.startTime = format_dfu_msg_time(uFilesecond);

		convert_btol_uint32(&(*pMsg)[nOffset], uFilenanosecond);//录波时刻（纳秒）
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uFaultsecond);//故障时间（秒）
		nOffset += 4;
		head.faultTime = format_dfu_msg_time(uFaultsecond);

		convert_btol_uint32(&(*pMsg)[nOffset], uFilenanosecond);//故障时刻（纳秒）
		nOffset += 4;

		convert_btol_float32(&(*pMsg)[nOffset], head.lineFreq);//线路频率
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uSamplesectionNum);//采样段数目
		head.rateCount += uSamplesectionNum;
		nOffset += 4;

		convert_btol_uint32(&(*pMsg)[nOffset], uDatablockNum);//数据块数目
		nOffset += 4;
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[AnalyzeFileMsgHeader]analyze msg header exception！");
		return false;
	}

	return true;
}

bool CDfuMainFlow::AnalyzeFileMsgSamples(list<sampleInfo>& samples, float uSampleNum, DFU_COMMU_MSG* pMsg, int& nOffset)
{
	try
	{
		for (UINT uNo = 0; uNo < uSampleNum; uNo++)
		{
			sampleInfo sa;

			convert_btol_float32(&(*pMsg)[nOffset], sa.rate);//采样率
			nOffset += 4;

			convert_btol_int32(&(*pMsg)[nOffset], sa.lastIndex);//此采用频率最后一个采样的序号
			nOffset += 4;

			samples.push_back(sa);
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[AnalyzeFileMsgSamples]analyze msg samples exception！");
		return false;
	}

	return true;
}

bool CDfuMainFlow::AnalyzeFileMsgAis(list<short>& data_vals, UINT uAiNum, DFU_COMMU_MSG* pMsg, int& nOffset)
{
	UINT uDival(0);

	try
	{
		for (UINT uDataNo = 0; uDataNo < uAiNum; uDataNo++)//模拟量
		{
			//ConvertUint32BigedianToL(&(*pMsg)[nOffset], ai.index);
			nOffset += 4;//模拟通道编号

			convert_btol_uint32(&(*pMsg)[nOffset], uDival);//模拟通道编号
			nOffset += 4;

			data_vals.push_back(uDival);
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[AnalyzeFileMsgAis]analyze msg ai exception！");
		return false;
	}

	return true;
}

bool CDfuMainFlow::AnalyzeFileMsgDis(list<short>& data_vals, UINT uDiNum, DFU_COMMU_MSG* pMsg, int& nOffset)
{
	UINT uDiBlockNo = 0;
	UINT uDival(0);
	UINT uOneDival(0);
	UINT uDiRealNum(0);
	UINT uDataNo(0);

	try
	{
		for (uDiBlockNo = 0; uDiBlockNo <= (uDiNum / 32); uDiBlockNo++)
		{
			convert_btol_uint32(&(*pMsg)[nOffset], uDival);
			nOffset += 4;//开关量

			uDiRealNum = (uDiBlockNo == (uDiNum / 32))?(uDiNum % 32):32;

			for (uDataNo = 0; uDataNo < uDiRealNum; uDataNo++)
			{
				uOneDival = ((uDival >> uDataNo) & 0x1);

				data_vals.push_back(uOneDival);
			}
		}
	}
	catch (...)
	{
		m_LogFile.FormatAdd(CLogFile::error, "[AnalyzeFileMsgDis]analyze msg di exception！");
		return false;
	}

	return true;
}
