// COMTRADEParser.h: interface for the CCOMTRADEParser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMTRADEPARSER_H__2291058C_3BBF_4E61_A9A8_EDB98D42EFFF__INCLUDED_)
#define AFX_COMTRADEPARSER_H__2291058C_3BBF_4E61_A9A8_EDB98D42EFFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(INT16)
typedef signed short        INT16;
#endif
#if !defined(INT32)
typedef signed int          INT32;
#endif

#include "Channel.h"

#include "constant.h"

class CCOMTRADEParser  
{
public:
	CCOMTRADEParser();
	virtual ~CCOMTRADEParser();

/*
 *  @brief   	GetFileName	 取得文件名，不带后缀 
 *  @param 		[In]a param of Type  CXJString  指定要打开文件的文件名
 *  @return 	void 
 */
	void GetFileName(CStrongString sFileName);
	
/*
 *  @brief   	LoadCfgData	 读取CFG文件 
 *  @param 		[Out]a param of Type  CXJString&  错误信息
 *  @return 	int 读取失败返回<0, 成功返回>=0
 */
	int LoadCfgData(CStrongString& strError);    
	
/*
 *  @brief   	LoadDataFromFile	 读取DAT文件 
 *  @param 		[Out]a param of Type  CXJString&  错误信息
 *  @return 	int 读取失败返回<0, 成功返回>=0
 */
	int LoadDataFromFile(CStrongString& strError); 

/*
 *  @brief   	CompatibleDiff	 兼容对规范的不同理解 
 *  @return 	void 
 */
	void CompatibleDiff();

/*
 *  @brief   	LoadFile	 载入波形文件 
 *  @param 		[In]a param of Type  CXJString  波形文件路径
 *  @param 		[Out]a param of Type  CXJString&  错误信息
 *  @return 	int 载入成功返回>=0, 失败返回<0
 */
	int LoadFile(CStrongString strFileName, CStrongString& strError);

/*
 *  @brief   	GetChannel	 取得指定索引号的通道对象 
 *  @param 		[In]a param of Type  int  指定通道索引
 *  @return 	CChannel* 通道对象指针
 */
	CChannel* GetChannel(int nChannelNO);

/*
 *  @brief   	GetRecordValue	 取得指定通道指定采样点的值 
 *  @param 		[In]a param of Type  int  指定通道索引
 *  @param 		[In]a param of Type  int  指定采样点号
 *  @return 	int 采样值.DAT文件里记录的值
 */
	int GetRecordValue(int nChannelNo, int nSampleNo);

/*
 *  @brief   	GetRealValue	 取得指定通道指定采样点的值 
 *  @param 		[In]a param of Type  int  指定通道索引
 *  @param 		[In]a param of Type  int  指定采样点号
 *  @param		[In]a param of Type  int  指定需要的值的类型，默认为二次值.0为二次值，1为一次值
 *  @return 	int 真实值，模拟通道经过ax+b计算得来.
 */
	float GetRealValue(int nChannelNo, int nSampleNo, int nDataType = 0);

/*
 *  @brief   	TransformSecondaryValue	 将指定通道的指定值转化为二次值 
 *  @param 		[In]a param of Type  CChannel*  通道
 *  @param 		[In]a param of Type  float  要转化的值
 *  @return 	float 二次值
 */
	float TransformSecondaryValue(CChannel* pChl, float fValue);

/*
 *  @brief   	TransformPrimaryValue	 将指定通道的指定值转化为一次值 
 *  @param 		[In]a param of Type  CChannel*  通道
 *  @param 		[In]a param of Type  float  要转化的值
 *  @return 	float 一次值
 */
	float TransformPrimaryValue(CChannel* pChl, float fValue);

/*
 *  @brief   	GetTimeBySampling	 取得指定采样点相对于采样开始时刻的时间
 *  @param 		[In]a param of Type  int  指定采样点号
 *  @return 	int 相对于采样开始时刻的时间(单位：毫秒)
 */
	int GetTimeBySampling(int nSampleNo);

/*
 *  @brief   	GetTimeByTrigger	 取得指定采样点相对于故障开始时刻的时间
 *  @param 		[In]a param of Type  int  指定采样点号
 *  @return 	int 相对于故障开始时刻的时间(单位：毫秒)
 */
	int GetTimeByTrigger(int nSampleNo);

/*
 *  @brief   	FreeData	 清除数据
 *  @return 	bool 成功返回true，失败返回false
 */
	bool FreeData();

/*
 *  @brief   	FillData	 填充数据到通道
 *  @param 		[In]a param of Type  BOOL  是否Ascii格式
 *  @param 		[In]a param of Type  INT32*  指定数据数组
 *  @param 		[In]a param of Type  int*  用于记录开关量的数组
 *  @return 	void
 */
	void FillData(BOOL bAscii, INT32 *pData, int* pDigital);

/*
 *  @brief   	SetMaxValue	 设置默认最大值
 *  @return 	void
 */
	void SetMaxValue();

