#ifndef INCLUDED_RECORD_MAN_THREAD_H
#define INCLUDED_RECORD_MAN_THREAD_H

#pragma warning(disable : 4786)//屏蔽4786错误

#include "sys_define.h"
#include "common_lib.h"

/**
 * @defgroup 	CRecordmanThread	线程封装
 * @{
 */
/**
 * @brief       类功能详细描述
*  @author      LP
 * @version     ver1.0
 * @date        14/12/15

 *
 * example
 * @code
 *    // 举例说明类如何使用
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
 *  @brief   	Start	 开始线程 
 *  @param 		[In]a param of Type  XJ_THREAD_ROUTINE  线程函数
 *  @param 		[In]a param of Type  LPVOID  附加值
 *  @return 	bool true：成功 false：失败
 */
	bool Start(RECORD_MAN_THREAD_ROUTINE threadfun, LPVOID lpParameter);

/*
 *  @brief   	Stop	 停止线程 
 *  @return 	bool true：成功 false：失败
 */
    bool Stop();

/*
 *  @brief   	IsValid	 是否有效线程 
 *  @return 	bool true：有效 false：无效
 */
	bool IsValid();

/*
 *  @brief   	GetThreadHandle	 获取线程句柄 
 *  @return 	THREAD_HANDLE 线程句柄
 */
	THREAD_HANDLE GetThreadHandle();

/*
 *  @brief   	GetThreadId	 获取线程ID 
 *  @return 	THREAD_ID 线程ID
 */
	THREAD_ID GetThreadId();

private:
/**	\brief 线程句柄*/
	THREAD_HANDLE m_hHande;
	
/**	\brief 线程ID*/
	THREAD_ID m_hId;
};

#endif
