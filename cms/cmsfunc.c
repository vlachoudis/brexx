/*
  These routines provide the CMS specific interfcaes for REXX

  Dave Wade - 15-Jan-2006

  Released into the Public Domain

*/
#include "lstring.h"
#include "lerror.h"

/* following are C equivalents of flags in the NUCON macro */
/*  CMS OPtions Byte */
#define CMSOPT  0x5e0
#define NOABBREV 0x10
#define NOIMPEX 0x80
#define NOIMPCP 0x40
#define NOSTDSYN 0x20
#define NOPAGREL 0x08
#define NOVMREAD 0x04

/* MISC Flags */
#define MISFLAGS 0x5e1
#define KXSWITCH 0x80
#define KOSWITCH 0x40
#define RELPAGES 0x20
#define GRAFDEV  0x10
#define QSWITCH  0x08
#define NODDISC  0x04
#define NEGITS   0x02
#define ATTNHIT  0x01

/* Message Flags */
#define MSGFLAGS 0x5e2
#define NOTYPOUT 0x80
#define NOTYPING 0x40
#define NORDYMSG 0x20
#define NORDYTIM 0x10
#define REDERRID 0x08
#define NOERRMSG 0x04
#define NOERRTXT 0x02
#define SPECCLF  0x01

/* EXECTRAC */
#define EXECFLAGS 0x5e6
#define EXECTRAC 0x80

/* CMS Protection flags */
#define PROTFLAGS 0x5e7
#define PRFPOFF 0x80

/* SUBSET Flags */
#define SUBFLAGS 0x5e9
#define SUBACT 0x01

/* DOS Flags */
#define DOSFLAGS 0x4d8
#define DOSMODE 0x80


/*
 * Process the CMS Flag function
 *
 * Peek into the Nucleus
*/

/* ------------------ cmsflag ---------------- */
void __CDECL
CMSFLAG( const PLstr value, const PLstr flag )
{
 int optval;
unsigned char *optptr,opbyte,misbyte,msgbyte,execbyte;
unsigned char dosbyte,subbyte,protbyte;

char *sflag;

     optptr = (unsigned char *) CMSOPT ;
     opbyte=optptr[0];
     misbyte=optptr[1];
     msgbyte=optptr[2];
     execbyte=optptr[6];
     protbyte=optptr[7];
     subbyte=optptr[9];
     optptr = (unsigned char *) DOSFLAGS;
     dosbyte = optptr[0];

     L2STR(flag);
     Lupper(flag);
     sflag=LSTR(*flag);
     if(!strcmp(sflag,"ABBREV")){
         optval =( (opbyte & NOABBREV) ==0);
     }
     else if (!strcmp(sflag,"AUTOREAD")){
         optval =( (opbyte & NOVMREAD) ==0);
     }
     else if (!strcmp(sflag,"CMSTYPE")){
         optval =( (msgbyte & NOTYPING) ==0);
     }
     else if (!strcmp(sflag,"DOS")){
         optval =( (dosbyte & DOSMODE) !=0);
     }
     else if (!strcmp(sflag,"EXECTRAC")){
         optval =( (execbyte & EXECTRAC) !=0);
     }
     else if (!strcmp(sflag,"IMPCP")){
         optval =( (opbyte & NOIMPCP) ==0);
     }
     else if (!strcmp(sflag,"IMPEX")){
         optval =( (opbyte & NOIMPEX) ==0);
     }
     else if (!strcmp(sflag,"PROTECT")){
         optval =( (protbyte & PRFPOFF) ==0);
     }
     else if (!strcmp(sflag,"RELPAGE")){
         optval =( (opbyte & NOPAGREL)==0);
     }
     else if (!strcmp(sflag,"SUBSET")){
         optval = 1-( (subbyte & SUBACT)==0);
     }
     else
     {
/*        fprintf(stderr,"Illegal CMS sflag %s\n",sflag); */
          Lerror(ERR_INCORRECT_CALL,0);
     }

     Licpy(value,optval);
} /* CMSFLAG */
/*
 return the CMS line Length
*/
CMSLINE( const PLstr value )
{
     int optval;
     optval=diag24();
     Licpy(value,optval);
} /* CMS Line */
CMSUSER(const PLstr value)
{
     char userid[9];
     getusr(userid);
     userid[8]=0x00;
     Lscpy(value,userid);
} /* CMSUESER */
CMSSTORE(const PLstr value)
{
     int store;
     store=diag60();
     Licpy(value,store);
} /* CMSSTORE */
