/*
 * $Id: winfunc.c,v 1.6 2005/05/20 16:02:21 bnv Exp $
 * $Log: winfunc.c,v $
 * Revision 1.6  2005/05/20 16:02:21  bnv
 * Added: SETBREAK & SETPALETTE
 *
 * Revision 1.5  2004/08/16 15:34:53  bnv
 * Added: Color and Scrollbar functions
 *
 * Revision 1.4  2002/06/11 12:38:06  bnv
 * Added: CDECL
 *
 * Revision 1.3  2001/06/25 18:52:24  bnv
 * Header -> Id
 *
 * Revision 1.2  1999/11/26 13:22:36  bnv
 * Added: The routine CE_GetRegData
 *
 * Revision 1.1  1999/09/13 15:06:41  bnv
 * Initial revision
 *
 */

#include <rexx.h>
#include <rxdefs.h>
#include <lstring.h>
#include <compile.h>
#include <winio.h>

#include <cefunc.h>

enum	ce_msgs {
		f_bgcolor,
		f_clreol,
		f_clrscr,
		f_copyfile,
		f_createdirectory,
		f_deletefile,
		f_fgcolor,
		f_getch,
		f_kbhit,
		f_movefile,
		f_removedirectory,
		f_setbreak,
		f_setcolor,
		f_setfontsize,
		f_setpalette,
		f_scrollbars,
		f_wherex,
		f_wherey,
		f_windowtitle,
		f_winexit
};

extern	HWND		_crtWindow;
extern	HINSTANCE	_CrtInstance;

/* --------------------------------------------------------------- */
BOOL __CDECL
CE_GetRegData(HKEY key, TCHAR *keyPath, TCHAR *varName,
		DWORD type, LPBYTE pvData, LPDWORD cbData )
{
	HKEY	hKey;
	BOOL	fSuccess;

	if ( RegOpenKeyEx(	key,
				keyPath,
				0,
				KEY_ALL_ACCESS,
				&hKey ) != ERROR_SUCCESS )
		return FALSE;
	fSuccess = RegQueryValueEx( hKey,
			varName,
			0,
			&type,
			pvData,
			cbData );
	RegCloseKey( hKey );
	return (fSuccess==ERROR_SUCCESS);
} /* CE_GetRegData */

/* --------------------------------------------------------------- */
/*  MSGBOX(text, title, [option])                                  */
/* --------------------------------------------------------------- */
void __CDECL
CE_MsgBox(const int func)
{
	TCHAR	*msgText, *msgTitle;
	long	opt;

	if (!IN_RANGE(2,ARGN,3))
		Lerror(ERR_INCORRECT_CALL,0);
	must_exist(1);
	must_exist(2);
	get_oi0(3,opt);

	LASCIIZ(*ARG1);
	LASCIIZ(*ARG2);
	msgText  = (TCHAR *)MALLOC(sizeof(TCHAR)*LLEN(*ARG1)+2,NULL);
	msgTitle = (TCHAR *)MALLOC(sizeof(TCHAR*)LLEN(*ARG1)+2,NULL);
	mbstowcs(msgText,LSTR(*ARG1),LLEN(*ARG1));	msgText[LLEN(*ARG1)] = 0;
	mbstowcs(msgTitle,LSTR(*ARG2),LLEN(*ARG2));	msgTitle[LLEN(*ARG2)] = 0;

	Licpy(ARGR, MessageBox(_crtWindow, msgText, msgTitle, opt));
	InvalidateRect(_crtWindow,NULL,TRUE);
	FREE(msgText);
	FREE(msgTitle);
	return;
} /* CE_MsgBox */

