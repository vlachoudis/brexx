/*
 * $Header: /home/bnv/tmp/brexx/lstring/RCS/lstring.c,v 1.2 1999/05/14 13:11:47 bnv Exp $
 * $Log: lstring.c,v $
 * Revision 1.2  1999/05/14 13:11:47  bnv
 * Minor changes
 *
 * Revision 1.1  1998/07/02 17:18:00  bnv
 * Initial Version
 *
 */

#define __LSTRING_C__

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lerror.h>
#include <lstring.h>

/* ================= Lstring routines ================== */

/* -------------------- Linit ---------------- */
void
Linit( LerrorFunc Lerr)
{
	size_t	i;

	/* setup error function */
	if (Lerr)
		Lerror = Lerr;
	else
		Lerror = Lstderr;

	/* setup upper */
	for (i=0; i<256; i++)  u2l[i] = l2u[i] = i;
	for (i=0; clower[i]; i++) {
		l2u[ (byte)clower[i] & 0xFF ] = cUPPER [i];
		u2l[ (byte)cUPPER[i] & 0xFF ] = clower [i];
	}

	/* setup time */
	_Ltimeinit();
} /* Linit */

/* -------------- _Lfree ------------------- */
void
_Lfree(void *str)
{
	LPFREE((PLstr)str);
} /* _Lfree */

/* ---------------- Lfx -------------------- */
void
Lfx( const PLstr s, const size_t len )
{
	size_t	max;

	if (LISNULL(*s)) {
		LSTR(*s) = (char *) MALLOC( (max = LNORMALISE(len))+LEXTRA, "Lstr" );
		LLEN(*s) = 0;
		LMAXLEN(*s) = max;
		LTYPE(*s) = LSTRING_TY;
		LOPT(*s) = 0;
	} else
	if (!LOPTION(*s,LOPTFIX) && LMAXLEN(*s)<len) {
		LSTR(*s) = (char *) REALLOC( LSTR(*s), (max=LNORMALISE(len))+LEXTRA);
		LMAXLEN(*s) = max;
	}
} /* Lfx */

/* ---------------- Licpy ------------------ */
void
Licpy( const PLstr to, const long from )
{
	LLEN(*to)  = sizeof(long);
	LTYPE(*to) = LINTEGER_TY;
	LINT(*to)  = from;
} /* Licpy */

/* ---------------- Lrcpy ------------------ */
void
Lrcpy( const PLstr to, const double from )
{
	LLEN(*to)  = sizeof(double);
	LTYPE(*to) = LREAL_TY;
	LREAL(*to) = from;
} /* Lrcpy */

/* ---------------- Lscpy ------------------ */
void
Lscpy( const PLstr to, const char *from )
{
	size_t	len;

	if (!from) {
		Lfx(to,len=0);
		LLEN(*to) = 0;
	} else {
		Lfx(to,len = STRLEN(from));
		MEMCPY( LSTR(*to), from, len );
	}
	LLEN(*to) = len;
	LTYPE(*to) = LSTRING_TY;
} /* Lscpy */

/* ---------------- Lcat ------------------- */
void
Lcat( const PLstr to, const char *from )
{
	size_t	l;

	if (from==NULL) return;

	if (LLEN(*to)==0)
		Lscpy( to, from );
	else {
		L2STR(to);
		l=LLEN(*to) + STRLEN(from);
		if (LMAXLEN(*to)<l) Lfx(to,l);
		STRCPY( LSTR(*to) + LLEN(*to), from );
		LLEN(*to) = l;
	}
} /* Lcat */

/* ------------------ Lcmp ------------------- */
int
Lcmp( const PLstr a, const char *b )
{
	int	r,blen;

	L2STR(a);

	blen = STRLEN(b);
	if ( (r=MEMCMP( LSTR(*a), b, MIN(LLEN(*a),blen)))!=0 )
		return r;
	else {
		if (LLEN(*a) > blen)
			return 1;
		else
		if (LLEN(*a) == blen)
			return 0;
		else
			return -1;
	}
} /* Lcmp */

