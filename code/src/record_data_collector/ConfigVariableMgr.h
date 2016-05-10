#if !defined(RECORD_CONFIG_VARIABLE_MGR_INCLUDE)
#define RECORD_CONFIG_VARIABLE_MGR_INCLUDE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "const_define.h"
#include "../../common_open_source/tinyxml2/tinyxml2.h"

using namespace tinyxml2;

typedef tinyxml2::XMLNode TiXmlNode;
typedef tinyxml2::XMLElement TiXmlElement;
typedef tinyxml2::XMLDocument TiXmlDocument;
typedef tinyxml2::XMLAttribute TiXmlAttribute;
typedef tinyxml2::XMLComment TiXmlComment;
typedef tinyxml2::XMLText TiXmlText;
typedef tinyxml2::XMLDeclaration TiXmlDeclaration ;
typedef tinyxml2::XMLUnknown TiXmlUnknown;
typedef tinyxml2::XMLPrinter TiXmlPrinter;

class CConfigVariableMgr
{
public:
	CConfigVariableMgr(CMessageLog* pLogFile);
	~CConfigVariableMgr(void);

private:
	COLLECTOR_DATA_SYS_PARAM m_collector_sys_param;

	COLLECTOR_ADVANCE_RABBITMQ_PARAM m_rabbit_mq_param;

	COLLECTOR_DFU_COMMU_PARAM m_fault_dfu_param;

	COLLECTOR_DFU_COMMU_PARAM m_contin_dfu_param;

	RECORD_MONGO_BASIC_PARAM m_mongo_access_param;

private:
	CMessageLog* m_pLogFile;

public:
	bool InitCollectorSysparam();

	bool LoadCollectorSysParam();

public:
	char* GetSysParam_LogPath();

	int GetSysParam_LogLevel();

	int GetSysParam_LogDays();

	int GetSysParam_RecTimeOut();

	int GetSysParam_SendTimeOut();

	COLLECTOR_DATA_SYS_PARAM* GetSysParamHandler();

public:
	int GetRabmqadParam_RevChannel();

	char* GetRabmqadParam_RevQueName();

	char* GetRabmqadParam_WebResultQueName();

public:
	int GetRabmqBasicParam_ServerPort();

	int GetRabmqBasicParam_ChannelID();

	int GetRabmqBasicParam_ChannelMax();

	int GetRabmqBasicParam_FrameMax();

	int GetRabmqBasicParam_HeartbeatTime();

	char* GetRabmqBasicParam_Hostname();

	char* GetRabmqBasicParam_Username();

	char* GetRabmqBasicParam_Password();

	char* GetRabmqBasicParam_Vhost();

	RABBIT_MQ_BASIC_ACCESS_PARAM* GetRabmqBasicParamHandle();

public:
	int GetDfuCommuParam_Port(bool bFaultDfu = true);

	int GetDfuCommuParam_Idletime(bool bFaultDfu = true);

	int GetDfuCommuParam_ChecknewfileTime(bool bFaultDfu = true);

	char* GetDfuCommuParam_Addr(bool bFaultDfu = true);

	char* GetDfuCommuParam_FileSavePath(bool bFaultDfu = true);

	COLLECTOR_DFU_COMMU_PARAM* GetDfuCommuParamHandler(bool bFaultDfu = true);

public:
	int GetMongoParam_Port();

	char* GetMongoParam_Addr();

	char* GetMongoParam_User();

	char* GetMongoParam_Passwd();

	char* GetMongoParam_DbName();

	RECORD_MONGO_BASIC_PARAM* GetMongoParamHandle();

private:
	bool LoadSystemLogConfig(TiXmlElement* pRootXmlElement);

	bool LoadRabbitMqConfig(TiXmlElement* pRootXmlElement);

	bool LoadRabbitMqAdvanceConfig(TiXmlElement* pRootXmlElement);

	bool LoadDfuCommuConfig(TiXmlElement* pRootXmlElement);

	bool LoadContinDfuCommuConfig(TiXmlElement* pRootXmlElement);

	bool LoadMongoDbConfig(TiXmlElement* pRootXmlElement);

private:
	bool GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, int& nParam, int nParamSize = -1);

	bool GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, char* pParam, int nParamSize);
};

#endif
