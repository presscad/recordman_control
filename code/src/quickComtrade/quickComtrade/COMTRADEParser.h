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
 *  @brief   	GetFileName	 ȡ���ļ�����������׺ 
 *  @param 		[In]a param of Type  CXJString  ָ��Ҫ���ļ����ļ���
 *  @return 	void 
 */
	void GetFileName(CStrongString sFileName);
	
/*
 *  @brief   	LoadCfgData	 ��ȡCFG�ļ� 
 *  @param 		[Out]a param of Type  CXJString&  ������Ϣ
 *  @return 	int ��ȡʧ�ܷ���<0, �ɹ�����>=0
 */
	int LoadCfgData(CStrongString& strError);    
	
/*
 *  @brief   	LoadDataFromFile	 ��ȡDAT�ļ� 
 *  @param 		[Out]a param of Type  CXJString&  ������Ϣ
 *  @return 	int ��ȡʧ�ܷ���<0, �ɹ�����>=0
 */
	int LoadDataFromFile(CStrongString& strError); 

/*
 *  @brief   	CompatibleDiff	 ���ݶԹ淶�Ĳ�ͬ��� 
 *  @return 	void 
 */
	void CompatibleDiff();

/*
 *  @brief   	LoadFile	 ���벨���ļ� 
 *  @param 		[In]a param of Type  CXJString  �����ļ�·��
 *  @param 		[Out]a param of Type  CXJString&  ������Ϣ
 *  @return 	int ����ɹ�����>=0, ʧ�ܷ���<0
 */
	int LoadFile(CStrongString strFileName, CStrongString& strError);

/*
 *  @brief   	GetChannel	 ȡ��ָ�������ŵ�ͨ������ 
 *  @param 		[In]a param of Type  int  ָ��ͨ������
 *  @return 	CChannel* ͨ������ָ��
 */
	CChannel* GetChannel(int nChannelNO);

/*
 *  @brief   	GetRecordValue	 ȡ��ָ��ͨ��ָ���������ֵ 
 *  @param 		[In]a param of Type  int  ָ��ͨ������
 *  @param 		[In]a param of Type  int  ָ���������
 *  @return 	int ����ֵ.DAT�ļ����¼��ֵ
 */
	int GetRecordValue(int nChannelNo, int nSampleNo);

/*
 *  @brief   	GetRealValue	 ȡ��ָ��ͨ��ָ���������ֵ 
 *  @param 		[In]a param of Type  int  ָ��ͨ������
 *  @param 		[In]a param of Type  int  ָ���������
 *  @param		[In]a param of Type  int  ָ����Ҫ��ֵ�����ͣ�Ĭ��Ϊ����ֵ.0Ϊ����ֵ��1Ϊһ��ֵ
 *  @return 	int ��ʵֵ��ģ��ͨ������ax+b�������.
 */
	float GetRealValue(int nChannelNo, int nSampleNo, int nDataType = 0);

/*
 *  @brief   	TransformSecondaryValue	 ��ָ��ͨ����ָ��ֵת��Ϊ����ֵ 
 *  @param 		[In]a param of Type  CChannel*  ͨ��
 *  @param 		[In]a param of Type  float  Ҫת����ֵ
 *  @return 	float ����ֵ
 */
	float TransformSecondaryValue(CChannel* pChl, float fValue);

/*
 *  @brief   	TransformPrimaryValue	 ��ָ��ͨ����ָ��ֵת��Ϊһ��ֵ 
 *  @param 		[In]a param of Type  CChannel*  ͨ��
 *  @param 		[In]a param of Type  float  Ҫת����ֵ
 *  @return 	float һ��ֵ
 */
	float TransformPrimaryValue(CChannel* pChl, float fValue);

/*
 *  @brief   	GetTimeBySampling	 ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ��
 *  @param 		[In]a param of Type  int  ָ���������
 *  @return 	int ����ڲ�����ʼʱ�̵�ʱ��(��λ������)
 */
	int GetTimeBySampling(int nSampleNo);

/*
 *  @brief   	GetTimeByTrigger	 ȡ��ָ������������ڹ��Ͽ�ʼʱ�̵�ʱ��
 *  @param 		[In]a param of Type  int  ָ���������
 *  @return 	int ����ڹ��Ͽ�ʼʱ�̵�ʱ��(��λ������)
 */
	int GetTimeByTrigger(int nSampleNo);

