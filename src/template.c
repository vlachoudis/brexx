/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/template.c,v 1.2 1999/11/26 13:13:47 bnv Exp $
 * $Log: template.c,v $
 * Revision 1.2  1999/11/26 13:13:47  bnv
 * Changed: To use the new macros.
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

/* ----------- vrefp --------- */
/* variable reference position */
static void
vrefp( void )
{
	nextsymbol();	/* skip left parenthesis */

	if (symbol != ident_sy)
	Lerror( ERR_STRING_EXPECTED,7,&symbolstr);

	_CodeAddByte( load_mn );
		_CodeAddPtr( SYMBOLADD2LITS );
		TraceByte( nothing_middle );
	nextsymbol();

	_mustbe(ri_parent,ERR_INV_VAR_REFERENCE,1);
} /* vrefp */

/* -------- position ------- */
/* variable reference position */
static void
position(void)
{
	int	type;

	if (symbol==le_parent)
		vrefp();
	else
	if (symbol==literal_sy) {
		type = _Lisnum(&symbolstr);
		if (type==LREAL_TY)
			Lerror(ERR_INVALID_INTEGER,4,&symbolstr);
		else
		if (type==LSTRING_TY)
			Lerror(ERR_INVALID_TEMPLATE,2,&symbolstr);
		_CodeAddByte(push_mn);
			_CodeAddPtr(SYMBOLADD2LITS_KEY);
			TraceByte( nothing_middle );
		nextsymbol();
	} else
		Lerror(ERR_INVALID_TEMPLATE,2,&symbolstr);
	_CodeAddByte(toint_mn);
} /* position */

/* -------------------------------------------------------------- */
/*  template_list := template | [template] ',' [template_list]    */
/*  template   := (trigger | target | Msg38.1)+                   */
/*  target     := VAR_SYMBOL | '.'                                */
/*  trigger    := pattern | positional                            */
/*  pattern    := STRING | vrefp                                  */
/*  vrefp      := '(' (VAR_SYMBOL | Msg19.7) (')' | Msg46.1)      */
/*  positional := absolute_pos | relative_pos                     */
/*  absolute_pos := NUMBER | '=' position                         */
/*  position   := NUMBER | vrefp  | Msg38.2                       */
/*  relative_pos := ('+' | '-') position                          */
/* -------------------------------------------------------------- */
void
C_template(void)
{
	void	*target_ptr=NULL;
	bool	trigger, dot=FALSE;
	bool	sign;
	int	type;

	_CodeAddByte(parse_mn);
	while ((symbol!=semicolon_sy) && (symbol!=comma_sy)) {
		trigger = FALSE;
		switch (symbol) {
			case ident_sy:
			case dot_sy:
				if (target_ptr || dot) {
					/* trigger space */
					trigger = TRUE;
					_CodeAddByte(tr_space_mn);
					/* do not go to next symbol */
				} else {
					if (symbol==ident_sy)
						target_ptr = SYMBOLADD2LITS;
					else
						dot = TRUE;
					nextsymbol();
				}
				break;

			case minus_sy:
			case plus_sy:
				trigger = TRUE;
				sign = (symbol==minus_sy);
				nextsymbol();
				position();
				if (sign) {
					_CodeAddByte(neg_mn);
					TraceByte( nothing_middle );
				}
				_CodeAddByte(tr_rel_mn);
				break;

			case literal_sy:
				trigger = TRUE;

				if (symbolisstr) {
					_CodeAddByte(push_mn);
						_CodeAddPtr(SYMBOLADD2LITS_KEY);
						TraceByte( nothing_middle );
					_CodeAddByte(tr_lit_mn);
					nextsymbol();
				} else {
					type = _Lisnum(&symbolstr);
					if (type==LREAL_TY)
						Lerror(ERR_INVALID_INTEGER,4,&symbolstr);
					else
					if (type==LSTRING_TY)
						Lerror(ERR_INVALID_TEMPLATE,1,&symbolstr);

					_CodeAddByte(push_mn);
						_CodeAddPtr(SYMBOLADD2LITS_KEY);
						TraceByte( nothing_middle );
					_CodeAddByte(toint_mn);
					_CodeAddByte(tr_abs_mn);
					nextsymbol();
				}
				break;

			case le_parent:
				trigger = TRUE;
				vrefp();
				_CodeAddByte(tr_lit_mn);
				break;

			case eq_sy:
				trigger = TRUE;
				nextsymbol();
				position();
				_CodeAddByte(toint_mn);
				_CodeAddByte(tr_abs_mn);
				break;

			default:
				Lerror(ERR_INVALID_TEMPLATE,0);
		} /* end of switch */
		if (trigger) {
			if (target_ptr) {
				_CodeAddByte(create_mn);
					_CodeAddPtr(target_ptr);
				_CodeAddByte(pvar_mn);
					TraceByte( other_middle );
				target_ptr = NULL;
			} else
			if (dot) {
				_CodeAddByte(pdot_mn);
					TraceByte( dot_middle );
				dot = FALSE;
			}
		}
	} /* end of while */

	if (target_ptr) {	/* assign the remaining part */
		_CodeAddByte(tr_end_mn);
		_CodeAddByte(create_mn);
			_CodeAddPtr(target_ptr);
		_CodeAddByte(pvar_mn);
			TraceByte( other_middle );
	} else
	if (dot) {
		_CodeAddByte(tr_end_mn);
		_CodeAddByte(pdot_mn);
			TraceByte( dot_middle );
	}

	_CodeAddByte(pop_mn);
		_CodeAddByte(1);
} /* C_template */
