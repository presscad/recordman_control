// Channel.cpp: implementation of the CChannel class.
//
//////////////////////////////////////////////////////////////////////


#include "Channel.h"



//##ModelId=489955E203D8
CChannel::CChannel()
{
	m_nIndex			= -1;
	m_bDerived			= false;
	m_bAnalog			= true;
	m_dMaxValue			= 0;
	m_dMinValue			= 0;
	m_fOffset			= 0.0f;
	m_fPrimary			= 0.0f;
	m_fScale			= 0.0f;
	m_fSecondary		= 0.0f;
	m_fSkew				= 0.0f;
	m_nChannelNo		= 0;
	m_nStatus			= 0;
	m_strCcbm			= " ";
	m_strChannelName	= " ";
	m_strDType			= " ";
	m_strPhase			= " ";
	m_strUnit			= " ";
	m_fMaxValue			= 0.0f;

}

//##ModelId=489955E203D9
CChannel::~CChannel()
{
//	m_arrValue.RemoveAll();
}

/************************************
�� �� ��: FillStructValue
���ܸ�Ҫ: ���ͨ��������Ϣ��ͨ���ṹ��
�� �� ֵ: ģ����ͨ������1��������ͨ������0
��    ��: param1 ͨ��������Ϣ�ṹ��
		  param2 
************************************/
int CChannel::FillStructValue( ComtradeChannel& chl )
{
	chl.nChannelNo = m_nChannelNo;
	chl.sChannelName = m_strChannelName.c_str();
	chl.sPhase = m_strPhase.c_str();
	chl.sCcbm = m_strCcbm.c_str();
	chl.sUnit = m_strUnit.c_str();
	chl.fScale = m_fScale;
	chl.fOffset = m_fOffset;
	chl.fSkew = m_fSkew;
	chl.nMinValue = m_dMinValue;
	chl.nMaxValue = m_dMaxValue;
	chl.fPrimary = m_fPrimary;
	chl.fSecondary = m_fSecondary;
	chl.sPSType = m_strDType;
	chl.nStatus = m_nStatus;

	return m_bAnalog;
}

/*************************************************************
Date: 2010/09/19 	Author:LYH
�� �� ��: ChannelType
���ܸ�Ҫ: ͨ�����͡�0:��ѹͨ����1:����ͨ����2:����ģ����ͨ����-1:������ͨ��
�� �� ֵ: 0:��ѹͨ����1:����ͨ����-1:������ͨ��
��    ��: param1 
		  param2 
*************************************************************/
int CChannel::ChannelType()
{
	if(!m_bAnalog) //������ͨ��
		return -1;
	//ģ����ͨ�����ݵ�λ�ж�
	m_strUnit.MakeUpper(); //��д
	int nFind = m_strUnit.Find('A', 0);
	if(nFind != -1)
	{
		//���ҵ�A������ͨ��
		return 1;
	}

	nFind = m_strUnit.Find('V', 0);
	if(nFind != -1)
	{
		//���ҵ�V����ѹͨ��
		return 0;
	}
	return 2;
}

