/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/interpre.c,v 1.1 1998/07/02 17:34:50 bnv Exp $
 * $Log: interpre.c,v $
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#define __INTERPRET_C__

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

#include <bnv.h>
#include <bmem.h>

#include <lerror.h>
#include <lstring.h>

#include <rexx.h>
#include <stack.h>
#include <trace.h>
#include <bintree.h>
#include <compile.h>
#include <interpre.h>

/* ---------------- global variables ------------------ */
int	_trace;		/* if trace is enabled */
PLstr   RxStck[STCK_SIZE];	/* Array of PLstr       */
int     RxStckTop;		/* top item of stack    */

/* ---------------- Local variables ------------------- */
static	Lstr	tmpstr[STCK_SIZE];	/* temporary strings	*/
static	Scope	VarScope;	/* Variables Scope      */
static	PLstr	ToParse;	/* Parsing variables	*/
static	int	DataStart,
		DataEnd,
		BreakStart,
		BreakEnd,
		SourceEnd;	/* Length of string+1	*/


static	jmp_buf  old_error;	/* keep old value of errortrap */

extern Lstr	stemvaluenotfound;	/* from variable.c */

#ifdef __DEBUG__
#	define DEBUGDISPLAY0(a)		if (__debug__) printf("\t%u\t%s\n",inst_ip,(a))
#	define DEBUGDISPLAY0nl(a)	if (__debug__) printf("\t%u\t%s\t",inst_ip,(a))
#	define DEBUGDISPLAY(a)		if (__debug__) {printf("\t%u\t%s\t\"",inst_ip,(a)); \
					Lprint(stdout,RxStck[RxStckTop]); printf("\"\n"); }
#	define DEBUGDISPLAYi(a,b)	if (__debug__) {printf("\t%u\t%s\t\"",inst_ip,(a)); \
					Lprint(stdout,(b)); printf("\"\n"); }
#	define DEBUGDISPLAY2(a)		if (__debug__) {printf("\t%u\t%s\t\"",inst_ip,(a)); \
					Lprint(stdout,RxStck[RxStckTop-1]); printf("\",\""); \
					Lprint(stdout,RxStck[RxStckTop]);printf("\"\n"); }

int	instr_cnt[256];		/* instruction counter */
#else
#	define DEBUGDISPLAY0(a)
#	define DEBUGDISPLAY0nl(a)
#	define DEBUGDISPLAY(a)
#	define DEBUGDISPLAYi(a,b)
#	define DEBUGDISPLAY2(a)
#endif

#define CHKERROR	if (RxStckTop==STCK_SIZE-1) Lerror(ERR_STORAGE_EXHAUSTED,0)
#define	INCSTACK	{ RxStckTop++; CHKERROR; }
#define	POP_C_POP_B_PEEK_A	{ POP(C); POP(B); PEEK(A); }
#define PEEK(x)		x = &(RxStck[RxStckTop])
#define PEEKR(x,r)	x = &(RxStck[RxStckTop-(r)])
#define POP(x)		x = &(RxStck[RxStckTop--])
#define PUSH(x)		{x = &(RxStck[++RxStckTop]); CHKERROR; }

#if !defined(ALIGN)
#	define PLEAF(x)		{ x = (PBinLeaf)(*(dword*)Rxcip); \
				Rxcip += sizeof(dword); }
#	define INCWORD(x)	(x) += sizeof(word)
#	define INCDWORD(x)	(x) += sizeof(dword)
#	define CWORD		word
#else
#	define PLEAF(x)		x = (PBinLeaf)(*Rxcip++)
#	define INCWORD(x)	(x)++
#	define INCDWORD(x)	(x)++
#	define CWORD		dword
#endif

/* ---------------- RxProcResize ---------------- */
static void
RxProcResize( void )
{
	size_t oldsize=_Proc_size;

	_Proc_size += PROC_INC;
	_Proc = (RxProc*) REALLOC( _Proc, _Proc_size * sizeof(RxProc) );
	MEMSET(_Proc+oldsize,0,PROC_INC*sizeof(RxProc));
} /* RxProcResize */

/* ------------- I_trigger_space -------------- */
static void
I_trigger_space( void )
{
	/* normalise to 0 .. len-1 */
	DataStart = BreakEnd-1;

	/* skip leading spaces */
	LSKIPBLANKS(*ToParse,DataStart);

	/* find word */
	BreakStart = DataStart;
	LSKIPWORD(*ToParse,BreakStart);

	/* skip trailing spaces */
	BreakEnd = BreakStart;
	LSKIPBLANKS(*ToParse,BreakEnd);

	/* again in rexx strings 1..len */
	DataStart++;
	BreakStart++;
	BreakEnd++;
} /* I_trigger_space */

/* ------------- I_trigger_litteral -------------- */
static void
I_trigger_litteral(const PLstr lit)
{
	int	PatternPos;

	PatternPos = (int)Lpos(lit,ToParse,DataStart);
	if (PatternPos>0) {
		BreakStart = PatternPos;
		BreakEnd = PatternPos + LLEN(*lit);
	} else {	/* the rest of the source is selected */
		DataStart = BreakEnd;
		BreakStart = SourceEnd;
		BreakEnd = SourceEnd;
	}
} /* I_trigger_litteral */

