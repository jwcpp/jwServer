#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <windows.h>
#include <assert.h>

#if _MSC_VER < 1300
#define DECLSPEC_DEPRECATED
// VC6: change this path to your Platform SDK headers
#include "M:\\dev7\\vs\\devtools\\common\\win32sdk\\include\\dbghelp.h"			// must be XP version of file
#else
// VC7: ships with updated headers
#include "dbghelp.h"
#endif

#include <iostream>
#include <tchar.h>

// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
									CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
									CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
									CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam
									);

static LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo );

#include "dumper.h"
#include <time.h>
#include <mutex>

static std::mutex dumpMutex;
static bool isDumping = false;

DumperHandler Dumper::dumperHandler;

Dumper::Dumper()
{
	dumperHandler = NULL;
	::SetUnhandledExceptionFilter( TopLevelFilter );
}

LONG WINAPI TopLevelFilter( struct _EXCEPTION_POINTERS *pExceptionInfo )
{
	{
		std::lock_guard<std::mutex> locker(dumpMutex);
		if (isDumping) {
			::MessageBox( NULL, "Already crushing...", "MiniDump", MB_OK );
			return EXCEPTION_EXECUTE_HANDLER;
		}
		isDumping = true;
	}


//	EXCEPTION_EXECUTE_HANDLER = 1
//	EXCEPTION_CONTINUE_EXECUTION = -1
//	这两个返回值都应该由调用UnhandledExceptionFilter后返回。
//	EXCEPTION_EXECUTE_HANDLER表示进程结束
//	EXCEPTION_CONTINUE_EXECUTION表示处理异常之后继续执行
//	EXCEPTION_CONTINUE_SEARCH = 0 进行系统通常的异常处理（错误消息对话框）

	LONG retval = EXCEPTION_CONTINUE_SEARCH;
	HWND hParent = NULL;						// find a better value for your app

	// firstly see if dbghelp.dll is around and has the function we need
	// look next to the EXE first, as the one in System32 might be old 
	// (e.g. Windows 2000)
	HMODULE hDll = NULL;
	char szDbgHelpPath[_MAX_PATH];

	if (GetModuleFileName( NULL, szDbgHelpPath, _MAX_PATH ))
	{
		char *pSlash = _tcsrchr( szDbgHelpPath, '\\' );
		if (pSlash)
		{
			_tcscpy( pSlash+1, "DBGHELP.DLL" );
			hDll = ::LoadLibrary( szDbgHelpPath );
		}
	}

	if (hDll==NULL)
	{
		// load any version we can
		hDll = ::LoadLibrary( "DBGHELP.DLL" );
	}

	LPCTSTR szResult = NULL;

	if (hDll)
	{
		MINIDUMPWRITEDUMP pDump = (MINIDUMPWRITEDUMP)::GetProcAddress( hDll, "MiniDumpWriteDump" );
		if (pDump)
		{
			char szDumpPath[_MAX_PATH];
			char szScratch [_MAX_PATH];

			// work out a good place for the dump file
			//if (!GetTempPath( _MAX_PATH, szDumpPath ))
			if (!GetModuleFileName( NULL, szDumpPath, _MAX_PATH ))
				_tcscpy( szDumpPath, "c:\\temp\\" );

			// get the time now
			char szTime[_MAX_PATH];
			time_t timeNow = time(NULL);
			strftime(szTime, _MAX_PATH, ".%Y%m%d-%H%M%S", localtime(&timeNow));

			_tcscat( szDumpPath, szTime );
			_tcscat( szDumpPath, ".dmp" );

			// ask the user if they want to save a dump file
	//		if (::MessageBox( NULL, "Something bad happened in your program, would you like to save a diagnostic file?", m_szAppName, MB_YESNO )==IDYES)
			{
				// create the file
				HANDLE hFile = ::CreateFile( szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS,
											FILE_ATTRIBUTE_NORMAL, NULL );

				if (hFile!=INVALID_HANDLE_VALUE)
				{
					_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

					ExInfo.ThreadId = ::GetCurrentThreadId();
					ExInfo.ExceptionPointers = pExceptionInfo;
					ExInfo.ClientPointers = NULL;

					// write the dump
#ifndef NDEBUG // Debug mode
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &ExInfo, NULL, NULL );
#else
					BOOL bOK = pDump( GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, &ExInfo, NULL, NULL );//MiniDumpNormal, &ExInfo, NULL, NULL );
#endif // NDEBUG
					if (bOK)
					{
						sprintf( szScratch, "Saved dump file to '%s'", szDumpPath );
						szResult = szScratch;
						retval = EXCEPTION_EXECUTE_HANDLER;
					}
					else
					{
						sprintf( szScratch, "Failed to save dump file to '%s' (error %d)", szDumpPath, GetLastError() );
						szResult = szScratch;
					}
					::CloseHandle(hFile);
				}
				else
				{
					sprintf( szScratch, "Failed to create dump file '%s' (error %d)", szDumpPath, GetLastError() );
					szResult = szScratch;
				}
			}
		}
		else
		{
			szResult = "DBGHELP.DLL too old";
		}
	}
	else
	{
		szResult = "DBGHELP.DLL not found";
	}

	// 调用上层处理器
	DumperHandler dumperHandler = Dumper::getDumperHandler();
	if (dumperHandler) {
		(*dumperHandler)();
	}


	if (szResult)
		::MessageBox( NULL, szResult, "MiniDump", MB_OK );

	return retval;
}


