/*
 * $Header: /home/bnv/tmp/brexx/win/RCS/winio.c,v 1.1 1999/09/13 15:06:41 bnv Exp $
 * $Log: winio.c,v $
 * Revision 1.1  1999/09/13 15:06:41  bnv
 * Initial revision
 *
 */

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include <os.h>
#include <ldefs.h>
#include <bstr.h>
#ifdef __BORLANDC__
#	define	TEXT(x)	x
#	define	TCHAR	char  
#	define	LPTSTR	LPSTR
#else
#	include <tchar.h>
#endif

#define	INITIAL_LINES	100
#define	PRGNAME		TEXT("WinIO")

// --- Popup Menu (Alt-Tab) Messages ---
#define PM_COPYALL	9100
#define PM_PASTE	9101
#define PM_CLEAR	9102
#define PM_TRACK	9200
#define PM_ABOUT	9999

POINT	_Origin;				// Client Area Origin
POINT	_ScreenSize;				// Screen buffer dimensions
POINT	_Cursor = { 0, 0 };			// Cursor location
BOOL	_CheckBreak = TRUE;			// Allow Ctrl-C for break?
BOOL	_WinTerminated = FALSE;			// Escape exists program
HFONT	_hFont;					// Current working font
HWND	_CrtWindow = 0;				// CRT window handle
HINSTANCE	_CrtInstance;			// CRT class instance

LRESULT CALLBACK _WinIOProc(HWND Window, UINT Message,
				WPARAM WParam, LONG LParam);

static BOOL	marking=FALSE;                  
static POINT	markBegin={0,0};	// Beggining of markting                     
static POINT	markEnd={0,0};		// End of marking area
static POINT	ClientSize;		// Client Area
static POINT	Range;			// Scroll bar ranges
static int	FirstLine  = 0;		// First line in circular buffer
static int	KeyCount   = 0;		// Count of keys in KeyBuffer
static BOOL	Focused = FALSE;	// Window focused?
static BOOL	Reading = FALSE;	// Reading from window?
static BOOL	Painting = FALSE;	// Handling wm_Paint?

static LPTSTR	ScreenBuffer;		// Screen buffer pointer
static POINT	CharSize;		// Character cell size
static int	CharAscent;		// Character ascent
static HDC	DC;			// Global device context
static PAINTSTRUCT	PS;		// Global paint structure
static HFONT	SaveFont;		// Saved device context font
static char	KeyBuffer[32];		// Keyboard type-ahead buffer

static int	ClpKeyCount=0;		// Keyboard buffer from Clipboard
static TCHAR	*ClpKeyBuffer;

#define	SIGINT		0
#define	SIG_IGN		NULL
static	void	(*SignalBreak)(int sig) = NULL;
static	BOOL	BreakActive = FALSE;
static	TCHAR	ModuleName[80];

/* ---- Wsignal ---- */
/* Substitue of the signal(), for trapping the Control-C */
void
Wsignal(int sig, void (*func)(int sig))
{
	switch (sig) {
		case SIGINT:
			BreakActive = FALSE;
			SignalBreak = func;
			_CheckBreak = (func!=NULL);
			break;
		default:
			break;
	}
} /* Wsignal */

/* ---- WSetTitle ---- */
void
WSetTitle(const char *title)
{
	size_t	len;
	TCHAR	str[100];
	len = STRLEN(title);
	mbstowcs(str,title,len+1);
	if (_CrtWindow)
		SetWindowText(_CrtWindow,str);
} /* WSetTitle */

/* ---- some Helper routines ----- */
static void
unicodeMemSet(LPTSTR buf, TCHAR ch, int len)
{
	int	i;
	for (i=0; i<len; i++)
		buf[i] = ch;
} /* unicodeMemSet */

/* ------------ Allocate device context ----------- */
static void
InitDeviceContext(void)
{
	if (Painting)
		DC = BeginPaint(_CrtWindow, &PS);
	else
		DC = GetDC(_CrtWindow);

	SaveFont = (HFONT)SelectObject (DC, _hFont);
} /* InitDeviceContext */

