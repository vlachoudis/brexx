/*
 * $Id: rxmysql.c,v 1.4 2011/05/17 06:53:10 bnv Exp $
 * $Log: rxmysql.c,v $
 * Revision 1.4  2011/05/17 06:53:10  bnv
 * Added SQLite
 *
 * Revision 1.3  2003/10/30 13:16:28  bnv
 * Variable name change
 *
 * Revision 1.2  2002/07/03 13:15:08  bnv
 * Corrected: Various stuf
 *
 * Revision 1.1  2002/06/06 08:24:54  bnv
 * Initial revision
 *
 */
#include <rexx.h>
#include <rxdefs.h>
#include <mysql/mysql.h>

static MYSQL	*sql = NULL;
static MYSQL_RES *sqlres=NULL;
static MYSQL_FIELD *sqlfieldtypes=NULL;
static bool	sqlnewquery = TRUE;
static long	sqlfields = -1;
static long	sqlrows = -1;

enum dbfunc {
	_enum_get,
	_enum_isnull
};

/* --- _dbfieldfromname --- */
static int _dbfieldfromname(char *name)
{
	int i;
	for (i=0; i<sqlfields; i++)
		if (!strcmp(name,sqlfieldtypes[i].name))
			return i;
	return -1;
} /* _dbfield */

/* --------------------------------------------------------------- */
/*  DBCONNECT(host,user,password,database)                         */
/* --------------------------------------------------------------- */
void R_dbconnect( const int func )
{
	char *user;
	char *password;

	if (ARGN!=4) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1); LASCIIZ(*ARG1);
	if (!exist(2))
		user = NULL;
	else {
		L2STR(ARG2); LASCIIZ(*ARG2);
		user = LSTR(*ARG2);
	}
	if (!exist(3))
		password = NULL;
	else {
		L2STR(ARG3); LASCIIZ(*ARG3);
		password = LSTR(*ARG3);
	}
	get_s(4); LASCIIZ(*ARG4);

	if (sql!=NULL) mysql_close(sql);
	sql = mysql_init(NULL);
	sql = mysql_real_connect(sql,LSTR(*ARG1),user,password,LSTR(*ARG4),
			0,NULL,0);
	if (sql!=NULL)
		Licpy(ARGR,mysql_errno(sql));
	else
		Licpy(ARGR,-1);
} /* R_dbconnect */

/* --------------------------------------------------------------- */
/*  DBCLOSE()                                                      */
/* --------------------------------------------------------------- */
void R_dbclose( const int func )
{
	if (ARGN) Lerror(ERR_INCORRECT_CALL,0);

	if (sql!=NULL)
		mysql_close(sql);

	if (sqlres!=NULL)
		mysql_free_result(sqlres);

	sql = NULL;
	sqlres = NULL;
	sqlfieldtypes = NULL;
	sqlfields = -1;
	sqlrows = -1;
} /* R_dbclose */

/* --------------------------------------------------------------- */
/*  DBERROR("A")                                                   */
/* --------------------------------------------------------------- */
void R_dberror( const int func )
{
	if (ARGN>1) Lerror(ERR_INCORRECT_CALL,0);
	if (ARGN==0) {
		if (sql)
			Licpy(ARGR,mysql_errno(sql));
		else
			Licpy(ARGR,0);
	} else {
		if (sql)
			Lscpy(ARGR,mysql_error(sql));
		else
			LZEROSTR(*ARGR);
	}
} /* R_dberror */

/* --------------------------------------------------------------- */
/*  DBESCSTR(str)                                                  */
/* --------------------------------------------------------------- */
void R_dbescstring( const int func )
{
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);
	LTYPE(*ARGR) = LSTRING_TY;
	if (sql) {
		Lfx(ARGR,LLEN(*ARG1)*2);
		LLEN(*ARGR) = mysql_real_escape_string(sql,LSTR(*ARGR),
				LSTR(*ARG1),LLEN(*ARG1));
	} else
		LLEN(*ARGR) = 0;
} /* R_dbesctring */