/*
 *  @brief   	FreeData	 �������
 *  @return 	bool �ɹ�����true��ʧ�ܷ���false
 */
	bool FreeData();

/*
 *  @brief   	FillData	 ������ݵ�ͨ��
 *  @param 		[In]a param of Type  BOOL  �Ƿ�Ascii��ʽ
 *  @param 		[In]a param of Type  INT32*  ָ����������
 *  @param 		[In]a param of Type  int*  ���ڼ�¼������������
 *  @return 	void
 */
	void FillData(BOOL bAscii, INT32 *pData, int* pDigital);

/*
 *  @brief   	SetMaxValue	 ����Ĭ�����ֵ
 *  @return 	void
 */
	void SetMaxValue();

	int ParseHdr();

	string HdrObjToJson();

public:
	/** @brief           COMTRADE�ļ��汾��*/
	int m_nVersion;

	/** @brief           ����·�����ļ���,��������չ��*/
	CStrongString m_strName;

	/** @brief           ��վ��*/
	CStrongString m_strStationName;

	/** @brief           ����¼������*/
	CStrongString m_strDFRID;

	/** @brief           ����������*/
	int m_cSamples;

	/** @brief           ģ����ͨ������*/
	int m_cAChannels;

	/** @brief           ������ͨ������*/
	int m_cDChannels;

	/** @brief           ��·Ƶ��*/
	float m_fLineFreq;

	/** @brief           ����Ƶ����*/
	int m_nNRates;

	/** @brief           ʱ�����ϵ��*/
	float m_fTimeMult;

	/** @brief           ��ʼ����ʱ��*/
	CStrongString m_strDTFirst;

	/** @brief           ���ϴ����¼�*/
	CStrongString m_strDTTrigger;

	/** @brief           true: ASCII format ��false: Binary format*/
	bool m_bAscii;

	/** @brief           ��Ӧ���ֲ���Ƶ�ʵĲ���������*/
	vector<int>  m_arrSamples;

	/** @brief           ���ֲ���Ƶ�ʡ���λ��Hz*/
	vector<float> m_arrSampleRate;

 	/** @brief           ͨ����Ϣ*/
 	vector<CChannel*> m_arrChannel;

	/** @brief           ��¼���������Ӧ��ʱ�䣨����ڿ�ʼ����ʱ�䣩����λ��΢��*/
	vector<int> m_arrSampleTime;

	/** @brief           ���ڶ�ȡDAT�ļ�ʱ����*/
	int m_nCount;

	/** @brief           ���ϴ���ʱ������ڲ�����ʼʱ�̵�ʱ�䣨��λ�����룩*/
	int		m_nTriggerTime;

	/** @brief           CT���*/
	float	m_fCT;
	/** @brief           PT���*/
	float	m_fPT;

	/** @brief           �������ֵ*/
	float		m_fMaxIValue;
	/** @brief           ��ѹ���ֵ*/
	float		m_fMaxUValue;
	/** @brief           �������ֵ*/
	float		m_fMaxOther;

	/** @brief           ����һ��ֵ���ֵ*/
	float		m_fMaxIValueP;
	/** @brief           ��ѹһ��ֵ���ֵ*/
	float		m_fMaxUValueP;
	/** @brief           ����һ��ֵ���ֵ*/
	float		m_fMaxOtherP;

	/** @brief           ͷ�ļ���Ϣ*/
	HdrObj		m_Hdr;
	/** @brief           -1:δ������0:HDR�ļ������ڣ�1:���Ǳ�׼XML��ʽ��2:��׼XML��ʽ���Ѿ�����*/
	int			m_nHdrParseResult;
	string		m_sHdrJson;
private:
/*
 *  @brief   	CalTriggerTime	 ������ϴ���ʱ������ڲ�����ʼʱ�̵�ʱ�䣨��λ�����룩�����������m_nTriggerTime
 *  @return 	
 */
	void CalTriggerTime();

};

#endif // !defined(AFX_COMTRADEPARSER_H__2291058C_3BBF_4E61_A9A8_EDB98D42EFFF__INCLUDED_)
