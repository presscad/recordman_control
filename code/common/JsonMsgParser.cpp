#include "JsonMsgParser.h"


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
	m_mapJsonToMsgFun.insert(make_pair(RECORD_DATA_MSG_ID_SET_TIME_ZONE, (PJSONCOMMANDTOMSGFUNC)Json_20023_to_msg));
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

bool CJsonMsgParser::JsonToRecordDfuMsg(vector<RECORD_DFU_MSG>& veMsg, cJSON* pJonObj /* = NULL */)
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
	nProcessResult = (*pFunc)(veMsg, this, 0);

	return (1 == nProcessResult)?true:false;
}

//read config file from dfu
int CJsonMsgParser::Json_20001_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	CDfuMsgParser dfu_msg_paraser;
	RECORD_DFU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(RECORD_DFU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgLength(8);
	dfu_msg_paraser.SetMsgCommand(0x12);
	dfu_msg_paraser.SetEndMask();
	dfu_msg_paraser.SetMsgEndFlag(true);

	veMsg.push_back(dfuMsg);

	return 0;
}

//call setting data from dfu
int CJsonMsgParser::Json_20003_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	return 1;
}

//switch zone to dfu
int CJsonMsgParser::Json_20005_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	return 1;
}

//read curzone from dfu
int CJsonMsgParser::Json_20007_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	return 1;
}

//read selfcheck info from dfu
int CJsonMsgParser::Json_20009_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	CDfuMsgParser dfu_msg_paraser;
	RECORD_DFU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(RECORD_DFU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgLength(8);
	dfu_msg_paraser.SetMsgCommand(0x91);
	dfu_msg_paraser.SetEndMask();
	dfu_msg_paraser.SetMsgEndFlag(true);

	veMsg.push_back(dfuMsg);

	return 1;
}

//read version
int CJsonMsgParser::Json_20011_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	CDfuMsgParser dfu_msg_paraser;
	RECORD_DFU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(RECORD_DFU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgLength(8);
	dfu_msg_paraser.SetMsgCommand(0x92);
	dfu_msg_paraser.SetEndMask();
	dfu_msg_paraser.SetMsgEndFlag(true);

	veMsg.push_back(dfuMsg);

	return 1;
}

//read time
int CJsonMsgParser::Json_20013_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	CDfuMsgParser dfu_msg_paraser;
	RECORD_DFU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(RECORD_DFU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgLength(8);
	dfu_msg_paraser.SetMsgCommand(0x93);
	dfu_msg_paraser.SetEndMask();
	dfu_msg_paraser.SetMsgEndFlag(true);

	veMsg.push_back(dfuMsg);

	return 1;
}

//red sub module
int CJsonMsgParser::Json_20015_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	CDfuMsgParser dfu_msg_paraser;
	RECORD_DFU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(RECORD_DFU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgLength(8);
	dfu_msg_paraser.SetMsgCommand(0x94);
	dfu_msg_paraser.SetEndMask();
	dfu_msg_paraser.SetMsgEndFlag(true);

	veMsg.push_back(dfuMsg);

	return 1;
}

//reset
int CJsonMsgParser::Json_20017_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	CDfuMsgParser dfu_msg_paraser;
	RECORD_DFU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(RECORD_DFU_MSG));

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgLength(8);
	dfu_msg_paraser.SetMsgCommand(0xA0);
	dfu_msg_paraser.SetEndMask();
	dfu_msg_paraser.SetMsgEndFlag(true);

	veMsg.push_back(dfuMsg);

	return 1;
}

//set ip addr
int CJsonMsgParser::Json_20019_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	return 1;
}

//set time
int CJsonMsgParser::Json_20021_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	return 1;
}

//set zone
int CJsonMsgParser::Json_20023_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	return 1;
}

//down config file to dfu
int CJsonMsgParser::Json_20060_to_msg(vector<RECORD_DFU_MSG>& veMsg, void* pParm, int nOption)
{
	string strFileDir = "";
	string strFileName = "";
	CDfuMsgParser dfu_msg_paraser;
	RECORD_DFU_MSG dfuMsg;
	bzero(&dfuMsg, sizeof(RECORD_DFU_MSG));
	CJsonMsgParser* pJsonMsgParser = (CJsonMsgParser*)pParm;

	pJsonMsgParser->GetJsonItem(strFileDir, "file_dir");
	pJsonMsgParser->GetJsonItem(strFileName, "file_name");

	dfu_msg_paraser.Attach(&dfuMsg);

	dfu_msg_paraser.SetMsgStartMask();
	dfu_msg_paraser.SetMsgProtocolMask();
	dfu_msg_paraser.SetMsgReserve();
	dfu_msg_paraser.SetMsgLength(8);
	dfu_msg_paraser.SetMsgCommand(0x11);
	dfu_msg_paraser.SetEndMask();
	dfu_msg_paraser.SetMsgEndFlag(true);

	veMsg.push_back(dfuMsg);

	return 1;
}