/* ------------ Release device context ------------ */
static void
DoneDeviceContext(void)
{
	SelectObject(DC, SaveFont);
	if (Painting)
		EndPaint(_CrtWindow, &PS);
	else
		ReleaseDC(_CrtWindow, DC);
} /* DoneDeviceContext */

/* ---------- Show caret --------- */
static void
_ShowCursor(void)
{
	CreateCaret(_CrtWindow, 0, CharSize.x, 2);
	SetCaretPos(	(_Cursor.x - _Origin.x) * CharSize.x,
			(_Cursor.y - _Origin.y) * CharSize.y + CharAscent);
	ShowCaret(_CrtWindow);
} /* _ShowCursor */

/* ----------- Hide caret ------------ */
static void
_HideCursor(void)
{
	DestroyCaret();
} /* _HideCursor */

/* ----------- UpdateScroll bars ------ */
static void
SetScrollBars()
{
	SetScrollRange(_CrtWindow, SB_VERT, 0, Range.y, FALSE);
	SetScrollPos(_CrtWindow, SB_VERT, _Origin.y, TRUE);
} /* SetScrollBars */

/* --------- Set cursor position ---------- */
static void
_CursorTo(int X, int Y)
{
	_Cursor.x = RANGE(0, X, _ScreenSize.x - 1);
	_Cursor.y = RANGE(0, Y, _ScreenSize.y - 1);
} /* _CursorTo */

/* ------ Scroll window to given origin ------ */
static void
_ScrollTo(int X, int Y)
{
	if (_CrtWindow) {
		RECT	rc;
		X = RANGE(0, X, Range.x);
		Y = RANGE(0, Y, Range.y);
		if ((X != _Origin.x) || (Y != _Origin.y)) {
			//if (X != _Origin.x)
			//	SetScrollPos(CrtWindow, SB_HORZ, X, TRUE);
			if (Y != _Origin.y)
				SetScrollPos(_CrtWindow, SB_VERT, Y, TRUE);
			GetClientRect(_CrtWindow,&rc);
			ScrollWindowEx(_CrtWindow,
				(_Origin.x - X) * CharSize.x,
				(_Origin.y - Y) * CharSize.y,
				&rc, &rc,
				NULL, NULL, SW_INVALIDATE | SW_ERASE);
			_Origin.x = X;
			_Origin.y = Y;
			UpdateWindow(_CrtWindow);
		}
	}
} /* _ScrollTo */

/* ----- Scroll to make cursor visible ------ */
void
WTrackCursor(void)
{
	_ScrollTo(max(_Cursor.x - ClientSize.x + 1,
		min(_Origin.x, _Cursor.x)),
		max(_Cursor.y - ClientSize.y + 1,
		min(_Origin.y, _Cursor.y)));
} /* WTrackCursor */

/* ------ Return pointer to location in screen buffer ----------- */
static LPTSTR
ScreenPtr(int X, int Y)
{
	Y += FirstLine;
	if (Y >= _ScreenSize.y)
		Y -= _ScreenSize.y;
	return ScreenBuffer+ (Y * _ScreenSize.x + X);
} /* ScreenPtr */

/* ------ Update text on cursor line -------- */
static void
ShowText(int L, int R)
{
	if (L < R) {
		InitDeviceContext();
		ExtTextOut(DC, (L - _Origin.x)*CharSize.x,
			(_Cursor.y  - _Origin.y)*CharSize.y,
			ETO_OPAQUE, NULL,
			ScreenPtr(L, _Cursor.y), R - L, NULL);
		DoneDeviceContext();
	}
} /* ShowText */

