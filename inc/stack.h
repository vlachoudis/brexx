/*
 * $Id: stack.h,v 1.2 2001/06/25 18:52:04 bnv Exp $
 * $Log: stack.h,v $
 * Revision 1.2  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __STACK_H__
#define __STACK_H__

#include <lstring.h>
#include <dqueue.h>

/* ---- function prototypes ---- */
void	CreateStack( void );
void	DeleteStack( void );
void	Queue2Stack( PLstr str );
void	Push2Stack( PLstr str );
PLstr	PullFromStack( void );
long	StackQueued( void );

#endif
