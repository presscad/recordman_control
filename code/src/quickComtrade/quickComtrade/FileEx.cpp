// FileEx.cpp: implementation of the CFileEx class.
//
//////////////////////////////////////////////////////////////////////

#include "FileEx.h"
#include "constant.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*************************************************************
 函 数 名：ReadString()
 功能概要：从文件中读取一行
 返 回 值: int 实际读取到的字节数
 参    数：strLIne:存放读取的字符串
           

***************************************************************/
//##ModelId=489955E200AB
int  CFileEx::ReadString(CStrongString& strLine)   
{   
	char str[MAX_LINE];
	int   len=0;   
	char   ch;   
//	str=strLine.GetBuffer(0); 
	int	i = 0;
	while(1)   
	{   
		if(feof(pfile))     
		{   
			*str   =   '\0';   
			return   -3;   
		}   
		ch   =   fgetc(pfile);   
		if((ch   ==   '\r')   ||   (ch   ==   '\n')   ||   (ch   ==   '\0'))   
		{   
			if(len   ==   0)   continue;   
			break;   
		}   
		str[i]   =   ch;   
		i++;   
		len   ++;   
	}   
	str[i]   =   '\0';  
	strLine.Format("%s",str);
	return   len;   
}

//##ModelId=489955E200AD
CFileEx::CFileEx()
{
	pfile = NULL;

	memset(m_chError,0,FILE_ERROR_MAX_LEN);

	memset(m_chName,0,FILE_NAME_MAX_LEN);
}

//##ModelId=489955E200AE
CFileEx::~CFileEx()
{
	if(pfile != NULL)
		fclose(pfile);

	pfile = NULL;
}

/*************************************************************
 函 数 名：Open()
 功能概要：打开文件
 返 回 值: BOOL  TRUE:成功 FALSE:失败
 参    数：const char * name   文件名称
           UINT  nOpenFlags    打开标志
		   #define modeCreate                0          //0000 0000
           #define modeRead                  1          //0000 0001
           #define modeWrite                 2          //0000 0010
           #define modeReadWrite             4          //0000 0100
		   #define modeNoTruncate            8          //0000 1000
***************************************************************/
//##ModelId=489955E200BE
bool CFileEx::Open(const char *name, UINT nOpenFlags)
{
	if(name == NULL)
	{
		memset(m_chError,0,FILE_ERROR_MAX_LEN);
		strcpy(m_chError,"File Name Is NULL,Can't Open");
        printf("%s\n",m_chError);
		return false;
	}
	
	//save file name
	if( sizeof(name) > FILE_NAME_MAX_LEN )
	{
		memset(m_chError,0,FILE_ERROR_MAX_LEN);
		sprintf(m_chError,"file Name len =%d,too leng",sizeof(name));
        printf("%s\n",m_chError);
		return false;
	}
	sprintf(m_chName,"%s",name);
    
	if(pfile != NULL)
	{
		memset(m_chError,0,FILE_ERROR_MAX_LEN);
		sprintf(m_chError,"the File '%s' has been Opend,please close it Firstly",name);
		printf("%s\n",m_chError);
		return false;
	}
    
	switch(nOpenFlags)
	{
	case 0:    
		/*Opens an empty file for both reading and writing. If the given file 
		  exists, its contents are destroyed; creates the file first if it 
		  doesn’t exist. */
		pfile = fopen(name,"w+b");
		break;
	case 1:      
		/*Opens for reading. If the file does not exist or cannot be found, 
		  the fopen call fails*/
		pfile = fopen(name,"rb");
		break;
	case 2:
	case 8:
		/*Opens for writing at the end of the file (appending) without removing 
		  the EOF marker before writing new data to the file; creates the file 
		  first if it doesn’t exist*/
		pfile = fopen(name,"ab");
		break;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		/*Opens for both reading and writing. (The file must exist.) */
		pfile = fopen(name,"r+b");
		break;
	default:
		/*Opens for reading and appending; the appending operation includes the 
		  removal of the EOF marker before new data is written to the file and 
		  the EOF marker is restored after writing is complete; creates the file 
		  first if it doesn’t exist*/
		pfile = fopen(name,"a+b");
		break;
	}

	if(pfile == NULL)
	{
		memset(m_chError,0,FILE_ERROR_MAX_LEN);
		sprintf(m_chError,"open %s failed,reason:%s",name,strerror(errno));
        printf("%s\n",m_chError);
		return false;
	}
	return true;
}