/* --------------------------------------------------------------- */
/*  DBSQL(sqlcmd)                                                  */
/* --------------------------------------------------------------- */
void R_dbsql( const int func )
{
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1);

	if (sql==NULL)
		Lerror(ERR_DATABASE,1);

	if (sqlres!=NULL) {
		mysql_free_result(sqlres);
		sqlres = NULL;
	}
	sqlnewquery = TRUE;

	/* must construct the real_escape string.... */
	Licpy(ARGR,mysql_real_query(sql,LSTR(*ARG1),LLEN(*ARG1)));

	if (LINT(*ARGR)==0) {
		sqlfields = mysql_field_count(sql);
		if (sqlfields) {
			sqlres = mysql_store_result(sql);
			sqlrows = (long)mysql_num_rows(sqlres);
			sqlfieldtypes = mysql_fetch_fields(sqlres);
		} else {
			sqlrows = (long)mysql_affected_rows(sql);
			sqlfieldtypes = NULL;
		}
	} else {
		sqlrows = 0;
		sqlfields = 0;
		sqlfieldtypes = NULL;
	}
} /* R_dbsql */

/* --------------------------------------------------------------- */
/*  DBFIELD(["num|fieldname"]                                      */
/*                [,"Name" - (Default) name of the field           */
/*                  "Def" - (Default) name of the field           */
/*                  "Type" - type of the field                     */
/*                  "Key"  - if it is used as a key                */
/*                  "Length" - Length of the field                 */
/*                  "Maxlength" - Maximum length of the field      */
/*                  "Null" - if it accepts null values             */
/*                  "Autoincrement" - if it is autoincrement       */
/*                  "Flags" - field flags                          */
/* --------------------------------------------------------------- */
void R_dbfield( const int func )
{
	int	f;

	struct {
		enum enum_field_types type;
		char *name;
	} fieldtypes[] = {
		{ FIELD_TYPE_TINY,	"TINY"},
		{ FIELD_TYPE_SHORT,	"SHORT"},
		{ FIELD_TYPE_LONG,	"LONG"},
		{ FIELD_TYPE_INT24,	"INT24"},
		{ FIELD_TYPE_LONGLONG,	"LONGLONG"},
		{ FIELD_TYPE_DECIMAL,	"DECIMAL"},
		{ FIELD_TYPE_FLOAT,	"FLOAT"},
		{ FIELD_TYPE_DOUBLE,	"DOUBLE"},
		{ FIELD_TYPE_TIMESTAMP,	"TIMESTAMP"},
		{ FIELD_TYPE_DATE,	"DATE"},
		{ FIELD_TYPE_NEWDATE,	"NEWDATE"},
		{ FIELD_TYPE_TIME,	"TIME"},
		{ FIELD_TYPE_DATETIME,	"DATETIME"},
		{ FIELD_TYPE_YEAR,	"YEAR"},
		{ FIELD_TYPE_STRING,	"STRING"},
		{ FIELD_TYPE_BLOB,	"BLOB"},
		{ FIELD_TYPE_TINY_BLOB,	"TINY_BLOB"},
		{ FIELD_TYPE_LONG_BLOB,	"LONG_BLOB"},
		{ FIELD_TYPE_MEDIUM_BLOB,"MEDIUM_BLOB"},
		{ FIELD_TYPE_SET,	"SET"},
		{ FIELD_TYPE_ENUM,	"ENUM"},
		{ FIELD_TYPE_VAR_STRING,"VAR_STRING"},
		{ FIELD_TYPE_NULL,	"NULL"},
		{ FIELD_TYPE_CHAR,	"CHAR"},
		{ -1, NULL}};

	if (ARGN>2) Lerror(ERR_INCORRECT_CALL,0);
	if (ARGN==0) {
		Licpy(ARGR,sqlfields);
		return;
	}

	LZEROSTR(*ARGR);
	if (sqlres==NULL) return;

	must_exist(1);
	if (Ldatatype(ARG1,'N'))
		f = Lrdint(ARG1)-1;
	else {
		LASCIIZ(*ARG1);
		f = _dbfieldfromname(LSTR(*ARG1));
		if (f<0)
			Lerror(ERR_DATABASE,2);
	}
	if (f>=sqlfields) return;
	if (!exist(2)) {
		Lscpy(ARGR,sqlfieldtypes[f].name);
		return;
	}
	L2STR(ARG2);
	switch (LSTR(*ARG2)[0]) {
		case 'D': case 'd':
			Lscpy(ARGR,sqlfieldtypes[f].def);
			break;
		case 'T': case 't': {
			int i;
			for (i=0; fieldtypes[i].name!=NULL; i++)
				if (fieldtypes[i].type==sqlfieldtypes[f].type) {
					Lscpy(ARGR,fieldtypes[i].name);
					return;
				}
				Lscpy(ARGR,"UNKNOWN");
				return;
			}
			break;
		case 'K': case 'k':
			Licpy(ARGR,IS_PRI_KEY(sqlfieldtypes[f].flags));
			break;
		case 'L': case 'l':
			Licpy(ARGR,sqlfieldtypes[f].length);
			break;
		case 'M': case 'm':
			Licpy(ARGR,sqlfieldtypes[f].max_length);
			break;
		case 'U': case 'u':
			Licpy(ARGR,IS_NOT_NULL(sqlfieldtypes[f].flags));
			break;
		case 'A': case 'a':
			Licpy(ARGR,sqlfieldtypes[f].flags&AUTO_INCREMENT_FLAG);
			break;
		case 'F': case 'f':
			Licpy(ARGR,sqlfieldtypes[f].flags);
			break;
		default:
			Lscpy(ARGR,sqlfieldtypes[f].name);
	}
} /* R_dbfield */

