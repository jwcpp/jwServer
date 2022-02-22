#include "CrtDebug.h"

#if defined(_DEBUG) && defined(SYSTEM_WIN)

	#include <stdio.h>
	void openCheckMem()
	{
		int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | flag);

		freopen("./memory.txt", "w+", stderr);
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
		_CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
	}

#else
	void openCheckMem() {
	}

#endif


