/*
 * $Header: /home/bnv/tmp/brexx/src/RCS/stack.c,v 1.1 1998/07/02 17:34:50 bnv Exp $
 * $Log: stack.c,v $
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

#define __STACK_C__

#include <bnv.h>
#include <bmem.h>
#include <stdio.h>

#include <lerror.h>
#include <lstring.h>
#include <stack.h>

#include <rexx.h>

/* ----------------- CreateStack ----------------------- */
void
CreateStack( void )
{
	DQueue	*stck;

	stck = (DQueue*) MALLOC(sizeof(DQueue),"Stack");
	DQINIT(*stck);
	DQPUSH(&StackList,stck);
} /* CreateStack */

/* ----------------- DeleteStack ----------------------- */
void
DeleteStack( void )
{
	DQueue *stck;
	stck = DQPop(&StackList);
	DQFlush(stck,_Lfree);
	FREE(stck);
} /* DeleteStack */

/* ----------------- Queue2Stack ----------------------- */
void
Queue2Stack( PLstr str )
{
	DQueue *stck;
	stck = DQPEEK(&StackList);
	DQAdd2Head(stck,str);
} /* Queue2Stack */

/* ----------------- Push2Stack ----------------------- */
void
Push2Stack( PLstr str )
{
	DQueue *stck;
	stck = DQPEEK(&StackList);
	DQAdd2Tail(stck,str);
} /* Push2Stack */

/* ----------------- PullFromStack ----------------------- */
PLstr
PullFromStack( )
{
	DQueue *stck;
	stck = DQPEEK(&StackList);
	return (PLstr)DQPop(stck);
} /* PullFromStack */

/* ----------------- StackQueued ----------------------- */
long
StackQueued( void )
{
	DQueue *stck;
	stck = DQPEEK(&StackList);
	return stck->items;
} /* StackQueued*/
