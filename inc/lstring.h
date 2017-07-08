/*
 * $Id: lstring.h,v 1.20 2011/06/29 08:32:12 bnv Exp $
 * $Log: lstring.h,v $
 * Revision 1.20  2011/06/29 08:32:12  bnv
 * Added android
 *
 * Revision 1.19  2011/06/20 08:32:49  bnv
 * Added android and global SMALL number
 *
 * Revision 1.18  2011/05/17 06:53:00  bnv
 * Added sqlite
 *
 * Revision 1.17  2009/06/02 09:41:43  bnv
 * MVS/CMS corrections
 *
 * Revision 1.16  2008/07/15 14:57:11  bnv
 * Added include to lmvs.h
 *
 * Revision 1.15  2008/07/15 07:40:07  bnv
 * MVS, CMS support
 *
 * Revision 1.14  2008/07/14 13:09:21  bnv
 * MVS,CMS support
 *
 * Revision 1.13  2006/01/26 10:29:39  bnv
 * Corrected windows CE support
 *
 * Revision 1.12  2004/08/16 15:32:27  bnv
 * PACKAGE
 *
 * Revision 1.11  2004/03/27 08:35:51  bnv
 * Changed: Max Numeric digits for numbers on unix
 *
 * Revision 1.10  2003/02/26 16:31:22  bnv
 * Added: LMKCONST
 *
 * Revision 1.9  2003/02/12 16:38:39  bnv
 * Added: Lhashvalue
 *
 * Revision 1.8  2002/06/11 12:37:56  bnv
 * Added: CDECL
 *
 * Revision 1.7  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.6  2000/12/15 16:44:02  bnv
 * Changed: LMAXNUMERICDIGITS from 15 to 14. To avoid overflow
 *
 * Revision 1.5  1999/11/29 14:58:00  bnv
 * Changed: Some defines
 *
 * Revision 1.4  1999/05/14 13:06:01  bnv
 * Minor corrections
 *
 * Revision 1.3  1999/03/10 16:57:31  bnv
 * Changed: from __GREEK__ to GREEK
 *
 * Revision 1.2  1998/09/09 08:44:59  bnv
 * Removed somed "const" in function calls
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __LSTRING_H__
#define __LSTRING_H__

#include "ldefs.h"
#include "os.h"

#if defined(WIN)
#	include <windows.h>
#	include "winio.h"
#	include	"bio.h"
//#	ifndef PACKAGE_STRING
//#		define PACKAGE_NAME "brexx"
//#		define PACKAGE_STRING PACKAGE_NAME " V2.1"
//#	endif
#else
#	if !defined(__CMS__) && !defined(__MVS__)
#		include <wchar.h>
#	endif
#endif

#ifdef WCE
#	include "bstr.h"
#else
#	include <ctype.h>
#	include <stdio.h>
#endif

#if defined(__CMS__) || defined(__MVS__)
#	include "lmvs.h"
#endif

#include "bmem.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define SMALL	1E-20

/* --- Lstring types --- */
enum	TYPES	{ LSTRING_TY,
		LINTEGER_TY,
		LREAL_TY };

#ifdef WCE
	typedef void	 (*LerrorFunc)(const int,const int,...);
#else
	typedef void	__CDECL (*LerrorFunc)(const int,const int,...);
#endif

/* ------------------------- */
/* --- Lstring structure --- */
/* ------------------------- */
typedef struct Lstr_st {
	unsigned char	*pstr;	/* String (Data) pointer		*/
	size_t	len;		/* Actual length of string(data)	*/
	size_t	maxlen;		/* Maximum length allocated for string	*/
	short	type;		/* Type of data STRING, INT, REAL	*/
#ifdef USEOPTION
	short   options;	/* Bit-options				*/
#endif
} Lstr;

typedef Lstr	*PLstr;

