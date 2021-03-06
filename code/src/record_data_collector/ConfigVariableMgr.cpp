#include "ConfigVariableMgr.h"


CConfigVariableMgr::CConfigVariableMgr(CMessageLog* pLogFile)
{
	m_pLogFile = pLogFile;
}


CConfigVariableMgr::~CConfigVariableMgr(void)
{
}

bool CConfigVariableMgr::InitCollectorSysparam()
{
	try
	{
		m_collector_sys_param.nLoglevel = CLogFile::trace;
		m_collector_sys_param.nLogDays = COLLECTOR_LOG_SAVE_DEFAULT_DAYS;
		m_collector_sys_param.nRecvTimeout = COLLECTOR_COMMU_RECV_TIMEOUT;
		m_collector_sys_param.nSendTimeout = COLLECTOR_COMMU_SEND_TIMEOUT;
		sprintf(m_collector_sys_param.chLogpath, "%s", "./data_collector_log/");

		m_fault_dfu_param.nDfuport = COLLECTOR_DFU_LISTEN_PORT;
		m_fault_dfu_param.nIdleTime = COLLECTOR_IDLE_CHECEK_DEFAULT_TIME;
		m_fault_dfu_param.nCheckNewFileTime = COLLECTOR_COMMU_CHECK_NEW_FILE_TIME;
		sprintf(m_fault_dfu_param.chDfuAddr, "%s", "10.123.16.56");
		sprintf(m_fault_dfu_param.chFileSavePath, "%s", "/fault_record_data/");

		m_contin_dfu_param.nDfuport = COLLECTOR_DFU_LISTEN_PORT;
		m_contin_dfu_param.nIdleTime = COLLECTOR_IDLE_CHECEK_DEFAULT_TIME;
		m_contin_dfu_param.nCheckNewFileTime = COLLECTOR_COMMU_CHECK_NEW_FILE_TIME;
		sprintf(m_contin_dfu_param.chDfuAddr, "%s", "10.123.16.57");
		sprintf(m_contin_dfu_param.chFileSavePath, "%s", "/contin_record_data/");

		m_rabbit_mq_param.nCollectorRecvChannel = RABBIT_MQ_DEFAULT_CHANNEL_ID;
		sprintf(m_rabbit_mq_param.chCollectorRecvQueName, "%s", DATA_COLLECTOR_RECV_QUEUE_NAME);
		m_rabbit_mq_param.rabbitmq_basick_param.nserver_port = RABBIT_MQ_DEFAULT_ACCESS_PORT;
		m_rabbit_mq_param.rabbitmq_basick_param.nChannelID = RABBIT_MQ_DEFAULT_CHANNEL_ID;
		m_rabbit_mq_param.rabbitmq_basick_param.nChannelMax = RABBIT_MQ_DEFAULT_CHANNEL_MAX;
		m_rabbit_mq_param.rabbitmq_basick_param.nFrameMax = RABBIT_MQ_DEFAULT_FRAME_MAX;
		m_rabbit_mq_param.rabbitmq_basick_param.nHeartbeatTime = 0;
		sprintf(m_rabbit_mq_param.rabbitmq_basick_param.chhostname, "%s", "10.123.16.100");
		sprintf(m_rabbit_mq_param.rabbitmq_basick_param.chusrname, "%s", "admin");
		sprintf(m_rabbit_mq_param.rabbitmq_basick_param.chpassword, "%s", "admin");
		sprintf(m_rabbit_mq_param.rabbitmq_basick_param.chVhost, "%s", RABBIT_MQ_DEFAULT_V_HOST);

		m_mongo_access_param.nPort = 27017;
		sprintf(m_mongo_access_param.chAddr, "%s", "127.0.0.1");
		sprintf(m_mongo_access_param.chUser, "%s", "recordman");
		sprintf(m_mongo_access_param.chPasswd, "%s", "Don0tpanic42");
		sprintf(m_mongo_access_param.chDataBase, "%s", "recordman");
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[InitCollectorSysparam]init system param find unknown exception��");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::LoadCollectorSysParam()
{
	TiXmlDocument doc;
	TiXmlElement* pRootXmlElement = NULL;
	int nRet = -1;

	try
	{
		nRet = doc.LoadFile(RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
		if (0 != nRet)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadConfig]load config file��%s failed��reason��%s", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE, doc.GetErrorStr1());

			return false;
		}

		pRootXmlElement = doc.RootElement();
		if (NULL == pRootXmlElement)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadConfig]get root element from config file��%s failed", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			
			return false;
		}

		if (false == LoadSystemLogConfig(pRootXmlElement))
		{
			return false;
		}

		if (false == LoadRabbitMqConfig(pRootXmlElement))
		{
			return false;
		}

		if (false == LoadRabbitMqAdvanceConfig(pRootXmlElement))
		{
			return false;
		}

		if (false == LoadDfuCommuConfig(pRootXmlElement))
		{
			return false;
		}

		if (false == LoadContinDfuCommuConfig(pRootXmlElement))
		{
			return false;
		}

		if (false == LoadMongoDbConfig(pRootXmlElement))
		{
			return false;
		}
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[LoadConfig]get param from config file find exception��");
		return false;
	}

	return true;
}

