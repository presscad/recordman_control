// COMTRADEParser.cpp: implementation of the CCOMTRADEParser class.
//
//////////////////////////////////////////////////////////////////////

#include "COMTRADEParser.h"

#include "FileEx.h"

#include "../../../common_open_source/tinyxml2/tinyxml2.h"
using namespace tinyxml2;

#include "../../../common/GECodeConvert.h"
#include "../../../common_open_source/cJSON/cJSON.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//保存成新文件时所限制的大小 1.33M(1024 * 1024 * 1.33)
//目的为了一张软盘能容得下.
const int  FILESIZE     = 1394606;   

//在共享内存中,头256个字节存放文件名,接下来的sizeof(int)个字节存放有效数据长度,往后存放数据
const int  FILENAMESIZE = 256 ;

inline char* PreFmt(CStrongString& str)
{
	if(str.IsEmpty())
	{
		return NULL;
	}
	if(str[0] == ',')
		//str = " " + str;
	   str.Format(" %s",str.c_str());

	while(str.Find(",,") != -1) 
		str.Replace(",,", ", ,");

	return str.GetBuffer(str.GetLength());
}

inline void PostFmt(CStrongString& str)
{
	if(str.IsEmpty())
	{
		return;
	}
	str.TrimLeft(" ");
}

//##ModelId=489955E20197
CCOMTRADEParser::CCOMTRADEParser()
{
	m_cAChannels		=	0;
	m_cDChannels		=	0;
	m_strStationName	=	"";
	m_strDFRID			=	"";
	m_fLineFreq			=	50.0f;
	m_nNRates           =   0;
	m_cSamples          =   0;
	m_strName           =   "";
	m_strDTTrigger      =   "";
	m_strDTFirst        =   "";
	m_nVersion          =   0;
	m_fTimeMult         =   1.0f;
	m_bAscii            =   true;
	m_nTriggerTime		=	0;
	m_fCT				=  1.0f;
	m_fPT				=  1.0f;
	m_fMaxIValue		=	0.0;
	m_fMaxUValue		=	0.0;
	m_fMaxOther			=	0.0;
	m_fMaxIValueP		=	0.0;
	m_fMaxUValueP		=	0.0;
	m_fMaxOtherP		=	0.0;
	m_nHdrParseResult	=	-1;
	m_sHdrJson			=	"";
}

//##ModelId=489955E201A5
CCOMTRADEParser::~CCOMTRADEParser()
{
	FreeData();
}
//清除前次调用对下次调用的影响
//##ModelId=489955E20213
bool CCOMTRADEParser::FreeData()
{
	m_cAChannels		=	0;
	m_cDChannels		=	0;
	m_strStationName	=	"";
	m_strDFRID			=	"";
	m_fLineFreq			=	50.0f;
	m_nNRates           =   0;
	m_cSamples          =   0;
	m_strName           =   "";
	m_strDTTrigger      =   "";
	m_strDTFirst        =   "";
	m_nVersion          =   0;
	m_fTimeMult         =   1.0f;
	m_bAscii            =   true;
	m_nTriggerTime		=	0;
	m_fCT				=   0.0f;
	m_fPT				=   0.0f;
	for(int i = 0; i < m_arrChannel.size(); i++)
	{
		CChannel* pChl = (CChannel*)m_arrChannel[i];
		if(pChl != NULL)
			delete pChl;
	}
	m_arrChannel.clear();
	m_arrSampleRate.clear();
	m_arrSamples.clear();
	m_arrSampleTime.clear();
	return true;
}

