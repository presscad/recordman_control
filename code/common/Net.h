//
// Net.h
//
// This is a TCP/IP socket  class
//
// On Windows the program that uses this utility
// should be linked with Ws2_32.lib
//
// Copyright (C) 2006
// Version 1.0
//

#ifndef INCLUDED_SOCKETS_H
#define INCLUDED_SOCKETS_H

#include "define.h"
#include "Lock.h"
#include "LogFile.h"

/*************************************************************
 函 数 名：socketsInit()
 功能概要：初始化socket
           windows下需要在程序中使用Ws2_32.dll
 返 回 值：成功true,失败false 
 参    数：无
***************************************************************/
inline bool socketsInit()
{
#ifdef OS_WINDOWS

	WSADATA wsadata;

	//初始化对SOCKET2.0的调用
	if (WSAStartup(MAKEWORD(2,0), &wsadata) == 0)
		return true;
	else
		return false;
#endif
	// Linux/Unix do not require any initialization
	return true;
}

/*************************************************************
 函 数 名：socketsEnd()
 功能概要：结束对Ws2_32.dll的调用
 返 回 值：无 
 参    数：无
 修改历史：
***************************************************************/
inline void socketsEnd()
{
#ifdef OS_WINDOWS

	//结束套接字的调用
	WSACleanup();
#endif
}


//通讯类 (纯虚类)
class CCommon
{
private:

	//通讯类型 0-网络 1-串口 2-拨号 3-其它
	int       m_nType;                           

protected:

	//发送缓冲区
    XJBUFFER    m_SendBuff;                     

	//接收缓冲区
	XJBUFFER    m_RecvBuff;                           
	
	//互斥体
	CLock     m_Lock;

	//日志记录
	CLogFile  m_LogFile;

public:

	//构造函数
	CCommon(int nType=ID_NET/*默认为网络*/);

    //析构函数
	~CCommon();

	//获得接收发送缓冲区
	void *      GetSendBuffer();

	void *      GetRecvBuffer();

	//设置日志输出级别
	void        SetLogLevel(int nLevel);  

	//获得日志输出级别
	int         GetLogLevel();

	//设置日志路径
	void        SetLogPath(char *pPath);
	
	//初始化日志文件
	BOOL        InitLogFile(char * pFileName);

//以下为纯虚函数,供子类重载

    //打开通讯口
    virtual bool open()=0;                                        

	//发送本地发送缓冲区指定数目的数据(不关心内容结构)
    virtual int  write(int nLen=MAXMSGLEN)=0;           
	
	//发送指定缓冲区中指定长度的数据
	virtual int  write(void * pBuf,int nLen) =0;
    
	//接收指定数目的数据到本地接收缓冲区(不关心内容结构)
	virtual int  read(int nLen=MAXMSGLEN)=0;   
	
	//接收指定数目的数据到指定的缓冲区
	virtual int  read(void * pBuf,int nLen) =0;
	
	//关闭通讯口
    virtual bool close()=0;                                       
    
	//设置通讯口选项
    virtual bool SetOptions(int nFlag,int nTime,int Reserve=0)=0;               

	//拷贝缓冲区数据
    //virtual int  CopyTo(CCommon *target);                       
};

enum sockstate_type {INITIALIZED, BINDED, LISTENING, CONNECTED, CLOSED};

//网络通讯类 公有继承CCommon
class CNet: public CCommon
{
private:
	//socket handler
	SOCKET			m_hSocket; 
	
	//域 AF_INET/PF_INET
	int				m_nDomain;  
	
	//socket流类型 TCP-SOCK_STREAM FTP-SOCK_DGRAM
	int				m_Type;      
	
    //协议类型
	int				m_nProtocol;
	
    //本地角色：0--服务器, 1--客户端, -1--尚未初始化
	int				m_nLocalRole;	            

    //接受超时 单位：MS
	int				m_nRecvTimes;
	
	
    //发送超时 单位：MS
	int				m_nSendTimes;

	//socket状态
	sockstate_type	m_eState;                   

	//对端地址
	NETADDR         m_OpponentAddr;       
	
	//本地地址
	NETADDR         m_LocalAddr;   

	//组播地址
	NETADDR         m_MultiCastAddr;   

protected:
	int			  write_udp(int nLen=MAXMSGLEN);
	int           write_udp(void *pBuf, int nLen);
	int			  read_udp(int nLen=MAXMSGLEN);
	int			  read_udp(void *pBuf, int nLen);

	//获得socket句柄
	SOCKET        GetHandler();

	//设置socket句柄
	void          SetHandler(SOCKET handler);

	//客户端工具(连接服务器)
	bool          ConnectServerB(const char * ip=NULL,int port=-1,int nLocalPort=-1,int nTimeout=-1);

public:
//构造函数
	CNet(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);

	CNet(const NETADDR* addr,int domain = AF_INET, \
		  int type = SOCK_STREAM, int protocol = 0);
	
//析构函数
	virtual ~CNet();

//重载CCommon虚函数

	//创建Socket
	bool          open();    
	bool          open(int flag); 

	 //关闭socket
	bool          close();                                         
    
	//发送数据
	int           write(int nLen=MAXMSGLEN);     
	int           write(void * pBuf,int nLen);

	//接收数据
	int           read(int nLen=MAXMSGLEN);    
	int           read(void * pBuf,int nLen);
	
    //设置选项
	bool          SetOptions(int nFlag,int nTime,int Reserve=0);                   
    
//自身成员函数

	//拷贝缓冲区数据
	int           CopyTo(const CNet & net,int nFlag=2);                         

	//客户端工具(连接服务器), 
	bool          ConnectServer(const char * ip=NULL,int port=-1,int nLocalPort=-1,int nTimeout=-1);

	//服务器端工具(绑定操作)
	bool          Bind (u_short port);

	bool          Bind (const char* host,u_short port);

	//监听操作,SERVER
	bool          Listen (int queue = SOMAXCONN);
	
	//接受连接,SERVER
	bool          Accept(CNet &sock);
	
	BOOL		  AddToMultiCast();
	
	//创建、绑定、监听操作,SERVER
	bool          ListenServer(u_short port,const char * host=NULL,\
		                       int nqueue=SOMAXCONN);

	//设置socket选项
	bool SetSocketOption(int nOptionName,  \
		                 const char* lpOptionValue, \
		                 int nOptionLen, \
						 int nLevel);                      
	
	//取得socket错误消息
	void          GetSocketErrorMsg(char * cError);     
	
	//取得本地IP地址
	bool          GetLocalComputerIp(char *ipAddr);                 

	//设置对端地址
	void          SetOpponentAddr(const NETADDR * addr);    
	void		  SetLocalAddr(const NETADDR *addr);
	void		  SetMultiCastAddr(const NETADDR *addr);
	
	//获得对端地址
	void          GetOpponentAddr(NETADDR * addr);    
	void          GetLocalAddr(NETADDR * addr); 
	void          GetMultiCastAddr(NETADDR * addr); 

	//获得角色
	int           GetLocalRole();  
	
	//设置角色
	void          SetLocalRole(int nRole);
 
	//设置当前socket状态
    void          SetState(sockstate_type state);

	//取得当前socket状态
	sockstate_type GetState() const { return m_eState; }
	
	int GetSocketType()const { return m_Type;}
};

#endif 