/* -------------------------------------------------------------- */
/*  CLRSCR()                                                      */
/* -------------------------------------------------------------- */
/*  CLREOL()                                                      */
/* -------------------------------------------------------------- */
/*  GETCH()                                                       */
/* -------------------------------------------------------------- */
/*  KBHIT()                                                       */
/* -------------------------------------------------------------- */
/*  WHEREX()                                                      */
/* -------------------------------------------------------------- */
/*  WHEREY()                                                      */
/* -------------------------------------------------------------- */
/*  FGCOLOR()                                                     */
/* -------------------------------------------------------------- */
/*  BGCOLOR()                                                     */
/* -------------------------------------------------------------- */
/*  WINEXIT()                                                     */
/* -------------------------------------------------------------- */
void __CDECL
CE_O(const int func)
{
	if (ARGN) Lerror(ERR_INCORRECT_CALL,0);
	LZEROSTR(*ARGR);
	switch (func) {
		case f_clrscr:
			WClrscr();
			break;
		case f_clreol:
			WClreol();
			break;
		case f_wherex:
			Licpy(ARGR,WWhereX());
			break;
		case f_wherey:
			Licpy(ARGR,WWhereY());
			break;
		case f_getch:
			Lfx(ARGR,1);
			LTYPE(*ARGR) = LSTRING_TY;
			LLEN(*ARGR) = 1;
			LSTR(*ARGR)[0] = WReadKey();
			break;
		case f_kbhit:
			Licpy(ARGR,WKeyPressed());
			break;
		case f_winexit:
			WMessage(WM_DESTROY,0,0);
			break;
		case f_fgcolor:
			Licpy(ARGR,WGetColor()&0xf);
			break;
		case f_bgcolor:
			Licpy(ARGR,(WGetColor()>>4)&0xf);
			break;
		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
} /* CE_O */

/* -------------------------------------------------------------- */
/*  GOTOXY(x,y)                                                   */
/* -------------------------------------------------------------- */
void __CDECL
CE_gotoxy(const int func)
{
	int	x, y;
	if (ARGN!=2)
		Lerror(ERR_INCORRECT_CALL,0);
	get_i(1,x);
	get_i(2,y);
	WGotoXY(x,y);
	LZEROSTR(*ARGR);
} /* CE_gotoxy */

/* -------------------------------------------------------------- */
/*  SETCOLOR(fg[,bg])                                             */
/* -------------------------------------------------------------- */
void __CDECL
CE_setcolor(const int func)
{
	int	fg, bg;

	if (ARGN>2)
		Lerror(ERR_INCORRECT_CALL,0);

	get_i0(1,fg);

	if (ARGN==2) {
		get_i0(2,bg);
	} else
		bg = ((WGetColor()>>4)&0x0f);

	Licpy(ARGR,WGetColor());
	WSetColor((BYTE)(((bg&0x0f)<<4)|(fg&0x0f)));
} /* CE_setcolor */

/* -------------------------------------------------------------- */
/*  SETPALETTE(col[,r,g,b])                                       */
/* -------------------------------------------------------------- */
void __CDECL
CE_setpalette(const int func)
{
	int	col, r, g, b;

	if (ARGN!=1 && ARGN!=4)
		Lerror(ERR_INCORRECT_CALL,0);

	get_i(1,col);
	if (col>16) Lerror(ERR_INCORRECT_CALL,0);
	col--;

	Licpy(ARGR,WGetPalette(col));

	if (ARGN==4) {
		get_i0(2,r);
		get_i0(3,g);
		get_i0(4,b);
		if (r>255 || g>255 || b>255)
			Lerror(ERR_INCORRECT_CALL,0);
		WSetPalette(col,r,g,b);
	}
} /* CE_setpalette */

/* -------------------------------------------------------------- */
/*  COPYFILE(src,dst)                                             */
/* -------------------------------------------------------------- */
/*  MOVEFILE(src,dst)                                             */
/* -------------------------------------------------------------- */
void __CDECL
CE_SS(const int func)
{
	TCHAR	src[MAX_PATH];
	TCHAR	dst[MAX_PATH];

	if (ARGN!=2)
		Lerror(ERR_INCORRECT_CALL,0);

	get_s(1);
	get_s(2);
	mbstowcs(src,LSTR(*ARG1),LLEN(*ARG1));	src[LLEN(*ARG1)] = 0;
	mbstowcs(dst,LSTR(*ARG2),LLEN(*ARG2));	dst[LLEN(*ARG2)] = 0;

	switch (func) {
		case f_copyfile:
			Licpy(ARGR,CopyFile(src,dst,TRUE)?1:0);
			break;
		case f_movefile:
			Licpy(ARGR,MoveFile(src,dst)?1:0);
			break;
		default:
			break;
	}
} /* CE_SS */

/* -------------------------------------------------------------- */
/*  WINDOWTITLE(title)                                            */
/* -------------------------------------------------------------- */
void __CDECL
CE_oS(const int func)
{
	TCHAR	path[MAX_PATH];

	if (ARGN>1)
		Lerror(ERR_INCORRECT_CALL,0);

	GetWindowText(_crtWindow, path, sizeof(path)/sizeof(TCHAR));
	Lwscpy(ARGR, path);
	if (ARGN==1) {
		L2STR(ARG1);
		LASCIIZ(*ARG1);
		WSetTitle(LSTR(*ARG1));
	}
} /* CE_oS */

/* -------------------------------------------------------------- */
/*  SETFONTSIZE(n)                                                */
/* -------------------------------------------------------------- */
void __CDECL
CE_I(const int func)
{
	int	size;

	if (ARGN!=1)
		Lerror(ERR_INCORRECT_CALL,0);

	get_i(1,size);

	if (size<6)
		size=6;
	else
	if (size>50)
		size=50;

	Licpy(ARGR,WGetFontSize());
	WSetFontSize(size);
} /* CE_I */

/* -------------------------------------------------------------- */
/*  SCROLLBARS([0|1])                                             */
/* -------------------------------------------------------------- */
/*  SETBREAK([0|1])                                               */
/* -------------------------------------------------------------- */
void __CDECL
CE_B(const int func)
{
	int	b;

	if (ARGN>1)
		Lerror(ERR_INCORRECT_CALL,0);

	get_oiv(1,b,-1);
	if (b<-1 || b>1)
		Lerror(ERR_INCORRECT_CALL,0);

	switch (func) {
		case f_scrollbars:
			Licpy(ARGR,WGetScrollBars());
			if (b>=0)
				WSetScrollBars(b);
			break;
		case f_setbreak:
			Licpy(ARGR,WSetBreak(b));
			break;
	}
} /* CE_scrollbars */

/* -------------------------------------------------------------- */
/*  MKDIR(directory)                                              */
/* -------------------------------------------------------------- */
/*  RMDIR(directory)                                              */
/* -------------------------------------------------------------- */
/*  DELFILE(file)                                                 */
/* -------------------------------------------------------------- */
void __CDECL
CE_S(const int func)
{
	TCHAR	path[MAX_PATH];

	if (ARGN!=1)
		Lerror(ERR_INCORRECT_CALL,0);

	get_s(1);
	mbstowcs(path,LSTR(*ARG1),LLEN(*ARG1));
	path[LLEN(*ARG1)] = 0;

	switch (func) {
		case f_createdirectory:
			Licpy(ARGR, CreateDirectory(path,NULL)? 1:0);
			break;
		case f_removedirectory:
			Licpy(ARGR, RemoveDirectory(path)? 1:0);
			break;
		case f_deletefile:
			Licpy(ARGR, DeleteFile(path)? 1:0);
			break;
		default:
			break;
	}
} /* CE_S */

/* -------------------------------------------------------------- */
/*  FILEDIALOG([LOAD|SAVE,TITLE,FILTER,INITIALPATH)               */
/* -------------------------------------------------------------- */
/****
CE_FileDialog()
{
	char	cmd;
	OPENFILENAME	ofn;

	if (ARGN!=4)
		Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	cmd = l2u[(byte)LSTR(*ARG1)[0]];

	get_s(2);
	get_s(3);
	get_s(4);

	memset(ofn,0,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = _crtWindow;
	ofn.hInstance = _CrtInstance;


} * CE_FileDialog *
***/

/* -------------------------------------------------------------- */
/*  CLIPBOARD([type|cmd [,data]])                                 */
/*  cmd = 'List' | 'Clear'                                        */
/* -------------------------------------------------------------- */
void __CDECL
CE_Clipboard(const int func)
{
	char	cmd;

	if (ARGN>2)
		Lerror(ERR_INCORRECT_CALL,0);

	if (exist(1)) {
		if ((LTYPE(*ARG1)!=LINTEGER_TY) && (_Lisnum(ARG1)!=LINTEGER_TY))
			cmd = l2u[(byte)LSTR(*ARG1)[0]];
		else
			cmd = 'G';
	} else
		cmd = 'L';

	OpenClipboard(_crtWindow);
	LZEROSTR(*ARGR);
	switch (cmd) {
		case 'C':
			EmptyClipboard();

			break;
		case 'L':
			{
				UINT	uFormat = 0;
				Lstr	tmp;
				LINITSTR(tmp); Lfx(&tmp,1);
				while (1) {
					uFormat = EnumClipboardFormats(uFormat);
					if (uFormat==0) break;
					Licpy(&tmp,uFormat);
					Lstrcat(ARGR,&tmp);
					Lcat(ARGR," ");
				}
				LFREESTR(tmp);
			}
			break;
		case 'G':
			{
				HLOCAL hClp = GetClipboardData(Lrdint(ARG1));
				size_t	siz = LocalSize(hClp);
				Lfx(ARGR,siz);
				MEMCPY(LSTR(*ARGR),hClp,siz);
				LLEN(*ARGR) = siz;
				if (exist(2)) {
					EmptyClipboard();
					hClp = LocalAlloc(LMEM_DISCARDABLE,LLEN(*ARG2));
					MEMCPY(hClp,LSTR(*ARG2),LLEN(*ARG2));
					SetClipboardData(Lrdint(ARG1),hClp);
				}
			}
			break;
		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
	CloseClipboard();
} /* CE_Clipboard */

/* -------------------------------------------------------------- */
/*  DIR(path)                                                     */
/*  Return a string containing the complet directory, entries     */
/*  are separated with "\n"                                       */
/* -------------------------------------------------------------- */
void __CDECL
CE_Dir(const int func)
{
	HANDLE		findHandle;
	WIN32_FIND_DATA	findData;
	TCHAR		path[MAX_PATH+50];
	Lstr		tmp;

	if (ARGN!=1)
		Lerror(ERR_INCORRECT_CALL,0);

	get_s(1);

	LASCIIZ(*ARG1);
	mbstowcs(path,LSTR(*ARG1),LLEN(*ARG1)+1);

	LZEROSTR(*ARGR);

	findHandle = FindFirstFile(path,&findData);
	if (findHandle == INVALID_HANDLE_VALUE)
		return;

	LINITSTR(tmp);

	do {
		TCHAR	attr[10];
		TCHAR	*ch;
		SYSTEMTIME	fileTime;
		/* prepare the attributes */
		ch = attr;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			*ch++ = TEXT('D');
		else
			*ch++ = TEXT('-');
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
			*ch++ = TEXT('A');
		else
			*ch++ = TEXT('-');
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
			*ch++ = TEXT('H');
		else
			*ch++ = TEXT('-');
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			*ch++ = TEXT('R');
		else
			*ch++ = TEXT('-');
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
			*ch++ = TEXT('S');
		else
			*ch++ = TEXT('-');
#if defined(WCE)
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_INROM)
			*ch++ = TEXT('O');
		else
			*ch++ = TEXT('-');
		*ch = (TCHAR)0;
#endif

		FileTimeToSystemTime(&(findData.ftLastWriteTime), &fileTime);

		swprintf(path,TEXT("%s %8d %02d-%02d-%4d %02d:%02d:%02d %s\n"),
				attr,
				findData.nFileSizeLow,
				fileTime.wDay,
				fileTime.wMonth,
				fileTime.wYear,
				fileTime.wHour,
				fileTime.wMinute,
				fileTime.wSecond,
				findData.cFileName);
		Lwscpy(&tmp,path);
		Lstrcat(ARGR,&tmp);
	} while (FindNextFile(findHandle,&findData));

	LFREESTR(tmp);

	FindClose(findHandle);
} /* CE_Dir */

/* --- RxCEInitialize --- */
void RxCEInitialize()
{
	RxRegFunction( "BGCOLOR",	CE_O		,f_bgcolor	);
	RxRegFunction( "CLIPBOARD",	CE_Clipboard	,0		);
	RxRegFunction( "CLREOL",	CE_O		,f_clreol	);
	RxRegFunction( "CLRSCR",	CE_O		,f_clrscr	);
	RxRegFunction( "COPYFILE",	CE_SS		,f_copyfile	);
	RxRegFunction( "DELFILE",	CE_S		,f_deletefile	);
	RxRegFunction( "DIR",		CE_Dir		,0		);
	RxRegFunction( "FGCOLOR",	CE_O		,f_fgcolor	);
	RxRegFunction( "GETCH",		CE_O		,f_getch	);
	RxRegFunction( "GOTOXY",	CE_gotoxy	,0		);
	RxRegFunction( "KBHIT",		CE_O		,f_kbhit	);
	RxRegFunction( "MKDIR",		CE_S		,f_createdirectory);
	RxRegFunction( "MOVEFILE",	CE_SS		,f_movefile	);
	RxRegFunction( "MSGBOX",	CE_MsgBox	,0		);
	RxRegFunction( "RMDIR",		CE_S		,f_removedirectory);
	RxRegFunction( "SCROLLBARS",	CE_B		,f_scrollbars	);
	RxRegFunction( "SETBREAK",	CE_B		,f_setbreak	);
	RxRegFunction( "SETCOLOR",	CE_setcolor	,f_setcolor	);
	RxRegFunction( "SETFONTSIZE",	CE_I		,f_setfontsize	);
	RxRegFunction( "SETPALETTE",	CE_setpalette	,f_setpalette	);
	RxRegFunction( "WHEREX",	CE_O		,f_wherex	);
	RxRegFunction( "WHEREY",	CE_O		,f_wherey	);
	RxRegFunction( "WINDOWEXIT",	CE_O		,f_winexit	);
	RxRegFunction( "WINDOWTITLE",	CE_oS		,f_windowtitle	);
} /* RxCEInitialize */
