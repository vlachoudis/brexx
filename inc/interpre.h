/*
 * $Header: /home/bnv/tmp/brexx/inc/RCS/interpre.h,v 1.1 1998/07/02 17:35:50 bnv Exp $
 * $Log: interpre.h,v $
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __INTERPRET_H_
#define __INTERPRET_H_

#include <lstring.h>
#include <bintree.h>

#ifdef  __INTERPRET_C__
#	define EXTERN
#else
#	define EXTERN extern
#endif

#if !defined(ALIGN)
#	define CIPTYPE	byte
#else
#	define CIPTYPE	dword
#endif

/* -------- variables definitions ------ */
EXTERN	int	Rx_proc;		/* current procedure	*/
EXTERN	int	Rx_id;			/* current program id	*/
EXTERN	CIPTYPE	*Rxcodestart;		/* actual code          */
EXTERN	CIPTYPE	*Rxcip;			/* instruction pointer	*/

/* -------- function prototypes ------- */
void	RxInitInterpret( void );
void	RxDoneInterpret( void );
int	RxInterpret( void );

#undef EXTERN
#endif
