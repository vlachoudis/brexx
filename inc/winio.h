/*
 * $Id: winio.h,v 1.3 2002/06/11 12:37:56 bnv Exp $
 * $Log: winio.h,v $
 * Revision 1.3  2002/06/11 12:37:56  bnv
 * Added: CDECL
 *
 * Revision 1.2  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.1  1999/11/29 14:58:00  bnv
 * Initial revision
 *
 */

#ifndef __WINIO_H__
#define __WINIO_H__

#ifndef	SIG_IGN
#	define	SIG_IGN		NULL
#	define	SIGINT		0
#endif

void	__CDECL WSetTitle(const char *title);
void	__CDECL WGotoXY(int X, int Y);
int	__CDECL WWhereX(void);
int	__CDECL WWhereY(void);
void	__CDECL WClrscr(void);
void	__CDECL WClreol(void);

void	__CDECL WSignal(int sig, void (__CDECL *func)(int));

void	__CDECL WWriteBuf(LPTSTR Buffer, WORD Count);
void	__CDECL WWriteChar(TCHAR Ch);
BOOL	__CDECL WKeyPressed(void);
int	__CDECL WReadKey(void);
WORD	__CDECL WReadBuf(char *Buffer, WORD Count);

void	__CDECL WInitWinIO(HINSTANCE,HINSTANCE,int);
void	__CDECL WExitWinIO(void);

#endif
