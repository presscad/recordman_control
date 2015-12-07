#if !defined(AFX_READWAVE_H__F2CC2B0E_B7AF_441E_B3B5_A1F1236078CA__INCLUDED_)
#define AFX_READWAVE_H__F2CC2B0E_B7AF_441E_B3B5_A1F1236078CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#endif
#include "./include/jni.h"
#include "./include/linux/jni_md.h"
#include "Comtrade.h"
#ifndef _Included_com_wave_server_waveQueryDll
#define _Included_com_wave_server_waveQueryDll
#ifdef __cplusplus


extern "C"
{
#endif


 

////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * Class:     com_xjwave_server_XJComtradeWrap
 * Method:    getComtradeWeb
 * Signature: (Ljava/lang/String;Ljava/io/ObjectOutputStream;I)I
 */
JNIEXPORT jint JNICALL Java_com_wave_server_ComtradeWrap_getComtradeWeb
  (JNIEnv *, jclass, jstring, jobject, jint);
  
/*
 * Class:     com_xjwave_server_waveQueryDll
 * Method:    getComtrade
 * Signature: (Ljava/lang/String;Lcom/xjwave/server/ComtradeCFG;Ljava/util/List;Ljava/util/List;)I
 */
JNIEXPORT jstring JNICALL Java_com_wave_server_ComtradeWrap_getComtrade
  (JNIEnv *, jclass, jstring, jobject, jobject, jobject);


/*
 * Class:     com_xjwave_server_XJComtradeWrap
 * Method:    getComtradeB
 * Signature: (Ljava/lang/String;Lcom/xjwave/server/ComtradeCFG;Ljava/util/List;Ljava/util/List;Z)I
 */
JNIEXPORT jstring JNICALL Java_com_wave_server_ComtradeWrap_getComtradeB
  (JNIEnv *, jclass, jstring, jobject, jobject, jobject, jboolean);



 

JNIEXPORT void JNICALL Java_com_wave_server_ComtradeWrap_freeXJComtrade
	(JNIEnv *, jobject);

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
#endif
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_READWAVE_H__F2CC2B0E_B7AF_441E_B3B5_A1F1236078CA__INCLUDED_)

