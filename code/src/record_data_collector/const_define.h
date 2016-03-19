#ifndef RECORD_CONSTANT_DEFINE_H
#define RECORD_CONSTANT_DEFINE_H

#pragma warning(disable : 4786)//屏蔽4786错误

#include "../../common/sys_define.h"
#include "../../common/common_lib.h"
#include "../../common/CCommonFile.h"
#include "../../common/LogFile.h"
#include "../../common/Net.h"
#include "../../common/StrongString.h"
#include "../../common/Lock.h"
#include "../../common/MessageLog.h"
#include "../../common/RecordmanThread.h"
#include "../../common/DfuMsgParser.h"
#include "GlobalFunction.h"

/**	\brief 日志默认保存天数*/
const int COLLECTOR_LOG_SAVE_DEFAULT_DAYS = 5;
/**	\brief 日志默认大小*/
const int COLLECTOR_LOG_DEFAULT_SIZE = 5*1024;
/**	\brief DFU监听端口*/
const int COLLECTOR_DFU_LISTEN_PORT = 3309;
/**	\brief 数据库连接个数*/
const int COLLECTOR_DB_DEFAULT_CONN_NUM = 2;
/**	\brief 空闲巡检时间*/
const int COLLECTOR_IDLE_CHECEK_DEFAULT_TIME = 10;
/**	\brief 接收超时*/
const int COLLECTOR_COMMU_RECV_TIMEOUT = 90*1000;
/**	\brief 发送超时*/
const int COLLECTOR_COMMU_SEND_TIMEOUT = 90*1000;

/**	\brief 日志默认路径*/
#define COLLECTOR_LOG_DEFAULT_PATH "./default_log/"

/**	\brief 命令接收队列名*/
const char DATA_COLLECTOR_RECV_QUEUE_NAME[] = "DATA_COLLECTOR_COMMAND_QUEUE";

/**	\brief 报文定义*/
const int RECORD_DATA_MSG_ID_CALL_DFU_CONFIG = 20001;
const int RECORD_DATA_MSG_ID_RESULT_CALL_DFU_CONFIG = 20002;
const int RECORD_DATA_MSG_ID_CALL_SETTING = 20003;
const int RECORD_DATA_MSG_ID_RESULT_CALL_SETTING = 20004;
const int RECORD_DATA_MSG_ID_SWITCH_ZONE = 20005;
const int RECORD_DATA_MSG_ID_RESULT_SWITCH_ZONE = 20006;
const int RECORD_DATA_MSG_ID_CALL_ZONE = 20007;
const int RECORD_DATA_MSG_ID_RESULT_CALL_ZONE = 20008;
const int RECORD_DATA_MSG_ID_QUERY_SELFCHECK = 20009;
const int RECORD_DATA_MSG_ID_RESULT_QUERY_SELFCHECK = 20010;
const int RECORD_DATA_MSG_ID_QUERY_VER = 20011;
const int RECORD_DATA_MSG_ID_RESULT_QUERY_VER = 20012;
const int RECORD_DATA_MSG_ID_QUERY_TIME = 20013;
const int RECORD_DATA_MSG_ID_RESULT_QUERY_TIME = 20014;
const int RECORD_DATA_MSG_ID_QUERY_MOULE = 20015;
const int RECORD_DATA_MSG_ID_RESULT_QUERY_MOULE = 20016;
const int RECORD_DATA_MSG_ID_RESET_DEV = 20017;
const int RECORD_DATA_MSG_ID_RESULT_RESET_DEV = 20018;
const int RECORD_DATA_MSG_ID_SET_NET_PARAM = 20019;
const int RECORD_DATA_MSG_ID_RESULT_SET_NET_PARAM = 20020;
const int RECORD_DATA_MSG_ID_SET_TIME = 20021;
const int RECORD_DATA_MSG_ID_RESULT_SET_TIME = 20022;
const int RECORD_DATA_MSG_ID_SET_TIME_ZONE = 20023;
const int RECORD_DATA_MSG_ID_RESULT_SET_TIME_ZONE = 20024;
const int RECORD_DATA_MSG_ID_DOWN_DFU_CONFIG = 20060;
const int RECORD_DATA_MSG_ID_RESULT_DOWN_DFU_CONFIG = 20061;
const int RECORD_DATA_MSG_ID_DOWN_MGR_CONFIG = 20070;
const int RECORD_DATA_MSG_ID_RESULT_DOWN_MGR_CONFIG = 20071;
const int RECORD_DATA_MSG_ID_UPLOAD_NEW_OSC_FILE = 20090;

//配置文件
#define RECORD_MANAGEMENT_BOARD_CONFIG_FILE "../../conf/recordman_manager_config.xml"

//dfu配置
typedef struct _COLLECTOR_DFU_COMMU_PARAM
{
	char chDfuAddr[MAX_FIELD_NAME_LEN];//dfu监听端口
	int nDfuport;

	_COLLECTOR_DFU_COMMU_PARAM()
	{
		bzero(chDfuAddr, sizeof(chDfuAddr));
		nDfuport = COLLECTOR_DFU_LISTEN_PORT;
	}

}COLLECTOR_DFU_COMMU_PARAM;

//系统参数
typedef struct _COLLECTOR_DATA_SYS_PARAM
{
	//日志级别
	int nLoglevel;
	//日志保留天数
	int nLogDays;
	//空闲巡检时间
	int nIdleCheckTime;
	//接收超时
	int nRecvTimeout;
	//发送超时
	int nSendTimeout;
	//日志保存路径
	char chLogpath[MAX_LINE_LENGTH];
	//故障dfu参数
	COLLECTOR_DFU_COMMU_PARAM fault_dfu_param;
	//连续录波dfu参数
	COLLECTOR_DFU_COMMU_PARAM contin_dfu_param;

	_COLLECTOR_DATA_SYS_PARAM()
	{
		nLoglevel = CLogFile::trace;
		nLogDays = COLLECTOR_LOG_SAVE_DEFAULT_DAYS;
		nIdleCheckTime = COLLECTOR_IDLE_CHECEK_DEFAULT_TIME;
		nRecvTimeout = COLLECTOR_COMMU_RECV_TIMEOUT;
		nSendTimeout = COLLECTOR_COMMU_SEND_TIMEOUT;
		bzero(chLogpath, sizeof(chLogpath));
	}

}COLLECTOR_DATA_SYS_PARAM;

typedef struct _COLLECTOR_ADVANCE_RABBITMQ_PARAM
{
	//接收通道号
	int nCollectorRecvChannel;
	//采集进程接收队列名称
	char chCollectorRecvQueName[FILE_NAME_MAX_LEN];
	//web结果队列名称
	char chWebResultQueName[FILE_NAME_MAX_LEN];
	//基本参数
	RABBIT_MQ_BASIC_ACCESS_PARAM rabbitmq_basick_param;

	_COLLECTOR_ADVANCE_RABBITMQ_PARAM()
	{
		nCollectorRecvChannel = RABBIT_MQ_DEFAULT_CHANNEL_ID;
		bzero(chCollectorRecvQueName, sizeof(chCollectorRecvQueName));
		bzero(chWebResultQueName, sizeof(chWebResultQueName));
	}

}COLLECTOR_ADVANCE_RABBITMQ_PARAM;

/////////////////////////////////////////////////////////////////////////
#endif