/* ----------------- I_LoadOption ---------------- */
static void
I_LoadOption( const PLstr value, const int opt )
{
	char	*ch;

	switch (opt) {
		case environment_opt:
			Lstrcpy(value,_Proc[_rx_proc].env);
			break;

		case digits_opt:
			Licpy(value,_Proc[_rx_proc].digits);
			break;

		case fuzz_opt:
			Licpy(value,_Proc[_rx_proc].fuzz);
			break;

		case form_opt:
			Lscpy(value,(_Proc[_rx_proc].form)?"SCIENTIFIC":"ENGINEERING");
			break;

		case author_opt:
			Lscpy(value,AUTHOR);
			break;
		
		case version_opt:
			Lscpy(value,VERSION);
			break;

		case os_opt:
			Lscpy(value,OS);
			break;

		case calltype_opt:
			switch (_Proc[_rx_proc].calltype) {
				case CT_PROCEDURE:
					Lscpy(value,"PROCEDURE");
					break;
				case CT_FUNCTION:
					Lscpy(value,"FUNCTION");
					break;
				default:
					Lscpy(value,"COMMAND");
			}
			break;

		case filename_opt:
			Lstrcpy(value,&(CompileClause[0].fptr)->filename);
			break;

		case prgname_opt:
			Lscpy(value,_prgname);
			break;

		case shell_opt:
			ch = getenv(SHELL);
			if (ch)
				Lscpy(value,ch);
			else
				LZEROSTR(*value);
			break;

		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
} /* I_LoadOption */

/* ----------------- I_StoreOption --------------- */
static void
I_StoreOption( const PLstr value, const int opt )
{
	long	l;

	switch (opt) {
		case environment_opt:
			if (LLEN(*value) > 250)
				Lerror(ERR_ENVIRON_TOO_LONG,1,value);
			if (_Proc[_rx_proc].env == _Proc[_rx_proc-1].env)
				LPMALLOC(_Proc[_rx_proc].env);
			Lstrcpy(_Proc[_rx_proc].env,value);
			break;

		case trace_opt:
			TraceSet(value);
			if ((_Proc[_rx_proc].trace == normal_trace) ||
				(_Proc[_rx_proc].trace == off_trace))
					_trace = FALSE;
			else
					_trace = TRUE;

			break;

		case digits_opt:
			if (LLEN(*value)==0)
				_Proc[_rx_proc].digits = 9;
			else {
				l = Lrdint(value);
				if (l <= 0)
					Lerror(ERR_INVALID_INTEGER,5,value);
				_Proc[_rx_proc].digits = (int)l;
			}
			if (_Proc[_rx_proc].digits <= _Proc[_rx_proc].fuzz)
				Lerror(ERR_INVALID_RESULT,1);
			break;

		case fuzz_opt:
			if (LLEN(*value)==0)
				_Proc[_rx_proc].fuzz = 0;
			else {
				l = Lrdint(value);
				if (l <= 0)
					Lerror(ERR_INVALID_INTEGER,6,value);
				_Proc[_rx_proc].fuzz = (int)l;
			}
			if (_Proc[_rx_proc].digits <= _Proc[_rx_proc].fuzz)
				Lerror(ERR_INVALID_RESULT,1);
			break;

		case form_opt:
			_Proc[_rx_proc].form = (int)Lrdint(value);
			break;

		case set_signal_opt:
		case set_signal_name_opt:
			switch (LSTR(*value)[0]) {
				case 'E':
					_Proc[_rx_proc].condition |= SC_ERROR;
					if (opt==set_signal_name_opt)
						_Proc[_rx_proc].lbl_error = RxStck[RxStckTop-1];
					else
						_Proc[_rx_proc].lbl_error = &(ErrorStr->key);
					break;
				case 'H':
					_Proc[_rx_proc].condition |= SC_HALT;
					if (opt==set_signal_name_opt)
						_Proc[_rx_proc].lbl_halt = RxStck[RxStckTop-1];
					else
						_Proc[_rx_proc].lbl_halt = &(HaltStr->key);
					break;
				case 'N':
					if (LSTR(*value)[2]=='V') {
						_Proc[_rx_proc].condition |= SC_NOVALUE;
						if (opt==set_signal_name_opt)
							_Proc[_rx_proc].lbl_novalue = RxStck[RxStckTop-1];
						else
							_Proc[_rx_proc].lbl_novalue = &(NoValueStr->key);
					} else {
						_Proc[_rx_proc].condition |= SC_NOTREADY;
						if (opt==set_signal_name_opt)
							_Proc[_rx_proc].lbl_notready = RxStck[RxStckTop-1];
						else
							_Proc[_rx_proc].lbl_notready = &(NotReadyStr->key);
					}
					break;
				case 'S':
					_Proc[_rx_proc].condition |= SC_SYNTAX;
					if (opt==set_signal_name_opt)
						_Proc[_rx_proc].lbl_syntax = RxStck[RxStckTop-1];
					else
						_Proc[_rx_proc].lbl_syntax = &(SyntaxStr->key);
					break;
				default:
					Lerror(ERR_INTERPRETER_FAILURE,0);
			}
			break;

		case unset_signal_opt:
			switch (LSTR(*value)[0]) {
				case 'E':
					_Proc[_rx_proc].condition &= ~SC_ERROR;
					break;
				case 'H':
					_Proc[_rx_proc].condition &= ~SC_HALT;
					break;
				case 'N':
					if (LSTR(*value)[2]=='V')
						_Proc[_rx_proc].condition &= ~SC_NOVALUE;
					else
						_Proc[_rx_proc].condition &= ~SC_NOTREADY;
					break;
				case 'S':
					_Proc[_rx_proc].condition &= ~SC_SYNTAX;
					break;
				default:
					Lerror(ERR_INTERPRETER_FAILURE,0);
			}
			break;

		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
} /* I_StoreOption */

/* ---------------- I_MakeIntArgs ---------------- */
/* prepare arguments for an internal function call */
/* returns stack position after call               */
#ifdef INLINE
inline
#endif
static int
I_MakeIntArgs( const int na, const int realarg, const word existarg )
{
	int	i,st;
	word	bp;	/* bit position */

	Rxarg.n = na;
	bp = (1 << (na-1));

	/* must doit reverse */
	MEMSET(Rxarg.a,0,sizeof(Rxarg.a));

	Rxarg.r = RxStck[RxStckTop-realarg];
	
	st = RxStckTop;	/* stack position of arguments */
	for (i=na-1; i>=0; i--) {
		if (existarg & bp) {
			if (Rxarg.r == RxStck[st]) {
				Lstrcpy(&(tmpstr[st]),RxStck[st]);
				Rxarg.a[i] = &tmpstr[st];
			} else
				Rxarg.a[i] = RxStck[st];
			st--;
		}
		bp >>= 1;
	}
	return st;
} /* I_MakeIntArgs */

/* ---------------- I_MakeArgs ---------------- */
/* prepare arguments for a call to a func       */
#ifdef INLINE
inline
#endif
static void
I_MakeArgs( const int calltype, const int na, const word existarg )
{
	int	i,st;
	word	bp;	/* bit position */
	RxProc	*pr;
	Args	*arg;

	_rx_proc++;		/* increase program items       */

	if (_rx_proc==_Proc_size) RxProcResize();
	pr = _Proc+_rx_proc;

	/* initialise pr structure */
	/* use the old values      */
	MEMCPY(pr,_Proc+_rx_proc-1,sizeof(*pr));
	pr->calltype	= calltype;
	pr->ip		= (size_t)((byte huge *)Rxcip - (byte huge *)Rxcodestart);
	pr->stacktop	= RxStckTop;

	/* setup arguments */
	arg = &(pr->arg);
	arg->n	= na;

	bp = (1 << (na-1));

	/* must doit reverse */
	MEMSET(arg->a,0,sizeof(arg->a));
	
	st = RxStckTop;	/* stack position of arguments */
	for (i=na-1; i>=0; i--) {
		if (existarg & bp) {
			arg->a[i] = RxStck[st];
			st--;
		} else
			arg->a[i] = NULL;
		bp >>= 1;
	}
	arg->r = RxStck[st];

	if (calltype==CT_FUNCTION)
		pr->stack	= st;
	else
		pr->stack	= st-1;
} /* I_MakeArgs */

/* -------------- I_CallFunction ---------------- */
static int
I_CallFunction( void )
{
	PBinLeaf	leaf,litleaf;
	RxFunc	*func;
	int	ct,nargs,realarg,st;
	CTYPE	existarg, line;
	Lstr	cmd;
	PLstr	res;
#ifdef __DEBUG__
	size_t	inst_ip;
#endif

	/* --- read the arguments here --- */
	PLEAF(leaf);			/* function number	*/
	nargs = *(Rxcip++);		/* number of args	*/
	realarg = *(Rxcip++);		/* real arguments	*/
	existarg = *(CWORD *)Rxcip;	/* existing arguments	*/
		INCWORD(Rxcip);
	line  = *(CWORD *)Rxcip;	/* SIGL line		*/
		INCWORD(Rxcip);
	ct = *(Rxcip++);		/* call type		*/
	func = (RxFunc *)(leaf->value);

#ifdef __DEBUG__
	if (__debug__) {
		int	i;
		for (i=0; i<LLEN(leaf->key); i++)
			putchar(LSTR(leaf->key)[i]);
		printf(" NoArgs=%d, Exist=%X Type=%s\n",
			nargs, existarg,(func->type==FT_BUILTIN)?"builtin":"other");
	}
#endif

	if (func->type == FT_BUILTIN) {
		nargs = I_MakeIntArgs(nargs,realarg,existarg);
		(func->builtin->func)(func->builtin->opt);
		if (ct==CT_PROCEDURE) {
			RxVarSet(VarScope,ResultStr,Rxarg.r);
			RxStckTop = nargs-1;	/* clear stack */
		} else
			RxStckTop = nargs;
		return TRUE;
	} else {
		if (func->label == UNKNOWN_LABEL) {
			/* try an external function */
/***
/// First check to see if this prg exist with
/// the extension of the calling program
/// but this should be done in compiling time..
***/
			st = RxStckTop-realarg;
			res = RxStck[st++];
			LINITSTR(cmd);
			Lstrcpy(&cmd,&(leaf->key));
			while (st<=RxStckTop) {
				Lcat(&cmd," ");
				Lstrcat(&cmd,RxStck[st++]);
			}
			RxRedirectCmd(&cmd,FALSE,TRUE,res);
			LFREESTR(cmd);
			RxStckTop -= realarg;
			return TRUE;
		} else {
			Rxcip++;
			RxSetSpecialVar(SIGLVAR,line);
			I_MakeArgs(ct,nargs,existarg);
			Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + func->label);
			Rxcip++;	/* skip the newclause_mn */
			if (_trace) TraceClause();

			/* handle proc_mn code */
			if (*Rxcip == proc_mn) {
				int	exposed;

				/* give a unique program id */
				/* we might have a problem after 2*32 routine calls!! */
				_procidcnt++;
				_Proc[_rx_proc].id = _procidcnt;
				Rx_id  = _procidcnt;
#ifdef __DEBUG__
				if (__debug__)
					inst_ip = (size_t)((byte huge *)Rxcip - (byte huge *)Rxcodestart);
#endif
				DEBUGDISPLAY0nl("PROC ");
				Rxcip++;
				_Proc[_rx_proc].scope = RxScopeMalloc();
				VarScope = _Proc[_rx_proc].scope;

				/* handle exposed variables */
				exposed = *(Rxcip++);
#ifdef __DEBUG__
				if (__debug__ && exposed>0)
					printf("EXPOSE");
#endif
				for (;exposed>0;exposed--) {
					PLEAF(litleaf);		/* Get pointer to variable */
#ifdef __DEBUG__
					if (__debug__) {
						putchar(' ');
						Lprint(stdout,&(litleaf->key));
					}
#endif
					RxVarExpose(VarScope,litleaf);
				}
#ifdef __DEBUG__
				if (__debug__)
					putchar('\n');
#endif
			}
		}
		return FALSE;
	}
} /* I_CallFunction */

/* ---------------- I_ReturnProc -------------- */
/* restore arguments after a procedure return	*/
static void
I_ReturnProc( void )
{
		/* fix ip and stack */
	Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + _Proc[_rx_proc].ip);
	RxStckTop = _Proc[_rx_proc].stack;

	if (_rx_proc>0) {
			/* free everything that it is new */
		if (VarScope!=_Proc[_rx_proc-1].scope) {
			RxScopeFree(VarScope);
			FREE(VarScope);
		}

		if (_Proc[_rx_proc].env != _Proc[_rx_proc-1].env)
			LPFREE(_Proc[_rx_proc].env);
	}

		/* load previous data and exit */
	_rx_proc--;
	Rx_id = _Proc[_rx_proc].id;
	VarScope = _Proc[_rx_proc].scope;

	if ((_Proc[_rx_proc].trace == normal_trace) ||
		(_Proc[_rx_proc].trace == off_trace))
			_trace = FALSE;
	else
			_trace = TRUE;
} /* I_ReturnProc */

