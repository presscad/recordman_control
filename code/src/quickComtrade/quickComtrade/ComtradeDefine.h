#ifndef COMTRADEDEFINE_H_
#define COMTRADEDEFINE_H_

struct ComtradeCFG
{
	/** @brief           ��վ��*/
	string sStation;
	/** @brief           װ����*/
	string sDevice;
	/** @brief           �ļ���׼���*/
	int		nYear;
	/** @brief           ģ����ͨ������*/
	int		nAChannels;
	/** @brief           ������ͨ������*/
	int		nDChannels;
	/** @brief           ��·Ƶ��*/
	float	fLineFreq;
	/** @brief           ��ʼ����ʱ��*/
	string	sStartTime;
	/** @brief           ���ϴ����¼�*/
	string	sTriggerTime;
	/** @brief           �����ļ���ʽ��0-ascii, 1-binary*/
	int		nDATType;
	/** @brief           ʱ�걶��ϵ��*/
	float	fTimeMult;
	ComtradeCFG()
	{
		sStation = "";
		sDevice = "";
		nYear = 0;
		nAChannels = 0;
		nDChannels = 0;
		fLineFreq = 0.0;
		sStartTime = "";
		sTriggerTime = "";
		nDATType = 0;
		fTimeMult = 0.0;
	}
};

struct ComtradeChannel 
{
	/** @brief           ͨ��������*/
	int		nChannelNo;
	/** @brief           ͨ������*/
	string	sChannelName;
	/** @brief           ��λ��ʶ��*/
	string	sPhase;
	/** @brief           �����ӵĻ�·Ԫ��*/
	string	sCcbm;
	/** @brief           ��λ*/
	string	sUnit;
	/** @brief           ͨ������ϵ��a*/
	float	fScale;
	/** @brief           ͨ��ƫ����b*/
	float	fOffset;
	/** @brief           ����ʱ��*/
	float	fSkew;
	/** @brief           ��Сֵ��Χ*/
	int		nMinValue;
	/** @brief           ���ֵ��Χ*/
	int		nMaxValue;
	/** @brief           һ�α�������*/
	float	fPrimary;
	/** @brief           ���α�������*/
	float	fSecondary;
	/** @brief           һ�λ���α�ʶ��.P,p,S,s*/
	string	sPSType;
	/** @brief           ͨ������״̬.0/1*/
	int		nStatus;
	ComtradeChannel()
	{
		nChannelNo = 0;
		sChannelName = "";
		sPhase = "";
		sCcbm = "";
		sUnit = "";
		fScale = 0.0;
		fOffset = 0.0;
		fSkew = 0.0;
		nMinValue = 0;
		nMaxValue = 0;
		fPrimary = 0.0;
		fSecondary = 0.0;
		sPSType = "";
		nStatus = -1;
	}
};
#endif