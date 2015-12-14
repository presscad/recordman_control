#ifndef INCLUDED_RECORD_MAN_THREAD_H
#define INCLUDED_RECORD_MAN_THREAD_H

#pragma warning(disable : 4786)//����4786����

#include "sys_define.h"
#include "common_lib.h"

/**
 * @defgroup 	CRecordmanThread	�̷߳�װ
 * @{
 */
/**
 * @brief       �๦����ϸ����
*  @author      LP
 * @version     ver1.0
 * @date        14/12/15

 *
 * example
 * @code
 *    // ����˵�������ʹ��
 * @endcode
 */
/** @} */ //OVER

class CRecordmanThread
{
public:
	CRecordmanThread();
	~CRecordmanThread();

public:
/*
 *  @brief   	Start	 ��ʼ�߳� 
 *  @param 		[In]a param of Type  XJ_THREAD_ROUTINE  �̺߳���
 *  @param 		[In]a param of Type  LPVOID  ����ֵ
 *  @return 	bool true���ɹ� false��ʧ��
 */
	bool Start(RECORD_MAN_THREAD_ROUTINE threadfun, LPVOID lpParameter);

/*
 *  @brief   	Stop	 ֹͣ�߳� 
 *  @return 	bool true���ɹ� false��ʧ��
 */
    bool Stop();

/*
 *  @brief   	IsValid	 �Ƿ���Ч�߳� 
 *  @return 	bool true����Ч false����Ч
 */
	bool IsValid();

/*
 *  @brief   	GetThreadHandle	 ��ȡ�߳̾�� 
 *  @return 	THREAD_HANDLE �߳̾��
 */
	THREAD_HANDLE GetThreadHandle();

/*
 *  @brief   	GetThreadId	 ��ȡ�߳�ID 
 *  @return 	THREAD_ID �߳�ID
 */
	THREAD_ID GetThreadId();

private:
/**	\brief �߳̾��*/
	THREAD_HANDLE m_hHande;
	
/**	\brief �߳�ID*/
	THREAD_ID m_hId;
};

#endif
