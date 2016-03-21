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
	CConfigVariableMgr(void);
	~CConfigVariableMgr(void);

public:
/**	\brief 系统参数*/
	COLLECTOR_DATA_SYS_PARAM m_collector_sys_param;

/**	\brief rabbit参数*/
	COLLECTOR_ADVANCE_RABBITMQ_PARAM m_rabbit_mq_param;

	//fault osc dfu
	COLLECTOR_DFU_COMMU_PARAM m_fault_dfu_param;

	//continue osc dfu
	COLLECTOR_DFU_COMMU_PARAM m_contin_dfu_param;

	//mogodb access param
	RECORD_MONGO_BASIC_PARAM m_mongo_access_param;

private:
	//log handler
	CMessageLog* m_pLogFile;

public:
	//set log handler
	void SetLogAccessHandler(CMessageLog* pLog);

	//初始化参数
	//true：成功 false：失败
	bool InitCollectorSysparam();

	//从配置文件中读取参数
	//true：成功 false：失败
	bool LoadCollectorSysParam();

private:
	bool LoadSystemLogConfig(TiXmlElement* pRootXmlElement);

	bool LoadRabbitMqConfig(TiXmlElement* pRootXmlElement);

	bool LoadRabbitMqAdvanceConfig(TiXmlElement* pRootXmlElement);

	bool LoadDfuCommuConfig(TiXmlElement* pRootXmlElement);

	bool LoadContinDfuCommuConfig(TiXmlElement* pRootXmlElement);

	bool LoadMongoDbConfig(TiXmlElement* pRootXmlElement);

private:
/*
 *  @brief   	GetCopyNodeValue	 读取指定子节点值 
 *  @param 		[In]a param of Type  TiXmlElement*  父节点指针
 *  @param 		[In]a param of Type  const char*  子节点名
 *  @param 		[Out]a param of Type  int&  数字型的值
 *  @param 		[In]a param of Type  int&  长度
 *  @return 	bool true：成功 false：失败
 */
	bool GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, int& nParam, int nParamSize = -1);

/*
 *  @brief   	GetCopyNodeValue	 读取指定子节点值 
 *  @param 		[In]a param of Type  TiXmlElement*  父节点指针
 *  @param 		[In]a param of Type  const char*  子节点名
 *  @param 		[Out]a param of Type  int&  字符型的值
 *  @param 		[Out]a param of Type  int  允许存储的最大长度
 *  @return 	bool true：成功 false：失败
 */
	bool GetCopyNodeValue(TiXmlElement* pRootNode, const char* pNodeName, char* pParam, int nParamSize);
};

#endif
