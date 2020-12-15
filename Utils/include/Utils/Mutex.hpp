#ifndef __MUTEX_HPP__
#define __MUTEX_HPP__

#include <semaphore.h>

#include <string>
/**
 * This class is a wrapper for the semaphore to make it work like
 * a mutex and easier to use. 
 */
class Mutex
{
	std::string name_;
	sem_t *mtx_;

public:
	Mutex(const std::string &name);
	~Mutex();
	void lock();
	void unlock();
};

#endif	// __MUTEX_HPP__
