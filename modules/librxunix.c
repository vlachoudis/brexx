/*
 * $Id: librxunix.c,v 1.3 2013/09/02 08:25:32 bnv Exp $
 * $Log: librxunix.c,v $
 * Revision 1.3  2013/09/02 08:25:32  bnv
 * _init, _fini changed to constructor and destructor
 *
 * Revision 1.2  2011/06/28 20:49:33  bnv
 * 2.1.9 implementation
 *
 * Revision 1.1  2011/06/28 20:47:10  bnv
 * Initial revision
 *
 * Revision 1.1  2002/08/22 12:27:09  bnv
 * Initial revision
 *
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "lerror.h"
#include "lstring.h"

#include "rexx.h"
#include "rxdefs.h"

/* --------------------------------------------------------------- */
/*  CHDIR([directory])                                             */
/*     with no arguments returns current directory                 */
/*     otherwise changes the default directory                     */
/* --------------------------------------------------------------- */
void __CDECL
R_chdir( const int func )
{
	if (ARGN>1) Lerror(ERR_INCORRECT_CALL,0);
	Lfx(ARGR,PATH_MAX);
	Lscpy(ARGR,getcwd(LSTR(*ARGR),LMAXLEN(*ARGR)));
	if (ARGN==1) {
		int	err;
		L2STR(ARG1);
		LASCIIZ(*ARG1);
		err = chdir(LSTR(*ARG1));
	}
} /* R_chdir */

/* --------------------------------------------------------------- */
/*  SOCKET(host,port)                                              */
/*  Prepare the socket connection                                  */
/*  This function is just boilerplate TCP socket setup code.       */
/*  It returns an fd for a socket connected to the requested       */
/*  host and port.                                                 */
/* --------------------------------------------------------------- */
void R_socket( const int func )
{
	int i, socket_fd, port;
	struct hostent *host;
	struct sockaddr_in socket_address;

	// Check arguments
	if (ARGN!=2) Lerror(ERR_INCORRECT_CALL,0);
	get_s(1); LASCIIZ(*ARG1);
	get_i(2, port);

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		Lerror(ERR_SYSTEM_FAILURE,1);
		return;
	}

	host = gethostbyname(LSTR(*ARG1));
	if (host == NULL) {
		Lerror(ERR_SYSTEM_FAILURE,1);
		return;
	}

	MEMSET((char*)&socket_address, 0, sizeof(socket_address));

	socket_address.sin_family      = AF_INET;
//	socket_address.sin_addr.s_addr = htonl(host->h_addr);
	for (i=0; i<host->h_length; i++)
		((char *)&socket_address.sin_addr.s_addr)[i] = ((char *)host->h_addr)[i];
	socket_address.sin_port        = htons(port);


	if (connect(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
		Lerror(ERR_SYSTEM_FAILURE,1);
		return;
	}

	Licpy(ARGR, socket_fd);
} /* R_socket */

/* --------------------------------------------------------------- */
/*  LISTEN(port)                                                   */
/*  Prepare the socket connection for listening                    */
/* --------------------------------------------------------------- */
void R_listen( const int func )
{
	int i, socket_fd, port, clilen;
	struct sockaddr_in socket_address;
	struct sockaddr_in client_address;

	// Check arguments
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	get_i(1, port);

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1) {
		Lerror(ERR_SYSTEM_FAILURE,1);
		return;
	}

	MEMSET((char*)&socket_address, 0, sizeof(socket_address));

	socket_address.sin_family      = AF_INET;
	socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
	socket_address.sin_port        = htons(port);

	if (bind(socket_fd, (struct sockaddr *)&socket_address, sizeof(socket_address)) < 0) {
		Lerror(ERR_SYSTEM_FAILURE,1);
		return;
	}

	if (listen(socket_fd, 5) == -1) {
		Lerror(ERR_SYSTEM_FAILURE, 1);
		return;
	}

	Licpy(ARGR, accept(socket_fd, (struct sockaddr *)&client_address, &clilen));
} /* R_socket */