/* ------------ RxInitInterStr -------------- */
void
RxInitInterStr( PLstr str )
{
	RxProc	*pr;

	_rx_proc++;		/* increase program items       */
	if (_rx_proc==_Proc_size) RxProcResize();
	pr = _Proc+_rx_proc;

	/* program id is the same */
	MEMCPY(pr,_Proc+_rx_proc-1,sizeof(*pr));
	pr->calltype	= CT_INTERPRET;
	pr->ip		= (size_t)((byte huge *)Rxcip - (byte huge *)Rxcodestart);
	pr->codelen	= LLEN(*_code);
	pr->clauselen	= CompileCurClause;
	pr->stack	= RxStckTop;
	pr->stacktop	= RxStckTop;

	/* setup arguments */
	(pr->arg).n	= 0;
	MEMSET(pr->arg.a,0,sizeof(pr->arg.a));
	(pr->arg).r = NULL;


	/* --- save state --- */
	MEMCPY(old_error,_error_trap,sizeof(_error_trap));
	signal(SIGINT,SIG_IGN);

	/* compile the program */
	RxInitCompile(rxfile,str);
	RxCompile();

	/* --- restore state --- */
	MEMCPY(_error_trap,old_error,sizeof(_error_trap));
	signal(SIGINT,RxHaltTrap);
	/* might have changed position */
	Rxcodestart = (CIPTYPE*)LSTR(*_code);
	Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + pr->codelen);

	/* check for an error in compilation */
	if (RxReturnCode) {
		/* --- load previous data and exit ---- */
		Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + pr->ip);
		_rx_proc--;
		Rx_id = _Proc[_rx_proc].id;
		VarScope = _Proc[_rx_proc].scope;

		RxSetSpecialVar(RCVAR,RxReturnCode);
		RxSignalCondition(SC_SYNTAX);
	}
} /* RxInitInterStr */