/* ---------------- Macros ------------------- */
/* --- lstring data --- */
#define LSTR(L)		((L).pstr)
#define LINT(L)		(*(long*)((L).pstr))
#define LREAL(L)	(*(double*)((L).pstr))
#define LLEN(L)		((L).len)
#define LTYPE(L)	((L).type)
#define LMAXLEN(L)	((L).maxlen)
#define LOPT(L)		((L).options)
#define	LMKCONST(L,s)	{LSTR(L)=(s); \
			 LMAXLEN(L)=LLEN(L)=STRLEN(s); \
			 LTYPE(L)=LSTRING_TY;}

/* --- options --- */
#define LOPTION(L,O)	((L).options & (O))
#define LSETOPT(L,O)	((L).options |= (O))
#define LUNSETOPT(L,O)	((L).options &= ~(O))

/* --- string --- */
#define LASCIIZ(s)	{LSTR(s)[LLEN(s)] = '\0';}
#define LZEROSTR(s)	{(s).len=0; (s).type=LSTRING_TY;}
#define LISNULL(s)	(!(s).pstr)

/* --- number --- */
#define IS_NUMBER(A)	(_Lisnum(A) != LSTRING_TY)
#define IS_INT(A)	(LREAL(A) == (double)(long)LREAL(A))
#define TOREAL(A)	((LTYPE(A)==LINTEGER_TY)? (double) LINT(A): LREAL(A))
#define TOINT(A)	((LTYPE(A)==LINTEGER_TY)? LINT(A): (long)LREAL(A))
#define ODD(n)		((n)&0x1)

/* --- allocate strings --- */
#ifdef USEOPTION

#define LINITSTR(s)	{(s).pstr=NULL;(s).len=0;(s).maxlen=0; \
			(s).type=LSTRING_TY;(s).options=0;}
#define LFREESTR(s)	{if ((s).pstr && !LOPTION((s),LOPTFIX)) \
				FREE((s).pstr); }
#define LMOVESTR(d,s)	{(d).pstr=(s).pstr;	(s).pstr=NULL; \
			(d).len=(s).len;	(s).len=0; \
			(d).maxlen=(s).maxlen;	(s).maxlen=0; \
			(d).type=(s).type;	(s).type=0; \
			(d).options=(s).options;(s).options=0;}

#define LPMALLOC(s)	{(s)=(PLstr)MALLOC(sizeof(Lstr),"PLstr");LINITSTR(*(s));}
#define LPFREE(s)	{if ((s)->pstr && !LOPTION((*s),LOPTFIX)) \
				FREE((s)->pstr); FREE((s));}

#else		/* DO NOT USE OPTION */

#define LINITSTR(s)	{(s).pstr=NULL;(s).len=0;(s).maxlen=0; \
			(s).type=LSTRING_TY;}
#define LFREESTR(s)	{if ((s).pstr) FREE((s).pstr); }
#define LMOVESTR(d,s)	{(d).pstr=(s).pstr;	(s).pstr=NULL; \
			(d).len=(s).len;	(s).len=0; \
			(d).maxlen=(s).maxlen;	(s).maxlen=0; \
			(d).type=(s).type;	(s).type=0;}

#define LPMALLOC(s)	{(s)=(PLstr)MALLOC(sizeof(Lstr),"PLstr");LINITSTR(*(s));}
#define LPFREE(s)	{if ((s)->pstr) FREE((s)->pstr); FREE((s));}
#endif

#define LICPY(s,i)	{	LINT(s)  = (i); \
				LLEN(s)  = sizeof(long); \
				LTYPE(s) = LINTEGER_TY; }
#ifdef UNICODE
#	define LWSCPY	Lwscpy
#else
#	define LWSCPY	Lscpy
#endif

/* --- word --- */
#define LSKIPBLANKS(S,P) {while (((P)<LLEN(S)) && ISSPACE(LSTR(S)[P])) (P)++;}
#define LSKIPWORD(S,P)	 {while (((P)<LLEN(S)) && !ISSPACE(LSTR(S)[P])) (P)++;}

/* --- transform --- */
#define L2INT(s)	if (LTYPE(*(s))!=LINTEGER_TY)	L2int((s))
#define L2REAL(s)	if (LTYPE(*(s))!=LREAL_TY)	L2real((s))
#define _L2NUM(s,t)	if (LTYPE(*(s))==LSTRING_TY)	_L2num((s),(t))
#define L2NUM(s)	if (LTYPE(*(s))==LSTRING_TY)	L2num((s))
#define L2STR(s)	if (LTYPE(*(s))!=LSTRING_TY)	L2str((s))

