/********************************************************************
	created:	2008/09/18
	created:	17:9:2008   14:37
	file base:	SafeLock
	file ext:	h
	author:		
	
	purpose:	
*********************************************************************/
#ifndef Safe_Lock_h__
#define Safe_Lock_h__

#include "Lock.h"

/**
 * @defgroup 	CLockableObject	��˵��
 * @{
 */
/**
 * @brief       �������������
*  @author      
 * @version     ver1.0
 * @date        20/09/08

 *
 * example
 * @code
 *    
 * @endcode
 */
class CLockableObject {
public:
    /*
     *  @brief   	���캯�� 
     *  @return 	�� 
     */
	CLockableObject()	{}
    /*
     *  @brief   	��������
     *  @return 	�� 
     */
	virtual ~CLockableObject(){}
    /*
     *  @brief   	���� 
     *  @return 	bool True-�ɹ� False-ʧ�� 
     */
	virtual bool Lock() = 0;
    /*
     *  @brief   	������� 
     *  @return 	bool True-�ɹ� False-ʧ��
     */
	virtual bool Unlock() = 0;
};
/** @} */ //OVER

/**
 * @defgroup 	CLockUp	��˵��
 * @{
 */
/**
 * @brief       �����������Guard��
*  @author      
 * @version     ver1.0
 * @date        20/09/08

 *
 * example
 * @code
 *    
 * @endcode
 */
class CLockUp {
public:
    /*
     *  @brief   	���캯��
     *  @param 		[In/Out]CLockableObject*  ����������ָ��
     *  @return 	�� 
     */
	explicit CLockUp(CLockableObject* pLockable);
    /*
     *  @brief   	��������
     *  @return 	�� 
     */
	~CLockUp();
private:
	CLockUp(const CLockUp&); //����copy���캯��
	CLockUp& operator= (const CLockUp&);//����assign��������
	/** @brief  		 ����������ָ�� */
	CLockableObject* m_pLockable;
};
/** @} */ //OVER


/**
 * @defgroup 	CSafeLock	��˵��
 * @{
 */
/**
 * @brief       CSafeLock��İ�װ���򻯳�ʼ����ɾ������
*  @author      
 * @version     ver1.0
 * @date        01/10/08

 *
 * example
 * @code
 *    
 * @endcode
 */
class CSafeLock:public CLockableObject{
public:
    /*
     *  @brief   	���캯��
     *  @param 		[In]const char*  ��־���Ǹ�ģ����ʹ�øû�����
     *  @return 	�� 
     */

	explicit CSafeLock(const char* pHost);
    /*
     *  @brief   	�������� 
     *  @return 	�� 
     */

	virtual ~CSafeLock();
    /*
     *  @brief   	����ȡ������Ȩ
     *  @return 	bool True-�ɹ� False-ʧ��
     */

	virtual bool Lock();
    /*
     *  @brief   	�ó�����Ȩ
     *  @return 	bool True-�ɹ� False-ʧ��
     */

	virtual bool Unlock();
    /*
     *  @brief   	������ȡ������Ȩ 
     *  @return 	bool True-�ɹ� False-ʧ��
     */

	bool Trylock();
private:
	CSafeLock(const CSafeLock&); //����copy���캯��
	CSafeLock& operator= (const CSafeLock&); //����assign��������
	/** @brief  		 CLock��������� */
	CLock m_lock;
};
/** @} */ //OVER

#endif // Safe_Lock_h__
