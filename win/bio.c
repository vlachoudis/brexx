/*
 * $Header: /home/bnv/tmp/brexx/win/RCS/bio.c,v 1.1 1999/09/13 15:06:41 bnv Exp $
 * $Log: bio.c,v $
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
#endif
	BFILE	*f;
	HANDLE	hnd;
	int	bitmode=0;
	int	textmode=1;
	char	*ch;

	/* scan mode string to find options */
	for (ch=(const char *)mode; *ch; ch++) {
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
			case '+':
				bitmode |= BIO_APPEND;
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
#endif
	f = (BFILE*)malloc(sizeof(BFILE));
	f->handle = hnd;
	f->mode = bitmode;
	return f;
} /* Bfopen */

/* ----- Bfclose ----- */
int
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
int
Bfflush(BFILE *stream)
{
	if (!stream) return EOF;
	return SetEndOfFile(stream->handle);
} /* Bfflush */

/* ----- Bfeof ----- */
int
Bfeof(BFILE *stream)
{
	return (stream->mode & BIO_EOF);
} /* Bfeof */

/* ----- Bfgetc ----- */
int
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
int
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
void
Bfputs( const char *s, BFILE *stream )
{
	while (*s)
		Bfputc(*s++,stream);
} /* Bfputs */

/* ------ Read a Character -------- */
char
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
void
Bputch( char ch )
{
	TCHAR	tch;
	tch = (TCHAR)ch & 0xFF;
	WWriteBuf(&tch, 1);
} /* Bputch */

/* ------ Write an ASCII string ----- */
// Wanring: DIFFERENT behavior from puts(). It doesn't append a new line
void
Bputs( const char *str )
{
	while (*str)
		Bputch(*str++);
} /* Bputs */

/* ------ Write a number ----- */
void
Bputint( long num, int length, int radix )
{
	char	str[10];
	Bputs(Bl2a(str,num,length,radix));
} /* Bputint */
