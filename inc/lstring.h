/*
 * $Header: /home/bnv/tmp/brexx/inc/RCS/lstring.h,v 1.2 1998/09/09 08:44:59 bnv Exp $
 * $Log: lstring.h,v $
 * Revision 1.2  1998/09/09 08:44:59  bnv
 * Removed somed "const" in function calls
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __LSTRING_H__
#define __LSTRING_H__

#include <bnv.h>
#include <bmem.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#if defined(__BORLANDC__)&&(defined(__HUGE__)||defined(__LARGE__))
/* //// There isn't any _fmemmove!!!!!! //// */
#	define	MEMMOVE(a,b,s)	memmove(a,b,s)
#	define	MEMCPY(a,b,s)	_fmemcpy(a,b,s)
#	define	MEMCMP(a,b,s)	_fmemcmp(a,b,s)
#	define	MEMCHR(s,c,n)	_fmemchr(s,c,n)
#	define	MEMSET(s,c,n)	_fmemset(s,c,n)
#	define	STRCPY(a,b)	_fstrcpy(a,b)
#	define	STRCMP(a,b)	_fstrcmp(a,b)
#	define	STRCAT(a,b)	_fstrcat(a,b)
#	define	STRCHR(a,b)	_fstrchr(a,b)
#	define	STRLEN(a)	_fstrlen(a)
#else
#	ifndef __BORLANDC__
#		define huge
#	endif
#	define	MEMMOVE(a,b,s)	memmove(a,b,s)
#	define	MEMCPY(a,b,s)	memcpy(a,b,s)
#	define	MEMCMP(a,b,s)	memcmp(a,b,s)
#	define	MEMCHR(s,c,n)	memchr(s,c,n)
#	define	MEMSET(s,c,n)	memset(s,c,n)
#	define	STRCPY(a,b)	strcpy(a,b)
#	define	STRCMP(a,b)	strcmp(a,b)
#	define	STRCAT(a,b)	strcat(a,b)
#	define	STRCHR(a,b)	strchr(a,b)
#	define	STRLEN(a)	strlen(a)
#endif

/* --- Lstring types --- */
enum	TYPES { LSTRING_TY,
		LINTEGER_TY,
		LREAL_TY };

typedef void    (*LerrorFunc)(const int,const int,...);

/* ------------------------- */
/* --- Lstring structure --- */
/* ------------------------- */
typedef struct Lstr_st {
	char	*pstr;		/* String (Data) pointer		*/
	size_t	len;		/* Actual length of string(data)	*/
	size_t	maxlen;		/* Maximum length allocated for string	*/
	short	type;		/* Type of data STRING, INT, REAL	*/
	short	options;	/* Bit-options				*/
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
#define TOREAL(A)	((LTYPE(A)==LINTEGER_TY)? (double) LINT(A):  LREAL(A) )
#define TOINT(A)	((LTYPE(A)==LINTEGER_TY)? LINT(A):  (long)LREAL(A) )
#define ODD(n)		(n&0x1)

/* --- allocate strings --- */
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

#define LICPY(s,i)	{	LINT(s)  = (i); \
				LLEN(s)  = sizeof(long); \
				LTYPE(s) = LINTEGER_TY; }

/* --- word --- */
#define LSKIPBLANKS(S,P) {while (((P)<LLEN(S)) && ISSPACE(LSTR(S)[P])) (P)++;}
#define LSKIPWORD(S,P)   {while (((P)<LLEN(S)) && !ISSPACE(LSTR(S)[P])) (P)++;}

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
#define LREST		-1      /* for rest of string ie in substr */
#define LNOTFOUND	0       /* for not found ie Lindex         */

/* --- file options --- */
#define LSTARTPOS	-1
#define LREADINCSIZE	32
#define LREADLINE	 0
#define LREADFILE	-1

/* --- for interal use of lstring --- */
/* With some extra chars */
/* it must have enough space to allow transformation from */
/* int or real to string without resizing */
#define LNORMALISE(size)	((size) | 0x000F)
#define LEXTRA		1

