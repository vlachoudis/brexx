/*
 * $Header: /home/bnv/tmp/brexx/inc/RCS/variable.h,v 1.1 1998/07/02 17:35:50 bnv Exp $
 * $Log: variable.h,v $
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __VARIABLE_H_
#define __VARIABLE_H_

#include <bintree.h>

/* ------------ some defines ------------------ */
#define VARTREES	7
#define NO_CACHE	-1
#define NO_PROC		-1

/* --- Read VarTree options --- */
#define RVT_NOTHING	0
#define RVT_DEPTH	1
#define RVT_HEX		2

#define LEAFVAL(leaf)	&(((Variable*)((leaf)->value))->value)

typedef BinTree *Scope;

typedef
struct tvariable {
	Lstr	value;
	Scope	stem;
	bool	exposed;
} Variable;

/* --------------- function prototypes ----------------- */
void	RxInitVariables(void);
void	RxDoneVariables(void);
void	RxVarFree(void *var);
PBinLeaf RxVarAdd(Scope scope, PLstr name, int hasdot, PBinLeaf stemleaf );
PBinLeaf RxVarFind(const Scope scope, const PBinLeaf litleaf, bool *found);
PBinLeaf RxVarFindOld(Scope scope, PLstr name, bool *found);
void	RxVarDel(Scope scope, PBinLeaf litleaf, PBinLeaf varleaf);
void	RxVarDelOld(Scope scope, PLstr name, PBinLeaf varleaf);
void	RxVarDelInd(Scope scope, PLstr vars);
PBinLeaf RxVarExpose(Scope scope, PBinLeaf litleaf);
void	RxVarSet(Scope scope, PBinLeaf varleaf, PLstr value);
void	RxSetSpecialVar(int,long);
Scope	RxScopeMalloc( void );
void	RxScopeFree(Scope scope);
void	RxScopeAssign(PBinLeaf leaf);

void	RxVar2Str( PLstr result, PBinLeaf leaf, int option );
void	RxReadVarTree(PLstr result, Scope scope, PLstr head, int option);

int	RxRegPool(char *poolname, int (*getf)(PLstr,PLstr),
                        int (*setf)(PLstr,PLstr));
	
#undef EXTERN
#endif
