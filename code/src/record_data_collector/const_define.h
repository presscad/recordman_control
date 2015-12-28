#ifndef RECORD_CONSTANT_DEFINE_H
#define RECORD_CONSTANT_DEFINE_H

#pragma warning(disable : 4786)//����4786����

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

/**	\brief ��־Ĭ�ϱ�������*/
const int COLLECTOR_LOG_SAVE_DEFAULT_DAYS = 5;
/**	\brief ��־Ĭ�ϴ�С*/
const int COLLECTOR_LOG_DEFAULT_SIZE = 5*1024;
/**	\brief DFU�����˿�*/
const int COLLECTOR_DFU_LISTEN_PORT = 3309;
/**	\brief ���ݿ����Ӹ���*/
const int COLLECTOR_DB_DEFAULT_CONN_NUM = 2;
/**	\brief ����Ѳ��ʱ��*/
const int COLLECTOR_IDLE_CHECEK_DEFAULT_TIME = 10;
/**	\brief ���ճ�ʱ*/
const int COLLECTOR_COMMU_RECV_TIMEOUT = 90*1000;
/**	\brief ���ͳ�ʱ*/
const int COLLECTOR_COMMU_SEND_TIMEOUT = 90*1000;

/**	\brief ��־Ĭ��·��*/
#define COLLECTOR_LOG_DEFAULT_PATH "./default_log/"

/**
* @brief       ���ݿ���ʶ�̬��������
* @author      pengl
* @version     ver1.0
* @date        07/13/2008
*
*/
#ifdef OS_WINDOWS
	#define LIBMYSAL      "DBAccessMysql.dll"//mysql���ݿ���ʶ�̬��
	#define LIBSQLSERVER  "DBAccessSqlserver.dll"//sqlserver���ݿ���ʶ�̬��
	#define LIBORACLE     "DBAccessOracle.dll"//oracle���ݿ���ʶ�̬��
	#define LIBDAMENG     "DBAccessDaMeng.dll"//�������ݿ���ʶ�̬��
	#define LIBKINGBASE   "DBAccessKingbase.dll"//������ݿ���ʶ�̬��
	#define LIBFTPCLIENTMANAGER "XJFtpClientManager.dll"//ftp�ͻ��˲�����
	#define LIBXJSTTPNET  "XJSttpNet.dll"//sttpnet��̬��
	#define LIBSTTPXMLTRANSFORM "XJSttpXmlTransform.dll"//sttp��xml��ת��̬��
#else
	#define LIBMYSAL      "libDBAccessMysql.so"
	#define LIBSQLSERVER  "libDBAccessSqlserver.so"
	#define LIBORACLE     "libDBAccessOracle.so"
	#define LIBDAMENG     "libDBAccessDaMeng.so"//�������ݿ���ʶ�̬��
	#define LIBKINGBASE   "libDBAccessKingbase.so"//�������ݿ���ʶ�̬��
	#define LIBFTPCLIENTMANAGER "libXJFtpClientManager.so"//ftp�ͻ��˲�����
	#define LIBXJSTTPNET  "libXJSttpNet.so"//sttpnet��̬��
	#define LIBSTTPXMLTRANSFORM "libXJSttpXmlTransform.so"//sttp��xml��ת��̬��
#endif

//ϵͳ����
typedef struct _COLLECTOR_DATA_SYS_PARAM
{
	int nLoglevel;//��־����
	int nLogDays;//��־��������
	int nDfuPort;//dfu�����˿�
	int nIdleCheckTime;//����Ѳ��ʱ��
	int nRecvTimeout;//���ճ�ʱ
	int nSendTimeout;//���ͳ�ʱ
	char chLogpath[MAX_LINE_LENGTH];//��־����·��
	char chDfuAddr[MAX_FIELD_NAME_LEN];//dfu�����˿�

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
