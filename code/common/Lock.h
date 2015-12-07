//
// Lock.h
//
// This is a multithreading lock Operator  class
//
// Copyright (C) 2006
// Version 1.0
//

#ifndef LOCK_H_HEADER_INCLUDED_BAD5A27F
#define LOCK_H_HEADER_INCLUDED_BAD5A27F

#include "sys_define.h"

//## �������࣬����̼߳����������
class CLock
{
  public:
	//���캯��
	 CLock();

	//��������
	~CLock();

	//��ʼ����
    BOOL init(char * phost = NULL);

	//ɾ����
    //##ModelId=4519E3E701F2
    BOOL del();

	//�����ȴ�ֱ��ȡ��ӵ��Ȩ
    BOOL lock();

	//�����Ƿ�ȡ��ӵ��Ȩ,�������Ϸ���
	BOOL trylock();

	//�ó�ӵ��Ȩ
    BOOL unlock();

  private:
    MUTEX m_mutex;

	//��mutex����������
	char  ch_host[FILE_NAME_MAX_LEN];

	//��ʼ����־
	BOOL  bInit;

};



#endif /* LOCK_H_HEADER_INCLUDED_BAD5A27F */
