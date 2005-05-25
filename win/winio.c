/*
 * $Id: winio.c,v 1.10 2005/05/25 16:01:54 bnv Exp $
 * $Log: winio.c,v $
 * Revision 1.10  2005/05/25 16:01:54  bnv
 * Corrected for Windows CE HPC
 * Resize of window, Menu On/Off
 * Line per line selection (not square any more)
 *
 * Revision 1.9  2005/05/20 16:02:21  bnv
 * Corrected: Support for CE V2.0
 * Changed: Coping follows the lines on screen not a square
 *
 * Revision 1.8  2004/08/16 15:34:53  bnv
 * Corrected: Scrolling behavior
 * Corrected: Menus
 * Added: Colors and scrollbars controls
 *
 * Revision 1.8  2003/10/30 13:17:47  bnv
 * Cosmetics
 *
 * Revision 1.7  2002/08/22 12:29:46  bnv
 * CR removed!
 *
 * Revision 1.6  2002/08/19 15:38:49  bnv
 * Corrected: Cursor behavior
 *
 * Revision 1.5  2002/06/11 12:38:06  bnv
 * Added: CDECL
 *
 * Revision 1.4  2002/01/14 09:14:46  bnv
 * Corrected: To handle correctly the requested fonts
 *
 * Revision 1.3  2001/06/25 18:52:24  bnv
 * Header -> Id
 *
 * Revision 1.2  1999/11/26 13:22:36  bnv
 * Changed: The marking mechanism to avoid flickering.
 *
 * Revision 1.1  1999/09/13 15:06:41  bnv
 * Initial revision
 *
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
#include "resource.h"
#ifdef WCE
#	include <afxres.h>
#if _WIN32_WCE > 211
#		include <aygshell.h>
#else
#		include <commctrl.h>
#endif
#	define MENU_HEIGHT	26
#endif

#include <os.h>
#include <ldefs.h>
#include <bstr.h>
#include <rexx.h>
#include <cefunc.h>
#ifdef __BORLANDC__
#	define	TEXT(x)	x
#	define	TCHAR	char
#	define	LPTSTR	LPSTR
#else
#	include <tchar.h>
#endif
#include <winio.h>

#define SHGetSubMenu(hWndMB,ID_MENU) (HMENU)SendMessage((hWndMB), \
			SHCMBM_GETSUBMENU, (WPARAM)0, (LPARAM)ID_MENU);


LRESULT CALLBACK _WinIOProc(HWND Window, UINT Message,
				WPARAM WParam, LONG LParam);

POINT	_screenSize = { 80, 100 };	// Screen buffer dimensions
POINT	_origin = { 0, 0 };		// Client Area Origin
POINT	_cursor = { 0, 0 };		// Cursor location
BOOL	_checkBreak = TRUE;		// Allow Ctrl-C for break?
BOOL	_winTerminated = FALSE;		// Escape exists program
HFONT	_hFont;				// Current working font
HWND	_mainWindow = 0;		// Main Window
HWND	_crtWindow = 0;			// CRT window handle
HINSTANCE	_crtInstance;		// CRT class instance

static HWND	hwndCB;			// Menu Command Bar Window
static BOOL	markTool=TRUE;		// Move or Mark
static BOOL	showScrollBars=TRUE;	// Show Scrollbars
static BOOL	showMenu=TRUE;		// Show Menu
static BOOL	marking=FALSE;		// Marking enabled
static POINT	markBegin={0,0};	// Beggining of marking
static POINT	markEnd={0,0};		// End of marking area
static POINT	oldOrigin;		// Old origin begore moving
static POINT	clientSize;		// Client Area
static POINT	range;			// Scroll bar ranges
static int	firstLine  = 0;		// First line in circular buffer
static int	keyCount   = 0;		// Count of keys in keyBuffer
static BOOL	focused = FALSE;	// Window focused?
static BOOL	painting = FALSE;	// Handling wm_Paint?

static LPTSTR	screenBuffer;		// Screen buffer pointer
static POINT	charSize={8,15};	// Character cell size
static int	charAscent;		// Character ascent
static int      marginTop=0;		// Vertical Margin
static HDC	DC;			// Global device context
static PAINTSTRUCT	PS;		// Global paint structure
static BOOL	windowPainted = FALSE;	// Sometimes when scrolling,
					// it doesn't invalidate correctly the regions
static HFONT	saveFont;		// Saved device context font
static int	fontSize;		// Font size
static char	keyBuffer[32];		// Keyboard type-ahead buffer
static HMENU	hPopupMenu;		// Popup menu

static int	clpKeyCount=0;		// Keyboard buffer from Clipboard
static LPTSTR	clpKeyBuffer;

#define	SIGINT		0
#define	SIG_IGN		NULL
static void	(*SignalBreak)(int sig) = NULL;
static BOOL	breakActive = FALSE;
static TCHAR	moduleName[80];

static BYTE	*colorBuffer;		// Color buffer pointer
static BYTE	activeColor=0xF0;	// Active Color

static COLORREF colorRef[16] = {
			RGB(  0,  0,  0),	// Black/0
			RGB(191,  0,  0),	// Red/1
			RGB(  0,191,  0),	// Green/2
			RGB(255,128,  0),	// Brown(AKA Yellow)/3
			RGB(  0,  0,191),	// Blue/4
			RGB(191,  0,191),	// Magenta/5
			RGB(  0,191,191),	// Cyan/6
			RGB(192,192,192),	// White/7
			RGB(128,128,128),	// B.Black/8
			RGB(255,  0,  0),	// B.Red/9
			RGB(  0,255,  0),	// B.Green/10
			RGB(255,255,  0),	//   Yellow/11
			RGB(  0,  0,255),	// B.Blue/12
			RGB(255,  0,255),	// B.Magenta/13
			RGB(  0,255,255),	// B.Cyan/14
			RGB(255,255,255) };	// B.White/15

/* --- Local Function Prototypes --- */
static void WindowResize(void);

/* ---- WSignal ---- */
/* Substitue of the signal(), for trapping the Control-C */
void __CDECL
WSignal(int sig, void (*func)(int sig))
{
	switch (sig) {
		case SIGINT:
			breakActive = FALSE;
			SignalBreak = func;
			_checkBreak = (func!=NULL);
			break;
		default:
			break;
	}
} /* WSignal */

/* ---- WSetTitle ---- */
void __CDECL
WSetTitle(const char *title)
{
	size_t	len;
	TCHAR	str[100];
	len = STRLEN(title);
	mbstowcs(str,title,len+1);
	if (_crtWindow)
		SetWindowText(_crtWindow,str);
} /* WSetTitle */

