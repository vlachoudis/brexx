/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/rexxfunc.c,v 1.2 1999/11/26 13:13:47 bnv Exp $
 * $Log: rexxfunc.c,v $
 * Revision 1.2  1999/11/26 13:13:47  bnv
 * Added: Windows CE routines.
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#include <math.h>

#include <lerror.h>
#include <lstring.h>

#include <rexx.h>
#include <rxdefs.h>
#include <compile.h>

#ifdef WCE
#	include <cefunc.h>
#endif

#define DECL( A )  void R_##A ( const int );

DECL( SSoI     )   DECL( SIoC  )  DECL( S   )   DECL( SIoI )
DECL( SSoIoIoC )   DECL( SoSoC )  DECL( SoI )   DECL( IoI  )
DECL( O        )   DECL( SI    )  DECL( C   )   DECL( oSoS )
DECL( SS       )   DECL( SoSoS )

DECL( arg       )  DECL( compare   )  DECL( copies    )  DECL( close     )
DECL( datatype  )  DECL( eof       )  DECL( errortext )
DECL( filesize  )  DECL( format    )  DECL( intr      )  DECL( load      )
DECL( max       )  DECL( min       )  DECL( open      )  DECL( random    )
DECL( read      )  DECL( seek      )  DECL( substr    )  DECL( sourceline)
DECL( strip     )  DECL( storage   )  DECL( space     )  DECL( translate )
DECL( trunc     )  DECL( verify    )  DECL( write     )  DECL( xrange    )

DECL( dropbuf   )
DECL( changestr )
DECL( flush     )
DECL( port      )

DECL( charslines )
DECL( charlinein )
DECL( charlineout )
DECL( stream )

/* Math routines */
DECL( abs_sign  )
DECL( math )
DECL( atanpow )
#undef DECL

#ifdef WCE
#define DECL( A )  void CE_##A ( const int );
DECL( MsgBox    )  DECL( O         )  DECL( gotoxy     )  DECL( oS       )
DECL( S         )  DECL( SS        )  DECL( Clipboard  )  DECL( Dir      )
#undef DECL
#endif