/* ------ Write text buffer to window -------- */
static void
NewLine(int *L, int *R)
{
	ShowText(*L, *R);
	*L = 0;
	*R = 0;
	_Cursor.x = 0;
	++_Cursor.y;
	if (_Cursor.y == _ScreenSize.y) {
		RECT	rc;
		--_Cursor.y;
		++FirstLine;
		if (FirstLine == _ScreenSize.y)
			FirstLine = 0;
		unicodeMemSet(ScreenPtr(0, _Cursor.y), TEXT(' '), _ScreenSize.x);
		GetClientRect(_CrtWindow,&rc);
		ScrollWindowEx(_CrtWindow, 0, -CharSize.y, &rc, &rc,
			NULL, NULL, SW_INVALIDATE | SW_ERASE);
		UpdateWindow(_CrtWindow);
	}
} /* NewLine */

/* --------- Write the contents of Buffer ----------- */
void
WWriteBuf(LPTSTR Buffer, WORD Count)
{
	int L, R;

	/* Check if there is any break pending */
	if (BreakActive) {
		BreakActive = FALSE;
		(SignalBreak)(SIGINT);
	}

	L = _Cursor.x;
	R = _Cursor.x;
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
					*(ScreenPtr(_Cursor.x,_Cursor.y))=TEXT(' ');
					++_Cursor.x;
					if (_Cursor.x > R)
						R = _Cursor.x;
					if (_Cursor.x == _ScreenSize.x) {
						NewLine(&L, &R);
						break;
					}
				} while (_Cursor.x % 8);
				break;
			case  8:
				if (_Cursor.x > 0) {
					--_Cursor.x;
					*(ScreenPtr(_Cursor.x,_Cursor.y))=TEXT(' ');
					if (_Cursor.x < L )
						L = _Cursor.x;
					}
				break;
			case  7:
				MessageBeep(0);
				break;
			default:
				*(ScreenPtr(_Cursor.x, _Cursor.y)) = Buffer[0];
				++_Cursor.x;
				if (_Cursor.x > R)
					R = _Cursor.x;
				if (_Cursor.x == _ScreenSize.x)
					NewLine(&L, &R);
		}

		++Buffer;
		--Count;
	}
	ShowText(L, R);
	WTrackCursor();
} /* WWriteBuf */

/* ------- Write character to window ---------- */
void
WWriteChar(TCHAR Ch)
{
	WWriteBuf(&Ch, 1);
} /* WWriteChar */

/* ------ Return keyboard status -------- */
BOOL
WKeyPressed(void)
{
	MSG M;

	while (PeekMessage(&M, 0, 0, 0, PM_REMOVE)) {
		TranslateMessage(&M);
		DispatchMessage(&M);
	}
	return (BOOL)((KeyCount>0) || (ClpKeyCount>0));
} /* WKeyPressed */

/* ------- Read key from window ---------- */
int
WReadKey(void)
{
	int readkey;

	if (!WKeyPressed()) {
		MSG M;
		Reading = TRUE;
		if (Focused)
			_ShowCursor();
		while ( (KeyCount==0) && (ClpKeyCount==0) &&
			GetMessage(&M, 0, 0, 0)) {
				TranslateMessage(&M);
				DispatchMessage(&M);
			}
		if (Focused)
			_HideCursor();
		Reading = FALSE;
	}
	if (ClpKeyCount) {
		readkey = (char)ClpKeyBuffer[0];
		if (--ClpKeyCount)
			memmove(ClpKeyBuffer, ClpKeyBuffer+1, ClpKeyCount*sizeof(TCHAR));
		else
			LocalFree(ClpKeyBuffer);
	} else {
		readkey = KeyBuffer[0];
		--KeyCount;
		memmove(KeyBuffer, KeyBuffer+1, KeyCount);
	}
	return readkey;
} /* WReadKey */

/* -------- Set cursor position ------------- */
void
WGotoXY(int X, int Y)
{
	_CursorTo(X - 1, Y - 1);
} /* WGotoXY */

/* ------- Return cursor X position --------- */
int
WWhereX(void)
{
	return (_Cursor.x + 1);
} /* WWhereX */

