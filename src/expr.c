/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/expr.c,v 1.2 1999/11/26 13:13:47 bnv Exp $
 * $Log: expr.c,v $
 * Revision 1.2  1999/11/26 13:13:47  bnv
 * Some spaces changed to tabs.
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#include <lerror.h>
#include <lstring.h>

#include <rexx.h>
#include <trace.h>
#include <compile.h>

/*-----------------* local function prototypes *------------ */
static void Exp0( void );
static void Exp1( void );
static void Exp2( void );
static void Exp3( void );
static void Exp4( void );
static void Exp5( void );
static void Exp6( void );
static void Exp7( void );
static void Exp8( void );
static void C_function( void );

/* ----------- local variables ----------- */
static	int	exp_ct;
static	size_t	exp_pos;

/* ========================= C_expr ========================== */
/* return if it had exited with another code than copy_mn */
/* so something is left in stack */
int
C_expr( int calltype )
{
	exp_ct  = calltype;
	exp_pos = CompileCodeLen;
	Exp0();

	/* If nothing was processed in the expr then push a Null string */
	if (exp_pos == CompileCodeLen) {
		_CodeAddByte(push_mn);
			_CodeAddPtr(&(NullStr->key));
			TraceByte( nothing_middle );
	}
	switch (exp_ct) {
		case exp_assign:
			_CodeAddByte(copy_mn);
			return FALSE;

		case exp_tmp:
			_CodeAddByte(copy2tmp_mn);
			break;
		
		case exp_normal:
			/* do nothing */
			break;

		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	}
	return TRUE;
} /* C_expr */

/* ---------------- InsTmp ---------------- */
static int
InsTmp( size_t pos, int pushtmp)
{
	/* do we need to push a tmp value */
	if ((exp_ct==exp_assign) && (exp_pos==pos)) {
		/* because we have return the value in a temporary var	*/
		/* change the type to exp_normal			*/
		if (pushtmp)
			exp_ct = exp_normal;
	} else {
		if (pushtmp)
			_CodeInsByte(pos,pushtmp_mn);
		else
			return TRUE;
	}
	return FALSE;
} /* InsTmp */

/* ----------------- Exp0 ----------------- */
static void
Exp0( void )
{
	enum mnemonic_type orxor;
	CTYPE	pos;

	pos = CompileCodeLen;
	Exp1();
	orxor = (symbol==xor_sy)? xor_mn : or_mn;
	while ((symbol==or_sy) || (symbol==xor_sy)) {  /* Logical OR; XOR */
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		nextsymbol(); 
		Exp1();
		InsTmp(pos,TRUE);
		_CodeAddByte(orxor);
			TraceByte( operator_middle );
		orxor = (symbol==xor_sy)? xor_mn : or_mn;
	}
} /* Exp0 */

/* ----------------- Exp1 ----------------- */
static void
Exp1( void )
{
	CTYPE	pos;

	pos = CompileCodeLen;
	Exp2();
	while (symbol == and_sy) {	/* Logical AND  */
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		nextsymbol();
		Exp2();
		InsTmp(pos,TRUE);
		_CodeAddByte( and_mn );
			TraceByte( operator_middle );
	}
} /* Exp1 */

