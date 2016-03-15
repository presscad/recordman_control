#if !defined(RECORD_JSON_MSG_PARSER_INCLUDE)
#define RECORD_JSON_MSG_PARSER_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "sys_define.h"
#include "../common_open_source/cJSON/cJSON.h"

class CJsonMsgParser
{
public:
	CJsonMsgParser(void);
	~CJsonMsgParser(void);

public:
	void Attach(cJSON* pJsonObj);

public:
	int GetCommandID();

private:
	cJSON* m_pJsonMsg;
};

#endif

