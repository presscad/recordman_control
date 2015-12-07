// StrongString.cpp: implementation of the CStrongString class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4275)
#pragma warning(disable : 4267)

#include "StrongString.h"

//////////////////////////////////////////////////////////////////////////
//                  Class  CXJString   ʵ��                                    
//////////////////////////////////////////////////////////////////////////

CStrongString::CStrongString(const CStrongString& string1)
{
	this->append(string1);
}
CStrongString::CStrongString(const char *ch)
{
	*this=ch;
//	this->append(ch);
}
CStrongString::CStrongString(const char ch)
{
	*this += ch;
}

CStrongString::CStrongString()
{}

/*********************************************************

   Deletes a character or characters from a string.

*********************************************************/
int CStrongString::Delete( int nIndex, int nCount)
{
	this->erase(nIndex,nCount);
	return this->GetLength();
}
/*********************************************************

   Inserts a single character or a substring at the given 
   index within the string.

*********************************************************/
int CStrongString::Insert( int nIndex, const char * pstr )
{
	this->insert(nIndex,pstr);
	return this->GetLength();	
}
int CStrongString::Insert( int nIndex, char ch )
{	
	CStrongString strTmp(ch);
	this->insert(nIndex,strTmp);
	strTmp.Empty();
	return this->GetLength();
}
/*********************************************************

   Removes indicated characters from a string.

*********************************************************/
int CStrongString::Remove( char ch )
{
	CStrongString::iterator iter;
	int count = 0;
	for(iter = this->begin(); iter != this->end();iter ++)
	{
		if(*iter == ch)
		{
			this->erase(iter);count++;
		}
	}
	return count;
}
/*********************************************************

   Reverses the characters in this string.

*********************************************************/
void CStrongString::MakeReverse( )
{
	CStrongString strTmp;
	CStrongString::iterator iter;
	iter=this->end();
	iter--;
	for(; iter != this->begin(); iter--)
	{
		strTmp += *iter;
	}
	strTmp += *iter;
	*this = strTmp;
	strTmp.Empty();
}
/*********************************************************

   Finds a character or substring inside a larger string.

*********************************************************/
int CStrongString::Find( char ch ) const
{
	return this->find(ch);
}
int CStrongString::Find( const char * lpszSub ) const
{
	return this->find(lpszSub);
}
int CStrongString::Find( char ch, int nStart ) const
{
	return this->find(ch,nStart);
}
int CStrongString::Find( const char * pstr, int nStart ) const
{
	return this->find(pstr,nStart);
}
/*********************************************************

   Finds a character inside a larger string; starts from the end.

*********************************************************/
int CStrongString::ReverseFind( char ch ) const
{
	return this->find_last_of(ch);	
}
/*********************************************************

   Finds the first matching character from a set.

*********************************************************/
int CStrongString::FindOneOf( const char * lpszCharSet ) const
{
	return this->find_first_of(lpszCharSet);
}
/*********************************************************

   Format the string as sprintf does.

*********************************************************/
int CStrongString::Format(const char* pstrFormat, ... )
{///����������֧��ANSI��׼�ַ��� '%[flags] [width] [.precision] [{h | l | I64 | L}]type'	 	
	assert(pstrFormat!=NULL);
	va_list argList;
	va_start(argList,pstrFormat); 
	int nMaxLen = 0;
	for (const char * p = pstrFormat; *p != '\0';p++ )
	{
		if (*p != '%' || *(++p) == '%')
		{// �������'%'��ֱ���ۼƳ��ȣ������'%%'Ҳʹ���ȼ�1
			nMaxLen += 1;
			continue;
		}
		int nItemLen = 0; //��������ÿ�������ĳ���
		int nWidth = 0; //��������ÿһ��Ŀ��
		for (; *p != '\0'; p ++)
		{
			if (*p == '#')
				nMaxLen += 2;   // ���� '0x'
			else if (*p == '*')
				nWidth = va_arg(argList, int);  //�磺'%5f' �е�5
			else if (*p == '-' || *p == '+' || *p == '0'|| *p == ' ')
				;  //���Ը÷���
			else // ���Ǳ�־�ַ����˳�ѭ��
				break;
		}
		if (nWidth == 0)
		{ //��ȡ���
			nWidth = atoi(p);
			for (; *p != '\0' && isdigit(*p); p ++)
				;
		}
		assert(nWidth >= 0);//��Ч���
		int nPrecision = 0; //����λ��
		if (*p == '.')
		{
			p++;// ���� '.'�ַ� (���.����)			
			if (*p == '*')
			{ //�в�������
				nPrecision = va_arg(argList, int);
				p ++;// ȡ�þ��ȣ������ַ�
			}
			else
			{ //�ڸ�ʽ�����п��
				nPrecision = atoi(p);
				for (; *p != '\0' && isdigit(*p);	p ++)
					;
			}
			assert(nPrecision >= 0);//��Ч���
		}
		switch (*p)
		{
		case 'h':     //short int ��
			p ++;
			break;
		case 'l':	 //long double ��
			p ++;
			break;
		case 'F':	 //��ָ��
		case 'N':	//Զָ��	
		case 'L':	//long double ��
			p++;
			break;
		}
		switch (*p)
		{
		case 'c':   //// �����ַ�
		case 'C':
			nItemLen = 2;
			va_arg(argList,int);
			break;
		case 's':	//// �ַ���
		case 'S':
			nItemLen = strlen(va_arg(argList, const char*));
			nItemLen = ((1) > (nItemLen)) ? (1) : (nItemLen);//����ǿմ���ʹ��1 ������'\0'
			break;
		}
		if (nItemLen != 0)		
		{
			nItemLen = ((nItemLen) > (nWidth)) ? (nItemLen) : (nWidth);//ʹ�ô���
			if (nPrecision != 0)
				nItemLen = ((nItemLen) < (nPrecision)) ? (nItemLen) : (nPrecision);
		}
		else
		{
			switch (*p)
			{
			case 'd':    //�����Ĵ���
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				va_arg(argList, int);
				nItemLen = 32;  //���ֽ�
				nItemLen = ((nItemLen) > (nWidth+nPrecision)) ? (nItemLen) : (nWidth+nPrecision);//ʹ�ô���
				break;
			case 'e':	//������
			case 'f':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 32;//���ֽ�
				nItemLen = ((nItemLen) > (nWidth+nPrecision)) ? (nItemLen) : (nWidth+nPrecision);//ʹ�ô���;
				break;
			case 'p':	//ָ��
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = ((nItemLen) > (nWidth+nPrecision)) ? (nItemLen) : (nWidth+nPrecision);//ʹ�ô���;
				break;
			case 'n':
				va_arg(argList, int*); //ָ��������ָ��,��BorlanderC++3.1�⺯��P352
				break;
			default:
				assert(false);  //���ܴ���ĸ�ʽ����������
			}
		}
		nMaxLen += nItemLen;//�Ѹ���ĳ����ۼ�
	}
	va_end(argList);
	va_start(argList, pstrFormat);  // ���¿�ʼ��ȡ����
	char* ch = new char[nMaxLen+1]; //�����ڴ�
	vsprintf(ch, pstrFormat, argList);
	//assert(vsprintf(ch, pstrFormat, argList) <= nMaxLen);
    //�������ԭ����������
	this->Empty();

	this->append(ch); //�ӵ�string��β��
	delete[] ch;      //�ͷ��ڴ�
	va_end(argList);
	return nMaxLen;
}
/*********************************************************

   Returns the number of characters in a CXJString object. For 
   multibyte characters, counts each 8-bit character; that is, 
   a lead and trail byte in one multibyte character are counted 
   as two characters.

*********************************************************/
int CStrongString::GetLength() const
{
	return this->length();
}
/*********************************************************

   Extracts the left part of a string (like the Basic LEFT$ 
   function).

*********************************************************/
CStrongString CStrongString::Left(int nCount) const
{
	if (nCount <=0) 
		return CStrongString("");	
	CStrongString strTmp;
	strTmp = this->substr(0,nCount);
	return strTmp;
}
/*********************************************************

   Extracts the right part of a string (like the Basic RIGHT$ 
   function).

*********************************************************/
CStrongString CStrongString::Right(int nCount) const
{
	if (nCount <=0) 
		return CStrongString("");
	CStrongString strTmp;
	if (nCount > GetLength())
		strTmp = this->substr(0);
	else
		strTmp = this->substr(GetLength()-nCount);
	return strTmp;
}
/*********************************************************

   Extracts the middle part of a string (like the Basic MID$ 
   function).

*********************************************************/
CStrongString CStrongString::Mid(int nFirst) const
{
	CStrongString strTmp;
	if (nFirst >= GetLength())
		return CStrongString("");
	if (nFirst <= 0)
		strTmp = this->substr(0);
	else
		strTmp = this->substr(nFirst);
	return strTmp;		
}
CStrongString CStrongString::Mid( int nFirst, int nCount) const
{
	if (nCount <= 0) 
		return CStrongString("");
	if (nFirst >= GetLength())
		return CStrongString("");
	CStrongString strTmp;
	if (nFirst <= 0)
		strTmp = this->substr(0,nCount);
	else
		strTmp = this->substr(nFirst,nCount);
	return strTmp;				
}
/*********************************************************

   Assigns a new value to a CXJString object.

*********************************************************/
CStrongString& CStrongString::operator=(const std::string str)
{
	if (this->compare(str) == 0) return *this;
	this->assign(str);
	return *this;
}
CStrongString& CStrongString::operator=(char ch)
{
	this->Empty();
	this->insert(this->begin(),ch);
	return *this;
}
CStrongString& CStrongString::operator =( const char * lpsz )
{
	this->Empty();
	this->append(lpsz);
	return *this;
}