/*************************************************************
 函 数 名：Read()
 功能概要：从文件中读取指定的字节数目
 返 回 值: UINT 实际读取到的字节数
 参    数：void * lpbuf  存放读取到数据的缓冲区
           UINT   nCount 读取的字节数,实际返回值可能小于该值

***************************************************************/
//##ModelId=489955E200CD
UINT CFileEx::Read(void * lpbuf, UINT nCount)
{
	UINT nRead = 0;
	if(pfile == NULL) 
	{
        memset(m_chError,0,FILE_ERROR_MAX_LEN);
		sprintf(m_chError,"read from %s failed because the file handler is NULL",m_chName);
		printf("%s\n",m_chError);
		return 0;
	}

	if(!feof(pfile))
	{
		//if( fgets((char*)lpbuf,nCount,pfile) == NULL)
		nRead = fread((char*)lpbuf,sizeof(char),nCount,pfile);
		if(ferror(pfile))
		{
			sprintf(m_chError,"read from %s error,reason:%s",m_chName,strerror(errno));
			printf("%s\n",m_chError);
			Flush();
			return nRead;
		}
		
		//读与写进行转换时需要增加一个fflush或fseek或fsetpos
		Flush();
		return  nRead;//(strlen((char*)lpbuf)+1);	
	}
    
	//file pointer is end
	sprintf(m_chError,"the file %s's pointer has been eof,can't read",m_chName);
	printf("%s\n",m_chError);
	return 0;
}

/*************************************************************
 函 数 名：Write()
 功能概要：向文件中写入指定数目的数据
 返 回 值: UINT 实际写入文件中的数据数目
 参    数：const void * lpbuf  要写入的数据缓存区
           UINT  nCount        要写入的数据的数目
***************************************************************/
//##ModelId=489955E200D0
UINT CFileEx::Write(const void * lpBuf,UINT nCount)
{
	UINT nwrite = 0;

	if(pfile == NULL) 
	{
        memset(m_chError,0,FILE_ERROR_MAX_LEN);
		sprintf(m_chError,"write to %s failed because the file handler is NULL",m_chName);
		printf("%s\n",m_chError);
		return 0;
	}

	//write data
    nwrite = fwrite(lpBuf,sizeof(char),nCount,pfile);
	if( int n=ferror(pfile) )
	{
		sprintf(m_chError,"write to %s error,reason:%s",m_chName,strerror(errno));
	    printf("%s\n",m_chError);
		Flush();
		return nwrite;
	}
		
	//读与写进行转换时需要增加一个fflush或fseek或fsetpos
	Flush();
	return nwrite;	
} 

/*************************************************************
 函 数 名：Flush()
 功能概要：强制将缓存数据写入文件
 返 回 值: BOOL
 参    数：无
***************************************************************/
//##ModelId=489955E200DC
bool CFileEx::Flush()
{
	if(pfile == NULL) 
	{
		sprintf(m_chError,"fflush %s failed because the file handler is NULL",m_chName);
		printf("%s\n",m_chError);
		return false;
	}

	if(fflush(pfile) == 0)
		return true;
	else{
	    sprintf(m_chError,"fflush %s error,reason:%s",m_chName,strerror(errno));
		printf("%s\n",m_chError);
	}
	return false;
}

/*************************************************************
 函 数 名：Close()
 功能概要：关闭文件
 返 回 值: BOOL
 参    数：无
 修改历史：
***************************************************************/
//##ModelId=489955E200DD
bool CFileEx::Close()
{
	if(pfile == NULL) 
	{
		return true;
	}

	if(fclose(pfile) == 0)
	{
		pfile = NULL;
		return true;
	}
	else{
	    sprintf(m_chError,"fclose %s error,reason:%s",m_chName,strerror(errno));
		printf("%s\n",m_chError);
	}
	return false;
}

