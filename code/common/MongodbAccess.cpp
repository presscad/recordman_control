#include "MongodbAccess.h"


CMongodbAccess::CMongodbAccess(void)
{
	m_pMongoParam = NULL;
	m_pMongoAccessHandler = NULL;
}


CMongodbAccess::~CMongodbAccess(void)
{
}

void CMongodbAccess::SetMongDbAccessparam(RECORD_MONGO_BASIC_PARAM* pMongoParam)
{
	m_pMongoParam = pMongoParam;
}

bool CMongodbAccess::ConnectMongoServer(string& strErrorMsg)
{
	char chConnectStr[256] = "";
	bzero(chConnectStr, sizeof(chConnectStr));
	
	sprintf(chConnectStr, "mongodb://%s:%s@%s:%d/%s", 
		m_pMongoParam->chUser, m_pMongoParam->chPasswd, 
		m_pMongoParam->chAddr, m_pMongoParam->nPort, 
		m_pMongoParam->chDataBase);

// 	sprintf(chConnectStr, "mongodb://%s:%d", 
// 		m_pMongoParam->chAddr, m_pMongoParam->nPort);

	//mongo::DBClientConnection c;
	mongo::client::initialize();

	ConnectionString cs = ConnectionString::parse(chConnectStr, strErrorMsg);
	if (!cs.isValid())
	{
		return false;
	}

	try
	{
		m_pMongoAccessHandler = cs.connect(strErrorMsg);
		if (NULL == m_pMongoAccessHandler)
		{
			return false;
		}
	}
	catch (UserException& e)
	{
		strErrorMsg = e.toString();
		return false;
	}

	printf("conn %s£¬succeed£¡\n", chConnectStr);
	
	return true;
}