CStrongString& CStrongString::operator=( const CStrongString& str )
{
	if (this->compare(str) == 0) return *this;
	this->Empty();
	this->append(str.c_str());
	return *this;
}
/*********************************************************

   Converts all the characters in this string to uppercase 
   characters.

*********************************************************/
void CStrongString::MakeUpper()
{
/*	std::transform(this->begin (),
		this->end (),this->begin (),
		toupper);*/

	char * pchar = (char*)this->c_str();

	for (int i =0; i< this->length(); i++)
	{
		*(pchar+i) = toupper(*(pchar+i));
	}
}

void CStrongString::MakeUpper(CStrongString & str)
{
	char * pchar = str.GetBuffer(str.GetLength());

	for (int i =0; i< str.GetLength(); i++)
	{
		*(pchar+i) = toupper(*(pchar+i));
	}

	str.ReleaseBuffer();

}

/*********************************************************

   Converts all the characters in this string to lowercase 
   characters.

*********************************************************/
void CStrongString::MakeLower()
{
/*	std::transform(this->begin (),
		this->end (),this->begin (),
		tolower);*/
	char * pchar = (char*)this->c_str();

	for (int i =0; i< this->length(); i++)
	{
		*(pchar+i) = tolower(*(pchar+i));
	}
}

void CStrongString::MakeLower(CStrongString & str)
{
	char * pchar = str.GetBuffer(str.GetLength());

	for (int i =0; i< str.GetLength(); i++)
	{
		*(pchar+i) = tolower(*(pchar+i));
	}

	str.ReleaseBuffer();
}

