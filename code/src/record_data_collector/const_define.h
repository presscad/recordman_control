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
const int COLLECTOR_COMMU_RECV_TIMEOUT = 3*1000;
/**	\brief 发送超时*/
const int COLLECTOR_COMMU_SEND_TIMEOUT = 3*1000;
/**	\brief 新文件检查时间*/
const int COLLECTOR_COMMU_CHECK_NEW_FILE_TIME = 2;

/**	\brief 日志默认路径*/
#define COLLECTOR_LOG_DEFAULT_PATH "./default_log/"

/**	\brief 命令接收队列名*/
const char DATA_COLLECTOR_RECV_QUEUE_NAME[] = "DATA_COLLECTOR_COMMAND_QUEUE";

//配置文件
#define RECORD_MANAGEMENT_BOARD_CONFIG_FILE "../../conf/recordman_manager_config.xml"

//dfu配置
typedef struct _COLLECTOR_DFU_COMMU_PARAM
{
	char chDfuAddr[MAX_FIELD_NAME_LEN];//dfu监听端口
	int nDfuport;
	int nIdleTime;
	int nCheckNewFileTime;

	_COLLECTOR_DFU_COMMU_PARAM()
	{
		bzero(chDfuAddr, sizeof(chDfuAddr));
		nDfuport = COLLECTOR_DFU_LISTEN_PORT;
		nIdleTime = COLLECTOR_IDLE_CHECEK_DEFAULT_TIME;
		nCheckNewFileTime = COLLECTOR_COMMU_CHECK_NEW_FILE_TIME;
	}

}COLLECTOR_DFU_COMMU_PARAM;

//系统参数
typedef struct _COLLECTOR_DATA_SYS_PARAM
{
	//日志级别
	int nLoglevel;
	//日志保留天数
	int nLogDays;
	//接收超时
	int nRecvTimeout;
	//发送超时
	int nSendTimeout;
	//日志保存路径
	char chLogpath[MAX_LINE_LENGTH];

	_COLLECTOR_DATA_SYS_PARAM()
	{
		nLoglevel = CLogFile::trace;
		nLogDays = COLLECTOR_LOG_SAVE_DEFAULT_DAYS;
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