/* ---------------- Lstrcpy ----------------- */
void
Lstrcpy( const PLstr to, const PLstr from )
{
	if (LLEN(*from)==0) {
		LLEN(*to) = 0;
		LTYPE(*to) = LSTRING_TY;
	} else {
		if (LMAXLEN(*to)<=LLEN(*from)) Lfx(to,LLEN(*from));
		switch ( LTYPE(*from) ) {
			case LSTRING_TY:
				MEMCPY( LSTR(*to), LSTR(*from), LLEN(*from) );
				break;

			case LINTEGER_TY:
				LINT(*to) = LINT(*from);
				break;

			case LREAL_TY:
				LREAL(*to) = LREAL(*from);
				break;
		}
		LTYPE(*to) = LTYPE(*from);
		LLEN(*to) = LLEN(*from);
	} 
} /* Lstrcpy */

/* ----------------- Lstrcat ------------------ */
void
Lstrcat( const PLstr to, const PLstr from )
{
	size_t	l;
	if (LLEN(*from)==0) return;

	if (LLEN(*to)==0) {
		Lstrcpy( to, from );
		return;
	}

	L2STR(to);
	L2STR(from);

	l = LLEN(*to)+LLEN(*from);
	if (LMAXLEN(*to) <= l)
		Lfx(to, l);
	MEMCPY( LSTR(*to) + LLEN(*to), LSTR(*from), LLEN(*from) );
	LLEN(*to) = l;
} /* Lstrcat */

/* ----------------- _Lstrcmp ----------------- */
/* -- Low level strcmp, suppose that both of -- */
/* -- are of the same type                      */
int
_Lstrcmp( const PLstr a, const PLstr b )
{
	int	r;

	if ( (r=MEMCMP( LSTR(*a), LSTR(*b), MIN(LLEN(*a),LLEN(*b))))!=0 )
		return r;
	else {
		if (LLEN(*a) > LLEN(*b))
			return 1;
		else
		if (LLEN(*a) == LLEN(*b)) {
			if (LTYPE(*a) > LTYPE(*b))
				return 1;
			else
			if (LTYPE(*a) < LTYPE(*b))
				return -1;
			return 0;
		} else
			return -1;
	}
} /* _Lstrcmp */

/* ----------------- Lstrcmp ------------------ */
int
Lstrcmp( const PLstr a, const PLstr b )
{
	int	r;

	L2STR(a);
	L2STR(b);

	if ( (r=MEMCMP( LSTR(*a), LSTR(*b), MIN(LLEN(*a),LLEN(*b))))!=0 )
		return r;
	else {
		if (LLEN(*a) > LLEN(*b))
			return 1;
		else
		if (LLEN(*a) == LLEN(*b))
			return 0;
		else
			return -1;
	}
}  /* Lstrcmp */

/* ----------------- Lstrset ------------------ */
void
Lstrset( const PLstr to, const size_t length, const char value)
{
	Lfx(to,length);
	LTYPE(*to) = LSTRING_TY;
	LLEN(*to) = length;
	MEMSET(LSTR(*to),value,length);
}  /* Lstrset */

/* ----------------- _Lsubstr ----------------- */
/* WARNING!!! length is size_t type DO NOT PASS A NEGATIVE value */
void
_Lsubstr( const PLstr to, const PLstr from, size_t start, size_t length )
{
	L2STR(from);

	start--;
	if ((length==0) || (length+start>LLEN(*from)))
		length = LLEN(*from) - start;

	if (start<LLEN(*from)) {
		if (LMAXLEN(*to)<length) Lfx(to,length);
		MEMCPY( LSTR(*to), LSTR(*from)+start, length );
		LLEN(*to) = length;
	} else
		LZEROSTR(*to);
	LTYPE(*to) = LSTRING_TY;
}  /* Lstrsub */

