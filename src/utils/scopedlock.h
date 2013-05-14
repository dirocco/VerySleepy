#ifndef SCOPED_LOCK_H
#define SCOPED_LOCK_H

#include <windows.h>

class ScopedLock
{
public:
	ScopedLock( CRITICAL_SECTION& lock )
		: l(lock)
	{
		EnterCriticalSection(&l);
	}

	~ScopedLock()
	{
		LeaveCriticalSection(&l);
	}

	CRITICAL_SECTION& l;
};

#endif
