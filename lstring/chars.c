/*
 * $Id: chars.c,v 1.4 2002/06/11 12:37:15 bnv Exp $
 * $Log: chars.c,v $
 * Revision 1.4  2002/06/11 12:37:15  bnv
 * Added: CDECL
 *
 * Revision 1.3  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.2  1999/11/26 09:53:28  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <lstring.h>

/* ---------------- Lchars ------------------- */
long __CDECL
Lchars( FILEP f )
{
#if defined(WCE) && !defined(__BORLANDC__)
	return GetFileSize(f->handle,NULL) - FTELL(f);
#else
	long	l,chs;

	l = FTELL(f);		/* read current position */
	FSEEK(f,0L,SEEK_END);	/* seek at the end */
	chs = FTELL(f) - l ;
	FSEEK(f,l,SEEK_SET);
	return chs;
#endif
} /* Lchars */
