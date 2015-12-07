 
#ifndef XJReadWave_Comtrade_2012_04_25
#define XJReadWave_Comtrade_2012_04_25
 
#include "./include/jni.h" 
//#include "../XJComtrade/XJComtrade.h"
#include "../../../Interface/ComtradeDefine.h"
#include "../../../common/define.h"


//  载入波形文件 
typedef int (*LoadFileFUN)(char* sFilePath, char* sError, XJHANDLE& hComtrade);
// 	关闭文件
typedef void (*CloseFileFUN)(XJHANDLE hComtrade);
//2取得采样频率数
typedef int (*GetRatesCountFUN)(XJHANDLE hComtrade);
//3 取得CFG文件记录的波形数据配置属性 
typedef void (*GetCFGAttrFUN)(XJHANDLE hComtrade, ComtradeCFG& cfgAttr);
//4取得采样点总数 
typedef int (*GetTotalSamplesFUN)(XJHANDLE hComtrade);  
//5 取得指定通道的配置信息 
typedef int (*GetChannelCFGFUN)(XJHANDLE hComtrade,int nCn,ComtradeChannel& channel); // 
//6	    取得指定采样点相对于采样开始时刻的时间
typedef int (*GetTimeRelativeStartFUN)(XJHANDLE hComtrade,int nSampleNo);  
//7  	取得指定采样点的采样频率
typedef float (*GetRateFUN)(XJHANDLE hComtrade,int nSamplePos); 
//8	 取得故障触发时刻时间
typedef int  (*GetTriggerTimeFUN)(XJHANDLE hComtrade); 
//9	取得指定采样频率索引的采样频率和采样点数
typedef void (*GetRateSampleFUN)(XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples);
//10	取得指定通道的类型
typedef int (*GetChannelTypeFUN)(XJHANDLE hComtrade, int nCn); 
////////////////////////////////////////////////////////////////////////////////////////////////////////////// 
// 取得指定采样点相对于故障触发时刻的时间   
typedef int (*GetTimeRelativeTriggerFUN)(XJHANDLE hComtrade, int nSampleNo); 
//	 取得指定通道指定采样点的真实值  
typedef float (*GetRealValueFUN)(XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType  ); 
//  取得指定通道指定采样点的记录值  
typedef int (*GetRecordValueFUN)(XJHANDLE hComtrade, int nCn, int nSampleNo); 
// 取得总时间   
typedef int(*GetTotalTimeFUN)(XJHANDLE hComtrade); 
/*
*  @brief   	GetMaxValue	 取得指定通道的最大值
*  @param 		[In]a param of Type  int  指定波形文件数据对象指针
*  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
*	@param		[In]a param of Type	 int  指定是否取公共最大值. 0表示取公共最大值,1表示各通道自己的最大值
*	@param		[In]a param of Type	 int  指定需要的值的类型，0为二次值，1为一次值
*  @return 	int  最大值
*/ 
typedef float (*GetMaxValueFUN)(XJHANDLE hComtrade, int nCn, int nGroup, int nDataType); 
//    取得CTPT变比 
typedef void (*GetCTPTFUN)(XJHANDLE hComtrade, float& fCT, float& fPT); 
//设置CTPT变比  
typedef void (*SetCTPTFUN)(XJHANDLE hComtrade, float fCT, float fPT);
typedef string (*GetHdrInfoJSON)(XJHANDLE);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
 
class Comtrade
{ 
private: 
	static  bool loadQuickComtrade(); //加载XJComtrade.dll
	static bool  isLoadQuickComtrade ;
	 XJHANDLE  hQuickComtrade;
	 char sMsgError[256] ;
	 int m_nA;
	 int m_nD;
	 int nTotalSamples ;//采样点总数
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
函 数 名：jstringToNative()
功能概要：将Java的Unicode字符串转化本地字符;
返 回 值: char* rtn 
参    数：JNIEnv  *env,        
参    数：jstring jstr         字符串      
**************************************************************/
char* jstringToWindows(JNIEnv *env, jstring jstr)  ;
   

/*************************************************************
函 数 名：nativeTojstring()  
功能概要：将Java的Unicode字符串转化本地字符;
返 回 值: jstring jstr 
参    数：JNIEnv  *env,        
参    数：char* rtn         字符串      
**************************************************************/
jstring windowsToJstring( JNIEnv* env,const char* str );  

void freeQuickComtrade();

#endif // !defined(AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_)