/* --------------- Lstr function prototypes --------------- */
void  Linit(LerrorFunc);		/* must be called at initialisation */

void  _Lfree(void *str);		/* free a Lstring */

void  Lfx   (const PLstr  s, const size_t len );
void  Licpy (const PLstr to, const long   from );
void  Lrcpy (const PLstr to, const double from );
void  Lscpy (const PLstr to, const char *from );
void  Lcat  (const PLstr to, const char *from );
int   Lcmp  (const PLstr  a, const char *b );

void  Lstrcpy ( const PLstr to, const PLstr from );
void  Lstrcat ( const PLstr to, const PLstr from );
void  Lstrset ( const PLstr to, const size_t length, const char value);
int   _Lstrcmp( const PLstr a, const PLstr b );
int   Lstrcmp ( const PLstr a, const PLstr b );

void  _Lsubstr( const PLstr to, const PLstr from, size_t start, size_t length );
int   _Lisnum ( const PLstr s );

void  L2str  ( const PLstr s );
void  L2int  ( const PLstr s );
void  L2real ( const PLstr s );
void  _L2num ( const PLstr s, const int type );
void  L2num  ( const PLstr s );

long  Lrdint ( const PLstr s );
double Lrdreal( const PLstr s );

void  _Ltimeinit(void);

bool  Labbrev ( const PLstr information, const PLstr info,
		long length);
void  Lb2x    ( const PLstr to, const PLstr from );
void  Lbitand ( const PLstr to, const PLstr s1, const PLstr s2,
		const bool usepad, const char pad);
void  Lbitor  ( const PLstr to, const PLstr s1, const PLstr s2,
		const bool usepad, const char pad);
void  Lbitxor ( const PLstr to, const PLstr s1, const PLstr s2,
		const bool usepad, const char pad);
void  Lc2d    ( const PLstr to, const PLstr from, long n );
void  Lc2x    ( const PLstr to, const PLstr from );
void  Lcenter ( const PLstr to, const PLstr str, const long length,
		const char pad);
void  Lchangestr(const PLstr to, const PLstr oldstr, const PLstr str,
		const PLstr newstr);
long  Lcountstr(const PLstr target, const PLstr source);
void  Lcharin ( FILE *f, const PLstr line, const long start,
		const long length );
void  Lcharout( FILE *f, const PLstr line, const long start );
long  Lchars  ( FILE *f );
long  Lcompare( const PLstr A, const PLstr B, const char pad);
void  Lcopies ( const PLstr to, const PLstr str, long n );
void  Ld2c    ( const PLstr to, const PLstr from, long n );
void  Ld2x    ( const PLstr to, const PLstr from, long length );
int   Ldatatype(const PLstr str, char type );
void  Ldate   ( const PLstr datestr, char option );
void  Ldelstr ( const PLstr to, const PLstr str, long start, long length );
void  Ldelword( const PLstr to, const PLstr str, long start, long length );
void  Lerrortext( const PLstr to, const int errno, const int subno, va_list ap);
void  Lformat ( const PLstr to, const PLstr num, long before,
		long after, long expp, long expt );
word  Lhashvalue( const PLstr s );
long  Lindex  ( const PLstr haystack, const PLstr needle, long p);
void  Linsert ( const PLstr to, const PLstr target, const PLstr newstr,
		long n, long length, const char pad);
void  Ljustify( const PLstr to, const PLstr str, long length, char pad);
long  Llastpos( const PLstr needle, const PLstr haystack, long p );
void  Llinein ( FILE *f, const PLstr line, long *curline,
		long start, long length );
int   Llineout( FILE *f, const PLstr line, long *curline, long start );
long  Llines  ( FILE *f );
void  Llower  ( const PLstr s );
void  Loverlay( const PLstr to, const PLstr str, const PLstr target,
		long n, long length, const char pad);
void  Lprint  ( FILE *f, const PLstr str );
void  Lread   ( FILE *f, const PLstr line, long size );
void  Lreverse( const PLstr s);
void  Lright  ( const PLstr to, const PLstr str, const long length,
		const char pad);
