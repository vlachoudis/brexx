/*
 * $Id: bintree.h,v 1.2 2001/06/25 18:52:04 bnv Exp $
 * $Log: bintree.h,v $
 * Revision 1.2  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.1  1998/07/02 17:35:50  bnv
 * Initial revision
 *
 */

#ifndef __BINTREE_H__
#define __BINTREE_H__

#include <lstring.h>

#ifdef __BINTREE_C__
#	define	EXTERN
#else
#	define	EXTERN	extern
#endif

#define BALANCE_START	7
#define BALANCE_INC	8

typedef struct binleaf_st {
	Lstr	key;
	struct binleaf_st  *left, *right;
	void	*value;
} BinLeaf;

typedef BinLeaf	*PBinLeaf;

typedef struct {
	PBinLeaf	parent;
	int	items;
	int	maxdepth;
	int	balancedepth;
} BinTree;

#define BINTREEINIT(t)	{(t).parent=NULL; (t).items=0; \
		(t).maxdepth=0; (t).balancedepth=BALANCE_START;}

/* =================== function prototypes =================== */
PBinLeaf BinAdd( BinTree *tree, PLstr name, void *dat );
PBinLeaf BinFind( BinTree *tree, PLstr name );
void	BinDisposeLeaf( BinTree *tree, PBinLeaf leaf, void (BinFreeData)(void *) );
void	BinDisposeTree( BinTree *tree, void (BinFreeData)(void *) );
void	BinDel( BinTree *tree, PLstr name, void (BinFreeData)(void *dat) );
void	BinPrint( PBinLeaf leaf );
void	BinBalance( BinTree *tree );

#undef EXTERN
#endif
