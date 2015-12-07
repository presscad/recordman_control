// Channel.h: interface for the CChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_)
#define AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constant.h"

//ͨ�����ýṹ����˵�����⣬����������ݿ��е��ֶ�һһ��Ӧ
//##ModelId=489955E203BA
typedef struct{
	//����¼������ʶ�������ʶ����Ϊ�Լ�����ģ���COMTRADE�ļ��еĲ�һ����������Լ�
	//û�����¶��壬���ֵӦ����COMTRADE�ļ��еĹ���¼����ID��
	CStrongString DFRID;          
	//����ĳһDFR��һ���豸����·����ʶ
	CStrongString PID;            
	//ͨ����λ������δ�� 
	CStrongString Phase;          
	//ͨ�����ͣ�V/A��������δ��
	CStrongString Unit;            
	//ͨ���ţ�COMTRADE�ļ���Index
	int     ChannelNo;      
	//�Ƿ�ʹ�ã�����δ��
	int     IsUse;           
	//�Ƿ���ʹ���TRUE ���ʹ���FALSE δ���ʹ�(���д������)
	bool    bAccess;        
} _ACHANNEL;
//##ModelId=489955E203C8
class CChannel
{

public:
	//##ModelId=489955E203D8
	CChannel();
	//##ModelId=489955E203D9
	virtual ~CChannel();
	
	public:
	//�Ƿ��Զ���ͨ��
	//##ModelId=489955E203DB
	bool m_bDerived;

	//�Ƿ�ģ����ͨ����
	//##ModelId=489955E203DC
	bool m_bAnalog;

	//ͨ����
	//##ModelId=489955E203DD
	int m_nChannelNo;

	//ͨ������
	//##ModelId=489955E30000
	CStrongString m_strChannelName;

	//��ȡ��硰A������B������C��
	//##ModelId=489955E30001
	CStrongString m_strPhase;

	//��λ������ȡֵΪ��A������KA������V������KV��
	//##ModelId=489955E30002
	CStrongString m_strUnit;

	//��������a
	//##ModelId=489955E3000F
	float m_fScale;

	//ƫ��������b
	//##ModelId=489955E30010
	float m_fOffset;

	//��������c
	//##ModelId=489955E30011
	float m_fSkew;

	//��ͨ���������ֵ
	//##ModelId=489955E30012
	int m_dMinValue;

	//��ͨ���������ֵ
	//##ModelId=489955E3001F
	int m_dMaxValue;

	//ģ����ͨ�����ͣ�ֻ��ȡp,P,s,S
	//##ModelId=489955E30020
	CStrongString m_strDType;

	//�����ϵͳ�ĳ���/����״̬�����Կ�����ͨ������
	//##ModelId=489955E30021
	int m_nStatus;

	//���������Scale
	//##ModelId=489955E30022
	float m_fSecondary;

	//һ�������Scale
	//##ModelId=489955E30023
	float m_fPrimary;

	//�����ĵ���Ԫ��
	
	//##ModelId=489955E3002E
	CStrongString m_strCcbm;

	/** @brief           ͨ��������ϵͳ�ڲ�ʹ��*/
	int m_nIndex;

	/** @brief           ���ֵ��ȡ����ֵ�����ֵ��*/
	float m_fMaxValue;

	/** @brief           һ�����ֵ��ȡ����ֵ�����ֵ��*/
	float m_fMaxValueP;
	//##ModelId=489955E3002F
	const CChannel& operator =(const CChannel& chl)
	{
		m_nIndex			=	chl.m_nIndex;
		m_fMaxValue			=	chl.m_fMaxValue;
		m_fMaxValueP		=	chl.m_fMaxValueP;
		m_bAnalog			=	chl.m_bAnalog;
		m_dMaxValue			=	chl.m_dMaxValue;
		m_dMinValue			=	chl.m_dMinValue;
		m_fOffset			=	chl.m_fOffset;
		m_fPrimary			=	chl.m_fPrimary;
		m_fScale			=	chl.m_fScale;
		m_fSecondary		=	chl.m_fSecondary;
		m_fSkew				=	chl.m_fSkew;
		m_nChannelNo		=	chl.m_nChannelNo;
		m_nStatus			=	chl.m_nStatus;
		m_strCcbm			=	chl.m_strCcbm;
		m_strChannelName	=	chl.m_strChannelName;
		m_strDType			=	chl.m_strDType;
		m_strPhase			=	chl.m_strPhase;
		m_strUnit			=	chl.m_strUnit;
		m_bDerived			=	chl.m_bDerived;
//		m_arrValue.Copy(chl.m_arrValue);
		for(int i=0;i<chl.m_arrValue.size();i++)
			m_arrValue.push_back(chl.m_arrValue[i]);
		return *this;
	}
	
	//##ModelId=489955E30031
	CChannel(const CChannel& chl)
	{
		m_nIndex			=	chl.m_nIndex;
		m_fMaxValue			=	chl.m_fMaxValue;
		m_fMaxValueP		=	chl.m_fMaxValueP;
		m_bAnalog			=	chl.m_bAnalog;
		m_dMaxValue			=	chl.m_dMaxValue;
		m_dMinValue			=	chl.m_dMinValue;
		m_fOffset			=	chl.m_fOffset;
		m_fPrimary			=	chl.m_fPrimary;
		m_fScale			=	chl.m_fScale;
		m_fSecondary		=	chl.m_fSecondary;
		m_fSkew				=	chl.m_fSkew;
		m_nChannelNo		=	chl.m_nChannelNo;
		m_nStatus			=	chl.m_nStatus;
		m_strCcbm			=	chl.m_strCcbm;
		m_strChannelName	=	chl.m_strChannelName;
		m_strDType			=	chl.m_strDType;
		m_strPhase			=	chl.m_strPhase;
		m_strUnit			=	chl.m_strUnit;
		m_bDerived			=	chl.m_bDerived;
		for(int i=0;i<chl.m_arrValue.size();i++)
			m_arrValue.push_back(chl.m_arrValue[i]);
	//	m_arrValue.Copy(chl.m_arrValue);
	}

	/**************************************************
	�������¼��ͨ�����������Ӧ�Ĳ���ֵ��
	1�������ͨ��Ϊģ����ͨ������
	�����±��ʾ�����㣬��ֵΪ����ֵ���Ѿ�������ȱ任����
	���鳤��Ϊm_cSamples��
	2�������ͨ��Ϊ������ͨ������
	��������ֻ��¼��λ�㡣��ʽΪ��
	������1��������1��������2��������2��...������n��������n
	���ϱ�����
	��1��������1Ϊ��һ�����㣬������Ϊ������1
	��2����������2��������λ��������Ϊ������2
	�Դ�����
	****************************************************/
//	 CArray <_int16, _int16> m_arrValue;
	//##ModelId=489955E30041
	vector<int> m_arrValue;

public:
	//���ṹ������
	int FillStructValue(ComtradeChannel& chl);
/*
 *  @brief   	ChannelType	 ͨ�����͡�0:��ѹͨ����1:����ͨ����2:����ģ����ͨ����-1:������ͨ�� 
 *  @return 	int 0:��ѹͨ����1:����ͨ����-1:������ͨ��
 */
	int ChannelType();
};

#endif // !defined(AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_)