char* CConfigVariableMgr::GetSysParam_LogPath()
{
	return m_collector_sys_param.chLogpath;
}

int CConfigVariableMgr::GetSysParam_LogLevel()
{
	return m_collector_sys_param.nLoglevel;
}

int CConfigVariableMgr::GetSysParam_LogDays()
{
	return m_collector_sys_param.nLogDays;
}

int CConfigVariableMgr::GetSysParam_RecTimeOut()
{
	return m_collector_sys_param.nRecvTimeout;
}

int CConfigVariableMgr::GetSysParam_SendTimeOut()
{
	return m_collector_sys_param.nSendTimeout;
}

COLLECTOR_DATA_SYS_PARAM* CConfigVariableMgr::GetSysParamHandler()
{
	return &m_collector_sys_param;
}

int CConfigVariableMgr::GetRabmqadParam_RevChannel()
{
	return m_rabbit_mq_param.nCollectorRecvChannel;
}

char* CConfigVariableMgr::GetRabmqadParam_RevQueName()
{
	return m_rabbit_mq_param.chCollectorRecvQueName;
}

char* CConfigVariableMgr::GetRabmqadParam_WebResultQueName()
{
	return m_rabbit_mq_param.chWebResultQueName;
}

int CConfigVariableMgr::GetRabmqBasicParam_ServerPort()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.nserver_port;
}

int CConfigVariableMgr::GetRabmqBasicParam_ChannelID()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.nChannelID;
}

int CConfigVariableMgr::GetRabmqBasicParam_ChannelMax()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.nChannelMax;
}

int CConfigVariableMgr::GetRabmqBasicParam_FrameMax()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.nFrameMax;
}

int CConfigVariableMgr::GetRabmqBasicParam_HeartbeatTime()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.nHeartbeatTime;
}

char* CConfigVariableMgr::GetRabmqBasicParam_Hostname()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.chhostname;
}

char* CConfigVariableMgr::GetRabmqBasicParam_Username()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.chusrname;
}

char* CConfigVariableMgr::GetRabmqBasicParam_Password()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.chpassword;
}

char* CConfigVariableMgr::GetRabmqBasicParam_Vhost()
{
	return m_rabbit_mq_param.rabbitmq_basick_param.chVhost;
}

RABBIT_MQ_BASIC_ACCESS_PARAM* CConfigVariableMgr::GetRabmqBasicParamHandle()
{
	return &m_rabbit_mq_param.rabbitmq_basick_param;
}

int CConfigVariableMgr::GetDfuCommuParam_Port(bool bFaultDfu /*= true*/)
{
	if (bFaultDfu == true)
	{
		return m_fault_dfu_param.nDfuport;
	}
	else
	{
		return m_contin_dfu_param.nDfuport;
	}
}

