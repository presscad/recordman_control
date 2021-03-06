//
// sys_define.h
//
// This is a sys_define and include file
//
// Copyright (C) 2015
// Version 1.0
//

#ifndef SYS_COMMON_DEFINE_H
#define SYS_COMMON_DEFINE_H

#pragma warning(disable : 4786)//屏蔽4786错误

#ifdef OS_HPUNIX
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 1
#endif
#endif

// c_runtime lib
#include <stdio.h>  
#include <stdlib.h>  
#include <time.h>
#include <signal.h>
#ifdef OS_AIX
	#ifndef _THREAD_SAFE
	#define _THREAD_SAFE 1
	#endif
#endif
#include <errno.h>
#include <string.h>  
#include <ctype.h>
#include <stddef.h>
#include <stdarg.h>

//standard c++ lib
#include <iostream>
#include <strstream>
#include <string>
#include <iterator>
#include <cassert>
#include <fstream>
#include <list>
#include <deque>
#include <map>
#include <numeric>            
#include <set>
#include <stack>
#include <vector>
#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <complex>
#include <cctype>             
#include <new>
#include <cstdarg>             
#include <utility>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <cerrno>
#include <csignal>
#include <ctime>

/* ---------------------------------------- */

using namespace std;

#ifdef OS_SUNUNIX
	#include <sys/sockio.h>
	#include <sys/param.h>
	#include <net/if_arp.h> 
        #include <poll.h>
	/* 互斥量差异 */
	#define PTHREAD_MUTEX_RECURSIVE_NP PTHREAD_MUTEX_RECURSIVE
	#define SOL_IP IPPROTO_IP
	#define MSG_NOSIGNAL 0
        #ifndef __SOPOLL_TIMEOUT
        #define __SOPOLL_TIMEOUT	
   	#endif	
#endif
/* ---------------------------------------- */

#ifdef OS_LINUX 

    //头文件包含
    #include <sys/types.h>  
    #include <netinet/in.h>  
    #include <sys/socket.h>  
    #include <sys/wait.h>  
    #include <arpa/inet.h>       
    #include <unistd.h>         
    #include <sys/select.h>
    #include <sys/time.h>
    #include <sys/ioctl.h> 
    #include <termios.h>
    #include <netdb.h>
	#include <pthread.h>
    #include <fcntl.h>
	#include <sys/ipc.h>
    #include <sys/msg.h>
    #include <sys/stat.h>
	#include <dirent.h>

	#include <semaphore.h>
	#include <sys/ipc.h>
	#include <sys/sem.h>
	#include <signal.h>
	#include <dlfcn.h>  
	#include <termios.h>    

	// on Linux, socket is just a descriptor number
	typedef int                     SOCKET;
    #define INVALID_SOCKET          -1              //(~0)
    #define SOCKET_ERROR            -1              //(-1) 
    #define SOCKET_SEND_FLAG        MSG_NOSIGNAL

	#ifndef __XJSOCKLEN_T
	#define __XJSOCKLEN_T	
		typedef unsigned int            SOCK_LEN;       //linux下socket的一些api中采用unsigned int
	#endif	

	//BOOL define
	typedef int                     BOOL;
	#define FALSE                   0
    #define TRUE                    1

	//LPVOID define
	typedef void                    *LPVOID;
	//LPTSTR define
	typedef char*					LPTSTR;

	//thread define
	typedef pthread_t               THREAD_ID;      //linux下线程ID为pthread_t (unsigned int)
	typedef pthread_t               THREAD_HANDLE;  //linux下线程创建后返回为int
	typedef void *                  THREAD_FUNC;    //Linux下线程入口函数返回类型
    #define WINAPI                                  //Linux下没有WINAPI(__stdcall) 
    #define INVALID_THREAD          -1              //Linux下线程创建失败返回-1
    #define THREAD_RETURN           NULL            //Linux下线程的返回值    
	
	//process define
	typedef pid_t					HPROCESS;
	
	//mutex
	typedef pthread_mutex_t         MUTEX;          //Linux下线程互坼体类型
	typedef pthread_cond_t			HCOND;

	//semaphore define
	typedef int						HSEM;
	typedef sem_t					HNSEM;
	
	//handle define
	typedef void*					XJHANDLE;

#endif

