[sample warning](/screenshots/sample_usage_warning.png)

# Clog
A simple, lightweight and thread-safe logging library written in C.

# Installation
Clone the repository and place the source files inside your project.
```
$ git clone https://github.com/shivenkashyapp/clog
```

### Example project structure
```
├── include
├── src
└── README.md
```

Place `clog.c` inside `src/`, and `clog.h` inside `include/`. 
Add these sources to a `Makefile`, if you're using one.

For instance,
```make
# File names alongside are Project's dependencies.

Project: include/file1.h src/file1.c ... src/clog.c include/clog.h
    @gcc $(SOME_FLAGS) -I./include/ src/*
```

See [GNU Make](https://www.gnu.org/software/make/).

# Usage
The function definition for logging is:
```c
void clog_log(clog_priority priority, FILE* stream, const char* fmt, ...)
```

For instance:
```c
clog_log( CLOG_WARN, stdout, "This is some warning where an invalid access was made to the address %p", 0x0f0f );
```
[sample warning](/screenshots/sample_usage_warning.png)

# TODOs
- Cross platform support for Windows and Mac.
- Better (production level) thread safety.
- Managing backup logs.

# License
[MIT](https://opensource.org/license/MIT)
