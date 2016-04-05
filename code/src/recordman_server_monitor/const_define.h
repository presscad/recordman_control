#ifndef RECORDMAN_CONSTANT_DEFINE_H
#define RECORDMAN_CONSTANT_DEFINE_H

#pragma warning(disable : 4786)//����4786����

#include "../../common/sys_define.h"
#include "../../common/common_lib.h"
#include "../../common/CCommonFile.h"
#include "../../common/LogFile.h"
#include "../../common/StrongString.h"
#include "../../common/Lock.h"
#include "../../common/MessageLog.h"
#include "../../common/RecordmanThread.h"
#include "../../common_open_source/rabbitmq/include/amqp.h"
#include "../../common_open_source/rabbitmq/include/amqp_framing.h"
#include "../../common_open_source/rabbitmq/include/amqp_tcp_socket.h"
#include "../../common_open_source/rabbitmq/include/amqp_ssl_socket.h"

/**	\brief ��־Ĭ�ϱ�������*/
const int MONITOR_LOG_SAVE_DEFAULT_DAYS = 2;
/**	\brief ��־Ĭ�ϴ�С*/
const int MONITOR_LOG_DEFAULT_SIZE = 5*1024;
/**	\brief ����Ѳ��ʱ��*/
const int MONITOR_IDLE_CHECEK_DEFAULT_TIME = 10;
/**	\brief ���������*/
const char SERVER_MONITOR_RESULT_QUEUE_NAME[] = "SERVER_MONITOR_RESULT_QUEUE";
/**	\brief �����ļ�*/
#define RECORD_MANAGEMENT_BOARD_CONFIG_FILE "../../conf/recordman_manager_config.xml"
/**	\brief ��־��Ŀ¼*/
#define MANAGER_BOARD_DEFAULT_ROOT_LOG "./server_monitor_log/"

//ϵͳ����
typedef struct _COLLECTOR_DATA_SYS_PARAM
{
	//��־����
	int nLoglevel;
	//��־��������
	int nLogDays;
	//��־����·��
	char chLogpath[MAX_LINE_LENGTH];

	_COLLECTOR_DATA_SYS_PARAM()
	{
		nLoglevel = CLogFile::trace;
		nLogDays = MONITOR_LOG_SAVE_DEFAULT_DAYS;
		bzero(chLogpath, sizeof(chLogpath));
	}

}COLLECTOR_DATA_SYS_PARAM;

//rabbit advance param
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
