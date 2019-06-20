/*
 * $Id: rexx.c,v 1.18 2013/09/03 20:02:44 bnv Exp $
 * $Log: rexx.c,v $
 * Revision 1.18  2013/09/03 20:02:44  bnv
 * add (char*) cast in dlerror()
 *
 * Revision 1.17  2011/06/29 08:32:25  bnv
 * Corrected dlopen for android
 *
 * Revision 1.16  2011/06/20 08:31:57  bnv
 * removed the FCVT and GCVT replaced with sprintf
 *
 * Revision 1.15  2011/05/17 06:53:10  bnv
 * Added SQLite
 *
 * Revision 1.14  2010/01/27 13:20:11  bnv
 * Shared library correction
 *
 * Revision 1.13  2009/09/14 14:00:56  bnv
 * __DEBUG__ format correction
 *
 * Revision 1.12  2009/06/30 13:51:40  bnv
 * Added -a option to break arg into words
 *
 * Revision 1.11  2008/07/15 14:57:55  bnv
 * mvs corrections
 *
 * Revision 1.10  2008/07/15 07:40:25  bnv
 * #include changed from <> to ""
 *
 * Revision 1.9  2008/07/14 13:08:42  bnv
 * MVS,CMS support
 *
 * Revision 1.8  2006/01/26 10:25:52  bnv
 * Windows CE
 *
 * Revision 1.7  2004/08/16 15:28:54  bnv
 * Changed: name of mnemonic operands from xxx_mn to O_XXX
 *
 * Revision 1.6  2003/10/30 13:16:28  bnv
 * Variable name change
 *
 * Revision 1.5  2002/06/11 12:37:38  bnv
 * Added: CDECL
 *
 * Revision 1.4  2002/06/06 08:24:36  bnv
 * Corrected: Registry support for WCE
 *
 * Revision 1.3  2001/06/25 18:51:48  bnv
 * Header -> Id
 *
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

#include "lerror.h"
#include "lstring.h"

#include "rexx.h"
#include "stack.h"
#include "trace.h"
#include "bintree.h"
#include "compile.h"
#include "interpre.h"
#include "nextsymb.h"

#if defined(UNIX)
#	include <dlfcn.h>
#elif defined(WCE)
#	include <winfunc.h>
#endif

/* ----------- Function prototypes ------------ */
void	__CDECL Rerror(const int,const int,...);
void    __CDECL RxInitFiles(void);
void    __CDECL RxDoneFiles(void);
void	__CDECL RxRegFunctionDone(void);

/* ----------- External variables ------------- */
extern Lstr	errmsg;

/* ---------------- RxInitProc ---------------- */
static void
RxInitProc( void )
{
	_rx_proc = -1;
	_proc_size = PROC_INC;
	_proc = (RxProc*) MALLOC( _proc_size * sizeof(RxProc), "RxProc" );
	MEMSET(_proc,0,_proc_size*sizeof(RxProc));
} /* RxInitProc */

/* ----------------- RxInitialize ----------------- */
void __CDECL
RxInitialize( char *prorgram_name )
{
	Lstr	str;

	_prgname = prorgram_name;

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

	DQINIT(rxStackList);	/* initialise stacks	*/
	CreateStack();		/* create first stack	*/
	rxFileList = NULL;	/* intialise rexx files	*/
	LPMALLOC(_code);
	CompileClause = NULL;

	RxInitProc();		/* initialize prg list	*/
	RxInitInterpret();	/* initialise interpreter*/
	RxInitFiles();		/* initialise files	*/
	RxInitVariables();	/* initialise hash table for variables	*/

	BINTREEINIT(_labels);	/* initialise labels	*/
	BINTREEINIT(rxLitterals);	/* initialise litterals	*/

		Lscpy(&str,"HALT");	haltStr   = _Add2Lits( &str, FALSE );

		Lscpy(&str,"1");	oneStr    = _Add2Lits( &str, FALSE );
		Lscpy(&str,"");		nullStr   = _Add2Lits( &str, FALSE );
		Lscpy(&str,"0");	zeroStr   = _Add2Lits( &str, FALSE );
		Lscpy(&str,"ERROR");	errorStr  = _Add2Lits( &str, FALSE );

		Lscpy(&str,"RESULT");	resultStr = _Add2Lits( &str, FALSE );
		Lscpy(&str,"NOVALUE");	noValueStr= _Add2Lits( &str, FALSE );
		Lscpy(&str,"NOTREADY");	notReadyStr= _Add2Lits( &str, FALSE );
		Lscpy(&str,"SIGL");	siglStr   = _Add2Lits( &str, FALSE );
		Lscpy(&str,"RC");	RCStr     = _Add2Lits( &str, FALSE );
		Lscpy(&str,"SYNTAX");	syntaxStr = _Add2Lits( &str, FALSE );
		Lscpy(&str,"SYSTEM");	systemStr = _Add2Lits( &str, FALSE );

	LFREESTR(str);
} /* RxInitialize */

