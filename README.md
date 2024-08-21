![sample warning](/screenshots/sample_usage_warning.png)

# Clog
A header-only, simple and thread-safe logging library written in C.

# Installation
Clone the repository and place the source files inside your project.
```
$ git clone https://github.com/thesvn/Clog
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

# TODOs
- Cross platform support for Windows and Mac.
- Better thread safety.
- Managing backup logs to some local file.

# License
[MIT](https://opensource.org/license/MIT)
