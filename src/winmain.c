/*
 * $Id: winmain.c,v 1.5 2002/08/22 12:27:47 bnv Exp $
 * $Log: winmain.c,v $
 * Revision 1.5  2002/08/22 12:27:47  bnv
 * Deleted: time checking
 *
 * Revision 1.4  2002/07/03 13:15:08  bnv
 * Changed: Version define
 *
 * Revision 1.3  2002/06/06 08:26:05  bnv
 * Corrected: Font handling
 *
 * Revision 1.2  2001/06/25 18:51:48  bnv
 * Header -> Id
 *
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

#if defined(WIN32) || defined(WCE)
	_szRxAppKey = REGAPPKEY;
#endif

	len = sizeof(_FontHeight);
	if (!RXREGGETDATA(TEXT("HT"),REG_DWORD,&_FontHeight,&len))
		_FontHeight = 12;
	WInitWinIO(hInstance,hPrevInstance,nCmdShow);

	/* Set our Icon */
	SendMessage(_CrtWindow,WM_SETICON,FALSE,
		(LPARAM)LoadImage(hInstance,MAKEINTRESOURCE(REXXICON),
				IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));

	if (lpCmdLine[0]==0) {
		WSetTitle("BRexx");
		PUTS("rexx \"<filename>\" <args>...\n"
			VERSIONSTR"\n"
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