/* ------------ RxDoneInterStr -------------- */
static void
RxDoneInterStr( void )
{
	extern PLstr	InteractiveStr;

		/* fix ip and stack */
	if (_Proc[_rx_proc].calltype == CT_INTERACTIVE) {
		LPFREE(InteractiveStr);
		if ((_Proc[_rx_proc].trace == normal_trace) ||
			(_Proc[_rx_proc].trace == off_trace))
				_trace = FALSE;
			else
				_trace = TRUE;
	}

	Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + _Proc[_rx_proc].ip);
	RxStckTop = _Proc[_rx_proc].stack;

	/* fixup code length, cut the interpretation code */
	LLEN(*_code) = _Proc[_rx_proc].codelen;
	CompileCurClause = _Proc[_rx_proc].clauselen;
	if (_Proc[_rx_proc].env != _Proc[_rx_proc-1].env) {
		Lstrcpy(_Proc[_rx_proc-1].env, _Proc[_rx_proc].env);
		LPFREE(_Proc[_rx_proc].env);
	}

		/* --- load previous data and exit ---- */
	_rx_proc--;
	Rx_id = _Proc[_rx_proc].id;

	_Proc[_rx_proc].trace = _Proc[_rx_proc+1].trace;
	_Proc[_rx_proc].interactive_trace = _Proc[_rx_proc+1].interactive_trace;
	VarScope = _Proc[_rx_proc].scope;
} /* RxDoneInterStr */

/* ---------------- RxInitInterpret --------------- */
void
RxInitInterpret( void )
{
	int	i;

#ifdef __DEBUG__
	MEMSET(instr_cnt,sizeof(instr_cnt),0);
#endif
	MEMSET(RxStck,0,(STCK_SIZE)*sizeof(PLstr));
	RxStckTop = -1;
	MEMSET(tmpstr,0,(STCK_SIZE)*sizeof(Lstr));
	for (i=0; i<STCK_SIZE;i++)
		Lfx(&(tmpstr[i]),0);
} /* RxInitInterpret */

/* ---------------- RxDoneInterpret --------------- */
void
RxDoneInterpret( void )
{
	int	i;
#ifdef __DEBUG__
	FILE	*fout;
	fout = fopen("instr.cnt","w");
	fprintf(fout,"Instr\tCount\n");
	for (i=0; i<pow_mn; i++)	/* pow is the last command */
		fprintf(fout,"%d\t%d\n",i,instr_cnt[i]);
	fclose(fout);

	if (RxStckTop>=0)
		fprintf(stderr,"interpret: Something left in stack %d\n", RxStckTop);
#endif

	/* clear stack */
	for (i=0; i<STCK_SIZE; i++) {
#ifdef __DEBUG__
		if (__debug__ && LLEN(tmpstr[i])) {
			fprintf(stderr,"Freeing... %d: \"",i);
			Lprint(stderr,&(tmpstr[i]));
			fprintf(stderr,"\"\n");
		}
#endif
		LFREESTR(tmpstr[i]);
	}
} /* RxDoneInterpret */

