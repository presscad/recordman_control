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
 �� �� ����socketsInit()
 ���ܸ�Ҫ����ʼ��socket
           windows����Ҫ�ڳ�����ʹ��Ws2_32.dll
 �� �� ֵ���ɹ�true,ʧ��false 
 ��    ������
***************************************************************/
inline bool socketsInit()
{
#ifdef OS_WINDOWS

	WSADATA wsadata;

	//��ʼ����SOCKET2.0�ĵ���
	if (WSAStartup(MAKEWORD(2,0), &wsadata) == 0)
		return true;
	else
		return false;
#endif
	// Linux/Unix do not require any initialization
	return true;
}

/*************************************************************
 �� �� ����socketsEnd()
 ���ܸ�Ҫ��������Ws2_32.dll�ĵ���
 �� �� ֵ���� 
 ��    ������
 �޸���ʷ��
***************************************************************/
inline void socketsEnd()
{
#ifdef OS_WINDOWS

	//�����׽��ֵĵ���
	WSACleanup();
#endif
}


//ͨѶ�� (������)
class CCommon
{
private:

	//ͨѶ���� 0-���� 1-���� 2-���� 3-����
	int       m_nType;                           

protected:

	//���ͻ�����
    XJBUFFER    m_SendBuff;                     

	//���ջ�����
	XJBUFFER    m_RecvBuff;                           
	
	//������
	CLock     m_Lock;

	//��־��¼
	CLogFile  m_LogFile;

public:

	//���캯��
	CCommon(int nType=ID_NET/*Ĭ��Ϊ����*/);

    //��������
	~CCommon();

	//��ý��շ��ͻ�����
	void *      GetSendBuffer();

	void *      GetRecvBuffer();

	//������־�������
	void        SetLogLevel(int nLevel);  

	//�����־�������
	int         GetLogLevel();

	//������־·��
	void        SetLogPath(char *pPath);
	
	//��ʼ����־�ļ�
	BOOL        InitLogFile(char * pFileName);

//����Ϊ���麯��,����������

    //��ͨѶ��
    virtual bool open()=0;                                        

	//���ͱ��ط��ͻ�����ָ����Ŀ������(���������ݽṹ)
    virtual int  write(int nLen=MAXMSGLEN)=0;           
	
	//����ָ����������ָ�����ȵ�����
	virtual int  write(void * pBuf,int nLen) =0;
    
	//����ָ����Ŀ�����ݵ����ؽ��ջ�����(���������ݽṹ)
	virtual int  read(int nLen=MAXMSGLEN)=0;   
	
	//����ָ����Ŀ�����ݵ�ָ���Ļ�����
	virtual int  read(void * pBuf,int nLen) =0;
	
	//�ر�ͨѶ��
    virtual bool close()=0;                                       
    
	//����ͨѶ��ѡ��
    virtual bool SetOptions(int nFlag,int nTime,int Reserve=0)=0;               

	//��������������
    //virtual int  CopyTo(CCommon *target);                       
};

enum sockstate_type {INITIALIZED, BINDED, LISTENING, CONNECTED, CLOSED};

//����ͨѶ�� ���м̳�CCommon
class CNet: public CCommon
{
private:
	//socket handler
	SOCKET			m_hSocket; 
	
	//�� AF_INET/PF_INET
	int				m_nDomain;  
	
	//socket������ TCP-SOCK_STREAM FTP-SOCK_DGRAM
	int				m_Type;      
	
    //Э������
	int				m_nProtocol;
	
    //���ؽ�ɫ��0--������, 1--�ͻ���, -1--��δ��ʼ��
	int				m_nLocalRole;	            

    //���ܳ�ʱ ��λ��MS
	int				m_nRecvTimes;
	
	
    //���ͳ�ʱ ��λ��MS
	int				m_nSendTimes;

	//socket״̬
	sockstate_type	m_eState;                   

	//�Զ˵�ַ
	NETADDR         m_OpponentAddr;       
	
	//���ص�ַ
	NETADDR         m_LocalAddr;   

	//�鲥��ַ
	NETADDR         m_MultiCastAddr;   

protected:
	int			  write_udp(int nLen=MAXMSGLEN);
	int           write_udp(void *pBuf, int nLen);
	int			  read_udp(int nLen=MAXMSGLEN);
	int			  read_udp(void *pBuf, int nLen);

	//���socket���
	SOCKET        GetHandler();

	//����socket���
	void          SetHandler(SOCKET handler);

	//�ͻ��˹���(���ӷ�����)
	bool          ConnectServerB(const char * ip=NULL,int port=-1,int nLocalPort=-1,int nTimeout=-1);

public:
//���캯��
	CNet(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);

	CNet(const NETADDR* addr,int domain = AF_INET, \
		  int type = SOCK_STREAM, int protocol = 0);
	
//��������
	virtual ~CNet();

//����CCommon�麯��

	//����Socket
	bool          open();    
	bool          open(int flag); 

	 //�ر�socket
	bool          close();                                         
    
	//��������
	int           write(int nLen=MAXMSGLEN);     
	int           write(void * pBuf,int nLen);

	//��������
	int           read(int nLen=MAXMSGLEN);    
	int           read(void * pBuf,int nLen);
	
    //����ѡ��
	bool          SetOptions(int nFlag,int nTime,int Reserve=0);                   
    
//�����Ա����

	//��������������
	int           CopyTo(const CNet & net,int nFlag=2);                         

	//�ͻ��˹���(���ӷ�����), 
	bool          ConnectServer(const char * ip=NULL,int port=-1,int nLocalPort=-1,int nTimeout=-1);

	//�������˹���(�󶨲���)
	bool          Bind (u_short port);

	bool          Bind (const char* host,u_short port);

	//��������,SERVER
	bool          Listen (int queue = SOMAXCONN);
	
	//��������,SERVER
	bool          Accept(CNet &sock);
	
	BOOL		  AddToMultiCast();
	
	//�������󶨡���������,SERVER
	bool          ListenServer(u_short port,const char * host=NULL,\
		                       int nqueue=SOMAXCONN);

	//����socketѡ��
	bool SetSocketOption(int nOptionName,  \
		                 const char* lpOptionValue, \
		                 int nOptionLen, \
						 int nLevel);                      
	
	//ȡ��socket������Ϣ
	void          GetSocketErrorMsg(char * cError);     
	
	//ȡ�ñ���IP��ַ
	bool          GetLocalComputerIp(char *ipAddr);                 

	//���öԶ˵�ַ
	void          SetOpponentAddr(const NETADDR * addr);    
	void		  SetLocalAddr(const NETADDR *addr);
	void		  SetMultiCastAddr(const NETADDR *addr);
	
	//��öԶ˵�ַ
	void          GetOpponentAddr(NETADDR * addr);    
	void          GetLocalAddr(NETADDR * addr); 
	void          GetMultiCastAddr(NETADDR * addr); 

	//��ý�ɫ
	int           GetLocalRole();  
	
	//���ý�ɫ
	void          SetLocalRole(int nRole);
 
	//���õ�ǰsocket״̬
    void          SetState(sockstate_type state);

	//ȡ�õ�ǰsocket״̬
	sockstate_type GetState() const { return m_eState; }
	
	int GetSocketType()const { return m_Type;}
};

#endif 