void  Lsoundex( const PLstr to, const PLstr str );
void  Lstderr ( const int errno, const int subno, ... );
void  Lstrip  ( const PLstr to, const PLstr str, const char action,
		const char pad);
void  Lspace  ( const PLstr to, const PLstr str, long n, const char pad);
void  Lsubstr ( const PLstr to, const PLstr str, long start,
		long length, const char pad);
void  Lsubword( const PLstr to, const PLstr from, long n, long length);
void  Ltime   ( const PLstr timestr, char option );
void  Ltranslate(const PLstr to, const PLstr from, const PLstr tableout,
		const PLstr tablein, const char pad);
void  Ltrunc  ( const PLstr to, const PLstr from, long n );
long  Lverify ( const PLstr str, const PLstr ref, const bool match,
		long start);
void  Lupper  ( const PLstr s );
void  Lword   ( const PLstr to, const PLstr from, long n );
long  Lwordindex( const PLstr str, long n );
long  Lwordlength( const PLstr str, long n);
long  Lwordpos( const PLstr phrase, const PLstr s, long n );
long  Lwords  ( const PLstr from );
void  Lwrite  ( FILE *f, const PLstr line, const bool newline );
void  Lx2b    ( const PLstr to, const PLstr from );
void  Lx2c    ( const PLstr to, const PLstr from );
void  Lx2d    ( const PLstr to, const PLstr from, long n);
void  Lxrange ( const PLstr to, byte start, byte stop);

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
void  Ladd  ( const PLstr to, const PLstr A, const PLstr B);
int   Lbool ( const PLstr num );
void  Ldec  ( const PLstr num );
void  Ldiv  ( const PLstr to, const PLstr A, const PLstr B );
int   Lequal( const PLstr A, const PLstr B);
void  Lexpose(const PLstr to, const PLstr A, const PLstr B );
void  Linc  ( const PLstr num );
void  Lintdiv(const PLstr to, const PLstr A, const PLstr B );
void  Lmod  ( const PLstr to, const PLstr A, const PLstr B );
void  Lmult ( const PLstr to, const PLstr A, const PLstr B );
void  Lneg  ( const PLstr to, const PLstr num );
void  Lsub  ( const PLstr to, const PLstr A, const PLstr B );

void  Labs  ( const PLstr result, const PLstr num );
int   Lsign ( const PLstr num );
void  Lpow  ( const PLstr result, const PLstr num, const PLstr p );
#define DECLMATH( func )  void L##func(const PLstr result, const PLstr num)
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
#	if defined(__GREEK__)
#		if defined(MSDOS)
		*clower="abcdefghijklmnopqrstuvwxyz˜™š›œŸ ¡¢£¤¥¦§¨©«¬­®¯àªáâãåæçéäè",
		*cUPPER="ABCDEFGHIJKLMNOPQRSTUVWXYZ€‚ƒ„…†‡ˆ‰Š‹Œ‘’“”•–—‘êëìíîïğˆ“",
#		else
		*clower="abcdefghijklmnopqrstuvwxyzáâãäåæçèéêëìíîïğñóôõö÷øùÜİŞßúÀüıûàşò",
		*cUPPER="ABCDEFGHIJKLMNOPQRSTUVWXYZÁÂÃÅÄÆÇÈÉÊËÌÍÎÏĞÑÓÔÕÖ×ØÙ¶¸¹ºÚÚ¼¾ÛÛ¿Ó",
#		endif
#	else
		*clower="abcdefghijklmnopqrstuvwxyz",
		*cUPPER="ABCDEFGHIJKLMNOPQRSTUVWXYZ",
#	endif
		*cdigits = "0123456789",
		*chex    = "0123456789ABCDEFabcdef",
		*crxsymb = "@#$_.?!";

	char formatStringToReal[10] = "%.8lG";
	byte l2u[256], u2l[256];
	LerrorFunc Lerror;
#else
	extern char
		*clower,
		*cUPPER,
		*cdigits,
		*chex,
		*crxsymb;
	extern char formatStringToReal[];
	extern byte l2u[], u2l[];
	extern LerrorFunc Lerror;
#endif
#endif
