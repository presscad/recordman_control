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
 *  @brief   	LoadFile	 载入波形文件 
 *  @param 		[In]a param of Type  char*  波形文件路径
 *  @param 		[Out]a param of Type  char*  错误信息
 *  @param 		[Out]a param of Type  int&  生成的波形文件数据对象指针
 *  @return 	int 载入成功返回>=0, 失败返回<0
 */
	extern "C" __declspec( dllexport ) int LoadFile(char* sFilePath, char* sError, XJHANDLE& hComtrade);

/*
 *  @brief   	CloseFile	 关闭波形文件 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	void
 */
	extern "C" __declspec( dllexport ) void CloseFile(XJHANDLE hComtrade);

/*
 *  @brief   	GetCFGAttr	 取得CFG文件记录的波形数据配置属性 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[Out]a param of Type  ComtradeCFG&  波形数据配置属性
 *  @return 	void 
 */
	extern "C" __declspec( dllexport ) void GetCFGAttr(XJHANDLE hComtrade, ComtradeCFG& cfgAttr);

/*
 *  @brief   	GetRatesCount	 取得采样频率数
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int 采样频率数 
 */
	extern "C" __declspec( dllexport ) int GetRatesCount(XJHANDLE hComtrade);

/*
 *  @brief   	GetRateSample	 取得指定采样频率索引的采样频率和采样点数 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样频率索引.索引从0开始
 *	@param		[Out]a param of Type float& 采样频率
 *	@param		[Out]a param of Type int& 采样频率下的采样点个数
 *  @return 	void 
 */
	extern "C" __declspec( dllexport ) void GetRateSample(XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples);

/*
*  @brief   	GetRate	 取得指定采样点的采样频率 
*  @param 		[In]a param of Type  int  指定波形文件数据对象指针
*  @param 		[In]a param of Type  int  指定采样点
 *  @return 	float 采样频率 
 */
	extern "C" __declspec( dllexport ) float GetRate(XJHANDLE hComtrade, int nSamplePos);

/*
 *  @brief   	GetChannelCFG	 取得指定通道的配置信息 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *	@param		[Out]a param of Type ComtradeAChannel& 通道配置信息结构体
 *  @return 	int  通道类型.1表示模拟量通道，0表示开关量通道
 */
	extern "C" __declspec( dllexport ) int GetChannelCFG(XJHANDLE hComtrade, int nCn, ComtradeChannel& chl);

/*
 *  @brief   	GetChannelType	 取得指定通道的类型 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *  @return 	int  通道类型.1表示模拟量通道，0表示开关量通道
 */
	extern "C" __declspec( dllexport ) int GetChannelType(XJHANDLE hComtrade, int nCn);

