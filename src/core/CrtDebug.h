#pragma once

#include "Platform.h"

#if defined(_DEBUG) && defined(SYSTEM_WIN)
	#define _CRTDBG_MAP_ALLOC
	#include<crtdbg.h>

	#define New new(_CLIENT_BLOCK, __FILE__, __LINE__)
	#define Delete delete
	#define Malloc(SIZE) _malloc_dbg(SIZE, _CLIENT_BLOCK, __FILE__, __LINE__)
	#define Free(P) _free_dbg(P, _CLIENT_BLOCK)

#else
	#define New new
	#define Delete delete
	#define Malloc(SIZE) ::malloc(SIZE)
	#define Free(P) ::free(P)
#endif

void openCheckMem();
