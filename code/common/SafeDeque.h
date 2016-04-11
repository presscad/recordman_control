#ifndef Safe_Deque_h__
#define Safe_Deque_h__


#include <deque>
#include "SafeLock.h"

/**
 * @defgroup 	CSafeDeque	类说明
 * @{
 */
/**
 * @brief       通用数据deque，包含互斥锁的封装，支持std::deque的大部分操作（除了各比较操作符）
*  @author      pengl
 * @version     ver1.0
 * @date        01/10/08

 *
 * example
 * @code
 *    
 * @endcode
 */
using namespace std;
template <class Type>

class CSafeDeque {
  public:
	typedef CSafeDeque<Type> _Myt;
	typedef typename deque<Type>::allocator_type allocator_type;
	typedef typename deque<Type>::size_type size_type;
	typedef typename deque<Type>::difference_type difference_type;
	typedef typename deque<Type>::iterator iterator;
	typedef typename deque<Type>::const_iterator const_iterator;
	typedef typename deque<Type>::reverse_iterator reverse_iterator;
	typedef typename deque<Type>::const_reverse_iterator const_reverse_iterator;
	typedef typename deque<Type>::reference reference;
	typedef typename deque<Type>::const_reference const_reference;
	typedef typename deque<Type>::value_type value_type;
  public:
	explicit CSafeDeque(const allocator_type& _Al = allocator_type())
		: mDeque(_Al),mLock("CSafeDeque")
	{
	}

	~CSafeDeque(void)
	{
	}

	iterator begin(void)
	{
		CLockUp locallock(&mLock);
		return (mDeque.begin());
	}

	const_iterator begin(void)	const
	{
		return (mDeque.begin());
	}

	iterator end(void)
	{
		CLockUp locallock(&mLock);
		return (mDeque.end());
	}

	const_iterator end(void)	const
	{
		return (mDeque.end());
	}

	reverse_iterator rbegin()
	{
		CLockUp locallock(&mLock);
		return (mDeque.rbegin());
	}

	const_reverse_iterator rbegin() const
	{
		return (mDeque.rbegin());
	}

	reverse_iterator rend()
	{
		CLockUp locallock(&mLock);
		return (mDeque.rend());
	}

	const_reverse_iterator rend() const
	{
		return (mDeque.rend());
	}

	void resize(int cSize)
	{
		CLockUp locallock(&mLock);
		mDeque.resize(cSize);
	}

	int size(void)	const
	{
		return (mDeque.size());
	}

	int max_size(void) const
	{
		return (mDeque.max_size());
	}

	bool empty(void) const
	{
		return (mDeque.empty());
	}

	allocator_type get_allocator() const
	{
		return (mDeque.get_allocator()); 
	}

	reference front(void)
	{
		CLockUp locallock(&mLock);
		return (mDeque.front());
	}

	const_reference front(void)	const
	{
		return (mDeque.front());
	}

	reference back(void)
	{
		CLockUp locallock(&mLock);
		return (mDeque.back());
	}

	const_reference back(void)	const
	{
		return (mDeque.back());
	}

	void push_front(const Type& element)
	{
		CLockUp locallock(&mLock);
		mDeque.push_front(element);
	}

	void pop_front(void)
	{
		CLockUp locallock(&mLock);
		mDeque.pop_front();
	}

	void push_back(const Type& element)
	{
		CLockUp locallock(&mLock);
		mDeque.push_back(element);
	}

	void pop_back(void)
	{
		CLockUp locallock(&mLock);
		mDeque.pop_back();
	}

	void assign(const_iterator CIteBegin, const_iterator CIteEnd)
	{
		CLockUp locallock(&mLock);
		mDeque.assign(CIteBegin,CIteEnd);
	}

	void assign(size_type cNum, const Type& element)
	{
		CLockUp locallock(&mLock);
		mDeque.assign(cNum,element);
	}

	iterator insert(iterator cIte, const Type& element)
	{
		CLockUp locallock(&mLock);
		mDeque.insert(cIte, element);
	}

	void insert(iterator cIte, size_type cNum, const Type& element)
	{
		CLockUp locallock(&mLock);
		mDeque.insert(cIte, cNum, element);
	}

	void insert(iterator cIte, const_iterator CIteBegin, const_iterator CIteEnd)
	{
		CLockUp locallock(&mLock);
		mDeque.insert(cIte, CIteBegin, CIteEnd);
	}

	iterator erase(iterator cIte)
	{
		CLockUp locallock(&mLock);
		mDeque.erase(cIte);
	}

	iterator erase(iterator cIteS, iterator cIteE)
	{
		CLockUp locallock(&mLock);
		mDeque.erase(cIteS, cIteE);
	}

	void clear(void)
	{
		CLockUp locallock(&mLock);
		mDeque.clear();
	}

	bool get_front_and_pop(Type& front_element)
	{
		CLockUp locallock(&mLock);

		if (mDeque.empty() == true)
		{
			return false;
		}

		front_element = mDeque.front();

		mDeque.pop_front();

		return true;
	}

	bool get_back_and_pop(Type& back_element)
	{
		CLockUp locallock(&mLock);

		if (mDeque.empty() == true)
		{
			return false;
		}

		back_element = mDeque.back();

		mDeque.pop_back();

		return true;
	}

  private:
	deque<Type> mDeque;

	CSafeLock mLock;
};
/** @} */ //OVER
#endif // Safe_Deque_h__