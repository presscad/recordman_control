//��װ���ֱ���֮����໥ת��
//ת���������������ɱ��ฺ�������ڴ�ռ�(ʹ��new����)
//���Ǳ��಻����ɾ���ڴ�ռ䣬�ⲿ���������ɾ���ڴ�ռ����(ʹ��delete[] ����)
////////////////////////////////////////////////////////////////

#ifndef _GECODECONVERT_H_
#define	_GECODECONVERT_H_

#include "stdint.h"

class CGECodeConvert
{
public:
	CGECodeConvert(void);
	~CGECodeConvert(void);
public:
	/*
	* @brief	Gbk2Unicode	��GBK�ַ���ת��Ϊ��ϵͳʹ�õ�unicode�ַ���
	* @param	[In]a param of Type char * pGbk		GBK�ַ���
	* @param	[In]a param of Type long nGbkCount	GBK�ַ�����	(strlen)
	* @param	[Out]a param of Type wchar_t * & pUnicode	����ת��������unicode�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��	
	* @return   long	ת���ɹ�����unicode�ַ�������ʧ�ܷ���-1 
	*/
	long Gbk2Unicode(const char* pGbk, long nGbkCount, wchar_t*& pUnicode);

	/*
	* @brief	Unicode2Gbk	����ϵͳʹ�õ�unicode�ַ���ת��ΪGBK�ַ���
	* @param	[In]a param of Type wchar_t * pUnicode	unicode�ַ���	
	* @param	[In]a param of Type long nUniCount		unicode�ַ���(wcslen)
	* @param	[Out]a param of Type char * & pGbk		����ת��������GBK�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	* @return   long	ת���ɹ�����gbk�ַ�������ʧ�ܷ���-1
	*/
	long Unicode2Gbk(const wchar_t* pUnicode, long nUniCount, char*& pGbk);

	/*
	* @brief	Default2Unicode	��ansi�ַ���ת��Ϊ��ϵͳʹ�õ�unicode�ַ���,ansi�ַ�������ϵͳ�ı��������й�
	* @param	[In]a param of Type char * pSrc	ansi�ַ���	
	* @param	[In]a param of Type long nSrc		ansi�ַ�����(strlen)
	* @param	[Out]a param of Type wchar_t * & pUnicode	����ת��������unicode�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��		
	* @return   long	ת���ɹ�����unicode�ַ�������ʧ�ܷ���-1 
	*/
	long Default2Unicode(const char* pSrc, long nSrc, wchar_t*& pUnicode);

	/*
	* @brief	Unicode2Default	����ϵͳʹ�õ�unicode�ַ���ת��Ϊansi�ַ���,ansi�ַ�������ϵͳ�ı��������й�
	* @param	[In]a param of Type wchar_t * pUnicode	unicode�ַ���	
	* @param	[In]a param of Type long nUniCount			unicode�ַ�����(wcslen)
	* @param	[Out]a param of Type char * & pDest		����ת��������ansi�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	* @return   long	ת���ɹ�����ansi�ַ�������ʧ�ܷ���-1
	*/
	long Unicode2Default(const wchar_t* pUnicode, long nUniCount, char*& pDest);

	/*
	* @brief	Utf82Unicode	��utf8�ַ���ת��Ϊ��ϵͳʹ�õ�unicode�ַ���
	* @param	[In]a param of Type char * pUtf8		utf8�ַ���	
	* @param	[In]a param of Type long nUtf8Count	utf8�ַ�����	
	* @param	[Out]a param of Type wchar_t * & pUnicode	����ת��������unicode�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��	
	* @return   long	ת���ɹ�����unicode�ַ�������ʧ�ܷ���-1
	*/
	long Utf82Unicode(const char* pUtf8, long nUtf8Count, wchar_t*& pUnicode);

	/*
	* @brief	Unicode2Utf8	����ϵͳʹ�õ�unicode�ַ���ת��Ϊutf8�ַ���
	* @param	[In]a param of Type wchar_t * pUnicode	unicode�ַ���	
	* @param	[In]a param of Type long nUniCount		unicode�ַ�����(wcslen)
	* @param	[Out]a param of Type char * & pUtf8		����ת��������utf8�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	* @return   long	ת���ɹ�����utf8�ַ�������ʧ�ܷ���-1
	*/
	long Unicode2Utf8(const wchar_t* pUnicode, long nUniCount, char*& pUtf8);


	/*
	* @brief	Utf16ToUtf32_BE	�����utf16�ַ�ת��Ϊ���utf32�ַ�
	* @param	[In]a param of Type uint16_t * pUtf16	���utf16�ַ�	
	* @param	[In]a param of Type long nUtf16Count		���utf16�ַ�����
	* @param	[Out]a param of Type uint32_t * & pUtf32	����ת�������Ĵ��utf32�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	* @return   long	ת���ɹ�����utf32�ַ�������ʧ�ܷ���-1
	*/
	long Utf16ToUtf32_BE(const uint16_t* pUtf16, long nUtf16Count, uint32_t*& pUtf32);

