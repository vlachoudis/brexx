/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/winmain.c,v 1.1 1999/11/26 13:13:47 bnv Exp $
 * $Log: winmain.c,v $
 * Revision 1.1  1999/11/26 13:13:47  bnv
 * Initial revision
 *
 */
#include <string.h>
#include <lstring.h>

#include <rexx.h>
#include <rxdefs.h>

#include <windows.h>
#include <winio.h>
#include <cefunc.h>
#include "resource.h"

extern	HWND	_CrtWindow;
extern	DWORD	_FontHeight;

/* --------------------- WinMain ---------------------- */
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow )
{
	Lstr	args, file, tmp;
	char	*prgname;
	DWORD	len;
	SYSTEMTIME	time;

	len = sizeof(_FontHeight);
	if (!RXREGGETDATA(TEXT("HT"),REG_DWORD,&_FontHeight,&len))
		_FontHeight = 14;

	WInitWinIO(hInstance,hPrevInstance,nCmdShow);

	/* Set our Icon */
	SendMessage(_CrtWindow,WM_SETICON,FALSE,
		(LPARAM)LoadImage(hInstance,MAKEINTRESOURCE(REXXICON),
				IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));

	GetLocalTime(&time);
	if (time.wYear>=2000 && time.wMonth>=3) {
		MessageBox(_CrtWindow,TEXT("This version is old.\n")
			TEXT("Please download the new version from\n")
			TEXT("ftp://ftp.gwdg.de/pub/languages/pub/rexx/brexx\n"),
			TEXT("BRexx"),MB_OK|MB_ICONINFORMATION);
	}

	if (lpCmdLine[0]==0) {
		WSetTitle("BRexx");
		PUTS("rexx \"<filename>\" <args>...\n"
			VERSION"\n"
			"Author: "AUTHOR"\n"
			"Please report any bugs, fatal errors or comments to the\n"
			"above address.");
		WExitWinIO();
		return 0;
	}

	LINITSTR(tmp);
#ifdef __BORLANDC__
	Lscpy(&tmp,lpCmdLine);
#else
	Lwscpy(&tmp,lpCmdLine);
#endif
	LASCIIZ(tmp);

	LINITSTR(args);
	LINITSTR(file);
	if ((LSTR(tmp)[0] == '\"') || (LSTR(tmp)[0] == '\'')) {
		char	*ch = STRCHR(LSTR(tmp)+1,LSTR(tmp)[0]);
		if (ch)
			len = (DWORD)ch - (DWORD)LSTR(tmp);
		else
			len = LLEN(tmp);
		Lsubstr(&file, &tmp, 2, len-1, ' ');
		Lsubstr(&args, &tmp, len+3, LREST, ' ');
	} else {
		Lword(&file, &tmp, 1);
 		Lsubword(&args, &tmp, 2, LREST);
	}
	Lstrcpy(&tmp,&args);
	Lstrip(&args,&tmp,LBOTH,' '); /* Strip arguments from spaces */
	LASCIIZ(file);
	LFREESTR(tmp);


#ifdef __DEBUG__
	__debug__ = FALSE;
#endif
	LASCIIZ(file);
	WSetTitle(LSTR(file));

	/* --- Initialise --- */
	RxInitialize("brexxce");

	/* --- Run the program --- */
	RxRun(LSTR(file),NULL,&args,NULL,NULL);

	/* --- Free everything --- */
	RxFinalize();
	free(prgname);
	LFREESTR(args);
	LFREESTR(file);

	WExitWinIO();

	return RxReturnCode;
} /* WinMain */