/*********************************************************

   Tests whether a CXJString object contains no characters

*********************************************************/
bool CStrongString::IsEmpty( ) const
{
	return this->empty(); 
}
/*********************************************************

   Forces a string to have 0 length

*********************************************************/
void CStrongString::Empty( )
{
	this->erase(this->begin(),this->end());
}
/*********************************************************

   Returns the character at a given position.

*********************************************************/
char CStrongString::GetAt( int nIndex ) const
{
	return this->at(nIndex);
}
/*********************************************************

   Returns the character at a given position �� operator 
   substitution for GetAt.

*********************************************************/
char CStrongString::operator []( int nIndex ) const
{
	return this->at(nIndex);
}
/*********************************************************

   Sets a character at a given position.

*********************************************************/
void CStrongString::SetAt( int nIndex, char ch )
{
	this->at(nIndex) = ch;
}

/*********************************************************

   Directly accesses characters stored in a CXJString object 
   as a C-style string.

*********************************************************/
CStrongString::operator const char *( ) const
{
	return this->c_str();
}
/*********************************************************

   Compares two strings (case sensitive).

*********************************************************/
int CStrongString::Compare( const char * lpsz ) const
{
	CStrongString str;
	str.append(lpsz);
	return this->compare(str);
}
int CStrongString::Compare( const CStrongString& string1 ) const
{
	return this->compare(string1);
}
/*********************************************************

   Compares two strings (case insensitive).

*********************************************************/
int CStrongString::CompareNoCase( const char * lpsz ) const
{
	CStrongString str,strThis,strSource;
	str.append(lpsz);

	strncpy(strSource.GetBuffer(str.GetLength()),
		    str.c_str(),str.GetLength());
	strSource.ReleaseBuffer();

	strncpy(strThis.GetBuffer(this->GetLength()),
		   this->c_str(),this->length());
	strThis.ReleaseBuffer();

	strSource.MakeLower(strSource);
	strThis.MakeLower(strThis);
	return strThis.compare(strSource);
}