/* ------- Return cursor Y position ------- */
int
WWhereY(void)
{
	return(_Cursor.y + 1);
} /* WWhereY */

/* ------ Clear screen ------ */
void
WClrscr(void)
{
	unicodeMemSet(ScreenBuffer, TEXT(' '),_ScreenSize.x * _ScreenSize.y);
	_Cursor.x = 0;
	_Cursor.y = 0;
	_Origin.x = 0;
	_Origin.y = 0;
	SetScrollBars();
	InvalidateRect(_CrtWindow, NULL, TRUE);
	UpdateWindow(_CrtWindow);
} /* Wclrscr */

/* ------ Clear to end of line --------- */
void
WClreol(void)
{
	unicodeMemSet(ScreenPtr(_Cursor.x,_Cursor.y),TEXT(' '),
			(_ScreenSize.x-_Cursor.x));
	ShowText(_Cursor.x, _ScreenSize.x);
} /* Wclreol */          

/* ------- DrawMarked area by inversing --------------- */             
static void
DrawMarkedArea(void)
{         
	HBRUSH	hOldBrush;	
	int	oldR2;
	int	top, bottom;
	
	/* We assume that the previous routine has got a device context */	    
	if (markBegin.x==markEnd.x) return;

	hOldBrush = SelectObject(DC, GetStockObject(BLACK_BRUSH));
	oldR2 = SetROP2(DC,R2_NOT);

	top = (markBegin.y-_Origin.y)*CharSize.y;        
	bottom = (markEnd.y-_Origin.y)*CharSize.y;
        
	if (top>bottom) {
		int tmp = top;
		top = bottom;
		bottom = tmp;
	}                    
	bottom += CharSize.y;
                                         
	Rectangle(DC,	(markBegin.x-_Origin.x)*CharSize.x,
			top,
			(markEnd.x-_Origin.x)*CharSize.x,
			bottom);
        
	/* restore everything */
	SetROP2(DC,oldR2);
	SelectObject(DC,hOldBrush);
} /* DrawMarkedArea */                   

/* ---- Copy2Clipboard ------ */
static void
Copy2Clipboard()        
{                          
	LPVOID	hMem;
	LPTSTR	dst;
	int	y;
	size_t	siz;
	
	if (markBegin.x > markEnd.x) {
		int	tmp;
		tmp = markBegin.x;
		markBegin.x = markEnd.x;
		markEnd.x = tmp;
	}
	if (markBegin.y > markEnd.y) {
		int	tmp;
		tmp = markBegin.y;
		markBegin.y = markEnd.y;
		markEnd.y = tmp;		
	}                       
	
	siz = (markEnd.x - markBegin.x+1) * (markEnd.y-markBegin.y+1);	
	if (siz==0) return;
	
	hMem = LocalAlloc(LMEM_MOVEABLE,(siz+2*(markEnd.y-markBegin.y)+2)*sizeof(TCHAR));
	dst = (LPTSTR)hMem;
	for (y=markBegin.y; y<=markEnd.y; y++) {
		TCHAR	*ptr;
		size_t	length = markEnd.x - markBegin.x;
		/* remove trailing spaces */
		ptr = ScreenPtr(markEnd.x-1,y);
		while (length && (*ptr==TEXT(' '))) {
			ptr--;
			length--;
		}                             
		if (length) {
			memcpy(dst, ScreenPtr(markBegin.x,y), sizeof(TCHAR)*length);
			dst += length;
		}
		if (y<markEnd.y) {
			*dst++ = (TCHAR)0x0D;
			*dst++ = (TCHAR)0x0A;
		}
	}                          
	// -- Search from the bottom to remove all Empty CR-LF --
	while (dst>(LPTSTR)hMem) {
		if ((*(dst-1)==(TCHAR)0x0A) && (*(dst-3)==(TCHAR)0x0A))
			dst -= 2;
		else
			break;
	}
	*dst = (TCHAR)0;
	
	if (!OpenClipboard(_CrtWindow))
		return;
	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT,hMem);
	CloseClipboard();	
} /* Copy2Clipboard */

