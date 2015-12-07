#include "Comtrade.h"
#include "COMTRADEParser.h"
#include "Channel.h"

/************************************
函 数 名: LoadFile
功能概要: 载入波形文件
返 回 值: 载入成功返回>=0, 失败返回<0
参    数: param1 波形文件路径
		  param2 错误信息
		  param3 生成的波形数据对象指针
************************************/
int LoadFile( char* sFilePath, char* sError, XJHANDLE& hComtrade )
{
	hComtrade = 0;
	//生成Comtrade对象
	CCOMTRADEParser* pComtrade = new CCOMTRADEParser;
	//载入文件
	CStrongString strFile = sFilePath;
	CStrongString strError = "";
	int nResult = pComtrade->LoadFile(strFile, strError);
	strcpy(sError, strError.c_str());
	if(nResult >= 0)//载入成功
		hComtrade = (XJHANDLE)pComtrade;
	else //载入失败
		delete pComtrade;
	return nResult;
}

/************************************
函 数 名: CloseFile
功能概要: 关闭波形文件
返 回 值: 
参    数: param1 指定波形文件数据对象指针
		  param2 
************************************/
void CloseFile( XJHANDLE hComtrade )
{
	if(hComtrade == NULL)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade != NULL)
		delete pComtrade;
}

/************************************
函 数 名: GetCFGAttr
功能概要: 取得CFG文件记录的波形数据配置属性
返 回 值: 
参    数: param1 指定波形文件数据对象指针
		  param2 波形数据配置属性
************************************/
void GetCFGAttr(XJHANDLE hComtrade, ComtradeCFG& cfgAttr)
{
	if(hComtrade == NULL)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return;
	cfgAttr.sStation		= pComtrade->m_strStationName;
	cfgAttr.sDevice			= pComtrade->m_strDFRID;
	cfgAttr.nYear			= pComtrade->m_nVersion;
	cfgAttr.nAChannels		= pComtrade->m_cAChannels;
	cfgAttr.nDChannels		= pComtrade->m_cDChannels;
	cfgAttr.fLineFreq		= pComtrade->m_fLineFreq;
	cfgAttr.sStartTime		= pComtrade->m_strDTFirst;
	cfgAttr.sTriggerTime	= pComtrade->m_strDTTrigger;
	cfgAttr.nDATType		= pComtrade->m_bAscii;
	cfgAttr.fTimeMult		= pComtrade->m_fTimeMult;
}

/************************************
函 数 名: GetRatesCount
功能概要: 取得采样频率数
返 回 值: 采样频率数
参    数: param1 指定波形文件数据对象指针
		  param2 
************************************/
int GetRatesCount( XJHANDLE hComtrade )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	return pComtrade->m_nNRates;
}

/************************************
函 数 名: GetRateSample
功能概要: 取得指定采样频率索引的采样频率和采样点数
返 回 值: 
参    数: param1 指定波形文件数据对象指针
		  param2 指定采样频率索引
		  param3 采样频率
		  param4 采样频率下的采样点个数
************************************/
void GetRateSample( XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples )
{
	if(hComtrade == NULL)
		return;
	//初始化
	fSampleRate = 0.0;
	nSamples = 0;
	if(nRateIndex < 0)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return;
	//取得对应数据
	if(pComtrade->m_arrSampleRate.size() > nRateIndex)
		fSampleRate = pComtrade->m_arrSampleRate[nRateIndex];
	if(pComtrade->m_arrSamples.size() > nRateIndex)
		nSamples = pComtrade->m_arrSamples[nRateIndex];
}

/************************************
函 数 名: GetRate
功能概要: 取得指定采样点的采样频率
返 回 值: 采样频率
参    数: param1 指定波形文件数据对象指针
		  param2 指定采样点
************************************/
float GetRate( XJHANDLE hComtrade, int nSamplePos )
{
	if(hComtrade == NULL)
		return 0.0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0.0;
	//判断文件是否指定了采样频率
	if(pComtrade->m_nNRates == 0)
		return 0.0;
	//判断是否多采样频率
	if(pComtrade->m_nNRates == 1)
	{
		//单频率
		if(pComtrade->m_arrSampleRate.size() > 0)
			return pComtrade->m_arrSampleRate[0];
	}
	else
	{
		//多频率，先查找指定采样点属于第几个频段
		int nRatePos = 0;
		int nSamplesCount = 0;
		for(int i = 0; i < pComtrade->m_arrSamples.size(); i++)
		{
			int nCurSamples = pComtrade->m_arrSamples[i];
			nSamplesCount += nCurSamples;
			if(nSamplePos <= nSamplesCount)
			{
				//找到频段
				nRatePos = i;
				break;
			}
		}
		//取得频率
		if(pComtrade->m_arrSampleRate.size() > nRatePos && nRatePos >= 0)
			return pComtrade->m_arrSampleRate[nRatePos];
	}
	return 0.0;
}

