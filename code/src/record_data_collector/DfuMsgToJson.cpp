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
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	cJSON* pSettingSet = NULL;
	cJSON* pOneSet = NULL;
	CDFUMsgAttach msgAttach;
	DFUMESSAGE* pMsgList = NULL;
	bool bSetZone = false;
	int nOffset = 22;
	int nSettingNum = 0;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20004);//command id
	cJSON_AddItemToObject(pJsonMsg, "set_vals", pSettingSet = cJSON_CreateArray());//setting vals array

	pMsgList = pClassObj->GetDfuResultMsg();
	if (pMsgList->result_msg.size() <= 0)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);//fail result
		return 0;
	}
	else
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_NORMAL);//succeed result
	}

	vector<DFU_COMMU_MSG>::iterator itResultMsg = pMsgList->result_msg.begin();
	for (; itResultMsg != pMsgList->result_msg.end(); itResultMsg++)//for multi msg
	{
		DFU_COMMU_MSG* pOneMsg = &*itResultMsg;
		msgAttach.Attach(pOneMsg);

		if (false == bSetZone)
		{
			cJSON_AddNumberToObject(pJsonMsg, "set_zone", msgAttach.GetMsgSettingZone());
			bSetZone = true;
		}

		nSettingNum = msgAttach.GetSettingNum();//setting nums

		for (int i = 0; i < nSettingNum; i++)//set group
		{
			int nSettingID = 0;
			int nSettingIndex = 0;
			int nDataType = 0;
			int nGroupIndex = 0;
			int nGroupNo = 0;

			ConvertInt32BigedianToL(&(*pOneMsg)[nOffset], nSettingID);
			nOffset += 4;

			ConvertInt32BigedianToL(&(*pOneMsg)[nOffset], nSettingIndex);
			nDataType = nSettingIndex & 0x0F;
			nGroupIndex = (nSettingIndex & 0x1FFF0) >> 4;
			nGroupNo = nSettingIndex >> 18;
			nOffset += 4;

			cJSON_AddItemToObject(pSettingSet, "", pOneSet = cJSON_CreateObject());//setting vals array
			cJSON_AddNumberToObject(pOneSet, "set_id", nSettingID);//set_id
			cJSON_AddNumberToObject(pOneSet, "data_type", nDataType);//data_type
			cJSON_AddNumberToObject(pOneSet, "set_index", nGroupIndex);//set_index
			cJSON_AddNumberToObject(pOneSet, "set_group_no", nGroupNo);//set_group_no

			if ((RECORD_DFU_SETTING_DATATYPE_BOOL == nDataType) || 
				(RECORD_DFU_SETTING_DATATYPE_INT == nDataType) || 
				(RECORD_DFU_SETTING_DATATYPE_UINT == nDataType))
			{
				int nValue(0);
				ConvertInt32BigedianToL(&(*pOneMsg)[nOffset], nValue);
				cJSON_AddNumberToObject(pOneSet, "set_val", nValue);//set_val
			}
			else if (RECORD_DFU_SETTING_DATATYPE_FOLAT32 == nDataType)
			{
				float fValue(0);
				ConvertFloat32BigedianToL(&(*pOneMsg)[nOffset], fValue);
				cJSON_AddNumberToObject(pOneSet, "set_val", fValue);//set_val
			}
			nOffset += 4;
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
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;
	int nOffset = 18;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20010);

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

	int nDevMask(0);
	int nModuleMask(0);
	int nPluginMask(0);

	ConvertInt32BigedianToL(&(*pOneMsg)[nOffset], nDevMask);
	ConvertInt32BigedianToL(&(*pOneMsg)[nOffset + 4], nModuleMask);

	cJSON_AddNumberToObject(pJsonMsg, "dev_mask", nDevMask);
	cJSON_AddNumberToObject(pJsonMsg, "module_mask", msgAttach.GetZoneNum());
	cJSON_AddNumberToObject(pJsonMsg, "plugin_mask", msgAttach.GetZoneNum());

	return 0;
}

//query version
int CDfuMsgToJson::DfuMsg_92_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;
	int nOffset = 18;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20012);

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

	string strPlatformVer = GetInt32VersionInfo(&(*pOneMsg)[nOffset]);
	string strPlatformCrc = GetDfucrcInfo(&(*pOneMsg)[nOffset + 2]);
	string strSoftwareVer = GetInt32VersionInfo(&(*pOneMsg)[nOffset + 4]);
	string strSoftwareCrc = GetDfucrcInfo(&(*pOneMsg)[nOffset + 6]);
	string strConfigVer = GetInt32VersionInfo(&(*pOneMsg)[nOffset + 8]);
	string strConfigCrc = GetDfucrcInfo(&(*pOneMsg)[nOffset + 10]);

	cJSON_AddStringToObject(pJsonMsg, "platform_ver", strPlatformVer.c_str());
	cJSON_AddStringToObject(pJsonMsg, "platform_crc", strPlatformCrc.c_str());
	cJSON_AddStringToObject(pJsonMsg, "software_ver", strSoftwareVer.c_str());
	cJSON_AddStringToObject(pJsonMsg, "software_crc", strSoftwareCrc.c_str());
	cJSON_AddStringToObject(pJsonMsg, "config_ver", strConfigVer.c_str());
	cJSON_AddStringToObject(pJsonMsg, "config_crc", strConfigCrc.c_str());

	return 0;
}

