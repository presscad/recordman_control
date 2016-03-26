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
#include "GlobalFunction.h"

/**	\brief ��־Ĭ�ϱ�������*/
const int COLLECTOR_LOG_SAVE_DEFAULT_DAYS = 2;
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

//��־��Ŀ¼
#define MANAGER_BOARD_DEFAULT_ROOT_LOG "./data_collector_log/"


/** @brief           ������*/
const BYTE RECORD_COMMU_CHAR_START_VAR = 0xA8;
/** @brief           ������*/
const BYTE RECORD_COMMU_CHAR_END_VAR = 0x5F;
/** @brief           Э�����һλ*/
const BYTE RECORD_COMMU_CHAR_PROTOCOL_START_VAR = 0x61;
/** @brief           Э����ڶ�λ*/
const BYTE RECORD_COMMU_CHAR_PROTOCOL_END_VAR = 0x01;
/** @brief           ����*/
const WORD DEFAULT_LENGTH_MASK = 0x0fff;

/**
 * @defgroup STTP ���Ľṹ����
 * @{
 */

/** @brief           ��������󳤶�(1400)*/
const UINT MAX_RECORD_MSG_LEN = 0x578;

/**
 * @brief       commu protocol define
 * @author      pengl
 * @version     ver1.0
 * @date        
 *
 */
/*  record private commu protocol  */
const int DEFINE_COMMU_PROTOCOL_RECORD_SELF = 1;

/** @brief           BYTE����*/
/** @brief           Ѳ������*/
const BYTE RECORD_COMMAND_CHAR_PATROL_VAR = 0x01;
/** @brief           ��������*/
const BYTE RECORD_COMMAND_CHAR_CONFIG_SEND_VAR = 0x11;
/** @brief           ��������*/
const BYTE RECORD_COMMAND_CHAR_CONFIG_RECV_VAR = 0x12;
/** @brief           ��ֵ����*/
const BYTE RECORD_COMMAND_CHAR_SETTING_ADJUST_VAR = 0x21;
/** @brief           ��ֵ��ȡ*/
const BYTE RECORD_COMMAND_CHAR_SETTING_READ_VAR = 0x22;
/** @brief           ��ֵ���л�*/
const BYTE RECORD_COMMAND_CHAR_SETZONE_CHANGE_VAR = 0x23;
/** @brief           ���ֵ����ȡ*/
const BYTE RECORD_COMMAND_CHAR_CURZONE_READ_VAR = 0x24;
/** @brief           ��¼����ѯ*/
const BYTE RECORD_COMMAND_CHAR_NEW_OSC_QUERY_VAR = 0x31;
/** @brief           ¼��������ȡ*/
const BYTE RECORD_COMMAND_CHAR_OSC_INDEX_READ_VAR = 0x32;
/** @brief           ¼����ȡ*/
const BYTE RECORD_COMMAND_CHAR_OSC_FILE_READ_VAR = 0x33;
/** @brief           �ֶ�¼��*/
const BYTE RECORD_COMMAND_CHAR_MANUAL_OSC_VAR = 0x3A;
/** @brief           ʵʱ���ζ�ȡ*/
const BYTE RECORD_COMMAND_CHAR_REALTIME_OSC_READ_VAR = 0x41;
/** @brief           ʵʱ���ݶ�ȡ*/
const BYTE RECORD_COMMAND_CHAR_REALTIME_DATA_READ_VAR = 0x42;
/** @brief           �Լ��ѯ*/
const BYTE RECORD_COMMAND_CHAR_SELF_CHECK_VAR = 0x91;
/** @brief           �汾��ѯ*/
const BYTE RECORD_COMMAND_CHAR_VERSION_QUERY_VAR = 0x92;
/** @brief           ʱ���ѯ*/
const BYTE RECORD_COMMAND_CHAR_TIME_QUERY_VAR = 0x93;
/** @brief           ��ģ����Ϣ��ѯ*/
const BYTE RECORD_COMMAND_CHAR_SUB_MODULE_QUERY_VAR = 0x94;
/** @brief           װ�ø�λ*/
const BYTE RECORD_COMMAND_CHAR_RESET_VAR = 0xA0;
/** @brief           дip��ַ*/
const BYTE RECORD_COMMAND_CHAR_IP_SET_VAR = 0xA1;
/** @brief           ʱ������*/
const BYTE RECORD_COMMAND_CHAR_TIME_SET_VAR = 0xA2;
/** @brief           ʱ������*/
const BYTE RECORD_COMMAND_CHAR_TIMEZONE_SET_VAR = 0xA3;

//dfu msg
typedef vector<BYTE> DFU_COMMU_MSG;

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
