/*
 * $Id: librxjson.c,v 1.2 2013/09/02 08:25:32 bnv Exp $
 * $Log: librxjson.c,v $
 * Revision 1.2  2013/09/02 08:25:32  bnv
 * Avoid infinite loop on problematic json strings
 *
 * Revision 1.1  2011/06/28 20:47:10  bnv
 * Initial revision
 *
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "lerror.h"
#include "lstring.h"

#include "rexx.h"
#include "rxdefs.h"

#define SKIP_BLANK(j,p)	while (j[p]==' ') p++
#define MUSTBE(j,p,c)	if (j[p]==c) p++

static int jsonParseValue(const char *json, int ptr);

/* --- jsonParseNumber --- */
static int jsonParseNumber(const char *json, int ptr)
{
	if (json[ptr]=='-' || json[ptr]=='+')    ptr++;		/* accept + or -	*/
	while (json[ptr]>='0' && json[ptr]<='9') ptr++;		/* accept digits 0..9	*/
	if (json[ptr]=='.') ptr++;				/* accept .		*/
	while (json[ptr]>='0' && json[ptr]<='9') ptr++;		/* accept digits 0..9	*/
	if (json[ptr]=='e' || json[ptr]=='E')    ptr++;		/* accept e or E	*/
	if (json[ptr]=='-' || json[ptr]=='+')    ptr++;		/* accept + or -	*/
	while (json[ptr]>='0' && json[ptr]<='9') ptr++;		/* accept digits 0..9	*/
	return ptr;
} /* jsonParseNumber */

/* --- jsonParseString --- */
static int jsonParseString(const char *json, int ptr)
{
	if (json[ptr]=='"') ptr++;				/* mustbe "		*/
	while (json[ptr]) {
		if (json[ptr]=='"')
			return ptr+1;
		else
		if (json[ptr]=='\\') {
			ptr++;
			if (json[ptr] == 'u')
				ptr += 5;
			else
				ptr++;
		} else
			ptr++;
	}
	return ptr;
} /* jsonParseString */

/* --- jsonParseArray --- */
static int jsonParseArray(const char *json, int ptr)
{
	MUSTBE(json,ptr,'[');

	while (json[ptr]) {
		SKIP_BLANK(json, ptr);
		ptr = jsonParseValue(json, ptr);
		SKIP_BLANK(json, ptr);

		if (json[ptr]==']') return ptr+1;
		MUSTBE(json,ptr,',');
	}
	return ptr;
} /* jsonParseArray */

/* --- jsonParseObject --- */
static int jsonParseObject(const char *json, int ptr)
{
	MUSTBE(json,ptr,'{');

	while (json[ptr]) {
		if (json[ptr]=='}') return ptr+1;
		SKIP_BLANK(json,ptr);

		ptr = jsonParseValue(json, ptr);	/* variable	*/

		SKIP_BLANK(json,ptr);
		MUSTBE(json,ptr,':');
		SKIP_BLANK(json,ptr);

		ptr = jsonParseValue(json, ptr);	/* value	*/
		SKIP_BLANK(json,ptr);
		MUSTBE(json,ptr,',');
	}
	return ptr;
} /* jsonParseObject */

/* --- jsonParseValue --- */
static int jsonParseValue(const char *json, int ptr)
{
	if (!strncmp(json+ptr, "null",  4))	return ptr+4;
	if (!strncmp(json+ptr, "false", 5))	return ptr+5;
	if (!strncmp(json+ptr, "true",  4))	return ptr+4;
	if (json[ptr]=='"')	return jsonParseString(json, ptr);
	if (json[ptr]=='-' || (json[ptr]>='0' && json[ptr]<='9'))
				return jsonParseNumber(json, ptr);
	if (json[ptr]=='[')	return jsonParseArray(json, ptr);
	if (json[ptr]=='{')	return jsonParseObject(json, ptr);
	return ptr+1;
} /* jsonParseValue */

