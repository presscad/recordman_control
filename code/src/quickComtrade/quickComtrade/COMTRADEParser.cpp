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

//��������ļ�ʱ�����ƵĴ�С 1.33M(1024 * 1024 * 1.33)
//Ŀ��Ϊ��һ���������ݵ���.
const int  FILESIZE     = 1394606;   

//�ڹ����ڴ���,ͷ256���ֽڴ���ļ���,��������sizeof(int)���ֽڴ����Ч���ݳ���,����������
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
//���ǰ�ε��ö��´ε��õ�Ӱ��
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
�� �� ��: LoadCfgData
���ܸ�Ҫ: ��ȡCFG�ļ�
�� �� ֵ: ��ȡʧ�ܷ���<0, �ɹ�����>=0
��    ��: param1 ������Ϣ
		  param2 
************************************/
int CCOMTRADEParser::LoadCfgData(CStrongString& strError)
{
	CStrongString lpszPathName, strMsg;
	//temp variables 
	char* buff;
	char szStation[100], szDevice[100], szChannelName[100], szPhase[10], szCbm[100], szUU[100], szPS[2];	

	//û�и����ļ���,����ִ��װ�����ݲ���
	if(m_strName.GetLength() < 1)
		return -1;
	printf("��CFG�ļ�:%s\n",m_strName.c_str());
	//CFG�ļ���
	lpszPathName.Format("%s.cfg",m_strName.c_str());
	CFileEx std_File;
    try
	{
		if(!std_File.Open(lpszPathName,CXJFile::modeRead))//����ǿ�ƹر�
		{
			lpszPathName.Format("%s.CFG", m_strName.c_str());
			if(!std_File.Open(lpszPathName,CXJFile::modeRead))
			{
				strError.Format("���ļ�ʧ�ܣ��ļ�·��Ϊ:%s", lpszPathName.c_str());
				return -1;
			}
		}
		printf("���ļ��ɹ�:%s\n",lpszPathName.c_str());
		CStrongString strTemp;
		//���ļ� 
		// station_name,rec_dev_id,rev_year <CR/LF>
		std_File.ReadString(strTemp);
		buff = PreFmt(strTemp);
		if(buff == NULL)
		{
			std_File.Close();
			strError = "��ȡ��վ��Ϣ��ʧ��";
			return -1;
		}
		sscanf(buff, "%[^,],%[^,],%d", szStation, szDevice, &m_nVersion);
		//��վ��
		m_strStationName = szStation;	
		PostFmt(m_strStationName);
		//¼������
		m_strDFRID = szDevice; 			
		PostFmt(m_strDFRID);
		//����abb�ı���¼���ļ���׼��2001�ģ���1999����
        if(m_nVersion == 2001)
		{
			m_nVersion = 1999;
		}
		//COMTRADE�ļ��ı�׼����Ϊ1997 
		if(m_nVersion != 1999 && m_nVersion != 1997)
			m_nVersion = 1991;
		
		//ͨ������
		std_File.ReadString(strTemp);
        if(strTemp.IsEmpty())
		{
			std_File.Close();
			strError = "ͨ��������Ϣ��Ϊ��";
			return -2;
		}
		int cChannels = 0;
		strTemp.Replace("A", "");
		strTemp.Replace("D", "");
		//����ȥ�ո�
		strTemp.Replace(" ", "");
		buff = PreFmt(strTemp);
		if(buff != NULL)
		{
			//TT, ##A, ##D <CR/LF>
			sscanf(buff, "%d,%d,%d", &cChannels, &m_cAChannels, &m_cDChannels);
			if(m_cAChannels == 0 && m_cDChannels == 0 )
			{
				std_File.Close();
				strError = "��¼���ļ�ͨ����Ϊ0";
				return -3;
			}
		}
		else
		{
			std_File.Close();
			strError = "��ȡͨ��������Ϣ��ʧ��";
			return -4;
		}
		//����ͨ�������С
// 		m_arrChannel.reserve(cChannels);
// 		m_arrChannel.begin();
		
		int i = 0;
		int nIndexTemp = 0;
		//��ȡģ����ͨ������
		for(i = 0; i < m_cAChannels; i++) {
			std_File.ReadString(strTemp);
			
			buff = PreFmt(strTemp);
			if(buff == NULL)
			{
				std_File.Close();
				strError.Format("��ȡ��%d��ģ����ͨ����Ϣʧ��", i+1);
				return -5;
			}
			CChannel* chl = new CChannel;
			chl->m_bAnalog = true;
			chl->m_nIndex = nIndexTemp++;
			if(m_nVersion == 1999 || m_nVersion == 1997)//97,99��ʽ
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
			else //91��ʽ
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
		
		//��ȡ������ͨ������
		for(i = 0; i < m_cDChannels; i++)
		{
			std_File.ReadString(strTemp);
			
			buff = PreFmt(strTemp);
			if(buff == NULL)
			{
				std_File.Close();
				strError.Format("��ȡ��%d��������ͨ����Ϣʧ��", i+1);
				return -6;
			}
			CChannel* chl = new CChannel;
			chl->m_bAnalog = false;
			chl->m_nIndex = nIndexTemp++;
			if(m_nVersion == 1999 || m_nVersion == 1997) //97,99��ʽ
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
			else //91��ʽ
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

		//��·Ƶ��
		std_File.ReadString(strTemp);
		
		buff = PreFmt(strTemp);
		if(buff == NULL)
		{
			std_File.Close();
			this->FreeData();
			strError = "��ȡ��·Ƶ����Ϣ��ʧ��";
			return -7;
		}
		sscanf(buff, "%f", &m_fLineFreq);
		if(m_fLineFreq == 0) //���ϵͳƵ��
		{
			std_File.Close();
			this->FreeData();
			strError.Format("��·Ƶ�ʷǷ�,ֵΪ:%d", m_fLineFreq);
			return -8;
		}
		
		//����Ƶ�ʸ�����Ϣ
		std_File.ReadString(strTemp);
		
		buff = PreFmt(strTemp);
		if(buff == NULL)
		{
			std_File.Close();
			this->FreeData();
			strError = "��ȡ����Ƶ������Ϣ��ʧ��";
			return -9;
		}
		sscanf(buff, "%d", &m_nNRates);
		if(m_nNRates < 0)   //���Ƶ�ʸ��� add by yinzhehong 20050512
		{
			std_File.Close();
			this->FreeData();
			strError.Format("����Ƶ�ʷǷ�,ֵΪ:%d", m_nNRates);
			return -10;
		}
		else if(m_nNRates == 0)//��nrates��samp��Ϊ0ʱ��endsamp�ֶκ������ļ�DAT�е�timestamp�������
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
				
				strTemp.Replace(" ", "");//����ȥ�ո�
				
				buff = PreFmt(strTemp);
				//samp,endsamp <CR/LF>
				if(buff) //���ӿ��ж� add by yinzhehong 20060529
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
				
				strTemp.Replace(" ", "");//����ȥ�ո�
				
				buff = PreFmt(strTemp);
				//samp,endsamp <CR/LF>
				if(buff) //���ӿ��ж� add by yinzhehong 20060529
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
		// ������ݵ���
        if(m_cSamples <= 0)  //add by yinzhehong 20050512
		{
			std_File.Close();
			this->FreeData();
			strError.Format("�����������Ƿ�,ֵΪ:%d", m_cSamples);
			return -11;
		}
		
		//����ģ��ͨ�����������ǹ̶���,�ڴ����ò����������С,�����ִ��Ч��,
		//��������ͨ������ֵ�ǲ���ѹ���洢��ʽ(ֻ�洢��λͨ���Ĳ���������ֵ�Ͳ�����ֵ),��С�ǲ��̶���.
// 		for(i = 0; i <m_cAChannels; i++)
// 		{    
// 			CChannel* chl = GetChannel(i);
// 			chl->m_arrValue.reserve(m_cSamples);
// 		}
		
		//m_arrSampleTime.reserve(m_cSamples);
		//m_arrSampleTime.begin();
		
		//m_strDTFirst ����ʱ��
		std_File.ReadString(m_strDTFirst);
		
		//m_strDTTrigger ����ʱ��(����ʱ��)
		std_File.ReadString(m_strDTTrigger);

		//���㴥��ʱ������ڲ�����ʼʱ���ֵ
		CalTriggerTime();
		if(m_nTriggerTime < 0)
		{
			std_File.Close();
			this->FreeData();
			strError.Format("����ʱ�����ڲ�����ʼʱ��");
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
�� �� GetChannel()
���ܸ�Ҫ����ȡͨ�������е���nChannelNOָ����ͨ��
�� �� ֵ: ��Ӧ��ͨ��
��    ����ͨ����
**************************************************************/
//##ModelId=489955E201D7
CChannel* CCOMTRADEParser::GetChannel(int nChannelNO)
{
	//��ֹ����Խ��,С����Сֵȡ��Сֵ,�������ֵȡ���ֵ
	if(nChannelNO < 0)
		return NULL;
	if(nChannelNO >= m_arrChannel.size())
		return NULL;
	CChannel* pChl = (CChannel*)m_arrChannel[nChannelNO];
	if(pChl == NULL)
		return NULL;
	if(pChl->m_nIndex != nChannelNO)//����Ƿ�ƥ��
		return NULL;

	return pChl;
}

/************************************
�� �� ��: LoadDataFromFile
���ܸ�Ҫ: ��ȡDAT�ļ�
�� �� ֵ: ��ȡʧ�ܷ���<0, �ɹ�����>=0
��    ��: param1 ������Ϣ
		  param2
************************************/
int CCOMTRADEParser::LoadDataFromFile(CStrongString& strError)
{
	//û�и����ļ���,����ִ��װ�����ݲ���
	if(m_strName.GetLength() < 1)
		return -1;

	CStrongString lpszPathName, strMsg;

	int register i;

	//�γ������ļ�����
	vector<CStrongString> arrDataFile;
	FILE* fp = NULL;
	//ȡ�������ļ���
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
		//ʹ�ô�д
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

	printf("���ļ��ɹ�:%s\n",lpszPathName.c_str());

	if(arrDataFile.size() == 0)
	{
		strError.Format("���ܴ��ļ�%s,�Ҳ�����Ӧ�ļ�!",lpszPathName.c_str());
		printf("���ܴ��ļ�%s,�Ҳ�����Ӧ�ļ�!",lpszPathName.c_str());
		return -2;
	}

	//����ÿ�β��������ֽ���
	//��������ͨ��,ÿ16��Ϊһ��,�洢��һ��_int16�͵ı�����,ÿ������ͨ����Ӧһλ,��洢��ʽΪ:
	//��_int16�ͱ����д�0����15λ�������ͨ������������������.
	int nLineLength, nDLength;
	if(m_bAscii == true)
	{
		//�����ָ��","
		//˵��:��׼99��ʽnLineLength=11+11+m_cAChannels*7 + m_cDChannels * 2 ;�ڲ��Թ����з���
		//�е��ļ���ģ������������ռ��7���ַ��Ŀ��(��׼99��ʽӦΪ6���ַ����,������","),״̬��
		//��������Ҳ��ռ��7���ַ��Ŀ��,���Բ�������� *10 ��ʽ.
		nLineLength = 11 + 11 + m_cAChannels * 10 + m_cDChannels * 10 ;
	} 
	else 
	{
		nDLength =(m_cDChannels / 16) * 2;

		if(m_cDChannels % 16 != 0 && m_cDChannels != 0) 
			nDLength += 2;

		nLineLength = 4 + 4 + m_cAChannels * 2 + nDLength;
	}
	//���ڶ�ȡһ�У�һ�Σ��������ݣ�ԭʼASCII ���� BINRAY��
	char* pBuffer	= new char[nLineLength];
	//������ʱ�洢ת�����һ�У�һ�Σ���������
	INT32 *pData	= new INT32[m_cAChannels+m_cDChannels+2];
	//������ʱ�洢��һ�β����еĿ�������ʵ��ѹ���洢��
	int* pDigital = new int[m_cDChannels];
	char *head, *tail; 

	//����arrMask����,������ȡ��Ӧλ������ͨ����ֵ
	// arrMask = {
	//				0000000000000001
	//				0000000000000010
	//				...
	//				1000000000000000
	//			 }
	INT16 arrMask[16]={0};
	INT16 mask = 0x0001;
	if(!m_bAscii)//�ı��ļ�,��ѭ��ִ��������
	{
		for( i = 0; i < 16; i++ ) 
			arrMask[i] = mask << i ;
	}
	INT16 n16Temp=0;

	try
	{
		m_nCount = 0;
		//����������ļ�����ȡ����
		for(int j = 0; j < arrDataFile.size(); j++) 
		{
			//		lpszPathName = arrDataFile.GetAt(j);
			lpszPathName = arrDataFile[j];
			if( ( fp = fopen(lpszPathName, m_bAscii ? "r" : "rb") ) == NULL ) {			
				strError.Format("���ܴ��ļ�%s!",lpszPathName.c_str());
				return -3;
			}
			//���У������������ȡ����
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
					//����������������������Ч
					m_nCount--;
					break;
				}
				
				memset(pData, 0, m_cAChannels+m_cDChannels+2);
				if(m_bAscii == true) 
				{ //�ı��ļ�
					//��ȡһ�в�������
					memset(pBuffer, 0, nLineLength);
					fgets(pBuffer, nLineLength, fp);

					if(strlen(pBuffer) == 0)
					{
						m_nCount--;
						break;
					}

					head=tail=pBuffer; 
					i=0;
					//�����������ݣ������������ݴ���pData����
					while(*tail && i < m_cAChannels + m_cDChannels + 2) 
					{ 
						while(*tail && *tail != ',') tail++; 
						*tail = '\0'; 
						pData[i] = atoi(head); 
						i++;
						head = ++tail;
					}
					//����COMTRADE�ļ���ʽ,��ȡ�ļ���pData[0]��ŵ�Ӧ�ǲ��������(�ڼ���������),
					//���pData[0]<1 ���� pData[0]>m_cSamples(����������,�������ļ��ж����),��˵������������ЧӦ����,
					//������Ч�����Ѷ���,���ζ�ȡ��ֵ��������.
// 					if(pData[0] < 1 || pData[0] > m_cSamples)
// 					{
// 						m_nCount--;
// 						continue;
// 					}
					pData[0] = m_nCount;
				} 
				else 
				{ //�������ļ�
					//��ȡһ�β�������
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

					//�����������ݣ������������ݴ���data����
					//pBuffer0~3λΪ����������
					memcpy(&pData[0], pBuffer, 4);
					//����COMTRADE�ļ���ʽ,��ȡ�ļ���pData[0]��ŵ�Ӧ�ǲ��������(�ڼ���������),
					//���ڶ������ļ�����������ʽ��ŵ�,���pData[0]<1 ���� pData[0]>m_cSamples(����������,�������ļ��ж����),
					//˵����Ч�����Ѷ���,���ζ�ȡ��ֵ��������.
// 					if(pData[0] < 1 || pData[0] > m_cSamples)
// 					{
// 						m_nCount--;
// 						continue;
// 					}
					pData[0] = m_nCount;

					//pBuffer4~7λΪ����ʱ������
					memcpy(&pData[1], pBuffer + 4, 4);

					//pBuffer8~Ϊģ�������ݣ�_int16��������pDataΪ�������飬�ʿ����������У�����Ϊ0
					for(i = 0; i < m_cAChannels; i++) 
						memcpy(&pData[2 + i] , pBuffer + 8 + i * 2, 2); 

					//���Ϊ��������16·������ռ��һ��_int16
					for(i = 0; i < m_cDChannels; i++) {
						memcpy(&n16Temp,pBuffer+8 + m_cAChannels * 2 + (i/16)*2,2);
						if(n16Temp & arrMask[i%16] ) {
							pData[2 + m_cAChannels + i ] = 1;
						} else {
							pData[2 + m_cAChannels + i ] = 0;
						}
					}
				}//end �������ļ�

				//���ڣ����β��������Ѿ��洢��pData�����С����潫���ݴ�pData���鸴�Ƶ���Ӧͨ����
				//pData[0] Ϊ�����㣬pData[1]Ϊ����ʱ��
				//m_arrSampleTime.assign(pData[0]-1, (UINT)pData[1]);
				m_arrSampleTime.push_back((int)pData[1]*m_fTimeMult);
				//�������
				FillData(m_bAscii, pData, pDigital);
			}	
			fclose(fp);
		}
	}
	catch(...)
	{
		strError = "����δ֪����";
		return -4;
	}

	delete[] pData;
	delete[] pBuffer;
	delete[] pDigital;

	//����Ĭ�����ֵ
	SetMaxValue();

	return 0;
}

/************************************
�� �� ��: GetFileName
���ܸ�Ҫ: ȡ���ļ�����������׺
�� �� ֵ: void
��    ��: param1 ָ��Ҫ���ļ����ļ���
************************************/
void CCOMTRADEParser::GetFileName( CStrongString sFileName )
{
	//˵��:���ڳ�����Ҫ���������ļ��������ļ�,��m_strName�ǰ���·������չ���������ļ���,Ϊ�����洦����,
	//�����ڴ�ȥ����չ��,���������ļ�ʱʹ lpszPathName=m_strName+".cfg";
	//���������ļ�ʱʹ lpszPathName=m_strName+".dat" �� + ".d00" ...;
	//Ѱ���ļ���
	m_strName = sFileName;
	/*CFileEx file;
	try
	{
		if(!file.Open(m_strName,CXJFile::modeRead))
			return;//�����һ�����ܴ򿪵��ļ��������ļ�������
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
	else//���·���¿���û��
	{
		int j = m_strName.ReverseFind('.');
		if(j > 0)
		{
			m_strName = m_strName.Left(j);
		}
	}
}

/************************************
�� �� ��: CompatibleDiff
���ܸ�Ҫ: ���ݶԹ淶�Ĳ�ͬ���
�� �� ֵ:  
��    ��: param1
		  param2
************************************/
void CCOMTRADEParser::CompatibleDiff()
{
	//����CFG�ļ��еĲ���Ƶ���µĲ��������������ڲ�ͬ�����
	//��ʽ0: ÿ������Ƶ�ʺ�����ŵĲ�������Ϊ��Ƶ���µĲ�������
	//��ʽ1: ÿ������Ƶ�ʺ�����ŵĲ�������Ϊ��Ƶ���²�����Ľ���������
	//��Ϊ���ڹ淶����ⲻͬ,�������ܲ�����ļ�������Щ��ͬ.����Ϊ�����������
	
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

	//������ָ������Ƶ�ʵ��������Ƶ�ʼ���ʱ�䣬��ʹ��DAT�ļ����ʱ��
	if(m_nNRates > 0 && bCal)
	{
		for(int i = 0; i < m_cSamples; i++)
		{
			if(i == 0)
			{
				//��һ��ʱ��һ��Ϊ0
				m_arrSampleTime[0] = 0;
			}
			else
			{
				//����ȷ���ڵڼ���Ƶ�ʶ�
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
				//�õ���Ƶ���µ�����
				int nRate = (int)m_arrSampleRate[nPos]; //Ƶ��
				int nCyc = 1000000/nRate; //����
				//ʱ��Ϊǰһ��ʱ��+����
				int nTime = m_arrSampleTime[i-1] + nCyc;
				m_arrSampleTime[i] = nTime;
			}
		}
	}
}

/************************************
�� �� ��: LoadFile
���ܸ�Ҫ: ���벨���ļ�
�� �� ֵ: ����ɹ�����>=0, ʧ�ܷ���<0
��    ��: param1 �����ļ�·��
		  param2 ������Ϣ
************************************/
int CCOMTRADEParser::LoadFile( CStrongString strFileName, CStrongString& strError )
{
	//��ȡ�ļ���
	GetFileName(strFileName);
	//��ȡ�����ļ�
	int nReturn = LoadCfgData(strError);
	if(nReturn < 0)
		return nReturn;
	//��ȡ�����ļ�
	nReturn = LoadDataFromFile(strError);
	if(nReturn < 0)
		return nReturn;
	//���ݴ���
	CompatibleDiff();
	return 0;
}

/************************************
�� �� ��: GetRealValue
���ܸ�Ҫ: ȡ��ָ��ͨ��ָ���������ֵ
�� �� ֵ: ��ʵֵ��ģ��ͨ������ax+b�������
��    ��: param1 ָ��ͨ������
param2 ָ���������
param3 ָ����Ҫ��ֵ�����ͣ�Ĭ��Ϊ����ֵ.0Ϊ����ֵ��1Ϊһ��ֵ
************************************/
float CCOMTRADEParser::GetRealValue(int nChannelNo, int nSampleNo, int nDataType/* = 0*/)
{
	//ָ����ͨ���Ż���ָ���Ĳ����㲻����,���Ƿ���0
	if(nSampleNo >= m_cSamples || nSampleNo < 0)
		return 0.0f;
	CChannel* pchl = GetChannel(nChannelNo);
	if(pchl != NULL)
	{
		if(pchl->m_bAnalog)
		{
			//ģ����ͨ��
			float fValue = (float)pchl->m_fScale * GetRecordValue(nChannelNo, nSampleNo) + pchl->m_fOffset;
			if(nDataType == 1)//һ��ֵ
			{
				return TransformPrimaryValue(pchl, fValue);
			}
			else//����ֵ
			{
				return TransformSecondaryValue(pchl, fValue);
			}
		}
		else
		{
			//������ͨ��
			return GetRecordValue(nChannelNo, nSampleNo);
		}
	}
	return 0.0f;
}

/************************************
�� �� ��: GetRecordValue
���ܸ�Ҫ: ȡ��ָ��ͨ��ָ���������ֵ
�� �� ֵ: ����ֵ.DAT�ļ����¼��ֵ
��    ��: param1 ָ��ͨ������
		  param2 ָ��������� 
************************************/
int CCOMTRADEParser::GetRecordValue( int nChannelNo, int nSampleNo )
{
	//ָ����ͨ���Ż���ָ���Ĳ����㲻����,���Ƿ���0
	if(nSampleNo >= m_cSamples || nSampleNo < 0)
		return 0.0f;
	CChannel* pchl = GetChannel(nChannelNo);
	if(pchl != NULL)
	{
		if(pchl->m_bAnalog)//ģ����ͨ��
		{
			return pchl->m_arrValue[nSampleNo];
		}
		else//������ͨ��
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
					//���һ����λ
					if(i == pchl->m_arrValue.size() -2)
						return pchl->m_arrValue[i+1];
				}
			}
		}
	}
	return 0;
}

/************************************
�� �� ��: GetTimeBySampling
���ܸ�Ҫ: ȡ��ָ������������ڲ�����ʼʱ�̵�ʱ��
�� �� ֵ: ����ڲ�����ʼʱ�̵�ʱ��(��λ������)
��    ��: param1 ָ���������
		  param2 
************************************/
int CCOMTRADEParser::GetTimeBySampling( int nSampleNo )
{
	if(nSampleNo < 0 || nSampleNo > m_arrSampleTime.size())
		return -1;
	int nTime = m_arrSampleTime[nSampleNo];//΢��
	nTime /= 1000;
	return nTime;
}

/************************************
�� �� ��: GetTimeByTrigger
���ܸ�Ҫ: ȡ��ָ������������ڹ��Ͽ�ʼʱ�̵�ʱ��
�� �� ֵ: ����ڹ��Ͽ�ʼʱ�̵�ʱ��(��λ������)
��    ��: param1 ָ���������
		  param2 
************************************/
int CCOMTRADEParser::GetTimeByTrigger( int nSampleNo )
{
	int nTime = GetTimeBySampling(nSampleNo);
	int nStartTime = GetTimeBySampling(0);
	return (nTime - (nStartTime+m_nTriggerTime));
}

/************************************
�� �� ��: CalTriggerTime
���ܸ�Ҫ: ������ϴ���ʱ������ڲ�����ʼʱ�̵�ʱ�䣨��λ�����룩�����������m_nTriggerTime
�� �� ֵ: 
��    ��: param1 
		  param2 
************************************/
void CCOMTRADEParser::CalTriggerTime()
{
	//���ڲ�����
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
	//ֵ�����뼶�ͺ��뼶
	//��������
	//������ʼʱ��
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
	//���ϴ���ʱ��
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

	//�����ֵ
	m_nTriggerTime = ((((nTriggerHH-nFirstHH)*60 + nTriggerMM - nFirstMM)*60 + nTriggerSS - nFirstSS)*1000+nTriggerMS-nFirstMS);

	if(m_nTriggerTime < 0)
		m_nTriggerTime = 0;
}

/*************************************************************
Date: 2010/09/19 	Author:LYH
 �� �� ��: TransformSecondaryValue()
 ���ܸ�Ҫ: ��ָ��ͨ����ָ��ֵת��Ϊ����ֵ
 �� �� ֵ: ����ֵ
 ��    ��: param1 ͨ��
		   Param2 Ҫת����ֵ
**************************************************************/
float CCOMTRADEParser::TransformSecondaryValue( CChannel* pChl, float fValue )
{
	//���������Ч��
	if(pChl == NULL)
		return fValue;
	if(!pChl->m_bAnalog)
		return fValue;
	if(m_nVersion == 1991)
	{
		//91���ļ�����Ϊax+b�ó���ֵ���Ƕ���ֵ
		return fValue;
	}
	else //97��99�汾
	{
		//�ж������һ�α�ȺͶ��α���Ƿ���Ч
		if((pChl->m_fPrimary < 0.00000001) || (pChl->m_fSecondary < 0.00000001))
		{
			//��������Ч
			return fValue;
		}
		else
		{
			//��������Ч
			if(pChl->m_strDType == "p" || pChl->m_strDType == "P") //ͨ����¼����һ��ֵ
				return (fValue/pChl->m_fPrimary)*pChl->m_fSecondary;
			else
				return fValue;
		}
	}
	return fValue;
}

/*************************************************************
Date: 2010/09/19 	Author:LYH
�� �� ��: TransformPrimaryValue
���ܸ�Ҫ: ��ֵת��Ϊһ��ֵ
�� �� ֵ: һ��ֵ
��    ��: param1 ͨ��
		  param2 Ҫת����ֵ
*************************************************************/
float CCOMTRADEParser::TransformPrimaryValue( CChannel* pChl, float fValue )
{
	//���������Ч��
	if(pChl == NULL)
		return fValue;
	if(!pChl->m_bAnalog)
		return fValue;
	if(m_nVersion == 1991)
	{
		//91���ļ�����Ϊax+b�ó���ֵ���Ƕ���ֵ,����CT��PT��֮ת��Ϊһ��ֵ
		if(pChl->ChannelType() == 0) //��ѹͨ��
		{
			//����PT���
			return fValue*m_fPT;
		}
		else if(pChl->ChannelType() == 1) //����ͨ��
		{
			//����CT���
			return fValue*m_fCT;
		}
		else
			return fValue;
	}
	else //97��99�汾
	{
		//�ж������һ�α�ȺͶ��α���Ƿ���Ч
		if((pChl->m_fPrimary < 0.00000001) || (pChl->m_fSecondary < 0.00000001))
		{
			//��������Ч
			if(pChl->ChannelType() == 0) //��ѹͨ��
			{
				//����PT���
				return fValue*m_fPT;
			}
			else if(pChl->ChannelType() == 1) //����ͨ��
			{
				//����CT���
				return fValue*m_fCT;
			}
			else
				return fValue;
		}
		else
		{
			//��������Ч
			if(pChl->m_strDType == "s" || pChl->m_strDType == "S") //ͨ����¼���Ƕ���ֵ
				return (fValue*pChl->m_fPrimary)/pChl->m_fSecondary;
			else
				return fValue;
		}
	}
	return fValue;
}

/*************************************************************
Date: 2011/05/04 	Author:LYH
�� �� ��: FillData
���ܸ�Ҫ: ������ݵ�ͨ��
�� �� ֵ: 
��    ��: param1 �Ƿ�Ascii��ʽ
		  param2 ָ����������
		  param3 ���ڼ�¼������������
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

			float fValueS = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 0));//����ֵ
			float fValueP = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 1));//һ��ֵ

			if(fValueS > chl->m_fMaxValue)//����������ֵ
				chl->m_fMaxValue = fValueS;
			if(fValueP > chl->m_fMaxValueP)
				chl->m_fMaxValueP = fValueP;
			switch(chl->ChannelType())
			{
			case 0://��ѹ
				if(fValueS > m_fMaxUValue)
					m_fMaxUValue = fValueS;
				if(fValueP > m_fMaxUValueP)
					m_fMaxUValueP = fValueP;
				break;
			case 1://����
				if(fValueS > m_fMaxIValue)
					m_fMaxIValue = fValueS;
				if(fValueP > m_fMaxIValueP)
					m_fMaxIValueP = fValueP;
				break;
			case 2://����
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

			float fValueS = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 0));//����ֵ
			float fValueP = abs(GetRealValue(chl->m_nIndex, m_nCount-1, 1));//һ��ֵ

			if(fValueS > chl->m_fMaxValue)//����������ֵ
				chl->m_fMaxValue = fValueS;
			if(fValueP > chl->m_fMaxValueP)
				chl->m_fMaxValueP = fValueP;
			if(chl->ChannelType() == 1 && fValueS > 1)
				printf("..");
			switch(chl->ChannelType())
			{
			case 0://��ѹ
				if(fValueS > m_fMaxUValue)
					m_fMaxUValue = fValueS;
				if(fValueP > m_fMaxUValueP)
					m_fMaxUValueP = fValueP;
				break;
			case 1://����
				if(fValueS > m_fMaxIValue)
					m_fMaxIValue = fValueS;
				if(fValueP > m_fMaxIValueP)
					m_fMaxIValueP = fValueP;
				break;
			case 2://����
				if(fValueS > m_fMaxOther)
					m_fMaxOther = fValueS;
				if(fValueP > m_fMaxOtherP)
					m_fMaxOtherP = fValueP;
				break;
			}
		}
	}

	/**************************************************************
	//	pData[2 + m_cAChannels]...Ϊ������
	//	����������ѹ���洢������ֻ��¼��λ�㡣��ʽΪ��
	//	������1��������1��������2��������2��...������n��������n
	//	���ϱ�����
	//		��1��������1Ϊ��һ�����㣬������Ϊ������1
	//		��2����������2��������λ��������Ϊ������2
	//	��������
	//
	//	���ڵ�һ�㣺��¼[1, ������]
	//	���ں����㣺��������������仯�����¼[n, ������]
	//	��������pDigital������һ�β���������ֵ��
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
�� �� ��: SetMaxValue
���ܸ�Ҫ: ����Ĭ�����ֵ
�� �� ֵ: 
��    ��: param1 
		  param2 
*************************************************************/
void CCOMTRADEParser::SetMaxValue()
{
	//�������ֵ
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
		//�Ҳ����ļ�
	}
	
	XMLElement * faultreport = doc.FirstChildElement("FaultReport");

	if( !faultreport )
	{
		//��ʽ����
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
			//���벻֧��
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