int CConfigVariableMgr::GetDfuCommuParam_Idletime(bool bFaultDfu /*= true*/)
{
	if (bFaultDfu == true)
	{
		return m_fault_dfu_param.nIdleTime;
	}
	else
	{
		return m_contin_dfu_param.nIdleTime;
	}
}

int CConfigVariableMgr::GetDfuCommuParam_ChecknewfileTime(bool bFaultDfu /*= true*/)
{
	if (bFaultDfu == true)
	{
		return m_fault_dfu_param.nCheckNewFileTime;
	}
	else
	{
		return m_contin_dfu_param.nCheckNewFileTime;
	}
}

char* CConfigVariableMgr::GetDfuCommuParam_Addr(bool bFaultDfu /*= true*/)
{
	if (bFaultDfu == true)
	{
		return m_fault_dfu_param.chDfuAddr;
	}
	else
	{
		return m_contin_dfu_param.chDfuAddr;
	}
}

char* CConfigVariableMgr::GetDfuCommuParam_FileSavePath(bool bFaultDfu /*= true*/)
{
	if (bFaultDfu == true)
	{
		return m_fault_dfu_param.chFileSavePath;
	}
	else
	{
		return m_contin_dfu_param.chFileSavePath;
	}
}

COLLECTOR_DFU_COMMU_PARAM* CConfigVariableMgr::GetDfuCommuParamHandler(bool bFaultDfu /*= true*/)
{
	if (bFaultDfu == true)
	{
		return &m_fault_dfu_param;
	}
	else
	{
		return &m_contin_dfu_param;
	}
}

int CConfigVariableMgr::GetMongoParam_Port()
{
	return m_mongo_access_param.nPort;
}

char* CConfigVariableMgr::GetMongoParam_Addr()
{
	return m_mongo_access_param.chAddr;
}

char* CConfigVariableMgr::GetMongoParam_User()
{
	return m_mongo_access_param.chUser;
}

char* CConfigVariableMgr::GetMongoParam_Passwd()
{
	return m_mongo_access_param.chPasswd;
}

char* CConfigVariableMgr::GetMongoParam_DbName()
{
	return m_mongo_access_param.chDataBase;
}

RECORD_MONGO_BASIC_PARAM* CConfigVariableMgr::GetMongoParamHandle()
{
	return &m_mongo_access_param;
}