/* --------------------------------------------------------------- */
/*  DBGET(row,[field|fieldName])                                   */
/* --------------------------------------------------------------- */
/*  DBISNULL(row,[field|fieldName])                                */
/* --------------------------------------------------------------- */
void R_dbgetnull( const int func )
{
	long rowno, fieldno;
	static unsigned long *lengths;
	static MYSQL_ROW row;
	static long lastrow;
	char *data;

	LZEROSTR(*ARGR);

	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);

	get_i(1,rowno);	rowno--;

	must_exist(2);
	if (Ldatatype(ARG2,'N'))
		fieldno = Lrdint(ARG2)-1;
	else {
		LASCIIZ(*ARG2);
		fieldno = _dbfieldfromname(LSTR(*ARG2));
		if (fieldno<0)
			Lerror(ERR_DATABASE,2);
	}

	if (sqlres == NULL || rowno>=sqlrows || fieldno>=sqlfields)
		return;

	if (sqlnewquery || rowno != lastrow) {
		mysql_data_seek(sqlres,rowno);
		row = mysql_fetch_row(sqlres);
		lengths = mysql_fetch_lengths(sqlres);
		lastrow = rowno;
		sqlnewquery = FALSE;
	}

	data = row[fieldno];
	if (func == _enum_get) {
		if (data==NULL) return;
		Lfx(ARGR,lengths[fieldno]);
		LLEN(*ARGR) = lengths[fieldno];
		LTYPE(*ARGR) = LSTRING_TY;
		MEMCPY(LSTR(*ARGR),data,LLEN(*ARGR));
	} else
		Licpy(ARGR,data==NULL?1:0);
} /* R_dbgetnull */

/* --------------------------------------------------------------- */
/*  DBINFO('Rows'|'Fields'|'Insertid')                             */
/* --------------------------------------------------------------- */
void R_dbinfo( const int func )
{
	if (ARGN>1) Lerror(ERR_INCORRECT_CALL,0);
	if (ARGN==0) {
		if (sql!=NULL)
			Lscpy(ARGR,mysql_info(sql));
		else
			LZEROSTR(*ARGR);
		return;
	}
	L2STR(ARG1);
	switch (LSTR(*ARG1)[0]) {
		case 'R': case 'r':
			Licpy(ARGR,sqlrows);
			return;
		case 'F': case 'f':
			Licpy(ARGR,sqlfields);
			return;
		case 'I': case 'i':
			Licpy(ARGR,(unsigned long)mysql_insert_id(sql));
			return;
		default:
			Lerror(ERR_INCORRECT_CALL,0);
	}
} /* R_dbinfo */

/* --------------------------------------------------------------- */
/*  Initialize/Finalize                                            */
/* --------------------------------------------------------------- */
void RxMySQLInitialize()
{
	RxRegFunction("DBCONNECT",	R_dbconnect,	0);
	RxRegFunction("DBCLOSE",	R_dbclose,	0);
	RxRegFunction("DBERROR",	R_dberror,	0);
	RxRegFunction("DBESCSTR",	R_dbescstring,	0);
	RxRegFunction("DBFIELD",	R_dbfield,	0);
	RxRegFunction("DBGET",		R_dbgetnull,	_enum_get);
	RxRegFunction("DBISNULL",	R_dbgetnull,	_enum_isnull);
	RxRegFunction("DBSQL",		R_dbsql,	0);
	RxRegFunction("DBINFO",		R_dbinfo,	0);
} /* RxMySQLInitialize() */

void RxMySQLFinalize()
{
	ARGN=0;
	R_dbclose(0);
} /* RxMySQLFinalize */

#ifndef RXMYSQLSTATIC
/* --- Shared library init/fini functions --- */
void _init(void)
{
	RxMySQLInitialize();
} /* _init */

void _fini(void)
{
	RxMySQLFinalize();
} /* RxMySQLFinalize */
#endif