/*************************************************************
 函 数 名：Seek()
 功能概要：移动文件指针到指定的位置
 返 回 值: BOOL  成功或失败
 参    数：long lOff   偏移量
           UINT nFrom  偏移起始地址
***************************************************************/
//##ModelId=489955E200DE
bool CFileEx::Seek(long lOff,UINT nFrom)
{
	int nFlag = 0;
	if(pfile == NULL) 
	{
		sprintf(m_chError,"fseek %s failed because the file handler is NULL",m_chName);
		printf("%s\n",m_chError);
		return false;
	}
    
	//判断position
	switch(nFrom)
	{
	case CCommonFile::begin:
		nFlag = SEEK_SET;
		break;
	case CCommonFile::current:
		nFlag = SEEK_CUR;
		break;
	case CCommonFile::end:
		nFlag = SEEK_END;
		break;
	default:
		sprintf(m_chError,"fseek %s failed because the second argument is error",m_chName);
		printf("%s\n",m_chError);
		return false;
	}
      
	//fseek
	if(fseek(pfile,lOff,nFlag) == 0)
		return true;
	else{
		sprintf(m_chError,"fseek %s %l len error,reason:%s",m_chName,lOff,strerror(errno));
        printf("%s\n",m_chError);
	}
	return false;
}

/*************************************************************
 函 数 名：SeekToBegin()
 功能概要：移动文件指针到文件的起始地址
 返 回 值: BOOL
 参    数：无
***************************************************************/
//##ModelId=489955E200EA
bool CFileEx::SeekToBegin()
{
	if(pfile == NULL) 
	{
		sprintf(m_chError,"fseek to begien %s failed because the file handler is NULL",m_chName);
		printf("%s\n",m_chError);
		return false;
	}

	//set file position to begin
    if(fseek(pfile,0,SEEK_SET) == 0)
		return true;
	else{
		sprintf(m_chError,"fseek to begin %s error,reason:%s",m_chName,strerror(errno));
        printf("%s\n",m_chError);
	}
	return false;
}

/*************************************************************
 函 数 名：SeekToEnd()
 功能概要：移动文件指针到文件结束位置
 返 回 值: BOOL
 参    数：无
***************************************************************/
//##ModelId=489955E200EB
bool CFileEx::SeekToEnd()
{
	if(pfile == NULL) 
	{
		sprintf(m_chError,"fseek to end %s failed because the file handler is NULL",m_chName);
		printf("%s\n",m_chError);
		return false;
	}

	//set file position to end
    if(fseek(pfile,0,SEEK_END) == 0)
		return true;
	else{
		sprintf(m_chError,"fseek to end %s error,reason:%s",m_chName,strerror(errno));
        printf("%s\n",m_chError);
	}

	return false;
}

/*************************************************************
 函 数 名：GetLength()
 功能概要：获得文件总长度
 返 回 值: long 文件的总字节数
 参    数：无
***************************************************************/
//##ModelId=489955E200EC
long CFileEx::GetLength()
{
	long num = 0;

	int r=0;

	//用于保存文件当前位置的变量
	fpos_t  curpos;

	if(pfile == NULL) 
	{
		sprintf(m_chError,"get %s length failed because the file handler is NULL",m_chName);
		printf("%s\n",m_chError);
		return -1;
	}
    
	//保存当前文件位置
	if( fgetpos( pfile, &curpos ) != 0 )
	{
		sprintf(m_chError,"get file cur position failed when get %s's length,\
               rason:%s",m_chName,strerror(errno));
		printf("%s\n",m_chError);
		return -1;		    
	}

    //移动文件到首部
	if( !SeekToBegin() )
	{
		return -1;
	}

	// 获得首位置偏移
	long n_begin = ftell(pfile);

	// 移动到文件尾部
	if( !SeekToEnd() )
	{
		fsetpos(pfile,&curpos);

		return -1;
	}

	// 获得尾位置偏移
	long n_end = ftell(pfile);

	// 获得文件长度
	num = n_end - n_begin;

	//移动文件指针到原来位置
	if( fsetpos(pfile,&curpos) != 0)
	{
		sprintf(m_chError,"set file cur position failed when get %s's length,\
               rason:%s",m_chName,strerror(errno));
		printf("%s\n",m_chError);
		return num;		  
	}

	return num;
}	

/*************************************************************
 函 数 名：GetError()
 功能概要：获得错误原因
 返 回 值: char * 错误描述
 参    数：无
***************************************************************/
//##ModelId=489955E200ED
char *CFileEx::GetError()
{
	return m_chError;
}