/* ---- some Helper routines ---- */
static void
unicodeMemSet(LPTSTR buf, TCHAR ch, int len)
{
	int	i;
	for (i=0; i<len; i++)
		buf[i] = ch;
} /* unicodeMemSet */

/* ---- Allocate device context ---- */
static void
InitDeviceContext(void)
{
	if (painting)
		DC = BeginPaint(_crtWindow, &PS);
	else
		DC = GetDC(_crtWindow);

	saveFont = (HFONT)SelectObject (DC, _hFont);
} /* InitDeviceContext */

/* ---- Release device context ---- */
static void
DoneDeviceContext(void)
{
	SelectObject(DC, saveFont);
	if (painting)
		EndPaint(_crtWindow, &PS);
	else
		ReleaseDC(_crtWindow, DC);
} /* DoneDeviceContext */

/* ---- Show caret ---- */
static void
_ShowCursor(void)
{
	if (focused) {
		SetCaretPos(	(_cursor.x - _origin.x) * charSize.x,
				(_cursor.y - _origin.y) * charSize.y
				  + charAscent + marginTop);
		ShowCaret(_crtWindow);
	}
} /* _ShowCursor */

/* ---- Hide caret ---- */
static void
_HideCursor(void)
{
	if (focused)
		HideCaret(_crtWindow);
} /* _HideCursor */

/* ---- Create caret ---- */
static void
_CreateCursor(void)
{
	CreateCaret(_crtWindow, 0, charSize.x? charSize.x : 8, 2);
	_ShowCursor();
} /* _CreateCursor */

/* ---- SetScrollBars  ---- */
static void
SetScrollBars()
{
	if (!showScrollBars)
		return;

	if (clientSize.x < _screenSize.x) {
		SetScrollRange(_crtWindow, SB_HORZ, 0, max(1,range.x), FALSE);
		SetScrollPos(_crtWindow, SB_HORZ, _origin.x, TRUE);
	} else
		SetScrollRange(_crtWindow, SB_HORZ, 0, 0, FALSE);

	if (clientSize.y < _screenSize.y) {
		SetScrollRange(_crtWindow, SB_VERT, 0, max(1,range.y), FALSE);
		SetScrollPos(_crtWindow, SB_VERT, _origin.y, TRUE);
	} else
		SetScrollRange(_crtWindow, SB_VERT, 0, 0, FALSE);
} /* SetScrollBars */

/* ---- Set cursor position ---- */
static void
_cursorTo(int X, int Y)
{
	_cursor.x = RANGE(0, X, _screenSize.x - 1);
	_cursor.y = RANGE(0, Y, _screenSize.y - 1);
} /* _cursorTo */

/* ---- Scroll window to given origin ---- */
static void
_ScrollTo(int X, int Y)
{
	RECT	rc;
//TCHAR str[100];

	if (!_crtWindow) return;

	X = RANGE(0, X, range.x);
	Y = RANGE(0, Y, range.y);
	if ((X != _origin.x) || (Y != _origin.y)) {
		if (X!=_origin.x)
			SetScrollPos(_crtWindow, SB_HORZ, X, TRUE);
		if (Y!=_origin.y)
			SetScrollPos(_crtWindow, SB_VERT, Y, TRUE);
		GetClientRect(_crtWindow,&rc);
		ScrollWindowEx(_crtWindow,
			(_origin.x - X) * charSize.x,
			(_origin.y - Y) * charSize.y,
			&rc, &rc,
			NULL, NULL, SW_INVALIDATE | SW_ERASE);
		_origin.x = X;
		_origin.y = Y;
		OutputDebugString(_T("\tScroll\n"));
		windowPainted = FALSE;
		UpdateWindow(_crtWindow);
		if (!windowPainted) {
			InvalidateRect(_crtWindow, NULL, TRUE);
			UpdateWindow(_crtWindow);
		}
	}
} /* _ScrollTo */

/* ---- Scroll to make cursor visible ---- */
void __CDECL
WTrackCursor(void)
{
	_ScrollTo(max(_cursor.x - clientSize.x + 1,
		min(_origin.x, _cursor.x)),
		max(_cursor.y - clientSize.y + 1,
		min(_origin.y, _cursor.y)));
} /* WTrackCursor */

/* ---- Return pointer to location in screen buffer ---- */
static LPTSTR
ScreenPtr(int X, int Y)
{
	Y += firstLine;
	if (Y >= _screenSize.y)
		Y -= _screenSize.y;
	return screenBuffer+ (Y * _screenSize.x + X);
} /* ScreenPtr */

/* ---- Return pointer to location in color buffer ---- */
static BYTE *ColorPtr(int X, int Y)
{
	Y += firstLine;
	if (Y >= _screenSize.y)
		Y -= _screenSize.y;
	return (colorBuffer+ (Y * _screenSize.x + X));
} /* ColorPtr */

/* ---- ColorTextOut, displays color characters ---- */
static void ColorTextOut( int x, int y, int R )
{
	LPTSTR	 sb;
	BYTE	*cb,*c,col;
	int	i,L;

	sb = ScreenPtr(x, y);
	cb = ColorPtr( x, y);

	L = 0;
	while (L<R) {
		col = *cb;
		c   = cb+1;
		for (i=L+1; (i<R) && (col==*c); i++,c++)
			/* do nothing */;

		SetTextColor(DC, colorRef[col&0xf]);
		SetBkColor(DC, colorRef[ (col>>4)&0xf ]);
		ExtTextOut(DC,
			(x+L-_origin.x)*charSize.x,
			(y-_origin.y)*charSize.y+marginTop,
			ETO_OPAQUE, NULL,
			sb, i - L, NULL);
		cb = c;
		sb += i-L;
		L = i;
	}
} /* ColorTextOut */

/* ---- Update text on cursor line ---- */
static void
ShowText(int L, int R)
{
	if (L < R) {
		InitDeviceContext();
		ColorTextOut( L, _cursor.y, R-L );
//		ExtTextOut(DC, (L - _origin.x)*charSize.x,
//			(_cursor.y  - _origin.y)*charSize.y,
//			ETO_OPAQUE, NULL,
//			ScreenPtr(L, _cursor.y), R - L, NULL);
		DoneDeviceContext();
	}
} /* ShowText */

/* ---- Write text buffer to window ---- */
static void
NewLine(int *L, int *R)
{
	ShowText(*L, *R);
	*L = 0;
	*R = 0;
	_cursor.x = 0;
	++_cursor.y;
	if (_cursor.y == _screenSize.y) {
		RECT	rc;
		--_cursor.y;
		++firstLine;
		if (firstLine == _screenSize.y)
			firstLine = 0;
		unicodeMemSet(ScreenPtr(0, _cursor.y), TEXT(' '), _screenSize.x);
		memset(ColorPtr( 0, _cursor.y), activeColor, _screenSize.x);
		GetClientRect(_crtWindow,&rc);
		ScrollWindowEx(_crtWindow, 0, -charSize.y, &rc, &rc,
			NULL, NULL, SW_INVALIDATE | SW_ERASE);
		UpdateWindow(_crtWindow);
	}
} /* NewLine */

