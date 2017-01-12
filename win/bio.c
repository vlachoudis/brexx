/*
 * $Id: bio.c,v 1.9 2017/01/12 11:08:21 bnv Exp $
 * $Log: bio.c,v $
 * Revision 1.9  2017/01/12 11:08:21  bnv
 * Window corrections
 *
 * Revision 1.8  2005/05/20 16:02:03  bnv
 * *** empty log message ***
 *
 * Revision 1.7  2004/08/16 15:33:58  bnv
 * Corrected: Append
 *
 * Revision 1.7  2003/10/30 13:17:47  bnv
 * Cosmetics
 *
 * Revision 1.6  2003/02/26 16:30:00  bnv
 * Export of the CrtWindow
 *
 * Revision 1.5  2002/08/22 12:28:17  bnv
 * Added: port programming support in the open() function
 *
 * Revision 1.4  2002/06/11 12:38:06  bnv
 * Added: CDECL
 *
 * Revision 1.3  2002/01/14 09:14:19  bnv
 * Added: Bfseek function
 *
 * Revision 1.2  2001/06/25 18:52:24  bnv
 * Header -> Id
 *
 * Revision 1.1  1999/09/13 15:06:41  bnv
 * Initial revision
 *
 *
 * This file provides some substitus for the common fxxxx I/O commands
 * that unfortunatelly do not exist on the Windows CE of Visual C++.
 */

#ifdef WCE

#include "bio.h"
#include "bstr.h"
#include <string.h>
#include <stdlib.h>

static	char	buffer[MAX_PATH];
static	int	bufferpos=0;
static	BOOL	newline=FALSE;
static	TCHAR	cwd[MAX_PATH] = TEXT("\\");		// Current working directory

