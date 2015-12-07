#include "Comtrade.h"
#include "COMTRADEParser.h"
#include "Channel.h"

/************************************
�� �� ��: LoadFile
���ܸ�Ҫ: ���벨���ļ�
�� �� ֵ: ����ɹ�����>=0, ʧ�ܷ���<0
��    ��: param1 �����ļ�·��
		  param2 ������Ϣ
		  param3 ���ɵĲ������ݶ���ָ��
************************************/
int LoadFile( char* sFilePath, char* sError, XJHANDLE& hComtrade )
{
	hComtrade = 0;
	//����Comtrade����
	CCOMTRADEParser* pComtrade = new CCOMTRADEParser;
	//�����ļ�
	CStrongString strFile = sFilePath;
	CStrongString strError = "";
	int nResult = pComtrade->LoadFile(strFile, strError);
	strcpy(sError, strError.c_str());
	if(nResult >= 0)//����ɹ�
		hComtrade = (XJHANDLE)pComtrade;
	else //����ʧ��
		delete pComtrade;
	return nResult;
}

/************************************
�� �� ��: CloseFile
���ܸ�Ҫ: �رղ����ļ�
�� �� ֵ: 
��    ��: param1 ָ�������ļ����ݶ���ָ��
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
�� �� ��: GetCFGAttr
���ܸ�Ҫ: ȡ��CFG�ļ���¼�Ĳ���������������
�� �� ֵ: 
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ����������������
************************************/
void GetCFGAttr(XJHANDLE hComtrade, ComtradeCFG& cfgAttr)
{
	if(hComtrade == NULL)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
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
�� �� ��: GetRatesCount
���ܸ�Ҫ: ȡ�ò���Ƶ����
�� �� ֵ: ����Ƶ����
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 
************************************/
int GetRatesCount( XJHANDLE hComtrade )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	return pComtrade->m_nNRates;
}

/************************************
�� �� ��: GetRateSample
���ܸ�Ҫ: ȡ��ָ������Ƶ�������Ĳ���Ƶ�ʺͲ�������
�� �� ֵ: 
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ������Ƶ������
		  param3 ����Ƶ��
		  param4 ����Ƶ���µĲ��������
************************************/
void GetRateSample( XJHANDLE hComtrade, int nRateIndex, float& fSampleRate, int& nSamples )
{
	if(hComtrade == NULL)
		return;
	//��ʼ��
	fSampleRate = 0.0;
	nSamples = 0;
	if(nRateIndex < 0)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return;
	//ȡ�ö�Ӧ����
	if(pComtrade->m_arrSampleRate.size() > nRateIndex)
		fSampleRate = pComtrade->m_arrSampleRate[nRateIndex];
	if(pComtrade->m_arrSamples.size() > nRateIndex)
		nSamples = pComtrade->m_arrSamples[nRateIndex];
}

/************************************
�� �� ��: GetRate
���ܸ�Ҫ: ȡ��ָ��������Ĳ���Ƶ��
�� �� ֵ: ����Ƶ��
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ��������
************************************/
float GetRate( XJHANDLE hComtrade, int nSamplePos )
{
	if(hComtrade == NULL)
		return 0.0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0.0;
	//�ж��ļ��Ƿ�ָ���˲���Ƶ��
	if(pComtrade->m_nNRates == 0)
		return 0.0;
	//�ж��Ƿ�����Ƶ��
	if(pComtrade->m_nNRates == 1)
	{
		//��Ƶ��
		if(pComtrade->m_arrSampleRate.size() > 0)
			return pComtrade->m_arrSampleRate[0];
	}
	else
	{
		//��Ƶ�ʣ��Ȳ���ָ�����������ڵڼ���Ƶ��
		int nRatePos = 0;
		int nSamplesCount = 0;
		for(int i = 0; i < pComtrade->m_arrSamples.size(); i++)
		{
			int nCurSamples = pComtrade->m_arrSamples[i];
			nSamplesCount += nCurSamples;
			if(nSamplePos <= nSamplesCount)
			{
				//�ҵ�Ƶ��
				nRatePos = i;
				break;
			}
		}
		//ȡ��Ƶ��
		if(pComtrade->m_arrSampleRate.size() > nRatePos && nRatePos >= 0)
			return pComtrade->m_arrSampleRate[nRatePos];
	}
	return 0.0;
}

