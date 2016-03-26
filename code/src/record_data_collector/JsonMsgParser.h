#if !defined(RECORD_JSON_MSG_PARSER_INCLUDE)
#define RECORD_JSON_MSG_PARSER_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../../common/sys_define.h"
#include "../../common_open_source/cJSON/cJSON.h"
#include "DfuMsgAttach.h"

typedef int (*PJSONCOMMANDTOMSGFUNC)(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);

class CJsonMsgParser
{
public:
	CJsonMsgParser(void);
	~CJsonMsgParser(void);

public:
	void Attach(cJSON* pJsonObj);

	int GetCommandID();

	bool JsonToRecordDfuMsg(vector<DFU_COMMU_MSG>& veMsg, cJSON* pJonObj = NULL);

public:
	static int Json_20001_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20003_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20005_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20007_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20009_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20011_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20013_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20015_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20017_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20019_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20021_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20023_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);
	static int Json_20060_to_msg(vector<DFU_COMMU_MSG>& veMsg, void* pParm, int nOption);

private:
	int InitJsonToMsgFunc();

public:
	bool GetJsonItem(string& strItemval, const char* pItemName);

	bool GetJsonItem(int& nItemval, const char* pItemName);

private:
	cJSON* m_pJsonMsg;

	map<int, PJSONCOMMANDTOMSGFUNC> m_mapJsonToMsgFun;
};

#endif