/* --- jsonCopyValue --- */
static void jsonCopyValue(PLstr str, const char *json, int from, int to)
{
	int len, nlen, uc;
	/* Parse the input text into an unescaped cstring, and populate item. */
	static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	if (json[from]=='"') {
		from++;			/* ignore quotes	*/
		to--;
	}

	Lfx(str, to-from);
	LTYPE(*str) = LSTRING_TY;
	LLEN(*str)  = 0;

	nlen = 0;
	while (from<to) {
		if (json[from]=='\\') {
			from++;
			switch (json[from]) {
				case 'b':
					LSTR(*str)[nlen++] = '\b';
					break;
				case 'f':
					LSTR(*str)[nlen++] = '\f';
					break;
				case 'n':
					LSTR(*str)[nlen++] = '\n';
					break;
				case 'r':
					LSTR(*str)[nlen++] = '\r';
					break;
				case 't':
					LSTR(*str)[nlen++] = '\t';
					break;
				case 'u':
					from++;
					sscanf(json+from,"%4x",&uc);
					from += 4;
					len=3;
					if (uc<0x80) len=1;
					else
					if (uc<0x800) len=2;
					switch (len) {
						case 3: LSTR(*str)[nlen++] = ((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: LSTR(*str)[nlen++] = ((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: LSTR(*str)[nlen++] =  (uc | firstByteMark[len]);
					}
					break;
				default:
					LSTR(*str)[nlen++] = json[from];
			}
			from++;
		} else
			LSTR(*str)[nlen++] = json[from++];
	}
	LLEN(*str) = nlen;
} /* jsonCopyValue */

/* --------------------------------------------------------------- */
/*  JSON(json, name [,start])                                      */
/*  return value of name variable inside a json string             */
/* --------------------------------------------------------------- */
void __CDECL
R_json( const int func )
{
	int ptr, optr, nlen;
	const char *json, *name;
	int found=FALSE;

	if (ARGN<2 || ARGN>3) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	get_s(2);
	get_oi(3,ptr);
	if (--ptr<0) ptr = 0;

	if (ptr>=LLEN(*ARG1)) {
		LTYPE(*ARGR) = LSTRING_TY;
		LLEN(*ARGR)  = 0;
		return;
	}

	LASCIIZ(*ARG1);
	LASCIIZ(*ARG2);

	json = CLSTR(*ARG1);
	name = CLSTR(*ARG2);
	nlen = strlen(name);

	LTYPE(*ARGR) = LSTRING_TY;
	LLEN(*ARGR)  = 0;

	SKIP_BLANK(json,ptr);
	if (json[ptr]=='{') ptr++;

	while (json[ptr] && json[ptr]!='}') {
		SKIP_BLANK(json,ptr);

		optr = ptr;
		ptr = jsonParseValue(json, optr);	/* name of variable	*/
		/* check value */
		if (ptr-optr-2==nlen && !strncmp(json+optr+1, name, nlen))
			found = TRUE;

		SKIP_BLANK(json,ptr);
		MUSTBE(json,ptr,':');
		SKIP_BLANK(json,ptr);

		optr = ptr;
		ptr = jsonParseValue(json, optr);
		if (found) {
			jsonCopyValue(ARGR, json, optr, ptr);
			return;
		}

		SKIP_BLANK(json,ptr);
		MUSTBE(json,ptr,',');
	}
} /* R_json */

/* --------------------------------------------------------------- */
/*  JSONFIND(json, name [,start])                                  */
/*  find location of name variable inside a json string            */
/* --------------------------------------------------------------- */
void __CDECL
R_jsonfind( const int func )
{
	int ptr, optr, nlen;
	const char *json, *name;
	int found=FALSE;

	if (ARGN<2 || ARGN>3) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	get_s(2);
	get_oi(3,ptr);
	if (--ptr<0) ptr = 0;

	if (ptr>=LLEN(*ARG1)) {
		LTYPE(*ARGR) = LINTEGER_TY;
		LINT(*ARGR)  = 0;
		return;
	}

	LASCIIZ(*ARG1);
	LASCIIZ(*ARG2);

	json = CLSTR(*ARG1);
	name = CLSTR(*ARG2);
	nlen = strlen(name);

	SKIP_BLANK(json,ptr);
	if (json[ptr]=='{') ptr++;

	while (json[ptr] && json[ptr]!='}') {
		SKIP_BLANK(json,ptr);

		optr = ptr;
		ptr = jsonParseValue(json, optr);	/* name of variable	*/
		/* check value */
		if (ptr-optr-2==nlen && !strncmp(json+optr+1, name, nlen))
			found = TRUE;

		SKIP_BLANK(json,ptr);
		MUSTBE(json,ptr,':');
		SKIP_BLANK(json,ptr);

		optr = ptr;
		ptr = jsonParseValue(json, optr);
		if (found) {
			Licpy(ARGR,optr+1);
			return;
		}

		SKIP_BLANK(json,ptr);
		MUSTBE(json,ptr,',');
	}
	Licpy(ARGR,0);
} /* R_jsonfind */

/* --------------------------------------------------------------- */
/*  JSONARRAY(json[, idx, [,start]])                               */
/*  return number of elements in array if no idx is specified      */
/*  else return index idx from array starting from 1               */
/*  assume array at location start                                 */
/* --------------------------------------------------------------- */
void __CDECL
R_jsonarray( const int func )
{
	int idx=0, ptr=0, optr, i;
	const char *json;

	if (ARGN<1 || ARGN>3) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	get_oi(2,idx);
	get_oi(3,ptr);
	if (--ptr<0) ptr = 0;

	if (ptr>=LLEN(*ARG1)) {
		LTYPE(*ARGR) = LSTRING_TY;
		LLEN(*ARGR)  = 0;
		return;
	}

	LASCIIZ(*ARG1);

	json = CLSTR(*ARG1);

	LTYPE(*ARGR) = LSTRING_TY;
	LLEN(*ARGR)  = 0;

	if (json[ptr] != '[') return;	/* Not a valid array */
	ptr++;

	i = 1;
	while (json[ptr] && json[ptr]!=']') {
		SKIP_BLANK(json,ptr);

		optr = ptr;
		ptr = jsonParseValue(json, optr);	/* name of variable	*/
		if (idx>0 && idx==i) {
			jsonCopyValue(ARGR, json, optr, ptr);
			return;
		}

		SKIP_BLANK(json,ptr);
		MUSTBE(json,ptr,',');
		i++;
	}
	Licpy(ARGR,i-1);
} /* R_jsonarray */

/* --------------------------------------------------------------- */
/*  JSONTYPE(json, pos)                                            */
/*  return type at position pos                                    */
/* --------------------------------------------------------------- */
void __CDECL
R_jsontype( const int func )
{
	int ptr;
	const char *json;

	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	get_i(2,ptr);
	if (--ptr<0) ptr = 0;

	if (ptr>=LLEN(*ARG1)) {
		LTYPE(*ARGR) = LSTRING_TY;
		LLEN(*ARGR)  = 0;
		return;
	}

	json = CLSTR(*ARG1);

	LTYPE(*ARGR) = LSTRING_TY;
	LLEN(*ARGR)  = 0;

	if (!strncmp(json+ptr, "null",  4))	Lscpy(ARGR,"null");
	else
	if (!strncmp(json+ptr, "false", 5))	Licpy(ARGR,0);
	else
	if (!strncmp(json+ptr, "true",  4))	Licpy(ARGR,1);
	else
	if (json[ptr]=='"')			Lscpy(ARGR,"string");
	else
	if (json[ptr]=='-' || (json[ptr]>='0' && json[ptr]<='9'))
				Lscpy(ARGR,"number");
	else
	if (json[ptr]=='[')	Lscpy(ARGR, "array");
	else
	if (json[ptr]=='{')	Lscpy(ARGR, "object");
} /* R_jsontype */

/* --------------------------------------------------------------- */
/*  JSONVALUE(json, pos)                                           */
/*  return type at position pos                                    */
/* --------------------------------------------------------------- */
void __CDECL
R_jsonvalue( const int func )
{
	int ptr, nptr;
	const char *json;

	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	get_i(2,ptr);
	if (--ptr<0) ptr = 0;

	json = CLSTR(*ARG1);
	if (ptr>=LLEN(*ARG1)) {
		LTYPE(*ARGR) = LSTRING_TY;
		LLEN(*ARGR)  = 0;
		return;
	}

	SKIP_BLANK(json,ptr);

	if (!strncmp(json+ptr, "null",  4))	Lscpy(ARGR,"");
	else
	if (!strncmp(json+ptr, "false", 5))	Licpy(ARGR,0);
	else
	if (!strncmp(json+ptr, "true",  4))	Licpy(ARGR,1);
	else {
		nptr = jsonParseValue(json, ptr);
		jsonCopyValue(ARGR, json, ptr, nptr);
	}
} /* R_jsonvalue */

/* --------------------------------------------------------------- */
/*  JSONNEXT(json, pos)                                            */
/*  return position of next element                                */
/* --------------------------------------------------------------- */
void __CDECL
R_jsonnext( const int func )
{
	int ptr;
	const char *json;

	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	get_i(2,ptr);
	if (--ptr<0) ptr = 0;

	if (ptr>=LLEN(*ARG1)) {
		LTYPE(*ARGR) = LINTEGER_TY;
		LINT(*ARGR)  = 0;
		return;
	}

	json = CLSTR(*ARG1);

	SKIP_BLANK(json,ptr);
	ptr = jsonParseValue(json, ptr);
	SKIP_BLANK(json,ptr);

	Licpy(ARGR,ptr+1);
} /* R_jsonnext */

/* --------------------------------------------------------------- */
/*  JSONESC(str)                                                   */
/*  Escape str to be accepted as a json object                     */
/* --------------------------------------------------------------- */
void __CDECL
R_jsonesc( const int func )
{
	unsigned char *pin, *pout;
	int len;

	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	LASCIIZ(*ARG1);

	Lfx(ARGR, LLEN(*ARG1));
	LTYPE(*ARGR) = LSTRING_TY;
	len = 0;

	for (pin=LSTR(*ARG1), pout=LSTR(*ARGR); *pin; pin++) {
		if (len+3 >= LMAXLEN(*ARGR))
			Lfx(ARGR, LMAXLEN(*ARGR)+32);
		switch (*pin) {
			case '"':
				*pout++ = '\\';
				*pout++ = '"';
				len += 2;
				break;
			case '\b':
				*pout++ = '\\';
				*pout++ = 'b';
				len += 2;
				break;
			case '\f':
				*pout++ = '\\';
				*pout++ = 'f';
				len += 2;
				break;
			case '\n':
				*pout++ = '\\';
				*pout++ = 'n';
				len += 2;
				break;
			case '\r':
				*pout++ = '\\';
				*pout++ = 'r';
				len += 2;
				break;
			case '\t':
				*pout++ = '\\';
				*pout++ = 't';
				len += 2;
				break;
			default:
				/*
				if (*pin & 0x80) {
					// convert utf to unicode
					if (len+7 >= LMAXLEN(*ARGR))
						Lfx(ARGR, LMAXLEN(*ARGR)+32);
					*pout++ = '\\';
					*pout++ = 'u';
					len += 6;
				} else {
				*/
					*pout++ = *pin;
					len++;
				//}
		}
	}
	*pout = '\0';
	LLEN(*ARGR) = STRLEN(CLSTR(*ARGR));
} /* JSONESC */

/* --- Register functions --- */
void __CDECL
RxJsonInitialize()
{
	RxRegFunction("JSON"       , R_json          , 0);
	RxRegFunction("JSONARRAY"  , R_jsonarray     , 0);
	RxRegFunction("JSONESC"    , R_jsonesc       , 0);
	RxRegFunction("JSONFIND"   , R_jsonfind      , 0);
	RxRegFunction("JSONNEXT"   , R_jsonnext      , 0);
	RxRegFunction("JSONTYPE"   , R_jsontype      , 0);
	RxRegFunction("JSONVALUE"  , R_jsonvalue     , 0);
} /* RxUnixInitialize */

void __CDECL
RxJsonFinalize()
{
} /* RxJsonFinalize */

#ifndef STATIC
/* --- Shared library init/fini functions --- */
void __attribute__ ((constructor)) _rx_init(void)
{
	RxJsonInitialize();
} /* _rx_init */

void __attribute__ ((destructor)) _rx_fini(void)
{
	RxJsonFinalize();
} /* _rx_fini */
#endif