/* ------------------------ _Lisnum ----------------------- */
/* _Lisnum      - returns if it is possible to convert      */
/*               a LSTRING to NUMBER                        */
/*               and a LREAL to LINTEGER                    */
/* There is one possibility that is missing...              */
/* that is to hold an integer number as a real in a string. */
/* ie.   '  2.0 '  this should be LINTEGER not LREAL        */
/* -------------------------------------------------------- */
int
_Lisnum( const PLstr s )
{
	bool	F, R;
	register char	*ch;

/* ---
	if (LOPT(*s) & (LOPTINT | LOPTREAL)) {
		if (LOPT(*s) & LOPTINT)
			return LINTEGER_TY;
		else
			return LREAL_TY;
	}
--- */

	ch = LSTR(*s);
	if (ch==NULL) return LSTRING_TY;
	LASCIIZ(*s);	/*	///// Remember to erase LASCIIZ
				///// before all the calls to Lisnum */

	/* skip leading spaces */
	while (isspace(*ch)) ch++;

	/* accept one sign */
	if (*ch=='-' || *ch=='+') ch++;

	/* skip following spaces after sign */
	while (isspace(*ch)) ch++;

	/* accept many digits */
	R = FALSE;
	if (IN_RANGE('0',*ch,'9')) {
		ch++;
		F = TRUE;
		while (IN_RANGE('0',*ch,'9')) ch++;
		if (!*ch) goto isnumber;
	} else
		F = FALSE;

	/* accept one dot */
	if (*ch=='.') {
		R = TRUE;
		ch++;

		/* accept many digits */
		if (IN_RANGE('0',*ch,'9')) {
			ch++;
			while (IN_RANGE('0',*ch,'9')) ch++;
		} else
			if (!F) return LSTRING_TY;

		if (!*ch) goto isnumber;
	} else
		if (!F) return LSTRING_TY;


	/* accept on 'e' or 'E' */
	if (*ch=='e' || *ch=='E') {
		ch++;
		R = TRUE;
		/* accept one sign */
		if (*ch=='-' || *ch=='+') ch++;
		/* accept many digits */
		if (IN_RANGE('0',*ch,'9')) {
			ch++;
			while (IN_RANGE('0',*ch,'9')) ch++;
		} else
			return LSTRING_TY;
	}

	/* accept many blanks */
	while (isspace(*ch)) ch++;

	/* is it end of string */
	if (*ch) return LSTRING_TY;

isnumber:
	if (R)
		return LREAL_TY;
	else
		return LINTEGER_TY;
} /* _Lisnum */

/* ------------------ L2str ------------------- */
void
L2str( const PLstr s )
{
	if (LTYPE(*s)==LINTEGER_TY) {
		sprintf(LSTR(*s), "%ld", LINT(*s));
		LLEN(*s) = STRLEN(LSTR(*s));
	} else {	/* LREAL_TY */
		sprintf(LSTR(*s), formatStringToReal, LREAL(*s));
		LLEN(*s) = STRLEN(LSTR(*s));
	}
	LTYPE(*s) = LSTRING_TY;
} /* L2str */

/* ------------------ L2int ------------------- */
void
L2int( const PLstr s )
{
	if (LTYPE(*s)==LREAL_TY) {
		if ((double)((long)LREAL(*s)) == LREAL(*s))
			LINT(*s) = (long)LREAL(*s);
		else
			Lerror(ERR_INVALID_INTEGER,0);
	} else { /* LSTRING_TY */
		LASCIIZ(*s);
		switch (_Lisnum(s)) {
			case LINTEGER_TY:
				LINT(*s) = atol( LSTR(*s) );
				break;

			case LREAL_TY:
				LREAL(*s) = strtod( LSTR(*s), NULL );
				if ((double)((long)LREAL(*s)) == LREAL(*s))
					LINT(*s) = (long)LREAL(*s);
				else
					Lerror(ERR_INVALID_INTEGER,0);
				break;

			default:
				Lerror(ERR_INVALID_INTEGER,0);
		}
	}
	LTYPE(*s) = LINTEGER_TY;
	LLEN(*s) = sizeof(long);
} /* L2int */

