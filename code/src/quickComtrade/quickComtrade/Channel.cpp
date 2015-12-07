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
函 数 名: FillStructValue
功能概要: 填充通道配置信息到通道结构体
返 回 值: 模拟量通道返回1，开关量通道返回0
参    数: param1 通道配置信息结构体
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
函 数 名: ChannelType
功能概要: 通道类型。0:电压通道。1:电流通道。2:其它模拟量通道。-1:开关量通道
返 回 值: 0:电压通道。1:电流通道。-1:开关量通道
参    数: param1 
		  param2 
*************************************************************/
int CChannel::ChannelType()
{
	if(!m_bAnalog) //开关量通道
		return -1;
	//模拟量通道根据单位判断
	m_strUnit.MakeUpper(); //大写
	int nFind = m_strUnit.Find('A', 0);
	if(nFind != -1)
	{
		//有找到A，电流通道
		return 1;
	}

	nFind = m_strUnit.Find('V', 0);
	if(nFind != -1)
	{
		//有找到V，电压通道
		return 0;
	}
	return 2;
}

