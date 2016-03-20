#include "ConfigVariableMgr.h"


CConfigVariableMgr::CConfigVariableMgr(void)
{
}


CConfigVariableMgr::~CConfigVariableMgr(void)
{
}

//************************************
// Method:    InitCollectorSysparam
// FullName:  CConfigVariableMgr::InitCollectorSysparam
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 初始化参数
//************************************
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

		m_contin_dfu_param.nDfuport = COLLECTOR_DFU_LISTEN_PORT;
		m_contin_dfu_param.nIdleTime = COLLECTOR_IDLE_CHECEK_DEFAULT_TIME;
		m_contin_dfu_param.nCheckNewFileTime = COLLECTOR_COMMU_CHECK_NEW_FILE_TIME;
		sprintf(m_contin_dfu_param.chDfuAddr, "%s", "10.123.16.57");

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
		printf("[InitCollectorSysparam]init system param find unknown exception！\n");
		return false;
	}

	return true;
}

//************************************
// Method:    LoadCollectorSysParam
// FullName:  CConfigVariableMgr::LoadCollectorSysParam
// Access:    public 
// Returns:   bool true：成功 false：失败
// Qualifier: 从配置文件中读取参数
//************************************
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
			printf("[LoadConfig]load config file：%s failed，reason：%s\n", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE, doc.GetErrorStr1());

			return false;
		}

		pRootXmlElement = doc.RootElement();
		if (NULL == pRootXmlElement)
		{
			printf("[LoadConfig]get root element from config file：%s failed\n", 
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
		printf("[LoadConfig]get param from config file find exception！\n");
		return false;
	}

	return true;
}