/************************************
函 数 名: LoadCfgData
功能概要: 读取CFG文件
返 回 值: 读取失败返回<0, 成功返回>=0
参    数: param1 错误信息
		  param2 
************************************/
int CCOMTRADEParser::LoadCfgData(CStrongString& strError)
{
	CStrongString lpszPathName, strMsg;
	//temp variables 
	char* buff;
	char szStation[100], szDevice[100], szChannelName[100], szPhase[10], szCbm[100], szUU[100], szPS[2];	

	//没有给定文件名,不能执行装入数据操作
	if(m_strName.GetLength() < 1)
		return -1;
	printf("打开CFG文件:%s\n",m_strName.c_str());
	//CFG文件名
	lpszPathName.Format("%s.cfg",m_strName.c_str());
	CFileEx std_File;
    try
	{
		if(!std_File.Open(lpszPathName,CXJFile::modeRead))//增加强制关闭
		{
			lpszPathName.Format("%s.CFG", m_strName.c_str());
			if(!std_File.Open(lpszPathName,CXJFile::modeRead))
			{
				strError.Format("打开文件失败，文件路径为:%s", lpszPathName.c_str());
				return -1;
			}
		}
		printf("打开文件成功:%s\n",lpszPathName.c_str());
		CStrongString strTemp;
		//读文件 
		// station_name,rec_dev_id,rev_year <CR/LF>
		std_File.ReadString(strTemp);
		buff = PreFmt(strTemp);
		if(buff == NULL)
		{
			std_File.Close();
			strError = "读取厂站信息行失败";
			return -1;
		}
		sscanf(buff, "%[^,],%[^,],%d", szStation, szDevice, &m_nVersion);
		//厂站名
		m_strStationName = szStation;	
		PostFmt(m_strStationName);
		//录波器名
		m_strDFRID = szDevice; 			
		PostFmt(m_strDFRID);
		//华东abb的保护录波文件标准是2001的，按1999处理
        if(m_nVersion == 2001)
		{
			m_nVersion = 1999;
		}
		//COMTRADE文件的标准可能为1997 
		if(m_nVersion != 1999 && m_nVersion != 1997)
			m_nVersion = 1991;
		
		//通道数量
		std_File.ReadString(strTemp);
        if(strTemp.IsEmpty())
		{
			std_File.Close();
			strError = "通道数量信息行为空";
			return -2;
		}
		int cChannels = 0;
		strTemp.Replace("A", "");
		strTemp.Replace("D", "");
		//增加去空格
		strTemp.Replace(" ", "");
		buff = PreFmt(strTemp);
		if(buff != NULL)
		{
			//TT, ##A, ##D <CR/LF>
			sscanf(buff, "%d,%d,%d", &cChannels, &m_cAChannels, &m_cDChannels);
			if(m_cAChannels == 0 && m_cDChannels == 0 )
			{
				std_File.Close();
				strError = "此录波文件通道数为0";
				return -3;
			}
		}
		else
		{
			std_File.Close();
			strError = "读取通道数量信息行失败";
			return -4;
		}
		//设置通道数组大小
// 		m_arrChannel.reserve(cChannels);
// 		m_arrChannel.begin();
		
		int i = 0;
		int nIndexTemp = 0;
		//读取模拟量通道定义
		for(i = 0; i < m_cAChannels; i++) {
			std_File.ReadString(strTemp);
			
			buff = PreFmt(strTemp);
			if(buff == NULL)
			{
				std_File.Close();
				strError.Format("读取第%d个模拟量通道信息失败", i+1);
				return -5;
			}
			CChannel* chl = new CChannel;
			chl->m_bAnalog = true;
			chl->m_nIndex = nIndexTemp++;
			if(m_nVersion == 1999 || m_nVersion == 1997)//97,99格式
			{
				//An,ch_id,ph,ccbm,uu,a,b,skew,min,max,primary,secondary,PS <CR/LF>
				sscanf(buff, "%d,%[^,],%[^,],%[^,],%[^,],%f,%f,%f,%d,%d,%f,%f,%[^,]", 
					&chl->m_nChannelNo, 
					szChannelName, szPhase, szCbm, szUU, 
					&chl->m_fScale, &chl->m_fOffset, &chl->m_fSkew, 
					&chl->m_dMinValue, &chl->m_dMaxValue, 
					&chl->m_fPrimary, &chl->m_fSecondary, szPS);
				
				chl->m_strChannelName = szChannelName;	
				PostFmt(chl->m_strChannelName);
				chl->m_strPhase = szPhase;				
				PostFmt(chl->m_strPhase);
				chl->m_strCcbm = szCbm;					
				PostFmt(chl->m_strCcbm);
				chl->m_strUnit = szUU;					
				PostFmt(chl->m_strUnit);
				chl->m_strDType = szPS;					
				PostFmt(chl->m_strDType);
			} 
			else //91格式
			{
				//An,ch_id,ph,ccbm,uu,a,b,skew,min,max <CR/LF>
				sscanf(buff, "%d,%[^,],%[^,],%[^,],%[^,],%f,%f,%f,%d,%d", 
					&chl->m_nChannelNo, 
					szChannelName, szPhase, szCbm, szUU, 
					&chl->m_fScale, &chl->m_fOffset, &chl->m_fSkew, 
					&chl->m_dMinValue, &chl->m_dMaxValue);
				
				chl->m_fPrimary = 1.0f; 
				chl->m_fSecondary = 1.0; 
				chl->m_strDType = "P";
				chl->m_strChannelName = szChannelName;	
				PostFmt(chl->m_strChannelName);
				chl->m_strPhase = szPhase;				
				PostFmt(chl->m_strPhase);
				chl->m_strCcbm = szCbm;					
				PostFmt(chl->m_strCcbm);
				chl->m_strUnit = szUU;					
				PostFmt(chl->m_strUnit);
			}
			//Add the new channel
			m_arrChannel.push_back(chl);
		}
		int ntemp  = m_arrChannel.size();
		
		//读取开关量通道定义
		for(i = 0; i < m_cDChannels; i++)
		{
			std_File.ReadString(strTemp);
			
			buff = PreFmt(strTemp);
			if(buff == NULL)
			{
				std_File.Close();
				strError.Format("读取第%d个开关量通道信息失败", i+1);
				return -6;
			}
			CChannel* chl = new CChannel;
			chl->m_bAnalog = false;
			chl->m_nIndex = nIndexTemp++;
			if(m_nVersion == 1999 || m_nVersion == 1997) //97,99格式
			{
				//Dn,ch_id,ph,ccbm,y <CR/LF>
				sscanf(buff, "%d,%[^,],%[^,],%[^,],%d", &chl->m_nChannelNo, szChannelName, szPhase, szCbm, &chl->m_nStatus);
				
				chl->m_strChannelName = szChannelName;	
				PostFmt(chl->m_strChannelName);
				chl->m_strPhase = szPhase;				
				PostFmt(chl->m_strPhase);
				chl->m_strCcbm = szCbm;					
				PostFmt(chl->m_strCcbm);
			} 
			else //91格式
			{
				//Dn,ch_id,y <CR/LF>
				sscanf(buff, "%d,%[^,],%d", &chl->m_nChannelNo, szChannelName, &chl->m_nStatus);
				chl->m_strPhase = "" ; 
				chl->m_strCcbm = "" ;
				chl->m_strChannelName = szChannelName;	
				PostFmt(chl->m_strChannelName);
			}
			//Add the new channel
			m_arrChannel.push_back(chl);
		}
		ntemp  = m_arrChannel.size();

		//线路频率
		std_File.ReadString(strTemp);
		
		buff = PreFmt(strTemp);
		if(buff == NULL)
		{
			std_File.Close();
			this->FreeData();
			strError = "读取线路频率信息行失败";
			return -7;
		}
		sscanf(buff, "%f", &m_fLineFreq);
		if(m_fLineFreq == 0) //检查系统频率
		{
			std_File.Close();
			this->FreeData();
			strError.Format("线路频率非法,值为:%d", m_fLineFreq);
			return -8;
		}
		
		//采样频率个数信息
		std_File.ReadString(strTemp);
		
		buff = PreFmt(strTemp);
		if(buff == NULL)
		{
			std_File.Close();
			this->FreeData();
			strError = "读取采样频率数信息行失败";
			return -9;
		}
		sscanf(buff, "%d", &m_nNRates);
		if(m_nNRates < 0)   //检查频率个数 add by yinzhehong 20050512
		{
			std_File.Close();
			this->FreeData();
			strError.Format("采样频率非法,值为:%d", m_nNRates);
			return -10;
		}
		else if(m_nNRates == 0)//当nrates和samp填为0时，endsamp字段和数据文件DAT中的timestamp则必须填
		{
			if(m_nVersion == 1999 || m_nVersion == 1997)
			{
				m_nNRates = 1;
// 				m_arrSampleRate.reserve(m_nNRates);
// 				m_arrSampleRate.begin();
// 				m_arrSamples.reserve(m_nNRates);
// 				m_arrSamples.begin();
				
				std_File.ReadString(strTemp);
				float fSamp;
				int nEndSamp;
				
				strTemp.Replace(" ", "");//增加去空格
				
				buff = PreFmt(strTemp);
				//samp,endsamp <CR/LF>
				if(buff) //增加空判断 add by yinzhehong 20060529
				{
					sscanf(buff, "%f,%d", &fSamp, &nEndSamp);
				}
				else
				{
					fSamp=0;
					nEndSamp=0;
				}		
				m_arrSampleRate.push_back(fSamp);
				m_arrSamples.push_back(nEndSamp);
			}
			else
			{
				std_File.Close();
				this->FreeData();
				return FALSE;
			}
		}
		else
		{
// 			m_arrSampleRate.reserve(m_nNRates);
// 			m_arrSampleRate.begin();
// 			m_arrSamples.reserve(m_nNRates);
// 			m_arrSamples.begin();

			for(i=0; i < m_nNRates; i++)
			{
				std_File.ReadString(strTemp);
				float fSamp;
				int nEndSamp;
				
				strTemp.Replace(" ", "");//增加去空格
				
				buff = PreFmt(strTemp);
				//samp,endsamp <CR/LF>
				if(buff) //增加空判断 add by yinzhehong 20060529
				{
					sscanf(buff, "%f,%d", &fSamp, &nEndSamp);
				}
				else
				{
					fSamp=0;
					nEndSamp=0;
				}
				m_arrSampleRate.push_back(fSamp);
				m_arrSamples.push_back(nEndSamp);
			}
		}
		
		m_cSamples=0;
		for(i = 0; i < m_arrSamples.size(); i++)
		{
			m_cSamples += m_arrSamples[i];
		}
		// 检查数据点数
        if(m_cSamples <= 0)  //add by yinzhehong 20050512
		{
			std_File.Close();
			this->FreeData();
			strError.Format("采样点总数非法,值为:%d", m_cSamples);
			return -11;
		}
		
		//由于模拟通道采样数量是固定的,在此设置采样点数组大小,以提高执行效率,
		//而数字量通道采样值是采用压缩存储方式(只存储变位通道的采样点索引值和采样点值),大小是不固定的.
// 		for(i = 0; i <m_cAChannels; i++)
// 		{    
// 			CChannel* chl = GetChannel(i);
// 			chl->m_arrValue.reserve(m_cSamples);
// 		}
		
		//m_arrSampleTime.reserve(m_cSamples);
		//m_arrSampleTime.begin();
		
		//m_strDTFirst 采样时间
		std_File.ReadString(m_strDTFirst);
		
		//m_strDTTrigger 触发时间(故障时间)
		std_File.ReadString(m_strDTTrigger);

		//计算触发时间相对于采样开始时间的值
		CalTriggerTime();
		if(m_nTriggerTime < 0)
		{
			std_File.Close();
			this->FreeData();
			strError.Format("触发时间先于采样开始时间");
			return -12;
		}
		
		//Data file type  
		std_File.ReadString(strTemp);
		
		if(strTemp[0] == 'A' || strTemp[0] == 'a') 
			m_bAscii=true;
		else 
			m_bAscii=false;
		
		//Time stamp multiplication factor
		if(m_nVersion == 1999 || m_nVersion == 1997) 
		{
			std_File.ReadString(strTemp);
			
			buff = PreFmt(strTemp);
			sscanf(buff, "%f", &m_fTimeMult);
		} 
		else 
		{
			m_fTimeMult = 1.0f;
		}
		
		std_File.Close();
 }

 catch(char *str)
 {
	 this->FreeData();
	 strError.Format("%s",str);
	 return -12;
 }
 return 0;
}