/* -------------------- definitions ---------------------- */
/* --- lstring options --- */
#define LOPTFIX		0x0001
#define LOPTINT		0x0002
#define LOPTREAL	0x0004

/* --- strip options --- */
#define LBOTH		'B'
#define LLEADING	'L'
#define LTRAILING	'T'

/* --- position options --- */
#define LREST		-1	/* for rest of string ie in substr	*/
#define LNOTFOUND	0	/* for not found ie Lindex		*/

/* --- file options --- */
#define LSTARTPOS	-1
#define LREADINCSIZE	32
#define LREADLINE	0
#define LREADFILE	-1

#ifdef MSDOS
#	define	LMAXNUMERICDIGITS	14
#else
#	define	LMAXNUMERICDIGITS	30
#endif

/* --- for interal use of lstring --- */
/* With some extra chars */
/* it must have enough space to allow transformation from */
/* int or real to string without resizing */
#if LMAXNUMERICDIGITS<16
#	define LNORMALISE(size)	((size) | 0x000F)
#else
#	define LNORMALISE(size)	((size) | 0x001F)
#endif
#define LEXTRA	1

#ifndef LONG_MAX
#define LONG_MAX MAXLONG
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* --------------- Lstr function prototypes --------------- */
void  __CDECL Linit(LerrorFunc);	/* must be called at initialisation */

void  __CDECL _Lfree(void *str);		/* free a Lstring */

void  __CDECL Lfx   (const PLstr  s, const size_t len );
void  __CDECL Licpy (const PLstr to, const long   from );
void  __CDECL Lrcpy (const PLstr to, const double from );
void  __CDECL Lscpy (const PLstr to, const char *from );
#ifndef JCC
void  __CDECL Lwscpy(const PLstr to, const wchar_t *from );
#endif
void  __CDECL Lcat  (const PLstr to, const char *from );
int   __CDECL Lcmp  (const PLstr  a, const char *b );

void  __CDECL Lstrcpy ( const PLstr to, const PLstr from );
void  __CDECL Lstrcat ( const PLstr to, const PLstr from );
void  __CDECL Lstrset ( const PLstr to, const size_t length, const char value);
int   __CDECL _Lstrcmp( const PLstr a, const PLstr b );
int   __CDECL Lstrcmp ( const PLstr a, const PLstr b );

void  __CDECL _Lsubstr( const PLstr to, const PLstr from,
		size_t start, size_t length );
int   __CDECL _Lisnum ( const PLstr s );

void  __CDECL L2str  ( const PLstr s );
void  __CDECL L2int  ( const PLstr s );
void  __CDECL L2real ( const PLstr s );
void  __CDECL _L2num ( const PLstr s, const int type );
void  __CDECL L2num  ( const PLstr s );

long  __CDECL Lrdint ( const PLstr s );
double __CDECL Lrdreal( const PLstr s );

void  __CDECL _Ltimeinit(void);

bool  __CDECL Labbrev ( const PLstr information, const PLstr info,
			long length);
void  __CDECL Lb2x    ( const PLstr to, const PLstr from );
void  __CDECL Lbitand ( const PLstr to, const PLstr s1, const PLstr s2,
			const bool usepad, const char pad);
void  __CDECL Lbitor  ( const PLstr to, const PLstr s1, const PLstr s2,
			const bool usepad, const char pad);
void  __CDECL Lbitxor ( const PLstr to, const PLstr s1, const PLstr s2,
			const bool usepad, const char pad);
void  __CDECL Lc2d    ( const PLstr to, const PLstr from, long n );
void  __CDECL Lc2x    ( const PLstr to, const PLstr from );
void  __CDECL Lcenter ( const PLstr to, const PLstr str, const long length,
			const char pad);
void  __CDECL Lchangestr(const PLstr to, const PLstr oldstr, const PLstr str,
			const PLstr newstr);
