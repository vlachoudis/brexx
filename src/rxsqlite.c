/*
 * $Id: rxsqlite.c,v 1.1 2011/05/17 06:53:10 bnv Exp $
 * $Log: rxsqlite.c,v $
 * Revision 1.1  2011/05/17 06:53:10  bnv
 * Initial revision
 *
 */
#include <rexx.h>
#include <rxdefs.h>
#include <sqlite3.h>

static sqlite3		*sqldb=NULL;
static sqlite3_stmt	*sqlstmt=NULL;

/* --------------------------------------------------------------- */
/*  SQLOPEN(filename)                                              */
/* --------------------------------------------------------------- */
void R_sqlopen( const int func )
{
	const char *filename;

	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1); LASCIIZ(*ARG1);
	filename = LSTR(*ARG1);

	if (sqldb!=NULL) sqlite3_close(sqldb);

	Licpy(ARGR, sqlite3_open(LSTR(*ARG1), &sqldb));
	sqlstmt = NULL;
} /* R_sqliteopen */

/* --------------------------------------------------------------- */
/*  SQLCLOSE()                                                     */
/* --------------------------------------------------------------- */
void R_sqlclose( const int func )
{
	if (ARGN) Lerror(ERR_INCORRECT_CALL,0);

	Licpy(ARGR,0);
	if (sqlstmt)	Licpy(ARGR, sqlite3_finalize(sqlstmt));
	if (sqldb)	Licpy(ARGR, sqlite3_close(sqldb));
	sqldb   = NULL;
	sqlstmt = NULL;
} /* R_sqlclose */

/* --------------------------------------------------------------- */
/*  SQL(sqlcmd)                                                    */
/* --------------------------------------------------------------- */
void R_sql( const int func )
{
	int rc;
	const char *tail;

	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);

	if (!sqldb)   Lerror(ERR_DATABASE,1);

	if (sqlstmt) {
		sqlite3_finalize(sqlstmt);
		sqlstmt = NULL;
	}

	Licpy(ARGR, sqlite3_prepare_v2(sqldb, LSTR(*ARG1), LLEN(*ARG1), &sqlstmt, &tail));
	if (LINT(*ARGR) == SQLITE_OK) {
		if (sqlite3_bind_parameter_count(sqlstmt)==0) {
			rc = sqlite3_step(sqlstmt);
			if (rc == SQLITE_ROW)
				Lscpy(ARGR, "ROW");
			else
			if (rc == SQLITE_DONE)
				Licpy(ARGR, SQLITE_OK);
			else
				Licpy(ARGR, rc);
		}
	}
} /* R_sql */

/* --------------------------------------------------------------- */
/*  SQLSTEP(sqlcmd)                                                */
/* --------------------------------------------------------------- */
void R_sqlstep( const int func )
{
	int rc;
	if (ARGN!=0)  Lerror(ERR_INCORRECT_CALL,0);
	if (!sqldb)   Lerror(ERR_DATABASE,1);
	if (!sqlstmt) Lerror(ERR_DATABASE,0);
	rc = sqlite3_step(sqlstmt);
	if (rc == SQLITE_ROW)
		Lscpy(ARGR, "ROW");
	else
	if (rc == SQLITE_DONE)
		Lscpy(ARGR, "DONE");
	else
		Licpy(ARGR, rc);
} /* R_sqlstep */

/* --------------------------------------------------------------- */
/*  SQLRESET()                                                     */
/* --------------------------------------------------------------- */
void R_sqlreset( const int func )
{
	if (ARGN!=0) Lerror(ERR_INCORRECT_CALL,0);
	if (!sqldb)   Lerror(ERR_DATABASE,1);
	if (!sqlstmt) Lerror(ERR_DATABASE,0);
	Licpy(ARGR, sqlite3_reset(sqlstmt));
} /* R_sqlreset */