#ifdef OS_WINDOWS
    //#define  sleep(x)                  Sleep(x*1000)  //采用与Linux下相同的形式  
    #include <Winsock2.h>
    #include <process.h>
    #include <conio.h>
	#include <Windows.h>
	#include <io.h>
	#include <sys/stat.h>


	//THREAD define
    typedef  unsigned long             THREAD_ID;      //windows下线程ID为DWORD(unsiged long)
    typedef  HANDLE                    THREAD_HANDLE;  //windows下线程创建后返回为HANDLE (void *)
    typedef  unsigned int              THREAD_FUNC;    //windows下线程入口函数返回类型
	#define INVALID_THREAD             NULL            //CreateThread()失败返回NULL
	#define THREAD_RETURN              0               //线程返回值为DWORD

	//socket define
	typedef int                        SOCK_LEN;       //windows下socket的一些api中采用 int
    #define SOCKET_SEND_FLAG           0

	//lock
	typedef CRITICAL_SECTION			MUTEX;          //windows下的互坼体类型

	typedef struct 
	{
		unsigned int count;
		unsigned int lastop; //0 none,1 signal,2 broadcast
		HANDLE		hEvent;
	}HCOND;

	//semaphore define
	typedef HANDLE					HSEM;
	typedef HANDLE					HNSEM;
	
	//handle define
	typedef HANDLE					XJHANDLE;

#endif

//数据类型定义
typedef unsigned char                   BYTE;
typedef unsigned short                  WORD;
typedef unsigned short                  u_short;
typedef unsigned int                    UINT;
typedef char							sint8;
typedef unsigned char					uint8;
typedef short							sint16;
typedef unsigned short					uint16;
typedef int								sint32;
typedef unsigned int					uint32;
typedef long							slong;
typedef unsigned long					ulong;
typedef unsigned long					DWORD;
typedef float							float32;
typedef double							float64;

//function define
#define _min(a,b) ((a)<(b)?(a):(b))
#define _max(a,b) ((a)>(b)?(a):(b))
#define bzero(ptr,n) memset(ptr,0,n)
    
#ifdef _BIG_ENDIAN_CPU_
#define SWAP_16(b)\
{\
    unsigned short&a =  *(unsigned short*)(&(b));\
    a = (a>>8)|(a<<8);\
}
#define SWAP_32(b)\
{\
    unsigned &a = *(unsigned *)(&(b));\
    a = (a>>24)|((a>>8)&0xFF00)|((a<<8)&0xFF0000)|(a<<24);\
}
#else
#define SWAP_16(b)
#define SWAP_32(b)
#endif


//msg define
#define MAXMSGLEN                0x2800                    //SOCKET数据缓冲区最大长度 10240(10K)
#define MAX_SESSION              300                       //最大的会话资源数
#define CREATE_THREAD_NUM        10                        //第一次启动创建个数
#define MAX_LINE_LENGTH          1024                      //文件中一行的最大长度
#define FILE_ERROR_MAX_LEN       255                       //操作文件错误时,错误描述长度
#define FILE_NAME_MAX_LEN        255                       //文件名最大长度

#define MAX_MSGLENGTH            512                       //消息的最大长度
#define MAX_FIELD_NAME_LEN       256                       //表名最大长度
#define MAX_FIELD_VALE_LEN       256                       //字段值最大长度
#define MAX_CONDITION_LEN        512                       //条件最大长度

/****************************net--socket*********************************/

#define LOCAL_ACT_AS_SERVER	     0                         //Socket作为服务器监听
#define LOCAL_ACT_AS_CLIENT	     1                         //socket作为客户端连接
#define LOCAL_ACT_AS_UNKNOW	    -1                         //socket功能为定义

#define ID_OUTPUT_SEND           0                         //发送
#define ID_OUTPUT_READ           1                         //接收

#define RECVTIME                 1                         //接收时间
#define SENDTIME                 2                         //发送时间
#define BLOCKING                 3                         //阻塞方式
#define UNBLOCKING               4                         //非阻塞方式
#define CONNTIME                 5                         //连接超时时间
#define BROADCAST                6						   //广播方式
#define REUSEADDR				 7						   //端口共用

#define ID_NET                   0                         //网络
#define ID_COM                   1                         //串口
#define ID_RAS                   2                         //modem 
#define ID_OTHERS                3                         //其它

