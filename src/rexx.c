/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/rexx.c,v 1.2 1999/11/26 13:13:47 bnv Exp $
 * $Log: rexx.c,v $
 * Revision 1.2  1999/11/26 13:13:47  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#define __REXX_C__

#include <string.h>
#include <setjmp.h>

#include <bmem.h>
#include <lerror.h>
#include <lstring.h>

#include <rexx.h>
#include <stack.h>
#include <trace.h>
#include <bintree.h>
#include <compile.h>
#include <interpre.h>
#include <nextsymb.h>

/* ----------- Function prototypes ------------ */
void	Rerror(int,int,...);
void    RxInitFiles(void);
void    RxDoneFiles(void);
void	RxRegFunctionDone(void);

/* ----------- External variables ------------- */
extern Lstr	errmsg;

/* ---------------- RxInitProc ---------------- */
static void
RxInitProc( void )
{
	_rx_proc = -1;
	_Proc_size = PROC_INC;
	_Proc = (RxProc*) MALLOC( _Proc_size * sizeof(RxProc), "RxProc" );
	MEMSET(_Proc,0,_Proc_size*sizeof(RxProc));
} /* RxInitProc */

/* ----------------- RxInitialize ----------------- */
void
RxInitialize( char *prorgram_name )
{
	Lstr	str;

	_prgname = prorgram_name;
#if defined(WIN32) || defined(WCE)
	_szRxAppKey = REGAPPKEY;
#endif

	LINITSTR(str);

	/* do the basic initialisation */
	Linit(Rerror);		/* initialise with Lstderr as error function */
	LINITSTR(symbolstr);
		Lfx(&symbolstr,250);	/* create symbol string */
	LINITSTR(errmsg);
		Lfx(&errmsg,250);	/* create error message string */

	/* --- first locate configuration file --- */
	/* rexx.rc for DOS in the executable program directory */
	/* .rexxrc for unix in the HOME directory */

	_procidcnt = 1;		/* Program id counter	*/

	DQINIT(StackList);	/* initialise stacks	*/
	CreateStack();		/* create first stack	*/
	rxfile = NULL;		/* intialise rexx files	*/
	LPMALLOC(_code);
	CompileClause = NULL;

	RxInitProc();		/* initialize prg list	*/
	RxInitInterpret();	/* initialise interpreter*/
	RxInitFiles();		/* initialise files	*/
	RxInitVariables();	/* initialise hash table for variables	*/

	BINTREEINIT(_labels);	/* initialise labels	*/
	BINTREEINIT(Litterals);	/* initialise litterals	*/

		Lscpy(&str,"HALT");	HaltStr   = _Add2Lits( &str, FALSE );

		Lscpy(&str,"1");	OneStr    = _Add2Lits( &str, FALSE );
		Lscpy(&str,"");		NullStr   = _Add2Lits( &str, FALSE );
		Lscpy(&str,"0");	ZeroStr   = _Add2Lits( &str, FALSE );
		Lscpy(&str,"ERROR");	ErrorStr  = _Add2Lits( &str, FALSE );

		Lscpy(&str,"RESULT");	ResultStr = _Add2Lits( &str, FALSE );
		Lscpy(&str,"NOVALUE");	NoValueStr= _Add2Lits( &str, FALSE );
		Lscpy(&str,"NOTREADY");NotReadyStr= _Add2Lits( &str, FALSE );
		Lscpy(&str,"SIGL");	SiglStr   = _Add2Lits( &str, FALSE );
		Lscpy(&str,"RC");	RCStr     = _Add2Lits( &str, FALSE );
		Lscpy(&str,"SYNTAX");	SyntaxStr = _Add2Lits( &str, FALSE );
		Lscpy(&str,"SYSTEM");	SystemStr = _Add2Lits( &str, FALSE );

	LFREESTR(str);
} /* RxInitialize */

/* ----------------- RxFinalize ----------------- */
void
RxFinalize( void )
{
	LFREESTR(symbolstr);	/* delete symbol string	*/
	LFREESTR(errmsg);	/* delete error msg str	*/
	RxDoneInterpret();
	FREE(_Proc);		/* free prg list	*/
	while (StackList.items>0) DeleteStack();
	LPFREE(_code);	_code = NULL;

	RxDoneFiles();		/* close all files	*/

		/* will free also NullStr, Zero and OneStr	*/
	BinDisposeLeaf(&Litterals,Litterals.parent,FREE);
	BinDisposeLeaf(&_labels,_labels.parent,FREE);
	RxDoneVariables();
	RxRegFunctionDone();	/* initialise register functions 	*/
} /* RxFinalize */

/* ----------------- RxLoadFile ------------------- */
int
RxLoadFile( RxFile *rxf )
{
	FILEP f;

	if ((f=FOPEN(LSTR(rxf->filename),"r"))==NULL)
		return FALSE;
	Lread(f,&(rxf->file), LREADFILE);
	FCLOSE(f);
	return TRUE;
} /* RxLoadFile */