/* ---- Write the contents of Buffer ---- */
void __CDECL
WWriteBuf(LPTSTR Buffer, int Count)
{
	int L, R;

	/* Check if there is any break pending */
	if (breakActive) {
		breakActive = FALSE;
		(SignalBreak)(SIGINT);
	}

	L = _cursor.x;
	R = _cursor.x;
	_HideCursor();
	while (Count > 0) {
		if (Buffer[0] == -1)
			Buffer[0] = TEXT(' ');
		switch (Buffer[0]) {
			case 13:
			case 10:
				NewLine(&L, &R);
				break;
			case  9:
				do {
					*(ScreenPtr(_cursor.x,_cursor.y))=TEXT(' ');
					*(ColorPtr(_cursor.x, _cursor.y)) = activeColor;
					++_cursor.x;
					if (_cursor.x > R)
						R = _cursor.x;
					if (_cursor.x == _screenSize.x) {
						NewLine(&L, &R);
						break;
					}
				} while (_cursor.x % 8);
				break;
			case  8:
				if (_cursor.x > 0) {
					--_cursor.x;
					*(ScreenPtr(_cursor.x,_cursor.y))=TEXT(' ');
					*(ColorPtr(_cursor.x, _cursor.y)) = activeColor;
					if (_cursor.x < L )
						L = _cursor.x;
					}
				break;
			case  7:
				MessageBeep(0);
				break;
			default:
				*(ScreenPtr(_cursor.x, _cursor.y)) = Buffer[0];
				*(ColorPtr(_cursor.x, _cursor.y)) = activeColor;
				++_cursor.x;
				if (_cursor.x > R)
					R = _cursor.x;
				if (_cursor.x == _screenSize.x)
					NewLine(&L, &R);
		}

		++Buffer;
		--Count;
	}
	ShowText(L, R);
	WTrackCursor();
	_ShowCursor();
} /* WWriteBuf */

/* ---- Write character to window ---- */
void __CDECL
WWriteChar(TCHAR Ch)
{
	WWriteBuf(&Ch, 1);
} /* WWriteChar */

/* ---- Return keyboard status ---- */
BOOL __CDECL
WKeyPressed(void)
{
	MSG M;

	while (PeekMessage(&M, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&M);
		DispatchMessage(&M);
	}
	return (BOOL)((keyCount>0) || (clpKeyCount>0));
} /* WKeyPressed */

/* ---- Read key from window ---- */
int __CDECL
WReadKey(void)
{
	int readkey;

	if (!WKeyPressed()) {
		MSG M;
		while ( (keyCount==0) && (clpKeyCount==0) &&
			GetMessage(&M, 0, 0, 0)) {
				TranslateMessage(&M);
				DispatchMessage(&M);
			}
	}
	if (clpKeyCount) {
		readkey = (char)clpKeyBuffer[0];
		if (--clpKeyCount)
			memmove(clpKeyBuffer, clpKeyBuffer+1, clpKeyCount*sizeof(TCHAR));
		else
			LocalFree(clpKeyBuffer);
	} else {
		readkey = keyBuffer[0];
		--keyCount;
		memmove(keyBuffer, keyBuffer+1, keyCount);
	}
	return readkey;
} /* WReadKey */

/* ---- Set new active color ---- */
void __CDECL
WSetColor(BYTE col)
{
	activeColor = col;
//	DeleteObject(old...brush GetClassLong...);
//	SetClassLong(_crtWindow,
//			GCL_HBRBACKGROUND,
//			(LONG)CreateSolidBrush(colorRef[(col>>4)&0xf]));
} /* SetColor */

BYTE __CDECL
WGetColor(void)
{
	return activeColor;
} /* WGetColor */

/* ---- WGetPalette ---- */
long __CDECL
WGetPalette(int col)
{
	if (IN_RANGE(0,col,15))
		return colorRef[col];
	else
		return 0;
} /* WGetPalette */

/* ---- WSetPalette ---- */
void __CDECL
WSetPalette(int col, int red, int green, int blue)
{
	if (IN_RANGE(0,col,15)) {
		colorRef[col] = RGB(red,green,blue);
	}
} /* WSetPalette */

/* ---- WSetScrollBars ---- */
void __CDECL
WSetScrollBars(BOOL show)
{
	HMENU hm;
#if _WIN32_WCE > 211
	hm = SHGetSubMenu(hwndCB,ID_VIEW);
#else
	hm = GetSubMenu(CommandBar_GetMenu(hwndCB,0),2);
#endif
	CheckMenuItem(hm, ID_VIEW_SCROLLBARS,
			MF_BYCOMMAND |
			(show?MF_CHECKED:MF_UNCHECKED));
	showScrollBars = show;
	if (showScrollBars) {
		SetScrollBars();
		InvalidateRect(_crtWindow, NULL, TRUE);
	} else {
		SetScrollRange(_crtWindow, SB_HORZ, 0, 0, FALSE);
		SetScrollRange(_crtWindow, SB_VERT, 0, 0, FALSE);
		InvalidateRect(_crtWindow, NULL, TRUE);
	}
} /* WSetScrollBars */

/* ---- WSetMenu ---- */
void __CDECL
WSetMenu(BOOL show)
{
	HMENU hm;
	if (!hwndCB) return;
#if _WIN32_WCE > 211
	hm = SHGetSubMenu(hwndCB,ID_VIEW_MENU);
#else
	hm = GetSubMenu(CommandBar_GetMenu(hwndCB,0),2);
#endif
	CheckMenuItem(hm, ID_VIEW_MENU,
			MF_BYCOMMAND |
			(show?MF_CHECKED:MF_UNCHECKED));
//	if (show) {
//		RECT rcm;
//		GetWindowRect(hwndCB, &rcm);
//		marginTop= rcm.bottom;
//	} else
		marginTop = 0;
	showMenu = show;
	ShowWindow(hwndCB, showMenu?SW_SHOW:SW_HIDE);
} /* WSetMenu */

/* ---- WSetMarkOrPan ---- */
void __CDECL
WSetMarkOrPan(BOOL mark)
{
	HMENU hm;
#if _WIN32_WCE > 211
	hm = SHGetSubMenu(hwndCB,ID_EDIT);
#else
	hm = GetSubMenu(CommandBar_GetMenu(hwndCB,0),1);
#endif
	markTool = mark;
	CheckMenuItem(hm, ID_EDIT_MARK,
		MF_BYCOMMAND | (markTool?MF_CHECKED:MF_UNCHECKED));
}

