 
#ifndef XJReadWave_Comtrade_2012_04_25
#define XJReadWave_Comtrade_2012_04_25
 
#include "./include/jni.h" 
//#include "../XJComtrade/XJComtrade.h"
#include "../../../Interface/ComtradeDefine.h"
#include "../../../common/define.h"


//  ���벨���ļ� 
typedef int (*LoadFileFUN)(char* sFilePath, char* sError, XJHANDLE& hComtrade);
// 	�ر��ļ�
typedef void (*CloseFileFUN)(XJHANDLE hComtrade);
//2ȡ�ò���Ƶ����
typedef int (*GetRatesCountFUN)(XJHANDLE hComtrade);
//3 ȡ��CFG�ļ���¼�Ĳ��������������� 
typedef void (*GetCFGAttrFUN)(XJHANDLE hComtrade, ComtradeCFG& cfgAttr);
//4ȡ�ò��������� 
typedef int (*GetTotalSamplesFUN)(XJHANDLE hComtrade);  
//5 ȡ��ָ��ͨ����������Ϣ 
typedef int (*GetChannelCFGFUN)(XJHANDLE hComtrade,int nCn,ComtradeChannel& channel); // 
//6	    ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ��
typedef int (*GetTimeRelativeStartFUN)(XJHANDLE hComtrade,int nSampleNo);  
//7  	ȡ��ָ��������Ĳ���Ƶ��
typedef float (*GetRateFUN)(XJHANDLE hComtrade,int nSamplePos); 
//8	 ȡ�ù��ϴ���ʱ��ʱ��
typedef int  (*GetTriggerTimeFUN)(XJHANDLE hComtrade); 
//9	ȡ��ָ������Ƶ�������Ĳ���Ƶ�ʺͲ�������
typedef void (*GetRateSampleFUN)(XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples);
//10	ȡ��ָ��ͨ��������
typedef int (*GetChannelTypeFUN)(XJHANDLE hComtrade, int nCn); 
////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
// ȡ��ָ������������ڹ��ϴ���ʱ�̵�ʱ��   
typedef int (*GetTimeRelativeTriggerFUN)(XJHANDLE hComtrade, int nSampleNo); 
//	 ȡ��ָ��ͨ��ָ�����������ʵֵ  
typedef float (*GetRealValueFUN)(XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType  ); 
//  ȡ��ָ��ͨ��ָ��������ļ�¼ֵ  
typedef int (*GetRecordValueFUN)(XJHANDLE hComtrade, int nCn, int nSampleNo); 
// ȡ����ʱ��   
typedef int(*GetTotalTimeFUN)(XJHANDLE hComtrade); 
/*
*  @brief   	GetMaxValue	 ȡ��ָ��ͨ�������ֵ
*  @param 		[In]a param of Type  int  ָ�������ļ����ݶ���ָ��
*  @param 		[In]a param of Type  int  ָ��ͨ������.������0��ʼ
*	@param		[In]a param of Type	 int  ָ���Ƿ�ȡ�������ֵ. 0��ʾȡ�������ֵ,1��ʾ��ͨ���Լ������ֵ
*	@param		[In]a param of Type	 int  ָ����Ҫ��ֵ�����ͣ�0Ϊ����ֵ��1Ϊһ��ֵ
*  @return 	int  ���ֵ
*/ 
typedef float (*GetMaxValueFUN)(XJHANDLE hComtrade, int nCn, int nGroup, int nDataType); 
//    ȡ��CTPT��� 
typedef void (*GetCTPTFUN)(XJHANDLE hComtrade, float& fCT, float& fPT); 
//����CTPT���  
typedef void (*SetCTPTFUN)(XJHANDLE hComtrade, float fCT, float fPT);
typedef string (*GetHdrInfoJSON)(XJHANDLE);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
 
class Comtrade
{ 
private: 
	static  bool loadQuickComtrade(); //����XJComtrade.dll
	static bool  isLoadQuickComtrade ;
	 XJHANDLE  hQuickComtrade;
	 char sMsgError[256] ;
	 int m_nA;
	 int m_nD;
	 int nTotalSamples ;//����������
public:
	Comtrade(void);
	~Comtrade(void); 
	int LoadFile(char* sFilePath, char* sError );

	int	GetComtradeCfg (JNIEnv *env, jclass,jobject cfg, char* sError);

	int	GetComtradeA  (JNIEnv *env, jclass,jobject a, char* sError);

	int	GetComtradeD (JNIEnv *env, jclass,jobject d, char* sError);

	jstring GetHdrInfo(JNIEnv* env);
	 
	bool isFreeQuickComtrade ;
};


/*************************************************************
�� �� ����jstringToNative()
���ܸ�Ҫ����Java��Unicode�ַ���ת�������ַ�;
�� �� ֵ: char* rtn 
��    ����JNIEnv  *env,        
��    ����jstring jstr         �ַ���      
**************************************************************/
char* jstringToWindows(JNIEnv *env, jstring jstr)  ;
   

/*************************************************************
�� �� ����nativeTojstring()  
���ܸ�Ҫ����Java��Unicode�ַ���ת�������ַ�;
�� �� ֵ: jstring jstr 
��    ����JNIEnv  *env,        
��    ����char* rtn         �ַ���      
**************************************************************/
jstring windowsToJstring( JNIEnv* env,const char* str );  

void freeQuickComtrade();

#endif // !defined(AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_)