/************************************
函 数 名: GetChannelCFG
功能概要: 取得指定通道的配置信息
返 回 值: 通道类型.1表示模拟量通道，0表示开关量通道
参    数: param1 指定波形文件数据对象指针
		  param2 指定通道索引.索引从0开始
		  param3 通道配置信息
************************************/
int GetChannelCFG( XJHANDLE hComtrade, int nCn, ComtradeChannel& chl )
{
	if(hComtrade == NULL)
		return -1;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return -1;
	//取得对应通道
	CChannel* pChl = pComtrade->GetChannel(nCn);
	if(pChl == NULL)
		return -1;
	int nReturn = pChl->FillStructValue(chl);
	return nReturn;
}

/************************************
函 数 名: GetChannelType
功能概要: 取得指定通道的类型
返 回 值: 通道类型.1表示模拟量通道，0表示开关量通道
参    数: param1 指定波形文件数据对象指针
		  param2 指定通道索引.索引从0开始
************************************/
int GetChannelType( XJHANDLE hComtrade, int nCn )
{
	if(hComtrade == NULL)
		return -1;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return -1;
	//取得对应通道
	CChannel* pChl = pComtrade->GetChannel(nCn);
	if(pChl == NULL)
		return -1;
	return pChl->m_bAnalog;
}

/************************************
函 数 名: GetTriggerTime
功能概要: 取得故障触发时刻相对于开始采样时刻的相对时间
返 回 值: 故障触发时刻相对于开始采样时刻的相对时间
参    数: param1 指定波形文件数据对象指针
		  param2 
************************************/
int GetTriggerTime(XJHANDLE hComtrade)
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	return pComtrade->m_nTriggerTime;
}

/************************************
函 数 名: GetTimeRelativeTrigger
功能概要: 取得指定采样点相对于故障触发时刻的时间
返 回 值: 相对于故障触发时刻的时间（毫秒）
参    数: param1 指定波形文件数据对象指针
		  param2 指定采样点索引.索引从0开始
************************************/
int GetTimeRelativeTrigger(XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	return pComtrade->GetTimeByTrigger(nSampleNo);
}

/************************************
函 数 名: GetTimeRelativeStart
功能概要: 取得指定采样点相对于采样开始时刻的时间
返 回 值: 相对于采样开始时刻的时间（毫秒）
参    数: param1 指定波形文件数据对象指针
		  param2 指定采样点索引.索引从0开始
************************************/
int GetTimeRelativeStart( XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	return (pComtrade->GetTimeBySampling(nSampleNo) - pComtrade->GetTimeBySampling(0));
}

/************************************
函 数 名: GetTotalSamples
功能概要: 取得采样点总数
返 回 值: 采样点总数
参    数: param1 指定波形文件数据对象指针
		  param2 
************************************/
int GetTotalSamples( XJHANDLE hComtrade )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	return pComtrade->m_cSamples;
}

/************************************
函 数 名: GetRealValue
功能概要: 取得指定通道指定采样点的真实值
返 回 值: 指定通道指定采样点的真实值
参    数: param1 指定波形文件数据对象指针
		  param2 指定通道索引.索引从0开始
		  param3 指定采样点索引.索引从0开始
		  param4 指定需要的值的类型，默认为二次值.0为二次值，1为一次值
************************************/
float GetRealValue( XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType/* = 0*/ )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0.0;
	return pComtrade->GetRealValue(nCn, nSampleNo, nDataType);
}

/************************************
函 数 名: GetTotalTime
功能概要: 取得总时间
返 回 值: 总时间
参    数: param1 指定波形文件数据对象指针
		  param2 
************************************/
int GetTotalTime( XJHANDLE hComtrade )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	int nSize = pComtrade->m_arrSampleTime.size();
	if(nSize > 0)
		return (pComtrade->m_arrSampleTime[nSize-1] - pComtrade->m_arrSampleTime[0])/1000;
	return 0;
}