bool CConfigVariableMgr::LoadSystemLogConfig(TiXmlElement* pRootXmlElement)
{
	TiXmlElement* pXmlElement = NULL;

	try
	{
		pXmlElement = pRootXmlElement->FirstChildElement("system_log_config");
		if (NULL == pXmlElement)
		{
			printf("[LoadSystemLogConfig]get system_log_config node from config file：%s failed！\n", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		GetCopyNodeValue(pXmlElement, "log_path", 
			m_collector_sys_param.chLogpath, sizeof(m_collector_sys_param.chLogpath));
		GetCopyNodeValue(pXmlElement, "log_level", 
			m_collector_sys_param.nLoglevel);
		GetCopyNodeValue(pXmlElement, "log_days", 
			m_collector_sys_param.nLogDays);
	}
	catch (...)
	{
		printf("[LoadSystemLogConfig]get log param find exception！\n");
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
			printf("[LoadRabbitMqConfig]get rabbit_mq_base_config node from config file：%s failed！\n", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		GetCopyNodeValue(pXmlElement, "addr", 
			m_rabbit_mq_param.rabbitmq_basick_param.chhostname, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chhostname));

		GetCopyNodeValue(pXmlElement, "port", 
			m_rabbit_mq_param.rabbitmq_basick_param.nserver_port);

		GetCopyNodeValue(pXmlElement, "user", 
			m_rabbit_mq_param.rabbitmq_basick_param.chusrname, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chusrname));
		
		GetCopyNodeValue(pXmlElement, "pwd", 
			m_rabbit_mq_param.rabbitmq_basick_param.chpassword, 
			sizeof(m_rabbit_mq_param.rabbitmq_basick_param.chpassword));
	}
	catch (...)
	{
		printf("[LoadRabbitMqConfig]get rabbitmq param find exception！\n");
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
			printf("[LoadRabbitMqAdvanceConfig]get rabbit_mq_advance_config node from config file：%s failed！\n", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		GetCopyNodeValue(pXmlElement, "collector_recv_queue", 
			m_rabbit_mq_param.chCollectorRecvQueName, 
			sizeof(m_rabbit_mq_param.chCollectorRecvQueName));

		GetCopyNodeValue(pXmlElement, "web_result_queue", 
			m_rabbit_mq_param.chWebResultQueName, 
			sizeof(m_rabbit_mq_param.chWebResultQueName));
	}
	catch (...)
	{
		printf("[LoadRabbitMqAdvanceConfig]get rabbit_mq_advance_config param find exception！\n");
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
			printf("[LoadDfuCommuConfig]get dfu_commu_config node from config file：%s failed！\n", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		GetCopyNodeValue(pXmlElement, "addr", 
			m_fault_dfu_param.chDfuAddr, 
			sizeof(m_fault_dfu_param.chDfuAddr));

		GetCopyNodeValue(pXmlElement, "port", 
			m_fault_dfu_param.nDfuport);

		GetCopyNodeValue(pXmlElement, "idle_time", 
			m_fault_dfu_param.nIdleTime);

		GetCopyNodeValue(pXmlElement, "newfile_check_time", 
			m_fault_dfu_param.nCheckNewFileTime);
	}
	catch (...)
	{
		printf("[LoadDfuCommuConfig]get dfu_commu_config param find exception！\n");
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
			printf("[LoadContinDfuCommuConfig]get continue_dfu_commu_config node from config file：%s failed！\n", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		GetCopyNodeValue(pXmlElement, "addr", 
			m_contin_dfu_param.chDfuAddr, 
			sizeof(m_contin_dfu_param.chDfuAddr));

		GetCopyNodeValue(pXmlElement, "port", 
			m_contin_dfu_param.nDfuport);

		GetCopyNodeValue(pXmlElement, "idle_time", 
			m_contin_dfu_param.nIdleTime);

		GetCopyNodeValue(pXmlElement, "newfile_check_time", 
			m_contin_dfu_param.nCheckNewFileTime);
	}
	catch (...)
	{
		printf("[LoadContinDfuCommuConfig]get continue_dfu_commu_config param find exception！\n");
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
			printf("[LoadMongoDbConfig]get mongo_db_config node from config file：%s failed！\n", 
				RECORD_MANAGEMENT_BOARD_CONFIG_FILE);
			return false;
		}

		GetCopyNodeValue(pXmlElement, "addr", 
			m_mongo_access_param.chAddr, 
			sizeof(m_mongo_access_param.chAddr));

		GetCopyNodeValue(pXmlElement, "port", 
			m_mongo_access_param.nPort);

		GetCopyNodeValue(pXmlElement, "name", 
			m_mongo_access_param.chDataBase, 
			sizeof(m_mongo_access_param.chDataBase));

		GetCopyNodeValue(pXmlElement, "user", 
			m_mongo_access_param.chUser, 
			sizeof(m_mongo_access_param.chUser));

		GetCopyNodeValue(pXmlElement, "pwd", 
			m_mongo_access_param.chPasswd, 
			sizeof(m_mongo_access_param.chPasswd));
	}
	catch (...)
	{
		printf("[LoadContinDfuCommuConfig]get continue_dfu_commu_config param find exception！\n");
		return false;
	}

	return true;
}

//************************************
// 函 数 名:  GetCopyNodeValue
// 功能概要:  读取指定子节点值
// 访问权限:  private 
// 返 回 值:  bool true：成功 false：失败
// 参    数:  TiXmlElement * pRootNode 父节点指针
// 参    数:  const char * pNodeName 子节点名
// 参    数:  int & nParam 数字型的值
// 参    数:  int nParamSize 长度
//************************************
bool CConfigVariableMgr::GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, int& nParam, int nParamSize)
{
	TiXmlElement* pXMlChild = NULL;
	const char* node_value = NULL;

	pXMlChild = pRootNode->FirstChildElement(pNodeName);
	if (NULL == pXMlChild)
	{
		printf("node：%s not exists in config file！\n", 
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
		printf("node：%s value is null！\n", 
			pNodeName);
	}

	return true;
}

//************************************
// 函 数 名:  GetCopyNodeValue
// 功能概要:  读取指定子节点值
// 访问权限:  private 
// 返 回 值:  bool true：成功 false：失败
// 参    数:  TiXmlElement * pRootNode 父节点指针
// 参    数:  const char * pNodeName 子节点名
// 参    数:  int & nParam 字符型的值
// 参    数:  int nParam nParamSize
//************************************
bool CConfigVariableMgr::GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, char* pParam, int nParamSize)
{
	TiXmlElement* pXMlChild = NULL;
	const char* node_value = NULL;

	if (NULL == pParam)
	{
		printf("node：%s local param is not point！\n", 
			pNodeName);
		
		return false;
	}
	
	pXMlChild = pRootNode->FirstChildElement(pNodeName);
	if (NULL == pXMlChild)
	{
		printf("node：%s not exists in config file！\n", 
			pNodeName);
		return false;
	}
	
	node_value = pXMlChild->GetText();
	if (NULL == node_value)
	{
		printf("node：%s value is null！\n", 
			pNodeName);
	}

	int nNodeValueLen = (strlen(node_value) < nParamSize)?strlen(node_value):(nParamSize-1);
	memcpy(pParam, node_value, nNodeValueLen);

	return true;
}