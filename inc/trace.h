/*
 * $Header: /home/bnv/tmp/brexx/inc/RCS/trace.h,v 1.1 1998/07/02 17:35:50 bnv Exp $
 * $Log: trace.h,v $
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __TRACE_H_
#define __TRACE_H_

#include <bnv.h>
#include <lstring.h>

#ifdef __TRACE_C__
#	define EXTERN
#else
#	define EXTERN extern
#endif


/* ------------------ tracing --------------------- */
enum tracetype	{	all_trace		= BIT0,
			commands_trace		= BIT1,
			error_trace		= BIT2,
			intermediates_trace	= BIT3,
			labels_trace		= BIT4,
			normal_trace		= BIT5,
			off_trace		= BIT6,
			results_trace		= BIT7,
			scan_trace		= BIT8
		};

#define AIR_TRACE  ( all_trace | intermediates_trace | results_trace )

/* =============== TRACE BYTE IN CODE ==================== * 
 *	Bit	Description
 *	 7 	Marks start of a clause	
 *	 6	End of Clause
 *	 5	Result trace display
 *	 4	- Not used -
 *	 3	- Not used -
 *	 2	middle character marker bit-2
 *	 1	middle character marker bit-1
 *	 0	middle character marker bit-0
 */

/* ---- will use the 3 last bits ---- */
enum MiddleCharType {	nothing_middle	= 0,
			other_middle	= 1,
			litteral_middle	= 2,
			variable_middle	= 3,
			compound_middle	= 4,
			operator_middle = 5,
			function_middle = 6,
			dot_middle	= 7
};

#define	TB_TRACE	BIT7
#define	TB_RESULT	BIT6
#define TB_MIDDLECHAR	MASK3

/* ------------------- Function Prototypes ------------------ */
int	TraceCurline( RxFile **rxf, int print );
void	TraceSet( PLstr );
void	TraceByte( int middlechar );
void	TraceClause( void );
void	TraceInstruction( byte inst );
int	TraceInteractive( int );

#undef EXTERN
#endif