/* ---- WGetScrollBars ---- */
BOOL __CDECL
WGetScrollBars(void)
{
	return showScrollBars;
} /* WGetScrollBars */

/* ---- WSetBreak ---- */
BOOL __CDECL
WSetBreak(int br)
{
	BOOL oldbr = _checkBreak;
	if (br==0 || br==1)
		_checkBreak = br;
	return oldbr;
} /* WSetBreak */

/* ---- Set cursor position ---- */
void __CDECL
WGotoXY(int X, int Y)
{
	_HideCursor();
	_cursorTo(X - 1, Y - 1);
	_ShowCursor();
} /* WGotoXY */

/* ---- Return cursor X position ---- */
int __CDECL
WWhereX(void)
{
	return (_cursor.x + 1);
} /* WWhereX */

/* ---- Return cursor Y position ---- */
int __CDECL
WWhereY(void)
{
	return(_cursor.y + 1);
} /* WWhereY */

/* ---- Clear screen ---- */
void __CDECL
WClrscr(void)
{
	unicodeMemSet(screenBuffer, TEXT(' '),_screenSize.x * _screenSize.y);
	memset(colorBuffer, activeColor, _screenSize.x * _screenSize.y);
	_cursor.x = 0;
	_cursor.y = 0;
	_origin.x = 0;
	_origin.y = 0;
	SetScrollBars();
	InvalidateRect(_crtWindow, NULL, TRUE);
	UpdateWindow(_crtWindow);
} /* Wclrscr */

/* ---- Clear to end of line ---- */
void __CDECL
WClreol(void)
{
	_HideCursor();
	unicodeMemSet(ScreenPtr(_cursor.x,_cursor.y),
			TEXT(' '),
			(_screenSize.x-_cursor.x));
	memset(ColorPtr(_cursor.x, _cursor.y),
			activeColor,
			_screenSize.x - _cursor.x);
	ShowText(_cursor.x, _screenSize.x);
	_ShowCursor();
} /* Wclreol */

/* ---- WMessage ---- */
LONG __CDECL
WMessage(UINT msg, WPARAM wParam, LONG lParam)
{
	return SendMessage(_crtWindow,msg,wParam,lParam);
} /* WMessage */

/* ---- SetFontSize ---- */
int __CDECL
WGetFontSize()
{
	return fontSize;
} /* WGetFontSize */

void __CDECL
WSetFontSize(int fh)
{
	LOGFONT	lf;
	SIZE	size;
	TEXTMETRIC Metrics;
	HMENU	hm;
	WPARAM	wid;

	fontSize = fh;

	// For the first time create the font (WCE)
	memset ((char *)&lf, 0, sizeof(lf));
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	lf.lfHeight = -(int)fh;
	_hFont = CreateFontIndirect (&lf);

	// Find Font metrics
	InitDeviceContext();
	GetTextMetrics(DC, &Metrics);
//WRONG on WindowsCE!!!		charSize.x = Metrics.tmMaxCharWidth;
	charSize.y = Metrics.tmHeight + Metrics.tmExternalLeading;
	charAscent = Metrics.tmAscent;
	GetTextExtentPoint(DC,_T("Hello"),5,&size); charSize.x = size.cx/5; charSize.y = size.cy;
	DoneDeviceContext();

#if _WIN32_WCE > 211
	hm = SHGetSubMenu(hwndCB,ID_VIEW);
#else
	hm = GetSubMenu(CommandBar_GetMenu(hwndCB,0),2);
#endif
	wid = fh-6+ID_FONT_6;
	CheckMenuRadioItem(hm, ID_FONT_6, ID_FONT_18, wid, MF_BYCOMMAND);

	if (_crtWindow != NULL) {
		WindowResize();
		InvalidateRect(_crtWindow, NULL, TRUE);
		UpdateWindow(_crtWindow);
	}
} /* WSetFontSize */

/* ---- FindEndColumn ---- */
static int
FindEndColumn(int row)
{
	int	col = _screenSize.x-1;
	TCHAR	*ch = ScreenPtr(col,row);
	while (col>0 && *ch==_T(' ')) {
		col--;
		ch--;
	}
	return col;
} /* FindEndColumn */

/* ---- MarkLine ---- */
static void
MarkLine(int row, int left, int right)
{
	int	y=(row-_origin.y)*charSize.y;
	Rectangle(DC,	(left-_origin.x)*charSize.x,
			y,
			(right-_origin.x+1)*charSize.x,
			y+charSize.y-1);
} /* MarkLine */

/* ---- DrawMarked area by inversing ---- */
static void
DrawMarkedArea(void)
{
	HBRUSH	hOldBrush;
	int	oldR2;
	int	row;
	POINT	*start, *stop;

	/* We assume that the previous routine has got a device context */
	if (markBegin.x==markEnd.x) return;

	hOldBrush = SelectObject(DC, GetStockObject(BLACK_BRUSH));
	oldR2 = SetROP2(DC,R2_NOT);

	if (markBegin.y <= markEnd.y) {
		start = &markBegin;
		stop  = &markEnd;
	} else {
		start = &markEnd;
		stop  = &markBegin;
	}

	if (start->y == stop->y)
		MarkLine(start->y, start->x, stop->x);
	else {
		row = start->y;
		MarkLine(row++, start->x, FindEndColumn(start->y));
		while (row<stop->y) {
			MarkLine(row, 0, FindEndColumn(row));
			row++;
		}
		MarkLine(row, 0, stop->x);
	}

	/* restore everything */
	SetROP2(DC,oldR2);
	SelectObject(DC,hOldBrush);
} /* DrawMarkedArea */

/* ---- CopyLine ---- */
static LPTSTR
CopyLine(LPTSTR buffer, int row, int left, int right, BOOL crlf)
{
	int	length = right-left+1;
	memcpy(buffer, ScreenPtr(left,row), length*sizeof(TCHAR));
	buffer += length;
	if (crlf) {
		*buffer++ = (TCHAR)0x0D;
		*buffer++ = (TCHAR)0x0A;
	}
	return buffer;
} /* CopyLine */