/* ----------------- RxFinalize ----------------- */
void __CDECL
RxFinalize( void )
{
	LFREESTR(symbolstr);	/* delete symbol string	*/
	LFREESTR(errmsg);	/* delete error msg str	*/
	RxDoneInterpret();
	FREE(_proc);		/* free prg list	*/
	while (rxStackList.items>0) DeleteStack();
	LPFREE(_code);	_code = NULL;

	RxDoneFiles();		/* close all files	*/

		/* will free also nullStr, zeroStr and oneStr	*/
	BinDisposeLeaf(&rxLitterals,rxLitterals.parent,FREE);
	BinDisposeLeaf(&_labels,_labels.parent,FREE);
	RxDoneVariables();
	RxRegFunctionDone();	/* initialise register functions	*/
} /* RxFinalize */

/* ----------------- RxFileAlloc ------------------- */
RxFile* __CDECL
RxFileAlloc(char *fname)
{
	RxFile	*rxf;

	rxf = (RxFile*)MALLOC(sizeof(RxFile),"RxFile");
	if (rxf==NULL)
		return rxf;
	MEMSET(rxf,0,sizeof(RxFile));
	Lscpy(&(rxf->name), fname);
	LASCIIZ(rxf->name);

	return rxf;
} /* RxFileAlloc */

/* ----------------- RxFileType ------------------- */
void __CDECL
RxFileType(RxFile *rxf)
{
	unsigned char *c;

	/* find file type */
	c = LSTR(rxf->name)+LLEN(rxf->name);
	for (;c>LSTR(rxf->name) && *c!='.';c--) ;;
	if (*c=='.')
		rxf->filetype = c;
	for (;c>LSTR(rxf->name) && *c!=FILESEP;c--) ;;
	if (c>LSTR(rxf->name))
		c++;
	rxf->filename = c;
} /* RxFileType */

/* ----------------- RxFileFree ------------------- */
void __CDECL
RxFileFree(RxFile *rxf)
{
	RxFile *f;

	while (rxf) {
		f = rxf;
		rxf = rxf->next;
		LFREESTR(f->name);
		LFREESTR(f->file);
#if defined(__GNUC__) && !defined(MSDOS) && !defined(__CMS__) \
    && !defined(__MVS__)
		if (f->libHandle!=NULL)
			dlclose(f->libHandle);
#endif
		FREE(f);
	}
} /* RxFileFree */

/* ----------------- RxFileLoad ------------------- */
int __CDECL
RxFileLoad( RxFile *rxf )
{
	FILEP f;
	if ((f=FOPEN(LSTR(rxf->name),"r"))==NULL)
		return FALSE;
	Lread(f,&(rxf->file), LREADFILE);
	FCLOSE(f);
	return TRUE;
} /* RxFileLoad */

