#ifndef PLATFORM_H
#define PLATFORM_H


#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define SYSTEM_WIN // win
#else
#define SYSTEM_LINUX // linux
#endif


#endif


