/*
 * $Id: stack.c,v 1.5 2003/10/30 13:16:28 bnv Exp $
 * $Log: stack.c,v $
 * Revision 1.5  2003/10/30 13:16:28  bnv
 * Variable name change
 *
 * Revision 1.4  2002/06/11 12:37:38  bnv
 * Added: CDECL
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

#define __STACK_C__

#include <bmem.h>

#include <lerror.h>
#include <lstring.h>
#include <stack.h>

#include <rexx.h>

/* ----------------- CreateStack ----------------------- */
void __CDECL
CreateStack( void )
{
	DQueue	*stck;

	stck = (DQueue*) MALLOC(sizeof(DQueue),"Stack");
	DQINIT(*stck);
	DQPUSH(&rxStackList,stck);
} /* CreateStack */

/* ----------------- DeleteStack ----------------------- */
void __CDECL
DeleteStack( void )
{
	DQueue *stck;
	stck = DQPop(&rxStackList);
	DQFlush(stck,_Lfree);
	FREE(stck);
} /* DeleteStack */

/* ----------------- Queue2Stack ----------------------- */
void __CDECL
Queue2Stack( PLstr str )
{
	DQueue *stck;
	stck = DQPEEK(&rxStackList);
	DQAdd2Head(stck,str);
} /* Queue2Stack */

/* ----------------- Push2Stack ----------------------- */
void __CDECL
Push2Stack( PLstr str )
{
	DQueue *stck;
	stck = DQPEEK(&rxStackList);
	DQAdd2Tail(stck,str);
} /* Push2Stack */

/* ----------------- PullFromStack ----------------------- */
PLstr __CDECL
PullFromStack( )
{
	DQueue *stck;
	stck = DQPEEK(&rxStackList);
	return (PLstr)DQPop(stck);
} /* PullFromStack */

/* ----------------- StackQueued ----------------------- */
long __CDECL
StackQueued( void )
{
	DQueue *stck;
	stck = DQPEEK(&rxStackList);
	return stck->items;
} /* StackQueued*/
