/*
 * $Id: bio.h,v 1.4 2002/06/06 08:22:34 bnv Exp $
 * $Log: bio.h,v $
 * Revision 1.4  2002/06/06 08:22:34  bnv
 * Corrected: Bfseek
 *
 * Revision 1.3  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.2  1999/11/29 14:57:42  bnv
 * Changed: Defines only
 *
 * Revision 1.1  1999/11/26 08:53:29  bnv
 * Initial revision
 *
 *
 * This file provides substitute routines for the stdio.h
 * For some window systems.
 */

#ifndef __BIO_H__
#define __BIO_H__

#ifndef __WINDOWS_H
#	include <windows.h>
#endif
#ifndef __OS_H__
#	include <os.h>
#endif
#ifndef __LDEFS_H__
#	include <ldefs.h>
#endif
#ifndef __WINIO_H__
#	include <winio.h>
#endif

#define	EOF		(-1)

#ifndef SEEK_SET
#	define	SEEK_SET	(FILE_BEGIN)
#	define	SEEK_CUR	(FILE_CURRENT)
#	define	SEEK_END	(FILE_END)
#endif

#define	BIO_READ	BIT0
#define	BIO_WRITE	BIT1
#define	BIO_APPEND	BIT2
#define BIO_TEXT	BIT3
#define BIO_BINARY	BIT4
#define BIO_UNICODE	BIT5
#define	BIO_EOF		BIT6

typedef struct {
	HANDLE	handle;
	int	mode;
} BFILE;

BFILE	*Bfopen(const char *filename, const char *mode);
int	Bfclose(BFILE *stream);
#ifdef __BORLANDC__
#	define Bfseek(s,o,w)	_llseek(s->handle,o,w)
#endif
int	Bfseek(BFILE *stream, int distance, int method);
#define	Bftell(s)	Bfseek(s,0L,SEEK_CUR)
int	Bfeof(BFILE *stream);
int	Bfflush(BFILE *stream);
void	Bfputs(const char *s, BFILE *stream);
int	Bfgetc(BFILE *stream);
int	Bfputc(char ch, BFILE *stream);

char	Bgetchar(void);
void	Bputs(const char *s);
void	Bputch(char ch);
void	Bputint(long num, int length, int radix);

#endif
