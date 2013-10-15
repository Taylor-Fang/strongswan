/*
 * Copyright (C) 2013 Martin Willi
 * Copyright (C) 2013 revosec AG
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.  See <http://www.fsf.org/copyleft/gpl.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */

/**
 * @defgroup windows windows
 * @{ @ingroup utils
 */

#ifndef WINDOWS_H_
#define WINDOWS_H_

#include <winsock2.h>
#include <ws2tcpip.h>
#include <direct.h>
#include <inttypes.h>

/* undef Windows variants evaluating values more than once */
#undef min
#undef max

/* interface is defined as an alias to "struct" in basetypes.h, but
 * we use it here and there as ordinary identifier. */
#undef interface

/* used by Windows API, but we have our own */
#undef CALLBACK

/* UID/GID types for capabilities, even if not supported */
typedef u_int uid_t;
typedef u_int gid_t;

/**
 * Replacement for random(3)
 */
static inline long random(void)
{
	return rand();
}

/**
 * Replacement for srandom(3)
 */
static inline void srandom(unsigned int seed)
{
	srand(seed);
}

/**
 * Provided via ws2_32
 */
const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);

/**
 * Provided via ws2_32
 */
int inet_pton(int af, const char *src, void *dst);

/**
 * Provided by printf hook backend
 */
int asprintf(char **strp, const char *fmt, ...);

/**
 * Provided by printf hook backend
 */
int vasprintf(char **strp, const char *fmt, va_list ap);

/**
 * timeradd(3) from <sys/time.h>
 */
static inline void timeradd(struct timeval *a, struct timeval *b,
							struct timeval *res)
{
	res->tv_sec = a->tv_sec + b->tv_sec;
	res->tv_usec = a->tv_usec + b->tv_usec;
	if (res->tv_usec >= 1000000)
	{
		res->tv_usec -= 1000000;
		res->tv_sec++;
	}
}

/**
 * timersub(3) from <sys/time.h>
 */
static inline void timersub(struct timeval *a, struct timeval *b,
							struct timeval *res)
{
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_usec = a->tv_usec - b->tv_usec;
	if (res->tv_usec < 0)
	{
		res->tv_usec += 1000000;
		res->tv_sec--;
	}
}

/**
 * gmtime_r(3) from <time.h>
 */
static inline struct tm *gmtime_r(const time_t *timep, struct tm *result)
{
	struct tm *ret;

	/* gmtime_s() and friends seem not to be implemented/functioning.
	 * Relying on gmtime() on Windows works as well, as it uses thread
	 * specific buffers. */
	ret = gmtime(timep);
	if (ret)
	{
		memcpy(result, ret, sizeof(*result));
	}
	return ret;
}

/**
 * localtime_r(3) from <time.h>
 */
static inline struct tm *localtime_r(const time_t *timep, struct tm *result)
{
	struct tm *ret;

	/* localtime_s() and friends seem not to be implemented/functioning.
	 * Relying on localtime() on Windows works as well, as it uses thread
	 * specific buffers. */
	ret = localtime(timep);
	if (ret)
	{
		memcpy(result, ret, sizeof(*result));
	}
	return ret;
}

/**
 * setenv(3) from <stdlib.h>, overwrite flag is ignored
 */
static inline int setenv(const char *name, const char *value, int overwrite)
{
	if (SetEnvironmentVariableA(name, value) == 0)
	{	/* failed */
		return -1;
	}
	return 0;
}

/**
 * dlerror(3) from <dlfcn.h>, printing error to an alloca() buffer
 */
#define dlerror() \
({ \
	char buf[128], *out;\
	ssize_t len; \
	DWORD err; \
	err = GetLastError(); \
	len = FormatMessage(0, NULL, err, 0, buf, sizeof(buf), NULL); \
	if (len <= 0) \
	{ \
		len = snprintf(buf, sizeof(buf), "(%u)", err); \
	} \
	len++; \
	out = alloca(len); \
	memcpy(out, buf, len); \
	out; \
})

/**
 * Lazy binding, ignored on Windows
 */
#define RTLD_LAZY 1

/**
 * dlopen(3) from <dlfcn.h>
 */
static inline void *dlopen(const char *filename, int flag)
{
	return LoadLibrary(filename);
}

/**
 * Default handle targeting .exe
 */
#define RTLD_DEFAULT (NULL)

/**
 * Find symbol in next library
 */
#define RTLD_NEXT ((void*)~(uintptr_t)0)

/**
 * dlsym() from <dlfcn.h>
 */
static inline void *dlsym(void *handle, const char *symbol)
{
	if (handle == RTLD_DEFAULT)
	{
		handle = GetModuleHandle(NULL);
	}
	else if (handle == RTLD_NEXT)
	{
		if (strcmp(symbol, "malloc") == 0 ||
			strcmp(symbol, "realloc") == 0 ||
			strcmp(symbol, "free") == 0)
		{
			/* for leak-detective */
			handle = GetModuleHandle("msvcrt");
		}
		else
		{
			return NULL;
		}
	}
	if (handle)
	{
		return GetProcAddress((HMODULE)handle, symbol);
	}
	return NULL;
}

/**
 * dlclose() from <dlfcn.h>
 */
static inline int dlclose(void *handle)
{
	return FreeLibrary((HMODULE)handle);
}

/* Windows does not support "ll" format printf length modifiers. Mingw
 * therefore maps these to the Windows specific I64 length modifier. That
 * won't work for us, as we use our own printf backend on Windows, which works
 * just fine with "ll". */
#undef PRId64
#define PRId64 "lld"
#undef PRId64
#define PRId64 "lld"
#undef PRIdLEAST64
#define PRIdLEAST64 "lld"
#undef PRIdFAST64
#define PRIdFAST64 "lld"
#undef PRIdMAX
#define PRIdMAX "lld"
#undef PRIi64
#define PRIi64 "lli"
#undef PRIiLEAST64
#define PRIiLEAST64 "lli"
#undef PRIiFAST64
#define PRIiFAST64 "lli"
#undef PRIiMAX
#define PRIiMAX "lli"
#undef PRIo64
#define PRIo64 "llo"
#undef PRIoLEAST64
#define PRIoLEAST64 "llo"
#undef PRIoFAST64
#define PRIoFAST64 "llo"
#undef PRIoMAX
#define PRIoMAX "llo"
#undef PRIu64
#define PRIu64 "llu"
#undef PRIuLEAST64
#define PRIuLEAST64 "llu"
#undef PRIuFAST64
#define PRIuFAST64 "llu"
#undef PRIuMAX
#define PRIuMAX "llu"
#undef PRIx64
#define PRIx64 "llx"
#undef PRIxLEAST64
#define PRIxLEAST64 "llx"
#undef PRIxFAST64
#define PRIxFAST64 "llx"
#undef PRIxMAX
#define PRIxMAX "llx"
#undef PRIX64
#define PRIX64 "llX"
#undef PRIXLEAST64
#define PRIXLEAST64 "llX"
#undef PRIXFAST64
#define PRIXFAST64 "llX"
#undef PRIXMAX
#define PRIXMAX "llX"

#ifdef _WIN64
# undef PRIdPTR
# define PRIdPTR "lld"
# undef PRIiPTR
# define PRIiPTR "lli"
# undef PRIoPTR
# define PRIoPTR "llo"
# undef PRIuPTR
# define PRIuPTR "llu"
# undef PRIxPTR
# define PRIxPTR "llx"
# undef PRIXPTR
# define PRIXPTR "llX"
#endif /* _WIN64 */

#endif /** WINDOWS_H_ @}*/