/* --- _LoadRexxLibrary --- */
static jmp_buf	old_trap;
static int
_LoadRexxLibrary(RxFile *rxf, PLstr libname)
{
	size_t	ip;
	char	*start, *stop;
	Lstr	rxlib_path;
#ifndef WCE
	char	*rxlib;
#else
	TCHAR	pathvalue[128];
	DWORD	pathlen;
#endif

	if (RxFileLoad( rxf ))
		goto FILEFOUND;

	/* let's try at the directory of rxlib */
	LINITSTR(rxlib_path);

#ifndef WCE
	if ((rxlib=getenv("RXLIB"))!=NULL) {
		Lscpy(&rxlib_path,rxlib);
#else
	pathlen = sizeof(pathvalue);
	if (RXREGGETDATA(TEXT("LIB"),REG_SZ,(LPBYTE)pathvalue,&pathlen)) {
		Lwscpy(&rxlib_path,pathvalue);
#endif
		LASCIIZ(rxlib_path);
		start = LSTR(rxlib_path);
		while (start!=NULL && *start) {
			// Find first directory
			stop = STRCHR(start,PATHSEP);
			if (stop!=NULL)
				*stop++='\0';
			Lscpy(&(rxf->name),start);

			ip = LLEN(rxf->name);
			if (LSTR(rxf->name)[ip-1] != FILESEP) {
				LSTR(rxf->name)[ip] = FILESEP;
				LLEN(rxf->name)++;
			}
			Lstrcat(&(rxf->name),libname);
			LASCIIZ(rxf->name);
			if (RxFileLoad( rxf )) {
				LFREESTR( rxlib_path );
				goto FILEFOUND;
			}
			start = stop;
		}
	}
	LFREESTR( rxlib_path );
	return 1;

FILEFOUND:
	ip = (size_t)((byte huge *)Rxcip - (byte huge *)Rxcodestart);
	MEMCPY(old_trap,_error_trap,sizeof(_error_trap));
	RxFileType(rxf);
	RxInitCompile(rxf,NULL);
	RxCompile();

	/* restore state */
	MEMCPY(_error_trap,old_trap,sizeof(_error_trap));
	Rxcodestart = (CIPTYPE*)LSTR(*_code);
	Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + ip);
	if (rxReturnCode)
		RxSignalCondition(SC_SYNTAX);
	return 0;
} /* _LoadRexxLibrary */

/* ----------------- RxLoadLibrary ------------------- */
int __CDECL
RxLoadLibrary( PLstr libname, bool shared )
{
	RxFile  *rxf, *last;
#if defined(UNIX) || defined(__CMS__) || defined(__MVS__) || defined(__APPLE__)
	Lstr	tmpstr;
	char	*dlerrorstr;
#endif

	/* Convert to ASCIIZ */
	L2STR(libname); LASCIIZ(*libname);

	/* check to see if it is already loaded */
	for (rxf = rxFileList; rxf != NULL; rxf = rxf->next)
		if (!strcmp(rxf->filename,LSTR(*libname)))
			return -1;

	/* create  a RxFile structure */
	rxf = RxFileAlloc(LSTR(*libname));

#if defined(__GNUC__) && !defined(MSDOS) && !defined(WCE)
	if (shared) {
		/* try to load it as a shared library */
#	if !defined(__CMS__) && !defined(__MVS__)
		rxf->libHandle = dlopen(LSTR(rxf->name),RTLD_NOW);
		dlerrorstr = (char*)dlerror();
#	else
		rxf->libHandle = NULL;
		dlerrorstr = NULL;
#	endif
		if (rxf->libHandle!=NULL) {
			/* load the main function and execute it...*/
			RxFileType(rxf);
			goto LIB_LOADED;
		}

		/* Unfortunatelly we have to handle errors with strings.
		 * Skip the errors when trying to load a rexx file instead
		 * of a dll-library */
		if (dlerrorstr != NULL) {
			/*printf("ERROR=%s\n",dlerrorstr);*/
			if (STRSTR(dlerrorstr,"invalid ELF header") &&
			    STRSTR(dlerrorstr,"cannot open shared object file") &&
			    STRSTR(dlerrorstr,"Win32 error ") &&
			    STRSTR(dlerrorstr,"Cannot load library:")) {
				LMKCONST(tmpstr,dlerrorstr);
				Lerror(ERR_LIBRARY,0,&tmpstr);
			}
		}
	}
#endif

	/* try to load the file as rexx library */
	if (_LoadRexxLibrary(rxf,libname)) {
		RxFileFree(rxf);
		return 1;
	}

#if defined(UNIX) || defined(__CMS__) || defined(__MVS__)
LIB_LOADED:
#endif
	/* find the last in the queue */
	for (last = rxFileList; last->next != NULL; )
		last = last->next;
	last->next = rxf;
	return 0;
} /* RxLoadLibrary */

