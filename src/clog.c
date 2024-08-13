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

#include "clog.h"

static void clog_lock(bool lock) {
	// imagine trying to write maintainable code
	lock? (pthread_mutex_unlock(&clog_mutex_global)): (pthread_mutex_lock(&clog_mutex_global));
}

inline void clog_backup_log(bool maintain) {
	clog_will_backup = maintain;
}

inline void clog_config_log(const char* file_path) {
#ifdef CLOG_HAS_LINUX
	assert(file_path != NULL && access(file_path, W_OK) == 0);
#else 
	assert(GetFileAttributes(path) != INVALID_FILE_ATTRIBUTES);
#endif

	clog_lock(1);
	clog_logfile = fopen(file_path, "a");
	clog_lock(0);
}

static void clog_init(void) {
	pthread_mutex_init(&clog_mutex_global, NULL);
}

static void clog_exit(void) {
	pthread_mutex_destroy(&clog_mutex_global);
}

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

	free(fmt_msg);
	va_end(argcopy);
	va_end(arglist);

	clog_lock(0);
}


void clog_set_time_fmt(char* format) {
	assert(format != NULL);
	CLOG_TIMESTAMP_FMT = format;
}