/* ----- Bfopen ----- */
BFILE* __CDECL
Bfopen( const char *filename, const char *mode )
{
#ifndef __BORLANDC__
	TCHAR	path[MAX_PATH];
	TCHAR	buffer[MAX_PATH];
	TCHAR	options[128];
	TCHAR	*wch;
#endif
	BFILE	*f;
	HANDLE	hnd;
	int	bitmode=0;
	int	textmode=1;
	char	*ch;

	/* scan mode string to find options */
	for (ch=(char *)mode; *ch; ch++) {
		switch (*ch) {
			case 'r':
				bitmode |= BIO_READ;
				break;
			case 'w':
				bitmode |= BIO_WRITE;
				break;
			case 'b':
				textmode = 0;
				bitmode |= BIO_BINARY;
				break;
			case 'a':
				bitmode |= (BIO_READ|BIO_WRITE|BIO_APPEND);
				break;
			case '+':
				bitmode |= (BIO_READ|BIO_WRITE);
				break;
			case 'u':
				bitmode |= BIO_UNICODE;
				break;
		}
	}
	if (textmode)
		bitmode |= BIO_TEXT;

#if defined(__BORLANDC__)
	if ((bitmode & (BIO_READ|BIO_WRITE)) == (BIO_READ|BIO_WRITE))
		hnd = _lopen(filename,READ_WRITE);
	else
	if (bitmode & BIO_READ)
		hnd = _lopen(filename,READ);
	else
	if (bitmode & BIO_WRITE)
		hnd = _lopen(filename,WRITE);
	else
		return NULL;
	if ((HFILE)hnd == HFILE_ERROR)
		return NULL;
#else
	mbstowcs(path,filename,STRLEN(filename)+1);

	/* Scan for a special file */
	wch = wcschr(path,TEXT(':'));
	if (wch != NULL) {
		wch++;
		wcscpy(options,wch);
		*wch = (TCHAR)0;
	} else {
		options[0] = (TCHAR)0;
		/* convert to absolute */
		Brel2absdir(buffer,MAX_PATH,path);
		wcscpy(path,buffer);
	}

	if ((bitmode & (BIO_READ|BIO_WRITE)) == (BIO_READ|BIO_WRITE)) {
		hnd = CreateFile( path,
				GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	} else
	if (bitmode & BIO_READ) {
		hnd = CreateFile(path,
				GENERIC_READ, FILE_SHARE_READ, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	} else
	if (bitmode & BIO_WRITE) {
		hnd = CreateFile( path,
				GENERIC_WRITE, FILE_SHARE_READ, NULL,
				OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	} else
		return NULL;

	if (hnd==INVALID_HANDLE_VALUE)
		return NULL;

	if (options[0]) {
		/* Check for serial port */
		if (path[0]==TEXT('c') || path[0]==TEXT('C')) {
			UINT	speed  = 9600;
			UINT	length = 8;
			TCHAR	parity = 'N';
			UINT	stop   = 1;
			UINT	buffer = 0;
			DCB	dcb;
			COMMTIMEOUTS commTimeouts;
			BOOL	ready;

			/* Options format COMx:<speed>,<length>,<parity>,<stop>[,buffer]
			 * length = 300, 600, 1200, 2400, ... 115200
			 * length = 7 | 8
			 * parity = N, P, E
			 * stop   = 1, 2
			 */
			swscanf(options,TEXT("%d,%d,%c,%d,%d"),
				&speed, &length, &parity, &stop, &buffer);
			if (buffer == 0) buffer = 128;
			ready = SetupComm(hnd,buffer,buffer);
			ready = GetCommState(hnd,&dcb);
			dcb.BaudRate = speed;
			dcb.ByteSize = length;
			switch (parity) {
				case TEXT('E'): case TEXT('e'):
					dcb.Parity = EVENPARITY;
					break;
				case TEXT('O'): case TEXT('o'):
					dcb.Parity = ODDPARITY;
					break;
				case TEXT('M'): case TEXT('m'):
					dcb.Parity = MARKPARITY;
					break;
				default:
					dcb.Parity = NOPARITY;
			}
			dcb.StopBits = (stop==1)? ONESTOPBIT : TWOSTOPBITS;
			ready = SetCommState(hnd,&dcb);

			ready = GetCommTimeouts(hnd, &commTimeouts);
			commTimeouts.ReadIntervalTimeout = 50;
			commTimeouts.ReadTotalTimeoutConstant = 50;
			commTimeouts.ReadTotalTimeoutMultiplier = 10;
			commTimeouts.WriteTotalTimeoutConstant = 50;
			commTimeouts.WriteTotalTimeoutMultiplier = 10;
			ready = SetCommTimeouts(hnd, &commTimeouts);
		} else {
			/* Unknown type skip options */
		}
	}
#endif
	f = (BFILE*)malloc(sizeof(BFILE));
	f->handle = hnd;
	f->mode   = bitmode;

	if (bitmode & BIO_APPEND)
		Bfseek(f,0,SEEK_END);
	return f;
} /* Bfopen */

/* ----- Bfclose ----- */
int __CDECL
Bfclose( BFILE *stream )
{
	if (!stream) return EOF;
#if defined(__BORLANDC__)
	_lclose(stream->handle);
#else
	CloseHandle(stream->handle);
#endif
	free(stream);
	return 0;
} /* Bfclose */

/* ----- Bfflush ----- */
int __CDECL
Bfseek(BFILE *stream, int distance, int method)
{
	if (!stream) return 0;
	stream->mode &= ~BIO_EOF;
	return SetFilePointer(stream->handle,distance,0,method);
} /* Bfseek */

/* ----- Bfflush ----- */
int __CDECL
Bfflush(BFILE *stream)
{
	if (!stream) return EOF;
	return SetEndOfFile(stream->handle);
} /* Bfflush */

/* ----- Bfeof ----- */
int __CDECL
Bfeof(BFILE *stream)
{
	if (stream)
		return (stream->mode & BIO_EOF);
	else
		return 0;
} /* Bfeof */

/* ----- Bfgetc ----- */
int __CDECL
Bfgetc( BFILE *stream )
{
	char	c;
#if !defined(__BORLANDC__)
	int	len;
#endif

	if (stream==NULL)
		return Bgetchar();

#if defined(__BORLANDC__)
	if (_lread(stream->handle, &c, 1)==0) {
		/* mark eof */
		stream->mode |= BIO_EOF;
		return EOF;
	}
#else
	ReadFile(stream->handle, &c, 1, &len, NULL);
#endif
	if ((c==0x0D) && (stream->mode & BIO_TEXT)) {
#if defined(__BORLANDC__)
		if (_lread(stream->handle, &c, 1)==0) {
			/* mark eof */
			stream->mode |= BIO_EOF;
			return EOF;
		}
#else
		ReadFile(stream->handle, &c, 1, &len, NULL);
#endif
	}
#if !defined(__BORLANDC__)
	if (len==0) {
		/* mark eof */
		stream->mode |= BIO_EOF;
		return EOF;
	}
#endif
	return (BYTE)c;
} /* Bfgetc */

/* ----- Bputc ----- */
int __CDECL
Bfputc( char ch, BFILE *stream )
{
#if !defined(__BORLANDC__)
	int	written;
#endif
	TCHAR	cr;

	if (stream==NULL) {
		PUTCHAR(ch);
		return ch;
	}

	if (ch==0x0A && (stream->mode & BIO_TEXT)) {
		cr = TEXT('\015');	// 0x0D
#if defined(__BORLANDC__)
		_lwrite(stream->handle, &cr, 1);
	}
	_lwrite(stream->handle, &ch, 1);
#else
		WriteFile(stream->handle, &cr, 1, &written, NULL);
	}
	WriteFile(stream->handle, &ch, 1, &written, NULL);
#endif
	return ch;
} /* Bfgetc */

/* ------ Bfputs -------- */
void __CDECL
Bfputs( const char *s, BFILE *stream )
{
	while (*s)
		Bfputc(*s++,stream);
} /* Bfputs */

/* ------ Read a Character -------- */
char __CDECL
Bgetchar()
{
	char	c;

	/* return a new character only a new line is found in the buffer */
	while (!newline) {
		c = WReadKey();
		switch (c) {
			case 0:
				WReadKey();
				continue;

			case 8:
				if (bufferpos) {
					bufferpos--;
					Bputch(c);
				}
				continue;

			case 13:
			case 10:
				newline = TRUE;
				c = '\n';
				break;
			case 27:
				// Erase contents of buffer
				WGotoXY(WWhereX()-bufferpos,WWhereY());
				bufferpos = 0;
				WClreol();
				continue;
		}
		if (bufferpos<sizeof(buffer)) {
			Bputch(c);
			buffer[bufferpos++] = c;
		} else
			MessageBeep(0);
	}
	c = buffer[0];
	if (c=='\n') newline = FALSE;
	bufferpos--;
	memmove(buffer, buffer+1, bufferpos);
	return c;
} /* Bgetchar */

/* ------ Write an ASCII char ----- */
void __CDECL
Bputch( char ch )
{
	TCHAR	tch;
	tch = (TCHAR)ch & 0xFF;
	WWriteBuf(&tch, 1);
} /* Bputch */

/* ------ Write an ASCII string ----- */
// Wanring: DIFFERENT behavior from puts(). It doesn't append a new line
void __CDECL
Bputs( const char *str )
{
#define BUFFER_LEN	132
	TCHAR	buffer[BUFFER_LEN];
	LPTSTR	ch;
	int	count;

	ch = buffer;
	count = 0;
	while (*str) {
		*ch++ = *str++;
		if (++count == BUFFER_LEN-1) {
			*ch=TEXT('\0');
			WWriteBuf(buffer,count);
			ch = buffer;
			count = 0;
		}
	}
	if (count != 0) {
		*ch=TEXT('\0');
		WWriteBuf(buffer,count);
	}
} /* Bputs */

/* ------ Write a number ----- */
void __CDECL
Bputint( long num, int length, int radix )
{
	char	str[10];
	Bputs(Bl2a(str,num,length,radix));
} /* Bputint */

/* --- Brel2absdir --- */
/* it doesn't make any check if the directory exists */
void __CDECL
Brel2absdir(LPTSTR buffer, int maxlen, LPCTSTR reldir)
{
	TCHAR	workdir[MAX_PATH];
	BOOL	eos = FALSE;
	int	buflen = 1;
	TCHAR	*start = workdir;
	TCHAR	*stop;

#ifdef _DEBUG
	OutputDebugString(L"RELDIR=\"");
	OutputDebugString(reldir);
	OutputDebugString(L"\"\n");
#endif
	/* if the first char is \ then we have an semi-abs reference */
	if (reldir[0]==L'\\')
		_tcscpy(workdir,reldir);
	else {
		/* if not add the current directory */
		_tcscpy(workdir,cwd);
		_tcscat(workdir,L"\\");
		_tcscat(workdir,reldir);
	}
#ifdef _DEBUG
	OutputDebugString(L"WORKDIR=\"");
	OutputDebugString(workdir);
	OutputDebugString(L"\"\n");
#endif

	/* start parsing the working directory */
	buffer[0] = L'\\';

#ifdef _DEBUG
	buffer[buflen]=0;
	OutputDebugString(L"BUFFER=\"");
	OutputDebugString(buffer);
	OutputDebugString(L"\"\n");
#endif
	while (!eos && start[1]) {
		stop = ++start;

		/* search for the next backslash */
		while (*stop && *stop!=L'\\')
			stop++;
		eos = (*stop == 0);
		*stop = 0;
#ifdef _DEBUG
		OutputDebugString(L"dir=\"");
		OutputDebugString(start);
		OutputDebugString(L"\"\n");
#endif

		if (*start==0 || !_tcscmp(start,L".")) {
			/* do nothing this is the current dir */
		} else
		if (!_tcscmp(start,L"..")) {
			/* go back one directory */
			/* search backwords for the last \ */
			if (buflen>1) {
				buflen--;
				while (buflen>1 && buffer[buflen-1]!=L'\\')
					buflen--;
			}
		} else {
			/* enter this directory */
			_tcscpy(buffer+buflen,start);
			buflen += _tcslen(start);
			buffer[buflen++] = L'\\';
#ifdef _DEBUG
			buffer[buflen]=0;
			OutputDebugString(L"BUFFER=\"");
			OutputDebugString(buffer);
			OutputDebugString(L"\"\n");
#endif
		}

		if (buflen >= maxlen)
			break;
		start = stop;
	}
	if (buflen>1)
		buffer[buflen-1] = 0;
	else
		buffer[buflen] = 0;
#ifdef _DEBUG
	OutputDebugString(L"BUFFER=\"");
	OutputDebugString(buffer);
	OutputDebugString(L"\"\n");
#endif
} /* Brel2absdir */

/* --- getcwd --- */
char* __CDECL
Bgetcwd(char *buffer, int maxlen)
{
	int len = _tcslen(cwd);
	if (len>=maxlen) len=maxlen-1;
	wcstombs(buffer,cwd,len);
	buffer[len] = 0;
	return buffer;
} /* getcwd */

/* --- chdir --- */
int __CDECL
Bchdir(char *newdir)
{
	int	len;
	TCHAR	newdirW[MAX_PATH];
	TCHAR	workdir[MAX_PATH];

	mbstowcs(newdirW,newdir,STRLEN(newdir)+1);

	/* find the relative path if any */
	Brel2absdir(workdir,MAX_PATH,newdirW);

	/* verify if the directory exists */
	if (GetFileAttributes(workdir)!=FILE_ATTRIBUTE_DIRECTORY)
		return 1;

	/* copy the directory */
	_tcsncpy(cwd,workdir,MAX_PATH);

	/* strip the ending backslash \ if any */
	len = _tcslen(cwd)-1;
	if (len>1 && cwd[len] == L'\\')
		cwd[len] = 0;
	return 0;
} /* chdir */

#endif
