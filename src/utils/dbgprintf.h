#ifndef __DBGPRINT_H__
#define __DBGPRINT_H__

#if _DEBUG
#define dbgprintf( ... ) do {						\
	char buf[512] = {0};							\
	_snprintf_s( buf, 511, 511, __VA_ARGS__ );		\
	OutputDebugStringA( buf );						\
	} while(false)
#else
#define dbgprintf( ... ) do {} while(false)
#endif

#endif
