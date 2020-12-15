#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>

#include <Utils/Mutex.hpp>
#include <cstring>
#include <iostream>

// Mutex::Mutex() : name_("") {}
Mutex::Mutex(const std::string &name) : name_(name)
{
	sem_unlink(name.c_str());
	// if (sem_unlink(name.c_str()) == -1)
	// 	std::cerr << "sem_unlink failed\n";
	// mtx_ = sem_open(name.c_str(), O_CREAT, S_IRWXU);
	mtx_ = sem_open(name.c_str(), O_CREAT | O_EXCL, S_IRWXU, 1);
	if (mtx_ == SEM_FAILED)
		throw std::runtime_error(strerror(errno));
}
// Mutex::Mutex(sem_t *mtx) : mtx_(mtx) {}
Mutex::~Mutex()
{
	if (!name_.empty())
	{
		// std::cout << "closing semaphore\n";
		if (sem_close(mtx_) == -1)
			std::cerr << "sem_close failed\n";
		// if (sem_unlink(name_.c_str()) == -1)
		// 	std::cerr << "sem_unlink failed\n";
		sem_unlink(name_.c_str());
	}
}
void Mutex::lock()
{
	sem_wait(mtx_);
}
void Mutex::unlock()
{
	sem_post(mtx_);
}
