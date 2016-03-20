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
#include "GlobalFunction.h"

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
const int COLLECTOR_COMMU_RECV_TIMEOUT = 3*1000;
/**	\brief ���ͳ�ʱ*/
const int COLLECTOR_COMMU_SEND_TIMEOUT = 3*1000;
/**	\brief ���ļ����ʱ��*/
const int COLLECTOR_COMMU_CHECK_NEW_FILE_TIME = 2;

/**	\brief ��־Ĭ��·��*/
#define COLLECTOR_LOG_DEFAULT_PATH "./default_log/"

/**	\brief ������ն�����*/
const char DATA_COLLECTOR_RECV_QUEUE_NAME[] = "DATA_COLLECTOR_COMMAND_QUEUE";

//�����ļ�
#define RECORD_MANAGEMENT_BOARD_CONFIG_FILE "../../conf/recordman_manager_config.xml"

//dfu����
typedef struct _COLLECTOR_DFU_COMMU_PARAM
{
	char chDfuAddr[MAX_FIELD_NAME_LEN];//dfu�����˿�
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

//ϵͳ����
typedef struct _COLLECTOR_DATA_SYS_PARAM
{
	//��־����
	int nLoglevel;
	//��־��������
	int nLogDays;
	//���ճ�ʱ
	int nRecvTimeout;
	//���ͳ�ʱ
	int nSendTimeout;
	//��־����·��
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
	//����ͨ����
	int nCollectorRecvChannel;
	//�ɼ����̽��ն�������
	char chCollectorRecvQueName[FILE_NAME_MAX_LEN];
	//web�����������
	char chWebResultQueName[FILE_NAME_MAX_LEN];
	//��������
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