	int ParseHdr();

	string HdrObjToJson();

public:
	/** @brief           COMTRADE文件版本号*/
	int m_nVersion;

	/** @brief           包括路径和文件名,不包括扩展名*/
	CStrongString m_strName;

	/** @brief           厂站名*/
	CStrongString m_strStationName;

	/** @brief           故障录波器名*/
	CStrongString m_strDFRID;

	/** @brief           采样点总数*/
	int m_cSamples;

	/** @brief           模拟量通道数量*/
	int m_cAChannels;

	/** @brief           开关量通道数量*/
	int m_cDChannels;

	/** @brief           线路频率*/
	float m_fLineFreq;

	/** @brief           采样频率数*/
	int m_nNRates;

	/** @brief           时间乘数系数*/
	float m_fTimeMult;

	/** @brief           开始采样时间*/
	CStrongString m_strDTFirst;

	/** @brief           故障触发事件*/
	CStrongString m_strDTTrigger;

	/** @brief           true: ASCII format ；false: Binary format*/
	bool m_bAscii;

	/** @brief           对应各种采样频率的采样点总数*/
	vector<int>  m_arrSamples;

	/** @brief           各种采样频率。单位：Hz*/
	vector<float> m_arrSampleRate;

 	/** @brief           通道信息*/
 	vector<CChannel*> m_arrChannel;

	/** @brief           记录各采样点对应的时间（相对于开始采样时间）。单位：微秒*/
	vector<int> m_arrSampleTime;

	/** @brief           用于读取DAT文件时计数*/
	int m_nCount;

	/** @brief           故障触发时刻相对于采样开始时刻的时间（单位：毫秒）*/
	int		m_nTriggerTime;

	/** @brief           CT变比*/
	float	m_fCT;
	/** @brief           PT变比*/
	float	m_fPT;

	/** @brief           电流最大值*/
	float		m_fMaxIValue;
	/** @brief           电压最大值*/
	float		m_fMaxUValue;
	/** @brief           其它最大值*/
	float		m_fMaxOther;

	/** @brief           电流一次值最大值*/
	float		m_fMaxIValueP;
	/** @brief           电压一次值最大值*/
	float		m_fMaxUValueP;
	/** @brief           其它一次值最大值*/
	float		m_fMaxOtherP;

	/** @brief           头文件信息*/
	HdrObj		m_Hdr;
	/** @brief           -1:未分析；0:HDR文件不存在；1:不是标准XML格式；2:标准XML格式，已经分析*/
	int			m_nHdrParseResult;
	string		m_sHdrJson;
private:
/*
 *  @brief   	CalTriggerTime	 计算故障触发时刻相对于采样开始时刻的时间（单位：毫秒），结果保存在m_nTriggerTime
 *  @return 	
 */
	void CalTriggerTime();

};

#endif // !defined(AFX_COMTRADEPARSER_H__2291058C_3BBF_4E61_A9A8_EDB98D42EFFF__INCLUDED_)
