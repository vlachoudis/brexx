/*
 * $Id: bintree.c,v 1.6 2003/10/30 13:15:12 bnv Exp $
 * $Log: bintree.c,v $
 * Revision 1.6  2003/10/30 13:15:12  bnv
 * default removed
 *
 * Revision 1.5  2002/06/11 12:37:38  bnv
 * Added: CDECL
 *
 * Revision 1.4  2001/06/25 18:51:48  bnv
 * Header -> Id
 *
 * Revision 1.3  1999/11/26 13:13:47  bnv
 * Changed: To use the new macros.
 *
 * Revision 1.2  1999/03/10 16:53:32  bnv
 * *** empty log message ***
 *
 * Revision 1.1  1998/07/02 17:34:50  bnv
 * Initial revision
 *
 */

/*
 * Binary Tree
 *  ~~~~~~ ~~~~
 * Very general purpose routines for binary tree implemetation.
 * Each leaf contains a (PLstr)key with the name of the leaf
 * and a (void*)value which contains the value of the leaf.
 * 
 * The searching is done with the key's checked with _Lstrcmp
 * that means that an INTEGER or a REAL is stored according
 * to its binary representation in memory.
 *
 * When adding a leaf no memory allocation is done for the key
 * and the value.
 */

#include <bmem.h>
#ifndef WCE
#	include <stdio.h>
#endif
#include <string.h>
#include <bintree.h>

#ifdef __DEBUG__
static int scandepth( BinLeaf *leaf, int depth );
#endif

/* ------------------ BinAdd ------------------ */
BinLeaf* __CDECL
BinAdd( BinTree *tree, PLstr name, void *dat )
{
	BinLeaf	*ThisEntry;
	BinLeaf	*LastEntry;
	BinLeaf	*leaf;
	bool	LeftTaken;
	int	cmp, dep=0;

	/* If tree is NULL then it will produce an error */
	ThisEntry = tree->parent;
	while (ThisEntry != NULL) {
		LastEntry = ThisEntry;
		cmp = _Lstrcmp(name,&(ThisEntry->key));
		if (cmp < 0) {
			ThisEntry = ThisEntry->left;
			LeftTaken = TRUE;
		} else
		if (cmp > 0) {
			ThisEntry = ThisEntry->right;
			LeftTaken = FALSE;
		} else
			return ThisEntry;
		dep++;
	}

	/* Create a new entry */
	leaf = (BinLeaf *) MALLOC( sizeof(BinLeaf),"BinLeaf" );

	/* just move the data to the new Lstring */
	/* and initialise the name LSTR(*name)=NULL */
	LMOVESTR(leaf->key, *name);

	leaf->value = dat;
	leaf->left = NULL;
	leaf->right = NULL;

	if (tree->parent==NULL)
		tree->parent = leaf;
	else {
		if (LeftTaken)
			LastEntry->left = leaf;
		else
			LastEntry->right = leaf;
	}
	tree->items++;
	if (dep>tree->maxdepth) {
		tree->maxdepth = dep;
		if (tree->maxdepth > tree->balancedepth)
			BinBalance(tree);
	}
	return leaf;
} /* BinAdd */

/* ------------------ BinFind ----------------- */
BinLeaf* __CDECL
BinFind( BinTree *tree, PLstr name )
{
	BinLeaf	*leaf;
	int	cmp;

	leaf = tree->parent;
	while (leaf != NULL) {
		cmp = _Lstrcmp(name, &(leaf->key));
		if (cmp < 0)
			leaf = leaf->left;
		else
		if (cmp > 0)
			leaf = leaf->right;
		else
			return leaf;
	}
	return NULL;
} /* BinFind */

