/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/maintest.c,v 1.3 1999/11/26 13:13:47 bnv Exp $
 * $Log: maintest.c,v $
 * Revision 1.3  1999/11/26 13:13:47  bnv
 * *** empty log message ***
 *
 * Revision 1.2  1999/05/14 12:31:22  bnv
 * Fork test routine
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */
#include <stdio.h>
#include <string.h>
#include <lstring.h>

#include <rexx.h>
#include <rxdefs.h>
#include <compile.h>

/* ----------- MyFunc -------- */
void
MyFunc( int test )
{
	if (test)
		Lscpy(ARGR,"Banana");
	else
		Lscpy(ARGR,"Bingo");
} /* MyFunc */

/* ----------- RxFork ---------- *
void
RxFork( )
{
	Licpy(ARGR,fork());
} * RxFork */

/* ----------- RxIndirectCall ---------- */
TBltFunc	*C_isBuiltin( PLstr );
void		RxProcResize();
void
RxIndirectCall( )
{
	PBinLeaf	leaf;
	TBltFunc	*bltfunc;	// Is a builtin function?

	if (ARGN<1)
		Lerror(ERR_INCORRECT_CALL,0);

	must_exist(1);
	Lupper(ARG1);

	/* ---- Search for the label ---- */
	leaf = BinFind(&_labels,ARG1);
	if (leaf==NULL || ((RxFunc*)(leaf->value))->label==UNKNOWN_LABEL) {
		/* ---- Last chance, trye a builtin ----- */
		if ((bltfunc=C_isBuiltin(ARG1))!=NULL) {
			int i;
			/* setup arguments */
			ARGN--;
			for (i=0; i<ARGN; i++)
				Rxarg.a[i] = Rxarg.a[i+1];
			Rxarg.a[i] = NULL;
			(bltfunc->func)(bltfunc->opt);
		} else
			Lerror(ERR_UNEXISTENT_LABEL,1,ARG1);
		return;
	}

	// -- Label found, prepare the arguments and do the function call --
printf("Label FOUND\n");
//	func = (RxFunc*)(leaf->value);
//	RxSetSpecialVar(SIGLVAR,TraceCurline(NULL,FALSE));
//	Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart + (size_t)(func->label));
} /* RxIndirectCall */

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

		} else {
			Rxcip++;
			RxSetSpecialVar(SIGLVAR,line);
			I_MakeArgs(ct,nargs,existarg);
			Rxcip = (CIPTYPE*)((byte huge *)Rxcodestart+func->label);
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
						Lprint(STDOUT,&(litleaf->key));
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
} /* RxIndirectCall */

/* --------------------- main ---------------------- */
int
main(int ac, char *av[])
{
	Lstr	args, tracestr, file;
	int	input,ia,ir;

	input = FALSE;
	LINITSTR(args);
	LINITSTR(tracestr);
	LINITSTR(file);

	if (ac<2) {
		puts("\nsyntax: rexx [-[trace]] <filename> <args>...\nrexx - (to use stdin)\n");
		puts(VERSION);
		puts("Author: "AUTHOR);
		puts("Please report any bugs, fatal errors or comments to the");
		puts("above address, or to <bnv@nisyros.physics.auth.gr>\n");
		return 0;
	}
#ifdef __DEBUG__
	__debug__ = FALSE;
#endif

	/* --- Initialise --- */
	RxInitialize(av[0]);

	/* --- User registered functions --- */
	RxRegFunction("BINGO",	MyFunc,	0);
	RxRegFunction("BANANA",	MyFunc,	1);
	//RxRegFunction("FORK",	RxFork,	0);

	RxRegFunction("THE",	RxIndirectCall, 0);

	/* --- scan arguments --- */
	ia = 1;
	if (av[ia][0]=='-') {
		if (av[ia][1]==0)
			input = TRUE;
		else
			Lscpy(&tracestr,av[ia]+1);
		ia++;
	} else
	if (av[ia][0]=='?' || av[ia][0]=='!') {
		Lscpy(&tracestr,av[ia]);
		ia++;
	}

	/* --- let's read a normal file --- */
	if (!input && ia<ac) {
		/* prepare arguments for program */
		for (ir=ia+1; ir<ac; ir++) {
			Lcat(&args,av[ir]);
			if (ir<ac-1) Lcat(&args," ");
		}
		RxRun(av[ia],NULL,&args,&tracestr,NULL);
	} else {
		if (ia>=ac)
			Lread(STDIN,&file,LREADFILE);
		else
			for (;ia<ac; ia++) {
				Lcat(&file,av[ia]);
				if (ia<ac-1) Lcat(&file," ");
			}
		RxRun(NULL,&file,&args,&tracestr,NULL);
	}

	/* --- Free everything --- */
	RxFinalize();
	LFREESTR(args);
	LFREESTR(tracestr);
	LFREESTR(file);

#ifdef __DEBUG__
	if (mem_allocated()!=0) {
		fprintf(STDERR,"\nMemory left allocated: %ld\n",mem_allocated());
		mem_list();
	}
#endif

	return RxReturnCode;
} /* main */
