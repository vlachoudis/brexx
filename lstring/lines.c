/*
 * $Id: lines.c,v 1.3 2001/06/25 18:49:48 bnv Exp $
 * $Log: lines.c,v $
 * Revision 1.3  2001/06/25 18:49:48  bnv
 * Header changed to Id
 *
 * Revision 1.2  1999/11/26 12:52:25  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#include <lstring.h>

/* ---------------- Llines ------------------- */
long
Llines( FILEP f )
{
	long	pos,l;
	int	ch,prev;

	pos = FTELL(f);		/* read current position */
	l = 0;
	prev = ' ';
	while (1) {
		ch = FGETC(f);
		if (ch==-1) {
			if (prev!='\n') l++;
			break;
		}
		if (ch=='\n') l++;
		prev = ch;
	}
	FSEEK(f,pos,SEEK_SET);
	return l;
} /* Llines */