/*************************************************************
函 数 GetChannel()
功能概要：获取通道数组中的由nChannelNO指定的通道
返 回 值: 对应的通道
参    数：通道号
**************************************************************/
//##ModelId=489955E201D7
CChannel* CCOMTRADEParser::GetChannel(int nChannelNO)
{
	//防止数组越界,小于最小值取最小值,大于最大值取最大值
	if(nChannelNO < 0)
		return NULL;
	if(nChannelNO >= m_arrChannel.size())
		return NULL;
	CChannel* pChl = (CChannel*)m_arrChannel[nChannelNO];
	if(pChl == NULL)
		return NULL;
	if(pChl->m_nIndex != nChannelNO)//检查是否匹配
		return NULL;

	return pChl;
}

/************************************
函 数 名: LoadDataFromFile
功能概要: 读取DAT文件
返 回 值: 读取失败返回<0, 成功返回>=0
参    数: param1 错误信息
		  param2
************************************/
int CCOMTRADEParser::LoadDataFromFile(CStrongString& strError)
{
	//没有给定文件名,不能执行装入数据操作
	if(m_strName.GetLength() < 1)
		return -1;

	CStrongString lpszPathName, strMsg;

	int register i;

	//形成数据文件数组
	vector<CStrongString> arrDataFile;
	FILE* fp = NULL;
	//取得数据文件名
	bool bOpen = true;;
	lpszPathName.Format("%s.dat",m_strName.c_str());
	if( ( fp = fopen(lpszPathName, "r") ) == NULL)
	{
		for(i = 0; i < 100; i++)
		{
			strMsg.Format(".d%02d", i);
			lpszPathName = m_strName + strMsg;
			if((fp = fopen(lpszPathName, "r")) != NULL )
			{
				fclose(fp);
				arrDataFile.push_back(lpszPathName);
			}
			else
			{
				if(0 == i)
				{
					bOpen = false;
					break;
				}
			}
		}
	}
	else
	{
		fclose(fp);
		arrDataFile.push_back(lpszPathName);
	}

	if(!bOpen)
	{
		//使用大写
		lpszPathName.Format("%s.DAT",m_strName.c_str());
		if( ( fp = fopen(lpszPathName, "r") ) == NULL)
		{
			for(i = 0; i < 100; i++)
			{
				strMsg.Format(".D%02d", i);
				lpszPathName = m_strName + strMsg;
				if((fp = fopen(lpszPathName, "r")) != NULL )
				{
					fclose(fp);
					arrDataFile.push_back(lpszPathName);
				}
			}
		}
		else
		{
			fclose(fp);
			arrDataFile.push_back(lpszPathName);
		}
	}

	printf("打开文件成功:%s\n",lpszPathName.c_str());

	if(arrDataFile.size() == 0)
	{
		strError.Format("不能打开文件%s,找不到对应文件!",lpszPathName.c_str());
		printf("不能打开文件%s,找不到对应文件!",lpszPathName.c_str());
		return -2;
	}

	//计算每次采样数据字节数
	//对于数字通道,每16个为一组,存储在一个_int16型的变量中,每个数字通道对应一位,其存储方式为:
	//在_int16型变量中从0到第15位存放数字通道的索引号依次增大.
	int nLineLength, nDLength;
	if(m_bAscii == true)
	{
		//包括分割符","
		//说明:标准99格式nLineLength=11+11+m_cAChannels*7 + m_cDChannels * 2 ;在测试过程中发现
		//有的文件的模拟量采样数据占了7个字符的宽度(标准99格式应为6个字符宽度,不包括","),状态量
		//采样数据也是占了7个字符的宽度,所以采用下面的 *10 格式.
		nLineLength = 11 + 11 + m_cAChannels * 10 + m_cDChannels * 10 ;
	} 
	else 
	{
		nDLength =(m_cDChannels / 16) * 2;

		if(m_cDChannels % 16 != 0 && m_cDChannels != 0) 
			nDLength += 2;

		nLineLength = 4 + 4 + m_cAChannels * 2 + nDLength;
	}
	//用于读取一行（一次）采样数据（原始ASCII 或者 BINRAY）
	char* pBuffer	= new char[nLineLength];
	//用于暂时存储转换后的一行（一次）采样数据
	INT32 *pData	= new INT32[m_cAChannels+m_cDChannels+2];
	//用于暂时存储上一次采样中的开关量（实现压缩存储）
	int* pDigital = new int[m_cDChannels];
	char *head, *tail; 

	//产生arrMask数组,用来获取对应位的数字通道的值
	// arrMask = {
	//				0000000000000001
	//				0000000000000010
	//				...
	//				1000000000000000
	//			 }
	INT16 arrMask[16]={0};
	INT16 mask = 0x0001;
	if(!m_bAscii)//文本文件,此循环执行无意义
	{
		for( i = 0; i < 16; i++ ) 
			arrMask[i] = mask << i ;
	}
	INT16 n16Temp=0;

	try
	{
		m_nCount = 0;
		//逐个打开数据文件，读取数据
		for(int j = 0; j < arrDataFile.size(); j++) 
		{
			//		lpszPathName = arrDataFile.GetAt(j);
			lpszPathName = arrDataFile[j];
			if( ( fp = fopen(lpszPathName, m_bAscii ? "r" : "rb") ) == NULL ) {			
				strError.Format("不能打开文件%s!",lpszPathName.c_str());
				return -3;
			}
			//逐行（逐个采样）读取数据
			//n, timestamp, A1, A2, ... Ak, D1, D2, ...Dm		
			while(!feof(fp)) 
			{
				if(m_nCount == 254)
				{
					int ds = 0;
				}
				m_nCount++;
				if(m_nCount > (int)m_cSamples)
				{
					//超出采样点总数的数据无效
					m_nCount--;
					break;
				}
				
				memset(pData, 0, m_cAChannels+m_cDChannels+2);
				if(m_bAscii == true) 
				{ //文本文件
					//读取一行采样数据
					memset(pBuffer, 0, nLineLength);
					fgets(pBuffer, nLineLength, fp);

					if(strlen(pBuffer) == 0)
					{
						m_nCount--;
						break;
					}

					head=tail=pBuffer; 
					i=0;
					//分析本行数据，将各数据项暂存入pData数组
					while(*tail && i < m_cAChannels + m_cDChannels + 2) 
					{ 
						while(*tail && *tail != ',') tail++; 
						*tail = '\0'; 
						pData[i] = atoi(head); 
						i++;
						head = ++tail;
					}
					//按照COMTRADE文件格式,读取文件后pData[0]存放的应是采样点序号(第几个采样点),
					//如果pData[0]<1 或者 pData[0]>m_cSamples(采样点总数,从配置文件中读入的),则说明本行数据无效应跳过,
					//或者有效数据已读完,本次读取的值不被处理.
// 					if(pData[0] < 1 || pData[0] > m_cSamples)
// 					{
// 						m_nCount--;
// 						continue;
// 					}
					pData[0] = m_nCount;
				} 
				else 
				{ //二进制文件
					//读取一次采样数据
					memset(pBuffer,0,nLineLength);
					if(nLineLength != fread(pBuffer, 1, nLineLength, fp))
					{
						m_nCount--;
						break;
					}

// 					if(strlen(pBuffer) == 0)
// 					{
// 						m_nCount--;
// 						break;
// 					}

					//分析本次数据，将各数据项暂存入data数组
					//pBuffer0~3位为采样点数据
					memcpy(&pData[0], pBuffer, 4);
					//按照COMTRADE文件格式,读取文件后pData[0]存放的应是采样点序号(第几个采样点),
					//由于二进制文件是依流的形式存放的,如果pData[0]<1 或者 pData[0]>m_cSamples(采样点总数,从配置文件中读入的),
					//说明有效数据已读完,本次读取的值不被处理.
// 					if(pData[0] < 1 || pData[0] > m_cSamples)
// 					{
// 						m_nCount--;
// 						continue;
// 					}
					pData[0] = m_nCount;

					//pBuffer4~7位为采样时间数据
					memcpy(&pData[1], pBuffer + 4, 4);

					//pBuffer8~为模拟量数据（_int16），由于pData为整型数组，故拷贝到低字中，高字为0
					for(i = 0; i < m_cAChannels; i++) 
						memcpy(&pData[2 + i] , pBuffer + 8 + i * 2, 2); 

					//最后为开关量。16路开关量占用一个_int16
					for(i = 0; i < m_cDChannels; i++) {
						memcpy(&n16Temp,pBuffer+8 + m_cAChannels * 2 + (i/16)*2,2);
						if(n16Temp & arrMask[i%16] ) {
							pData[2 + m_cAChannels + i ] = 1;
						} else {
							pData[2 + m_cAChannels + i ] = 0;
						}
					}
				}//end 二进制文件

				//现在，本次采样数据已经存储到pData数组中。下面将数据从pData数组复制到对应通道中
				//pData[0] 为采样点，pData[1]为采样时间
				//m_arrSampleTime.assign(pData[0]-1, (UINT)pData[1]);
				m_arrSampleTime.push_back((int)pData[1]*m_fTimeMult);
				//填充数据
				FillData(m_bAscii, pData, pDigital);
			}	
			fclose(fp);
		}
	}
	catch(...)
	{
		strError = "发生未知错误";
		return -4;
	}

	delete[] pData;
	delete[] pBuffer;
	delete[] pDigital;

	//设置默认最大值
	SetMaxValue();

	return 0;
}

