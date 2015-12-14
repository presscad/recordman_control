//封装各种编码之间的相互转换
//转换出来的新数据由本类负责申请内存空间(使用new操作)
//但是本类不负责删除内存空间，外部程序须调用删除内存空间操作(使用delete[] 操作)
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
	* @brief	Gbk2Unicode	将GBK字符串转换为本系统使用的unicode字符串
	* @param	[In]a param of Type char * pGbk		GBK字符串
	* @param	[In]a param of Type long nGbkCount	GBK字符个数	(strlen)
	* @param	[Out]a param of Type wchar_t * & pUnicode	保存转换出来的unicode字符串,包含结束符。使用完后请使用(delete[])进行删除	
	* @return   long	转换成功返回unicode字符个数，失败返回-1 
	*/
	long Gbk2Unicode(const char* pGbk, long nGbkCount, wchar_t*& pUnicode);

	/*
	* @brief	Unicode2Gbk	将本系统使用的unicode字符串转换为GBK字符串
	* @param	[In]a param of Type wchar_t * pUnicode	unicode字符串	
	* @param	[In]a param of Type long nUniCount		unicode字符数(wcslen)
	* @param	[Out]a param of Type char * & pGbk		保存转换出来的GBK字符串,包含结束符。使用完后请使用(delete[])进行删除
	* @return   long	转换成功返回gbk字符个数，失败返回-1
	*/
	long Unicode2Gbk(const wchar_t* pUnicode, long nUniCount, char*& pGbk);

	/*
	* @brief	Default2Unicode	将ansi字符串转换为本系统使用的unicode字符串,ansi字符编码与系统的本地设置有关
	* @param	[In]a param of Type char * pSrc	ansi字符串	
	* @param	[In]a param of Type long nSrc		ansi字符个数(strlen)
	* @param	[Out]a param of Type wchar_t * & pUnicode	保存转换出来的unicode字符串,包含结束符。使用完后请使用(delete[])进行删除		
	* @return   long	转换成功返回unicode字符个数，失败返回-1 
	*/
	long Default2Unicode(const char* pSrc, long nSrc, wchar_t*& pUnicode);

	/*
	* @brief	Unicode2Default	将本系统使用的unicode字符串转换为ansi字符串,ansi字符编码与系统的本地设置有关
	* @param	[In]a param of Type wchar_t * pUnicode	unicode字符串	
	* @param	[In]a param of Type long nUniCount			unicode字符个数(wcslen)
	* @param	[Out]a param of Type char * & pDest		保存转换出来的ansi字符串,包含结束符。使用完后请使用(delete[])进行删除
	* @return   long	转换成功返回ansi字符个数，失败返回-1
	*/
	long Unicode2Default(const wchar_t* pUnicode, long nUniCount, char*& pDest);

	/*
	* @brief	Utf82Unicode	将utf8字符串转换为本系统使用的unicode字符串
	* @param	[In]a param of Type char * pUtf8		utf8字符串	
	* @param	[In]a param of Type long nUtf8Count	utf8字符个数	
	* @param	[Out]a param of Type wchar_t * & pUnicode	保存转换出来的unicode字符串,包含结束符。使用完后请使用(delete[])进行删除	
	* @return   long	转换成功返回unicode字符个数，失败返回-1
	*/
	long Utf82Unicode(const char* pUtf8, long nUtf8Count, wchar_t*& pUnicode);

	/*
	* @brief	Unicode2Utf8	将本系统使用的unicode字符串转换为utf8字符串
	* @param	[In]a param of Type wchar_t * pUnicode	unicode字符串	
	* @param	[In]a param of Type long nUniCount		unicode字符个数(wcslen)
	* @param	[Out]a param of Type char * & pUtf8		保存转换出来的utf8字符串,包含结束符。使用完后请使用(delete[])进行删除
	* @return   long	转换成功返回utf8字符个数，失败返回-1
	*/
	long Unicode2Utf8(const wchar_t* pUnicode, long nUniCount, char*& pUtf8);


	/*
	* @brief	Utf16ToUtf32_BE	将大端utf16字符转换为大端utf32字符
	* @param	[In]a param of Type uint16_t * pUtf16	大端utf16字符	
	* @param	[In]a param of Type long nUtf16Count		大端utf16字符个数
	* @param	[Out]a param of Type uint32_t * & pUtf32	保存转换出来的大端utf32字符串,包含结束符。使用完后请使用(delete[])进行删除
	* @return   long	转换成功返回utf32字符个数，失败返回-1
	*/
	long Utf16ToUtf32_BE(const uint16_t* pUtf16, long nUtf16Count, uint32_t*& pUtf32);

	/*
	* @brief	Utf32ToUtf16_BE	将大端utf32字符转换为大端utf16字符
	* @param	[In]a param of Type uint32_t * pUtf32	大端utf32字符	
	* @param	[In]a param of Type long nUtf32Count		大端utf32字符个数
	* @param	[Out]a param of Type uint16_t * & pUtf16		保存转换出来的大端utf16字符串,包含结束符。使用完后请使用(delete[])进行删除
	* @return   long	转换成功返回utf16字符个数，失败返回-1
	*/
	long Utf32ToUtf16_BE(const uint32_t* pUtf32, long nUtf32Count, uint16_t*& pUtf16);

	/*
	* @brief	Utf16ToUtf32_LE	将小端utf16字符转换为小端utf32字符
	* @param	[In]a param of Type uint16_t * pUtf16	小端utf16字符	
	* @param	[In]a param of Type long nUtf16Count		小端utf16字符个数
	* @param	[Out]a param of Type uint32_t * & pUtf32	保存转换出来的小端utf32字符串,包含结束符。使用完后请使用(delete[])进行删除
	* @return   long	转换成功返回utf32字符个数，失败返回-1
	*/
	long Utf16ToUtf32_LE(const uint16_t* pUtf16, long nUtf16Count, uint32_t*& pUtf32);

	/*
	* @brief	Utf32ToUtf16_LE	将小端utf32字符转换为小端utf16字符
	* @param	[In]a param of Type uint32_t * pUtf32	小端utf32字符	
	* @param	[In]a param of Type long nUtf32Count		小端utf32字符个数
	* @param	[Out]a param of Type uint16_t * & pUtf16		保存转换出来的小端utf16字符串,包含结束符。使用完后请使用(delete[])进行删除
	* @return   long	转换成功返回utf16字符个数，失败返回-1
	*/
	long Utf32ToUtf16_LE(const uint32_t* pUtf32, long nUtf32Count, uint16_t*& pUtf16);

	/*
	 *  @brief   	Utf82Gbk	将UTF8字符串转换为GBK编码字符串 
	 *	@param 		[In]a param of Type  char * pUtf8	UTF8编码字符串
	 *	@param 		[In]a param of Type  long nUtf8Count	UTF8编码字符串长度
	 *	@param 		[In]a param of Type  char * & pGbk	保存转换出来的GBK字符串,包含结束符。使用完后请使用(delete[])进行删除
	 *  @return 	long	 转换成功后返回GBK字符个数，失败返回-1
	 */
	long Utf82Gbk(const char* pUtf8, long nUtf8Count, char*& pGbk);

	/*
	 *  @brief   	Gbk2Utf8	将GBK字符串转换为UTF8编码字符串 
	 *	@param 		[In]a param of Type  char * pGbk	GBK编码字符串
	 *	@param 		[In]a param of Type  long nGbkCount	GBK编码字符串长度
	 *	@param 		[In]a param of Type  char * & pUtf8	保存转换出来的UTF8字符串,包含结束符。使用完后请使用(delete[])进行删除
	 *  @return 	long	 转换成功后返回UTF8字符个数，失败返回-1
	 */
	long Gbk2Utf8(const char* pGbk, long nGbkCount, char*& pUtf8);

private:
	/*
	* @brief	swap16	 反转一个16位值的保存方式. 大小端反转
	* @param	[In/Out]a param of Type uint16_t & w		要反转的值
	* @return   void	
	*/
	void swap16(uint16_t& w);

	/*
	* @brief	swap32	 反转一个32位值的保存方式. 大小端反转
	* @param	[In/Out]a param of Type uint32_t & w		要反转的值
	* @return   void	
	*/
	void swap32(uint32_t& w);

	/*
	* @brief	IsLittleEndian	检查系统是否使用小端模式
	* @return   bool	使用小端模式返回true,使用大端模式返回false
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
