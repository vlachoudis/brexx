/*
 * $Id: bio.c,v 1.7 2004/08/16 15:33:58 bnv Exp $
 * $Log: bio.c,v $
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

#include <bio.h>
#include <bstr.h>
#include <string.h>
#include <stdlib.h>

static	char	buffer[128];
static	int	bufferpos=0;
static	BOOL	newline=FALSE;

/* ----- Bfopen ----- */
BFILE*
Bfopen( const char *filename, const char *mode )
{
#ifndef __BORLANDC__
	TCHAR	path[128];
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
	wch = wcschr(path,_T(':'));
	if (wch != NULL) {
		wch++;
		wcscpy(options,wch);
		*wch = (TCHAR)0;
	} else
		options[0] = (TCHAR)0;

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
		if (path[0]==_T('c') || path[0]==_T('C')) {
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
			swscanf(options,_T("%d,%d,%c,%d,%d"),
				&speed, &length, &parity, &stop, &buffer);
			if (buffer == 0) buffer = 128;
			ready = SetupComm(hnd,buffer,buffer);
			ready = GetCommState(hnd,&dcb);
			dcb.BaudRate = speed;
			dcb.ByteSize = length;
			switch (parity) {
				case _T('E'): case _T('e'):
					dcb.Parity = EVENPARITY;
					break;
				case _T('O'): case _T('o'):
					dcb.Parity = ODDPARITY;
					break;
				case _T('M'): case _T('m'):
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
	f->mode = bitmode;

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
			*ch=_T('\0');
			WWriteBuf(buffer,count);
			ch = buffer;
			count = 0;
		}
	}
	if (count != 0) {
		*ch=_T('\0');
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