/* ----------------- BinDisposeLeaf -------------------- */
void __CDECL
BinDisposeLeaf( BinTree *tree, BinLeaf *leaf,
		void (__CDECL *BinFreeData)(void *) )
{
	if (!leaf) return;
	if (leaf->left)
		BinDisposeLeaf(tree,leaf->left,BinFreeData);
	if (leaf->right)
		BinDisposeLeaf(tree,leaf->right,BinFreeData);

	LFREESTR(leaf->key);
	if (leaf->value && BinFreeData)
		BinFreeData(leaf->value);

	if (leaf==tree->parent)
		tree->parent = NULL;
	FREE(leaf);
	tree->items--;
} /* BinDisposeLeaf */

/* ----------------- BinDisposeTree -------------------- */
void __CDECL
BinDisposeTree( BinTree *tree, void (__CDECL *BinFreeData)(void *) )
{
	if (tree != NULL) {
		BinDisposeLeaf(tree,tree->parent,BinFreeData);
		FREE(tree);
	}
} /* BinDisposeTree */

/* -------------------------------------------------------------- */
/* To correctly delete a pointer from a Binary tree we must not   */
/* change the tree structure, that is the smaller values are the  */
/* left most. In order to satisfy this with few steps we must     */
/* replace the pointer that is to be erased with the one which is */
/* closest with a smaller value (the right most from the left     */
/* branch, as you can see below                                   */
/*                     ...                                        */
/*                    /                                           */
/*                 (name)   <-- to be dropped                     */
/*                 /    \                                         */
/*              (a)      (d)       (c)= newid from left branch    */
/*             /   \       ...          where  c->right=NULL      */
/*          ...     (c)            (a)= par_newidt parent of newid*/
/*                 /   \                                          */
/*              (b)     NIL       newid will become the new       */
/*             ...                sub-head when (name) is dropped */
/*                    |                                           */
/*                    |                                           */
/*                   \|/                                          */
/*                    V   ...                                     */
/*                      /                                         */
/*                    (c)           but in the case that          */
/*                   /   \          (a)=(c) when a->right = NULL  */
/*                 (a)    (d)       then the tree is very simple  */
/*                /   \    ....     we simply replace a->right=d  */
/*              ...   (b)                                         */
/*                    ....                                        */
/*                                                                */
/* -------------------------------------------------------------- */
void __CDECL
BinDel( BinTree *tree, PLstr name, void (__CDECL *BinFreeData)(void *) )
{
	BinLeaf	*thisid, *previous, *par_newid, *newid;
	bool	lefttaken;
	int	cmp;

	thisid = tree->parent;
	while (thisid != NULL) {
		cmp=_Lstrcmp(name,&(thisid->key));
		if (cmp < 0) {
			previous = thisid;
			thisid = thisid->left;
			lefttaken = TRUE;
		} else
		if (cmp > 0) {
			previous = thisid;
			thisid = thisid->right;
			lefttaken = FALSE;
		} else
			break;
	}
	if (thisid == NULL) return;	/* Not Found */

	if (thisid->right == NULL)
		newid = thisid->left;
	else
	if (thisid->left == NULL)
		newid = thisid->right;
	else {		/* when no node is empty */

		/* find the right most id of the */
		/* left branch of thisid         */

		par_newid = thisid;
		newid = thisid->left;
		while (newid->right != NULL) {
			par_newid = newid;
			newid = newid->right;
		}

		/* newid must now replace thisid */
		newid->right = thisid->right;

		if (par_newid != thisid) {
			par_newid->right = newid->left;
			newid->left = thisid->left;
		}
	}

	if (thisid == tree->parent)
		tree->parent = newid;
	else {
		if (lefttaken)
			previous->left = newid;
		else
			previous->right = newid;
	}
	thisid->left = NULL;
	thisid->right = NULL;
	BinDisposeLeaf(tree,thisid,BinFreeData);
} /* BinDel */