/* ------------- Register Functions Tree ----------- */
static BinTree	*ExtraFuncs = NULL;
/* !!!!!!!!!!!! WARNING THE LIST MUST BE SORTED !!!!!!!!!!! */
static
TBltFunc
rexx_routine[] = {
#ifdef WCE
	{ "A2U",	R_S		,f_a2u		},
#endif
	{ "ABBREV",	R_SSoI		,f_abbrev	},
	{ "ABS",	R_abs_sign	,f_abs		},
	{ "ACOS",	R_math		,f_acos		},
	{ "ADDR",	R_SoSoS		,f_addr		},
	{ "ADDRESS",	R_O		,f_address	},
	{ "ARG",	R_arg		,f_arg		},
	{ "ASIN",	R_math		,f_asin		},
	{ "ATAN",	R_math		,f_atan		},
	{ "ATAN2",	R_atanpow	,f_atan2	},
	{ "B2X",	R_S		,f_b2x		},
	{ "BITAND",	R_SoSoC		,f_bitand	},
	{ "BITOR",	R_SoSoC		,f_bitor	},
	{ "BITXOR",	R_SoSoC		,f_bitxor	},
	{ "C2D",	R_SoI		,f_c2d		},
	{ "C2X",	R_S		,f_c2x		},
	{ "CENTER",	R_SIoC		,f_center	},
	{ "CENTRE",	R_SIoC		,f_center	},
	{ "CHANGESTR",	R_changestr	,f_changestr	},
	{ "CHARIN",	R_charlinein	,f_charin	},
	{ "CHAROUT",	R_charlineout	,f_charout	},
	{ "CHARS",	R_charslines	,f_chars	},
#ifdef WCE
	{ "CLIPBOARD",	CE_Clipboard,	0		},
#endif
	{ "CLOSE",	R_close		,f_close	},
#ifdef WCE
	{ "CLREOL",	CE_O,		f_clreol	},
	{ "CLRSCR",	CE_O,		f_clrscr	},
#endif
	{ "COMPARE",	R_compare	,f_compare	},
	{ "COPIES",	R_copies	,f_copies	},
#ifdef WCE
	{ "COPYFILE",	CE_SS,		f_copyfile	},
#endif
	{ "COS",	R_math		,f_cos		},
	{ "COSH",	R_math		,f_cosh		},
	{ "COUNTSTR",	R_SS		,f_countstr	},
	{ "D2C",	R_IoI		,f_d2c		},
	{ "D2X",	R_IoI		,f_d2x		},
	{ "DATATYPE",	R_datatype	,f_datatype	},
	{ "DATE",	R_C		,f_date		},
#ifdef WCE
	{ "DELFILE",	CE_S,		f_deletefile	},
#endif
	{ "DELSTR",	R_SIoI		,f_delstr	},
	{ "DELWORD",	R_SIoI		,f_delword	},
	{ "DESBUF",	R_O		,f_desbuf	},
	{ "DIGITS",	R_O		,f_digits	},
#ifdef WCE
	{ "DIR",	CE_Dir,		0		},
#endif
	{ "DROPBUF",	R_dropbuf	,f_dropbuf	},
	{ "EOF",	R_eof		,f_eof		},
	{ "ERRORTEXT",	R_errortext	,f_errortext	},
	{ "EXP",	R_math		,f_exp		},
	{ "FIND",	R_SSoI		,f_find		},
	{ "FLUSH",	R_flush		,f_flush	},
	{ "FORM",	R_O		,f_form		},
	{ "FORMAT",	R_format	,f_format	},
	{ "FUZZ",	R_O		,f_fuzz		},
#ifdef WCE
	{ "GETCH",	CE_O,		f_getch		},
#endif
#ifndef WCE
	{ "GETENV",	R_S		,f_getenv	},
#endif
#ifdef WCE
	{ "GOTOXY",	CE_gotoxy,	0		},
#endif
	{ "INDEX",	R_SSoI		,f_index	},
	{ "INSERT",	R_SSoIoIoC	,f_insert	},
#if defined(__BORLANDC__) && !defined(__WIN32__) && !defined(WCE)
	{ "INTR",	R_intr		,f_intr		},
#endif
	{ "JUSTIFY",	R_SIoC		,f_justify	},
#ifdef WCE
	{ "KBHIT",	CE_O,		f_kbhit		},
	{ "LASTERROR",	R_O		,f_lasterror	},
#endif
	{ "LASTPOS",	R_SSoI		,f_lastpos	},
	{ "LEFT",	R_SIoC		,f_left		},
	{ "LENGTH",	R_S		,f_length	},
	{ "LINEIN",	R_charlinein	,f_linein	},
	{ "LINEOUT",	R_charlineout	,f_lineout	},
	{ "LINES",	R_charslines	,f_lines	},
	{ "LOAD",	R_load		,f_load		},
	{ "LOG",	R_math		,f_log		},
	{ "LOG10",	R_math		,f_log10	},
	{ "MAKEBUF",	R_O		,f_makebuf	},
	{ "MAX",	R_max		,f_max		},
	{ "MIN",	R_min		,f_min		},
#ifdef WCE
	{ "MKDIR",	CE_S,		f_createdirectory	},
	{ "MOVEFILE",	CE_SS,		f_movefile	},
	{ "MSGBOX",	CE_MsgBox,	0		},
#endif
	{ "OPEN",	R_open		,f_open		},
	{ "OVERLAY",	R_SSoIoIoC	,f_overlay	},
#if defined(__BORLANDC__) && !defined(__WIN32__) && !defined(WCE)
	{ "PORT",	R_port		,f_port		},
#endif
	{ "POS",	R_SSoI		,f_pos		},
	{ "POW",	R_atanpow	,f_pow		},
	{ "POW10",	R_math		,f_pow10	},
#ifndef WCE
	{ "PUTENV",	R_SS		,f_putenv	},
#endif
	{ "QUEUED",	R_C		,f_queued	},
	{ "RANDOM",	R_random	,f_random	},
	{ "READ",	R_read		,f_read		},
	{ "REVERSE",	R_S		,f_reverse	},
	{ "RIGHT",	R_SIoC		,f_right	},
#ifdef WCE
	{ "RMDIR",	CE_S,		f_removedirectory	},
#endif
	{ "SEEK",	R_seek		,f_seek		},
	{ "SIGN",	R_abs_sign	,f_sign		},
	{ "SIN",	R_math		,f_sin		},
	{ "SINH",	R_math		,f_sinh		},
	{ "SOUNDEX",	R_S		,f_soundex	},
	{ "SOURCELINE",	R_sourceline	,f_sourceline	},
	{ "SPACE",	R_space		,f_space	},
	{ "SQRT",	R_math		,f_sqrt		},
	{ "STORAGE",	R_storage	,f_storage	},
	{ "STREAM",	R_stream	,f_stream	},
	{ "STRIP",	R_strip		,f_strip	},
	{ "SUBSTR",	R_substr	,f_substr	},
	{ "SUBWORD",	R_SIoI		,f_subword	},
	{ "SYMBOL",	R_S		,f_symbol	},
	{ "TAN",	R_math		,f_tan		},
	{ "TANH",	R_math		,f_tanh		},
	{ "TIME",	R_C		,f_time		},
	{ "TRACE",	R_C		,f_trace	},
	{ "TRANSLATE",	R_translate	,f_translate	},
	{ "TRUNC",	R_trunc		,f_trunc	},
#ifdef WCE
	{ "U2A",	R_S		,f_u2a		},
#endif
	{ "VALUE",	R_SoSoS		,f_value	},
	{ "VARDUMP",	R_oSoS		,f_vartree	},
	{ "VERIFY",	R_verify	,f_verify	},
#ifdef WCE
	{ "WHEREX",	CE_O,		f_wherex	},
	{ "WHEREY",	CE_O,		f_wherey	},
	{ "WINDOWTITLE",CE_oS,		f_windowtitle	},
#endif
	{ "WORD",	R_SI		,f_word		},
	{ "WORDINDEX",	R_SI		,f_wordindex	},
	{ "WORDLENGTH",	R_SI		,f_wordlength	},
	{ "WORDPOS",	R_SSoI		,f_wordpos	},
	{ "WORDS",	R_S		,f_words	},
	{ "WRITE",	R_write		,f_write	},
	{ "X2B",	R_S		,f_x2b		},
	{ "X2C",	R_S		,f_x2c		},
	{ "X2D",	R_SoI		,f_x2d		},
	{ "XRANGE",	R_xrange	,f_xrange	}
};

