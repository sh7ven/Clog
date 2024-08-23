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

#define CLOG_VERSION "0.3"

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
#include <pwd.h>
#include <unistd.h>
#endif

#if defined(_WIN32) || defined(_WIN64)
#define CLOG_HAS_WINDOWS
#include <windows.h>
#include <shlobj.h>
#include <io.h>
#define access _access
#endif

/* TODO: Clang specific attribute(s)? */
#define CLOG_INTERNAL static

/// Constructor with defined integral priority.
#define CLOG_CONSTRUCTOR(n) __attribute__((constructor(n)))
#define CLOG_DESTRUCTOR 	__attribute__((  destructor  ))

/// Global mutex handle.
CLOG_INTERNAL pthread_mutex_t clog_mutex_global = PTHREAD_MUTEX_INITIALIZER;

/// Initialize the library.
CLOG_INTERNAL CLOG_CONSTRUCTOR(101) void clog_init(void) {
	pthread_mutex_init(&clog_mutex_global, NULL);
}

/// Lock the global mutex.
CLOG_INTERNAL void clog_lock(bool lock) {
	// imagine trying to write maintainable code
	lock? (pthread_mutex_lock(&clog_mutex_global)) : (pthread_mutex_unlock(&clog_mutex_global));
}

/// Priority of the logged message.
typedef enum {
	CLOG_INFO,
	CLOG_DEBUG,
	CLOG_WARN,
	CLOG_ERROR
} clog_priority;

CLOG_INTERNAL const uint8_t CLOG_MIN_PRIORITY = 0,
							CLOG_MAX_PRIORITY = 4;

CLOG_INTERNAL bool clog_will_backup = false;
CLOG_INTERNAL char clog_backup_fp[1024]; /* Can paths get any bigger? :P */
CLOG_INTERNAL FILE* clog_backup_file = NULL;

/* Does a file at `path` exist? */
CLOG_INTERNAL bool exists(const char* path) {
	return access(path, F_OK) == 0;
}

/// define a platform-specific backup file for Clog.
CLOG_INTERNAL CLOG_CONSTRUCTOR(102) void clog_define_backup(void) {
#ifdef CLOG_HAS_LINUX
	// pre-defined config directory.
	if (getenv("XDG_CONFIG_HOME") != NULL)
		strcpy(clog_backup_fp, getenv("XDG_CONFIG_HOME"));
	else
		strcpy(clog_backup_fp, getenv("HOME")); /* $HOME/.config */
	// TODO: HOME might return NULL as well.
#else
	char backup_fp[1024];
	/* TODO: alternate path if SHGetKnownFolderPath does not return `SUCCESS`. */
	SHGetKnownFolderPath(NULL, CSIDL_APPDATA, NULL, backup_fp);
#endif

	strcat(clog_backup_fp, "/clogfile");
	
	printf("Test: %s\n", clog_backup_fp);

	if (!exists(clog_backup_fp)) 
		clog_backup_file = fopen(clog_backup_fp, "w+"); // initial file creation
	else
		clog_backup_file = fopen(clog_backup_fp,  "a");
}

/// Tell Clog to backup logs to a file as well.
void clog_backup_logs(bool backup) { 
	clog_will_backup = backup; 
}

/// hardcoded string sizes to prevent calling `strlen()` again and again.
CLOG_INTERNAL const uint8_t CLOG_PRIORITY_STRING_SIZES[] = { 14, 15, 17, 15 };

CLOG_INTERNAL const char* CLOG_PRIORITY_STRINGS[] = {
	"\x1b[32m[INFO]\x1b[m",
	"\x1b[33m[DEBUG]\x1b[m",
	"\x1b[35m[WARNING]\x1b[m",
	"\x1b[31m[ERROR]\x1b[m"
};

CLOG_INTERNAL const char* CLOG_PRIORITY_STRINGS_NOESC[] = { "[INFO]", "[DEBUG]", "[WARNING]", "[ERROR]" };

#define CLOG_TIMESTAMP_FMT "%H:%M:%S"
#define CLOG_TIMESTAMP_FMT_MAX_SIZE 9

/// Logs a message described by `fmt` into a `stream` (for instance, `stdout`).
void clog_log(clog_priority priority, FILE* stream, const char* fmt, ...) {
	assert(priority >= CLOG_MIN_PRIORITY && priority <= CLOG_MAX_PRIORITY);
	clog_lock(1);

	va_list arglist, argcopy;
	va_start(arglist, fmt);
	va_copy(argcopy, arglist);

	char timestamp[CLOG_TIMESTAMP_FMT_MAX_SIZE];
	time_t now = time(NULL);
	strftime(timestamp, CLOG_TIMESTAMP_FMT_MAX_SIZE+1, CLOG_TIMESTAMP_FMT, localtime(&now));

	unsigned long fmt_msg_len = vsnprintf(NULL, 0, fmt, arglist);
	char* fmt_msg = (char*) malloc(fmt_msg_len);
	assert(fmt_msg != NULL);

	vsnprintf(fmt_msg, fmt_msg_len+1, fmt, argcopy);

	fprintf(stream, "%s %s %s\n", timestamp, CLOG_PRIORITY_STRINGS[priority], fmt_msg);

	if (clog_will_backup && clog_backup_file)
		fprintf(clog_backup_file, "%s %s %s\n", timestamp, CLOG_PRIORITY_STRINGS_NOESC[priority], fmt_msg);

	free(fmt_msg);
	va_end(argcopy);
	va_end(arglist);

	clog_lock(0);
}

CLOG_INTERNAL CLOG_DESTRUCTOR void clog_exit(void) {
	pthread_mutex_destroy(&clog_mutex_global);
	
	fflush(clog_backup_file);
	free(clog_backup_file);
}



/// Utility macros for quickly logging a message to stdout.
#define clog_info(...)  clog_log(CLOG_INFO,  __VA_ARGS__)
#define clog_debug(...) clog_log(CLOG_DEBUG, __VA_ARGS__)
#define clog_warn(...)  clog_log(CLOG_WARN,  __VA_ARGS__)
#define clog_err(...)   clog_log(CLOG_ERROR, __VA_ARGS__)

#endif // CLOG_H