/* ---- Copy2Clipboard ---- */
static void
Copy2Clipboard()
{
	LPVOID	hMem;
	LPTSTR	buffer;
	size_t	siz;
	int	row;
	POINT	*start, *stop;

	markBegin.x = RANGE(0, markBegin.x, _screenSize.x - 1);
	markBegin.y = RANGE(0, markBegin.y, _screenSize.y - 1);
	markEnd.x   = RANGE(0, markEnd.x, _screenSize.x - 1);
	markEnd.y   = RANGE(0, markEnd.y, _screenSize.y - 1);

	if (markBegin.y <= markEnd.y) {
		start = &markBegin;
		stop  = &markEnd;
	} else {
		start = &markEnd;
		stop  = &markBegin;
	}

	/* First estimate the size we need */
	if (start->y == stop->y)
		siz = stop->x-start->x+1;
	else {
		row = start->y;
		siz = FindEndColumn(row++)-start->x+3;	// Allow space for CR LF
		while (row<stop->y)
			siz += FindEndColumn(row++)+3;	// CRLF
		siz += stop->x+1;
	}

	/* allocate buffer */
	hMem = LocalAlloc(LMEM_MOVEABLE,(siz+1)*sizeof(TCHAR));	// + ending zero
	buffer = (LPTSTR)hMem;

	/* copy to clipboard */
	if (start->y == stop->y)
		buffer = CopyLine(buffer,start->y,start->x,stop->x, FALSE);
	else {
		row = start->y;
		buffer = CopyLine(buffer,row++,start->x,FindEndColumn(start->y), TRUE);
		while (row<stop->y) {
			buffer = CopyLine(buffer,row,0,FindEndColumn(row), TRUE);
			row++;
		}
		buffer = CopyLine(buffer,row,0,stop->x, FALSE);
	}
	*buffer = 0;

	if (!OpenClipboard(_crtWindow))
		return;
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT,hMem);
	CloseClipboard();
} /* Copy2Clipboard */

/* ---- CopyAll ---- */
static void
CopyAll(void)
{
	markBegin.x = 0;
	markBegin.y = 0;
	markEnd.y = _screenSize.y;
	do {
		markEnd.y--;
		markEnd.x = FindEndColumn(markEnd.y);
	} while (markEnd.y>0 && markEnd.x==0);
	Copy2Clipboard();
} /* CopyAll */

/* ---- WM_CREATE message handler ---- */
static void
WindowCreate(HWND hWnd)
{
	int	bufferSize;
	int	var, len;
#if _WIN32_WCE > 211
	SHMENUBARINFO mbi;
	HMENU hm;

	// Pocket PC devices
	// Create a MenuBar for WCE devices
	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = hWnd;
	// Becarefull there should be an RCDATA section in the .rc file with the same name
//	mbi.dwFlags    = SHCMBF_HMENU;
	mbi.nToolBarId = IDM_MAIN_MENU;
	mbi.hInstRes   = _crtInstance;
	mbi.nBmpId     = 0;
	mbi.cBmpImages = 0;

	SHCreateMenuBar(&mbi);
	if (!SHCreateMenuBar(&mbi))
		MessageBox(hWnd, L"SHCreateMenuBar Failed", L"Error", MB_OK);
	hwndCB = mbi.hwndMB;
#else
	hwndCB = CommandBar_Create(_crtInstance, hWnd, 1);
	CommandBar_InsertMenubar(hwndCB, _crtInstance, IDM_MAIN_MENU, 0);
	CommandBar_AddAdornments(hwndCB, 0, 0);
#endif

	// Create the Popup menu
	hPopupMenu = CreatePopupMenu();
#if _WIN32_WCE > 211
	hm = SHGetSubMenu(hwndCB,ID_VIEW);
#endif
	AppendMenu(hPopupMenu,MF_ENABLED,ID_EDIT_COPYALL,TEXT("Copy &All"));
	AppendMenu(hPopupMenu,MF_ENABLED,ID_EDIT_PASTE,TEXT("&Paste"));
	AppendMenu(hPopupMenu,MF_ENABLED,ID_EDIT_CLEAR,TEXT("&Clear"));
	AppendMenu(hPopupMenu,MF_ENABLED,ID_VIEW_REFRESH,TEXT("&Refresh"));
	AppendMenu(hPopupMenu,MF_SEPARATOR,0,NULL);
#if _WIN32_WCE > 211
	AppendMenu(hPopupMenu,MF_POPUP,(UINT)hm,TEXT("&View"));
#endif
	AppendMenu(hPopupMenu,MF_ENABLED | (showMenu?MF_CHECKED:0),
				ID_VIEW_MENU,TEXT("&Menu"));
	AppendMenu(hPopupMenu,MF_ENABLED | (showScrollBars?MF_CHECKED:0),
				ID_VIEW_SCROLLBARS,TEXT("&Scrollbars"));
	AppendMenu(hPopupMenu,MF_ENABLED | (markTool?MF_CHECKED:0),
				ID_EDIT_MARK,TEXT("&Mark or Pan"));
	AppendMenu(hPopupMenu,MF_ENABLED,ID_EDIT_TRACKCURSOR,TEXT("&Track Cursor"));
	AppendMenu(hPopupMenu,MF_SEPARATOR,0,NULL);
	AppendMenu(hPopupMenu,MF_ENABLED,ID_ACTION_BREAK,TEXT("Break"));
	AppendMenu(hPopupMenu,MF_ENABLED,ID_ACTION_ABOUT,TEXT("About"));
	AppendMenu(hPopupMenu,MF_ENABLED,ID_ACTION_EXIT,TEXT("&Exit"));

	// Create Screen Buffer
	bufferSize = _screenSize.x * _screenSize.y;
	screenBuffer = (LPTSTR) malloc(bufferSize * sizeof(TCHAR));
	colorBuffer = (BYTE *) malloc(bufferSize);
	memset(colorBuffer, activeColor, bufferSize);

	unicodeMemSet(screenBuffer, TEXT(' '), bufferSize);

	// Read registry
	len = sizeof(var);
	if (!RXREGGETDATA(TEXT("Font"),REG_DWORD,&var,&len))
		var=11;
	WSetFontSize(var);
	if (!RXREGGETDATA(TEXT("Menu"),REG_DWORD,&var,&len))
		var=1;
	WSetMenu(var);
	if (!RXREGGETDATA(TEXT("Scrollbars"),REG_DWORD,&var,&len))
		var=1;
	WSetScrollBars(var);
	if (!RXREGGETDATA(TEXT("Mark"),REG_DWORD,&var,&len))
		var=1;
	markTool = var;
} /* WindowCreate */

