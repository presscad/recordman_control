#if !defined(RECORD_DFU_MSG_TO_JSON_INCLUDE)
#define RECORD_DFU_MSG_TO_JSON_INCLUDE

#include "../../common/sys_define.h"
#include "../../common_open_source/cJSON/cJSON.h"
#include "DfuMsgAttach.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef int (*PDFUMSGTOJSONFUNC)(cJSON*& pJsonMsg, void* pParm, int nOption);

class CDfuMsgToJson
{
public:
	CDfuMsgToJson(void);
	~CDfuMsgToJson(void);

public:
	void Attach(DFUMESSAGE* pMsg);

	bool DfuMsgToJson(cJSON*& pJsonMsg, DFUMESSAGE* pMsg = NULL, int nOption = 0);

	DFUMESSAGE* GetDfuResultMsg();

public:
	static int DfuMsg_22_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_23_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_24_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_31_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_33_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_3A_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_91_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_92_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_93_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_94_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_A0_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_A1_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_A2_Json(cJSON*& pJsonMsg, void* pParm, int nOption);
	static int DfuMsg_A3_Json(cJSON*& pJsonMsg, void* pParm, int nOption);

private:
	int InitDfuMsgToJsonFunc();

private:
	DFUMESSAGE* m_pMsg;

	map<int, PDFUMSGTOJSONFUNC> m_mapDfuToJsonFunc;
};

#endif
