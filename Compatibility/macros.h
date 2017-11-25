//
// Created by stefano on 04/11/17.
//

#ifndef NB_MACROS_H
#define NB_MACROS_H

#if defined(__UNIX__) || defined(__unix__)

#include <unistd.h>
#include <sys/stat.h>
#define _SLEEP(ms) usleep(ms*1000)

#elif defined(_WIN32) || defined(WIN32)

#include <windows.h>
#define _SLEEP(ms) Sleep(ms)
#define open(name, ...) _open(name, __VA_ARGS__)
#define read(fd, buf, count) _read(fd, buf, count)
#define close(fd) _close(fd)
#define write(fd, buf, count) _write(fd, buf, count)
#define dup2(fd1, fd2) _dup2(fd1, fd2)
#define unlink(file) _unlink(file)
#define rmdir(dir) _rmdir(dir)
#define getpid() _getpid()
#define stat() _stat()

#endif

#endif //NB_MACROS_H
