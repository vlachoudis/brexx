/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/rxmath.c,v 1.3 1999/11/26 13:13:47 bnv Exp $
 * $Log: rxmath.c,v $
 * Revision 1.3  1999/11/26 13:13:47  bnv
 * Changed: The formatting of the code.
 *
 * Revision 1.2  1999/02/26 08:45:49  bnv
 * Correction of pow10 for BORLANDC.
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#include <math.h>
#include <stdlib.h>

#include <lerror.h>
#include <lstring.h>

#include <rexx.h>
#include <rxdefs.h>

/* --------------------------------------------------------------- */
/*  ABS(number)                                                    */
/* --------------------------------------------------------------- */
/*  SIGN(number)                                                   */
/* --------------------------------------------------------------- */
void
R_abs_sign( const int func )
{
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);

	if (func==f_abs)
		Labs(ARGR,ARG1);
	else
		Licpy(ARGR,Lsign(ARG1));
} /* R_abs_sign */

/* ----------------------* common math functions *---------------- */
void
R_math( const int func )
{
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	L2REAL(ARG1);
	Lstrcpy(ARGR,ARG1);
	switch (func) {
		case f_acos:
			LREAL(*ARGR) = acos(LREAL(*ARGR));
			break;

		case f_asin:
			LREAL(*ARGR) = asin(LREAL(*ARGR));
			break;

		case f_atan:
			LREAL(*ARGR) = atan(LREAL(*ARGR));
			break;

		case f_cos :
			LREAL(*ARGR) =  cos(LREAL(*ARGR));
			break;

		case f_cosh:
			LREAL(*ARGR) = cosh(LREAL(*ARGR));
			break;

		case f_exp :
			LREAL(*ARGR) =  exp(LREAL(*ARGR));
			break;

		case f_log :
			LREAL(*ARGR) =  log(LREAL(*ARGR));
			break;

		case f_log10:
			LREAL(*ARGR) = log10(LREAL(*ARGR));
			break;

		case f_pow10:
			LREAL(*ARGR) = pow(10.0,LREAL(*ARGR));
			break;

		case f_sin :
			LREAL(*ARGR) =  sin(LREAL(*ARGR));
			break;

		case f_sinh:
			LREAL(*ARGR) = sinh(LREAL(*ARGR));
			break;

		case f_sqrt:
			LREAL(*ARGR) = sqrt(LREAL(*ARGR));
			break;

		case f_tan :
			LREAL(*ARGR) =  tan(LREAL(*ARGR));
			break;

		case f_tanh:
			LREAL(*ARGR) = tanh(LREAL(*ARGR));
			break;

		default:
			Lerror(ERR_INTERPRETER_FAILURE,0);
	} /* switch */
} /* R_math */

/* --------------------------------------------------------------- */
/*  ATAN2(x,y)                                                     */
/* --------------------------------------------------------------- */
/*  POW(x,y)                                                       */
/* --------------------------------------------------------------- */
void
R_atanpow( const int func )
{
	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);
	L2REAL(ARG1);
	L2REAL(ARG2);
	Lstrcpy(ARGR,ARG1);
	if (func==f_atan2)
		LREAL(*ARGR) = atan2(LREAL(*ARGR),LREAL(*ARG2));
	else
		LREAL(*ARGR) = pow(LREAL(*ARGR),LREAL(*ARG2));
} /* R_atanpow */