/************************************
�� �� ��: GetChannelCFG
���ܸ�Ҫ: ȡ��ָ��ͨ����������Ϣ
�� �� ֵ: ͨ������.1��ʾģ����ͨ����0��ʾ������ͨ��
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ��ͨ������.������0��ʼ
		  param3 ͨ��������Ϣ
************************************/
int GetChannelCFG( XJHANDLE hComtrade, int nCn, ComtradeChannel& chl )
{
	if(hComtrade == NULL)
		return -1;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return -1;
	//ȡ�ö�Ӧͨ��
	CChannel* pChl = pComtrade->GetChannel(nCn);
	if(pChl == NULL)
		return -1;
	int nReturn = pChl->FillStructValue(chl);
	return nReturn;
}

/************************************
�� �� ��: GetChannelType
���ܸ�Ҫ: ȡ��ָ��ͨ��������
�� �� ֵ: ͨ������.1��ʾģ����ͨ����0��ʾ������ͨ��
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ��ͨ������.������0��ʼ
************************************/
int GetChannelType( XJHANDLE hComtrade, int nCn )
{
	if(hComtrade == NULL)
		return -1;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return -1;
	//ȡ�ö�Ӧͨ��
	CChannel* pChl = pComtrade->GetChannel(nCn);
	if(pChl == NULL)
		return -1;
	return pChl->m_bAnalog;
}

/************************************
�� �� ��: GetTriggerTime
���ܸ�Ҫ: ȡ�ù��ϴ���ʱ������ڿ�ʼ����ʱ�̵����ʱ��
�� �� ֵ: ���ϴ���ʱ������ڿ�ʼ����ʱ�̵����ʱ��
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 
************************************/
int GetTriggerTime(XJHANDLE hComtrade)
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	return pComtrade->m_nTriggerTime;
}

/************************************
�� �� ��: GetTimeRelativeTrigger
���ܸ�Ҫ: ȡ��ָ������������ڹ��ϴ���ʱ�̵�ʱ��
�� �� ֵ: ����ڹ��ϴ���ʱ�̵�ʱ�䣨���룩
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ������������.������0��ʼ
************************************/
int GetTimeRelativeTrigger(XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	return pComtrade->GetTimeByTrigger(nSampleNo);
}

/************************************
�� �� ��: GetTimeRelativeStart
���ܸ�Ҫ: ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ��
�� �� ֵ: ����ڲ�����ʼʱ�̵�ʱ�䣨���룩
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ������������.������0��ʼ
************************************/
int GetTimeRelativeStart( XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	return (pComtrade->GetTimeBySampling(nSampleNo) - pComtrade->GetTimeBySampling(0));
}

/************************************
�� �� ��: GetTotalSamples
���ܸ�Ҫ: ȡ�ò���������
�� �� ֵ: ����������
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 
************************************/
int GetTotalSamples( XJHANDLE hComtrade )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	return pComtrade->m_cSamples;
}

/************************************
�� �� ��: GetRealValue
���ܸ�Ҫ: ȡ��ָ��ͨ��ָ�����������ʵֵ
�� �� ֵ: ָ��ͨ��ָ�����������ʵֵ
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ��ͨ������.������0��ʼ
		  param3 ָ������������.������0��ʼ
		  param4 ָ����Ҫ��ֵ�����ͣ�Ĭ��Ϊ����ֵ.0Ϊ����ֵ��1Ϊһ��ֵ
************************************/
float GetRealValue( XJHANDLE hComtrade, int nCn, int nSampleNo, int nDataType/* = 0*/ )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0.0;
	return pComtrade->GetRealValue(nCn, nSampleNo, nDataType);
}

/************************************
�� �� ��: GetTotalTime
���ܸ�Ҫ: ȡ����ʱ��
�� �� ֵ: ��ʱ��
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 
************************************/
int GetTotalTime( XJHANDLE hComtrade )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	int nSize = pComtrade->m_arrSampleTime.size();
	if(nSize > 0)
		return (pComtrade->m_arrSampleTime[nSize-1] - pComtrade->m_arrSampleTime[0])/1000;
	return 0;
}

