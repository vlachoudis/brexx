/*
 * $Id: winfunc.h,v 1.1 2006/01/26 10:30:55 bnv Exp $
 * $Log: winfunc.h,v $
 * Revision 1.1  2006/01/26 10:30:55  bnv
 * Initial revision
 *
 * Revision 1.4  2004/08/16 15:30:30  bnv
 * Added: New functions for Colors, ScrollBars
 *
 * Revision 1.3  2002/06/11 12:37:56  bnv
 * Added: CDECL
 *
 * Revision 1.2  2001/06/25 18:52:04  bnv
 * Header -> Id
 *
 * Revision 1.1  1999/11/29 14:58:00  bnv
 * Initial revision
 *
 */
#ifndef __WINFUNC_H__
#define __WINFUNC_H__

/* ---- Function Prototypes ---- */
BOOL __CDECL Win_RegGetData(HKEY key, TCHAR *keyPath, TCHAR *varName,
		   DWORD type, LPVOID pvData, LPDWORD cbData );
BOOL __CDECL Win_RegSetData(HKEY key, TCHAR *keyPath, TCHAR *varName,
		   DWORD type, LPVOID pvData, DWORD cbData );

/* ---- Macros Definitions ---- */
#define RXREGGETDATA(v,t,d,l) Win_RegGetData(HKEY_CURRENT_USER,\
					_szRxAppKey, v, t, d, l)
#define RXREGSETDATA(v,t,d,l) Win_RegSetData(HKEY_CURRENT_USER,\
					_szRxAppKey, v, t, d, l)

#endif