/* ----------------- RxRun ------------------ */
int __CDECL
RxRun( char *filename, PLstr programstr,
	PLstr arguments, PLstr tracestr, char *environment )
{
	RxProc	*pr;
	int	i;

	/* --- set exit jmp position --- */
	if ((i=setjmp(_exit_trap))!=0)
		goto run_exit;
	/* --- set temporary error trap --- */
	if (setjmp(_error_trap)!=0)
		return rxReturnCode;

	/* ====== first load the file ====== */
	if (filename) {
		rxFileList = RxFileAlloc(filename);

		/* --- Load file --- */
		if (!RxFileLoad( rxFileList )) {
#ifndef WCE
			fprintf(STDERR,"Error %d running \"%s\": File not found\n",
					ERR_FILE_NOT_FOUND, LSTR(rxFileList->name));
#else
			PUTS("Error: File not found.");
#endif
			RxFileFree(rxFileList);
			return 1;
		}
	} else {
		rxFileList = RxFileAlloc("<STDIN>");
		Lfx(&(rxFileList->file), LLEN(*programstr));
		Lstrcpy(&(rxFileList->file), programstr);
	}
	RxFileType(rxFileList);
	LASCIIZ(rxFileList->file);

#ifdef __DEBUG__
	if (__debug__) {
		printf("File is:\n%s\n",LSTR(rxFileList->file));
		getchar();
	}
#endif

	/* ====== setup procedure ====== */
	_rx_proc++;		/* increase program items	*/
	pr = _proc+_rx_proc;	/* pr = Proc pointer		*/

	/* set program id counter */
	pr->id = _procidcnt++;

	/* --- initialise Proc structure --- */
				/* arguments...		*/
	pr->arg.n = 0;
	for (i=0; i<MAXARGS; i++) {
		if (LLEN(arguments[i])) {
			pr->arg.n = i+1;
			pr->arg.a[i] = &(arguments[i]);
		} else
			pr->arg.a[i] = NULL;
	}
	pr->arg.r = NULL;

	pr->calltype = CT_PROGRAM;	/* call type...		*/
	pr->ip = 0;			/* procedure ip		*/
	pr->stack = -1;		/* prg stck, will be set in interpret	*/
	pr->stacktop = -1;		/* no arguments		*/

	pr->scope = RxScopeMalloc();
	LPMALLOC(pr->env);
	if (environment)
		Lscpy(pr->env,environment);
	else
		Lstrcpy(pr->env,&(systemStr->key));
	pr->digits = LMAXNUMERICDIGITS;
	pr->fuzz = 0;
	pr->form = SCIENTIFIC;
	pr->condition = 0;
	pr->lbl_error    = &(errorStr->key);
	pr->lbl_halt     = &(haltStr->key);
	pr->lbl_novalue  = &(noValueStr->key);
	pr->lbl_notready = &(notReadyStr->key);
	pr->lbl_syntax   = &(syntaxStr->key);
	pr->codelen = 0;
	pr->trace = normal_trace;
	pr->interactive_trace = FALSE;
	if (tracestr && LLEN(*tracestr)) TraceSet(tracestr);

	/* ======= Compile file ====== */
	RxInitCompile(rxFileList,NULL);
	RxCompile();

#ifdef __DEBUG__
	if (__debug__) {
		printf("Litterals are:\n");
		BinPrint(rxLitterals.parent);
		getchar();

		printf("Labels(&functions) are:\n");
		BinPrint(_labels.parent);
		printf("Code Size: %zd\n\n",LLEN(*_code));
		getchar();
	}
#endif

	/* ======= Execute code ======== */
	if (!rxReturnCode)
		RxInterpret();

run_exit:
		/* pr pointer might have changed if Proc was resized */
	pr = _proc+_rx_proc;
#ifdef __DEBUG__
	if (__debug__)
		printf("Return Code = %d\n",rxReturnCode);
#endif

	/* ======== free up memory ======== */
	RxFileFree(rxFileList);

	LPFREE(pr->env);
	if (CompileClause) {
		FREE(CompileClause);
		CompileClause = NULL;
	}

	RxScopeFree(pr->scope);
	FREE(pr->scope);
	_rx_proc--;

	return rxReturnCode;
} /* RxRun */