long  __CDECL Lcountstr(const PLstr target, const PLstr source);
void  __CDECL Lcharin ( FILEP f, const PLstr line, const long start,
			const long length );
void  __CDECL Lcharout( FILEP f, const PLstr line, const long start );
long  __CDECL Lchars  ( FILEP f );
long  __CDECL Lcompare( const PLstr A, const PLstr B, const char pad);
void  __CDECL Lcopies ( const PLstr to, const PLstr str, long n );
void  __CDECL Ld2c    ( const PLstr to, const PLstr from, long n );
void  __CDECL Ld2x    ( const PLstr to, const PLstr from, long length );
int   __CDECL Ldatatype(const PLstr str, char type );
void  __CDECL Ldate   ( const PLstr datestr, char option );
void  __CDECL Ldelstr ( const PLstr to, const PLstr str,
			long start, long length );
void  __CDECL Ldelword( const PLstr to, const PLstr str, long start, long length );
void  __CDECL Lerrortext( const PLstr to, const int errn, const int subn, va_list *ap);
void  __CDECL Lformat ( const PLstr to, const PLstr num, long before,
			long after, long expp, long expt );
dword __CDECL Lhashvalue( const PLstr s );
long  __CDECL Lindex  ( const PLstr haystack, const PLstr needle, long p);
void  __CDECL Linsert ( const PLstr to, const PLstr target, const PLstr newstr,
			long n, long length, const char pad);
void  __CDECL Ljustify( const PLstr to, const PLstr str, long length, char pad);
long  __CDECL Llastpos( const PLstr needle, const PLstr haystack, long p );
void  __CDECL Llinein ( FILEP f, const PLstr line, long *curline,
			long start, long length );
int   __CDECL Llineout( FILEP f, const PLstr line, long *curline, long start );
long  __CDECL Llines  ( FILEP f );
void  __CDECL Llower  ( const PLstr s );
void  __CDECL Loverlay( const PLstr to, const PLstr str, const PLstr target,
			long n, long length, const char pad);
void  __CDECL Lprint  ( FILEP f, const PLstr str );
void  __CDECL Lread   ( FILEP f, const PLstr line, long size );
void  __CDECL Lreverse( const PLstr s);
void  __CDECL Lright  ( const PLstr to, const PLstr str, const long length,
			const char pad);
void  __CDECL Lsoundex( const PLstr to, const PLstr str );
void  __CDECL Lstderr ( const int errno, const int subno, ... );
void  __CDECL Lstrip  ( const PLstr to, const PLstr str, const char action,
			const char pad);
void  __CDECL Lspace  ( const PLstr to, const PLstr str, long n,
			const char pad);
void  __CDECL Lsubstr ( const PLstr to, const PLstr str, long start,
			long length, const char pad);
void  __CDECL Lsubword( const PLstr to, const PLstr from, long n, long length);
void  __CDECL Ltime   ( const PLstr timestr, char option );
void  __CDECL Ltranslate(const PLstr to, const PLstr from, const PLstr tableout,
			const PLstr tablein, const char pad);
void  __CDECL Ltrunc  ( const PLstr to, const PLstr from, long n );
long  __CDECL Lverify ( const PLstr str, const PLstr ref, const bool match,
			long start);
void  __CDECL Lupper  ( const PLstr s );
void  __CDECL Lword   ( const PLstr to, const PLstr from, long n );
long  __CDECL Lwordindex( const PLstr str, long n );
long  __CDECL Lwordlength( const PLstr str, long n);
long  __CDECL Lwordpos( const PLstr phrase, const PLstr s, long n );
long  __CDECL Lwords  ( const PLstr from );
void  __CDECL Lwrite  ( FILEP f, const PLstr line, const bool newline );
void  __CDECL Lx2b    ( const PLstr to, const PLstr from );
void  __CDECL Lx2c    ( const PLstr to, const PLstr from );
void  __CDECL Lx2d    ( const PLstr to, const PLstr from, long n);
void  __CDECL Lxrange ( const PLstr to, byte start, byte stop);