/* ---------------- RxInterpret --------------- */
int
RxInterpret( void )
{
	PLstr	a;
	IdentInfo	*inf;
	byte	na,nf;
	CTYPE	w;
	int	jc, errno, subno, found;
	PBinLeaf	litleaf,leaf;
	RxFunc	*func;
#ifdef __DEBUG__
	size_t	inst_ip;
	char	cmd='\n';
	int	i;
#endif
	RxReturnCode = 0;
	Rx_id  = _Proc[_rx_proc].id;

	Rxcodestart = (CIPTYPE*)LSTR(*_code);
	VarScope = _Proc[_rx_proc].scope;
	Rxcip   = (CIPTYPE*)((byte huge *)Rxcodestart + _Proc[_rx_proc].ip);
	_Proc[_rx_proc].stack = RxStckTop;

	if ((_Proc[_rx_proc].trace == normal_trace) ||
		(_Proc[_rx_proc].trace == off_trace))
			_trace = FALSE;
	else
			_trace = TRUE;

	signal(SIGINT,RxHaltTrap);

	if ((jc=setjmp(_error_trap))!=0) {
		 CIPTYPE *tmp_Rxcip;

		if (jc==JMP_EXIT) {
			RxStckTop = -1;
			goto interpreter_fin;
		}
		/* else if (jc==JMP_CONTINUE) .... CONTINUE code */

		/* exit from interpret, if we are in any */
		tmp_Rxcip = Rxcip;
		while (_Proc[_rx_proc].calltype==CT_INTERPRET)
			RxDoneInterStr();
		Rxcip = tmp_Rxcip;

		/* clear stack */
		RxStckTop = _Proc[_rx_proc].stacktop;
	}

	while (1) {
main_loop:

#ifdef __DEBUG__
	if (__debug__) {
		while (1) {
			cmd = getchar();
			switch (l2u[(byte)cmd]) {
				case 'M':
					printf("Memory allocated=%ld\n",
						mem_allocated());
					break;
				case 'S':
					if (RxStckTop<0)
						printf("Stack is empty\n");
					else
						for (i=RxStckTop; i>=0; i--) {
							printf("#%d: \"",i);
							Lprint(stdout,RxStck[i]);
							printf("\"\n");
						}
					break;
				case 'Q':
					goto interpreter_fin;
				case '\n':
					goto outofcmd;
			}
		}
outofcmd:
		printf("Stck:%d\t",RxStckTop+1);
		inst_ip = (size_t)((byte huge *)Rxcip - (byte huge *)Rxcodestart);
	}
	instr_cnt[(int)*Rxcip]++;
#endif
	switch (*(Rxcip++)) {
		/*
		 * [mnemonic] <type>[value]...
		 * type:	b = byte
		 *		w = word
		 *		p = pointer
		 */
		 		/* START A NEW COMMAND */
		case newclause_mn:
			DEBUGDISPLAY0("NEWCLAUSE");
			if (_trace) TraceClause();
			goto main_loop;

				/* POP = NO OPERATION	*/
		case nop_mn:
			DEBUGDISPLAY0("NOP");
			goto main_loop;

				/* PUSH p[lit]			*/
				/* push a litteral to stack	*/
		case push_mn:
			RxStckTop++;
			RxStck[RxStckTop] = (PLstr)(*(dword*)Rxcip);
			INCDWORD(Rxcip);
			CHKERROR;
			DEBUGDISPLAY("PUSH");
			goto chk4trace;

				/* PUSHTMP */
		case pushtmp_mn:
			RxStckTop++;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			CHKERROR;
			DEBUGDISPLAY0("PUSHTMP");
			goto main_loop;

				/* POP b[num]		*/
				/* pop NUM stack items	*/
		case pop_mn:
			DEBUGDISPLAY0("POP");
			RxStckTop -= *(Rxcip++);
			goto main_loop;

				/* DUP b[rel]			*/
				/* duplicate RELative stck item	*/
		case dup_mn:
			RxStckTop++;
			RxStck[RxStckTop] = RxStck[RxStckTop-*(Rxcip++)-1];
			CHKERROR;
			DEBUGDISPLAY("DUP");
			goto main_loop;

				/* COPY				*/
				/* copy (Lstrcpy) top item	*/
				/* to previous one		*/
		case copy_mn:
			DEBUGDISPLAY("COPY");
			Lstrcpy(RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto main_loop;

				/* COPY2TMP			*/
				/* if top item is not a pointer	*/
				/* to a tmp var then copy the	*/
				/* value to a tmp var		*/
		case copy2tmp_mn:
			/* copy to temporary only if different */
			if (RxStck[RxStckTop] != &(tmpstr[RxStckTop])) {
				Lstrcpy(&(tmpstr[RxStckTop]),RxStck[RxStckTop]);
				RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			}
			DEBUGDISPLAY("COPY2TMP");
			goto main_loop;

				/* PATCH w[rel] b[code]	*/
				/* patch CODE string to RELative pos with CODE */
		case patch_mn:
			w = *(CWORD *)Rxcip;	INCWORD(Rxcip);
			*(CIPTYPE*)((byte huge *)Rxcodestart + w) = *(Rxcip++);
			DEBUGDISPLAY0("PATCH");
			goto main_loop;

				/* RAISE b[cond] b[errno] b[subno]	*/
				/* raise an error condition		*/
		case raise_mn:
			errno = *(Rxcip++);
			subno = *(Rxcip++);
			DEBUGDISPLAY("RAISE");
			Lerror(errno,subno,RxStck[RxStckTop]);
			goto main_loop;

				/* LOADARG b[arg]		*/
				/* push an ARGument to stck	*/
		case loadarg_mn:
			INCSTACK;
			na = *(Rxcip++);	/* argument to push */
			if (_Proc[_rx_proc].arg.a[na])
				RxStck[RxStckTop] = _Proc[_rx_proc].arg.a[na];
			else {
				LZEROSTR(tmpstr[RxStckTop]);
				RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			}
			DEBUGDISPLAY("LOADARG");
			goto main_loop;


				/* LOADOPT [data]	*/
				/* load an option	*/
		case loadopt_mn:
			INCSTACK;
			nf = *(Rxcip++);	/* option to load */
/**
/// Maybe only pointer to Option!!!
**/
			RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			I_LoadOption(RxStck[RxStckTop],nf);
			DEBUGDISPLAY("LOADOPT");
			goto main_loop;

				/* STOREOPT [data]	*/
				/* store an option	*/
		case storeopt_mn:
			DEBUGDISPLAY("STOREOPT");
			nf = *(Rxcip++);	/* option to store */
			I_StoreOption(RxStck[RxStckTop],nf);
			RxStckTop--;
			goto main_loop;

				/* LOAD p[leaf]			*/
				/* push a VARiable to stck	*/
		case load_mn:
			INCSTACK;		/* make space	*/
			PLEAF(litleaf);		/* get variable ptr	*/
			DEBUGDISPLAYi("LOAD",&(litleaf->key));

			inf = (IdentInfo*)(litleaf->value);

			/* check to see if we have allready its position */
			if (inf->id == Rx_id) {
				leaf = inf->leaf[0];
				RxStck[RxStckTop] = LEAFVAL(leaf);
			} else {
				leaf = RxVarFind(VarScope, litleaf, &found);
				if (found)
					RxStck[RxStckTop] = LEAFVAL(leaf);
				else {
					if (inf->stem) {
						/* Lstrcpy to a temp variable */
						Lstrcpy(&(tmpstr[RxStckTop]),&stemvaluenotfound);
						RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
						if (leaf==NULL &&
				        		_Proc[_rx_proc].condition & SC_NOVALUE)
				        		RxSignalCondition(SC_NOVALUE);
					} else {
				        	if (_Proc[_rx_proc].condition & SC_NOVALUE)
				        		RxSignalCondition(SC_NOVALUE);
						RxStck[RxStckTop] = &(litleaf->key);
					}
				}
			}
			goto chk4trace;

				/* STORE p[leaf]				*/
				/* store top stck item to VARiable	*/
		case create_mn:		/* assigmnent	*/
			INCSTACK;
			PLEAF(litleaf);	/* Get pointer to variable */
			DEBUGDISPLAYi("CREATE",&(litleaf->key));

			inf = (IdentInfo*)(litleaf->value);
			if (inf->id == Rx_id) {
				leaf = inf->leaf[0];
				RxStck[RxStckTop] = LEAFVAL(leaf);
			} else {
				leaf = RxVarFind(VarScope,litleaf,&found);
			
				if (found)
					RxStck[RxStckTop] = LEAFVAL(leaf);
				else {
					leaf = RxVarAdd(VarScope,
						&(litleaf->key),
						inf->stem,
						leaf);
					RxStck[RxStckTop] = LEAFVAL(leaf);
					if (inf->stem==0) {
						inf->id = Rx_id;
						inf->leaf[0] = leaf;
					}
				} 

			}
			goto main_loop;

				/* DROP p[leaf]		*/
				/* drop VARiable	*/
		case drop_mn:
			PLEAF(litleaf);	/* Get pointer to variable */
			DEBUGDISPLAYi("DROP",&(litleaf->key));

			inf = (IdentInfo*)(litleaf->value);
			if (inf->id == Rx_id) {
				leaf = inf->leaf[0];
				RxVarDel(VarScope,litleaf,leaf);
			} else {
				leaf = RxVarFind(VarScope,litleaf,&found);
				if (found)
					RxVarDel(VarScope,litleaf,leaf);
			}
			inf->id = NO_CACHE;
			goto chk4trace;

				/* indirect drop, from stack	*/
				/* asssume that is UPPER case tmp */
		case dropind_mn:
			DEBUGDISPLAY("DROP_IND");
			RxVarDelInd(VarScope,RxStck[RxStckTop]);
			RxStckTop--;
			goto chk4trace;

		case assignstem_mn:
			PLEAF(litleaf);	/* Get pointer to stem */
			DEBUGDISPLAYi("ASSIGNSTEM",&(litleaf->key));
			inf = (IdentInfo*)(litleaf->value);
			if (inf->id == Rx_id) {
				leaf = inf->leaf[0];
				RxScopeAssign(leaf);
			} else {
				leaf = RxVarFind(VarScope,litleaf,&found);
				if (found)
					RxScopeAssign(leaf);
			}
			goto main_loop;

				/* BYINIT [patchpos]	*/
		case byinit_mn:
			w = *(CWORD *)Rxcip;	INCWORD(Rxcip);
			DEBUGDISPLAY("BYINIT");
			/* copy to temporary only if different */
			if (RxStck[RxStckTop] != &(tmpstr[RxStckTop])) {
				Lstrcpy(&(tmpstr[RxStckTop]),RxStck[RxStckTop]);
				RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			}
			/* patch comparision code */
			if (Llt(RxStck[RxStckTop],&(ZeroStr->key)))
				*(CIPTYPE*)((byte huge *)Rxcodestart + w) = tle_mn;
			else
				*(CIPTYPE*)((byte huge *)Rxcodestart + w) = tge_mn;
			goto main_loop;

				/* FORINIT		*/
				/* Initialise a FOR loop*/
		case forinit_mn:
			DEBUGDISPLAY("FORINIT");
			/* copy to temporary only if different */
			if (RxStck[RxStckTop] != &(tmpstr[RxStckTop])) {
				Lstrcpy(&(tmpstr[RxStckTop]),RxStck[RxStckTop]);
				RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			}
			L2INT(RxStck[RxStckTop]);	/* it is in temporary */
			if (Llt(RxStck[RxStckTop],&(ZeroStr->key)))
				Lerror(ERR_INVALID_INTEGER,3,RxStck[RxStckTop]);
			goto main_loop;

				/* DECFOR		*/
		case decfor_mn:
			DEBUGDISPLAY("DECFOR");
			a = RxStck[RxStckTop-*(Rxcip++)];
			if (Leq(a,&(ZeroStr->key)))
				Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart+ *(CWORD*)Rxcip);
			else
				INCWORD(Rxcip);
			Ldec(a);
			goto main_loop;

/**
//// Beware might lose something when number is like '   10   '
**/
				/* TOINT		*/
				/* change to integer	*/
		case toint_mn:
			DEBUGDISPLAY("TOINT");
			L2INT(RxStck[RxStckTop]);
			goto main_loop;

				/* LOWER 		*/
				/* upper top stack	*/
		case lower_mn:
			DEBUGDISPLAY("LOWER");
			Llower(RxStck[RxStckTop]);
			goto main_loop;

				/* UPPER 		*/
				/* upper top stack	*/
		case upper_mn:
			DEBUGDISPLAY("UPPER");
			Lupper(RxStck[RxStckTop]);
			goto main_loop;

				/* SIGNAL p[label]	*/
				/* clear stack and jmp to LABEL pos	*/
		case signal_mn:
			/* clear stack */
			RxStckTop = _Proc[_rx_proc].stacktop;

			/* check label */
			PLEAF(leaf);
			func = (RxFunc*)(leaf->value);
			DEBUGDISPLAYi("SIGNAL",&(leaf->key));

			if (func->label==UNKNOWN_LABEL)
				Lerror(ERR_UNEXISTENT_LABEL,1,&(leaf->key));
			/* jump */
			Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + (size_t)(func->label));
			goto main_loop;

				/* SIGNALVAL [address]	*/
				/* get address from stack */
		case signalval_mn:
			DEBUGDISPLAY("SIGNALEVAL");

			/* clear stack */
			RxStckTop = _Proc[_rx_proc].stacktop;

			/* search for label */
			L2STR(RxStck[RxStckTop]);
			leaf = BinFind(&_labels,RxStck[RxStckTop--]);
			if (leaf==NULL || ((RxFunc*)(leaf->value))->label == UNKNOWN_LABEL)
				Lerror(ERR_UNEXISTENT_LABEL,1,RxStck[RxStckTop+1]);
			func = (RxFunc*)(leaf->value);
			/* jump */
			Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + (size_t)(func->label));
			goto main_loop;

				/* JMP w[pos]				*/
				/* unconditional jump to POSition	*/
		case jmp_mn:
			DEBUGDISPLAY0nl("JMP");
			Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + *(CWORD *)Rxcip);
