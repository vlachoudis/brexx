/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/chars.c,v 1.1 1998/07/02 17:17:00 bnv Exp $
 * $Log: chars.c,v $
 * Revision 1.1  1998/07/02 17:17:00  bnv
 * Initial revision
 *
 */

#include <stdio.h>
#include <lstring.h>

/* ---------------- Lchars ------------------- */
long
Lchars( FILE *f )
{
	long	l,chs;

	l = ftell(f);		/* read current position */
	fseek(f,0L,SEEK_END);	/* seek at the end */
	chs = ftell(f) - l ;
	fseek(f,l,SEEK_SET);
	return chs;
} /* Lchars */