/* ---- WM_PAINT message handler ----- */
static void
WindowPaint(void)
{
	int X1, X2, Y1, Y2;

	Painting = TRUE;
	InitDeviceContext();

	X1 = max(0, PS.rcPaint.left / CharSize.x + _Origin.x);
	X2 = min(_ScreenSize.x,
		(PS.rcPaint.right + CharSize.x - 1) / CharSize.x + _Origin.x);
	Y1 = max(0, PS.rcPaint.top / CharSize.y + _Origin.y);
	Y2 = min(_ScreenSize.y,
		(PS.rcPaint.bottom + CharSize.y - 1) / CharSize.y + _Origin.y);
	while (Y1 < Y2) {
		ExtTextOut(DC, (X1 - _Origin.x) * CharSize.x,
			(Y1 - _Origin.y) * CharSize.y,
			ETO_OPAQUE, NULL,
			ScreenPtr(X1, Y1), X2 - X1, NULL);
		++Y1;
	}                                                             
	DoneDeviceContext();
	Painting = FALSE;
} /* WindowPaint */

/* ------ WM_SIZE message handler, Initialise Screen settings -------- */
static void
WindowResize(void)
{
	RECT	rc;
	static BOOL	FirstTime = TRUE;

	if (Focused && Reading)
		_HideCursor();

	if (FirstTime) {
		// Find Font metrics
		TEXTMETRIC Metrics;
		LOGFONT	lf;

		// For the first time create the font (WCE)
		memset ((char *)&lf, 0, sizeof(lf));
		lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
		lf.lfHeight = 14;
		_hFont = CreateFontIndirect (&lf);

		// Initialise the Device Context
		InitDeviceContext();
		GetTextMetrics(DC, &Metrics);
		CharSize.x = Metrics.tmMaxCharWidth;
		CharSize.y = Metrics.tmHeight + Metrics.tmExternalLeading;
		CharAscent = Metrics.tmAscent;
		DoneDeviceContext();
		FirstTime = FALSE;

		GetClientRect(_CrtWindow,&rc);

		ClientSize.x = (rc.right-rc.left) / CharSize.x;
		ClientSize.y = (rc.bottom-rc.top) / CharSize.y;
		_Origin.x = 0;
		_Origin.y = 0;		
		_ScreenSize.x = ClientSize.x;
		_ScreenSize.y = INITIAL_LINES;
		Range.x = max(0,_ScreenSize.x - ClientSize.x);
		Range.y = max(0,_ScreenSize.y - ClientSize.y);                              

		ScreenBuffer = (LPTSTR) malloc(_ScreenSize.x *
					_ScreenSize.y * sizeof(TCHAR));

		unicodeMemSet(ScreenBuffer, TEXT(' '),
			_ScreenSize.x * _ScreenSize.y);       
	} else {
		GetClientRect(_CrtWindow,&rc);
		ClientSize.x = (rc.right-rc.left) / CharSize.x;
		ClientSize.y = (rc.bottom-rc.top) / CharSize.y;
		Range.x = max(0,_ScreenSize.x - ClientSize.x);
		Range.y = max(0,_ScreenSize.y - ClientSize.y);
		_Origin.x = MIN(_Origin.x, Range.x);
		_Origin.y = MIN(_Origin.y, Range.y);
	}

	SetScrollBars();

	if (Focused && Reading)
		_ShowCursor();
} /* WindowResize */

/* ------ AddKey ------ */
static void
AddKey(char Ch)
{
	if (KeyCount < sizeof(KeyBuffer)) {
		KeyBuffer[KeyCount] = Ch;
		++KeyCount;
	}
} /* AddKey */

/* ----- WM_CHAR message handler ----- */
static void
WindowChar(char Ch)
{
	if (_CheckBreak  && (Ch == 3))  {
		WWriteBuf(TEXT("^C\n"),2);
		BreakActive = TRUE;
	}

	if (_WinTerminated) {
		if (Ch == 27) PostQuitMessage(0);
	} else
		AddKey(Ch);
} /* WindowChar */