#ifdef __DEBUG__
/* -------------------- BinPrint ---------------------- */
void __CDECL
BinPrint(BinLeaf *leaf)
{
	long i;
	static int depth = 0;

	if (!leaf) return;
	depth += 3;

	BinPrint(leaf->left);
	for (i=0; i<depth-3; i++) PUTCHAR(' ');
	PUTCHAR('\"');
	Lprint(STDOUT,&(leaf->key));
	switch (LTYPE(leaf->key)) {
		case LINTEGER_TY:
			PUTS("\"d = ");
			break;
		case LREAL_TY:
			PUTS("\"r = ");
			break;
		case LSTRING_TY:
			PUTS("\"s = ");
			break;
	}
	//Lprint(STDOUT,leaf->value);
	if (leaf->value)
		printf("%p\n",leaf->value);
	else
		PUTS("NULL\n");
	BinPrint(leaf->right);

	depth -= 3;
} /* BinPrint */
#endif

/* -------------------- LeafBalance --------------------- */
static void
LeafBalance( BinLeaf *leaf, BinLeaf **head, BinLeaf **tail )
{
	BinLeaf *Lhead, *Ltail;
	BinLeaf *Rhead, *Rtail;

	if (leaf == NULL) {
		*head = NULL;
		*tail = NULL;
		return;
	}

	LeafBalance(leaf->left,  &Lhead, &Ltail);
	LeafBalance(leaf->right, &Rhead, &Rtail);

	/* connect nodes */
	/*  head - left - middle - right - tail */

	if (Ltail) Ltail->right = leaf;
	leaf->left   = Ltail;

	leaf->right  = Rhead;
	if (Rhead) Rhead->left  = leaf;

	if (Lhead)
		*head = Lhead;
	else
		*head = leaf;

	if (Rtail)
		*tail = Rtail;
	else
		*tail = leaf;
} /* LeafBalance */

/* -------------------- LeafConstruct ------------------- */
static BinLeaf *
LeafConstruct( BinLeaf *head, BinLeaf *tail, int n, int *maxdepth )
{
	int	Lmaxd, Rmaxd, i, mid;
	BinLeaf	*Lleaf, *Rleaf, *LMidleaf, *Midleaf, *RMidleaf;

	if (n==0) return NULL;
	if (n==1) {
		/* then head must be equal to tail */
		head->left = NULL;
		head->right = NULL;
		return head;
	}
	if (n==2) {
		(*maxdepth)++;
		head->left = NULL;
		head->right = tail;
		tail->left = NULL;
		tail->right = NULL;
		return head;
	}

	/* --- find middle --- */
	mid = n/2;
	LMidleaf = head;
	for (i=0; i<mid-1; i++)
		LMidleaf = LMidleaf->right;
	Midleaf = LMidleaf->right;
	RMidleaf = Midleaf->right;

	/* --- do the same for left and right branch --- */
	Lmaxd = Rmaxd = *maxdepth+1;

	Lleaf = LeafConstruct(head,LMidleaf,mid,&Lmaxd);
	Rleaf = LeafConstruct(RMidleaf,tail,n-mid-1,&Rmaxd);

	*maxdepth = MAX(Lmaxd, Rmaxd);

	Midleaf->left = Lleaf;
	Midleaf->right = Rleaf;

	return Midleaf;
} /* LeafConstruct */

/* -------------------- BinBalance ---------------------- */
void __CDECL
BinBalance( BinTree *tree )
{
	BinLeaf *head, *tail;
	int	maxdepth=1;

	/* first we will make tree a double queue */
	LeafBalance( tree->parent, &head, &tail );

	/* now we must reconstruct the tree */
	tree->parent = 
		LeafConstruct( head, tail, tree->items, &maxdepth );
	tree->maxdepth = maxdepth;
	tree->balancedepth = maxdepth+BALANCE_INC;
} /* BinBalance */

#ifdef __DEBUG__
static int
scandepth( BinLeaf *leaf, int depth )
{
	int left, right;
	if (leaf==NULL)
		return depth;
	left =  scandepth(leaf->left,depth+1);
	right =	scandepth(leaf->right,depth+1);
	return MAX(left,right);
} /* scandepth */
#endif
