#include "DfuMsgParser.h"

CDfuMsgParser::CDfuMsgParser()
{
	m_pMsg=NULL ;
	m_pMsgHdr=NULL ;
}

CDfuMsgParser::CDfuMsgParser(RECORD_DFU_MSG *pMsg)
{
	m_pMsg = pMsg ;
	m_pMsgHdr = &pMsg->DfuMsgHdr;
}


CDfuMsgParser::~CDfuMsgParser()
{

}

RECORD_DFU_MSG_HEADER* CDfuMsgParser::GetMsgHdr()
{
	if (NULL == m_pMsgHdr)
	{
		return NULL;
	}

	return m_pMsgHdr ;
}

void CDfuMsgParser::SetMsgBody(BYTE* pMsgBody)
{
	memcpy(m_pMsg->MsgBody, pMsgBody, (GetMsgLength() - 8));
}

BYTE* CDfuMsgParser::GetMsgBody()
{
	return m_pMsg->MsgBody ;
}

void CDfuMsgParser::Attach(RECORD_DFU_MSG *pMsg)
{
	m_pMsg = pMsg;
	m_pMsgHdr = &pMsg->DfuMsgHdr;
}

void CDfuMsgParser::SetMsgStartMask()
{
	m_pMsgHdr->byteStartMask[0] = RECORD_COMMU_CHAR_START_VAR;
	m_pMsgHdr->byteStartMask[1] = RECORD_COMMU_CHAR_START_VAR;
}

bool CDfuMsgParser::CheckStartMask()
{
	if (NULL == m_pMsgHdr)
	{
		return false;
	}

	if ((RECORD_COMMU_CHAR_START_VAR != m_pMsgHdr->byteStartMask[0]) || 
		(RECORD_COMMU_CHAR_START_VAR != m_pMsgHdr->byteStartMask[1]))
	{
		return false;
	}

	return true;
}

UINT CDfuMsgParser::GetMsgTransMask()
{
    return ArrayToInt((char*)m_pMsgHdr->byteTransMark, 2);
}

void CDfuMsgParser::SetMsgTransMask(UINT nTransMark)
{
	if(nTransMark > 99)
	{
		return;
	}

	SWAP_16(nTransMark);
	memcpy(m_pMsgHdr->byteTransMark, &nTransMark, 2);
}

int CDfuMsgParser::GetMsgProtocolMask()
{
	return 1;
}

void CDfuMsgParser::SetMsgProtocolMask(int nProtocol)
{
	switch (nProtocol)
	{
	case DEFINE_COMMU_PROTOCOL_RECORD_SELF:
		{
			m_pMsgHdr->byteProtocolMark[0] = RECORD_COMMU_CHAR_PROTOCOL_START_VAR;
			m_pMsgHdr->byteProtocolMark[1] = RECORD_COMMU_CHAR_PROTOCOL_END_VAR;
		}
		break;
	}
}

bool CDfuMsgParser::CheckMsgProtocolMask()
{
	if (NULL == m_pMsgHdr)
	{
		return false;
	}

	if ((RECORD_COMMU_CHAR_PROTOCOL_START_VAR != m_pMsgHdr->byteProtocolMark[0]) || 
		(0x00 != m_pMsgHdr->byteProtocolMark[1]))
	{
		printf("recv wrong protocol code msg！\n");
	}

	return true;
}

void CDfuMsgParser::SetMsgReserve()
{
	m_pMsgHdr->byteReserve[1] = '0';
}

UINT CDfuMsgParser::GetMsgLength()
{
	int nMsgLeng = 0;
	nMsgLeng = m_pMsgHdr->byteMsgLen[0] * 256 + m_pMsgHdr->byteMsgLen[1];
	return nMsgLeng;
}

void CDfuMsgParser::SetMsgLength(UINT nLeng)
{
	SWAP_16(nLeng);
	memcpy(m_pMsgHdr->byteMsgLen, &nLeng, 2);
}

void CDfuMsgParser::SetMsgFunMask(int nFunMask)
{
	SWAP_16(nFunMask);
	memcpy(m_pMsgHdr->byteFunMask, &nFunMask, 2);
}

bool CDfuMsgParser::GetMsgEndFlag()
{
	if(m_pMsgHdr->byteFrameSeq[0] == '0')
	{
		return true;
	}
	else if(m_pMsgHdr->byteFrameSeq[0] == '1')
	{
		return false;
	}
	else
	{
		return false;
	}
}

void CDfuMsgParser::SetMsgEndFlag(bool bEndFlag)
{
	if(bEndFlag)
	{
		m_pMsgHdr->byteFrameSeq[0] = 0x00;
	}
	else
	{
		m_pMsgHdr->byteFrameSeq[0] = 0x01;
	}
}

//************************************
// 函 数 名： GetMsgCommand
// 功能概要： get command mask
// 访问权限： public 
// 返 回 值： int 
//************************************
int CDfuMsgParser::GetMsgCommand()
{
	int nMsgCommand = -1;

	nMsgCommand = m_pMsgHdr->byteCommandMask[0] + m_pMsgHdr->byteCommandMask[1];

	return nMsgCommand;
}

//************************************
// 函 数 名： SetMsgCommand
// 功能概要： set command mask
// 访问权限： public 
// 返 回 值： void 
// 参    数： int nMsgCommand 
//************************************
void CDfuMsgParser::SetMsgCommand(int nMsgCommand)
{
	int nCommand = nMsgCommand;
	SWAP_16(nCommand);
	memcpy(m_pMsgHdr->byteCommandMask, &nCommand, 2);
}

