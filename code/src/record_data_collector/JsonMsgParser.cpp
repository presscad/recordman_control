#include "JsonMsgParser.h"
#include "DfuMsgAttach.h"


CJsonMsgParser::CJsonMsgParser(void)
{
	m_pJsonMsg = NULL;
	InitJsonToMsgFunc();
}


CJsonMsgParser::~CJsonMsgParser(void)
{
	m_mapJsonToMsgFun.clear();
}

int CJsonMsgParser::InitJsonToMsgFunc()
{
	m_mapJsonToMsgFun.clear();
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_CALL_DFU_CONFIG, (PJSONCOMMANDTOMSGFUNC)Json_20001_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_CALL_SETTING, (PJSONCOMMANDTOMSGFUNC)Json_20003_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_SWITCH_ZONE, (PJSONCOMMANDTOMSGFUNC)Json_20005_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_CALL_ZONE, (PJSONCOMMANDTOMSGFUNC)Json_20007_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_QUERY_SELFCHECK, (PJSONCOMMANDTOMSGFUNC)Json_20009_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_QUERY_VER, (PJSONCOMMANDTOMSGFUNC)Json_20011_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_QUERY_TIME, (PJSONCOMMANDTOMSGFUNC)Json_20013_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_QUERY_MOULE, (PJSONCOMMANDTOMSGFUNC)Json_20015_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_RESET_DEV, (PJSONCOMMANDTOMSGFUNC)Json_20017_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_SET_NET_PARAM, (PJSONCOMMANDTOMSGFUNC)Json_20019_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_SET_TIME, (PJSONCOMMANDTOMSGFUNC)Json_20021_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_MANUAL_OSC_FILE, (PJSONCOMMANDTOMSGFUNC)Json_20025_to_msg));
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_DOWN_DFU_CONFIG, (PJSONCOMMANDTOMSGFUNC)Json_20060_to_msg));

	return 1;
}

void CJsonMsgParser::Attach(cJSON* pJsonObj)
{
	m_pJsonMsg = pJsonObj;
}

int CJsonMsgParser::GetCommandID()
{
	if (NULL == m_pJsonMsg)
	{
		return -1;
	}

	cJSON* pCommandIDJson = cJSON_GetObjectItem(m_pJsonMsg, "command_id");
	if (NULL == pCommandIDJson)
	{
		return -1;
	}

	int nCommandID = 0;
	if (cJSON_String == pCommandIDJson->type)
	{
		nCommandID = atoi(pCommandIDJson->valuestring);
	}
	else
	{
		nCommandID = pCommandIDJson->valueint;
	}

	return nCommandID;
}

bool CJsonMsgParser::GetJsonItem(string& strItemval, const char* pItemName)
{
	cJSON* pItemJson = cJSON_GetObjectItem(m_pJsonMsg, pItemName);
	if (NULL == pItemJson)
	{
		return false;
	}

	if (NULL != pItemJson->valuestring)
	{
		strItemval = pItemJson->valuestring;
	}

	return true;
}

bool CJsonMsgParser::GetJsonItem(int& nItemval, const char* pItemName)
{
	cJSON* pItemJson = cJSON_GetObjectItem(m_pJsonMsg, pItemName);
	if (NULL == pItemJson)
	{
		return false;
	}

	nItemval = pItemJson->valueint;

	return true;
}

bool CJsonMsgParser::JsonToRecordDfuMsg(vector<DFU_COMMU_MSG>& veMsg, int& nDfuCommandID, cJSON* pJonObj /* = NULL */)
{
	if (NULL == pJonObj)
	{
		if (NULL == m_pJsonMsg)
		{
			printf("[JsonToRecordDfuMsg]json msg obj is null£¡\n");
			return false;
		}
	}
	else
	{
		Attach(pJonObj);
	}

	int nCommandID = -1;
	int nProcessResult = -1;
	
	nCommandID = GetCommandID();
	map<int, PJSONCOMMANDTOMSGFUNC>::iterator it = m_mapJsonToMsgFun.find(nCommandID);
	if (it == m_mapJsonToMsgFun.end())
	{
		printf("[JsonToRecordDfuMsg]json msg id£º%d£¬can't find process func£¡\n", 
			nCommandID);
		return false;
	}

	PJSONCOMMANDTOMSGFUNC pFunc = it->second;
	nProcessResult = (*pFunc)(nDfuCommandID, veMsg, this, 0);

	return (1 == nProcessResult)?true:false;
}

