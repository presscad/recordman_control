#ifndef XJCOMTRADE_H_
#define XJCOMTRADE_H_

#pragma   warning   (disable   :   4786) 
#pragma   warning   (disable   :   4275) 

#include "constant.h"
#include "../../../interface/ComtradeDefine.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifdef OS_WINDOWS
/*
 *  @brief   	LoadFile	 ���벨���ļ� 
 *  @param 		[In]a param of Type  char*  �����ļ�·��
 *  @param 		[Out]a param of Type  char*  ������Ϣ
 *  @param 		[Out]a param of Type  int&  ���ɵĲ����ļ����ݶ���ָ��
 *  @return 	int ����ɹ�����>=0, ʧ�ܷ���<0
 */
	extern "C" __declspec( dllexport ) int LoadFile(char* sFilePath, char* sError, XJHANDLE& hComtrade);

/*
 *  @brief   	CloseFile	 �رղ����ļ� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	void
 */
	extern "C" __declspec( dllexport ) void CloseFile(XJHANDLE hComtrade);

/*
 *  @brief   	GetCFGAttr	 ȡ��CFG�ļ���¼�Ĳ��������������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[Out]a param of Type  ComtradeCFG&  ����������������
 *  @return 	void 
 */
	extern "C" __declspec( dllexport ) void GetCFGAttr(XJHANDLE hComtrade, ComtradeCFG& cfgAttr);

/*
 *  @brief   	GetRatesCount	 ȡ�ò���Ƶ����
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int ����Ƶ���� 
 */
	extern "C" __declspec( dllexport ) int GetRatesCount(XJHANDLE hComtrade);

/*
 *  @brief   	GetRateSample	 ȡ��ָ������Ƶ�������Ĳ���Ƶ�ʺͲ������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������Ƶ������.������0��ʼ
 *	@param		[Out]a param of Type float& ����Ƶ��
 *	@param		[Out]a param of Type int& ����Ƶ���µĲ��������
 *  @return 	void 
 */
	extern "C" __declspec( dllexport ) void GetRateSample(XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples);

/*
*  @brief   	GetRate	 ȡ��ָ��������Ĳ���Ƶ�� 
*  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
*  @param 		[In]a param of Type  int  ָ��������
 *  @return 	float ����Ƶ�� 
 */
	extern "C" __declspec( dllexport ) float GetRate(XJHANDLE hComtrade, int nSamplePos);

/*
 *  @brief   	GetChannelCFG	 ȡ��ָ��ͨ����������Ϣ 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *	@param		[Out]a param of Type ComtradeAChannel& ͨ��������Ϣ�ṹ��
 *  @return 	int  ͨ������.1��ʾģ����ͨ����0��ʾ������ͨ��
 */
	extern "C" __declspec( dllexport ) int GetChannelCFG(XJHANDLE hComtrade, int nCn, ComtradeChannel& chl);

/*
 *  @brief   	GetChannelType	 ȡ��ָ��ͨ�������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *  @return 	int  ͨ������.1��ʾģ����ͨ����0��ʾ������ͨ��
 */
	extern "C" __declspec( dllexport ) int GetChannelType(XJHANDLE hComtrade, int nCn);