#ifdef __DEBUG__
			if (__debug__)
				printf("%d\n",(byte huge *)Rxcip-(byte huge *)Rxcodestart);
#endif
			goto main_loop;

				/* JF w[pos]			*/
				/* jump if top is 0 to POSition	*/
		case jf_mn:
			DEBUGDISPLAY0nl("JF");
#ifdef __DEBUG__
			if (__debug__) {
				w = *(CWORD *)Rxcip;
				if (!Lbool(RxStck[RxStckTop]))
					printf("%d *\n",w);
				else
					printf("%d\n",w);
			}
#endif
			if (!Lbool(RxStck[RxStckTop--]))
				Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart+ *(CWORD*)Rxcip);
			else
				INCWORD(Rxcip);
			goto main_loop;

				/* JT w[pos]			*/
				/* jump if top is 1 to POSition	*/
		case jt_mn:
			DEBUGDISPLAY0nl("JT");
#ifdef __DEBUG__
			if (__debug__) {
				w = *(CWORD *)Rxcip;
				if (Lbool(RxStck[RxStckTop]))
					printf("%d *\n",w);
				else
					printf("%d\n",w);
			}
#endif
			if (Lbool(RxStck[RxStckTop--]))
				Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart+ *(CWORD*)Rxcip);
			else
				INCWORD(Rxcip);
			goto main_loop;

				/* CALL p[label] b[noargs] w[existarg]	*/
				/* create new stack and jmp to LABEL pos*/
		case call_mn:
			DEBUGDISPLAY0nl("CALL");
			if (I_CallFunction())
				goto chk4trace;
			goto main_loop;

				/* RETURN			*/
				/* clear stack and return	*/
				/* if first prg then exit	*/
		case return_mn:
			DEBUGDISPLAY0("RETURN");
			if (_Proc[_rx_proc].calltype == CT_FUNCTION)
				Lerror(ERR_NO_DATA_RETURNED,0);
			if (_rx_proc==0) {	/* root program */
				RxReturnCode = 0;
				goto interpreter_fin;
			}
			I_ReturnProc();
			goto main_loop;

				/* RETURNF			*/
				/* move top of stack to correct */
				/* position of return arg and	*/
				/* clear stack			*/
		case returnf_mn:
			DEBUGDISPLAY0("RETURNF");
			if (_rx_proc==0) {	/* Root program */
				RxReturnCode = (int)Lrdint(RxStck[RxStckTop--]);
				goto interpreter_fin;
			} else
			if (_Proc[_rx_proc].calltype != CT_PROCEDURE)