/* ---- WM_PAINT message handler ---- */
static void
WindowPaint(void)
{
	int X1, X2, Y1, Y2;
	if (painting) return;

	windowPainted = TRUE;

	painting = TRUE;
	InitDeviceContext();
	_HideCursor();
	OutputDebugString(_T("\t\tPaint\n"));

	if (charSize.x != 0) {
		X1 = max(0, PS.rcPaint.left / charSize.x + _origin.x);
		X2 = min(_screenSize.x-1,
			(PS.rcPaint.right + charSize.x - 1) / charSize.x + _origin.x);
		Y1 = max(0, PS.rcPaint.top / charSize.y + _origin.y);
		Y2 = min(_screenSize.y-1,
			(PS.rcPaint.bottom + charSize.y - 1) / charSize.y + _origin.y);
		OutputDebugString(_T("\t\t\tDraw\n"));
		while (Y1 < Y2) {
			ColorTextOut(X1,Y1,X2-X1);
//			ExtTextOut(DC, (X1 - _origin.x) * charSize.x,
//				(Y1 - _origin.y) * charSize.y,
//				ETO_OPAQUE, NULL,
//				ScreenPtr(X1, Y1), X2 - X1, NULL);
			++Y1;
		}
	}

	DoneDeviceContext();
	_ShowCursor();
	painting = FALSE;
} /* WindowPaint */

/* ---- WM_VSCROLL and WM_HSCROLL message handler ---- */
static int
GetNewPos(int Pos, int Page, int Range, int Action, int Thumb)
{
	switch (Action) {
		case SB_LINEUP:
			return(Pos - 1);
		case SB_LINEDOWN:
			return(Pos + 1);
		case SB_PAGEUP:
			return(Pos - Page);
		case SB_PAGEDOWN:
			return(Pos + Page);
		case SB_TOP:
			return(0);
		case SB_BOTTOM:
			return(Range);
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			return(Thumb);
		default:
			return(Pos);
	}
} /* GetNewPos */

/* ---- WM_xSCROLL message ---- */
static void
WindowScroll(int Which, int Action, int Thumb)
{
	int X, Y;

	X = _origin.x;
	Y = _origin.y;
	switch (Which) {
		case SB_HORZ:
			X = GetNewPos(X, clientSize.x/2, range.x, Action, Thumb);
			break;
		case SB_VERT:
			Y = GetNewPos(Y, clientSize.y/2, range.y, Action, Thumb);
	}
	_HideCursor();
	_ScrollTo(X, Y);
	_ShowCursor();
} /* WindowScroll */


/* ---- WM_SIZE message handler, Initialise Screen settings ---- */
static void
WindowResize(void)
{
	RECT	rc;

	if (!_crtWindow) return;

	DestroyCaret();

	GetClientRect(_crtWindow,&rc);

	clientSize.x = (rc.right-rc.left) / charSize.x;
	clientSize.y = (rc.bottom-rc.top) / charSize.y - marginTop;

	range.x = max(0,_screenSize.x - clientSize.x);
	range.y = max(0,_screenSize.y - clientSize.y);
	_origin.x = MIN(_origin.x, range.x);
	_origin.y = MIN(_origin.y, range.y);

//	SHSipPreference(_crtWindow,SIP_UP);

	SetScrollBars();
	_CreateCursor();
} /* WindowResize */

/* ---- AddKey ---- */
static void
AddKey(char Ch)
{
	if (keyCount < sizeof(keyBuffer)) {
		keyBuffer[keyCount] = Ch;
		++keyCount;
	}
} /* AddKey */

/* ---- WM_CHAR message handler ---- */
static void
WindowChar(char Ch)
{
	if (_checkBreak  && (Ch == 3))  {
		WWriteBuf(TEXT("^C\n"),2);
		breakActive = TRUE;
	}

	if (_winTerminated) {
		if (Ch == 27) PostQuitMessage(0);
	} else
		AddKey(Ch);
} /* WindowChar */

/* ---- WM_KEYDOWN message handler ---- */
static void
WindowKeyDown(WPARAM WParam)
{
	if (GetAsyncKeyState(VK_SHIFT) || !markTool) {
		//TCHAR	buf[100];
		//wsprintf(buf,_T("Key=%d\n"),WParam);
		//WWriteBuf(buf,wcslen(buf));
		switch (WParam) {
			case 33:
				WindowScroll(SB_VERT,SB_PAGEUP,1);
				break;
			case 34:
				WindowScroll(SB_VERT,SB_PAGEDOWN,1);
				break;
			case 35:
				WindowScroll(SB_HORZ,SB_PAGERIGHT,1);
				break;
			case 36:
				WindowScroll(SB_HORZ,SB_PAGELEFT,1);
				break;

			case 37:
				WindowScroll(SB_HORZ,SB_LINELEFT,1);
				break;
			case 38:
				WindowScroll(SB_VERT,SB_LINEUP,1);
				break;
			case 39:
				WindowScroll(SB_HORZ,SB_LINERIGHT,1);
				break;
			case 40:
				WindowScroll(SB_VERT,SB_LINEDOWN,1);
				break;
		}
	} else
	if (IN_RANGE(33,(BYTE)WParam,46)) {
		AddKey(0);
		AddKey((BYTE)WParam);
	}
} /* WindowKeyDown */

/* ---- WM_LBUTTONDOWN message handler ---- */
static void
WindowButtonDown(int X, int Y)
{
#if _WIN32_WCE > 211
	SHRGINFO shrgi = {0};

	shrgi.cbSize = sizeof(SHRGINFO);
	shrgi.hwndClient = _crtWindow;
	shrgi.ptDown.x = X;
	shrgi.ptDown.y = Y;
	shrgi.dwFlags = SHRG_RETURNCMD;
#endif

	if (GetAsyncKeyState(VK_MENU)
#if _WIN32_WCE > 211
			|| SHRecognizeGesture(&shrgi)
#endif
			) {

		/* Modify the popup menu */
		CheckMenuItem(hPopupMenu, ID_VIEW_MENU,
				MF_BYCOMMAND | (showMenu?MF_CHECKED:MF_UNCHECKED));
		CheckMenuItem(hPopupMenu, ID_VIEW_SCROLLBARS,
				MF_BYCOMMAND | (showScrollBars?MF_CHECKED:MF_UNCHECKED));
		CheckMenuItem(hPopupMenu, ID_EDIT_MARK,
				MF_BYCOMMAND | (markTool?MF_CHECKED:MF_UNCHECKED));
		TrackPopupMenu(hPopupMenu,0, X, Y, 0, _crtWindow,NULL);
	} else {
		_HideCursor();
		if (markTool) {
			InitDeviceContext();
			SetCapture(_crtWindow);
			markBegin.x = markEnd.x = X/charSize.x + _origin.x;
			markBegin.y = markEnd.y = Y/charSize.y + _origin.y;
		} else {
			markBegin.x = markEnd.x = X/charSize.x;
			markBegin.y = markEnd.y = Y/charSize.y;
			oldOrigin.x = _origin.x;
			oldOrigin.y = _origin.y;
		}
		marking = TRUE;
	}
} /* WindowButtonDown */

