#ifndef _THREAD_H
#define _THREAD_H

#include <pthread.h>

class Thread
{
public:
	Thread();
	virtual ~Thread();

	bool start();

	void join();

	virtual void run() = 0;
	virtual void stop() {} 
private:
	static unsigned int _runProc(Thread *thr);	

protected:
	pthread_t thrId;
};

#endif
