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
|  tools.c  -  implementation, general tool routines                         |
\****************************************************************************/

#include <string.h>

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <sys/stat.h>
#if defined(HAVE_SYS_TIME_H)
#include <sys/time.h>
#if defined(TIME_WITH_SYS_TIME)
#include <time.h> 
#endif /* defined(TIME_WITH_SYS_TIME) */
#else /* !defined(HAVE_SYS_TIME_H) */
#include <time.h>
#endif /* !defined(HAVE_SYS_TIME_H) */ 
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <string.h> */
#include <stdarg.h>
#include "tools.h"

int EndianType;
double basetime_time;

char *my_errlist[] = {
			"too few arguments",
			"too many arguments",
			"read error",
			"write error",
			"wrong predefined game type",
			"unknown game type",
			"bad LMP file",
			"bad version byte",
			"unknown version byte",
			"impossible version change",
			"argument missing",
			"too many action parameters",
			"invalid option value",
			"illegal option",
			"missing input filename",
			"missing output filename",
			"compression error",
			"decompression error",
			"not supported yet",
			"bad DMO file",
                        "bad DEM file",
			"DEM parsing: unterminated string constant",
			"DEM parsing: constant is out-of-bounds",
			"DEM parsing: bad escape sequence",
			"DEM structure: internal error",
			"DEM parsing: unrecognized token",
                        "network problem",
                        "shared memory problem",
                        "bad QWD file",
                        "general error",
                        "not compiled in",
			"bad DM2 file",
			"text parsing: unterminated string constant",
			"text parsing: constant is out-of-bounds",
			"text parsing: unrecognized token",
			"QWD structure: internal error",
			"DM2 structure: internal error"
		       };

char *progname;


void syserror(int errorcode, char* message, ...)
{
  va_list args;
  va_start(args, message);
  syswarning(errorcode, message, args);
  va_end(args);
  exit(errorcode);
}

void syswarning(int errorcode, char* message, ...)
{
  /* I know, buffer overrun etc. etc., who really cares ? */
  char buffer[1000];
  va_list args;
  
  va_start(args, message);
  vsprintf(buffer,message,args);
  va_end(args);
  if (0<errorcode && errorcode<SYS_LASTERROR)
    fprintf(stderr,"%s: %s: %s\n", progname, buffer, strerror(errorcode));
  else
    if (err_base<=errorcode && errorcode<LASTERR) 
      fprintf(stderr,"%s: %s: %s\n", progname, buffer, my_errlist[errorcode-err_base]);
    else
      fprintf(stderr,"%s: %s: errno=%d\n", progname, buffer, errorcode);
}

void syntaxerror(long linenumber, char *message)
{
  fprintf(stderr, "%s: Syntax error in line %ld. %s.\n", progname, linenumber, message);
  exit(1);
}

void syntaxwarning(long linenumber, char *message)
{
  fprintf(stderr, "%s: Syntax warning in line %ld. %s.\n", progname, linenumber, message);
}

char *Time2String(double time, char *buf)
{
  int hour, min, secFull, sec100;
  double sec;

  hour = (int) (time/3600.0);
  min = (int)((time -3600.0*hour)/60.0);
  sec = time -3600.0*hour -60.0*min;
  secFull = (int)sec;
  sec100 = (int)((sec-secFull)*100.0);
  if (hour!=0)
    sprintf(buf, "%d:%02d:%02d.%02dh", hour, min, secFull, sec100);
  else
    if (min!=0)
      sprintf(buf, "%d:%02d.%02dm", min, secFull, sec100);
    else 
      sprintf(buf, "%d.%02ds", secFull, sec100);
  return buf;
}

char *Time2StringLong(double time, char *buf)
{
  int hour, min;
  double sec;

  hour = (int)(time/3600.0);
  min = (int)((time -3600.0*hour)/60.0);
  sec = time -3600.0*hour -60.0*min;
  if (hour!=0)
    sprintf(buf, "%d:%02d:%#.8fh", hour, min, sec);
  else
    if (min!=0)
      sprintf(buf, "%d:%#.8fm", min, sec);
    else
      sprintf(buf, "%#.8fs", sec);
  return buf;
}


void copyopenfiles(FILE *f1, FILE *f2)
{
  int c;

  while ((c=fgetc(f1))!=EOF) fputc(c,f2);
}


char *delspaces(char *a)
{
  while(strlen(a)>0 && (a[strlen(a)-1]==' '   || 
			a[strlen(a)-1]=='\t'  || 
			a[strlen(a)-1]=='\n'  ||
			a[strlen(a)-1]=='\015'   )) 
    a[strlen(a)-1]='\0';
  while(strlen(a)>0 && (a[0]==' ' || a[0]=='\t')) 
    a++;
  return a;
}


int EndianTest(void)
{
  unsigned long int CT = 0L;
  ((char*)(&CT))[0] = 1;
  return ((CT==1)?LittleEndian:BigEndian);
}


double realtime(void)
{
  #ifdef HAVE_GETTIMEOFDAY
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return tv.tv_sec + tv.tv_usec * 1e-6;
  #else
    return time(NULL);
  #endif
}


double basetime(void)
{
  return realtime() - basetime_time;
}


double basetime_init(double time)
{
  return basetime_time=time;
}
  

/******************************************************************************/
/** TEXT **********************************************************************/
/******************************************************************************/
#define TEXT_BUFFER 65536L
void TEXT_init(TEXT_t *l, char *filename, char *mode)
{
  struct stat buf;

  if ((l->filename = strdup(filename))==NULL) syserror(errno,"strdup");
  if ((l->file=fopen(l->filename, mode))==NULL) syserror(errno,l->filename);
  if (SETVBUF(l->file,NULL,_IOFBF,BUFSIZ>TEXT_BUFFER?BUFSIZ:TEXT_BUFFER)!=0) syserror(errno,"setvbuf");
  if (stat(l->filename,&buf)==-1) syserror(errno,l->filename);
  l->filesize=buf.st_size;
}

void TEXT_done(TEXT_t *l)
{
  if (fclose(l->file)!=0) syserror(errno,l->filename);
  free(l->filename);
}


int WriteLine(FILE *f, char *s)
{
  #if defined (MSDOS) || defined (_WIN32)
    return fprintf(f,"%s\015\n", s);
  #else
    return fprintf(f,"%s\n", s); 
  #endif
}


/*-- file end tools.c -------------------------------------------------------*/
