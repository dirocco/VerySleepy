#ifndef __SLEEPY_H__
#define __SLEEPY_H__

bool SleepySubscribeThisThread();
bool SleepyUnsubscribeThisThread();


// The rest of this file can be put in a cpp if you prefer

#define SLEEPY_SUBSCRIBE_MESSAGE_TYPE		1
#define SLEEPY_UNSUBSCRIBE_MESSAGE_TYPE		2

#pragma pack( push )
#pragma pack( 1 )

struct SleepyMessage
{
	int messageType;
	int threadId;
};

#pragma pack( pop )

#ifndef WIN32_LEAN_AND_MEAN
#define SLEEPY_DEFINED_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef SLEEPY_DEFINED_LEAN_AND_MEAN
#undef SLEEPY_DEFINED_LEAN_AND_MEAN
#undef WIN32_LEAN_AND_MEAN
#endif

bool SleepySendMessage(SleepyMessage* message)
{
	char pipename[MAX_PATH] = {0};
	_snprintf_s(pipename, MAX_PATH - 1, MAX_PATH - 1, "\\\\.\\pipe\\SleepyPipe-%d", GetCurrentProcessId());

	HANDLE pipe = NULL;
	while( true )
	{
		pipe = CreateFileA(pipename, 
			GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);

		if(pipe != INVALID_HANDLE_VALUE)
			break;

		int err = GetLastError();
		if(err != ERROR_PIPE_BUSY)
		{
			fprintf(stderr, "Sleepy send message open pipe failed: %d\n", err);
			return false;
		}

		if(!WaitNamedPipeA(pipename, 1000))
		{
			err = GetLastError();
			fprintf(stderr, "Sleepy send message wait for pipe failed: %d, retrying\n", err);
			continue;
		}
	}

	DWORD bytesWritten = 0;
	BOOL result = WriteFile(pipe, (void*)message, sizeof(SleepyMessage), &bytesWritten, NULL);
	if(!result)
	{
		fprintf(stderr, "Sleepy subscription write failed: %d\n", GetLastError());
		return false;
	}

	CloseHandle(pipe);
	return true;
}

bool SleepySubscribeThisThread()
{
	SleepyMessage msg;
	msg.messageType = SLEEPY_SUBSCRIBE_MESSAGE_TYPE;
	msg.threadId = GetCurrentThreadId();

	return SleepySendMessage(&msg);
}

bool SleepyUnsubscribeThisThread()
{
	SleepyMessage msg;
	msg.messageType = SLEEPY_UNSUBSCRIBE_MESSAGE_TYPE;
	msg.threadId = GetCurrentThreadId();

	return SleepySendMessage(&msg);
}

#endif