/* ------------- C_isBuiltin --------------- */
TBltFunc *
C_isBuiltin( PLstr func )
{
	int	first, middle, last, cmp;
	PBinLeaf	leaf;

	first = 0;	/* Use binary search to find intruction */
	last  = DIMENSION(rexx_routine)-1;

	while (first<=last)   {
		middle = (first+last)/2;
		if ((cmp=Lcmp(func,rexx_routine[middle].name))==0)
			return (rexx_routine+middle);
		else
		if (cmp<0)
			last  = middle-1;
		else
			first = middle+1;
	}

	/* try to see if it exists in the extra functions */
	if (ExtraFuncs) {
		leaf = BinFind(ExtraFuncs,func);
		if (leaf)
			return (TBltFunc*)(leaf->value);
	}
	return NULL;
} /* C_isBuiltin */

/* ----------- RxRegFunction ------------- */
/* returns TRUE if an error occurs */
int
RxRegFunction( char *name, void (*func)(int), int opt )
{
	Lstr	fn;
	TBltFunc *fp;

	if (ExtraFuncs==NULL) {
		ExtraFuncs = (BinTree*)MALLOC(sizeof(BinTree),"ExtraFuncs");
		BINTREEINIT(*ExtraFuncs);
	}

	LINITSTR(fn);
	Lscpy(&fn,name);
	Lupper(&fn);	/* translate to upper case */

	fp = (TBltFunc*)MALLOC(sizeof(TBltFunc),"RegFunc");

	if (!fp) return TRUE;
	fp->name = NULL;
	fp->func = func;
	fp->opt  = opt;
	BinAdd(ExtraFuncs,&fn,fp);
	LFREESTR(fn);
	return FALSE;
} /* RxRegFunction */

/* ----------- RxRegFunctionDone --------- */
void
RxRegFunctionDone( void )
{
	if (!ExtraFuncs) return;
	BinDisposeLeaf(ExtraFuncs,ExtraFuncs->parent,FREE);
	FREE(ExtraFuncs);
	ExtraFuncs = NULL;
} /* RxRegFunctionDone */
