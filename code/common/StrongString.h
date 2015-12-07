//
// StrongString.h
//
// This is a string operator  class
//
// Copyright (C) 2006
// Version 1.0
//


#ifndef StrongString_h__
#define StrongString_h__

#include "sys_define.h"

using namespace std;

class CStrongString : public std::string  
{	
public:

	CStrongString(const CStrongString& string1);
	CStrongString(const char *ch);
	CStrongString(const char ch);
	
	CStrongString();

	int Delete( int nIndex, int nCount = 1 );

	int Insert( int nIndex, const char * pstr );
	int Insert( int nIndex, char ch );

	int Remove( char ch );

	void MakeReverse( );

	int Find( char ch ) const;
	int Find( const char * lpszSub ) const;
	int Find( char ch, int nStart ) const;
	int Find( const char * pstr, int nStart ) const;

	int ReverseFind( char ch ) const;

	int FindOneOf( const char * lpszCharSet ) const;

	int Format(const char* pstrFormat, ... );

	int GetLength() const;

	CStrongString Left(int nCount) const;
	CStrongString Right(int nCount) const;

	CStrongString Mid(int nFirst) const;
	CStrongString Mid( int nFirst, int nCount) const;

	CStrongString& operator=(const std::string str);
	CStrongString& operator=(char ch);
	CStrongString& operator =( const char * lpsz );
	CStrongString& operator=(const CStrongString& str);
	void MakeUpper();
	void MakeLower();
	void MakeLower(CStrongString & str);
	void MakeUpper(CStrongString & str);

	bool IsEmpty( ) const;

	void Empty( );

	char GetAt( int nIndex ) const;

	char operator []( int nIndex ) const;

	void SetAt( int nIndex, char ch );

	operator const char * ( ) const;

	int Compare( const char * lpsz ) const;
	int Compare( const CStrongString& string1 ) const;
	int CompareNoCase( const char * lpsz ) const;
	int CompareNoCase( const CStrongString& string1 ) const;

	void TrimLeft( );
	void TrimLeft( char chTarget );
	void TrimLeft( char * pchar);
	
	void TrimRight( );
	void TrimRight( char chTarget );
	void TrimRight( char * pchar );

	void Replace( char chOld, char chNew );
	void Replace(const char* chOld,const char* chNew );
	
	char * GetBuffer( int nMinBufLength );

	void ReleaseBuffer( int nNewLength = -1 );

	friend CStrongString operator + (const CStrongString& string1, const CStrongString& string2)
	{
		CStrongString str;
		str.append(string1);
		str.append(string2);
		return str;
	}
	friend CStrongString operator + ( const CStrongString& string1, char ch )
	{
		CStrongString str;
		str.append(string1);
		str.insert(str.end(),ch);
		return str;
	}
	friend CStrongString operator + ( const CStrongString& string1, char* ch )
	{
		CStrongString str;
		str.append(string1);
		str.append(ch);
		return str;
	}
};
#endif // StrongString_h__