//************************************
// 函 数 名： SetEndMask
// 功能概要： set msg end mask，support record internal protocol
// 访问权限： public 
// 返 回 值： void 
// 参    数： int nProtocol protocol id，default DEFINE_COMMU_PROTOCOL_RECORD_SELF
//************************************
void CDfuMsgParser::SetEndMask(int nProtocol /*= DEFINE_COMMU_PROTOCOL_RECORD_SELF*/)
{
	if (NULL == m_pMsgHdr)
	{
		return;
	}

	switch (nProtocol)
	{
	case DEFINE_COMMU_PROTOCOL_RECORD_SELF:
		{
			m_pMsgHdr->byteEndMask[0] = RECORD_COMMU_CHAR_END_VAR;
			m_pMsgHdr->byteEndMask[1] = RECORD_COMMU_CHAR_END_VAR;
		}
		break;
	}
}

//************************************
// 函 数 名： CheckEndMask
// 功能概要： check end mask from recved msg
// 访问权限： public 
// 返 回 值： bool true：right false：wrong
//************************************
bool CDfuMsgParser::CheckEndMask()
{
	if (NULL == m_pMsgHdr)
	{
		return false;
	}

	if ((RECORD_COMMU_CHAR_END_VAR != m_pMsgHdr->byteEndMask[0]) || 
		(RECORD_COMMU_CHAR_END_VAR != m_pMsgHdr->byteEndMask[1]))
	{
		return false;
	}

	return true;
}

double CDfuMsgParser::ArrayToDouble(char *szByte, int iLength)
{
	char * pByte;
	char strTemp[16];
	memcpy(strTemp,szByte,iLength);
	strTemp[iLength]='\0';
	return strtod(strTemp,&pByte);
}

UINT CDfuMsgParser::ArrayToHex(char *byte, int nCount)
{
	if(nCount>8)
		return 0;
	else
	{
		char ptempbyte[9];
		memset(ptempbyte,0,9);
		memcpy(ptempbyte,byte,nCount);
		return strtol(ptempbyte,NULL,16);
	}
}

int CDfuMsgParser::ArrayToInt(char *byte, int nCount)
{
	
	if(nCount>16)
		return 0;
	else
	{
		char ptempbyte[17];
		memset(ptempbyte,0,17);
		memcpy(ptempbyte,byte,nCount);
		return strtol(ptempbyte,NULL,10);
	}
}

void CDfuMsgParser::IntToArray(UINT l, char *ch, int length)
{
	if(length>16)
		return;
	else
	{
		//要求的填写格式为数字左靠，不足部分用空格填充如（"89  "）
		char strFormat[6],strTemp[16];
		strcpy(strFormat,"%-");
//		sprintf()
//		itoa(length,strFormat+2,10);
		Itoa(length,strFormat+2,10);//用私有的
		strcpy(strFormat+3+(length/10),"d");
		//填写格式化字符串的规则
		sprintf(strTemp,strFormat,l);
		memcpy(ch,strTemp,_min( length, 16 ));
	};
}

void CDfuMsgParser::DoubleToArray(double fDouble, char *szByte, int iLength)
{
	char strTemp[16];
	sprintf(strTemp,"%f",fDouble);
	memcpy(szByte,strTemp,iLength);
}

char* CDfuMsgParser::Itoa(int value, char *str, int radix)
{
    int  rem = 0;
    int  pos = 0;
	//char ch  = '!';
	do
	{
		char ch  = '!';
		rem = value % radix;
		value /= radix;
		if(16 == radix)
		{
			if(rem >= 10 && rem <= 15)
			{
				switch(rem)
				{
				case 10:
					ch = 'a';
					break;
				case 11:
					ch = 'b';
					break;
				case 12:
					ch = 'c';
					break;
				case 13:
					ch = 'd';
					break;
				case 14:
					ch = 'e';
					break;
				case 15:
					ch = 'f';
					break;
				}
			}
		}
		if('!' == ch)
		{
			str[pos++] = (char) (rem + 0x30);
		}
		else
		{
			str[pos++] = ch;
		}
	}while( value != 0);
    str[pos] = '\0';
	return Strrev(str);
}

char* CDfuMsgParser::Strrev(char *szT)
{
	if(!szT)                     // 处理传入的空串.
		return "";
	int i = strlen(szT);
	int t = !(i%2)?1:0;          // 检查串长度
	for(int j = i-1,k=0; j> (i/2 -t); j--)
	{
		char ch = szT[j];
		szT[j]  = szT[k];
		szT[k++] = ch;
	}
	return szT;
}

UINT CDfuMsgParser::EndianConvertLToB(UINT InputNum)
{
	UCHAR *p = (UCHAR*)&InputNum;
	return(((UINT)*p<<24) + ((UINT)*(p+1)<<16) + ((UINT)*(p+2)<<8) + (UINT)*(p+3));
}

UINT CDfuMsgParser::EndianConvertBToL(UINT InputNum)
{
	UCHAR *p = (UCHAR*)&InputNum;
	return(((UINT)*p) + ((UINT)*(p+1)<<8) + ((UINT)*(p+2)<<16) + (UINT)*(p+3)<<24);
}
