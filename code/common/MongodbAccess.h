#ifndef RECORD_RECORD_MONGODB_ACCESS_H
#define RECORD_RECORD_MONGODB_ACCESS_H

#ifdef OS_WINDOWS
	#include <Winsock2.h>
	#include <Windows.h>
	#include "../common_open_source/mongo/include/mongo/client/dbclient.h"
#else
	#include "../common_open_source/mongo/include/mongo/client/dbclient.h"
#endif

#include "sys_define.h"

using namespace mongo;

class CMongodbAccess
{
public:
	CMongodbAccess(void);
	~CMongodbAccess(void);

public:
	void SetMongDbAccessparam(RECORD_MONGO_BASIC_PARAM* pMongoParam);

public:
	bool ConnectMongoServer(string& strErrorMsg);

	bool QueryData(string strTbName, auto_ptr<mongo::DBClientCursor>& result);

private:
	//mongodb access param
	RECORD_MONGO_BASIC_PARAM* m_pMongoParam;

	//mongodb access handler
	DBClientBase* m_pMongoAccessHandler;
};

#endif