/**
// It is possible to do a DUP in the compile code of returnf
**/
				Lstrcpy(_Proc[_rx_proc].arg.r, RxStck[RxStckTop]);
			else
				a = RxStck[RxStckTop];

			I_ReturnProc();

			if (_Proc[_rx_proc+1].calltype == CT_PROCEDURE)
				/* Assign the the RESULT variable */
				RxVarSet(VarScope,ResultStr,a);
			goto main_loop;

				/* INTERPRET [string] */
		case interpret_mn:
			DEBUGDISPLAY("INTERPRET");
			/* copy to a temporary var */
			if (RxStck[RxStckTop] != &(tmpstr[RxStckTop])) {
				Lstrcpy(&(tmpstr[RxStckTop]),RxStck[RxStckTop]);
				RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			}
			RxInitInterStr(RxStck[RxStckTop]);
			goto main_loop;

		case inter_end_mn:
			DEBUGDISPLAY0("INTER_END");
			RxDoneInterStr();
			goto main_loop;

				/* PROC */
		case proc_mn:
			DEBUGDISPLAY0("ERROR-PROC");
			Lerror(ERR_UNEXPECTED_PROC,1);
			goto chk4trace;

				/* SAY			*/
				/* display TOP item	*/
		case say_mn:
			DEBUGDISPLAY("SAY");
			Lprint(stdout,RxStck[RxStckTop--]);
			putchar('\n');
			goto main_loop;

				/* SYSTEM		*/
				/* execute a system call*/
		case system_mn:
			DEBUGDISPLAY2("SYSTEM");
			L2STR(RxStck[RxStckTop]);
			LASCIIZ(*(RxStck[RxStckTop]));
			RxExecuteCmd(RxStck[RxStckTop],RxStck[RxStckTop-1]);
			RxStckTop -= 2;
			goto main_loop;

				/* EXIT			*/
				/* exit prg with RC	*/
		case exit_mn:
			DEBUGDISPLAY("EXIT");
			RxReturnCode = (int)Lrdint(RxStck[RxStckTop--]);
			/* free everything from stack */
#ifndef __DEBUG__
			RxStckTop = -1;
#endif
			goto interpreter_fin;

				/* PARSE		*/
				/* Initialise PARSING	*/
		case parse_mn:
			DEBUGDISPLAY("PARSE");
			/* Do not remove from stack */
			ToParse = RxStck[RxStckTop];
			L2STR(ToParse);
			DataStart = BreakStart = BreakEnd = 1;
			SourceEnd = LLEN(*ToParse)+1;
			goto main_loop;

				/* PVAR			*/
				/* Parse to stack	*/
		case pvar_mn:
			DEBUGDISPLAY("PVAR");
			if (BreakEnd<=DataStart)
				DataEnd = SourceEnd;
			else
				DataEnd = BreakStart;

			if (DataEnd!=DataStart)
				_Lsubstr(RxStck[RxStckTop--],ToParse,DataStart,DataEnd-DataStart);
			else {
				LZEROSTR(*(RxStck[RxStckTop]));
				RxStckTop--;
			}
			if (_trace) {
				RxStckTop++;
				TraceInstruction(*Rxcip);
				RxStckTop--;
			}
			Rxcip++;
			goto main_loop;
			/*** goto chk4trace; ***/

				/* PDOT			*/
				/* Parse to hyperspace	*/
		case pdot_mn:
			/* Only for debugging */
			DEBUGDISPLAY0("PDOT");
			if (_trace) {
				/* Make space	*/
				RxStckTop++;
				RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
				if (BreakEnd<=DataStart)
					DataEnd = SourceEnd;
				else
					DataEnd = BreakStart;
				if (DataEnd!=DataStart)
					_Lsubstr(RxStck[RxStckTop],ToParse,DataStart,DataEnd-DataStart);
				else
					LZEROSTR(*(RxStck[RxStckTop]));
				TraceInstruction(*Rxcip);
				RxStckTop--;	/* free space */
			}
			Rxcip++;
			goto main_loop;

				/* TR_SPACE		*/
				/* trigger a space	*/
		case tr_space_mn:
			DEBUGDISPLAY0("TR_SPACE");
			I_trigger_space();
			goto main_loop;

				/* TR_LIT			*/
				/* trigger a litteral from stck	*/
		case tr_lit_mn:
			DEBUGDISPLAY("TR_LIT");
			DataStart = BreakEnd;
			I_trigger_litteral(RxStck[RxStckTop--]);
			goto main_loop;

				/* TR_ABS			*/
				/* trigger ABSolute position	*/
		case tr_abs_mn:
			DEBUGDISPLAY("TR_ABS");
/**
//			L2INT(**A);
**/
			DataStart = BreakEnd;
			BreakStart = (size_t)LINT(*(RxStck[RxStckTop--]));

			/* check for boundaries */
			BreakStart = RANGE(1,BreakStart,SourceEnd);
			BreakEnd = BreakStart;
			goto main_loop;

				/* TR_REL			*/
				/* trigger RELative position	*/
		case tr_rel_mn:
			DEBUGDISPLAY("TR_REL");