#define Leq(A,B)	(Lequal(A,B)==0)
#define Lne(A,B)	(Lequal(A,B)!=0)
#define Llt(A,B)	(Lequal(A,B)<0)
#define Lle(A,B)	(Lequal(A,B)<1)
#define Lge(A,B)	(Lequal(A,B)>-1)
#define Lgt(A,B)	(Lequal(A,B)>0)

#define Ldeq(A,B)	(Lstrcmp(A,B)==0)
#define Ldne(A,B)	(Lstrcmp(A,B)!=0)
#define Ldlt(A,B)	(Lstrcmp(A,B)<0)
#define Ldle(A,B)	(Lstrcmp(A,B)<1)
#define Ldge(A,B)	(Lstrcmp(A,B)>-1)
#define Ldgt(A,B)	(Lstrcmp(A,B)>0)

#define Lfind(str,phrase,n)		Lwordpos(phrase,str,n)
#define Lleft(to,from,length,pad)	Lsubstr(to,from,1,length,pad)
#define Lpos(needle,haystack,p)		Lindex(haystack,needle,p)

/* ------------- Math functions --------------------- */
void  __CDECL Ladd  ( const PLstr to, const PLstr A, const PLstr B);
int   __CDECL Lbool ( const PLstr num );
void  __CDECL Ldec  ( const PLstr num );
void  __CDECL Ldiv  ( const PLstr to, const PLstr A, const PLstr B );
int   __CDECL Lequal( const PLstr A, const PLstr B);
void  __CDECL Lexpose(const PLstr to, const PLstr A, const PLstr B );
void  __CDECL Linc  ( const PLstr num );
void  __CDECL Lintdiv(const PLstr to, const PLstr A, const PLstr B );
void  __CDECL Lmod  ( const PLstr to, const PLstr A, const PLstr B );
void  __CDECL Lmult ( const PLstr to, const PLstr A, const PLstr B );
void  __CDECL Lneg  ( const PLstr to, const PLstr num );
void  __CDECL Lsub  ( const PLstr to, const PLstr A, const PLstr B );

void  __CDECL Labs  ( const PLstr result, const PLstr num );
int   __CDECL Lsign ( const PLstr num );
void  __CDECL Lpow  ( const PLstr result, const PLstr num, const PLstr p );
#define DECLMATH( func )  void __CDECL L##func(const PLstr result, const PLstr num)
DECLMATH( acos );
DECLMATH( asin );
DECLMATH( atan );
DECLMATH( cos  );
DECLMATH( cosh );
DECLMATH( exp  );
DECLMATH( log  );
DECLMATH( log10);
DECLMATH( pow10);
DECLMATH( sin  );
DECLMATH( sinh );
DECLMATH( sqrt );
DECLMATH( tan  );
DECLMATH( tanh );
#undef DECLMATH

/* ====================== Some variables ================ */
#ifdef __LSTRING_C__
	char
#	ifdef GREEK
#	   ifdef MSDOS
		*clower="abcdefghijklmnopqrstuvwxyz���������������������������������",
		*cUPPER="ABCDEFGHIJKLMNOPQRSTUVWXYZ����������������������������������",
#	   else
		*clower="abcdefghijklmnopqrstuvwxyz������������������������������������",
		*cUPPER="ABCDEFGHIJKLMNOPQRSTUVWXYZ�����������������������ٶ����ڼ��ۿ�",
#	   endif
#	else
		*clower="abcdefghijklmnopqrstuvwxyz",
		*cUPPER="ABCDEFGHIJKLMNOPQRSTUVWXYZ",
#	endif
		*cdigits = "0123456789",
		*chex    = "0123456789ABCDEFabcdef",
		*crxsymb = "@#$_.?!";

	double	lLastScannedNumber=0.0;
	int	lNumericDigits = LMAXNUMERICDIGITS;
	/* int  form */
	byte l2u[256], u2l[256];
	LerrorFunc Lerror;
#else
	extern char
		*clower,
		*cUPPER,
		*cdigits,
		*chex,
		*crxsymb;
	extern double	lLastScannedNumber;
	extern int  lNumericDigits;
	extern byte l2u[], u2l[];
	extern LerrorFunc Lerror;
#endif

#ifdef __cplusplus
}
#endif

#endif
