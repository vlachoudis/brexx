/*
 * $Id: winfunc.c,v 1.4 2002/06/11 12:38:06 bnv Exp $
 * $Log: winfunc.c,v $
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

extern	HWND		_CrtWindow;
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
CE_MsgBox()
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
	msgText = (TCHAR *)MALLOC(sizeof(TCHAR)*LLEN(*ARG1)+2,NULL);
	msgTitle = (TCHAR *)MALLOC(sizeof(TCHAR*)LLEN(*ARG1)+2,NULL);
	mbstowcs(msgText,LSTR(*ARG1),LLEN(*ARG1));	msgText[LLEN(*ARG1)] = 0;
	mbstowcs(msgTitle,LSTR(*ARG2),LLEN(*ARG2));	msgTitle[LLEN(*ARG2)] = 0;

	Licpy(ARGR, MessageBox(_CrtWindow, msgText, msgTitle, opt));
	InvalidateRect(_CrtWindow,NULL,TRUE);
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
		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
} /* CE_O */

/* -------------------------------------------------------------- */
/*  GOTOXY(x,y)                                                   */
/* -------------------------------------------------------------- */
void __CDECL
CE_gotoxy()
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
CE_oS()
{
	TCHAR	path[MAX_PATH];

	if (ARGN>1)
		Lerror(ERR_INCORRECT_CALL,0);

	GetWindowText(_CrtWindow, path, sizeof(path)/sizeof(TCHAR));
	Lwscpy(ARGR, path);
	if (ARGN==1) {
		L2STR(ARG1);
		LASCIIZ(*ARG1);
		WSetTitle(LSTR(*ARG1));
	}
} /* CE_oS */

/* -------------------------------------------------------------- */
/*  MKDIR(directory)                                              */
/* -------------------------------------------------------------- */
/*  RMDIR(directory)                                              */
/* -------------------------------------------------------------- */
/*  DELFILE(file)                                                  */
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
	ofn.hwndOwner = _CrtWindow;
	ofn.hInstance = _CrtInstance;


} * CE_FileDialog *
***/

/* -------------------------------------------------------------- */
/*  CLIPBOARD([type|cmd [,data]])                                 */
/*  cmd = 'List' | 'Clear'                                        */
/* -------------------------------------------------------------- */
void __CDECL
CE_Clipboard()
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

	OpenClipboard(_CrtWindow);
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
CE_Dir()
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
