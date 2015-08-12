// Copyright (C) 2015 SRG Technology, LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "anyrpc/api.h"
#include "anyrpc/internal/time.h"

#if defined(ANYRPC_THREADING)
#include <thread>
#include <chrono>
#endif

#if defined(_MSC_VER)
#elif defined(__MINGW32__)
#include <unistd.h>
#else
#include <time.h>
#endif

namespace anyrpc
{

#ifdef WIN32
int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
	// Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
	static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

	SYSTEMTIME  system_time;
	FILETIME    file_time;
	uint64_t    time;

	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	time = ((uint64_t)file_time.dwLowDateTime);
	time += ((uint64_t)file_time.dwHighDateTime) << 32;

	tp->tv_sec = (long)((time - EPOCH) / 10000000L);
	tp->tv_usec = (long)(system_time.wMilliseconds * 1000);
	return 0;
}
#endif

#if defined(__MINGW32__)
struct tm* localtime_r(const time_t *timep, struct tm *result)
{
	// with Windows localtime is threadsafe since the pointer is to thread local storage
	struct tm *t=localtime(timep);
	memcpy(result,t,sizeof(struct tm));
	return result;
}
#endif

int MilliTimeDiff( struct timeval &time1, struct timeval &time2 )
{
    return (time1.tv_sec - time2.tv_sec) * 1000 + (time1.tv_usec - time2.tv_usec)/1000;
}

int64_t MicroTimeDiff( struct timeval &time1, struct timeval &time2 )
{
    return (int64_t)(time1.tv_sec - time2.tv_sec) * 1000000 + (int64_t)(time1.tv_usec - time2.tv_usec);
}

void MilliSleep(unsigned ms)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
    Sleep(ms);
#else
    struct timespec ts;
    ts.tv_sec = ms/1000;
    ts.tv_nsec = (ms - 1000*ts.tv_sec) * 1000000;
    nanosleep(&ts,NULL);
#endif
}

} // namespace anyrpc
