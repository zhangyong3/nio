#include "thread.h"

#define THREAD_STACK_SIZE (1024*800)

Thread::Thread()
{
}

Thread::~Thread()
{
}

bool Thread::start()
{
	bool rets;
	pthread_attr_t thrattr;
	pthread_attr_init(&thrattr);

	pthread_attr_setscope(&thrattr, PTHREAD_SCOPE_SYSTEM);
	pthread_attr_setstacksize(&thrattr, THREAD_STACK_SIZE);

	rets = (pthread_create(&thrId, &thrattr, (void*(*)(void*))Thread::_runProc, this) == 0);
	pthread_attr_destroy(&thrattr);

	return rets;
}

void Thread::join()
{
	void* attr=0;
	pthread_join(thrId, &attr);
}


unsigned int Thread::_runProc(Thread *thr)
{
	thr->run();
	return 0;
}
