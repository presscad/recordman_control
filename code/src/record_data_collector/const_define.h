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
#include "GlobalFunction.h"
#include "../../common_open_source/rabbitmq/include/amqp.h"
#include "../../common_open_source/rabbitmq/include/amqp_framing.h"
#include "../../common_open_source/rabbitmq/include/amqp_tcp_socket.h"
#include "../../common_open_source/rabbitmq/include/amqp_ssl_socket.h"

/**	\brief 日志默认保存天数*/
const int COLLECTOR_LOG_SAVE_DEFAULT_DAYS = 2;
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
/**	\brief 命令接收队列名*/
const char DATA_COLLECTOR_RECV_QUEUE_NAME[] = "DATA_COLLECTOR_COMMAND_QUEUE";
/**	\brief 配置文件*/
#define RECORD_MANAGEMENT_BOARD_CONFIG_FILE "../../conf/recordman_manager_config.xml"
/**	\brief 日志根目录*/
#define MANAGER_BOARD_DEFAULT_ROOT_LOG "./data_collector_log/"

//dfu msg type
/** @brief           json command*/
const int RECORD_DFU_MESSAGE_TYPE_JSON = 0;
/** @brief           osc list query command*/
const int RECORD_DFU_MESSAGE_TYPE_OSC_LIST = 1;
/** @brief           osc file query command*/
const int RECORD_DFU_MESSAGE_TYPE_OSC_FILE = 2;
/** @brief           test command*/
const int RECORD_DFU_MESSAGE_TYPE_TEST = 3;
/** @brief           manual file*/
const int RECORD_DFU_MESSAGE_TYPE_MANUAL_FILE = 4;

//command result
//result is unknown
const int RECORD_COMMAND_RESULT_UNKNOWN = -1;
//result is normal
const int RECORD_COMMAND_RESULT_NORMAL = 0;
//result is failed
const int RECORD_COMMAND_RESULT_FAILED = 1;

//data type
//data type-bool
const int RECORD_DFU_SETTING_DATATYPE_BOOL = 0;
//data type-float32
const int RECORD_DFU_SETTING_DATATYPE_FOLAT32 = 1;
//data type-int
const int RECORD_DFU_SETTING_DATATYPE_INT = 2;
//data type-uint
const int RECORD_DFU_SETTING_DATATYPE_UINT = 3;

//dfu msg header num
const int RECORD_DFU_MSG_HEADER_OFFSET = 18;


/** @brief           启动码*/
const BYTE RECORD_COMMU_CHAR_START_VAR = 0xA8;
/** @brief           结束码*/
const BYTE RECORD_COMMU_CHAR_END_VAR = 0x5F;
/** @brief           协议码第一位*/
const BYTE RECORD_COMMU_CHAR_PROTOCOL_START_VAR = 0x61;
/** @brief           协议码第二位*/
const BYTE RECORD_COMMU_CHAR_PROTOCOL_END_VAR = 0x01;
/** @brief           掩码*/
const WORD DEFAULT_LENGTH_MASK = 0x0fff;
/** @brief           报文体最大长度(1400)*/
const UINT MAX_RECORD_MSG_LEN = 0x578;
/*  record private commu protocol  */
const int DEFINE_COMMU_PROTOCOL_RECORD_SELF = 1;
/** @brief           BYTE定义*/
/** @brief           巡检命令*/
const BYTE RECORD_COMMAND_CHAR_PATROL_VAR = 0x01;
/** @brief           配置命令*/
const BYTE RECORD_COMMAND_CHAR_CONFIG_SEND_VAR = 0x11;
/** @brief           配置命令*/
const BYTE RECORD_COMMAND_CHAR_CONFIG_RECV_VAR = 0x12;
/** @brief           定值整定*/
const BYTE RECORD_COMMAND_CHAR_SETTING_ADJUST_VAR = 0x21;
/** @brief           定值读取*/
const BYTE RECORD_COMMAND_CHAR_SETTING_READ_VAR = 0x22;
/** @brief           定值区切换*/
const BYTE RECORD_COMMAND_CHAR_SETZONE_CHANGE_VAR = 0x23;
/** @brief           活动定值区读取*/
const BYTE RECORD_COMMAND_CHAR_CURZONE_READ_VAR = 0x24;
/** @brief           新录波查询*/
const BYTE RECORD_COMMAND_CHAR_NEW_OSC_QUERY_VAR = 0x31;
/** @brief           录波索引读取*/
const BYTE RECORD_COMMAND_CHAR_OSC_INDEX_READ_VAR = 0x32;
/** @brief           录波读取*/
const BYTE RECORD_COMMAND_CHAR_OSC_FILE_READ_VAR = 0x33;
/** @brief           手动录波*/
const BYTE RECORD_COMMAND_CHAR_MANUAL_OSC_VAR = 0x3A;
/** @brief           实时波形读取*/
const BYTE RECORD_COMMAND_CHAR_REALTIME_OSC_READ_VAR = 0x41;
/** @brief           实时数据读取*/
const BYTE RECORD_COMMAND_CHAR_REALTIME_DATA_READ_VAR = 0x42;
/** @brief           自检查询*/
const BYTE RECORD_COMMAND_CHAR_SELF_CHECK_VAR = 0x91;
/** @brief           版本查询*/
const BYTE RECORD_COMMAND_CHAR_VERSION_QUERY_VAR = 0x92;
/** @brief           时间查询*/
const BYTE RECORD_COMMAND_CHAR_TIME_QUERY_VAR = 0x93;
/** @brief           子模块信息查询*/
const BYTE RECORD_COMMAND_CHAR_SUB_MODULE_QUERY_VAR = 0x94;
/** @brief           装置复位*/
const BYTE RECORD_COMMAND_CHAR_RESET_VAR = 0xA0;
/** @brief           写ip地址*/
const BYTE RECORD_COMMAND_CHAR_IP_SET_VAR = 0xA1;
/** @brief           时间设置*/
const BYTE RECORD_COMMAND_CHAR_TIME_SET_VAR = 0xA2;
/** @brief           时区设置*/
const BYTE RECORD_COMMAND_CHAR_TIMEZONE_SET_VAR = 0xA3;

