/*
 * $Id: compile.h,v 1.3 2001/06/25 18:52:04 bnv Exp $
 * $Log: compile.h,v $
 * Revision 1.3  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.2  1999/11/29 14:58:00  bnv
 * Changed: Some defines
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <bmem.h>
#include <lstring.h>
#include <bintree.h>
#include <nextsymb.h>

#ifdef __COMPILE_C__
#	define EXTERN
#else
#	define EXTERN extern
#endif

/* ----------- local defines --------------- */
#define UNKNOWN_LABEL		0xFFFF
#define CMP(a)			Lcmp(&symbolstr,a)
#define identCMP(a)		((symbol==ident_sy) && !CMP(a))
#define MUSTBE_SEMICOLON	_mustbe(semicolon_sy, ERR_EXTRAD_DATA,1)
#define SKIP_SEMICOLONS		while (symbol==semicolon_sy) nextsymbol()

#define SYMBOLADD2LITS		_Add2Lits(&symbolstr,symbolhasdot)
#define	SYMBOLADD2LITS_KEY	&(((PBinLeaf)SYMBOLADD2LITS)->key)

/* ----------- Function structure ----------- */
enum functypes {
	FT_LABEL,
	FT_FUNCTION,
	FT_BUILTIN,
	FT_INTERNAL,
	FT_EXTERNAL,
	FT_SYSTEM
};

typedef struct tfunction {
	int	type;			/* function type	*/
	TBltFunc *builtin;		/* builtin function	*/
	size_t	label;			/* offset in code	*/
} RxFunc;

/* ------------ Global Variables ------------ */
/* --- Compile Variables --- */
EXTERN RxFile	*CompileRxFile;		/* Rexx file pointer	*/
EXTERN byte	*CompileCodePtr;	/* code pointer		*/
EXTERN PLstr	CompileCode;		/* code space		*/
#ifndef ALIGN
EXTERN word	CompileCodeLen;		/* code length		*/
#else
EXTERN dword	CompileCodeLen;		/* code length		*/
#endif

EXTERN Clause	*CompileClause;		/* compile clauses	*/
EXTERN int	CompileCurClause;	/* current clause	*/
EXTERN int	CompileClauseItems;	/* maximum clause len	*/

EXTERN int	CompileNesting;		/* current nesting	*/

/* --------------- rexx options ----------------- */
enum options_type {
	 environment_opt	/* address envirnoment	*/
	,trace_opt		/* trace option		*/
	,digits_opt		/* numeric digits	*/
	,fuzz_opt		/* fuzz digits		*/
	,form_opt		/* numeric form		*/
	,author_opt		/* author string	*/
	,version_opt		/* version string	*/
	,os_opt			/* OS string		*/
	,calltype_opt		/* call type string	*/
	,filename_opt		/* rexx prg file name	*/
	,prgname_opt		/* program file name	*/
	,shell_opt		/* current shell	*/
	,set_signal_opt		/* signal condition	*/
	,set_signal_name_opt	/* signal name cond	*/
	,unset_signal_opt	/* signal condition	*/
};

enum expr_type {
	 exp_assign		/* expr for assingment	*/
	,exp_tmp		/* expr copied to tmp	*/
	,exp_normal		/* do not do anything	*/
};

/* ---------------- mnemonic codes --------------- */
enum mnemonic_type {
	 newclause_mn	/* new command used for trace	*/
	,nop_mn		/* Nothing dummy instruction	*/
	,push_mn	/* push lstring			*/
	,pushtmp_mn	/* push a temporary lstring	*/
	,pop_mn		/* pop lstring			*/
	,dup_mn		/* DUP [relative] duplicates something from stack	*/
	,copy_mn	/* COPY to previous (Lstrcpy)*/
	,copy2tmp_mn	/* COPY a value to temporary	*/
	,patch_mn	/* PATCH [relative] [byte] in code	*/
	,raise_mn	/* raise a condition error	*/

