/*-----------------------------------------------------------------------*
 * filename - systemx.c
 *
 * function(s)
 *    systemx - issues an MS-DOS command
 *	and returns the command return code
 *-----------------------------------------------------------------------*/

#include <dos.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* ------------- function prototypes ------------- */
static char *BDOSenv(char **envV, char *pathP, void **envSave);
int cdecl far _spawnx(char  far *__path, char  far *__cmd, char far *__env);
/* ------------- external variables -------------- */
extern  void    cdecl (*_exitbuf)(void);

/* ------------------ systemx -------------------- */
int far
systemx(const char far *cmd)
{
	char	*cmdP;
	int	cmdS;
	char	*envP;
	void	*envSave;
	char	*pathP;
	int	rc;

//  Get COMMAND.COM path from COMSPEC

	if ((pathP = getenv("COMSPEC")) == NULL) {
		errno = ENOENT;
		return (-1);
	}

//  Build command line "LEN /C cmd\r"

	cmdS = 1 + 3 + strlen(cmd) + 1;
	if (cmdS > 128) {
		errno = E2BIG;
		return (-1);
	}
	if ((cmdP = malloc(cmdS)) == NULL) {
		errno = ENOMEM;
		return (-1);
	}

	if (cmdS == 5) {
		cmdP[0] = 0;
		cmdP[1] = '\r';
	} else {
		*cmdP++ = cmdS - 2;
		*cmdP++ = getswitchar();
		cmdP = _stpcpy(cmdP, "c ");
		cmdP = _stpcpy(cmdP, cmd);
		*cmdP++ = '\r';
		cmdP -= cmdS;
	}

//  Build environment for cmd

	if ((envP = BDOSenv(environ, pathP, &envSave)) == NULL) {
		errno = ENOMEM;
		free(cmdP);
		return (-1);
	}

//  Flush all byte streams before calling cmd

	(*_exitbuf)();

//  Now, call the low level _spawnx function

	rc = _spawnx(pathP, cmdP, envP);

//  Release all buffers, and exit

	free(envSave);
	free(cmdP);
	return (rc);
} /* systemx */

/* ------------------ BDOSenv -------------------- */
//Description     This function allocates  a buffer and fill it  with all the
//                environment  strings one  after  the  others. If  the pathP
//                variable is nonzero, then it is  appended to the end of the
//                end of  the environment assumed it  is for a spawn  or exec
//                purpose.
//
//Return value    BDOSenv  returns a  pointer  to  the environment  buffer if
//                successful, and NULL on error.
//----------------------------------------------------
static char *
BDOSenv(char **envV, char *pathP, void **envSave)
{
	char	**envW;
	unsigned	envS;
	char	*bufP;

//	Compute the environment size including  the NULL string at the
//	end of the environment. (Environment size < 32 Kbytes)

	envS = 1;
	if ((envW = envV) != NULL)
	for (envS = 0; *envW && **envW; envS += strlen(*envW++) + 1) ;
	envS++;
	if (pathP)
		envS += 2 + strlen(pathP) + 1;
	if (envS >= 0x2000)
		return (NULL);

//	Allocate a buffer

	if ((bufP = malloc(envS + 15)) != NULL) {

//		The environment MUST be paragraph aligned
//
		*envSave = bufP;
		bufP += 15;
		(*((unsigned *)&(bufP))) &= 0xFFF0;

//		Concatenate all environment strings
//
		if ((envW = envV) != NULL && *envW != NULL)
			while (*envW && **envW) {
				bufP = _stpcpy(bufP, *envW++);
				*bufP++ = '\0';
		} else
			*bufP++ = '\0';
		*bufP++ = '\0';

//		Append program name to the environment
//
		if (pathP) {
			*((short *)bufP)++ = 1;
			bufP = _stpcpy(bufP, pathP);
			*bufP++ = '\0';
		}
		return bufP - envS;
	} else
		return NULL;
} /* BDOSenv */