/* ------------------ L2real ------------------- */
void
L2real( const PLstr s )
{
	if (LTYPE(*s)==LINTEGER_TY)
		LREAL(*s) = (double)LINT(*s);
	else { /* LSTRING_TY */
		LASCIIZ(*s);
		if (_Lisnum(s)!=LSTRING_TY)
			LREAL(*s) = strtod( LSTR(*s), NULL );
		else
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
	LTYPE(*s) = LREAL_TY;
	LLEN(*s) = sizeof(double);
} /* L2real */

/* ------------------- _L2num -------------------- */
/* this function is used when we know to what type */
/* we should change the string                     */
void
_L2num( const PLstr s, const int type )
{
	LASCIIZ(*s);
	switch (type) {
		case LINTEGER_TY:
			LINT(*s) = atol( LSTR(*s) );
			LTYPE(*s) = LINTEGER_TY;
			LLEN(*s) = sizeof(long);
			break;

		case LREAL_TY:
			LREAL(*s) = strtod( LSTR(*s), NULL );
			if ((double)((long)LREAL(*s)) == LREAL(*s)) {
				LINT(*s) = (long)LREAL(*s);
				LTYPE(*s) = LINTEGER_TY;
				LLEN(*s) = sizeof(long);
			} else {
				LTYPE(*s) = LREAL_TY;
				LLEN(*s) = sizeof(double);
			}
			break;
		default:
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
} /* _L2num */

/* ------------------ L2num ------------------- */
void
L2num( const PLstr s )
{
	LASCIIZ(*s);

	switch (_Lisnum(s)) {
		case LINTEGER_TY:
			LINT(*s) = atol( LSTR(*s) );
			LTYPE(*s) = LINTEGER_TY;
			LLEN(*s) = sizeof(long);
			break;

		case LREAL_TY:
			LREAL(*s) = strtod( LSTR(*s), NULL );
/*
//// Numbers like 2.0 should be treated as real and not as integer
//// because in cases like factorial while give an error result
////			if ((double)((long)LREAL(*s)) == LREAL(*s)) {
////				LINT(*s) = (long)LREAL(*s);
////				LTYPE(*s) = LINTEGER_TY;
////				LLEN(*s) = sizeof(long);
////			} else {
*/
				LTYPE(*s) = LREAL_TY;
				LLEN(*s) = sizeof(double);
/*
////			}
*/
			break;

		default:
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
} /* L2num */

/* ----------------- Lrdint ------------------ */
long
Lrdint( const PLstr s )
{
	double	d;

	if (LTYPE(*s)==LINTEGER_TY) return LINT(*s);

	if (LTYPE(*s)==LREAL_TY) {
		if ((double)((long)LREAL(*s)) == LREAL(*s))
			return (long)LREAL(*s);
		else
			Lerror(ERR_INVALID_INTEGER,0);
	} else { /* LSTRING_TY */
		LASCIIZ(*s);
		switch (_Lisnum(s)) {
			case LINTEGER_TY:
				return atol( LSTR(*s) );

			case LREAL_TY:
				d = strtod( LSTR(*s), NULL );
				if ((double)((long)d) == d)
					return (long)d;
				else
					Lerror(ERR_INVALID_INTEGER,0);
				break;

			default:
				Lerror(ERR_INVALID_INTEGER,0);
		}
	}
	return 0;	/* never gets here but keeps compiler happy */
} /* Lrdint */

/* ----------------- Lrdreal ------------------ */
double
Lrdreal( const PLstr s )
{
	if (LTYPE(*s)==LREAL_TY) return LREAL(*s);

	if (LTYPE(*s)==LINTEGER_TY)
		return (double)LINT(*s);
	else { /* LSTRING_TY */
		LASCIIZ(*s);
		if (_Lisnum(s)!=LSTRING_TY)
			return strtod( LSTR(*s), NULL );
		else
			Lerror(ERR_BAD_ARITHMETIC,0);
	}
	return 0.0;
} /* Lrdreal */