/************************************
�� �� ��: GetMaxValue
���ܸ�Ҫ: ȡ��ָ��ͨ�������ֵ
�� �� ֵ: ���ֵ
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ��ͨ������.������0��ʼ
************************************/
float GetMaxValue( XJHANDLE hComtrade, int nCn, int nGroup, int nDataType )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	//ȡ�ö�Ӧͨ��
	CChannel* pChl = pComtrade->GetChannel(nCn);
	if(pChl == NULL)
		return 0;

	if(nDataType == 0)//����ֵ
	{
		if(nGroup == 0)//ȡ�������ֵ
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
	else if(nDataType == 1)//һ��ֵ
	{
		if(nGroup == 0)//ȡ�������ֵ
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
		//ģ����ͨ��
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
�� �� ��: GetRecordValue
���ܸ�Ҫ: ȡ��ָ��ͨ��ָ��������ļ�¼ֵ
�� �� ֵ: ָ��ͨ��ָ��������ļ�¼ֵ
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 ָ��ͨ������.������0��ʼ
		  param3 ָ������������.������0��ʼ
************************************/
int GetRecordValue( XJHANDLE hComtrade, int nCn, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0.0;
	return pComtrade->GetRecordValue(nCn, nSampleNo);
}

/*************************************************************
�� �� ��: GetCTPT
���ܸ�Ҫ: ȡ��CTPT���
�� �� ֵ: 
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 CT���
		  param3 PT���
*************************************************************/
void GetCTPT( XJHANDLE hComtrade, float& fCT, float& fPT )
{
	if(hComtrade == NULL)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return;
	fCT = pComtrade->m_fCT;
	fPT = pComtrade->m_fPT;
}

/*************************************************************
�� �� ��: SetCTPT
���ܸ�Ҫ: ����CTPT���
�� �� ֵ: 
��    ��: param1 ָ�������ļ����ݶ���ָ��
		  param2 CT���
		  param3 PT���
*************************************************************/
void SetCTPT( XJHANDLE hComtrade, float fCT, float fPT )
{
	if(hComtrade == NULL)
		return;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return;
	pComtrade->m_fCT = fCT;
	pComtrade->m_fPT = fPT;
}

/****************************************************
Date:2012/5/15  Author:LYH
������:   GetTimeRelativeTriggerMSEC	
����ֵ:   double	����ڹ��ϴ���ʱ�̵�ʱ�䣨΢�룩
���ܸ�Ҫ: ȡ��ָ������������ڹ��ϴ���ʱ�̵�ʱ�䣬����΢��ֵ 
����: XJHANDLE hComtrade	ָ�������ļ����ݶ���ָ��
����: int nSampleNo	ָ������������.������0��ʼ
*****************************************************/
double GetTimeRelativeTriggerMSEC( XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	if(nSampleNo < 0 || nSampleNo > pComtrade->m_arrSampleTime.size())
		return -1;
	double dTime = pComtrade->m_arrSampleTime[nSampleNo];//΢��
	double dStartTime = pComtrade->m_arrSampleTime[0];
	return (dTime-(dStartTime+pComtrade->m_nTriggerTime*1000));
}

/****************************************************
Date:2012/5/15  Author:LYH
������:   GetTimeRelativeStartMSEC	
����ֵ:   double	����ڲ�����ʼʱ�̵�ʱ�䣨΢�룩
���ܸ�Ҫ: ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ�䣬����΢��ֵ
����: XJHANDLE hComtrade	ָ�������ļ����ݶ���ָ��
����: int nSampleNo	ָ������������.������0��ʼ
*****************************************************/
double GetTimeRelativeStartMSEC( XJHANDLE hComtrade, int nSampleNo )
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
		return 0;
	if(nSampleNo < 0 || nSampleNo > pComtrade->m_arrSampleTime.size())
		return -1;
	double dTime = pComtrade->m_arrSampleTime[nSampleNo];//΢��
	double dStartTime = pComtrade->m_arrSampleTime[0];
	return (dTime-dStartTime);
}

HdrObj* GetHdrInfoObject(XJHANDLE hComtrade)
{
	if(hComtrade == NULL)
		return 0;
	CCOMTRADEParser* pComtrade = (CCOMTRADEParser*)hComtrade;
	if(pComtrade == NULL)//��Ч��ָ��
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
	if(pComtrade == NULL)//��Ч��ָ��
		return "";
	if( pComtrade->m_nHdrParseResult < 0 )
		pComtrade->m_nHdrParseResult = pComtrade->ParseHdr();
	if( pComtrade->m_sHdrJson.empty() )
	{
		pComtrade->m_sHdrJson = pComtrade->HdrObjToJson();
	}
	return pComtrade->m_sHdrJson;
}