bool CConfigVariableMgr::LoadSystemLogConfig(TiXmlElement* pRootXmlElement)
{
	TiXmlElement* pXmlElement = NULL;
	char chTempBuf[1024] = "";

	try
	{
		pXmlElement = pRootXmlElement->FirstChildElement("system_log_config");
		if (NULL == pXmlElement)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadSystemLogConfig]get system_log_config node from config file��%s failed��", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		bzero(m_collector_sys_param.chLogpath, sizeof(m_collector_sys_param.chLogpath));
		bzero(chTempBuf, sizeof(chTempBuf));
		GetCopyNodeValue(pXmlElement, "log_path", 
			chTempBuf, sizeof(chTempBuf));
		sprintf(m_collector_sys_param.chLogpath, "%s%s", 
			GetFormatPath(chTempBuf).c_str(), "data_collector_log");

		GetCopyNodeValue(pXmlElement, "log_level", 
			m_collector_sys_param.nLoglevel);
		GetCopyNodeValue(pXmlElement, "log_days", 
			m_collector_sys_param.nLogDays);
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[LoadSystemLogConfig]get log param find exception��");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::LoadRabbitMqConfig(TiXmlElement* pRootXmlElement)
{
	TiXmlElement* pXmlElement = NULL;

	try
	{
		pXmlElement = pRootXmlElement->FirstChildElement("rabbit_mq_base_config");
		if (NULL == pXmlElement)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadRabbitMqConfig]get rabbit_mq_base_config node from config file��%s failed��", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		bzero(m_rabbit_mq_param.rabbitmq_basick_param.chhostname, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chhostname));
		GetCopyNodeValue(pXmlElement, "addr", 
			m_rabbit_mq_param.rabbitmq_basick_param.chhostname, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chhostname));

		GetCopyNodeValue(pXmlElement, "port", 
			m_rabbit_mq_param.rabbitmq_basick_param.nserver_port);

		bzero(m_rabbit_mq_param.rabbitmq_basick_param.chusrname, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chusrname));
		GetCopyNodeValue(pXmlElement, "user", 
			m_rabbit_mq_param.rabbitmq_basick_param.chusrname, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chusrname));
		
		bzero(m_rabbit_mq_param.rabbitmq_basick_param.chpassword, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chpassword));
		GetCopyNodeValue(pXmlElement, "pwd", 
			m_rabbit_mq_param.rabbitmq_basick_param.chpassword, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chpassword));
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[LoadRabbitMqConfig]get rabbitmq param find exception��");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::LoadRabbitMqAdvanceConfig(TiXmlElement* pRootXmlElement)
{
	TiXmlElement* pXmlElement = NULL;

	try
	{
		pXmlElement = pRootXmlElement->FirstChildElement("rabbit_mq_advance_config");
		if (NULL == pXmlElement)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadRabbitMqAdvanceConfig]get rabbit_mq_advance_config node from config file��%s failed��", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		bzero(m_rabbit_mq_param.chCollectorRecvQueName, 
			sizeof(m_rabbit_mq_param.chCollectorRecvQueName));
		GetCopyNodeValue(pXmlElement, "collector_recv_queue", 
			m_rabbit_mq_param.chCollectorRecvQueName, 
			sizeof(m_rabbit_mq_param.chCollectorRecvQueName));

		bzero(m_rabbit_mq_param.chWebResultQueName, 
			sizeof(m_rabbit_mq_param.chWebResultQueName));
		GetCopyNodeValue(pXmlElement, "web_result_queue", 
			m_rabbit_mq_param.chWebResultQueName, 
			sizeof(m_rabbit_mq_param.chWebResultQueName));
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[LoadRabbitMqAdvanceConfig]get rabbit_mq_advance_config param find exception��");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::LoadDfuCommuConfig(TiXmlElement* pRootXmlElement)
{
	TiXmlElement* pXmlElement = NULL;

	try
	{
		pXmlElement = pRootXmlElement->FirstChildElement("dfu_commu_config");
		if (NULL == pXmlElement)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadDfuCommuConfig]get dfu_commu_config node from config file��%s failed��", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		bzero(m_fault_dfu_param.chDfuAddr, sizeof(m_fault_dfu_param.chDfuAddr));
		GetCopyNodeValue(pXmlElement, "addr", 
			m_fault_dfu_param.chDfuAddr, 
			sizeof(m_fault_dfu_param.chDfuAddr));

		GetCopyNodeValue(pXmlElement, "port", 
			m_fault_dfu_param.nDfuport);

		GetCopyNodeValue(pXmlElement, "idle_time", 
			m_fault_dfu_param.nIdleTime);

		GetCopyNodeValue(pXmlElement, "newfile_check_time", 
			m_fault_dfu_param.nCheckNewFileTime);

		bzero(m_fault_dfu_param.chFileSavePath, sizeof(m_fault_dfu_param.chFileSavePath));
		GetCopyNodeValue(pXmlElement, "save_path", 
			m_fault_dfu_param.chFileSavePath, 
			sizeof(m_fault_dfu_param.chFileSavePath));
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[LoadDfuCommuConfig]get dfu_commu_config param find exception��");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::LoadContinDfuCommuConfig(TiXmlElement* pRootXmlElement)
{
	TiXmlElement* pXmlElement = NULL;

	try
	{
		pXmlElement = pRootXmlElement->FirstChildElement("continue_dfu_commu_config");
		if (NULL == pXmlElement)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadContinDfuCommuConfig]get continue_dfu_commu_config node from config file��%s failed��", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		bzero(m_contin_dfu_param.chDfuAddr, sizeof(m_contin_dfu_param.chDfuAddr));
		GetCopyNodeValue(pXmlElement, "addr", 
			m_contin_dfu_param.chDfuAddr, 
			sizeof(m_contin_dfu_param.chDfuAddr));

		GetCopyNodeValue(pXmlElement, "port", 
			m_contin_dfu_param.nDfuport);

		GetCopyNodeValue(pXmlElement, "idle_time", 
			m_contin_dfu_param.nIdleTime);

		GetCopyNodeValue(pXmlElement, "newfile_check_time", 
			m_contin_dfu_param.nCheckNewFileTime);

		bzero(m_contin_dfu_param.chFileSavePath, sizeof(m_contin_dfu_param.chFileSavePath));
		GetCopyNodeValue(pXmlElement, "save_path", 
			m_contin_dfu_param.chFileSavePath, 
			sizeof(m_contin_dfu_param.chFileSavePath));
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[LoadContinDfuCommuConfig]get continue_dfu_commu_config param find exception��");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::LoadMongoDbConfig(TiXmlElement* pRootXmlElement)
{
	TiXmlElement* pXmlElement = NULL;

	try
	{
		pXmlElement = pRootXmlElement->FirstChildElement("mongo_db_config");
		if (NULL == pXmlElement)
		{
			m_pLogFile->FormatAdd(CLogFile::error, 
				"[LoadMongoDbConfig]get mongo_db_config node from config file��%s failed��", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		bzero(m_mongo_access_param.chAddr, sizeof(m_mongo_access_param.chAddr));
		GetCopyNodeValue(pXmlElement, "addr", 
			m_mongo_access_param.chAddr, 
			sizeof(m_mongo_access_param.chAddr));

		GetCopyNodeValue(pXmlElement, "port", 
			m_mongo_access_param.nPort);

		bzero(m_mongo_access_param.chDataBase, sizeof(m_mongo_access_param.chDataBase));
		GetCopyNodeValue(pXmlElement, "name", 
			m_mongo_access_param.chDataBase, 
			sizeof(m_mongo_access_param.chDataBase));

		bzero(m_mongo_access_param.chUser, sizeof(m_mongo_access_param.chUser));
		GetCopyNodeValue(pXmlElement, "user", 
			m_mongo_access_param.chUser, 
			sizeof(m_mongo_access_param.chUser));

		bzero(m_mongo_access_param.chPasswd, sizeof(m_mongo_access_param.chPasswd));
		GetCopyNodeValue(pXmlElement, "pwd", 
			m_mongo_access_param.chPasswd, 
			sizeof(m_mongo_access_param.chPasswd));
	}
	catch (...)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"[LoadContinDfuCommuConfig]get continue_dfu_commu_config param find exception��");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, int& nParam, int nParamSize)
{
	TiXmlElement* pXMlChild = NULL;
	const char* node_value = NULL;

	pXMlChild = pRootNode->FirstChildElement(pNodeName);
	if (NULL == pXMlChild)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"node��%s not exists in config file��", 
			pNodeName);
		return false;
	}

	node_value = pXMlChild->GetText();
	if (NULL != node_value)
	{
		nParam = atoi(node_value);
	}
	else
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"node��%s value is null��", 
			pNodeName);
	}

	return true;
}

bool CConfigVariableMgr::GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, char* pParam, int nParamSize)
{
	TiXmlElement* pXMlChild = NULL;
	const char* node_value = NULL;

	if (NULL == pParam)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"node��%s local param is not point��", 
			pNodeName);
		
		return false;
	}
	
	pXMlChild = pRootNode->FirstChildElement(pNodeName);
	if (NULL == pXMlChild)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"node��%s not exists in config file��", 
			pNodeName);
		return false;
	}
	
	node_value = pXMlChild->GetText();
	if (NULL == node_value)
	{
		m_pLogFile->FormatAdd(CLogFile::error, 
			"node��%s value is null��", 
			pNodeName);
	}

	int nNodeValueLen = (strlen(node_value) < nParamSize)?strlen(node_value):(nParamSize-1);
	memcpy(pParam, node_value, nNodeValueLen);

	return true;
}
