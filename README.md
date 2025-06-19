![sample warning](/screenshots/sample_usage_warning.png)

# Clog
A header-only, simple and thread-safe logging library written in C.

> [!WARNING]
> I'm in the process of rewriting Clog to complete thread safety and cross-platform support, and add things like compliance with C++ standard.

# Installation
Clone the repository and place the source files inside your project.
```
$ git clone https://github.com/sh7ven/Clog/
```

Add `clog.h` to an include directory, and add it to a build script as well.

For instance:
```make
# File names alongside are Project's dependencies.

Project: include/file1.h src/file1.c ... include/clog.h
    @gcc $(SOME_FLAGS) -I./include/ src/*
```
See [GNU Make](https://www.gnu.org/software/make/).

# Usage
The function definition for logging is:
```c
void clog_log(clog_priority priority, FILE* stream, const char* fmt, ...)
```

For instance
```c
clog_log( CLOG_WARN, stdout, "This is some warning where an invalid access was made to the address %p", 0x0f0f );
```
![sample warning](/screenshots/sample_usage_warning.png)

You can also use some macros to quickly pipe output, instead of specifying a priority level everytime:
```
clog_info(...)
clog_debug(...)
clog_warn(...)
clog_err(...)
```

# Save logs to file
You can instruct Clog to save logs to a backup file as well. 
```c
// Call this before logging any output. 
clog_backup_logs(true);
```

This file is platform specific and is opened as soon you include `clog.h`.
On Linux distributions, it is `$XDG_CONFIG_HOME/clogfile` or `$HOME/.config/clogfile`.
On Windows, it is `..\AppData\clogfile`.

# TODOs
- Better cross platform support for Windows and Mac.
- Better thread safety.
- Pure private functions (hard) to prevent unnecessary problems.

# License
[MIT](https://opensource.org/license/MIT)

