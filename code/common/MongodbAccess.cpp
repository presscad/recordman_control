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

	sprintf(chConnectStr, "mongodb://%s:%d", 
		m_pMongoParam->chAddr, m_pMongoParam->nPort);

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

		if (false == m_pMongoAccessHandler->auth(
			m_pMongoParam->chDataBase, 
			m_pMongoParam->chUser, 
			m_pMongoParam->chPasswd, 
			strErrorMsg))
		{
			return false;
		}
	}
	catch (UserException& e)
	{
		strErrorMsg = e.toString();
		printf("connect mongo£º%s£º%d failed£¬reason£º%s", 
			m_pMongoParam->chAddr, m_pMongoParam->nPort, e.toString().c_str());
		return false;
	}

	printf("conn mongo£º%s£º%d£¬succeed£¡\n", 
		m_pMongoParam->chAddr, 
		m_pMongoParam->nPort);
	
	return true;
}

//query table data
bool CMongodbAccess::QueryData(string strTbName, auto_ptr<mongo::DBClientCursor>& result)
{
	if (NULL == m_pMongoAccessHandler)
	{
		return false;
	}

	string strDbTbName = m_pMongoParam->chDataBase;
	strDbTbName += ".";
	strDbTbName += strTbName;

	mongo::BSONObjBuilder query;
	result = m_pMongoAccessHandler->query(strDbTbName, query.obj());

	return true;
}