#define TIMEOUT                 -2                         //从socket接收数据超时时返回值


/**
 * @brief       数据库类型
 * @author      pengl
 * @version     ver1.0
 * @date        03/09/2008
 *
 */
/** @brief        MYSQL数据库*/
const int RECORD_DB_TYPE_MYSQL = 1;
/** @brief        ORACLE数据库*/
const int RECORD_DB_TYPE_ORACLE = 2;
/** @brief        SQLSERVER数据库*/
const int RECORD_DB_TYPE_SQLSERVER = 3;
/** @brief        达梦数据库*/
const int RECORD_DB_TYPE_DAMENG = 4;
/** @brief        金仓数据库*/
const int RECORD_DB_TYPE_KINGBASE = 5;
/** @brief        mongodb数据库*/
const int RECORD_DB_TYPE_MONGODB = 6;

/**
 * @brief       设备类型
 * @author      pengl
 * @version     ver1.0
 * @date        03/09/2008
 *
 */
//用整数标识的设备分类
const int TYPE_BUS		= 1;		//母线
const int TYPE_LINE		= 2;		//线路
const int TYPE_BREAK	= 3;		//开关
const int TYPE_GEN		= 4;		//发电机
const int TYPE_REACTOR	= 5;		//高压电抗器
const int TYPE_TRANS	= 6;		//变压器
const int TYPE_SWITCH	= 7;		//刀闸
const int TYPE_CAP      = 8;		//电容器
const int TYPE_PMS		= 9;		//保护管理机
const int TYPE_BAY		= 10;		//间隔
const int TYPE_STATION	= 18;		//厂站
const int TYPE_NET		= 19;		//地区
const int TYPE_SEC		= 20;		//二次设备//其它不可归某一次设备的二次设备
const int TYPE_PROTECT	= 21;		//保护 
const int TYPE_WAVEREC	= 22;		//录波器
const int TYPE_FD_DEV	= 23;		//故障测距装置虚拟类型
const int TYPE_FD_STATION	= 24;	//故障测距站虚拟类型
const int TYPE_STATION_GROUP = 50;  //厂站组标题
const int TYPE_DEVICE_GROUP  = 51;  //装置组标题
const int TYPE_BREAKER_DEVICE = 53; //开关与一次设备关联
const int TYPE_STATION_SVG = 54;    //厂站SVG
const int TYPE_CPU         = 99;    //CPU类型

//用字符型标识的设备分类
#define STRING_BUS		"母线"		//母线
#define STRING_LINE		"线路"		//线路
#define STRING_BREAK	"开关"		//开关
#define STRING_GEN		"发电机"	//发电机
#define STRING_CAP		"电容器"	//电容器
#define STRING_REACTOR	"高压电抗器"//高压电抗器
#define STRING_TRANS	"变压器"	//变压器
#define STRING_SWITCH	"刀闸"		//刀闸
#define STRING_PMS      "保护管理机" //保护管理机
#define STRING_BAY		"间隔"		//间隔
#define STRING_NET		"地区"		//地区
#define STRING_STATION	"厂站"		//厂站
#define	STRING_SEC		"二次设备"	//二次设备
#define STRING_PROTECT	"保护装置"	//保护装置
#define STRING_WAVEREC	"录波器"	//录波器
#define STRING_DISTANCE "测距装置"	//测距装置

//20149报文中的更新类型(外部要求数据平台更新内存要求)
const int TYPE_STATION_INFO    = 1;//厂站信息
const int TYPE_PROTECT_INFO    = 2;//装置信息
const int TYPE_NORMAL_INFO     = 3;//普通参数配置信息

//更新内存时用
const  int STATUS_NORMAL = 0;
const  int STATUS_UPDATE = 1;
const  int STATUS_WRITE  = 2;
const  int STATUS_READ   = 3;

//net 地址结构
typedef struct _NETADDR
{
	char	IpAddr[40];                                    //IP地址
	u_short	nPort;                                         //端口号
}NETADDR;

//socket缓冲区结构
typedef struct _BUFFER
{
	WORD    len;                                           //报文长度
	char    buf[MAXMSGLEN];
}XJBUFFER;


