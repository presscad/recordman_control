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

/**	\brief 命令接收通道号*/
const int DATA_COLLECTOR_COMMAND_CHANNEL = 1;
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

/**
* @brief       数据库访问动态库名定义
* @author      pengl
* @version     ver1.0
* @date        07/13/2008
*
*/
#ifdef OS_WINDOWS
	#define LIBMYSAL      "DBAccessMysql.dll"//mysql数据库访问动态库
	#define LIBSQLSERVER  "DBAccessSqlserver.dll"//sqlserver数据库访问动态库
	#define LIBORACLE     "DBAccessOracle.dll"//oracle数据库访问动态库
	#define LIBDAMENG     "DBAccessDaMeng.dll"//达梦数据库访问动态库
	#define LIBKINGBASE   "DBAccessKingbase.dll"//金仓数据库访问动态库
	#define LIBFTPCLIENTMANAGER "XJFtpClientManager.dll"//ftp客户端操作库
	#define LIBXJSTTPNET  "XJSttpNet.dll"//sttpnet动态库
	#define LIBSTTPXMLTRANSFORM "XJSttpXmlTransform.dll"//sttp与xml互转动态库
#else
	#define LIBMYSAL      "libDBAccessMysql.so"
	#define LIBSQLSERVER  "libDBAccessSqlserver.so"
	#define LIBORACLE     "libDBAccessOracle.so"
	#define LIBDAMENG     "libDBAccessDaMeng.so"//达梦数据库访问动态库
	#define LIBKINGBASE   "libDBAccessKingbase.so"//达梦数据库访问动态库
	#define LIBFTPCLIENTMANAGER "libXJFtpClientManager.so"//ftp客户端操作库
	#define LIBXJSTTPNET  "libXJSttpNet.so"//sttpnet动态库
	#define LIBSTTPXMLTRANSFORM "libXJSttpXmlTransform.so"//sttp与xml互转动态库
#endif

//系统参数
typedef struct _COLLECTOR_DATA_SYS_PARAM
{
	int nLoglevel;//日志级别
	int nLogDays;//日志保留天数
	int nDfuPort;//dfu监听端口
	int nIdleCheckTime;//空闲巡检时间
	int nRecvTimeout;//接收超时
	int nSendTimeout;//发送超时
	char chLogpath[MAX_LINE_LENGTH];//日志保存路径
	char chDfuAddr[MAX_FIELD_NAME_LEN];//dfu监听端口

	_COLLECTOR_DATA_SYS_PARAM()
	{
		nLoglevel = CLogFile::trace;
		nLogDays = COLLECTOR_LOG_SAVE_DEFAULT_DAYS;
		nDfuPort = COLLECTOR_DFU_LISTEN_PORT;
		nIdleCheckTime = COLLECTOR_IDLE_CHECEK_DEFAULT_TIME;
		nRecvTimeout = COLLECTOR_COMMU_RECV_TIMEOUT;
		nSendTimeout = COLLECTOR_COMMU_SEND_TIMEOUT;
		bzero(chLogpath, sizeof(chLogpath));
		bzero(chDfuAddr, sizeof(chDfuAddr));
	}

}COLLECTOR_DATA_SYS_PARAM;

/////////////////////////////////////////////////////////////////////////
#endif