/************************************
函 数 名: GetFileName
功能概要: 取得文件名，不带后缀
返 回 值: void
参    数: param1 指定要打开文件的文件名
************************************/
void CCOMTRADEParser::GetFileName( CStrongString sFileName )
{
	//说明:由于程序需要处理配置文件和数据文件,而m_strName是包含路径和扩展名的完整文件名,为了下面处理方便,
	//所以在此去掉扩展名,处理配置文件时使 lpszPathName=m_strName+".cfg";
	//处理数据文件时使 lpszPathName=m_strName+".dat" 或 + ".d00" ...;
	//寻找文件名
	m_strName = sFileName;
	/*CFileEx file;
	try
	{
		if(!file.Open(m_strName,CXJFile::modeRead))
			return;//如果是一个不能打开的文件，不对文件名操作
		file.Close();
	}
	catch(char *str)
	{
		return;
	}*/
	
	m_strName.Replace("\\", "/");
	m_strName.Replace("//", "\\\\");
	int i = m_strName.ReverseFind('/');
	CStrongString strFileName;
	if(i > 0)
	{
		strFileName = m_strName.Right(m_strName.GetLength() - i -1);
		int j  = strFileName.ReverseFind('.');
		if(j > 0)
		{
			strFileName = strFileName.Left(j);
		}
		m_strName   = m_strName.Left(i+1);
		m_strName   += strFileName;
	}
	else//相对路径下可能没有
	{
		int j = m_strName.ReverseFind('.');
		if(j > 0)
		{
			m_strName = m_strName.Left(j);
		}
	}
}