/* ---- WM_LBUTTONMOVE message handler ---- */
static void
WindowButtonMove(int X, int Y)
{
	int	dx, dy;

	if (markTool) {
		/* Find the new position */
		dx = X/charSize.x + _origin.x;
		dy = Y/charSize.y + _origin.y;

		/* Redraw only when different to avoid flickering */
		if (dx != markEnd.x || dy != markEnd.y) {
			/* Draw old area to erase it */
			DrawMarkedArea();

			/* Update the position */
			markEnd.x = dx;
			markEnd.y = dy;

			/* Draw the new marked area */
			DrawMarkedArea();
		}
	} else {
		dx = X/charSize.x - markBegin.x;
		dy = Y/charSize.y - markBegin.y;

		if (dx != markEnd.x || dy != markEnd.y) {
			OutputDebugString(_T("Move\n"));
			_ScrollTo(oldOrigin.x - dx,  oldOrigin.y - dy);

			/* Update the position */
			markEnd.x = dx;
			markEnd.y = dy;
		}
	}
} /* WindowButtonMove */

/* ---- WM_LBUTTONUP message handler ---- */
static void
WindowButtonUp()
{
	marking = FALSE;
	if (markTool) {
		DrawMarkedArea();
		_ShowCursor();
		ReleaseCapture();
		DoneDeviceContext();
		Copy2Clipboard();
	} else {
//		InvalidateRect(_crtWindow, NULL, TRUE);
//		UpdateWindow(_crtWindow);
	}
} /* WindowButtonUp */

/* ---- WM_DESTROY message handler ---- */
static void WindowDestroy(void)
{
	_winTerminated = TRUE;
	free(screenBuffer);
	free(colorBuffer);
	DeleteObject((HGDIOBJ)_hFont);
	DestroyMenu(hPopupMenu);
	DestroyWindow(hwndCB);
	PostQuitMessage(0);
} /* WindowDestroy */

/* ---- Mesage handler for the About box. ---- */
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
		case WM_INITDIALOG: {
#if _WIN32_WCE > 211
			//On Pocket PC devices you normally create all Dialog's as fullscreen dialog's
			// with an OK button in the upper corner.
			SHINITDLGINFO shidi;
			// Create a Done button and size it.
			shidi.dwMask = SHIDIM_FLAGS;
			shidi.dwFlags = SHIDIF_DONEBUTTON | SHIDIF_SIPDOWN | SHIDIF_SIZEDLGFULLSCREEN;
			shidi.hDlg = hDlg;
			//initialzes the dialog based on the dwFlags parameter
			SHInitDialog(&shidi);
#endif
			}
			return TRUE;

		case WM_COMMAND:
			if ((LOWORD(wParam) == IDOK) || (LOWORD(wParam) == IDCANCEL)) {
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
	return FALSE;
} /* About */

/* ---- WM_COMMAND message handler ---- */
static int
WindowCommand(WPARAM WParam)
{
	switch (WParam) {
		case ID_ACTION_RUN:
			break;

		case ID_ACTION_BREAK:
			breakActive = TRUE;
			break;

		case ID_ACTION_ABOUT:
			DialogBox(_crtInstance, (LPCTSTR)IDD_ABOUTBOX, _crtWindow, (DLGPROC)About);
			break;

		case ID_ACTION_EXIT:
			SendMessage(_crtWindow,WM_CLOSE,0,0);
			break;

		case ID_EDIT_COPYALL:
			CopyAll();
			break;

		case ID_EDIT_PASTE:
			if (!OpenClipboard(_crtWindow)) break;
			clpKeyBuffer =
				(LPTSTR)GetClipboardDataAlloc(CF_UNICODETEXT);
			CloseClipboard();
			clpKeyCount = wcslen(clpKeyBuffer);
			break;

		case ID_EDIT_CLEAR:
			_HideCursor();
			WClrscr();
			_ShowCursor();
			break;

		case ID_EDIT_MARK:
			WSetMarkOrPan(!markTool);
			break;

		case ID_VIEW_SCROLLBARS:
			WSetScrollBars(!showScrollBars);
			break;

		case ID_VIEW_MENU:
			WSetMenu(!showMenu);
			InvalidateRect(_crtWindow, NULL, TRUE);
			UpdateWindow(_crtWindow);
			break;

		case ID_EDIT_TRACKCURSOR:
			_HideCursor();
			WTrackCursor();
			_ShowCursor();
			break;

		case ID_VIEW_REFRESH:
			InvalidateRect(_crtWindow, NULL, TRUE);
			UpdateWindow(_crtWindow);
			break;

		case ID_FONT_6:
		case ID_FONT_7:
		case ID_FONT_8:
		case ID_FONT_9:
		case ID_FONT_10:
		case ID_FONT_11:
		case ID_FONT_12:
		case ID_FONT_14:
		case ID_FONT_16:
		case ID_FONT_18:
		case ID_FONT_20:
			WSetFontSize(WParam-ID_FONT_6+6);
			break;

		default:
			return FALSE;
	}
	return TRUE;
} /* WindowCommand */