/* ----------------- Exp2 ----------------- */
static void
Exp2( void )
{
	enum  symboltype  _symbol;
	CTYPE	pos;

	pos = CompileCodeLen;
	Exp3();
	_symbol = symbol;

	/* DO NOT CHANGE THE ORDER OF THIS SYMBOLS */
	if  ((symbol >= eq_sy)  && (symbol <= dgt_sy)) {
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		nextsymbol();
		Exp3();
/****
//		if (!InsTmp(pos,FALSE)) {	* do we need to add a pushtmp *
*****/
			InsTmp(pos,TRUE);	/* add the pushtmp byte */
			switch (_symbol) {
				case eq_sy  : _CodeAddByte(eq_mn);   break;
				case ne_sy  : _CodeAddByte(ne_mn);   break;
				case gt_sy  : _CodeAddByte(gt_mn);   break;
				case ge_sy  : _CodeAddByte(ge_mn);   break;
				case lt_sy  : _CodeAddByte(lt_mn);   break;
				case le_sy  : _CodeAddByte(le_mn);   break;
				case deq_sy : _CodeAddByte(deq_mn);  break;
				case dne_sy : _CodeAddByte(dne_mn);  break;
				case dgt_sy : _CodeAddByte(dgt_mn);  break;
				case dge_sy : _CodeAddByte(dge_mn);  break;
				case dlt_sy : _CodeAddByte(dlt_mn);  break;
				case dle_sy : _CodeAddByte(dle_mn);  break;
				default:
					Lerror(ERR_INTERPRETER_FAILURE,0);
			}
/*****
//		} else {
//			switch (_symbol) {
//				case eq_sy  : _CodeAddByte(teq_mn);   break;
//				case ne_sy  : _CodeAddByte(tne_mn);   break;
//				case gt_sy  : _CodeAddByte(tgt_mn);   break;
//				case ge_sy  : _CodeAddByte(tge_mn);   break;
//				case lt_sy  : _CodeAddByte(tlt_mn);   break;
//				case le_sy  : _CodeAddByte(tle_mn);   break;
//				case deq_sy : _CodeAddByte(tdeq_mn);  break;
//				case dne_sy : _CodeAddByte(tdne_mn);  break;
//				case dgt_sy : _CodeAddByte(tdgt_mn);  break;
//				case dge_sy : _CodeAddByte(tdge_mn);  break;
//				case dlt_sy : _CodeAddByte(tdlt_mn);  break;
//				case dle_sy : _CodeAddByte(tdle_mn);  break;
//				default:
//					Lerror(ERR_INTERPRETER_FAILURE,0);
//			}
//		}
*****/
		TraceByte( operator_middle );
	}
} /* Exp2 */

/* ----------------- Exp3 ----------------- */
static void
Exp3( void )
{
	int   _Concat;
	int   _Pblank;
	CTYPE	pos;

	pos = CompileCodeLen;
	Exp4();
	_Concat = (symbol==concat_sy);
	_Pblank = symbolPrevBlank;

	/* UNTIL NOT_SY there must be prefix and starting '(' operators */
	while ((symbol <= not_sy) ||  _Concat) {
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		if (_Concat) nextsymbol();
		Exp4();
		InsTmp(pos,TRUE);	/* add the pushtmp byte */
		if (_Concat || !_Pblank)
			_CodeAddByte(concat_mn);
		else
			_CodeAddByte(bconcat_mn);

		TraceByte( operator_middle );

		_Concat = (symbol==concat_sy);
		_Pblank = symbolPrevBlank;
	}
} /* Exp3 */

/* ----------------- Exp4 ----------------- */
static void
Exp4( void )
{
	enum symboltype _symbol;
	CTYPE	pos;

	pos = CompileCodeLen;
	Exp5();
	_symbol = symbol;

	while ((symbol==plus_sy) || (symbol==minus_sy)) {
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		nextsymbol();
		Exp5();
		InsTmp(pos,TRUE);
		if (_symbol==plus_sy)
			_CodeAddByte(add_mn);
		else
			_CodeAddByte(sub_mn);
		TraceByte( operator_middle );
		_symbol = symbol;
	}
} /* Exp4 */

/* ----------------- Exp5 ----------------- */
static void
Exp5( void )
{
	enum  symboltype _symbol;
	CTYPE	pos;

	pos = CompileCodeLen;
	Exp6();
	_symbol = symbol;

	/* ORDER IS IMPORTANT */
	while ((symbol >= times_sy) && (symbol<=intdiv_sy)) {
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		nextsymbol();
		Exp6();
		InsTmp(pos,TRUE);
		switch (_symbol) {
			case times_sy  : _CodeAddByte(mul_mn);   break;
			case div_sy    : _CodeAddByte(div_mn);   break;
			case intdiv_sy : _CodeAddByte(idiv_mn);  break;
			case mod_sy    : _CodeAddByte(mod_mn);   break;
			default:
				Lerror(ERR_INTERPRETER_FAILURE,0);
		}
		TraceByte( operator_middle );
		_symbol = symbol;
	}
}  /* Exp5 */

