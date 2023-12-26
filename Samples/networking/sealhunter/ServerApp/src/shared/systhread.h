#ifndef __SYSTHREAD_H__
#define __SYSTHREAD_H__

#include "iltthread.h"
#include "iltthreadevent.h"
#include "iltcriticalsection.h"


#ifdef __LINUX

#include "../Linux/linux_ltthread.h"
#include "../Linux/linux_ltthreadevent.h"
#include "../Linux/linux_ltcriticalsection.h"

typedef CLinux_LTThread			 		  CLTThread;
typedef CLinux_LTThreadEvent<AutoReset>	  CLTThreadEvent;
typedef CLinux_LTThreadEvent<ManualReset> CLTThreadEventManual;
typedef CLinux_LTCriticalSection 		  CLTCriticalSection;

#elif _WIN32

#include "../win32/win32_ltthread.h"
#include "../win32/win32_ltthreadevent.h"
#include "../win32/win32_ltcriticalsection.h"

typedef CWin32_LTThread					  CLTThread;
typedef CWin32_LTThreadEvent<AutoReset>	  CLTThreadEvent;
typedef CWin32_LTThreadEvent<ManualReset> CLTThreadEventManual;
typedef CWin32_LTCriticalSection		  CLTCriticalSection;

#endif

#endif // __SYSTHREAD_H__
