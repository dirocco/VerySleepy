VerySleepy
==========

Very Sleepy 0.82 modified to allow the profiled program to attach/deattach specific threads at runtime.

Thread Subscription
-------------------

To use this feature, include sleepy/sleepy.h in your code and call SleepySubscribeThisThread()/SleepyUnsubscribeThisThread().

When launching Very Sleepy, use the /s commandline switch to enable the subscription listening functionality.

Example: sleepy /s /r "myprogram.exe -my -program -arguments"


Show GUI flag
-------------

Use the /g flag to hide the GUI (useful for running sleepy from scripts).

Example: sleepy /g /r "myprogram.exe" /o capture.sleepy


