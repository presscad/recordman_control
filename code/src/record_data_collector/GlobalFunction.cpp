#pragma warning(disable : 4275)

#include "GlobalFunction.h"

char* recordman_strrev( char* szT )
{
	if(!szT)
	{
		return "";
	}

	int i = strlen(szT);
	int t = !(i%2)?1:0;
	for(int j = i-1,k = 0; j > (i/2 -t); j--)
	{
		char ch = szT[j];
		szT[j]  = szT[k];
		szT[k++] = ch;
	}
	return szT;	
}

char* recordman_itoa(int value, char* str, int radix)
{
	int  rem = 0;
	int  pos = 0;

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
					ch = 'A';
					break;

				case 11:
					ch = 'B';
					break;

				case 12:
					ch = 'C';
					break;

				case 13:
					ch = 'D';
					break;

				case 14:
					ch = 'E';
					break;

				case 15:
					ch = 'F';
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

	return recordman_strrev(str);	
}

std::string get_dfu_error_msg_string(int nErrorNum)
{
	string strErrorMsg = "";

	switch (nErrorNum)
	{
	case 0x0001:
		strErrorMsg = "无效命令";
		break;

	case 0x0002:
		strErrorMsg = "通讯忙";
		break;

	case 0x0003:
		strErrorMsg = "事务错误";
		break;

	case 0x0004:
		strErrorMsg = "不支持的协议";
		break;

	case 0x0005:
		strErrorMsg = "不支持的功能";
		break;

	case 0x0010:
		strErrorMsg = "无效数据";
		break;

	case 0x0011:
		strErrorMsg = "参数错误";
		break;

	case 0x0012:
		strErrorMsg = "多帧传输错误";
		break;

	case 0x0013:
		strErrorMsg = "请求的操作失败";
		break;

	case 0x0021:
		strErrorMsg = "数据准备失败";
		break;

	case 0x0022:
		strErrorMsg = "后续命令错误";
		break;

	case 0x0023:
		strErrorMsg = "帧序号错误";
		break;

	case 0x0024:
		strErrorMsg = "数据传输提前结束";
		break;

	case 0x0025:
		strErrorMsg = "数据已经完成";
		break;

	case 0xFFFF:
		strErrorMsg = "自定义错误";
		break;
	}

	return strErrorMsg;
}

void convert_btol_int32(XJHANDLE pChar, int& nVal)
{
	memcpy(&nVal, pChar, 4);
	RECORD_SWAP_32(nVal);
}

void convert_btol_float32(XJHANDLE pChar, float& fVal)
{
	memcpy(&fVal, pChar, 4);
	RECORD_SWAP_32(fVal);
}

void convert_btol_uint32(XJHANDLE pChar, UINT& nVal)
{
	memcpy(&nVal, pChar, 4);
	RECORD_SWAP_32(nVal);
}

void convert_btol_uint16(XJHANDLE pChar, uint16& uVal)
{
	memcpy(&uVal, pChar, 2);
	RECORD_SWAP_16(uVal);
}

string get_int32_version(XJHANDLE pChar)
{
	uint16 uMsgVer(0);
	UINT uMainVer(0);
	UINT uSeedVer(0);
	UINT uCorrectVer(0);
	char chVersion[512] = "";
	bzero(chVersion, sizeof(chVersion));

	convert_btol_uint16(pChar, uMsgVer);

	uCorrectVer = uMsgVer & 0x3F;
	uSeedVer = (uMsgVer & 0xFC0) >> 6;
	uMainVer = (uMsgVer & 0xF000) >> 12;

	sprintf(chVersion, "%d.%d.%d", 
		uMainVer, uSeedVer, uCorrectVer);

	return chVersion;
}

std::string get_dfu_crc(XJHANDLE pChar)
{
	BYTE bCrc[2];
	char temp[3] = "" ,temp1[2]="";
	string strCrc = "";
	strCrc.resize(4);
	char* pcrc = (char*)&strCrc[0];
	
	memcpy(&bCrc, pChar, 2);

	for (int i = 0; i < 2; i++)
	{
		if (bCrc[i] < 16)
		{
			recordman_itoa(bCrc[i], temp1, 16);
			temp[1] = temp1[0];
			temp[0] = '0';
			memset(temp1, 0, 1);
		}
		else
		{
			recordman_itoa(bCrc[i], temp, 16);
		}

		memcpy(pcrc, temp, 2);
		pcrc += 2;
	}

	return strCrc;
}

std::string format_dfu_msg_time(UINT uMsgtime, int nTimetype /*= CCommonTime::STTP19Time*/)
{
	time_t tTime = uMsgtime;
	CCommonTime commontime(tTime);

	return commontime.GetTimeString(nTimetype);
}