//read config file from dfu
int CJsonMsgParser::Json_20001_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_CONFIG_RECV_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 0;
}

//call setting data from dfu
int CJsonMsgParser::Json_20003_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();
	int nZone = -1;
	int nGroup = -1;
	int nGroupIndex = -1;

	CJsonMsgParser* pJsonMsgParser = (CJsonMsgParser*)pParm;
	pJsonMsgParser->GetJsonItem(nZone, "set_zone");
	pJsonMsgParser->GetJsonItem(nGroup, "set_group");
	pJsonMsgParser->GetJsonItem(nGroupIndex, "set_index");

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_SETTING_READ_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgSettingZone(nZone);
	dfu_msg_paraser.SetMsgSettingGroup(nGroup);
	dfu_msg_paraser.SetMsgSettingGroupIndex(nGroupIndex);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//switch zone to dfu
int CJsonMsgParser::Json_20005_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();
	int nZone = -1;

	CJsonMsgParser* pJsonMsgParser = (CJsonMsgParser*)pParm;
	pJsonMsgParser->GetJsonItem(nZone, "set_zone");

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_SETZONE_CHANGE_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgSettingZone(nZone);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//read curzone from dfu
int CJsonMsgParser::Json_20007_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_CURZONE_READ_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//read selfcheck info from dfu
int CJsonMsgParser::Json_20009_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(DFU_COMMU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_SELF_CHECK_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//read version
int CJsonMsgParser::Json_20011_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(DFU_COMMU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_VERSION_QUERY_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//read time
int CJsonMsgParser::Json_20013_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_TIME_QUERY_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//red sub module
int CJsonMsgParser::Json_20015_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();
	int nSubModuleNo = -1;

	CJsonMsgParser* pJsonMsgParser = (CJsonMsgParser*)pParm;
	pJsonMsgParser->GetJsonItem(nSubModuleNo, "module_id");

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_SUB_MODULE_QUERY_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgSubModuleNo(nSubModuleNo);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//reset
int CJsonMsgParser::Json_20017_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_RESET_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//set ip addr
int CJsonMsgParser::Json_20019_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	return 1;
}

//set time
int CJsonMsgParser::Json_20021_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();
	int nCursecond = -1;
	int nCurNanoSecond = 0;

	CJsonMsgParser* pJsonMsgParser = (CJsonMsgParser*)pParm;
	pJsonMsgParser->GetJsonItem(nCursecond, "cur_second");
	pJsonMsgParser->GetJsonItem(nCurNanoSecond, "cur_nanosecond");

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_TIME_SET_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgCurSecond(nCursecond);
	dfu_msg_paraser.SetMsgCurNanoSecond(nCurNanoSecond);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//manual osc file
int CJsonMsgParser::Json_20025_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(DFU_COMMU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_MANUAL_OSC_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}

//down config file to dfu
int CJsonMsgParser::Json_20060_to_msg(int& nDfuMsgID, vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption)
{
	string strFileDir = "";
	string strFileName = "";
	CDFUMsgAttach dfu_msg_paraser;
	DFU_COMMU_MSG dfuMsg;
	dfuMsg.clear();
	CJsonMsgParser* pJsonMsgParser = (CJsonMsgParser*)pParm;

	pJsonMsgParser->GetJsonItem(strFileDir, "file_dir");
	pJsonMsgParser->GetJsonItem(strFileName, "file_name");

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgFuncMask();
	dfu_msg_paraser.SetMsgCommand(RECORD_COMMAND_CHAR_CONFIG_SEND_VAR);
	dfu_msg_paraser.SetMsgEndFlag(true);
	dfu_msg_paraser.SetMsgLength();
	dfu_msg_paraser.SetEndMask();

	nDfuMsgID = dfu_msg_paraser.GetMsgCommand();

	veMsg.push_back(dfuMsg);

	return 1;
}