#define RECORD_SWAP_16(b)\
{\
	unsigned short&a =  *(unsigned short*)(&(b));\
	a = (a>>8)|(a<<8);\
}

#define RECORD_SWAP_32(b)\
{\
	unsigned &a = *(unsigned *)(&(b));\
	a = (a>>24)|((a>>8)&0xFF00)|((a<<8)&0xFF0000)|(a<<24);\
}

/** @brief           dfu msg*/
typedef vector<BYTE> DFU_COMMU_MSG;

//dfu配置
typedef struct _COLLECTOR_DFU_COMMU_PARAM
{
	int nDfuport;
	int nIdleTime;
	int nCheckNewFileTime;
	char chDfuAddr[MAX_FIELD_NAME_LEN];//dfu监听端口
	char chFileSavePath[MAX_LINE_LENGTH];

	_COLLECTOR_DFU_COMMU_PARAM()
	{
		bzero(chDfuAddr, sizeof(chDfuAddr));
		bzero(chFileSavePath, sizeof(chFileSavePath));
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

//rabbit advance param
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

//dfu msg struct
typedef struct _DFUMESSAGE
{
	//recv end msg
	bool bRecvEnd;

	//is processed
	bool bProcessed;

	//msg type
	int nMsgType;

	//trans mask
	int nTransMask;

	//dfu command id
	int nDfuCommandID;

	//command num
	int nCommandNum;

	//command execute result
	int nCommandProcessResult;

	//internal command id
	int nInternalCommandID;

	//msg send time
	time_t tSendTime;

	//command msg list
	vector<DFU_COMMU_MSG> command_msg;

	//result msg list
	vector<DFU_COMMU_MSG> result_msg;

	_DFUMESSAGE()
	{
		bRecvEnd = false;
		bProcessed = false;
		nMsgType = -1;
		nTransMask = -1;
		nDfuCommandID = -1;
		nCommandNum = -1;
		nInternalCommandID = -1;
		nCommandProcessResult = RECORD_COMMAND_RESULT_UNKNOWN;//-1未知，0正常，1错误
		command_msg.clear();
		result_msg.clear();
	}
}DFUMESSAGE;

typedef vector<DFUMESSAGE> DFUMESSAGE_BUF;

typedef vector<DFU_COMMU_MSG> DFUCOMMUMSG_BUF;

typedef int (*PRESULTDFUMSGCALLBACKFUNC)(DFU_COMMU_MSG& result_msg, XJHANDLE pReserved);

/////////////////////////////////////////////////////////////////////////
#endif