/* --------------------------------------------------------------- */
/*  SQLBIND(col, type, value)                                      */
/* --------------------------------------------------------------- */
void R_sqlbind( const int func )
{
	int col, i;
	double d;
	char type;

	if (!sqldb)   Lerror(ERR_DATABASE,1);
	if (!sqlstmt) Lerror(ERR_DATABASE,0);

	if (ARGN==0) {
		Licpy(ARGR, sqlite3_bind_parameter_count(sqlstmt));
		return;
	}

	if (ARGN!=3) Lerror(ERR_INCORRECT_CALL,0);

	if (Ldatatype(ARG1,'N'))
		col = Lrdint(ARG1);
	else {
		LASCIIZ(*ARG1);
		col = sqlite3_bind_parameter_index(sqlstmt, LSTR(*ARG1));
	}
	get_pad(2, type);

	switch (type) {
		case 'b': case 'B':	/* blob */
			Licpy(ARGR, sqlite3_bind_blob(sqlstmt, col,
				LSTR(*ARG3), LLEN(*ARG3), SQLITE_TRANSIENT));
			break;

		case 'i': case 'I':	/* integer */
			get_i(3, i);
			Licpy(ARGR, sqlite3_bind_int(sqlstmt, col, i));
			break;

		case 'd': case 'D':	/* double */
		case 'f': case 'F':
			L2REAL(ARG3);
			Licpy(ARGR, sqlite3_bind_double(sqlstmt, col, LREAL(*ARG3)));
			break;

		case 's': case 'S':	/* double */
		case 't': case 'T':
			L2STR(ARG3);
			Licpy(ARGR, sqlite3_bind_text(sqlstmt, col,
				LSTR(*ARG3), LLEN(*ARG3), SQLITE_TRANSIENT));
			break;

		case 'n': case 'N':	/* null */
			Licpy(ARGR, sqlite3_bind_null(sqlstmt, col));
			break;

		case 'z': case 'Z':	/* zero blob */
			get_i(3, i);
			Licpy(ARGR, sqlite3_bind_zeroblob(sqlstmt, col, i));
			break;

		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
} /* R_sqlbind */

/* --------------------------------------------------------------- */
/*  SQLGET(col)                                                    */
/* --------------------------------------------------------------- */
void R_sqlget( const int func )
{
	int col, bytes;

	if (ARGN==0) {
		Licpy(ARGR, sqlite3_column_count(sqlstmt));
		return;
	}
	if (ARGN>1) Lerror(ERR_INCORRECT_CALL,0);
	get_i0(1,col);
	col--;

	switch (sqlite3_column_type(sqlstmt, col)) {
		case SQLITE_INTEGER:
			Licpy(ARGR, sqlite3_column_int(sqlstmt, col));
			break;

		case SQLITE_FLOAT:
			Lrcpy(ARGR, sqlite3_column_double(sqlstmt, col));
			break;

		case SQLITE_TEXT:
			Lscpy(ARGR, (char*)sqlite3_column_text(sqlstmt, col));
			break;

		case SQLITE_BLOB:
			bytes = sqlite3_column_bytes(sqlstmt, col);
			Lfx(ARGR, bytes);
			LLEN(*ARGR) = bytes;
			LTYPE(*ARGR) = LSTRING_TY;
			MEMCPY(LSTR(*ARGR), sqlite3_column_blob(sqlstmt, col), bytes);
			break;

		case SQLITE_NULL:
			Lfx(ARGR,1);
			LTYPE(*ARGR)   = LSTRING_TY;
			LLEN(*ARGR)    = 1;
			LSTR(*ARGR)[0] = 0;
			break;

		default:
			Lfx(ARGR,0);
			LTYPE(*ARGR) = LSTRING_TY;
			LLEN(*ARGR)  = 0;
	}
} /* R_sqlget */

/* --------------------------------------------------------------- */
/*  Initialize/Finalize                                            */
/* --------------------------------------------------------------- */
void RxSQLiteInitialize()
{
	RxRegFunction("SQLOPEN",	R_sqlopen,	0);
	RxRegFunction("SQLCLOSE",	R_sqlclose,	0);
	RxRegFunction("SQL",		R_sql,		0);
	RxRegFunction("SQLSTEP",	R_sqlstep,	0);
	RxRegFunction("SQLRESET",	R_sqlreset,	0);
	RxRegFunction("SQLBIND",	R_sqlbind,	0);
	RxRegFunction("SQLGET",		R_sqlget,	0);
} /* RxSQLiteInitialize() */

void RxSQLiteFinalize()
{
	ARGN=0;
	R_sqlclose(0);
} /* RxSQLiteFinalize */

#ifndef RXSQLITESTATIC
/* --- Shared library init/fini functions --- */
void _init(void)
{
	RxSQLiteInitialize();
} /* _init */

void _fini(void)
{
	RxSQLiteFinalize();
} /* RxSQLiteFinalize */
#endif