/* ------ WM_xSCROLL message ------ */
static void
WindowScroll(int Action, int Thumb)
{
	int	Y;

	switch (Action) {
		case SB_LINEUP:
			Y = _Origin.y-1;
			break;
		case SB_LINEDOWN:
			Y = _Origin.y+1;
			break;
		case SB_PAGEUP:
			Y = _Origin.y - ClientSize.y/2;
			break;
		case SB_PAGEDOWN:
			Y = _Origin.y + ClientSize.y/2;
			break;
		case SB_TOP:
			Y = 0;
			break;
		case SB_BOTTOM:
			Y = Range.y;
			break;
		case SB_THUMBPOSITION:
			Y = Thumb;
			break;
		default:
			Y = _Origin.y;
			break;
	}
	_HideCursor();
	_ScrollTo(_Origin.x,Y);
	_ShowCursor();
} /* WindowScroll */

/* ------ WindowIO window procedure ------ */
LRESULT CALLBACK
_WinIOProc(HWND Window, UINT Message, WPARAM WParam, LONG LParam)
{
	switch (Message) {
		case WM_CREATE:
			ScreenBuffer = NULL;
			break;
		case WM_PAINT:
			WindowPaint();
			break;
		case WM_SIZE:
			WindowResize();
			break;
		case WM_VSCROLL:
			WindowScroll(LOWORD(WParam),HIWORD(WParam));
			break;
		case WM_CHAR:
			WindowChar((char)WParam);
			break;
		case WM_COMMAND:
			switch (WParam) {
				case PM_COPYALL:
					markBegin.x = 0;
					markEnd.x = _ScreenSize.x-1;
					markBegin.y = 0;
					markEnd.y = _ScreenSize.y-1;
					Copy2Clipboard();
					break;
				case PM_PASTE:
					if (!OpenClipboard(Window)) break;
					ClpKeyBuffer =
						GetClipboardDataAlloc(CF_UNICODETEXT);
					CloseClipboard();
					ClpKeyCount = wcslen(ClpKeyBuffer);
					break;
				case PM_CLEAR:
					_HideCursor();
					WClrscr();
					_ShowCursor();
					break;
				case PM_TRACK:
					_HideCursor();
					WTrackCursor();
					_ShowCursor();
					break;
				case PM_ABOUT:
					{
						TCHAR	AboutMsg[256];
						swprintf(AboutMsg,
							TEXT("Program: %s (%s)\nby V. Vlachoudis\n1999"),
							ModuleName,PRGNAME);

						MessageBox(Window,
							AboutMsg,
							TEXT("About"),0);
					}
					break;
				default:
					return DefWindowProc(Window, Message, WParam, LParam);
			}
			break;
		case WM_LBUTTONDOWN: 
			if (GetAsyncKeyState(VK_MENU)) {
				HMENU hMenu = CreatePopupMenu();
				AppendMenu(hMenu,MF_ENABLED,PM_COPYALL,TEXT("Copy &All"));
				AppendMenu(hMenu,MF_ENABLED,PM_PASTE,TEXT("&Paste"));
				AppendMenu(hMenu,MF_ENABLED,PM_CLEAR,TEXT("&Clear"));
				AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
				AppendMenu(hMenu,MF_ENABLED,PM_TRACK,TEXT("&Track Cursor"));
				AppendMenu(hMenu,MF_SEPARATOR,0,NULL);
				AppendMenu(hMenu,MF_ENABLED,PM_ABOUT,TEXT("About"));
				TrackPopupMenu(hMenu,0,
					LOWORD(LParam),HIWORD(LParam),
					0,Window,NULL);
				DestroyMenu(hMenu);
			} else {
				InitDeviceContext();
				markBegin.x = markEnd.x = LOWORD(LParam)/CharSize.x + _Origin.x;
				markBegin.y = markEnd.y = HIWORD(LParam)/CharSize.y + _Origin.y;
				marking = TRUE;
			}
			break;
		case WM_LBUTTONUP:     
			if (marking) {
				marking = FALSE;
				DrawMarkedArea();
				DoneDeviceContext();
				Copy2Clipboard();
			}
			break;
		case WM_MOUSEMOVE:      
			if (marking) {
				/* Draw old area to erase it */
				DrawMarkedArea();              
				
				/* Update the position */
				markEnd.x = LOWORD(LParam)/CharSize.x + _Origin.x;			
				markEnd.y = HIWORD(LParam)/CharSize.y + _Origin.y;

				/* Draw the new marked area */
				DrawMarkedArea();
			}
			break;
		//case WM_LBUTTONDBLCLK:           
			/* A little tricky find one word to mark */
			//break;
		case WM_KEYDOWN:
			if (IN_RANGE(33,(BYTE)WParam,46)) {
				AddKey(0);
				AddKey((BYTE)WParam);
			}
			break;
		case WM_SETFOCUS:
			Focused = TRUE;
			InvalidateRect(Window, NULL, TRUE);
			UpdateWindow(Window);
			if (Reading)
				_ShowCursor();
			break;
		case WM_KILLFOCUS:
			if (Reading)
				_HideCursor();
			Focused = FALSE;
			break;
		/*** WM_QUIT, needs special handling, DO NOT UNCOMMENT */
		/*** case WM_QUIT: ***/
		/***	break; ***/
		case WM_CLOSE:
			DefWindowProc(Window, Message, WParam, LParam);
			ExitThread(0);
			break;
		case WM_DESTROY:
			_WinTerminated = TRUE;
			free(ScreenBuffer);
			DeleteObject((HGDIOBJ)_hFont);
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(Window, Message, WParam, LParam);
	}
	return FALSE;
} /* _WinIOProc */