/************************************
函 数 名: GetMaxValue
功能概要: 取得指定通道的最大值
返 回 值: 最大值
参    数: param1 指定波形文件数据对象指针
		  param2 指定通道索引.索引从0开始
************************************/
float GetMaxValue( XJHANDLE hComtrade, int nCn, int nGroup, int nDataType )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	//取得对应通道
	CChannel* pChl = pComtrade->GetChannel(nCn);
	if(pChl == NULL)
		return 0;

	if(nDataType == 0)//二次值
	{
		if(nGroup == 0)//取公共最大值
		{
			float nReturn = 1;
			switch(pChl->ChannelType())
			{
			case 0:
				nReturn =  pComtrade->m_fMaxUValue;
				break;
			case 1:
				nReturn =  pComtrade->m_fMaxIValue;
				break;
			case 2:
				nReturn =  pComtrade->m_fMaxOther;
				break;
			case -1:
				nReturn = 1;
				break;
			}
			return nReturn;
		}
	}
	else if(nDataType == 1)//一次值
	{
		if(nGroup == 0)//取公共最大值
		{
			float nReturn = 1;
			switch(pChl->ChannelType())
			{
			case 0:
				nReturn =  pComtrade->m_fMaxUValueP;
				break;
			case 1:
				nReturn =  pComtrade->m_fMaxIValueP;
				break;
			case 2:
				nReturn =  pComtrade->m_fMaxOtherP;
				break;
			case -1:
				nReturn = 1;
				break;
			}
			return nReturn;
		}
	}
	if(pChl->m_bAnalog)
	{
		//模拟量通道
		if(nDataType == 0)
			return pChl->m_fMaxValue;
		else if(nDataType == 1)
			return pChl->m_fMaxValueP;

	}
	else
		return 1;
	return 0;
}

/************************************
函 数 名: GetRecordValue
功能概要: 取得指定通道指定采样点的记录值
返 回 值: 指定通道指定采样点的记录值
参    数: param1 指定波形文件数据对象指针
		  param2 指定通道索引.索引从0开始
		  param3 指定采样点索引.索引从0开始
************************************/
int GetRecordValue( XJHANDLE hComtrade, int nCn, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0.0;
	return pComtrade->GetRecordValue(nCn, nSampleNo);
}

/*************************************************************
函 数 名: GetCTPT
功能概要: 取得CTPT变比
返 回 值: 
参    数: param1 指定波形文件数据对象指针
		  param2 CT变比
		  param3 PT变比
*************************************************************/
void GetCTPT( XJHANDLE hComtrade, float& fCT, float& fPT )
{
	if(hComtrade == NULL)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return;
	fCT = pComtrade->m_fCT;
	fPT = pComtrade->m_fPT;
}

/*************************************************************
函 数 名: SetCTPT
功能概要: 设置CTPT变比
返 回 值: 
参    数: param1 指定波形文件数据对象指针
		  param2 CT变比
		  param3 PT变比
*************************************************************/
void SetCTPT( XJHANDLE hComtrade, float fCT, float fPT )
{
	if(hComtrade == NULL)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return;
	pComtrade->m_fCT = fCT;
	pComtrade->m_fPT = fPT;
}

/****************************************************
Date:2012/5/15  Author:LYH
函数名:   GetTimeRelativeTriggerMSEC	
返回值:   double	相对于故障触发时刻的时间（微秒）
功能概要: 取得指定采样点相对于故障触发时刻的时间，返回微秒值 
参数: XJHANDLE hComtrade	指定波形文件数据对象指针
参数: int nSampleNo	指定采样点索引.索引从0开始
*****************************************************/
double GetTimeRelativeTriggerMSEC( XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	if(nSampleNo < 0 || nSampleNo > pComtrade->m_arrSampleTime.size())
		return -1;
	double dTime = pComtrade->m_arrSampleTime[nSampleNo];//微秒
	double dStartTime = pComtrade->m_arrSampleTime[0];
	return (dTime-(dStartTime+pComtrade->m_nTriggerTime*1000));
}

/****************************************************
Date:2012/5/15  Author:LYH
函数名:   GetTimeRelativeStartMSEC	
返回值:   double	相对于采样开始时刻的时间（微秒）
功能概要: 取得指定采样点相对于采样开始时刻的时间，返回微秒值
参数: XJHANDLE hComtrade	指定波形文件数据对象指针
参数: int nSampleNo	指定采样点索引.索引从0开始
*****************************************************/
double GetTimeRelativeStartMSEC( XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	if(nSampleNo < 0 || nSampleNo > pComtrade->m_arrSampleTime.size())
		return -1;
	double dTime = pComtrade->m_arrSampleTime[nSampleNo];//微秒
	double dStartTime = pComtrade->m_arrSampleTime[0];
	return (dTime-dStartTime);
}

HdrObj* GetHdrInfoObject(XJHANDLE hComtrade)
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return 0;
	if( pComtrade->m_nHdrParseResult < 0 )
		pComtrade->m_nHdrParseResult = pComtrade->ParseHdr();
	return &pComtrade->m_Hdr;
}

std::string GetHdrInfoJSON(XJHANDLE hComtrade)
{
	if(hComtrade == NULL)
		return "";
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//无效的指针
		return "";
	if( pComtrade->m_nHdrParseResult < 0 )
		pComtrade->m_nHdrParseResult = pComtrade->ParseHdr();
	if( pComtrade->m_sHdrJson.empty() )
	{
		pComtrade->m_sHdrJson = pComtrade->HdrObjToJson();
	}
	return pComtrade->m_sHdrJson;
}