/**
//			L2INT(**A);
**/
			DataStart = BreakStart;
			BreakStart = DataStart + (size_t)LINT(*(RxStck[RxStckTop--]));

			/* check for boundaries */
			BreakStart = RANGE(1,BreakStart,SourceEnd);
			BreakEnd = BreakStart;
			goto main_loop;

				/* TR_END			*/
				/* trigger to END of data	*/
		case tr_end_mn:
			DEBUGDISPLAY0("TR_END");
			DataStart = BreakEnd;
			BreakStart = SourceEnd;
			BreakEnd = SourceEnd;
			goto main_loop;

				/* RX_QUEUE			*/
				/* queue stck to Rexx queue	*/
		case rx_queue_mn:
			DEBUGDISPLAY("RX_PUSH");
			LPMALLOC(a);	/* duplicate variable */
			Lfx(a,1);
			Lstrcpy(a,RxStck[RxStckTop--]);
			Queue2Stack(a);
			goto main_loop;

				/* RX_PUSH			*/
				/* push stck to Rexx queue	*/
		case rx_push_mn:
			DEBUGDISPLAY("RX_PUSH");
			LPMALLOC(a);	/* duplicate variable */
			Lfx(a,1);
			Lstrcpy(a,RxStck[RxStckTop--]);
			Push2Stack(a);
			goto main_loop;

				/* RX_PULL			*/
				/* pull stck from Rexx queue	*/
		case rx_pull_mn:
			RxStckTop++;
			RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			a = NULL;
			/* delete empty stacks */
			while (StackQueued()==0 && StackList.items>1)
				DeleteStack();
			if (StackQueued()>0) {
				a = PullFromStack();
				Lstrcpy(RxStck[RxStckTop],a);
				LPFREE(a);
				while (StackQueued()==0 && StackList.items>1)
					DeleteStack();
			} else {
				Lread(stdin,RxStck[RxStckTop],LREADLINE);
			}
			DEBUGDISPLAY("RX_PULL");
			goto main_loop;

				/* RX_EXTERNAL			*/
				/* read data from extrnal queue	*/
		case rx_external_mn:
			RxStckTop++;
			RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			Lread(stdin,RxStck[RxStckTop],LREADLINE);
			DEBUGDISPLAY("RX_EXTERNAL");
			goto main_loop;

		case eq_mn:
			DEBUGDISPLAY2("EQ");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Leq(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case ne_mn:
			DEBUGDISPLAY2("NE");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Lne(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case gt_mn:
			DEBUGDISPLAY2("GT");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Lgt(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case ge_mn:
			DEBUGDISPLAY2("GE");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Lge(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case lt_mn:
			DEBUGDISPLAY2("LT");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Llt(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case le_mn:
			DEBUGDISPLAY2("LE");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Lle(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case deq_mn:
			DEBUGDISPLAY2("DEQ");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Ldeq(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case dne_mn:
			DEBUGDISPLAY2("DNE");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Ldne(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case dgt_mn:
			DEBUGDISPLAY2("DGT");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Ldgt(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case dge_mn:
			DEBUGDISPLAY2("DGE");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Ldge(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case dlt_mn:
			DEBUGDISPLAY2("DLT");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Ldlt(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case dle_mn:
			DEBUGDISPLAY2("DLE");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Ldle(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case teq_mn:
			DEBUGDISPLAY2("TEQ");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Leq(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case tne_mn:
			DEBUGDISPLAY2("TNE");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Lne(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case tdeq_mn:
			DEBUGDISPLAY2("TDEQ");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Ldeq(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case tdne_mn:
			DEBUGDISPLAY2("TNDE");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Ldne(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case tgt_mn:
			DEBUGDISPLAY2("TGT");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Lgt(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case tge_mn:
			DEBUGDISPLAY2("TGE");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Lge(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case tlt_mn:
			DEBUGDISPLAY2("TLT");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Llt(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case tle_mn:
			DEBUGDISPLAY2("TLE");
			a = &(tmpstr[RxStckTop-1]);
			LICPY(*a,
				Lle(RxStck[RxStckTop-1],RxStck[RxStckTop]));
			RxStckTop--;
			RxStck[RxStckTop] = &tmpstr[RxStckTop];
			goto chk4trace;

		case not_mn:
			DEBUGDISPLAY("NOT");
			a = RxStck[RxStckTop-1];
			LICPY(*a,!Lbool(RxStck[RxStckTop]));
			RxStckTop--;
			goto chk4trace;

		case and_mn:
			DEBUGDISPLAY2("AND");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Lbool(RxStck[RxStckTop-1]) & Lbool(RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case or_mn:
			DEBUGDISPLAY2("OR");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Lbool(RxStck[RxStckTop-1]) | Lbool(RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case xor_mn:
			DEBUGDISPLAY2("XOR");
			a = RxStck[RxStckTop-2];
			LICPY(*a,
				Lbool(RxStck[RxStckTop-1]) ^ Lbool(RxStck[RxStckTop]));
			RxStckTop -= 2;
			goto chk4trace;

		case concat_mn:
			DEBUGDISPLAY2("CONCAT");
			a = RxStck[RxStckTop-2];
			if (a!=RxStck[RxStckTop]) {
				Lstrcpy(a,RxStck[RxStckTop-1]);
				Lstrcat(a,RxStck[RxStckTop]);
			} else {
				Lstrcpy(&(tmpstr[RxStckTop]),RxStck[RxStckTop]);
				Lstrcpy(a,RxStck[RxStckTop-1]);
				Lstrcat(a,&(tmpstr[RxStckTop]));
			}
			RxStckTop -= 2;
			goto chk4trace;

		case bconcat_mn:
			DEBUGDISPLAY2("BCONCAT");
			a = RxStck[RxStckTop-2];
			if (a==RxStck[RxStckTop]) {
				Lstrcpy(&(tmpstr[RxStckTop]),RxStck[RxStckTop]);
				RxStck[RxStckTop] = &(tmpstr[RxStckTop]);
			}
			Lstrcpy(a,RxStck[RxStckTop-1]);
			L2STR(a);
			LSTR(*a)[LLEN(*a)] = ' ';
			LLEN(*a)++;
			Lstrcat(a,RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		case neg_mn:
			DEBUGDISPLAY("NEG");
			Lneg(RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop--;
			goto chk4trace;

		case inc_mn:
			DEBUGDISPLAY("INC");
			Linc(RxStck[RxStckTop--]);
			goto chk4trace;

		case dec_mn:
			DEBUGDISPLAY("DEC");
			Ldec(RxStck[RxStckTop--]);
			goto chk4trace;

		case add_mn:
			DEBUGDISPLAY2("ADD");
			Ladd(RxStck[RxStckTop-2],
				RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		case sub_mn:
			DEBUGDISPLAY2("SUB");
			Lsub(RxStck[RxStckTop-2],
				RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		case mul_mn:
			DEBUGDISPLAY2("MUL");
			Lmult(RxStck[RxStckTop-2],
				RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		case div_mn:
			DEBUGDISPLAY2("DIV");
			Ldiv(RxStck[RxStckTop-2],
				RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		case idiv_mn:
			DEBUGDISPLAY2("IDIV");
			Lintdiv(RxStck[RxStckTop-2],
				RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		case mod_mn:
			DEBUGDISPLAY2("MOD");
			Lmod(RxStck[RxStckTop-2],
				RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		case pow_mn:
			DEBUGDISPLAY2("POW");
			Lexpose(RxStck[RxStckTop-2],
				RxStck[RxStckTop-1],RxStck[RxStckTop]);
			RxStckTop -= 2;
			goto chk4trace;

		default:
			DEBUGDISPLAY0("error, unknown mnemonic");
			Rxcip--;
			printf("Opcode found=%d (0x%02X)\n",*Rxcip,*Rxcip);
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
chk4trace:
	if (_trace)
			TraceInstruction(*Rxcip);
	Rxcip++;	/* skip trace byte */
	}
interpreter_fin:
	signal(SIGINT,SIG_IGN);
	return RxReturnCode;
} /* RxInterpret */