/* ----- Create window if required ----- */
void
WInitWinIO(HINSTANCE hInst, HINSTANCE hPrev, int cmdShow)
{
	WNDCLASS	CrtClass = {
		CS_HREDRAW | CS_VREDRAW, _WinIOProc, 0, 0, 0, 0, 0, 0,
		NULL, PRGNAME
	};
	if (hPrev == 0) {
		CrtClass.hInstance = hInst;
		CrtClass.hbrBackground = GetStockObject(WHITE_BRUSH);
		RegisterClass(&CrtClass);
	}
	_CrtInstance = hInst;

	_CrtWindow = CreateWindow(
			CrtClass.lpszClassName,
			CrtClass.lpszClassName,
			WS_CAPTION | WS_SYSMENU | WS_VSCROLL, 
			CW_USEDEFAULT, CW_USEDEFAULT,
			CW_USEDEFAULT, CW_USEDEFAULT,
			0,
			0,
			hInst,
			NULL);

	GetModuleFileName(hInst, ModuleName, sizeof(ModuleName)/sizeof(TCHAR));

	ShowWindow(_CrtWindow, cmdShow);
#if defined(WCE)
	InvalidateRect(_CrtWindow, NULL, TRUE);
#endif
	UpdateWindow(_CrtWindow);
} /* WInitWinIO */

/* ------ WinIO unit exit procedure ----- */
void
WExitWinIO(void)
{
	MSG	Message;
	TCHAR	Title[128], OldTitle[128];

	if (_CrtWindow) {
		GetWindowText(_CrtWindow, OldTitle, sizeof(OldTitle)/sizeof(TCHAR));
		wsprintf(Title, TEXT("[ %s ]"), OldTitle);
		SetWindowText(_CrtWindow, Title);
		_CheckBreak = FALSE;
		SignalBreak = NULL;
		BreakActive = FALSE;
		_WinTerminated = TRUE;
		while (GetMessage(&Message, 0, 0, 0)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}
	}
} /* WExitWinIO */
