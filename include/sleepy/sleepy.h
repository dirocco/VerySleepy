#ifndef SLEEPY_H
#define SLEEPY_H

void SleepySubscribeThisThread();
void SleepyUnsubscribeThisThread();


// The rest of this file can be put in a cpp if you prefer

#define SLEEPY_SUBSCRIBE_MESSAGE_TYPE 0x01
#define SLEEPY_UNSUBSCRIBE_MESSAGE_TYPE 0x02

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

void SleepySendMessage(SleepyMessage* message)
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
			fprintf(stderr, "Sleepy subscription open pipe failed: %d\n", err);
			return;
		}

		if(!WaitNamedPipeA(pipename, 10000))
		{
			err = GetLastError();
			fprintf(stderr, "Sleepy subscription wait for pipe failed: %d\n", err);
			return;
		}
	}

	DWORD bytesWritten = 0;
	BOOL result = WriteFile(pipe, (void*)message, sizeof(SleepyMessage), &bytesWritten, NULL);
	if(!result)
	{
		fprintf(stderr, "Sleepy subscription write failed: %d\n", GetLastError());
		return;
	}

	CloseHandle(pipe);
}

void SleepySubscribeThisThread()
{
	SleepyMessage msg;
	msg.messageType = SLEEPY_SUBSCRIBE_MESSAGE_TYPE;
	msg.threadId = GetCurrentThreadId();

	SleepySendMessage(&msg);
}

void SleepyUnsubscribeThisThread()
{
	SleepyMessage msg;
	msg.messageType = SLEEPY_UNSUBSCRIBE_MESSAGE_TYPE;
	msg.threadId = GetCurrentThreadId();

	SleepySendMessage(&msg);
}

#undef SLEEPY_SUBSCRIBE_MESSAGE_TYPE
#undef SLEEPY_UNSUBSCRIBE_MESSAGE_TYPE

#endif