int CStrongString::CompareNoCase( const CStrongString& string1 ) const
{
	CStrongString str,strThis;
    
	strncpy(str.GetBuffer(string1.size()),
		   string1.c_str(),string1.length());
	str.ReleaseBuffer();

	strncpy(strThis.GetBuffer(this->GetLength()),
		   this->c_str(),this->length());
	strThis.ReleaseBuffer();

	str.MakeLower(str);
	strThis.MakeLower(strThis);
	return strThis.compare(str);
}
/*********************************************************

   Trim trailing whitespace characters from the string.

*********************************************************/
void CStrongString::TrimRight( )
{
	TrimRight (' ');
}
/*********************************************************

   Trim leading whitespace characters from the string.

*********************************************************/
void CStrongString::TrimLeft( )
{   
	TrimLeft(' ');
}
/*********************************************************

   Trim leading target characters from the string.

*********************************************************/
void CStrongString::TrimLeft( char chTarget )
{
	std::string::size_type pos;
	pos = this->find_first_not_of(chTarget);
	if ((pos == npos)||(pos <= 0)) 
		return; 
	this->erase(this->begin(),this->begin()+pos);		
}
/*********************************************************

   Trim leading target string from the string.

*********************************************************/
void CStrongString::TrimLeft( char * pchar)
{
	std::string::size_type pos;
	pos = this->find_first_not_of(pchar);
	if ((pos == npos)||(pos <= 0)) 
		return; 
	this->erase(this->begin(),this->begin()+pos);	
}
/*********************************************************

   Trim trailing target characters from the string.

*********************************************************/
void CStrongString::TrimRight( char chTarget )
{
	std::string::size_type pos;
	pos = this->find_last_not_of(chTarget);
	++pos;
	if ((pos == npos)||(pos >= this->GetLength()))
		return; 
	this->erase(this->begin()+pos,this->end());		
}
/*********************************************************

   Trim trailing target string from the string.

*********************************************************/
void CStrongString::TrimRight( char * pchar )
{
	/*std::string::size_type pos;
	pos = this->find_last_not_of(pchar);
	++pos;
	if (pos == this->GetLength())
		return; 
	this->erase(this->begin()+pos,this->end());	*/

	int nLen = strlen(pchar);
	if(nLen <=0 )
		return;
    if( strcmp((const char*)Right(nLen),pchar) == 0)
	{
		this->erase( this->begin()+(this->GetLength() - nLen),this->end());
	}
}
/*********************************************************

   Replaces indicated characters with other characters.

*********************************************************/
void CStrongString::Replace( char chOld, char chNew )
{
	for(int i=0;i<this->GetLength();i++)
	{
		if (this->at(i) == chOld)
			this->at(i) = chNew;
	}		
}
void CStrongString::Replace(const char* chOld,const char* chNew )
{
	int index = this->find(chOld);
	while (index > -1)
	{
		this->erase(index,strlen(chOld));
		this->insert(index,chNew);
		index = this->find(chOld);
	}
}		
/*********************************************************

   Returns a pointer to the characters in the CXJString

*********************************************************/
char * CStrongString::GetBuffer( int nMinBufLength )
{
	if(nMinBufLength != 0)
		this->resize(nMinBufLength);

	return (char*)this->c_str();
}
/*********************************************************

   Releases control of the buffer returned by GetBuffer.

*********************************************************/
void CStrongString::ReleaseBuffer( int nNewLength)
{
	this->TrimRight('\0');
}