/************************************
函 数 名: CompatibleDiff
功能概要: 兼容对规范的不同理解
返 回 值:  
参    数: param1
		  param2
************************************/
void CCOMTRADEParser::CompatibleDiff()
{
	//对于CFG文件中的采样频率下的采样个数计数存在不同的理解
	//方式0: 每个采样频率后面跟着的采样个数为此频率下的采样个数
	//方式1: 每个采样频率后面跟着的采样个数为此频率下采样点的结束点的序号
	//因为对于规范的理解不同,出现在总采样点的计算上有些不同.以下为兼容两种情况
	
	if((m_cSamples - m_nCount) >= m_arrSamples[0] /2)
	{
		for(int i = m_arrSamples.size()-1; i > 0; i--)
		{
			int temp_i = m_arrSamples[i];
			int temp_i_1 = m_arrSamples[i-1];
			
			temp_i -= temp_i_1;
			
			m_arrSamples[i] = temp_i;
		}
	}
	m_cSamples = m_nCount;
	//m_arrSampleTime.reserve(m_cSamples);
	m_arrSampleTime.resize(m_cSamples);

	bool bCal = false;
	if(m_arrSampleTime.size() > 2)
	{
		if(m_arrSampleTime[0] == m_arrSampleTime[1])
			bCal = true;
	}

	//对于有指定采样频率的情况，按频率计算时间，不使用DAT文件里的时间
	if(m_nNRates > 0 && bCal)
	{
		for(int i = 0; i < m_cSamples; i++)
		{
			if(i == 0)
			{
				//第一个时间一定为0
				m_arrSampleTime[0] = 0;
			}
			else
			{
				//首先确定在第几个频率段
				int nTotal = 0;	
				int nPos = 0;
				for(int k = 0; k < m_arrSamples.size(); k++)
				{
					nTotal += m_arrSamples[k];
					if(i < nTotal)
					{
						nPos = k;
						break;
					}
				}
				//得到此频率下的周期
				int nRate = (int)m_arrSampleRate[nPos]; //频率
				int nCyc = 1000000/nRate; //周期
				//时间为前一个时间+周期
				int nTime = m_arrSampleTime[i-1] + nCyc;
				m_arrSampleTime[i] = nTime;
			}
		}
	}
}

/************************************
函 数 名: LoadFile
功能概要: 载入波形文件
返 回 值: 载入成功返回>=0, 失败返回<0
参    数: param1 波形文件路径
		  param2 错误信息
************************************/
int CCOMTRADEParser::LoadFile( CStrongString strFileName, CStrongString& strError )
{
	//截取文件名
	GetFileName(strFileName);
	//读取配置文件
	int nReturn = LoadCfgData(strError);
	if(nReturn < 0)
		return nReturn;
	//读取数据文件
	nReturn = LoadDataFromFile(strError);
	if(nReturn < 0)
		return nReturn;
	//兼容处理
	CompatibleDiff();
	return 0;
}

/************************************
函 数 名: GetRealValue
功能概要: 取得指定通道指定采样点的值
返 回 值: 真实值，模拟通道经过ax+b计算得来
参    数: param1 指定通道索引
param2 指定采样点号
param3 指定需要的值的类型，默认为二次值.0为二次值，1为一次值
************************************/
float CCOMTRADEParser::GetRealValue(int nChannelNo, int nSampleNo, int nDataType/* = 0*/)
{
	//指定的通道号或者指定的采样点不存在,总是返回0
	if(nSampleNo >= m_cSamples || nSampleNo < 0)
		return 0.0f;
	CChannel* pchl = GetChannel(nChannelNo);
	if(pchl != NULL)
	{
		if(pchl->m_bAnalog)
		{
			//模拟量通道
			float fValue = (float)pchl->m_fScale * GetRecordValue(nChannelNo, nSampleNo) + pchl->m_fOffset;
			if(nDataType == 1)//一次值
			{
				return TransformPrimaryValue(pchl, fValue);
			}
			else//二次值
			{
				return TransformSecondaryValue(pchl, fValue);
			}
		}
		else
		{
			//开关量通道
			return GetRecordValue(nChannelNo, nSampleNo);
		}
	}
	return 0.0f;
}

/************************************
函 数 名: GetRecordValue
功能概要: 取得指定通道指定采样点的值
返 回 值: 采样值.DAT文件里记录的值
参    数: param1 指定通道索引
		  param2 指定采样点号 
************************************/
int CCOMTRADEParser::GetRecordValue( int nChannelNo, int nSampleNo )
{
	//指定的通道号或者指定的采样点不存在,总是返回0
	if(nSampleNo >= m_cSamples || nSampleNo < 0)
		return 0.0f;
	CChannel* pchl = GetChannel(nChannelNo);
	if(pchl != NULL)
	{
		if(pchl->m_bAnalog)//模拟量通道
		{
			return pchl->m_arrValue[nSampleNo];
		}
		else//开关量通道
		{
			for(int i = 0; i < pchl->m_arrValue.size(); i += 2)
			{
				if(nSampleNo < (pchl->m_arrValue[i]-1))
				{
					return pchl->m_arrValue[i-1];
				}
				else if(nSampleNo == (pchl->m_arrValue[i]-1))
					return pchl->m_arrValue[i+1];
				else
				{
					//最后一个变位
					if(i == pchl->m_arrValue.size() -2)
						return pchl->m_arrValue[i+1];
				}
			}
		}
	}
	return 0;
}

/************************************
函 数 名: GetTimeBySampling
功能概要: 取得指定采样点相对于采样开始时刻的时间
返 回 值: 相对于采样开始时刻的时间(单位：毫秒)
参    数: param1 指定采样点号
		  param2 
************************************/
int CCOMTRADEParser::GetTimeBySampling( int nSampleNo )
{
	if(nSampleNo < 0 || nSampleNo > m_arrSampleTime.size())
		return -1;
	int nTime = m_arrSampleTime[nSampleNo];//微秒
	nTime /= 1000;
	return nTime;
}

/************************************
函 数 名: GetTimeByTrigger
功能概要: 取得指定采样点相对于故障开始时刻的时间
返 回 值: 相对于故障开始时刻的时间(单位：毫秒)
参    数: param1 指定采样点号
		  param2 
************************************/
int CCOMTRADEParser::GetTimeByTrigger( int nSampleNo )
{
	int nTime = GetTimeBySampling(nSampleNo);
	int nStartTime = GetTimeBySampling(0);
	return (nTime - (nStartTime+m_nTriggerTime));
}

