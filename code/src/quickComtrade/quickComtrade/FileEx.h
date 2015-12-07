// FileEx.h: interface for the CFileEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FILEEX_H__6E96EBE5_0561_4EBF_9657_A9691815A6FB__INCLUDED_)
#define AFX_FILEEX_H__6E96EBE5_0561_4EBF_9657_A9691815A6FB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "constant.h"

// #include "../../common/define.h"
// #include "../../common/XJString.h"
// #include "../../common/XJFile.h"
//##ModelId=489955E2009C
class CFileEx
{
public:
	//##ModelId=489955E200AB
	int   ReadString(CStrongString& strLine);   
	//##ModelId=489955E200AD
	CFileEx();
	//##ModelId=489955E200AE
	~CFileEx();
private:
    
	//文件指针
	//##ModelId=489955E200BB
	FILE * pfile ;

	//##ModelId=489955E200BC
	char   m_chError[FILE_ERROR_MAX_LEN];

	//##ModelId=489955E200BD
	char   m_chName[FILE_NAME_MAX_LEN];

protected:

public:

	//##ModelId=489955E200FA
	enum OpenFlags{
		//file mode
		//##ModelId=489955E200FC
        modeCreate         =       0  ,         //0000 0000
		//##ModelId=489955E200FD
        modeRead           =       1  ,         //0000 0001
		//##ModelId=489955E200FE
        modeWrite          =       2  ,         //0000 0010
		//##ModelId=489955E20109
        modeReadWrite      =       4  ,         //0000 0100
		//##ModelId=489955E2010A
        modeNoTruncate     =       8  ,         //0000 1000
		//##ModelId=489955E2010B
        shareDenyNone      =       16 ,         //0001 0000
		//##ModelId=489955E2010C
        shareDenyWrite     =       32 ,         //0010 0000
		//##ModelId=489955E20119
        shareDenyRead      =       64 ,         //0100 0000
		//##ModelId=489955E2011A
        shareExclusive     =       128,         //1000 0000
	};

	//##ModelId=489955E2011B
	enum PosFlags{
		//file position 
		//##ModelId=489955E20129
		begin                  =   0,          //文件开始
		//##ModelId=489955E2012A
        current                =   1,          //文件当前位置
		//##ModelId=489955E20138
        end                    =   2,         //文件尾部
	};

	/* Input/Output */

	//打开文件
	//##ModelId=489955E200BE
	bool Open(const char *, UINT nOpenFlags);

	//读文件
	//##ModelId=489955E200CD
	UINT Read(void * lpbuf, UINT nCount);

	//写文件
	//##ModelId=489955E200D0
	UINT Write(const void * lpBuf,UINT nCount);
    
	//强制写入
	//##ModelId=489955E200DC
	bool Flush();

	//关闭文件
	//##ModelId=489955E200DD
	bool Close();

	/*position */

	//##ModelId=489955E200DE
	bool Seek(long lOff,UINT nFrom);

	//##ModelId=489955E200EA
	bool SeekToBegin();

	//##ModelId=489955E200EB
	bool SeekToEnd();

	//##ModelId=489955E200EC
	long GetLength();

	/* 错误描述 */
	//##ModelId=489955E200ED
	char * GetError();

};

#endif // !defined(AFX_FILEEX_H__6E96EBE5_0561_4EBF_9657_A9691815A6FB__INCLUDED_)