//系统时间结构
typedef struct _SYSTIME
{
	u_short wYear; 
    u_short wMonth; 
    u_short wDayOfWeek; 
	u_short wDayOfYear;
    u_short wDay; 
    u_short wHour; 
    u_short wMinute; 
    u_short wSecond; 
    u_short wMilliseconds; 
} SYSTIME;

//系统各功能超时设置结构
typedef struct _TIMEOUTS
{
	//召唤
	UINT   call_protect_settingdata;            //召唤保护定值
	UINT   call_wav_settingdata;                //召唤录波器定值
	UINT   call_protect_comm;                   //召唤保护通讯状态
	UINT   call_anai;                           //召唤模拟量
	UINT   call_switch;                         //召唤开关量
	UINT   call_softboard;                      //召唤软压板
	UINT   call_runstatus;                      //召唤运行状态
	UINT   call_wavfile_list;                   //召唤录波文件列表
	UINT   call_waverfile;                      //召唤录波文件
	UINT   call_common_file;                    //召唤通用文件
	UINT   call_common_file_list;               //召唤通用文件列表
	UINT   call_zone;                           //召唤定值区号
	UINT   call_wav_comm;                       //召唤录波器通讯状态
	UINT   call_basic_info;                     //召唤基本信息
	UINT   call_dev_time;                       //召唤装置时间
    UINT   call_history_info;                   //召唤历史事件
    UINT   call_single_config;                  //召唤某台装置的某个信息表配置
	UINT   call_total;                          //总召唤
	UINT   call_channel_status;                 //召唤通道状态

	//控制
	UINT   set_dev_time;                        //对时
	UINT   set_setting_data;                    //修改定值
	UINT   set_zone;                            //定值区切换
	UINT   set_soft_board;                      //压板投退
	UINT   set_signal_reset;                    //信号复归
	UINT   set_wav_remote_touch;                //录波器远方触发
    UINT   set_event_signal;                    //子站信号量远程定制

	//103
	UINT   t0;                                  //连接建立超时
	UINT   t1;                                  //接收测试报文超时
	UINT   t2;                                  //无数据确认超时
	UINT   t3;                                  //长期空闲下发送测试报文超时

}TIMEOUTS;

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
const int RECORD_DATA_MSG_ID_MANUAL_OSC_FILE = 20025;
const int RECORD_DATA_MSG_ID_RESULT_MANUAL_FILE = 20026;
const int RECORD_DATA_MSG_ID_DOWN_DFU_CONFIG = 20060;
const int RECORD_DATA_MSG_ID_RESULT_DOWN_DFU_CONFIG = 20061;
const int RECORD_DATA_MSG_ID_DOWN_MGR_CONFIG = 20070;
const int RECORD_DATA_MSG_ID_RESULT_DOWN_MGR_CONFIG = 20071;
const int RECORD_DATA_MSG_ID_UPLOAD_NEW_OSC_FILE = 20090;

/**
 * @brief       报文发送方向定义
 * @author      pengl
 * @version     ver1.0
 * @date        03/09/2008
 *
 */
/** @brief            发送*/
const UINT MSG_SEND = 1;

/** @brief            接收*/
const UINT MSG_RECV = 2;


/**	\brief rabbitmq默认访问端口*/
const int RABBIT_MQ_DEFAULT_ACCESS_PORT = 5672;
const int RABBIT_MQ_DEFAULT_CHANNEL_MAX = 0;
const int RABBIT_MQ_DEFAULT_FRAME_MAX = 131072;
const char RABBIT_MQ_DEFAULT_V_HOST[] = "/";
const int RABBIT_MQ_DEFAULT_CHANNEL_ID = 1;
const int RABBIT_MQ_DEFAULT_RECV_FETCH_COUNT = 1000;

/** @} */ //OVER

typedef struct _FILE_STATUS_STRUCT
{
	char cFileName[100];
	time_t stWriteTime;
	int   nFileSize;
}FILE_STATUS_STRUCT;

/**
 * @brief       打印报文结构头
 * @author      pengl
 * @version     ver1.0
 * @date        
 *
 * example
 * @code
 *
 *
 *
 * @endcode
 */
typedef struct _LOG_BUFFER_HEAD
{
	/** @brief   对端网络地址*/
	NETADDR addr;
	
	/** @brief   数据传输方向*/
	int nWay;
		
	/** @brief   串口名称*/
	char ch_port_name[32];
}LOG_BUFFER_HEAD;

