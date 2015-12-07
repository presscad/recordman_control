// Channel.h: interface for the CChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_)
#define AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constant.h"

//通道配置结构，除说明项外，其余项和数据库中的字段一一对应
//##ModelId=489955E203BA
typedef struct{
	//故障录波器标识，这个标识可能为自己定义的（和COMTRADE文件中的不一样），如果自己
	//没有重新定义，这个值应该是COMTRADE文件中的故障录波器ID。
	CStrongString DFRID;          
	//属于某一DFR的一次设备（线路）标识
	CStrongString PID;            
	//通道相位，保留未用 
	CStrongString Phase;          
	//通道类型（V/A），保留未用
	CStrongString Unit;            
	//通道号，COMTRADE文件中Index
	int     ChannelNo;      
	//是否使用，保留未用
	int     IsUse;           
	//是否访问过。TRUE 访问过；FALSE 未访问过(库中此项不存在)
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
	//是否自定义通道
	//##ModelId=489955E203DB
	bool m_bDerived;

	//是否模拟量通道。
	//##ModelId=489955E203DC
	bool m_bAnalog;

	//通道号
	//##ModelId=489955E203DD
	int m_nChannelNo;

	//通道名称
	//##ModelId=489955E30000
	CStrongString m_strChannelName;

	//相比。如“A”、“B”、“C”
	//##ModelId=489955E30001
	CStrongString m_strPhase;

	//单位。允许取值为“A”、“KA”、“V”、“KV”
	//##ModelId=489955E30002
	CStrongString m_strUnit;

	//比例因子a
	//##ModelId=489955E3000F
	float m_fScale;

	//偏移量因子b
	//##ModelId=489955E30010
	float m_fOffset;

	//矫正因子c
	//##ModelId=489955E30011
	float m_fSkew;

	//本通道允许最大值
	//##ModelId=489955E30012
	int m_dMinValue;

	//本通道允许最大值
	//##ModelId=489955E3001F
	int m_dMaxValue;

	//模拟量通道类型，只能取p,P,s,S
	//##ModelId=489955E30020
	CStrongString m_strDType;

	//被监测系统的常开/常闭状态，仅对开关量通道有用
	//##ModelId=489955E30021
	int m_nStatus;

	//二次绕组的Scale
	//##ModelId=489955E30022
	float m_fSecondary;

	//一次绕组的Scale
	//##ModelId=489955E30023
	float m_fPrimary;

	//被监测的电流元件
	
	//##ModelId=489955E3002E
	CStrongString m_strCcbm;

	/** @brief           通道索引，系统内部使用*/
	int m_nIndex;

	/** @brief           最大值（取绝对值的最大值）*/
	float m_fMaxValue;

	/** @brief           一次最大值（取绝对值的最大值）*/
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
	该数组记录该通道各采样点对应的采样值。
	1、如果本通道为模拟量通道，则：
	数组下标表示采样点，数值为采样值（已经经过标度变换）。
	数组长度为m_cSamples；
	2、如果本通道为开关量通道，则：
	本数组中只记录变位点。格式为：
	采样点1，开关量1，采样点2，开关量2，...采样点n，开关量n
	以上表明：
	（1）采样点1为第一采样点，开关量为开关量1
	（2）到采样点2处发生变位，开关量为开关量2
	以此类推
	****************************************************/
//	 CArray <_int16, _int16> m_arrValue;
	//##ModelId=489955E30041
	vector<int> m_arrValue;

public:
	//填充结构体数据
	int FillStructValue(ComtradeChannel& chl);
/*
 *  @brief   	ChannelType	 通道类型。0:电压通道。1:电流通道。2:其它模拟量通道。-1:开关量通道 
 *  @return 	int 0:电压通道。1:电流通道。-1:开关量通道
 */
	int ChannelType();
};

#endif // !defined(AFX_CHANNEL_H__97DCF601_CDF4_4652_8E20_FFBF406A0A72__INCLUDED_)
