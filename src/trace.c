/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/trace.c,v 1.2 1999/03/10 16:55:35 bnv Exp $
 * $Log: trace.c,v $
 * Revision 1.2  1999/03/10 16:55:35  bnv
 * A bracket addition to keep compiler happy.
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#define __TRACE_C__

#include <bnv.h>
#include <stdio.h>
#include <stdlib.h>
#include <lstring.h>

#include <rexx.h>
#include <trace.h>
#include <compile.h>
#include <interpre.h>
#include <variable.h>
#include <nextsymb.h>

/* ---------- function prototypes ------------- */
void    RxInitInterStr( PLstr str );

/* ---------- Extern variables ---------------- */
extern	Clause	*CompileClause;		/* compile clauses	*/
extern	int	CompileCurClause;	/* current clause	*/
extern	ErrorMsg errortext[];	/* from lstring/errortxt.c */
extern	bool	_in_nextsymbol;		/* from nextsymb.c	*/
extern	int	_trace;			/* from interpret.c	*/
extern	PLstr	RxStck[];		/*     -// -		*/
extern	int	RxStckTop;		/*     -// -		*/

static	char	TraceChar[] = {' ','>','L','V','C','O','F','.'};

PLstr	InteractiveStr;

/* ----------------- TraceCurline ----------------- */
int
TraceCurline( RxFile **rxf, int print )
{
	size_t	line;
	size_t	cl, codepos;
	char	*ch, *chend;

	if (symbolptr==NULL) {	/* we are in intepret */
		if (CompileClause==NULL) {
			if (rxf) *rxf = rxfile;
			return -1;
		}

		codepos = (size_t)((byte huge *)Rxcip - (byte huge *)Rxcodestart);
		/* search for clause */
		cl = 0;
		while (CompileClause[cl].ptr) {
			if (CompileClause[cl].code >= codepos)
				break;
			cl++;
		}
		cl--;
		line = CompileClause[cl].line;
		ch = CompileClause[cl].ptr;
		chend = CompileClause[cl+1].ptr;
		if (chend==NULL)
			for (chend=ch; *chend!='\n'; chend++) /*do nothing*/;;
		_nesting = _rx_proc + CompileClause[cl].nesting;
		if (rxf)
			*rxf = CompileClause[ cl ].fptr;
	} else {		/* we are in compile  */
		if (CompileCurClause==0)
			cl = 0;
		else
			cl = CompileCurClause-1;

		_nesting   = CompileClause[ cl ].nesting;
		if (rxf) {
			if (CompileCurClause==0)
				*rxf = CompileRxFile;
			else
				*rxf = CompileClause[ cl ].fptr;
		}
		if (_in_nextsymbol) {
			line = symboline;
			ch = symbolptr;
			while (ch>symbolprevptr)
				if (*ch--=='\n') line--;
			ch = symbolprevptr;
		} else
		if (cl==0) {
			line = 1;
			ch   = LSTR((*rxf)->file);
		} else {
			cl   = CompileCurClause-1;
			line = CompileClause[ cl ].line;
			ch   = CompileClause[ cl ].ptr;
		}
		for (chend=ch; *chend!=';' && *chend!='\n'; chend++) /*do nothing*/;;
	}

	if (print) fprintf(stderr,"%6d *-* ",line);

	if (print) {
		int	i;
		for (i=1; i<_nesting; i++) fputc(' ',stderr);

		while (*ch && ch<chend) {
			if (*ch!='\n')
				fputc(*ch,stderr);
			ch++;
		}
		fputc('\n',stderr);
	}
	return line;
} /* TraceCurline */

/* ---------------- TraceSet -------------------- */
void
TraceSet( PLstr trstr )
{
	char	*ch;

	L2STR(trstr);
	Lupper(trstr);
	LASCIIZ(*trstr);
	ch = LSTR(*trstr);
	if (*ch=='!') {
		ch++;
	} else
	if (*ch=='?') {
		_Proc[_rx_proc].interactive_trace
			= 1 - _Proc[_rx_proc].interactive_trace;
		if (_Proc[_rx_proc].interactive_trace)
			fprintf(stderr,"       +++ %s +++\n",errortext[2].errmsg);
		ch++;
	}

	switch (*ch) {
		case 'A':
			_Proc[_rx_proc].trace = all_trace;
			break;
		case 'C':
			_Proc[_rx_proc].trace = commands_trace;
			break;
		case 'E':
			_Proc[_rx_proc].trace = error_trace;
			break;
/*
///		case 'F':
///			_Proc[_rx_proc].trace = ;
///			break;
*/
		case 'I':
			_Proc[_rx_proc].trace = intermediates_trace;
			break;
		case 'L':
			_Proc[_rx_proc].trace = labels_trace;
			break;
		case 'N':
			_Proc[_rx_proc].trace = normal_trace;
			break;
		case 'O':
			_Proc[_rx_proc].trace = off_trace;
			_Proc[_rx_proc].interactive_trace = FALSE;
			break;
		case 'R':
			_Proc[_rx_proc].trace = results_trace;
			break;
		case 'S':
			_Proc[_rx_proc].trace = scan_trace;
			break;
#ifdef __DEBUG__
		case 'D':
			__debug__ = 1-__debug__;
			if (__debug__)
				printf("\n\nInternal DEBUG starting...\n");
			else
				printf("\n\nInternal DEBUG ended\n");
			break;
#endif
		default:
			Lerror(ERR_INVALID_TRACE,1,trstr);
	}
} /* TraceSet */

/* --------------------- TraceByte -------------------- */
void
TraceByte( int middlechar )
{
	byte	tracebyte=0;

	tracebyte |= (middlechar & TB_MIDDLECHAR);
	tracebyte |= TB_TRACE;

	_CodeAddByte( tracebyte );
} /* TraceByte */

/* ------------------ TraceClause ----------------- */
void
TraceClause( void )
{
	if (_Proc[_rx_proc].interactive_trace) {
		/* return if user specified a string for interactive trace */
		if (TraceInteractive(TRUE))
			return;
	}
	TraceCurline(NULL,TRUE);
} /* TraceClause */

/* ------------------ TraceInstruction ----------------- */
void
TraceInstruction( byte inst )
{
	if ((inst & TB_MIDDLECHAR) != nothing_middle)
		if (_Proc[_rx_proc].trace == intermediates_trace) {
			int	i;
			fprintf(stderr,"       >%c>  ",TraceChar[ inst & TB_MIDDLECHAR ]);
			for (i=0; i<_nesting; i++) fputc(' ',stderr);
			fputc('\"',stderr);
			Lprint(stderr,RxStck[RxStckTop]);
			fprintf(stderr,"\"\n");
		}
} /* TraceInstruction */
/* ---------------- TraceInteractive ------------------- */
int
TraceInteractive( int frominterpret )
{
	LPMALLOC(InteractiveStr); Lfx(InteractiveStr,1);
	Lread(stdin,InteractiveStr,LREADLINE);
	if (!LLEN(*InteractiveStr)) {
		LPFREE(InteractiveStr);
		return FALSE;
	}

	_trace = FALSE;

	RxInitInterStr(InteractiveStr);
	_Proc[_rx_proc].calltype = CT_INTERACTIVE;
	if (frominterpret) {
		_Proc[_rx_proc].calltype = CT_INTERACTIVE;
		/* lets go again to NEWCLAUSE */
#ifdef ALIGN
		_Proc[_rx_proc].ip-=4;
#else
		_Proc[_rx_proc].ip--;
#endif
	}
	return TRUE;
} /* TraceInteractive */