//rabbitmq参数
typedef struct _RABBIT_MQ_BASIC_ACCESS_PARAM
{
	int nserver_port;	//rabbitmq listen port,default 5672
	int nChannelID;	//create channel id,default 1
	int nChannelMax;	//channel max,default 0
	int nFrameMax;	//frame max,default 131072
	int nHeartbeatTime;	//heartbeat time,default 0 disable
	char chhostname[64];	//rabbitmq server ip addr
	char chusrname[128];	//rabbitmq access username
	char chpassword[128];	//rabbitmq access user password
	char chVhost[MAX_FIELD_NAME_LEN];	//access vhost,default '/'

	_RABBIT_MQ_BASIC_ACCESS_PARAM()
	{
		nserver_port = RABBIT_MQ_DEFAULT_ACCESS_PORT;
		nChannelID = RABBIT_MQ_DEFAULT_CHANNEL_ID;
		nChannelMax = RABBIT_MQ_DEFAULT_CHANNEL_MAX;
		nFrameMax = RABBIT_MQ_DEFAULT_FRAME_MAX;
		nHeartbeatTime = 0;
		bzero(chhostname, sizeof(chhostname));
		bzero(chusrname, sizeof(chusrname));
		bzero(chpassword, sizeof(chpassword));
		bzero(chVhost, sizeof(chVhost));

		sprintf(chVhost, "%s", RABBIT_MQ_DEFAULT_V_HOST);
	}
}RABBIT_MQ_BASIC_ACCESS_PARAM;

typedef struct _RABBIT_RECV_PARAM
{
	int perfetchcount;
	int channelid;
	int passive;
	int durable;
	int exclusive;
	int autoDelete;
	int no_ack;
	char chQueueName[MAX_FIELD_NAME_LEN];	//recv queue name
	char chRouteKey[MAX_FIELD_NAME_LEN];	//access route key,default null
	
	_RABBIT_RECV_PARAM()
	{
		perfetchcount = RABBIT_MQ_DEFAULT_RECV_FETCH_COUNT;
		channelid = RABBIT_MQ_DEFAULT_CHANNEL_ID;
		passive = 0;
		durable = 0;
		exclusive = 0;
		autoDelete = 0;
		no_ack = 0;
		bzero(chQueueName, sizeof(chQueueName));
		bzero(chRouteKey, sizeof(chRouteKey));
	}

}RABBIT_RECV_PARAM;

//mongodb access param
typedef struct _RECORD_MONGO_BASIC_PARAM
{
	int nPort;
	char chAddr[64];
	char chUser[128];
	char chPasswd[128];
	char chDataBase[128];

	_RECORD_MONGO_BASIC_PARAM()
	{
		nPort = 27017;
		bzero(chAddr, sizeof(chAddr));
		bzero(chUser, sizeof(chUser));
		bzero(chPasswd, sizeof(chPasswd));
		bzero(chDataBase, sizeof(chDataBase));
	}

}RECORD_MONGO_BASIC_PARAM;

/*  全局函数声明  */
extern void GetSysTime(SYSTIME & curTime);
extern void CreateSavePath(char * szPath);
extern void GetIniKey(const char *filename, 
						   const char *title,
				           const char *key,
						   char *chResult);
extern void EndThread();
extern void MySleep(unsigned int nmilliseconds);
extern void GetFileStatusInDir(char *dir, vector <FILE_STATUS_STRUCT> *pFileStatusList,int nLevel);
extern int  GetFileStatus(char *filename, FILE_STATUS_STRUCT *pFileStatusStruct);
extern string GetFormatPath(string sourcePath);
extern string GetFileNameFromFullFileName(string sFullFileName);
extern string GetFilePathFromFullFileName(string sFullFileName);
extern bool PingDestination(const string pIp);
extern WORD ReverseWordEndian(WORD pSourse);
extern int ReverseIntEndian(int pSourse);
extern float ReverseFloatEndian(float pSourse);
extern string FormatGatewayList(string pGatewayA, string pGatewayB);
extern void GetGatewayFromList(string pGatewayList, string& pGatewayA, string& pGatewayB);
//取得.分隔的文件扩展名
extern bool GetFileExFileName(const string& pSrcFileName,string& strExtName);

#endif