/* --------------------------------------------------------------- */
/*  SOCKETCLOSE(fd)                                                */
/* --------------------------------------------------------------- */
void R_socketClose( const int func )
{
	int socket_fd;

	// Check arguments
	if (ARGN!=1) Lerror(ERR_INCORRECT_CALL,0);
	get_i(1, socket_fd);

	if (close(socket_fd)) {
		Lerror(ERR_SYSTEM_FAILURE,1);
		return;
	}
	Licpy(ARGR,0);
} /* R_socketClose */

/* --------------------------------------------------------------- */
/*  SEND(fd, data)                                                 */
/* --------------------------------------------------------------- */
void R_send( const int func )
{
	int socket_fd, result, flag=0;

	// Check arguments
	if (ARGN<2 || ARGN>3) Lerror(ERR_INCORRECT_CALL,0);
	get_i(1, socket_fd);
	get_s(2);
	get_oi(3, flag);
	LASCIIZ(*ARG2);

	Licpy(ARGR, send(socket_fd, LSTR(*ARG2), LLEN(*ARG2), flag));
} /* R_send */

/* --------------------------------------------------------------- */
/*  RECV(fd [,length])                                             */
/* --------------------------------------------------------------- */
void R_recv( const int func )
{
	int socket_fd, result, len, flag;

	// Check arguments
	if (ARGN<1 || ARGN>3) Lerror(ERR_INCORRECT_CALL,0);
	get_i(1, socket_fd);
	get_oi(2, len);
	get_oi(3, flag);

	LTYPE(*ARGR) = LSTRING_TY;
	if (len>0) {
		Lfx(ARGR, len);
		result = recv(socket_fd, LSTR(*ARGR), len, flag);
		if (result == -1) {
			Lerror(ERR_SYSTEM_FAILURE, 1);
			return;
		}
		LLEN(*ARGR) = result;
	} else {
#define BUFLEN 128
		LLEN(*ARGR)  = 0;
		while (TRUE) {
			Lfx(ARGR, LLEN(*ARGR)+BUFLEN);	// Increase length by BUFLEN
			result = recv(socket_fd, LSTR(*ARGR) + LLEN(*ARGR), BUFLEN, flag);
			if (result == -1) {
				Lerror(ERR_SYSTEM_FAILURE, 1);
				return;
			}
			LLEN(*ARGR) += result;
			if (result<BUFLEN) break;
		}
#undef BUFLEN
	}
} /* R_recv */

/* --------------------------------------------------------------- */
/*  SLEEP(time [,"US","SEC","MIN","HOUR","DAYS"])                  */
/* --------------------------------------------------------------- */
void R_sleep( const int func )
{
	int	duration, mult;
	char	unit='S';

	if (ARGN<1 || ARGN>2) Lerror(ERR_INCORRECT_CALL,0);

	get_i(1, duration);
	if (exist(2)) {
		L2STR(ARG2);
		unit = l2u[(byte)LSTR(*ARG2)[0]];
	}

	switch (unit) {
		case 'U':
			Licpy(ARGR, usleep(duration)+1);
			return;

		case 'M':
			mult = 60;
			break;

		case 'H':
			mult = 3600;
			break;

		case 'D':
			mult = 86400;
			break;

		default:
			mult = 1;
	}
	duration *= mult;
	Licpy(ARGR, sleep(duration)+1);
} /* R_sleep */

/* --- Register functions --- */
void __CDECL
RxUnixInitialize()
{
	RxRegFunction("CHDIR"      , R_chdir      , 0);
	RxRegFunction("LISTEN"     , R_listen     , 0);
	RxRegFunction("RECV"       , R_recv       , 0);
	RxRegFunction("SEND"       , R_send       , 0);
	RxRegFunction("SLEEP"      , R_sleep      , 0);
	RxRegFunction("SOCKET"     , R_socket     , 0);
	RxRegFunction("SOCKETCLOSE", R_socketClose, 0);
//	RxRegFunction("SELECT"     , R_select     , 0);
} /* RxUnixInitialize */

void __CDECL
RxUnixFinalize()
{
} /* RxUnixFinalize */

#ifndef STATIC
/* --- Shared library init/fini functions --- */
void __attribute__ ((constructor)) _rx_init(void)
{
	RxUnixInitialize();
} /* _rx_init */

void __attribute__ ((destructor)) _rx_fini(void)
{
	RxUnixFinalize();
} /* _rx_fini */
#endif
