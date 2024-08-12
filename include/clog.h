/*
Copyright 2024 Shiven Kashyap

Permission is hereby granted, free of charge, to any person obtaining 
a copy of this software and associated documentation files (the “Software”), 
to deal in the Software without restriction, including without limitation 
the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software 
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in 
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CLOG_H
#define CLOG_H

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
// TODO: better OS-classification for complete cross-platform support


#if defined(__linux__) || defined(__unix__)
#define CLOG_HAS_LINUX
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define CLOG_HAS_WINDOWS
#include <windows.h>
#endif


static pthread_mutex_t clog_mutex_global = PTHREAD_MUTEX_INITIALIZER;
static inline void clog_lock(bool) __attribute__((			   always_inline));
static inline void clog_init(void) __attribute__((constructor, always_inline));
static inline void clog_exit(void) __attribute__(( destructor, always_inline));

typedef enum {
	CLOG_INFO,
	CLOG_DEBUG,
	CLOG_WARN,
	CLOG_ERROR
} clog_priority;

static const uint8_t CLOG_MIN_PRIORITY = 0,
					 CLOG_MAX_PRIORITY = 4;

static bool clog_will_backup = false;

/// If set to `true`, Clog will save outputs (as backups) to a log file. See `clog_config_log()`.
extern void clog_backup_log(bool) __attribute__((always_inline));

// TODO: make dir for Clog specific log files. (probably a shell script)
static char* clog_logfile_path = NULL;

static const FILE* clog_logfile = NULL;

/// Set destination for storing Clog outputs. 
extern void clog_config_log(const char* file_path) __attribute__((always_inline));

/// hardcoded string sizes to prevent calling `strlen()` again and again.
static const uint8_t CLOG_PRIORITY_STRING_SIZES[] = { 14, 15, 17, 15 };

static const char* CLOG_PRIORITY_STRINGS[] = {
	"\x1b[32m[INFO]\x1b[m",
	"\x1b[33m[DEBUG]\x1b[m",
	"\x1b[35m[WARNING]\x1b[m",
	"\x1b[31m[ERROR]\x1b[m"
};

/// (internal) Timestamp format. Changes in `clog_set_time_fmt`.
static char* CLOG_TIMESTAMP_FMT = "%H:%M:%S";

/* (internal) Maximum size for a timestamp string, 
 * to prevent cluttering of the output message. */
static const uint8_t CLOG_TIMESTAMP_FMT_MAX_SIZE = 32;
#define CLOG_TIMESTAMP_FMT_SIZE strlen(CLOG_TIMESTAMP_FMT)

/// Logs a message described by `fmt` into a `stream` (for instance, `stdout`).
extern void clog_log(clog_priority priority, FILE* stream, const char* fmt, ...);

/// Utility macros for quickly logging a message to stdout.
#define clog_info(...)  clog_log(CLOG_INFO,  __VA_ARGS__)
#define clog_debug(...) clog_log(CLOG_DEBUG, __VA_ARGS__)
#define clog_warn(...)  clog_log(CLOG_WARN,  __VA_ARGS__)
#define clog_err(...)   clog_log(CLOG_ERROR, __VA_ARGS__)

/// Set format of the timestamp in a logged output. 
extern void clog_set_time_fmt(char* format) __attribute__((always_inline));

#endif
