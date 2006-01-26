/*
 * $Id: winmain.c,v 1.7 2006/01/26 10:28:32 bnv Exp $
 * $Log: winmain.c,v $
 * Revision 1.7  2006/01/26 10:28:32  bnv
 * Corrected: To compile on both PocketPC and WindowsCE
 *
 * Revision 1.6  2004/08/16 15:29:30  bnv
 * Changed: Fonts, check for active window
 *
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
#include <commctrl.h>
#include <commdlg.h>
#include <winio.h>
#include <winfunc.h>

extern	HWND	_crtWindow;
extern	void	RxWinInitialize(void);

#define RFILTER	TEXT("BRexx Files (*.r)\0*.r\0All Files (*.*)\0*.*\0")
TCHAR	fileName[256] = TEXT("");
//#define RUNFILE	"\\3dplot.r"

/* ---- WinMain ---- */
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPTSTR lpCmdLine, int nCmdShow )
{
	Lstr	args, file, tmp;
	DWORD	err;

#if defined(WIN32) || defined(WCE)
	_szRxAppKey = REGAPPKEY;
#endif
	if (lpCmdLine[0]==0 && FindWindow(TEXT(PACKAGE_NAME),NULL)) {
		SetForegroundWindow(FindWindow(TEXT(PACKAGE_NAME),NULL));
		return FALSE;
	}

	if (err = WInitWinIO(hInstance,hPrevInstance,nCmdShow))
		return err;

	LINITSTR(args);
	LINITSTR(file);

#ifdef RUNFILE
	Lscpy(&file,RUNFILE);
#else
	if (lpCmdLine[0]==0) {
		/* Pop up a dialog for a file to run */
		OPENFILENAME	ofn;

		memset(&ofn,0,sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hInstance = hInstance;
		ofn.lpstrFile = fileName;
		ofn.nMaxFile = sizeof(fileName)/sizeof(TCHAR);
		ofn.nFilterIndex=1;
		ofn.lpstrInitialDir = TEXT("");
		ofn.lpstrFilter = RFILTER;
		ofn.Flags = OFN_FILEMUSTEXIST;
		ofn.lpstrTitle = TEXT("Run a BRexx script");
		ofn.lpstrDefExt = TEXT("r");
		ofn.hwndOwner = _crtWindow;
		if (!GetOpenFileName(&ofn)) {
			WSetTitle("BRexx");
			WSetColor(0xF1);
			PUTS("rexx \"<filename>\" <args>...\n");
			WSetColor(0xF4);
			PUTS(VERSIONSTR"\n"
				"Author: "AUTHOR"\n"
				"Please report any bugs, fatal errors or comments to the\n"
				"above address.");
			WExitWinIO();
			return 0;
		}
		LWSCPY(&file,fileName);
	} else {
		LINITSTR(tmp);
		LWSCPY(&tmp,lpCmdLine);
		LASCIIZ(tmp);

		if ((LSTR(tmp)[0] == '\"') || (LSTR(tmp)[0] == '\'')) {
			DWORD len;
			const char *ch = STRCHR(LSTR(tmp)+1,LSTR(tmp)[0]);
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
	}
#endif

	Lstrcpy(&tmp,&args);
	Lstrip(&args,&tmp,LBOTH,' '); /* Strip arguments from spaces */
	LFREESTR(tmp);

#ifdef __DEBUG__
	__debug__ = FALSE;
#endif
	LASCIIZ(file);
	WSetTitle(LSTR(file));

	/* --- Initialise --- */
	RxInitialize("brexxce");
#ifdef WCE
	RxWinInitialize();
#endif

	/* --- Run the program --- */
	RxRun(LSTR(file),NULL,&args,NULL,NULL);

	/* --- Free everything --- */
	RxFinalize();
	LFREESTR(args);
	LFREESTR(file);

	WExitWinIO();

	return rxReturnCode;
} /* WinMain */
