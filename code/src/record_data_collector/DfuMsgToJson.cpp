#include "DfuMsgToJson.h"


CDfuMsgToJson::CDfuMsgToJson(void)
{
	m_pMsg = NULL;
	m_mapDfuToJsonFunc.clear();

	InitDfuMsgToJsonFunc();
}


CDfuMsgToJson::~CDfuMsgToJson(void)
{
}

int CDfuMsgToJson::InitDfuMsgToJsonFunc()
{
	m_mapDfuToJsonFunc.clear();
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_SETTING_READ_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_22_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_SETZONE_CHANGE_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_23_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_CURZONE_READ_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_24_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_NEW_OSC_QUERY_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_31_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_OSC_FILE_READ_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_33_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_SELF_CHECK_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_91_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_VERSION_QUERY_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_92_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_TIME_QUERY_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_93_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_SUB_MODULE_QUERY_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_94_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_RESET_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_A0_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_IP_SET_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_A1_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_TIME_SET_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_A2_Json));
	m_mapDfuToJsonFunc.insert(make_pair(RECORD_COMMAND_CHAR_TIMEZONE_SET_VAR, (PDFUMSGTOJSONFUNC)DfuMsg_A3_Json));

	return 0;
}

void CDfuMsgToJson::Attach(DFUMESSAGE* pMsg)
{
	m_pMsg = pMsg;
}

bool CDfuMsgToJson::DfuMsgToJson(cJSON*& pJsonMsg, DFUMESSAGE* pMsg /*= NULL*/, int nOption /*= 0*/)
{
	if (NULL == pMsg)
	{
		if (NULL == m_pMsg)
		{
			return false;
		}
	}
	else
	{
		Attach(pMsg);
	}

	bool bRet(false);

	if (m_pMsg->nCommandProcessResult == RECORD_COMMAND_RESULT_FAILED)
	{
		pJsonMsg = cJSON_CreateObject();
		cJSON_AddNumberToObject(pJsonMsg, "command_id", (m_pMsg->nInternalCommandID + 1));
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
		bRet = true;
	}
	else
	{
		map<int, PDFUMSGTOJSONFUNC>::iterator it = m_mapDfuToJsonFunc.find(m_pMsg->nDfuCommandID);
		if (it != m_mapDfuToJsonFunc.end())
		{
			PDFUMSGTOJSONFUNC pFun = it->second;
			bRet = (0 == pFun(pJsonMsg, this, nOption))?true:false;
		}
		else
		{
			bRet = false;
		}
	}

	return bRet;
}

DFUMESSAGE* CDfuMsgToJson::GetDfuResultMsg()
{
	return m_pMsg;
}

//call setting result
int CDfuMsgToJson::DfuMsg_22_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach dfuMsgAttach;
	DFUMESSAGE* pMsg = NULL;
	bool bSetZone = false;
	int nSettingNum = 0;
	UINT nSettingID = 0;
	int nContentNum = 22;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20004);
	cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);

	pMsg = pObj->GetDfuResultMsg();
	vector<DFU_COMMU_MSG>::iterator itResultMsg = pMsg->result_msg.begin();
	for (; itResultMsg != pMsg->result_msg.end(); itResultMsg++)
	{
		DFU_COMMU_MSG* pDfuMsg = &*itResultMsg;
		dfuMsgAttach.Attach(pDfuMsg);

		if (false == bSetZone)
		{
			cJSON_AddNumberToObject(pJsonMsg, "set_zone", dfuMsgAttach.GetMsgSettingZone());
			bSetZone = true;
		}

		nSettingNum = dfuMsgAttach.GetSettingNum();
		for (int i = 0; i < nSettingNum; i++)
		{
			int a = 0;
			memcpy(&a, &(*pDfuMsg)[22], 4);
			SWAP_32(a);
// 			nSettingID = 
// 				(*pDfuMsg)[nContentNum] + 
// 				((*pDfuMsg)[nContentNum + 1] << 8) + 
// 				((*pDfuMsg)[nContentNum + 2] << 16) + 
// 				((*pDfuMsg)[nContentNum + 3] << 24);
		}
	}

	return 0;
}

//switch curzone result
int CDfuMsgToJson::DfuMsg_23_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20006);

	pResultMsgList = pClassObj->GetDfuResultMsg();
	if (pResultMsgList->result_msg.size() <= 0)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
		return 0;
	}
	else
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_NORMAL);
	}

	pOneMsg = &pResultMsgList->result_msg.front();
	msgAttach.Attach(pOneMsg);

	cJSON_AddNumberToObject(pJsonMsg, "last_set_zone", msgAttach.GetOldZone());
	cJSON_AddNumberToObject(pJsonMsg, "cur_set_zone", msgAttach.GetNewZone());

	return 0;
}

//read curzone result
int CDfuMsgToJson::DfuMsg_24_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20008);

	pResultMsgList = pClassObj->GetDfuResultMsg();
	if (pResultMsgList->result_msg.size() <= 0)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
		return 0;
	}
	else
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_NORMAL);
	}

	pOneMsg = &pResultMsgList->result_msg.front();
	msgAttach.Attach(pOneMsg);

	cJSON_AddNumberToObject(pJsonMsg, "cur_set_zone", msgAttach.GetMsgSettingZone());
	cJSON_AddNumberToObject(pJsonMsg, "zone_num", msgAttach.GetZoneNum());

	return 0;
}

//query new file list
int CDfuMsgToJson::DfuMsg_31_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//call osc file
int CDfuMsgToJson::DfuMsg_33_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//self check
int CDfuMsgToJson::DfuMsg_91_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//query version
int CDfuMsgToJson::DfuMsg_92_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//query time
int CDfuMsgToJson::DfuMsg_93_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//submodule query
int CDfuMsgToJson::DfuMsg_94_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//reset
int CDfuMsgToJson::DfuMsg_A0_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//write ip addr
int CDfuMsgToJson::DfuMsg_A1_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//set time
int CDfuMsgToJson::DfuMsg_A2_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}

//set time zone
int CDfuMsgToJson::DfuMsg_A3_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	return 0;
}