/* ----------------- RxRun ------------------ */
int
RxRun( char *filename, PLstr programstr,
	PLstr arguments, PLstr tracestr, char *environment )
{
	RxFile	*rxf;
	RxProc	*pr;
	char	*c;
	int	i;

	/* --- set exit jmp position --- */
	if ((i=setjmp(_exit_trap))!=0)
		goto run_exit;
	/* --- set temporary error trap --- */
	if (setjmp(_error_trap)!=0)
		return RxReturnCode;

	/* ====== first load the file ====== */
	rxfile = (RxFile*)MALLOC(sizeof(RxFile),"RxFile");
	MEMSET(rxfile,0,sizeof(RxFile));

	if (filename) {
		/* --- copy the filename --- */
		Lscpy(&(rxfile->filename), filename);
		LASCIIZ(rxfile->filename);

		/* find file type */
		rxfile->filetype = NULL;
		c = LSTR(rxfile->filename)+LLEN(rxfile->filename);
		for (;c>LSTR(rxfile->filename) && *c!='.';c--) ;;
		if (*c=='.')
			rxfile->filetype = c+1;

		/* --- Load file --- */
		if (!RxLoadFile( rxfile )) {
#ifndef WCE
			fprintf(STDERR,"Error %d running \"%s\": File not found\n",
					ERR_FILE_NOT_FOUND, LSTR(rxfile->filename));
#else
			PUTS("Error: File not found.");
#endif
			LFREESTR(rxfile->filename);
			FREE(rxfile);
			return 1;
		}
	} else {
		Lscpy(&(rxfile->filename), "<STDIN>");
		rxfile->filetype = NULL;
		LASCIIZ(rxfile->filename);
		Lstrcpy(&(rxfile->file), programstr);
	}
	LASCIIZ(rxfile->file);

#ifdef __DEBUG__
	if (__debug__) {
		printf("File is:\n%s\n",LSTR(rxfile->file));
		getchar();
	}
#endif

	/* ====== setup procedure ====== */
	_rx_proc++;		/* increase program items	*/
	pr = _Proc+_rx_proc;	/* pr = Proc pointer		*/

	/* set program id counter */
	pr->id = _procidcnt++;

	/* --- initialise Proc structure --- */
				/* arguments...		*/
	pr->arg.n = 0;
	for (i=0; i<MAXARGS; i++)
		pr->arg.a[i] = NULL;
	pr->arg.r = NULL;
	if (LLEN(*arguments)) {
		pr->arg.n = 1;
		pr->arg.a[0] = arguments;
	} else
		pr->arg.n = 0;

	pr->calltype = CT_PROGRAM;	/* call type...		*/
	pr->ip = 0;			/* procedure ip		*/
	pr->stack = -1;		/* prg stck, will be set in interpret	*/
	pr->stacktop = -1;		/* no arguments		*/

	pr->scope = RxScopeMalloc();
	LPMALLOC(pr->env);
	if (environment)
		Lscpy(pr->env,environment);
	else
		Lstrcpy(pr->env,&(SystemStr->key));
	pr->digits = LMAXNUMERICDIGITS;
	pr->fuzz = 0;
	pr->form = SCIENTIFIC;
	pr->condition = 0;
	pr->lbl_error    = &(ErrorStr->key);
	pr->lbl_halt     = &(HaltStr->key);
	pr->lbl_novalue  = &(NoValueStr->key);
	pr->lbl_notready = &(NotReadyStr->key);
	pr->lbl_syntax   = &(SyntaxStr->key);
	pr->codelen = 0;
	pr->trace = normal_trace;
	pr->interactive_trace = FALSE;
	if (tracestr && LLEN(*tracestr)) TraceSet(tracestr);

	/* ======= Compile file ====== */
	RxInitCompile(rxfile,NULL);
	RxCompile();

#ifdef __DEBUG__
	if (__debug__) {
		printf("Litterals are:\n");
		BinPrint(Litterals.parent);
		getchar();

		printf("Labels(&functions) are:\n");
		BinPrint(_labels.parent);
		printf("Code Size: %d\n\n",LLEN(*_code));
		getchar();
	}
#endif

	/* ======= Execute code ======== */
	if (!RxReturnCode)
		RxInterpret();

run_exit:
		/* pr pointer might have changed if Proc was resized */
	pr = _Proc+_rx_proc;
#ifdef __DEBUG__
	if (__debug__)
		printf("Return Code = %d\n",RxReturnCode);
#endif

	/* ======== free up memory ======== */
	while (rxfile) {
		rxf = rxfile;
		rxfile = rxfile->next;
		LFREESTR(rxf->filename);
		LFREESTR(rxf->file);
		FREE(rxf);
	}

	LPFREE(pr->env);
	if (CompileClause) {
		FREE(CompileClause);
		CompileClause = NULL;
	}

	RxScopeFree(pr->scope);
	FREE(pr->scope);
	_rx_proc--;

	return RxReturnCode;
} /* RxRun */
