/*
 * $Id: librxsqlite.c,v 1.2 2013/09/02 08:25:32 bnv Exp $
 * $Log: librxsqlite.c,v $
 * Revision 1.2  2013/09/02 08:25:32  bnv
 * _init, _fini changed to constructor and destructor
 *
 * Revision 1.1  2011/06/28 20:47:10  bnv
 * Initial revision
 *
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
/*  SQLERROR()                                                     */
/* --------------------------------------------------------------- */
void R_sqlerror( const int func )
{
	if (ARGN) Lerror(ERR_INCORRECT_CALL,0);

	Lscpy(ARGR,sqlite3_errmsg(sqldb));
} /* R_sqlerror */

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
/*  SQLSTEP()                                                      */
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
			L2INT(ARG3);
			Licpy(ARGR, sqlite3_bind_int(sqlstmt, col, LINT(*ARG3)));
			break;

		case 'd': case 'D':	/* double */
		case 'f': case 'F':
			L2REAL(ARG3);
			Licpy(ARGR, sqlite3_bind_double(sqlstmt, col, LREAL(*ARG3)));
			break;

		case 's': case 'S':	/* string or text */
		case 't': case 'T':
			L2STR(ARG3);
			Licpy(ARGR, sqlite3_bind_text(sqlstmt, col,
				LSTR(*ARG3), LLEN(*ARG3), SQLITE_TRANSIENT));
			break;

		case 'n': case 'N':	/* null */
			Licpy(ARGR, sqlite3_bind_null(sqlstmt, col));
			break;

		case 'z': case 'Z':	/* zero blob */
			get_i0(3, i);
			Licpy(ARGR, sqlite3_bind_zeroblob(sqlstmt, col, i));
			break;

		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
} /* R_sqlbind */

/* --------------------------------------------------------------- */
/*  SQLGET([col[,['V','T','N']])                                   */
/* --------------------------------------------------------------- */
void R_sqlget( const int func )
{
	int col, bytes;
	char opt;

	if (ARGN==0) {
		Licpy(ARGR, sqlite3_column_count(sqlstmt));
		return;
	}
	if (ARGN>2) Lerror(ERR_INCORRECT_CALL,0);
	get_i0(1,col);
	get_pad(2,opt)
	col--;
	if (opt==' ') opt='V';	/* value */

	if (opt=='N') {
		Lscpy(ARGR, sqlite3_column_name(sqlstmt, col));
		return;
	}
	switch (sqlite3_column_type(sqlstmt, col)) {
		case SQLITE_INTEGER:
			if (opt=='T')
				Lscpy(ARGR, "INTEGER");
			else
				Licpy(ARGR, sqlite3_column_int(sqlstmt, col));
			break;

		case SQLITE_FLOAT:
			if (opt=='T')
				Lscpy(ARGR, "FLOAT");
			else
				Lrcpy(ARGR, sqlite3_column_double(sqlstmt, col));
			break;

		case SQLITE_TEXT:
			if (opt=='T')
				Lscpy(ARGR, "TEXT");
			else
				Lscpy(ARGR, (char*)sqlite3_column_text(sqlstmt, col));
			break;

		case SQLITE_BLOB:
			if (opt=='T')
				Lscpy(ARGR, "BLOB");
			else {
				bytes = sqlite3_column_bytes(sqlstmt, col);
				Lfx(ARGR, bytes);
				LLEN(*ARGR) = bytes;
				LTYPE(*ARGR) = LSTRING_TY;
				MEMCPY(LSTR(*ARGR), sqlite3_column_blob(sqlstmt, col), bytes);
			}
			break;

		case SQLITE_NULL:
			if (opt=='T')
				Lscpy(ARGR, "NULL");
			else {
				Lfx(ARGR,1);
				LTYPE(*ARGR)   = LSTRING_TY;
				LLEN(*ARGR)    = 1;
				LSTR(*ARGR)[0] = 0;
			}
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
	RxRegFunction("SQL",		R_sql,		0);
	RxRegFunction("SQLBIND",	R_sqlbind,	0);
	RxRegFunction("SQLCLOSE",	R_sqlclose,	0);
	RxRegFunction("SQLERROR",	R_sqlerror,	0);
	RxRegFunction("SQLGET",		R_sqlget,	0);
	RxRegFunction("SQLOPEN",	R_sqlopen,	0);
	RxRegFunction("SQLRESET",	R_sqlreset,	0);
	RxRegFunction("SQLSTEP",	R_sqlstep,	0);
} /* RxSQLiteInitialize() */

void RxSQLiteFinalize()
{
	ARGN=0;
	R_sqlclose(0);
} /* RxSQLiteFinalize */

#ifndef STATIC
/* --- Shared library init/fini functions --- */
void __attribute__ ((constructor)) _rx_init(void)
{
	RxSQLiteInitialize();
} /* _rx_init */

void __attribute__ ((destructor)) _rx_fini(void)
{
	RxSQLiteFinalize();
} /* _rx_fini */
#endif
