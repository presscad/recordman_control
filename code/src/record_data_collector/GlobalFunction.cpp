#pragma warning(disable : 4275)

#include "GlobalFunction.h"

char* Record_Strrev( char* szT )
{
	if(!szT)// 处理传入的空串.
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