/* ----------------- Exp6 ----------------- */
static void
Exp6( void )
{
	CTYPE	pos;

	pos = CompileCodeLen;
	Exp7();
	while (symbol==power_sy) {
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		nextsymbol();
		Exp7();
		InsTmp(pos,TRUE);
		_CodeAddByte(pow_mn);
			TraceByte( operator_middle );
	}
} /* Exp6 */

/* ----------------- Exp7 ----------------- */
static void
Exp7( void )
{
	enum symboltype _symbol;
	int  prefix;
	CTYPE	pos;

	pos = CompileCodeLen;
	_symbol = symbol;

	if ((symbol==not_sy) || (symbol==minus_sy)) {
		nextsymbol();
		prefix = TRUE;
	} else
		prefix = FALSE;

	if (!prefix && (symbol==plus_sy))
		nextsymbol();

	Exp8();
	if (prefix) {
		if (CompileCodeLen==pos) Lerror(ERR_INVALID_EXPRESSION,0);
		InsTmp(pos,TRUE);
		if (_symbol==not_sy)
			_CodeAddByte(not_mn);
		else
			_CodeAddByte(neg_mn);
		TraceByte( operator_middle );
	}
} /* Exp7 */

/* ----------------- Exp8 ----------------- */
static void
Exp8( void )
{
	if (symbol == ident_sy) {
		_CodeAddByte(load_mn);
			_CodeAddPtr(SYMBOLADD2LITS);
			TraceByte( variable_middle );
		nextsymbol();
	} else
	if (symbol==literal_sy) {
		_CodeAddByte(push_mn);
			_CodeAddPtr(SYMBOLADD2LITS_KEY);
			TraceByte( litteral_middle );
		nextsymbol();
	} else
	if (symbol == function_sy) {
		C_function();
		_mustbe(ri_parent,ERR_UNMATCHED_PARAN,0);
	} else
	if (symbol==le_parent) {
		nextsymbol();
		Exp0();
		_mustbe(ri_parent,ERR_UNMATCHED_PARAN,0);
	}
} /* Exp8 */

/* -------------------------------------------------------------- */
/*  [Function] ::= [Identifier]([[Expression][,[Expression]]...]) */
/* -------------------------------------------------------------- */
static void
C_function( void )
{
	int	ia,line,realarg=0;
	word	existarg=0,bp=1,lastarg; /* bit mapped, if arguments exist */
	void	*lbl;

	line = symboline;	/* keep line number */

	lbl = _AddLabel( FT_FUNCTION, UNKNOWN_LABEL );

	/* add a space in stack, for the result string */
	if ((exp_ct==exp_assign) && (exp_pos==CompileCodeLen))
		exp_ct = exp_normal;
	else
		_CodeAddByte(pushtmp_mn);

	nextsymbol();
	ia = lastarg = 0;

	if (symbol != ri_parent) {
		if (symbol != comma_sy) {
			Exp0();
			realarg++;
			lastarg = ia+1;
			existarg |= bp;	/* argument exist */
		}
		ia++;
		bp <<= 1;	/* increase bit position */

		while (symbol==comma_sy) {
			nextsymbol();
			if (ia>=MAXARGS)
				Lerror(ERR_INCORRECT_CALL,0);
			if (! ((symbol==comma_sy) || (symbol==ri_parent))) {
				Exp0();
				lastarg = ia+1;
				realarg++;
				existarg |= bp;
			}
			ia++;
			bp <<= 1;	/* increase bit position */
		}
	}

	_CodeAddByte(call_mn);
		_CodeAddPtr(lbl);	/* call pointer */
		_CodeAddByte(lastarg);	/* arguments	*/
		_CodeAddByte(realarg);	/* real args	*/
		_CodeAddWord(existarg);	/* which exist	*/
		_CodeAddWord(line);	/* symbol line	*/
		_CodeAddByte(CT_FUNCTION);	/* call type */
	TraceByte( function_middle );
} /* C_function */
