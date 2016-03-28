#pragma warning(disable : 4275)

#include "GlobalFunction.h"

char* Record_Strrev( char* szT )
{
	if(!szT)// ������Ŀմ�.
		return "";

	int i = strlen(szT);
	int t = !(i%2)?1:0;          // ��鴮����
	for(int j = i-1,k=0; j> (i/2 -t); j--)
	{
		char ch = szT[j];
		szT[j]  = szT[k];
		szT[k++] = ch;
	}
	return szT;	
}

char* Record_Itoa(int value, char* str, int radix)
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

	return Record_Strrev(str);	
}

std::string GetDfuErrorMsgString(int nErrorNum)
{
	string strErrorMsg = "";

	switch (nErrorNum)
	{
	case 0x0001:
		strErrorMsg = "��Ч����";
		break;

	case 0x0002:
		strErrorMsg = "ͨѶæ";
		break;

	case 0x0003:
		strErrorMsg = "�������";
		break;

	case 0x0004:
		strErrorMsg = "��֧�ֵ�Э��";
		break;

	case 0x0005:
		strErrorMsg = "��֧�ֵĹ���";
		break;

	case 0x0010:
		strErrorMsg = "��Ч����";
		break;

	case 0x0011:
		strErrorMsg = "��������";
		break;

	case 0x0012:
		strErrorMsg = "��֡�������";
		break;

	case 0x0013:
		strErrorMsg = "����Ĳ���ʧ��";
		break;

	case 0x0021:
		strErrorMsg = "����׼��ʧ��";
		break;

	case 0x0022:
		strErrorMsg = "�����������";
		break;

	case 0x0023:
		strErrorMsg = "֡��Ŵ���";
		break;

	case 0x0024:
		strErrorMsg = "���ݴ�����ǰ����";
		break;

	case 0x0025:
		strErrorMsg = "�����Ѿ����";
		break;

	case 0xFFFF:
		strErrorMsg = "�Զ������";
		break;
	}

	return strErrorMsg;
}

void ConvertInt32BigedianToL(XJHANDLE pChar, int& nVal)
{
	memcpy(&nVal, pChar, 4);
	RECORD_SWAP_32(nVal);
}

void ConvertFloat32BigedianToL(XJHANDLE pChar, float& fVal)
{
	memcpy(&fVal, pChar, 4);
	RECORD_SWAP_32(fVal);
}

void ConvertUint32BigedianToL(XJHANDLE pChar, UINT& nVal)
{
	memcpy(&nVal, pChar, 4);
	RECORD_SWAP_32(nVal);
}

string GetInt32VersionInfo(int nVersion)
{
	int nMainVer(0);
	int nSeedVer(0);
	int nCorrectVer(0);
	char chVersion[512] = "";
	bzero(chVersion, sizeof(chVersion));

	nCorrectVer = nVersion & 0x3F;
	nSeedVer = nVersion & 0xFC0;
	nMainVer = nVersion & 0xF000;

	sprintf(chVersion, "%d.%d.%d", 
		nMainVer, nSeedVer, nCorrectVer);

	return chVersion;
}
