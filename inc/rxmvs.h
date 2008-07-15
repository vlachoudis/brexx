/*
 * $Id: rxmvs.h,v 1.1 2008/07/15 14:57:11 bnv Exp $
 * $Log: rxmvs.h,v $
 * Revision 1.1  2008/07/15 14:57:11  bnv
 * Initial revision
 *
 *
 * Following sections contains name redefinitions for VM
 */
#ifndef __RXMVS_H
#define __RXMVS_H

#define BinDisposeLeaf BiDiLeaf
#define BinDisposeLeaf BiDiLeaf
/* compile.c */
#define CompileCode CmpC
#define CompileCurClause CmpCuCl
#define CompileCodeLen CmpCoLn
#define CompileClause  CmpCl
#define CompileClauseItems  CmpClIt
#define CompileCodePtr  CmpCoPt
#define _CodeAddPtr _CoAdPt
#define _CodeAddDWord _CoAdDw
#define _CodeAddWord _CoAdWo
#define _CodeAddByte _CoAdBy
/*
 interpre.c & interpre.h
*/
#define RxInitInterStr RxInInStr
#define RxInitInterpret RxInIn
#define RxDoneInterpret RxDoIn
#define RxDoneInterpretStr RxDoInStr
#define I_trigger_space I_tr_space
#define I_trigger_literal I_tr_literal
/*
  Nextsymbol and Compile
*/
#define InitNextch InNech
#define InitNextSymbol InNeSymbol
#define symbolstr symbstr
#define symbolstat symbstat
#define symbolPrevBlank symbPrBl
#define symbolprevptr symbprptr
/*
 RxFunction
*/
#define RxRegFunctionDone RxReFnDn
#define RxRegFunction RxReFn
#define R_charlinein R_chlnin
#define R_charlineout R_chlnot
#define charlinein chlnin
#define charlineout chlnot
/*
  BUILTIN.C & VARIABLE.C
*/
#define RxVarFindName RxVarFName
#define RxVarDelName RxVarDeName
#define RxVarDelInd RxVarDeInd
#define RxVarExposeInd RxVarExInd
#define SystemPoolGet SysPlGet
#define SystemPoolSet SysPlSet

#endif