/************************************
函 数 名: CalTriggerTime
功能概要: 计算故障触发时刻相对于采样开始时刻的时间（单位：毫秒），结果保存在m_nTriggerTime
返 回 值: 
参    数: param1 
		  param2 
************************************/
void CCOMTRADEParser::CalTriggerTime()
{
	//日期不处理
	if(m_strDTFirst == "")
	{
		m_nTriggerTime = 0;
		return;
	}
	if(m_strDTTrigger == "")
	{
		m_nTriggerTime = 0;
		return;
	}
	//值计算秒级和毫秒级
	//查找秒数
	//采样开始时间
	CStrongString strFirst = m_strDTFirst;
	int nFind = strFirst.Find(',', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	strFirst.Delete(0, nFind+1);
	//HH
	nFind = strFirst.Find(':', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	CStrongString sTemp = strFirst.Left(nFind);
	int nFirstHH = atoi(sTemp.c_str());
	strFirst.Delete(0, nFind+1);
	//MM
	nFind = strFirst.Find(':', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	sTemp = strFirst.Left(nFind);
	int nFirstMM = atoi(sTemp.c_str());
	strFirst.Delete(0, nFind+1);
	//SS
	nFind = strFirst.Find('.', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	sTemp = strFirst.Left(nFind);
	int nFirstSS = atoi(sTemp.c_str());
	strFirst.Delete(0, nFind+1);
	//MS
	if(strFirst.length() > 3)
	{
		strFirst = strFirst.Left(3);
	}
	int nFirstMS = atoi(strFirst.c_str());
	//故障触发时间
	strFirst = m_strDTTrigger;
	nFind = strFirst.Find(',', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	strFirst.Delete(0, nFind+1);
	//HH
	nFind = strFirst.Find(':', 0);
	//HH
	nFind = strFirst.Find(':', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	sTemp = strFirst.Left(nFind);
	int nTriggerHH = atoi(sTemp.c_str());
	strFirst.Delete(0, nFind+1);
	//MM
	nFind = strFirst.Find(':', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	sTemp = strFirst.Left(nFind);
	int nTriggerMM = atoi(sTemp.c_str());
	strFirst.Delete(0, nFind+1);
	//SS
	nFind = strFirst.Find('.', 0);
	if(nFind < 0)
	{
		m_nTriggerTime = 0;
		return;
	}
	sTemp = strFirst.Left(nFind);
	int nTriggerSS = atoi(sTemp.c_str());
	strFirst.Delete(0, nFind+1);
	//MS
	if(strFirst.length() > 3)
	{
		strFirst = strFirst.Left(3);
	}
	int nTriggerMS = atoi(strFirst.c_str());

	//计算差值
	m_nTriggerTime = ((((nTriggerHH-nFirstHH)*60 + nTriggerMM - nFirstMM)*60 + nTriggerSS - nFirstSS)*1000+nTriggerMS-nFirstMS);

	if(m_nTriggerTime < 0)
		m_nTriggerTime = 0;
}

/*************************************************************
Date: 2010/09/19 	Author:LYH
 函 数 名: TransformSecondaryValue()
 功能概要: 将指定通道的指定值转化为二次值
 返 回 值: 二次值
 参    数: param1 通道
		   Param2 要转化的值
**************************************************************/
float CCOMTRADEParser::TransformSecondaryValue( CChannel* pChl, float fValue )
{
	//检查数据有效性
	if(pChl == NULL)
		return fValue;
	if(!pChl->m_bAnalog)
		return fValue;
	if(m_nVersion == 1991)
	{
		//91版文件，认为ax+b得出的值都是二次值
		return fValue;
	}
	else //97或99版本
	{
		//判断自身的一次变比和二次变比是否有效
		if((pChl->m_fPrimary < 0.00000001) || (pChl->m_fSecondary < 0.00000001))
		{
			//自身变比无效
			return fValue;
		}
		else
		{
			//自身变比有效
			if(pChl->m_strDType == "p" || pChl->m_strDType == "P") //通道记录的是一次值
				return (fValue/pChl->m_fPrimary)*pChl->m_fSecondary;
			else
				return fValue;
		}
	}
	return fValue;
}

/*************************************************************
Date: 2010/09/19 	Author:LYH
函 数 名: TransformPrimaryValue
功能概要: 将值转化为一次值
返 回 值: 一次值
参    数: param1 通道
		  param2 要转化的值
*************************************************************/
float CCOMTRADEParser::TransformPrimaryValue( CChannel* pChl, float fValue )
{
	//检查数据有效性
	if(pChl == NULL)
		return fValue;
	if(!pChl->m_bAnalog)
		return fValue;
	if(m_nVersion == 1991)
	{
		//91版文件，认为ax+b得出的值都是二次值,根据CT，PT将之转化为一次值
		if(pChl->ChannelType() == 0) //电压通道
		{
			//乘以PT变比
			return fValue*m_fPT;
		}
		else if(pChl->ChannelType() == 1) //电流通道
		{
			//乘以CT变比
			return fValue*m_fCT;
		}
		else
			return fValue;
	}
	else //97或99版本
	{
		//判断自身的一次变比和二次变比是否有效
		if((pChl->m_fPrimary < 0.00000001) || (pChl->m_fSecondary < 0.00000001))
		{
			//自身变比无效
			if(pChl->ChannelType() == 0) //电压通道
			{
				//乘以PT变比
				return fValue*m_fPT;
			}
			else if(pChl->ChannelType() == 1) //电流通道
			{
				//乘以CT变比
				return fValue*m_fCT;
			}
			else
				return fValue;
		}
		else
		{
			//自身变比有效
			if(pChl->m_strDType == "s" || pChl->m_strDType == "S") //通道记录的是二次值
				return (fValue*pChl->m_fPrimary)/pChl->m_fSecondary;
			else
				return fValue;
		}
	}
	return fValue;
}

/*************************************************************
Date: 2011/05/04 	Author:LYH
函 数 名: FillData
功能概要: 填充数据到通道
返 回 值: 
参    数: param1 是否Ascii格式
		  param2 指定数据数组
		  param3 用于记录开关量的数组
*************************************************************/
void CCOMTRADEParser::FillData( BOOL bAscii, INT32 *pData, int* pDigital )
{
	if(pDigital == NULL)
		return;
	if(pData == NULL)
		return;

	int i = 0;

	if(m_bAscii == TRUE)
	{
		for (i = 0; i < m_cAChannels; i++) 
		{
			CChannel* chl = GetChannel(i);
			chl->m_arrValue.push_back((INT32)pData[i + 2]);

			float fValueS = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 0));//二次值
			float fValueP = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 1));//一次值

			if(fValueS > chl->m_fMaxValue)//保存最大绝对值
				chl->m_fMaxValue = fValueS;
			if(fValueP > chl->m_fMaxValueP)
				chl->m_fMaxValueP = fValueP;
			switch(chl->ChannelType())
			{
			case 0://电压
				if(fValueS > m_fMaxUValue)
					m_fMaxUValue = fValueS;
				if(fValueP > m_fMaxUValueP)
					m_fMaxUValueP = fValueP;
				break;
			case 1://电流
				if(fValueS > m_fMaxIValue)
					m_fMaxIValue = fValueS;
				if(fValueP > m_fMaxIValueP)
					m_fMaxIValueP = fValueP;
				break;
			case 2://其它
				if(fValueS > m_fMaxOther)
					m_fMaxOther = fValueS;
				if(fValueP > m_fMaxOtherP)
					m_fMaxOtherP = fValueP;
				break;
			}
		}
	}
	else
	{
		for (i = 0; i < m_cAChannels; i++) 
		{
			CChannel* chl = GetChannel(i);
			chl->m_arrValue.push_back((INT16)pData[i + 2]);

			float fValueS = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 0));//二次值
			float fValueP = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 1));//一次值

			if(fValueS > chl->m_fMaxValue)//保存最大绝对值
				chl->m_fMaxValue = fValueS;
			if(fValueP > chl->m_fMaxValueP)
				chl->m_fMaxValueP = fValueP;
			if(chl->ChannelType() == 1 && fValueS > 1)
				printf("..");
			switch(chl->ChannelType())
			{
			case 0://电压
				if(fValueS > m_fMaxUValue)
					m_fMaxUValue = fValueS;
				if(fValueP > m_fMaxUValueP)
					m_fMaxUValueP = fValueP;
				break;
			case 1://电流
				if(fValueS > m_fMaxIValue)
					m_fMaxIValue = fValueS;
				if(fValueP > m_fMaxIValueP)
					m_fMaxIValueP = fValueP;
				break;
			case 2://其它
				if(fValueS > m_fMaxOther)
					m_fMaxOther = fValueS;
				if(fValueP > m_fMaxOtherP)
					m_fMaxOtherP = fValueP;
				break;
			}
		}
	}

	/**************************************************************
	//	pData[2 + m_cAChannels]...为开关量
	//	开关量采用压缩存储法。即只记录变位点。格式为：
	//	采样点1，开关量1，采样点2，开关量2，...采样点n，开关量n
	//	以上表明：
	//		（1）采样点1为第一采样点，开关量为开关量1
	//		（2）到采样点2处发生变位，开关量为开关量2
	//	依此类推
	//
	//	对于第一点：记录[1, 开关量]
	//	对于后续点：如果开关量发生变化，则记录[n, 开关量]
	//	利用数组pDigital记忆上一次采样开关量值。
	**************************************************************/
	for (i = 0; i < m_cDChannels; i++)
	{
		CChannel* chl = GetChannel(m_cAChannels + i);
		if( m_nCount == 1 ) 
		{
			chl->m_arrValue.push_back((int)pData[0]);
			chl->m_arrValue.push_back((int)pData[2 + m_cAChannels + i]);
		} 
		else 
		{
			if( pData[2 + m_cAChannels + i] != pDigital[i] ) 
			{
				chl->m_arrValue.push_back((int)pData[0]);
				chl->m_arrValue.push_back((int)pData[2 + m_cAChannels + i]);
			}
		}
		pDigital[i] = pData[2 + m_cAChannels + i];
	}
}

