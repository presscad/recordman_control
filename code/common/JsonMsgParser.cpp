#include "JsonMsgParser.h"


CJsonMsgParser::CJsonMsgParser(void)
{
	m_pJsonMsg = NULL;
}


CJsonMsgParser::~CJsonMsgParser(void)
{
}

void CJsonMsgParser::Attach(cJSON* pJsonObj)
{
	m_pJsonMsg = pJsonObj;
}

int CJsonMsgParser::GetCommandID()
{
	cJSON* pCommandIDJson = cJSON_GetObjectItem(m_pJsonMsg, "COMMAND_ID");
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
