/*
 * $Header: /home/bnv/tmp/brexx/win/RCS/wintest.c,v 1.1 1999/11/26 08:48:34 bnv Exp $
 * $Log: wintest.c,v $
 * Revision 1.1  1999/11/26 08:48:34  bnv
 * Initial revision
 *
 */

#include <tchar.h>
#include <string.h>
#include <windows.h>
#include <winio.h>


/* --------------------- WinMain ---------------------- */
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPWSTR lpCmdLine, int nCmdShow )
{
	int	ch,i;

	_InitWinIO(hInstance,hPrevInstance,nCmdShow);
	for (i=50; i>0; i--)
		winprintf(TEXT("\nLine %d"),i);
	ch = _ReadKey();

	_ExitWinIO();

	return 0;
} /* WinMain */