/*************************************************************
Date: 2011/05/04 	Author:LYH
函 数 名: SetMaxValue
功能概要: 设置默认最大值
返 回 值: 
参    数: param1 
		  param2 
*************************************************************/
void CCOMTRADEParser::SetMaxValue()
{
	//处理最大值
	if(m_fMaxIValue < 1)
		m_fMaxIValue = 1;
	if(m_fMaxUValue < 1)
		m_fMaxUValue = 82;
	if(m_fMaxOther < 1)
		m_fMaxOther = 1;
	if(m_fMaxIValueP < 1)
		m_fMaxIValueP = 1;
	if(m_fMaxUValueP < 1)
		m_fMaxUValueP = 82;
	if(m_fMaxOtherP < 1)
		m_fMaxOtherP = 1;
}

string GetGBK(CGECodeConvert* pConver, string utf8)
{
	if( !pConver )
		return utf8;
	char* pGbk;
	long rt = pConver->Utf82Gbk(utf8.c_str(), utf8.length(), pGbk);
	if(rt >= 0){
		string gbk(pGbk);
		return gbk;
	}
	return "";
}

int CCOMTRADEParser::ParseHdr()
{
	tinyxml2::XMLDocument doc;
	CStrongString pathName;
	pathName.Format("%s.hdr", m_strName.c_str());
	XMLError eRet = doc.LoadFile( pathName.c_str() );
	if( XML_NO_ERROR != eRet ){
		pathName.Format("%s.HDR", m_strName.c_str());
		eRet = doc.LoadFile( pathName.c_str() );
		if( XML_NO_ERROR == eRet )
		{
			m_Hdr.result = 0;
			return 0;
		}
		//找不到文件
	}
	
	XMLElement * faultreport = doc.FirstChildElement("FaultReport");

	if( !faultreport )
	{
		//格式错误
		m_Hdr.result = 1;
		return 1;
	}

	string encoding = "";
	XMLNode* decl = doc.FirstChild();
	if( decl != NULL ){
		XMLDeclaration* declaration =decl->ToDeclaration();
		string declstr = declaration->Value();
		cout<<declstr.c_str()<<endl;
		int nfind = declstr.find("encoding");
		if( nfind < 0 )
			nfind = declstr.find("ENCODING");
		if( nfind >= 0 ){
			int n1 = declstr.find("\"",nfind+1);
			if( n1 >= 0 ){
				int n2 = declstr.find("\"",n1+1);
				if( n2 > n1 ){
					encoding = declstr.substr(n1+1, n2-n1-1);
				}
			}

		}
	}
	cout<<"encoding:"<<encoding.c_str()<<endl;
	CGECodeConvert convert;
	CGECodeConvert* pc = NULL;
	if( encoding.find("GB") >=0 || encoding.find("gb") >= 0){

	}else{
		if( encoding.find("utf") >= 0 || encoding.find("UTF") >= 0 ){
			pc = &convert;
		}else{
			//编码不支持
			m_Hdr.result = 3;
			return 3;
		}
	}
	
	XMLElement* child = faultreport->FirstChildElement();
	while( child )
	{
		string nodeName = child->Name();
		if( nodeName.compare("FaultStartTime") == 0 )
		{
			m_Hdr.FaultStartTime = child->GetText()==0?"":GetGBK(pc, child->GetText());
		}
		if( nodeName.compare("FaultKeepingTime") == 0 )
		{
			m_Hdr.FaultKeepingTime = child->GetText()==0?"":GetGBK(pc, child->GetText());
		}
		if( nodeName.compare("DataFileSize") == 0 )
		{
			m_Hdr.DataFileSize = child->GetText()==0?"":GetGBK(pc, child->GetText());
		}
		
		if( nodeName.compare("TripInfo") == 0 )
		{
			XMLElement* info = child->FirstChildElement();
			TripInfoObj trip;
			while( info )
			{
				string infoname = info->Name();
				if( infoname.compare("time") == 0 )
				{
					trip.time = info->GetText()==0?"":GetGBK(pc, info->GetText());
				}
				if( infoname.compare("name") == 0 )
				{
					trip.name = info->GetText()==0?"":GetGBK(pc, info->GetText());
				}
				if( infoname.compare("phase") == 0 )
				{
					trip.phase = info->GetText()==0?"":GetGBK(pc, info->GetText());
				}
				if( infoname.compare("value") == 0 )
				{
					trip.value = info->GetText()==0?"":GetGBK(pc, info->GetText());
				}
				if( infoname.compare("FaultInfo") == 0 )
				{				
						map<string, string> val;
						XMLElement* faultinfo = info->FirstChildElement();
						while(faultinfo)
						{
							val.insert( pair<string,string>(faultinfo->Name(), faultinfo->GetText()==0?"":GetGBK(pc, faultinfo->GetText())) );
							faultinfo = faultinfo->NextSiblingElement();
						}
						trip.FaultInfo.push_back(val);
				}
				info = info->NextSiblingElement();
			}
			m_Hdr.TripInfo.push_back(trip);
		}
	
		if( nodeName.compare("FaultInfo") == 0 )
		{
			XMLElement* info = child->FirstChildElement();
			map<string, string> val;
			while(info)
			{								
				val.insert( pair<string,string>(info->Name(), info->GetText()==0?"":GetGBK(pc, info->GetText())) );
				cout<<"parse hdr:faultinfo "<<info->Name()<<endl;
				info = info->NextSiblingElement();
			}
			m_Hdr.FaultInfo.push_back(val);
		}
		if( nodeName.compare("DigitalStatus") == 0 )
		{
			XMLElement* info = child->FirstChildElement();
			map<string, string> val;
			while(info)
			{								
				val.insert( pair<string,string>(info->Name(), info->GetText()==0?"":GetGBK(pc, info->GetText())) );
				info = info->NextSiblingElement();
			}
			m_Hdr.DigitalStatus.push_back(val);
		}	
		
		if( nodeName.compare("DigitalEvent") == 0 )
		{
			XMLElement* info = child->FirstChildElement();
			map<string, string> val;
			while(info)
			{								
				val.insert( pair<string,string>(info->Name(), info->GetText()==0?"":GetGBK(pc, info->GetText())) );
				info = info->NextSiblingElement();
			}
			m_Hdr.DigitalEvent.push_back(val);
		}
				
		if( nodeName.compare("SettingValue") == 0 )
		{
			XMLElement* info = child->FirstChildElement();
			map<string, string> val;
			while(info)
			{								
				val.insert( pair<string,string>(info->Name(), info->GetText()==0?"":GetGBK(pc, info->GetText())) );
				info = info->NextSiblingElement();
			}
			m_Hdr.SettingValue.push_back(val);
		}
		
		child = child->NextSiblingElement();
	}
	m_Hdr.result = 2;
	return 2;
}