/*
 *  @brief   	GetTriggerTime	 ȡ�ù��ϴ���ʱ������ڿ�ʼ����ʱ�̵����ʱ��
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int  ���ϴ���ʱ������ڿ�ʼ����ʱ�̵����ʱ�䣨���룩
 */
	extern "C" __declspec( dllexport ) int GetTriggerTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetTimeRelativeTrigger	 ȡ��ָ������������ڹ��ϴ���ʱ�̵�ʱ�� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	int  ����ڹ��ϴ���ʱ�̵�ʱ�䣨���룩
 */
	extern "C" __declspec( dllexport ) int GetTimeRelativeTrigger(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ�� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	int  ����ڲ�����ʼʱ�̵�ʱ�䣨���룩
 */
	extern "C" __declspec( dllexport ) int GetTimeRelativeStart(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTotalSamples	 ȡ�ò��������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int  ����������
 */
	extern "C" __declspec( dllexport ) int GetTotalSamples(XJHANDLE hComtrade);

/*
 *  @brief   	GetRealValue	 ȡ��ָ��ͨ��ָ�����������ʵֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *	@param		[In]a param of Type  int  ָ����Ҫ��ֵ�����ͣ�Ĭ��Ϊ����ֵ.0Ϊ����ֵ��1Ϊһ��ֵ
 *  @return 	float  ָ��ͨ��ָ�����������ʵֵ
 */
	extern "C" __declspec( dllexport ) float GetRealValue(XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType = 0);

/*
 *  @brief   	GetRecordValue	 ȡ��ָ��ͨ��ָ��������ļ�¼ֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	int  ָ��ͨ��ָ��������ļ�¼ֵ
 */
	extern "C" __declspec( dllexport ) int GetRecordValue(XJHANDLE hComtrade, int nCn, int nSampleNo);

/*
 *  @brief   	GetTotalTime	 ȡ����ʱ�� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int  ��ʱ��
 */
	extern "C" __declspec( dllexport ) int GetTotalTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetMaxValue	 ȡ��ָ��ͨ�������ֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *	@param		[In]a param of Type	 int  ָ���Ƿ�ȡ�������ֵ. 0��ʾȡ�������ֵ,1��ʾ��ͨ���Լ������ֵ
 *	@param		[In]a param of Type	 int  ָ����Ҫ��ֵ�����ͣ�0Ϊ����ֵ��1Ϊһ��ֵ
 *  @return 	int  ���ֵ
 */
	extern "C" __declspec( dllexport ) float GetMaxValue(XJHANDLE hComtrade, int nCn, int nGroup, int nDataType);

/*
 *  @brief   	GetCTPT	 ȡ��CTPT���
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[Out]a param of Type  float CT���
 *  @param 		[Out]a param of Type  float PT���
 *  @return 	void
 */
	extern "C" __declspec( dllexport ) void GetCTPT(XJHANDLE hComtrade, float& fCT, float& fPT);

	/*
 *  @brief   	SetCTPT	 ����CTPT���
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  float CT���
 *  @param 		[In]a param of Type  float PT���
 *  @return 	void
 */
	extern "C" __declspec( dllexport ) void SetCTPT(XJHANDLE hComtrade, float fCT, float fPT);

/*
 *  @brief   	GetTimeRelativeTrigger	 ȡ��ָ������������ڹ��ϴ���ʱ�̵�ʱ�䣬����΢��ֵ 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	double  ����ڹ��ϴ���ʱ�̵�ʱ�䣨΢�룩
 */
	extern "C" __declspec( dllexport ) double GetTimeRelativeTriggerMSEC(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ�䣬����΢��ֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	double  ����ڲ�����ʼʱ�̵�ʱ�䣨΢�룩
 */
	extern "C" __declspec( dllexport ) double GetTimeRelativeStartMSEC(XJHANDLE hComtrade, int nSampleNo);
	/*
	 *  @brief   	GetHdrInfoObject	 ȡ��HDR�ļ���Ϣ
	 *  @return 	HdrObj	 HDR�ļ���Ϣ����
	 */
	extern "C" __declspec( dllexport ) HdrObj* GetHdrInfoObject(XJHANDLE hComtrade);

	/*
	 *  @brief   	GetHdrInfoJSON	 ȡ��HDR�ļ���Ϣ
	 *  @return 	string	 JSON��ʽ��HDR�ļ���Ϣ
	 */
	extern "C" __declspec( dllexport ) string GetHdrInfoJSON(XJHANDLE hComtrade);

#else
/*
 *  @brief   	LoadFile	 ���벨���ļ� 
 *  @param 		[In]a param of Type  char*  �����ļ�·��
 *  @param 		[Out]a param of Type  char*  ������Ϣ
 *  @param 		[Out]a param of Type  int&  ���ɵĲ����ļ����ݶ���ָ��
 *  @return 	int ����ɹ�����>=0, ʧ�ܷ���<0
 */
	int LoadFile(char* sFilePath, char* sError, XJHANDLE& hComtrade);

/*
 *  @brief   	CloseFile	 �رղ����ļ� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	void
 */
	void CloseFile(XJHANDLE hComtrade);

/*
 *  @brief   	GetCFGAttr	 ȡ��CFG�ļ���¼�Ĳ��������������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[Out]a param of Type  ComtradeCFG&  ����������������
 *  @return 	void 
 */
	void GetCFGAttr(XJHANDLE hComtrade, ComtradeCFG& cfgAttr);

/*
 *  @brief   	GetRatesCount	 ȡ�ò���Ƶ����
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int ����Ƶ���� 
 */
	int GetRatesCount(XJHANDLE hComtrade);

/*
 *  @brief   	GetRateSample	 ȡ��ָ������Ƶ�������Ĳ���Ƶ�ʺͲ������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������Ƶ������.������0��ʼ
 *	@param		[Out]a param of Type float& ����Ƶ��
 *	@param		[Out]a param of Type int& ����Ƶ���µĲ��������
 *  @return 	void 
 */
	void GetRateSample(XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples);

/*
*  @brief   	GetRate	 ȡ��ָ��������Ĳ���Ƶ�� 
*  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
*  @param 		[In]a param of Type  int  ָ��������
 *  @return 	float ����Ƶ�� 
 */
	float GetRate(XJHANDLE hComtrade, int nSamplePos);

/*
 *  @brief   	GetChannelCFG	 ȡ��ָ��ͨ����������Ϣ 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *	@param		[Out]a param of Type ComtradeAChannel& ͨ��������Ϣ�ṹ��
 *  @return 	int  ͨ������.1��ʾģ����ͨ����0��ʾ������ͨ��
 */
	int GetChannelCFG(XJHANDLE hComtrade, int nCn, ComtradeChannel& chl);

/*
 *  @brief   	GetChannelType	 ȡ��ָ��ͨ�������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *  @return 	int  ͨ������.1��ʾģ����ͨ����0��ʾ������ͨ��
 */
	int GetChannelType(XJHANDLE hComtrade, int nCn);

/*
 *  @brief   	GetTriggerTime	 ȡ�ù��ϴ���ʱ������ڿ�ʼ����ʱ�̵����ʱ��
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int  ���ϴ���ʱ������ڿ�ʼ����ʱ�̵����ʱ�䣨���룩
 */
	int GetTriggerTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetTimeRelativeTrigger	 ȡ��ָ������������ڹ��ϴ���ʱ�̵�ʱ�� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	int  ����ڹ��ϴ���ʱ�̵�ʱ�䣨���룩
 */
	int GetTimeRelativeTrigger(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ�� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	int  ����ڲ�����ʼʱ�̵�ʱ�䣨���룩
 */
	int GetTimeRelativeStart(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTotalSamples	 ȡ�ò��������� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int  ����������
 */
	int GetTotalSamples(XJHANDLE hComtrade);

/*
 *  @brief   	GetRealValue	 ȡ��ָ��ͨ��ָ�����������ʵֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *	@param		[In]a param of Type  int  ָ����Ҫ��ֵ�����ͣ�Ĭ��Ϊ����ֵ.0Ϊ����ֵ��1Ϊһ��ֵ
 *  @return 	float  ָ��ͨ��ָ�����������ʵֵ
 */
	float GetRealValue(XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType = 0);

/*
 *  @brief   	GetRecordValue	 ȡ��ָ��ͨ��ָ��������ļ�¼ֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	int  ָ��ͨ��ָ��������ļ�¼ֵ
 */
	int GetRecordValue(XJHANDLE hComtrade, int nCn, int nSampleNo);

/*
 *  @brief   	GetTotalTime	 ȡ����ʱ�� 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @return 	int  ��ʱ��
 */
	int GetTotalTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetMaxValue	 ȡ��ָ��ͨ�������ֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
 *	@param		[In]a param of Type	 int  ָ���Ƿ�ȡ�������ֵ. 0��ʾȡ�������ֵ,1��ʾ��ͨ���Լ������ֵ
 *	@param		[In]a param of Type	 int  ָ����Ҫ��ֵ�����ͣ�0Ϊ����ֵ��1Ϊһ��ֵ
 *  @return 	int  ���ֵ
 */
	float GetMaxValue(XJHANDLE hComtrade, int nCn, int nGroup, int nDataType);

/*
 *  @brief   	GetCTPT	 ȡ��CTPT���
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[Out]a param of Type  float CT���
 *  @param 		[Out]a param of Type  float PT���
 *  @return 	void
 */
	void GetCTPT(XJHANDLE hComtrade, float& fCT, float& fPT);

	/*
 *  @brief   	SetCTPT	 ����CTPT���
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  float CT���
 *  @param 		[In]a param of Type  float PT���
 *  @return 	void
 */
	void SetCTPT(XJHANDLE hComtrade, float fCT, float fPT);

/*
 *  @brief   	GetTimeRelativeTrigger	 ȡ��ָ������������ڹ��ϴ���ʱ�̵�ʱ�䣬����΢��ֵ 
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	double  ����ڹ��ϴ���ʱ�̵�ʱ�䣨΢�룩
 */
	double GetTimeRelativeTriggerMSEC(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ�䣬����΢��ֵ
 *  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
 *  @param 		[In]a param of Type  int  ָ������������.������0��ʼ
 *  @return 	double  ����ڲ�����ʼʱ�̵�ʱ�䣨΢�룩
 */
	double GetTimeRelativeStartMSEC(XJHANDLE hComtrade, int nSampleNo);

	/*
	 *  @brief   	GetHdrInfoObject	 ȡ��HDR�ļ���Ϣ
	 *  @return 	HdrObj	 HDR�ļ���Ϣ����
	 */
	HdrObj* GetHdrInfoObject(XJHANDLE hComtrade);

	/*
	 *  @brief   	GetHdrInfoJSON	 ȡ��HDR�ļ���Ϣ
	 *  @return 	string	 JSON��ʽ��HDR�ļ���Ϣ
	 */
	string GetHdrInfoJSON(XJHANDLE hComtrade);
#endif
#ifdef __cplusplus
}
#endif
#endif
