/*  lmpc -- the Little Movie Processing Centre
    Copyright (C) 1994-99 Uwe Girlich

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    Uwe Girlich
    Dantestrasse 20
    04159 Leipzig
    Deutschland / Germany
    E-mail: Uwe.Girlich@itp.uni-leipzig.de */

/****************************************************************************\
|  tools.h  -  header file of tools.c, general tool routines                 |
\****************************************************************************/


#ifndef __TOOLS_H
#define __TOOLS_H


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <errno.h>
#include <stdio.h>

/*
 * The number of the last error is in the BSD world the external libc variable
 * sys_nerr. The Cygnus newlibc (for win32) knows __ELASTERROR only. 
 * The following hack should go in a configure script.
 */
#if 0 
#ifdef __ELASTERROR
  #define SYS_LASTERROR __ELASTERROR
#else
  #define SYS_LASTERROR sys_nerr
#endif

#define err_base (SYS_LASTERROR+10)
#endif
/* I really hate it, but the following solution is the easiest one: */
#define SYS_LASTERROR 10000
#define err_base 20000

#define FEWARGS  err_base
#define MANYARGS err_base+ 1
#define FIREAD   err_base+ 2
#define FIWRITE  err_base+ 3
#define DEFTYPE  err_base+ 4
#define UKTYPE   err_base+ 5
#define WLMP     err_base+ 6
#define WVER     err_base+ 7 
#define UKVER    err_base+ 8
#define IMPVCH   err_base+ 9
#define ARGMIS   err_base+10
#define MANYACT  err_base+11
#define INVOPT   err_base+12
#define ILLOPT   err_base+13
#define MISIFN   err_base+14
#define MISOFN   err_base+15
#define COMP     err_base+16
#define DECOMP   err_base+17
#define NOTSUP   err_base+18
#define WDMO     err_base+19
#define WDEM     err_base+20
#define DP_SC    err_base+21
#define DP_OOB   err_base+22
#define DP_BES   err_base+23
#define DINTE    err_base+24
#define DP_UTOK  err_base+25
#define NETGEN   err_base+26
#define SHMGEN   err_base+27
#define WQWD     err_base+28
#define GENERR   err_base+29
#define NOTCOM   err_base+30
#define WDM2     err_base+31
#define TP_SC    err_base+32
#define TP_OOB   err_base+33
#define TP_UTOK  err_base+34
#define QWDINTE  err_base+35
#define DM2INTE  err_base+36
#define WDM3     err_base+37

#define LASTERR  err_base+38
#define TRUE  1
#define FALSE 0

#define LittleEndian 0
#define BigEndian    1

#define arraylength(array) ((int)(sizeof(array)/sizeof(array[0])))

typedef struct {
                 char          *filename;
                 FILE          *file;
                 long          filesize;
               } TEXT_t;

extern int EndianType;

extern char *progname;

/* general routines, which are called even from C code */
/* C code means in this connection the yacc/lex created stuff */
#ifdef __cplusplus
extern "C" {
#endif
extern void syserror(int errorcode, char* message, ...);
extern void syntaxerror(long linenumber, char *message);
#ifdef __cplusplus
}
#endif

extern void syswarning(int errorcode, char* message, ...);
extern void syntaxwarning(long linenumber, char *message);
extern char *Time2String(double time, char *buf);
extern char *Time2StringLong(double time, char *buf);
extern void copyopenfiles(FILE *f1, FILE *f2);
extern char *delspaces(char *a);
extern int EndianTest(void);


/* time */
extern double realtime(void);
extern double basetime(void);
extern double basetime_init(double time);

/* TEXT */
extern void TEXT_init(TEXT_t *l, char *filename, char *mode);
extern void TEXT_done(TEXT_t *l);

int WriteLine(FILE *f, char *s);


/* include the following games */

#ifdef ENABLE_LMP
  #define ENABLE_DOOM
  #define ENABLE_DOOM2
  #define ENABLE_HERETIC
  #define ENABLE_HEXEN
  #define ENABLE_STRIFE
#endif
  
#ifdef ENABLE_DMO
  #define ENABLE_DUKE
  #define ENABLE_REDNECK
#endif

#ifdef ENABLE_DEM
  #define ENABLE_QUAKE
#endif

#ifdef ENABLE_QWD
  #define ENABLE_QUAKEWORLD
#endif

#ifdef ENABLE_DM2
  #define ENABLE_QUAKE2
#endif

#ifdef ENABLE_DM3
  #define ENABLE_QUAKE3
#endif


#endif /* __TOOLS_H */


/*-- file end tools.h -------------------------------------------------------*/