	/*
	* @brief	Utf32ToUtf16_BE	�����utf32�ַ�ת��Ϊ���utf16�ַ�
	* @param	[In]a param of Type uint32_t * pUtf32	���utf32�ַ�	
	* @param	[In]a param of Type long nUtf32Count		���utf32�ַ�����
	* @param	[Out]a param of Type uint16_t * & pUtf16		����ת�������Ĵ��utf16�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	* @return   long	ת���ɹ�����utf16�ַ�������ʧ�ܷ���-1
	*/
	long Utf32ToUtf16_BE(const uint32_t* pUtf32, long nUtf32Count, uint16_t*& pUtf16);

	/*
	* @brief	Utf16ToUtf32_LE	��С��utf16�ַ�ת��ΪС��utf32�ַ�
	* @param	[In]a param of Type uint16_t * pUtf16	С��utf16�ַ�	
	* @param	[In]a param of Type long nUtf16Count		С��utf16�ַ�����
	* @param	[Out]a param of Type uint32_t * & pUtf32	����ת��������С��utf32�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	* @return   long	ת���ɹ�����utf32�ַ�������ʧ�ܷ���-1
	*/
	long Utf16ToUtf32_LE(const uint16_t* pUtf16, long nUtf16Count, uint32_t*& pUtf32);

	/*
	* @brief	Utf32ToUtf16_LE	��С��utf32�ַ�ת��ΪС��utf16�ַ�
	* @param	[In]a param of Type uint32_t * pUtf32	С��utf32�ַ�	
	* @param	[In]a param of Type long nUtf32Count		С��utf32�ַ�����
	* @param	[Out]a param of Type uint16_t * & pUtf16		����ת��������С��utf16�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	* @return   long	ת���ɹ�����utf16�ַ�������ʧ�ܷ���-1
	*/
	long Utf32ToUtf16_LE(const uint32_t* pUtf32, long nUtf32Count, uint16_t*& pUtf16);

	/*
	 *  @brief   	Utf82Gbk	��UTF8�ַ���ת��ΪGBK�����ַ��� 
	 *	@param 		[In]a param of Type  char * pUtf8	UTF8�����ַ���
	 *	@param 		[In]a param of Type  long nUtf8Count	UTF8�����ַ�������
	 *	@param 		[In]a param of Type  char * & pGbk	����ת��������GBK�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	 *  @return 	long	 ת���ɹ��󷵻�GBK�ַ�������ʧ�ܷ���-1
	 */
	long Utf82Gbk(const char* pUtf8, long nUtf8Count, char*& pGbk);

	/*
	 *  @brief   	Gbk2Utf8	��GBK�ַ���ת��ΪUTF8�����ַ��� 
	 *	@param 		[In]a param of Type  char * pGbk	GBK�����ַ���
	 *	@param 		[In]a param of Type  long nGbkCount	GBK�����ַ�������
	 *	@param 		[In]a param of Type  char * & pUtf8	����ת��������UTF8�ַ���,������������ʹ�������ʹ��(delete[])����ɾ��
	 *  @return 	long	 ת���ɹ��󷵻�UTF8�ַ�������ʧ�ܷ���-1
	 */
	long Gbk2Utf8(const char* pGbk, long nGbkCount, char*& pUtf8);

private:
	/*
	* @brief	swap16	 ��תһ��16λֵ�ı��淽ʽ. ��С�˷�ת
	* @param	[In/Out]a param of Type uint16_t & w		Ҫ��ת��ֵ
	* @return   void	
	*/
	void swap16(uint16_t& w);

	/*
	* @brief	swap32	 ��תһ��32λֵ�ı��淽ʽ. ��С�˷�ת
	* @param	[In/Out]a param of Type uint32_t & w		Ҫ��ת��ֵ
	* @return   void	
	*/
	void swap32(uint32_t& w);

	/*
	* @brief	IsLittleEndian	���ϵͳ�Ƿ�ʹ��С��ģʽ
	* @return   bool	ʹ��С��ģʽ����true,ʹ�ô��ģʽ����false
	*/
	bool IsLittleEndian();
};


inline void CGECodeConvert::swap16(uint16_t& w)
{
	w = ((uint16_t)((((uint16_t)(w)&(uint16_t)0x00ffU) << 8) | (((uint16_t)(w)&(uint16_t)0xff00U) >> 8) ));
}

inline void CGECodeConvert::swap32(uint32_t& w)
{
	w = (uint32_t)( (((uint32_t)(w)&(uint32_t)0x000000ffUL) << 24 )|
		(((uint32_t)(w)&(uint32_t)0xff000000UL) >> 24 )|
		(((uint32_t)(w)&(uint32_t)0x0000ff00UL) << 8 )|
		(((uint32_t)(w)&(uint32_t)0x00ff0000UL) >> 8 ));
}

inline bool CGECodeConvert::IsLittleEndian()
{
	int i = 1;
	char* p = (char*)&i;
	if(*p == 1)
		return true;
	return false;
}
#endif
