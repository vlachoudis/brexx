/*
 * $Id: bstr.h,v 1.3 2002/06/11 12:37:56 bnv Exp $
 * $Log: bstr.h,v $
 * Revision 1.3  2002/06/11 12:37:56  bnv
 * Added: CDECL
 *
 * Revision 1.2  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.1  1999/11/29 14:58:00  bnv
 * Initial revision
 *
 *
 * This file provides a substitute for the common strxxx commands
 * that unfortunatelly do not exist on the Windows CE of Visual C++.
 * They exist only in a UNICODE format.
 */

#ifndef __BSTR_H__
#define __BSTR_H__

/* -------------- Replacement for ctype.h ---------------- */
#define _BIS_SPA 0x01	/* is space */
#define _BIS_DIG 0x02	/* is digit indicator */
#define _BIS_UPP 0x04	/* is upper case */
#define _BIS_LOW 0x08	/* is lower case */
#define _BIS_HEX 0x10	/* [0..9] or [A-F] or [a-f] */
#define _BIS_CTL 0x20	/* Control */
#define _BIS_PUN 0x40	/* punctuation */

extern  char	_Bctype[];    /* Character type array */

#define Bisalnum(c)  (_Bctype[(c) + 1] & (_BIS_DIG | _BIS_UPP | _BIS_LOW))
#define Bisalpha(c)  (_Bctype[(c) + 1] & (_BIS_UPP | _BIS_LOW))
#define Bisascii(c)  ((unsigned)(c) < 128)
#define Biscntrl(c)  (_Bctype[(c) + 1] & _BIS_CTL)
#define Bisdigit(c)  (_Bctype[(c) + 1] & _BIS_DIG)
#define Bisgraph(c)  ((c) >= 0x21 && (c) <= 0x7e)
#define Bislower(c)  (_Bctype[(c) + 1] & _BIS_LOW)
#define Bisprint(c)  ((c) >= 0x20 && (c) <= 0x7e)
#define Bispunct(c)  (_Bctype[(c) + 1] & _BIS_PUN)
#define Bisspace(c)  (_Bctype[(c) + 1] & _BIS_SPA)
#define Bisupper(c)  (_Bctype[(c) + 1] & _BIS_UPP)
#define Bisxdigit(c) (_Bctype[(c) + 1] & (_BIS_DIG | _BIS_HEX))

#define Btoascii(c)  ((c) & 0x7f)

#define Btoupper(c) ((c) + 'A' - 'a')
#define Btolower(c) ((c) + 'a' - 'A')

/* ------------- Replacement for string.h --------------- */
char*		__CDECL Bstrcpy(char *dest, const char *src);
int		__CDECL Bstrcmp(const char *s1, const char *s2);
char*		__CDECL Bstrcat(char *dest, const char *src);
const char*	__CDECL Bstrchr(const char *s, int c);
unsigned	__CDECL Bstrlen(const char *s);
char*		__CDECL Bl2a(	char *s, const long num,
			int length, const int radix);
/* char*		Bltoa(long num, char *s); */
#endif
