#ifndef __UNIQUE_PTR_HPP__
#define __UNIQUE_PTR_HPP__

#include <iostream>

template <class T>
class deleter
{
public:
	void operator()(T *el)
	{
		delete el;
	}
};
template <class T, class Del = deleter<T> >
class Unique_ptr
{
	T *ptr_;
	Del del_;

public:
	Unique_ptr(T *ptr, Del del = Del()) : ptr_(ptr), del_(del) {}
	~Unique_ptr() { del_(ptr_); }
	T *get()
	{
		return ptr_;
	}
	operator const T *() const
	{
		return ptr_;
	}
	T &operator*()
	{
		return *ptr_;
	}
};
#endif	// __UNIQUE_PTR_HPP__