string CCOMTRADEParser::HdrObjToJson()
{
	cJSON* fr = cJSON_CreateObject();

	cJSON_AddStringToObject(fr, "FaultStartTime", m_Hdr.FaultStartTime.c_str());
	cJSON_AddStringToObject(fr, "FaultKeepingTime", m_Hdr.FaultKeepingTime.c_str());
	cJSON_AddStringToObject(fr, "DataFileSize", m_Hdr.DataFileSize.c_str());
	cJSON_AddNumberToObject(fr, "result", m_Hdr.result);
	cJSON_AddStringToObject(fr, "OriginalContent", m_Hdr.OriginalContent.c_str());

	if( m_Hdr.TripInfo.size() > 0 )
	{
		cJSON* trips = cJSON_CreateArray();
		list<TripInfoObj>::iterator it = m_Hdr.TripInfo.begin();
		for( it; it != m_Hdr.TripInfo.end(); ++it )
		{
			cJSON* tripinfo = cJSON_CreateObject();
			cJSON_AddStringToObject(tripinfo, "time", (*it).time.c_str());
			cJSON_AddStringToObject(tripinfo, "name", (*it).name.c_str());
			cJSON_AddStringToObject(tripinfo, "phase", (*it).phase.c_str());
			cJSON_AddStringToObject(tripinfo, "value", (*it).value.c_str());
			if( (*it).FaultInfo.size() > 0 )
			{
				cJSON* faultinfos = cJSON_CreateArray();
				list< map<string, string> >::iterator itlist = ((*it).FaultInfo).begin();
				for( itlist; itlist != ((*it).FaultInfo).end(); ++itlist )
				{
					cJSON* faultinfo = cJSON_CreateObject();
					map<string, string>::iterator itmap = (*itlist).begin();
					for( itmap; itmap != (*itlist).end(); ++itmap )
					{
						cJSON_AddStringToObject(faultinfo, itmap->first.c_str(), itmap->second.c_str());
					}
					cJSON_AddItemToArray(faultinfos, faultinfo);
				}
				cJSON_AddItemToObject(tripinfo, "FaultInfo", faultinfos);
			}
			cJSON_AddItemToArray(trips, tripinfo);
		}
		cJSON_AddItemToObject(fr, "TripInfo", trips);
	}

	if( m_Hdr.FaultInfo.size() > 0 )
	{
		cJSON* infos = cJSON_CreateArray();
		list< map<string, string> >::iterator itlist = m_Hdr.FaultInfo.begin();
		for( itlist; itlist != m_Hdr.FaultInfo.end(); ++itlist )
		{
			cJSON* info = cJSON_CreateObject();
			map<string, string>::iterator itmap = (*itlist).begin();
			for( itmap; itmap != (*itlist).end(); ++itmap )
			{
				cJSON_AddStringToObject(info, itmap->first.c_str(), itmap->second.c_str());
			}
			cJSON_AddItemToArray(infos, info);
		}
		cJSON_AddItemToObject(fr, "FaultInfo", infos);
	}

	if( m_Hdr.DigitalEvent.size() > 0 )
	{
		cJSON* infos = cJSON_CreateArray();
		list< map<string, string> >::iterator itlist = m_Hdr.DigitalEvent.begin();
		for( itlist; itlist != m_Hdr.DigitalEvent.end(); ++itlist )
		{
			cJSON* info = cJSON_CreateObject();
			map<string, string>::iterator itmap = (*itlist).begin();
			for( itmap; itmap != (*itlist).end(); ++itmap )
			{
				cJSON_AddStringToObject(info, itmap->first.c_str(), itmap->second.c_str());
			}
			cJSON_AddItemToArray(infos, info);
		}
		cJSON_AddItemToObject(fr, "DigitalEvent", infos);
	}

	if( m_Hdr.DigitalStatus.size() > 0 )
	{
		cJSON* infos = cJSON_CreateArray();
		list< map<string, string> >::iterator itlist = m_Hdr.DigitalStatus.begin();
		for( itlist; itlist != m_Hdr.DigitalStatus.end(); ++itlist )
		{
			cJSON* info = cJSON_CreateObject();
			map<string, string>::iterator itmap = (*itlist).begin();
			for( itmap; itmap != (*itlist).end(); ++itmap )
			{
				cJSON_AddStringToObject(info, itmap->first.c_str(), itmap->second.c_str());
			}
			cJSON_AddItemToArray(infos, info);
		}
		cJSON_AddItemToObject(fr, "DigitalStatus", infos);
	}

	if( m_Hdr.SettingValue.size() > 0 )
	{
		cJSON* infos = cJSON_CreateArray();
		list< map<string, string> >::iterator itlist = m_Hdr.SettingValue.begin();
		for( itlist; itlist != m_Hdr.SettingValue.end(); ++itlist )
		{
			cJSON* info = cJSON_CreateObject();
			map<string, string>::iterator itmap = (*itlist).begin();
			for( itmap; itmap != (*itlist).end(); ++itmap )
			{
				cJSON_AddStringToObject(info, itmap->first.c_str(), itmap->second.c_str());
			}
			cJSON_AddItemToArray(infos, info);
		}
		cJSON_AddItemToObject(fr, "SettingValue", infos);
	}

	char* json = cJSON_PrintUnformatted(fr);
	cJSON_Delete(fr);
	string s = json;
	free(json);
	return s;
}