/*
 *  @brief   	GetTriggerTime	 取得故障触发时刻相对于开始采样时刻的相对时间
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int  故障触发时刻相对于开始采样时刻的相对时间（毫秒）
 */
	extern "C" __declspec( dllexport ) int GetTriggerTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetTimeRelativeTrigger	 取得指定采样点相对于故障触发时刻的时间 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	int  相对于故障触发时刻的时间（毫秒）
 */
	extern "C" __declspec( dllexport ) int GetTimeRelativeTrigger(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 取得指定采样点相对于采样开始时刻的时间 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	int  相对于采样开始时刻的时间（毫秒）
 */
	extern "C" __declspec( dllexport ) int GetTimeRelativeStart(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTotalSamples	 取得采样点总数 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int  采样点总数
 */
	extern "C" __declspec( dllexport ) int GetTotalSamples(XJHANDLE hComtrade);

/*
 *  @brief   	GetRealValue	 取得指定通道指定采样点的真实值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *	@param		[In]a param of Type  int  指定需要的值的类型，默认为二次值.0为二次值，1为一次值
 *  @return 	float  指定通道指定采样点的真实值
 */
	extern "C" __declspec( dllexport ) float GetRealValue(XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType = 0);

/*
 *  @brief   	GetRecordValue	 取得指定通道指定采样点的记录值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	int  指定通道指定采样点的记录值
 */
	extern "C" __declspec( dllexport ) int GetRecordValue(XJHANDLE hComtrade, int nCn, int nSampleNo);

/*
 *  @brief   	GetTotalTime	 取得总时间 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int  总时间
 */
	extern "C" __declspec( dllexport ) int GetTotalTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetMaxValue	 取得指定通道的最大值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *	@param		[In]a param of Type	 int  指定是否取公共最大值. 0表示取公共最大值,1表示各通道自己的最大值
 *	@param		[In]a param of Type	 int  指定需要的值的类型，0为二次值，1为一次值
 *  @return 	int  最大值
 */
	extern "C" __declspec( dllexport ) float GetMaxValue(XJHANDLE hComtrade, int nCn, int nGroup, int nDataType);

/*
 *  @brief   	GetCTPT	 取得CTPT变比
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[Out]a param of Type  float CT变比
 *  @param 		[Out]a param of Type  float PT变比
 *  @return 	void
 */
	extern "C" __declspec( dllexport ) void GetCTPT(XJHANDLE hComtrade, float& fCT, float& fPT);

	/*
 *  @brief   	SetCTPT	 设置CTPT变比
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  float CT变比
 *  @param 		[In]a param of Type  float PT变比
 *  @return 	void
 */
	extern "C" __declspec( dllexport ) void SetCTPT(XJHANDLE hComtrade, float fCT, float fPT);

/*
 *  @brief   	GetTimeRelativeTrigger	 取得指定采样点相对于故障触发时刻的时间，返回微秒值 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	double  相对于故障触发时刻的时间（微秒）
 */
	extern "C" __declspec( dllexport ) double GetTimeRelativeTriggerMSEC(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 取得指定采样点相对于采样开始时刻的时间，返回微秒值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	double  相对于采样开始时刻的时间（微秒）
 */
	extern "C" __declspec( dllexport ) double GetTimeRelativeStartMSEC(XJHANDLE hComtrade, int nSampleNo);
	/*
	 *  @brief   	GetHdrInfoObject	 取得HDR文件信息
	 *  @return 	HdrObj	 HDR文件信息对象
	 */
	extern "C" __declspec( dllexport ) HdrObj* GetHdrInfoObject(XJHANDLE hComtrade);

	/*
	 *  @brief   	GetHdrInfoJSON	 取得HDR文件信息
	 *  @return 	string	 JSON格式的HDR文件信息
	 */
	extern "C" __declspec( dllexport ) string GetHdrInfoJSON(XJHANDLE hComtrade);

#else
/*
 *  @brief   	LoadFile	 载入波形文件 
 *  @param 		[In]a param of Type  char*  波形文件路径
 *  @param 		[Out]a param of Type  char*  错误信息
 *  @param 		[Out]a param of Type  int&  生成的波形文件数据对象指针
 *  @return 	int 载入成功返回>=0, 失败返回<0
 */
	int LoadFile(char* sFilePath, char* sError, XJHANDLE& hComtrade);

/*
 *  @brief   	CloseFile	 关闭波形文件 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	void
 */
	void CloseFile(XJHANDLE hComtrade);

/*
 *  @brief   	GetCFGAttr	 取得CFG文件记录的波形数据配置属性 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[Out]a param of Type  ComtradeCFG&  波形数据配置属性
 *  @return 	void 
 */
	void GetCFGAttr(XJHANDLE hComtrade, ComtradeCFG& cfgAttr);

/*
 *  @brief   	GetRatesCount	 取得采样频率数
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int 采样频率数 
 */
	int GetRatesCount(XJHANDLE hComtrade);

/*
 *  @brief   	GetRateSample	 取得指定采样频率索引的采样频率和采样点数 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样频率索引.索引从0开始
 *	@param		[Out]a param of Type float& 采样频率
 *	@param		[Out]a param of Type int& 采样频率下的采样点个数
 *  @return 	void 
 */
	void GetRateSample(XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples);

/*
*  @brief   	GetRate	 取得指定采样点的采样频率 
*  @param 		[In]a param of Type  int  指定波形文件数据对象指针
*  @param 		[In]a param of Type  int  指定采样点
 *  @return 	float 采样频率 
 */
	float GetRate(XJHANDLE hComtrade, int nSamplePos);

/*
 *  @brief   	GetChannelCFG	 取得指定通道的配置信息 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *	@param		[Out]a param of Type ComtradeAChannel& 通道配置信息结构体
 *  @return 	int  通道类型.1表示模拟量通道，0表示开关量通道
 */
	int GetChannelCFG(XJHANDLE hComtrade, int nCn, ComtradeChannel& chl);

/*
 *  @brief   	GetChannelType	 取得指定通道的类型 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *  @return 	int  通道类型.1表示模拟量通道，0表示开关量通道
 */
	int GetChannelType(XJHANDLE hComtrade, int nCn);

/*
 *  @brief   	GetTriggerTime	 取得故障触发时刻相对于开始采样时刻的相对时间
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int  故障触发时刻相对于开始采样时刻的相对时间（毫秒）
 */
	int GetTriggerTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetTimeRelativeTrigger	 取得指定采样点相对于故障触发时刻的时间 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	int  相对于故障触发时刻的时间（毫秒）
 */
	int GetTimeRelativeTrigger(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 取得指定采样点相对于采样开始时刻的时间 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	int  相对于采样开始时刻的时间（毫秒）
 */
	int GetTimeRelativeStart(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTotalSamples	 取得采样点总数 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int  采样点总数
 */
	int GetTotalSamples(XJHANDLE hComtrade);

/*
 *  @brief   	GetRealValue	 取得指定通道指定采样点的真实值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *	@param		[In]a param of Type  int  指定需要的值的类型，默认为二次值.0为二次值，1为一次值
 *  @return 	float  指定通道指定采样点的真实值
 */
	float GetRealValue(XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType = 0);

/*
 *  @brief   	GetRecordValue	 取得指定通道指定采样点的记录值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	int  指定通道指定采样点的记录值
 */
	int GetRecordValue(XJHANDLE hComtrade, int nCn, int nSampleNo);

/*
 *  @brief   	GetTotalTime	 取得总时间 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @return 	int  总时间
 */
	int GetTotalTime(XJHANDLE hComtrade);

/*
 *  @brief   	GetMaxValue	 取得指定通道的最大值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定通道索引.索引从0开始
 *	@param		[In]a param of Type	 int  指定是否取公共最大值. 0表示取公共最大值,1表示各通道自己的最大值
 *	@param		[In]a param of Type	 int  指定需要的值的类型，0为二次值，1为一次值
 *  @return 	int  最大值
 */
	float GetMaxValue(XJHANDLE hComtrade, int nCn, int nGroup, int nDataType);

/*
 *  @brief   	GetCTPT	 取得CTPT变比
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[Out]a param of Type  float CT变比
 *  @param 		[Out]a param of Type  float PT变比
 *  @return 	void
 */
	void GetCTPT(XJHANDLE hComtrade, float& fCT, float& fPT);

	/*
 *  @brief   	SetCTPT	 设置CTPT变比
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  float CT变比
 *  @param 		[In]a param of Type  float PT变比
 *  @return 	void
 */
	void SetCTPT(XJHANDLE hComtrade, float fCT, float fPT);

/*
 *  @brief   	GetTimeRelativeTrigger	 取得指定采样点相对于故障触发时刻的时间，返回微秒值 
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	double  相对于故障触发时刻的时间（微秒）
 */
	double GetTimeRelativeTriggerMSEC(XJHANDLE hComtrade, int nSampleNo);

/*
 *  @brief   	GetTimeRelativeStart	 取得指定采样点相对于采样开始时刻的时间，返回微秒值
 *  @param 		[In]a param of Type  int  指定波形文件数据对象指针
 *  @param 		[In]a param of Type  int  指定采样点索引.索引从0开始
 *  @return 	double  相对于采样开始时刻的时间（微秒）
 */
	double GetTimeRelativeStartMSEC(XJHANDLE hComtrade, int nSampleNo);

	/*
	 *  @brief   	GetHdrInfoObject	 取得HDR文件信息
	 *  @return 	HdrObj	 HDR文件信息对象
	 */
	HdrObj* GetHdrInfoObject(XJHANDLE hComtrade);

	/*
	 *  @brief   	GetHdrInfoJSON	 取得HDR文件信息
	 *  @return 	string	 JSON格式的HDR文件信息
	 */
	string GetHdrInfoJSON(XJHANDLE hComtrade);
#endif
#ifdef __cplusplus
}
#endif
#endif