//query time
int CDfuMsgToJson::DfuMsg_93_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;
	int nOffset = 18;
	int nVal = 0;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20014);

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


	UINT uCurSecond = 0;
	UINT uCurNanoSecond = 0;
	int nCurZone = 0;

	ConvertUint32BigedianToL(&(*pOneMsg)[nOffset], uCurSecond);
	ConvertUint32BigedianToL(&(*pOneMsg)[nOffset + 4], uCurNanoSecond);
	ConvertInt32BigedianToL(&(*pOneMsg)[nOffset + 8], nCurZone);

	cJSON_AddNumberToObject(pJsonMsg, "cur_second", uCurSecond);
	cJSON_AddNumberToObject(pJsonMsg, "cur_nanosecond", uCurNanoSecond);
	cJSON_AddNumberToObject(pJsonMsg, "time_zone", nCurZone);

	return 0;
}

//submodule query
int CDfuMsgToJson::DfuMsg_94_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;
	int nOffset = 18;
	uint16 usubmoduleNo = 0;
	char chInfoNote[65] = "";
	bzero(chInfoNote, sizeof(chInfoNote));

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20016);

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

	ConvertUint16BigedianToL(&(*pOneMsg)[nOffset], usubmoduleNo);
	string strSoftwareVer = GetInt32VersionInfo(&(*pOneMsg)[nOffset + 2]);
	string strSoftwareCrc = GetDfucrcInfo(&(*pOneMsg)[nOffset + 4]);
	string strConfigVer = GetInt32VersionInfo(&(*pOneMsg)[nOffset + 6]);
	string strConfigCrc = GetDfucrcInfo(&(*pOneMsg)[nOffset + 8]);
	memcpy(chInfoNote, &(*pOneMsg)[nOffset + 26], 64);

	cJSON_AddNumberToObject(pJsonMsg, "module_id", usubmoduleNo);
	cJSON_AddStringToObject(pJsonMsg, "software_ver", strSoftwareVer.c_str());
	cJSON_AddStringToObject(pJsonMsg, "software_crc", strSoftwareCrc.c_str());
	cJSON_AddStringToObject(pJsonMsg, "config_ver", strConfigVer.c_str());
	cJSON_AddStringToObject(pJsonMsg, "config_crc", strConfigCrc.c_str());
	cJSON_AddStringToObject(pJsonMsg, "info_zone", "0");
	cJSON_AddStringToObject(pJsonMsg, "info_note", chInfoNote);

	return 0;
}

//reset
int CDfuMsgToJson::DfuMsg_A0_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20018);

	pResultMsgList = pClassObj->GetDfuResultMsg();
	if (pResultMsgList->result_msg.size() <= 0)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
		return 0;
	}

	pOneMsg = &pResultMsgList->result_msg.front();
	msgAttach.Attach(pOneMsg);

	if (msgAttach.GetMsgErrorFlag() == true)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
	}
	else
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_NORMAL);
	}

	return 0;
}

//write ip addr
int CDfuMsgToJson::DfuMsg_A1_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20020);

	pResultMsgList = pClassObj->GetDfuResultMsg();
	if (pResultMsgList->result_msg.size() <= 0)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
		return 0;
	}

	pOneMsg = &pResultMsgList->result_msg.front();
	msgAttach.Attach(pOneMsg);

	if (msgAttach.GetMsgErrorFlag() == true)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
	}
	else
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_NORMAL);
	}

	return 0;
}

//set time
int CDfuMsgToJson::DfuMsg_A2_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20022);

	pResultMsgList = pClassObj->GetDfuResultMsg();
	if (pResultMsgList->result_msg.size() <= 0)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
		return 0;
	}

	pOneMsg = &pResultMsgList->result_msg.front();
	msgAttach.Attach(pOneMsg);

	if (msgAttach.GetMsgErrorFlag() == true)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
	}
	else
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_NORMAL);
	}

	return 0;
}

//set time zone
int CDfuMsgToJson::DfuMsg_A3_Json(cJSON*& pJsonMsg, void* pParm, int nOption)
{
	CDfuMsgToJson* pClassObj = (CDfuMsgToJson*)pParm;
	CDFUMsgAttach msgAttach;
	DFU_COMMU_MSG* pOneMsg = NULL;
	DFUMESSAGE* pResultMsgList = NULL;

	pJsonMsg = cJSON_CreateObject();
	cJSON_AddNumberToObject(pJsonMsg, "command_id", 20024);

	pResultMsgList = pClassObj->GetDfuResultMsg();
	if (pResultMsgList->result_msg.size() <= 0)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
		return 0;
	}

	pOneMsg = &pResultMsgList->result_msg.front();
	msgAttach.Attach(pOneMsg);

	if (msgAttach.GetMsgErrorFlag() == true)
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_FAILED);
	}
	else
	{
		cJSON_AddNumberToObject(pJsonMsg, "result", RECORD_COMMAND_RESULT_NORMAL);
	}

	return 0;
}