/* ---- WindowIO window procedure ---- */
LRESULT CALLBACK
_WinIOProc(HWND Window, UINT Message, WPARAM WParam, LONG LParam)
{
#if _WIN32_WCE > 211
	static SHACTIVATEINFO sai;
#endif

//TCHAR str[100];
//swprintf(str,_T("WindowIO: %d %d %d\n"),Message,WParam,LParam);
//if (Message!=WM_MOUSEMOVE && Message!=32)
//	OutputDebugString(str);

	switch (Message) {
		case WM_CREATE:
			WindowCreate(Window);
			break;

		case WM_PAINT:
			WindowPaint();
			break;

		case WM_SIZE:
#if _WIN32_WCE < 211
			if (!_crtWindow)
				_crtWindow = Window;
#endif
			WindowResize();
			break;

#if _WIN32_WCE > 211
		case WM_SETTINGCHANGE:
			if (WParam == SPI_SETSIPINFO) {
				memset(&sai, 0, sizeof(SHACTIVATEINFO));
				SHHandleWMSettingChange(Window, -1, 0L, &sai);
			}
			break;

		case WM_ACTIVATE:
			if (WParam == SPI_SETSIPINFO) {
				memset(&sai, 0, sizeof(SHACTIVATEINFO));
				SHHandleWMActivate(Window,WParam,LParam,&sai,0);
			}
			break;
#endif
		case WM_VSCROLL:
			WindowScroll(SB_VERT, LOWORD(WParam),HIWORD(WParam));
			break;

		case WM_HSCROLL:
			WindowScroll(SB_HORZ, LOWORD(WParam),HIWORD(WParam));
			break;

		case WM_CHAR:
			WindowChar((char)WParam);
			break;

		case WM_KEYDOWN:
			WindowKeyDown(WParam);
			break;

		case WM_LBUTTONDOWN:
			WindowButtonDown(LOWORD(LParam),HIWORD(LParam)-marginTop);
			break;

		case WM_MOUSEMOVE:
			if (marking)
				WindowButtonMove(LOWORD(LParam),HIWORD(LParam)-marginTop);
			break;

		case WM_LBUTTONUP:
			if (marking)
				WindowButtonUp();
			break;

		case WM_COMMAND:
			if (!WindowCommand(WParam))
				DefWindowProc(Window, Message, WParam, LParam);
			break;

		//case WM_LBUTTONDBLCLK:
			/* A little tricky find one word to mark */
			//break;

		case WM_SETFOCUS:
			focused = TRUE;
			InvalidateRect(Window, NULL, TRUE);
			UpdateWindow(Window);
			_CreateCursor();
			break;

		case WM_KILLFOCUS:
			DestroyCaret();
			focused = FALSE;
			break;

		/*** WM_QUIT, needs special handling, DO NOT UNCOMMENT */
		/*** case WM_QUIT: ***/
		/***	break; ***/

		case WM_CLOSE:
			DefWindowProc(Window, Message, WParam, LParam);
			ExitThread(0);
			break;

		case WM_DESTROY:
			/* Update variables in registry */
			RXREGSETDATA(TEXT("Font"),REG_DWORD,&fontSize,sizeof(DWORD));
			RXREGSETDATA(TEXT("Menu"),REG_DWORD,&showMenu,sizeof(DWORD));
			RXREGSETDATA(TEXT("Scrollbars"),REG_DWORD,&showScrollBars,sizeof(DWORD));
			RXREGSETDATA(TEXT("Mark"),REG_DWORD,&markTool,sizeof(DWORD));
			WindowDestroy();
			break;

		default:
			return DefWindowProc(Window, Message, WParam, LParam);
	}
	return FALSE;
} /* _WinIOProc */

/* ---- Create window if required ---- */
int __CDECL
WInitWinIO(HINSTANCE hInst, HINSTANCE hPrev, int cmdShow)
{
#if _WIN32_WCE > 211
	SIPINFO si = {0};
	int	iDelta;
#endif
	int	cx, cy;

	WNDCLASS CrtClass = {
		CS_HREDRAW | CS_VREDRAW,
		_WinIOProc,
		0,
		0,
		0,			// Instance
		0,			// Icon
		0,
		0,			// Brush
		NULL,			// Menu
		TEXT(PACKAGE_NAME)	// Name
	};

	if (hPrev == 0) {
		CrtClass.hInstance = hInst;
		CrtClass.hIcon     = LoadIcon(hInst,MAKEINTRESOURCE(REXXICON));
		CrtClass.hbrBackground = CreateSolidBrush(colorRef[15]);
		//CrtClass.hbrBackground = GetStockObject(WHITE_BRUSH);

		if (RegisterClass(&CrtClass)==0)
			return GetLastError();
	}


#if _WIN32_WCE > 211
	si.cbSize = sizeof(si);
	SHSipInfo(SPI_GETSIPINFO, 0, &si, 0);

	// Consider the menu at the bottom
	iDelta = (si.fdwFlags & SIPF_ON)? 0 : MENU_HEIGHT;
	cx = si.rcVisibleDesktop.right - si.rcVisibleDesktop.left;
	cy = si.rcVisibleDesktop.bottom - si.rcVisibleDesktop.top - iDelta;
#else
	cx = CW_USEDEFAULT;
	cy = CW_USEDEFAULT;
#endif

	_crtInstance = hInst;
#if _WIN32_WCE > 211
	_crtWindow = CreateWindow(
			CrtClass.lpszClassName,
			CrtClass.lpszClassName,
			WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL,
			CW_USEDEFAULT, CW_USEDEFAULT,
			cx, cy,
			(HWND)NULL,
			NULL,
			_crtInstance,
			(LPTSTR)NULL);
#elif _WIN32_WCE_EMULATION
	_crtWindow = CreateWindow(
			CrtClass.lpszClassName,
			CrtClass.lpszClassName,
			WS_POPUP | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL,
			0, 0,
			cx, cy,
			(HWND)NULL,
			NULL,
			_crtInstance,
			(LPTSTR)NULL);
#else
	_crtWindow = CreateWindow(
			CrtClass.lpszClassName,
			CrtClass.lpszClassName,
			WS_VISIBLE | WS_VSCROLL,
			CW_USEDEFAULT, CW_USEDEFAULT,
			cx, cy,
			(HWND)NULL,
			NULL,
			_crtInstance,
			(LPTSTR)NULL);
#endif
	GetModuleFileName(hInst, moduleName, sizeof(moduleName)/sizeof(TCHAR));

#if _WIN32_WCE < 211
	if (hwndCB) {
		RECT rc, rcm;
		GetWindowRect(_crtWindow, &rc);
		GetWindowRect(hwndCB, &rcm);
		WSetMenu(showMenu);
		WSetScrollBars(showScrollBars);
//		marginTop= rcm.bottom;
//		rc.top = rcm.bottom;
		MoveWindow(_crtWindow, rc.left, rc.top, rc.right, rc.bottom, TRUE);
	}
#endif

	ShowWindow(_crtWindow, cmdShow);
#if defined(WCE)
	InvalidateRect(_crtWindow, NULL, TRUE);
#endif
	UpdateWindow(_crtWindow);

	/* Set our Icon */
	SendMessage(_crtWindow,WM_SETICON,FALSE,
		(LPARAM)LoadImage(_crtInstance,MAKEINTRESOURCE(REXXICON),
				IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR));

	return 0;
} /* WInitWinIO */

/* ---- WinIO unit exit procedure ---- */
void __CDECL
WExitWinIO(void)
{
	MSG	Message;
	TCHAR	Title[128], OldTitle[128];

	if (_crtWindow) {	/* Wait for windows to exit */
		GetWindowText(_crtWindow, OldTitle, sizeof(OldTitle)/sizeof(TCHAR));
		wsprintf(Title, TEXT("[ %s ]"), OldTitle);
		DestroyCaret();
		SetWindowText(_crtWindow, Title);
		_checkBreak = FALSE;
		SignalBreak = NULL;
		breakActive = FALSE;
		_winTerminated = TRUE;
		while (GetMessage(&Message, 0, 0, 0)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
} /* WExitWinIO */
