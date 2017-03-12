#pragma once
#include <winbase.h>
#include <shellapi.h>

typedef struct args
{
	int argc;
	char **argv;
} args;

void get_comand_line(args *args)
{
	LPWSTR *szArglist;
	int nArgs;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (NULL == szArglist) ExitProcess(1);

	args->argc = nArgs;
	args->argv = szArglist;
	LocalFree(szArglist);
}

/* 
// win32 console entry point
void _cdecl mainCRTStartup()
{
	args args;
	get_comand_line(&args);
	main(&args.argc, &args.argv);
}
*/