	,loadarg_mn	/* load argument	*/
	,loadopt_mn	/* load internal data	*/
	,storeopt_mn	/* save internal data	*/
	,load_mn	/* load a variable	*/
	,create_mn	/* create/load a var	*/
	,drop_mn	/* drop a variable	*/
	,dropind_mn	/* drop a var indirect	*/
	,assignstem_mn	/* assign array		*/

	,byinit_mn	/* DO .. BY .. init	*/
	,forinit_mn	/* DO FOR initialise	*/
	,decfor_mn	/* DEC of a DO FOR loop	*/

	,toint_mn	/* transform mnemonics	*/
	,lower_mn	/* to lowercase	a lit	*/
	,upper_mn	/* to uppercase	a lit	*/

	,signal_mn	/* Clear up stack and jmp rel	*/
	,signalval_mn	/* evaluate and jump	*/

	,jmp_mn		/* Unconditional jump	*/
	,jf_mn		/* Jump if false (0)	*/
	,jt_mn		/* Jump if true (1)	*/
	,call_mn	/* Call a procedure	*/
	,return_mn	/* return from a proc	*/
	,returnf_mn	/* return from a func	*/

	,interpret_mn	/* interpret a string	*/
	,inter_end_mn	/* end of interpret	*/

	,proc_mn	/* start procedure	*/

	,say_mn		/* print on stdout	*/
	,system_mn	/* execute a system command	*/
	,exit_mn	/* normal terminate prg	*/

	,parse_mn	/* load a template to parse	*/
	,pvar_mn	/* parse into a variable	*/
	,pdot_mn	/* parse to a dot	*/
	,tr_space_mn	/* trigger a space	*/
	,tr_lit_mn	/* trigger a litteral	*/
	,tr_abs_mn	/* trigger absolute pos	*/
	,tr_rel_mn	/* trigger relative pos	*/
	,tr_end_mn	/* trigger to end	*/

	,rx_queue_mn
	,rx_push_mn
	,rx_pull_mn
	,rx_external_mn

	,eq_mn		/* checking mnemonics	*/
	,ne_mn
	,gt_mn
	,ge_mn
	,lt_mn
	,le_mn
	,deq_mn
	,dne_mn
	,dgt_mn
	,dge_mn
	,dlt_mn
	,dle_mn

	,teq_mn		/* return to tmp */
	,tne_mn
	,tdeq_mn
	,tdne_mn
	,tgt_mn	
	,tge_mn	
	,tlt_mn	
	,tle_mn	

	,not_mn
	,and_mn
	,or_mn
	,xor_mn

	,concat_mn	/* concat two string	*/
	,bconcat_mn	/* concat two strings with space	*/

	,neg_mn
	,inc_mn
	,dec_mn

	,add_mn
	,sub_mn
	,mul_mn
	,div_mn
	,idiv_mn
	,mod_mn
	,pow_mn
};

/* ------- function prototypes ----- */
void	RxInitCompile( RxFile *rxf, PLstr src );
void	RxDoneCompile( void );
int	RxCompile( void );

#ifndef ALIGN
	word	_CodeInsByte( word pos, byte b );
	word	_CodeAddByte( byte b );
	word	_CodeAddWord( word w );
	word	_CodeAddPtr( void *ptr );
#else
#	define	_CodeAddByte(b)	_CodeAddDWord((dword)(b))
#	define	_CodeAddWord(w)	_CodeAddDWord((dword)(w))
	dword	_CodeInsByte( dword pos, dword d );
	dword	_CodeAddDWord( dword d);
	dword	_CodeAddPtr( void *ptr );
#endif
void	*_Add2Lits( PLstr lit, int hasdot );
void	_mustbe( enum symboltype sym, int err, int subno);
PBinLeaf _AddLabel( int type, size_t position );

#undef EXTERN
#endif
