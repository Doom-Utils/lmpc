/*  lmpc -- the Little Movie Processing Centre
    Copyright (C) 1994-2000 Uwe Girlich

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
    E-mail: Uwe.Girlich@itp.uni-leipzig */

/****************************************************************************\
|  lmpc.c  -  main source code                                               |
\****************************************************************************/


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#include "lmpc.h"
#include "tools.h"

#ifdef ENABLE_LMP
  #include "ulmp.h"
#endif
#ifdef ENABLE_DMO
  #include "udmo.h" 
#endif
#ifdef ENABLE_DEM
  #include "udem.h"
#endif
#ifdef ENABLE_QWD
  #include "uqwd.h"
#endif
#ifdef ENABLE_DM2
  #include "udm2.h"
#endif
#ifdef ENABLE_DM3
  #include "udm3.h"
#else
  int isDM3bin(char *filename) { return 0; }
  int isDM3txt(char *filename) { return 0; }
#endif

/* prototypes */

/* from the text parse code */
#ifdef __cplusplus
extern "C" {
#endif
extern FILE* yyin;
extern int yyparse(void);
#ifdef __cplusplus
}
#endif

#ifndef HAVE_MEMMEM_PROTOTYPE
  extern void *memmem(const void *haystack, size_t haystack_len,
                      const void *needle, size_t needle_len);
#endif           
void syntax(void);
void version(void);
void ActionInfoLMP(char *filename, opt_t *opt);
void ActionLMP2LS(char *lmpfilename, char *lsfilename, opt_t *opt);
void ActionLS2LMP(char *lsfilename, char *lmpfilename, int action);
void ActionConvert(char *infilename, char *outfilename, opt_t *opt);
void ActionInfoDMO(char *filename, opt_t *opt);
void ActionDMObin2DMOtxt(char *dmofilename, char *dsfilename, opt_t *opt);

void ActionInfoDEM(char *filename, opt_t *opt);
void ActionInplaceDEM(char *filename, opt_t *opt);
void ActionDEMbin2DEMbin(char *dembinfilename1, char *dembinfilename2,
       opt_t *opt);
void ActionDEMbin2DEMtxt(char *dembinfilename, char *demtxtfilename,
       opt_t *opt);
void ActionDEMtxt2DEMbin(char *demtxtfilename, char *dembinfilename,
       opt_t *opt);
void ActionDEMtxt2DEMtxt(char *demtxtfilename1, char *demtxtfilename2,
       opt_t *opt);

void ActionInfoQWD(char *filename, opt_t *opt);
void ActionQWDbin2QWDbin(char *qwdbinfilename1, char *qwdbinfilename2,
       opt_t *opt);
void ActionQWDbin2QWDtxt(char *qwdbinfilename, char *qwdtxtfilename,
       opt_t *opt);
void ActionQWDtxt2QWDbin(char *qwdtxtfilename, char *qwdbinfilename,
       opt_t *opt);
void ActionQWDtxt2QWDtxt(char *qwdtxtfilename1, char *qwdtxtfilename2,
       opt_t *opt);

void ActionInfoDM2(char *filename, opt_t *opt);
void ActionInplaceDM2(char *filename, opt_t *opt);
void ActionDM2bin2DM2bin(char *dm2binfilename1, char *dm2binfilename2,
       opt_t *opt);
void ActionDM2bin2DM2txt(char *dm2binfilename, char *dm2txtfilename,
       opt_t *opt);
void ActionDM2txt2DM2bin(char *dm2txtfilename, char *dm2binfilename,
       opt_t *opt);
void ActionDM2txt2DM2txt(char *dm2txtfilename1, char *dm2txtfilename2,
       opt_t *opt);

void ActionInfoDM3(char *filename, opt_t *opt);
void ActionInplaceDM3(char *filename, opt_t *opt);
void ActionDM3bin2DM3bin(char *dm3binfilename1, char *dm3binfilename2,
       opt_t *opt);
void ActionDM3bin2DM3txt(char *dm3binfilename, char *dm3txtfilename,
       opt_t *opt);
void ActionDM3txt2DM3bin(char *dm3txtfilename, char *dm3binfilename,
       opt_t *opt);
void ActionDM3txt2DM3txt(char *dm3txtfilename1, char *dm3txtfilename2,
       opt_t *opt);

/* special routines */
char *opt_string(opt_t *opt);

/* global variables */
int argf;
int argvf_base;


/******************************************************************************/
/** Main program **************************************************************/
/******************************************************************************/

int main(int argc, char **argv)
{
  int i;
  int c;
  char c_str[2]=" "; 
  char *pos_r, *pos_l;
  int a;
  char *optargp;
  int sep;
  opt_t opt;

  static struct option long_options[] = {
    {"help",           0, 0, 'h'},
    {"version",        0, 0, 'V'},
    {"info",           0, 0, 'i'},
    {"to-bin",         0, 0, 'l'},
    {"to-txt",         0, 0, 's'},
    {"change-version", 1, 0, 'v'},
    {"change-player",  1, 0, 'p'},
    {"add-WT",         1, 0, 'w'},
    {"remove-pause",   0, 0, 'r'}, 
    {"clear-save",     0, 0, 'c'},
    {"fix-multi",      0, 0, 'f'}, 
    {"tic",            1, 0, 't'},
    {"game",           1, 0, 'g'},
    {"mark-step",      1, 0, 'm'}, 
    {0,                0, 0, 0}
  };
  int option_index = 0;

  progname = strdup(argv[0]);

  opt.action    = 0;
  opt.option	= 0;
  opt.game      = UNKNOWN;
  opt.gamecheck = UNKNOWN;
  opt.tic_start = 0;
  opt.tic_end   = 0;
  opt.tic_step  = 1;

  EndianType=EndianTest();

  /* scan the command line */
  while (1) {
    c = getopt_long(argc, argv, "hVilsv:p:w:rcft:g:m:", long_options, &option_index);

    if (c==-1) break;
    *c_str = (char) c; 
    switch (c) {
      case ':': 
        syserror(ARGMIS,c_str); 
      break;
      case 'h':
        syntax();
      break;
      case 'V':
        version(); 
      break;
      case 'i': 
        if (opt.action) syserror(MANYACT,c_str);
        opt.action |= acInfo; 
      break;
      case 'l':
        if (opt.action) syserror(MANYACT,c_str);
        opt.action = ac2BIN;
      break;
      case 's':
        if (opt.action) syserror(MANYACT,c_str);
        opt.action = ac2TXT;
      break;
      case 'v':
        if (opt.action&(~acConvert)) syserror(MANYACT,c_str);
        opt.action |= acConvert;
        opt.option |= opVersion;
        opt.versionbyte = strtoversionbyte(optarg,0); 
        opt.versionstring = strdup(optarg);
      break;
      case 'p':
        if (opt.action&(~acConvert)) syserror(MANYACT,c_str);
        opt.action |= acConvert;
        opt.option |= opViewPlayer;
        a = sscanf(optarg,"%i",&i);
        opt.mainplayer = i;
        if (a!=1) syserror(INVOPT, optarg);
      break;
      case 'w':
        if (opt.action&(~acConvert)) syserror(MANYACT,c_str);
        opt.action |= acConvert;
        opt.option |= opAddWT;
        a = sscanf(optarg,"%li",&opt.addwt);
        if ((a!=1) || (opt.addwt<0)) syserror(INVOPT,optarg);
      break;
      case 'r':
        if (opt.action&(~acConvert)) syserror(MANYACT,c_str);
        opt.action |= acConvert;
        opt.option |= opRemovePause;
      break;
      case 'c':
        if (opt.action&(~acConvert)) syserror(MANYACT,c_str);
        opt.action |= acConvert;
        opt.option |= opClearSave;
      break;
      case 'f':
        opt.option |= opFixMulti;
      break;
      case 't':
        if (opt.option&opSelectTics) syserror(MANYARGS,c_str);
        opt.option |= opSelectTics;
        sep = (int)',';
        pos_l = strchr(optarg,sep);
        pos_r = strrchr(optarg,sep);
        if (pos_l == NULL) {
          sep = (int)':';
          pos_l = strchr(optarg,sep);
          pos_r = strrchr(optarg,sep);
        }
        if ((pos_l == NULL) || (pos_r!=pos_l)) syserror(INVOPT,optarg);
        if (pos_l == optarg) {
          opt.tic_start = 0;
        } else {
          *pos_l = '\0';
          a = sscanf(optarg,"%li",&opt.tic_start);
          if ((a!=1) || (opt.tic_start<0)) syserror(INVOPT,optarg);
          *pos_l = (char)sep;
        }
        if (*(++pos_r) != '\0') {
          a = sscanf(pos_r, "%li", &opt.tic_end);
          if ((a!=1) || (opt.tic_end<opt.tic_start)) syserror(INVOPT,pos_r);
        } else {
          opt.tic_end = 0;
        }
      break;
      case 'g':
        if (opt.option&opDefineGame) syserror(MANYARGS,c_str);
        opt.option |= opDefineGame;
        for(optargp=optarg;*optargp;optargp++)
          *optargp = (char) toupper((int)*optargp);
        if ((!strcmp(optarg,"DOOM")) || (!strcmp(optarg,"D"))) {
          opt.game=(DOOM_old|DOOM_new);
          break;
        }
        if ((!strcmp(optarg,"DOOM2")) || (!strcmp(optarg,"2"))) {
          opt.game=DOOM2; 
          break;
        }
        if ((!strcmp(optarg,"HERETIC")) || (!strcmp(optarg,"H"))) {
          opt.game=HERETIC; 
          break;
        }
        if ((!strcmp(optarg,"HEXEN")) || (!strcmp(optarg,"X"))) {
          opt.game=(HEXEN_old|HEXEN_new); 
          break;
        }
        if ((!strcmp(optarg,"STRIFE")) || (!strcmp(optarg,"S"))) {
          opt.game=STRIFE;
          break;
        }
        if ((!strcmp(optarg,"DUKE")) || (!strcmp(optarg,"N"))) {
          opt.game=DUKE_old|DUKE_new;
          break;
        }
        if ((!strcmp(optarg,"REDNECK")) || (!strcmp(optarg,"R"))) {
          opt.game=REDNECK;
          break;
        }
        if ((!strcmp(optarg,"QUAKE")) || (!strcmp(optarg,"Q"))) {
          opt.game=QUAKE;
          break;
        }
        if ((!strcmp(optarg,"QUAKEWORLD")) || (!strcmp(optarg,"W"))) {
          opt.game=QUAKEWORLD;
          break;
        }
        if ((!strcmp(optarg,"QUAKE2")) || (!strcmp(optarg,"q"))) {
          opt.game=QUAKE2;
          break;
        }
#ifdef ENABLE_DM3
        if ((!strcmp(optarg,"QUAKE3")) || (!strcmp(optarg,"A"))) {
          opt.game=QUAKE3;
          break;
        }
#endif
        syserror(INVOPT, optarg);
      break;
      case 'm':
        if (opt.option&opMarkStep) syserror(MANYARGS,c_str);
        opt.option |= opMarkStep;
        a = sscanf(optarg,"%i",&opt.tic_step);
        if (a!=1) syserror(INVOPT, optarg);
      break;
      case '?':
        exit(ILLOPT);
      break;
      default:
        syserror(EINVAL, c_str);
      break; 
    }
  }; 

  /* how many files ? */
  argf = argc-optind;   /* number of files on the command line */
  argvf_base = optind;  /* this is the first file */

  /* check the options */
  /* without options means acInfo or acInplace */
  if (!opt.action) {
    if (opt.option&opFixMulti) {
      opt.action |= acInplace;
    }
    else {
      opt.action |= acInfo;
    }
    if (!argf) syntax();  /* without any actions and files */
  }
  /* --tic only with --to-txt */
  if ((opt.option&opSelectTics) && 
      (opt.action&(~ac2TXT))) 
    syserror(ILLOPT,"--tic");
  /* --mark-step only with --to-txt */
  if ((opt.option&opMarkStep) && 
      (opt.action&(~ac2TXT))) 
    syserror(ILLOPT,"--mark-step");
  /* --game only with info, --to-txt and convert */
  /* --game can be everywhere (at least with DEM), this prevents stdin reading */
  if ((opt.option&opDefineGame) &&
      (opt.action&(~(acInfo|ac2TXT|ac2BIN|acInplace|acConvert)))) 
    syserror(ILLOPT, "--game");

  /* convert MS-DOS filenames to lower case, this may be deleted soon */
  #ifdef MSDOS
    for (i=0;i<argf;i++) { 
      char *p;
      for (p = argv[argvf_base+i]; *p; p++) *p = (char)tolower((int)*p);
    }
  #endif

  /* do the actions */
  switch (opt.action) {
    case acInfo:
      if (argf<1) syserror(MISIFN, "command line");
      for (i=0;i<argf;i++) {
        if (opt.option&opDefineGame) {
          if (opt.game & (DUKE_old|DUKE_new|REDNECK)) {
            ActionInfoDMO(argv[argvf_base+i], &opt);
            continue;
          }
          if (opt.game==QUAKE) {
            ActionInfoDEM(argv[argvf_base+i], &opt);
            continue;
          }
          if (opt.game==QUAKEWORLD) {
            ActionInfoQWD(argv[argvf_base+i], &opt);
            continue;
          }
          if (opt.game==QUAKE2) {
            ActionInfoDM2(argv[argvf_base+i], &opt);
            continue;
          }
#ifdef ENABLE_DM3          
          if (opt.game==QUAKE3) {
            ActionInfoDM3(argv[argvf_base+i], &opt);
            continue;
          }
#endif          
          /* there is no LMP type check here */
          ActionInfoLMP(argv[argvf_base+i], &opt);
          continue;
        }
        if ((opt.gamecheck=isDMO(argv[argvf_base+i]))!=0) {
          ActionInfoDMO(argv[argvf_base+i], &opt);
          continue;
        }
        if (isDEMbin(argv[argvf_base+i]) || isDEMtxt(argv[argvf_base+i])) {
          ActionInfoDEM(argv[argvf_base+i], &opt);
          continue;
        }  
        if (isQWDbin(argv[argvf_base+i]) || isQWDtxt(argv[argvf_base+i])) {
          ActionInfoQWD(argv[argvf_base+i], &opt);
          continue;
        }
        if (isDM2bin(argv[argvf_base+i]) || isDM2txt(argv[argvf_base+i])) {
          ActionInfoDM2(argv[argvf_base+i], &opt);
          continue;
        }
        if (isDM3bin(argv[argvf_base+i]) || isDM3txt(argv[argvf_base+i])) {
          ActionInfoDM3(argv[argvf_base+i], &opt);
          continue;
        }
        ActionInfoLMP(argv[argvf_base+i], &opt);
        continue; /* in fact sense-less */
      }
    break;
    case ac2TXT:
      if (argf<1) syserror(MISIFN, "command line");
      if (argf==1) syserror(MISOFN, "command line");
      if (argf>2) syserror(MANYARGS, "command line");
      if (opt.option&opDefineGame) {
        if (opt.game & (DUKE_old|DUKE_new|REDNECK)) {
          ActionDMObin2DMOtxt(argv[argvf_base], argv[argvf_base+1], &opt);
          break;
        }
        if (opt.game==QUAKE) {
          if (isDEMbin(argv[argvf_base])) {
            ActionDEMbin2DEMtxt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          } 
          else {
            ActionDEMtxt2DEMtxt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          }     
        }
        if (opt.game==QUAKEWORLD) {
          if (isQWDtxt(argv[argvf_base])) {
            ActionQWDtxt2QWDtxt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          } 
          else {
            ActionQWDbin2QWDtxt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          }     
        }
        if (opt.game==QUAKE2) {
          if (isDM2txt(argv[argvf_base])) {
            ActionDM2txt2DM2txt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          }
          else {
            ActionDM2bin2DM2txt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          }
        }
#ifdef ENABLE_DM3        
        if (opt.game==QUAKE3) {
          if (isDM3txt(argv[argvf_base])) {
            ActionDM3txt2DM3txt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          }
          else {
            ActionDM3bin2DM3txt(argv[argvf_base], argv[argvf_base+1], &opt);
            break;
          }
        }
#endif        
        /* there is no LMP type check here */
        ActionLMP2LS(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if ((opt.gamecheck=isDMO(argv[argvf_base]))!=0) {
        ActionDMObin2DMOtxt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }  
      if (isDEMtxt(argv[argvf_base])) {
        ActionDEMtxt2DEMtxt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      } 
      if (isDEMbin(argv[argvf_base])) {  
        ActionDEMbin2DEMtxt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }     
      if (isQWDtxt(argv[argvf_base])) {  
        ActionQWDtxt2QWDtxt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (isQWDbin(argv[argvf_base])) {  
        ActionQWDbin2QWDtxt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (isDM2txt(argv[argvf_base])) {  
        ActionDM2txt2DM2txt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (isDM2bin(argv[argvf_base])) {  
        ActionDM2bin2DM2txt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (isDM3txt(argv[argvf_base])) {  
        ActionDM3txt2DM3txt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (isDM3bin(argv[argvf_base])) {  
        ActionDM3bin2DM3txt(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      ActionLMP2LS(argv[argvf_base], argv[argvf_base+1], &opt);
      /* break; in fact sense-less */
    break;
    case ac2BIN:
      if (argf<1) syserror(MISIFN, "command line");
      if (argf==1) syserror(MISOFN, "command line");
      if (((opt.option&opDefineGame) && opt.game==QUAKE) ||
          (isDEMtxt(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionDEMtxt2DEMbin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (((opt.option&opDefineGame) && opt.game==QUAKE) ||
          (isDEMbin(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionDEMbin2DEMbin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (((opt.option&opDefineGame) && opt.game==QUAKEWORLD) ||
          (isQWDtxt(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionQWDtxt2QWDbin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (((opt.option&opDefineGame) && opt.game==QUAKEWORLD) ||
          (isQWDbin(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionQWDbin2QWDbin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (((opt.option&opDefineGame) && opt.game==QUAKE2) ||
          (isDM2txt(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionDM2txt2DM2bin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (((opt.option&opDefineGame) && opt.game==QUAKE2) ||
          (isDM2bin(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionDM2bin2DM2bin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
#ifdef ENABLE_DM3      
      if (((opt.option&opDefineGame) && opt.game==QUAKE3) ||
          (isDM3txt(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionDM3txt2DM3bin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
      if (((opt.option&opDefineGame) && opt.game==QUAKE3) ||
          (isDM3bin(argv[argvf_base]))) {
        if (argf!=2) syserror(MANYARGS, "command line");
        ActionDM3bin2DM3bin(argv[argvf_base], argv[argvf_base+1], &opt);
        break;
      }
#endif      
      /* argf >=2 */
      for (i=0;i<argf-1;i++) { /* count the LS files */
        /*
           the first call has to init the LMP file,
           the last one has to close it
        */
        ActionLS2LMP(argv[argvf_base+i], argv[argvf_base+argf-1],
                       (i==0?LMP_INIT:0)|(i==argf-2?LMP_END:0)); 
      }
    break;
    case acConvert:
      if (argf<1) syserror(MISIFN, "command line");
      if (argf>2) syserror(MANYARGS, "command line");
      if (
           (
             (opt.option&opDefineGame) 
              && 
             (opt.game & (DUKE_old|DUKE_new|REDNECK|QUAKE|QUAKEWORLD)) 
           ) 
           || 
           (
             isDMO   (argv[argvf_base]) || 
             isDEMtxt(argv[argvf_base]) ||
             isDEMbin(argv[argvf_base])
           )
         ) 
        syserror(NOTSUP,"game type");
      if (argf==1) 
        ActionConvert(argv[argvf_base], argv[argvf_base], &opt);
      else
        ActionConvert(argv[argvf_base], argv[argvf_base+1], &opt);
    break;
    case acInplace:
      if (argf<1) syserror(MISIFN, "command line");
      for (i=0;i<argf;i++) {
        if (isDEMbin(argv[argvf_base+i])) {
          ActionInplaceDEM(argv[argvf_base+i], &opt);
        }
        else {
          if (isDM2bin(argv[argvf_base+i])) {
            ActionInplaceDM2(argv[argvf_base+i], &opt);
          }
          else {
            syserror(NOTSUP,"game type of %s",argv[argvf_base+i]);
          }
        }
      }
    break;
  }

  return 0;
}


/******************************************************************************/
/** Utility functions *********************************************************/
/******************************************************************************/

void syntax(void)
{
  fprintf(stderr,"Little Movie Processing Centre\n");
  fprintf(stderr,"LMPC (c) U. Girlich, 1994-2000, Release %s %s (%s)\n", VERSION, DATE, COMMENT);
  fprintf(stderr,"lmpc [option ...] filename [filename ...]\n");
  fprintf(stderr,"-h, --help                   display this help and exit.\n");
  fprintf(stderr,"-V, --version                output version information and exit.\n");
  fprintf(stderr,"-i, --info                   prints out information about the LMP-files.\n");
  fprintf(stderr,"-s, --to-txt                 binary/text to text conversion.\n");
  fprintf(stderr,"-l, --to-bin                 binary/text, ... to binary conversion.\n");
  fprintf(stderr,"-v, --change-version VERSION changes the version of a LMP-file to VERSION\n");
  fprintf(stderr,"                             (0 means old DOOM).\n");
  fprintf(stderr,"-p, --change-player PLAYER   changes the recording player of a LMP-file to\n");
  fprintf(stderr,"                             PLAYER (0..3).\n");
  fprintf(stderr,"-w, --add-WT SECONDS         adds SECONDS (int) WT game tics to the LMP file\n");
  fprintf(stderr,"-r, --remove-pause           removes the game tics between PS and PE.\n");
  fprintf(stderr,"-c, --clear-save             clears SG tics.\n");
  fprintf(stderr,"-f, --fix-multi              fixes multi-level DEM and DM2 files.\n");
  fprintf(stderr,"-t, --tic FROM(,|:)TO        converts only a part of the LMP-file\n");
  fprintf(stderr,"                             (in connection with -s only).\n");
  fprintf(stderr,"-g, --game GAME              forces game (GAME=DOOM,DOOM2,HERETIC,HEXEN,STRIFE\n");
#ifdef ENABLE_DM3
  fprintf(stderr,"                             DUKE,REDNECK,QUAKE,QUAKEWORLD,QUAKE2,QUAKE3)\n");
  fprintf(stderr,"                             short: D2HXSNRQWqA (only with -(i|s|v|p|w|r|c)).\n");
#else  
  fprintf(stderr,"                             DUKE,REDNECK,QUAKE,QUAKEWORLD,QUAKE2)\n");
  fprintf(stderr,"                             short: D2HXSNRQWq (only with -(i|s|v|p|w|r|c)).\n");
#endif  
  fprintf(stderr,"-m, --mark-step STEP         marks every STEPth tic in the LS file (-s only)\n");
  fprintf(stderr,"                             STEP=0 doesn't mark any game tic; default: STEP=1.\n");
  exit(1);
}


void version(void)
{
  fprintf(stderr,"lmpc %s, %s (%s)\n", VERSION, DATE, COMMENT);
  exit(0);
}


/******************************************************************************/
/** Actions *******************************************************************/
/******************************************************************************/


/*----------------------------------------------------------------------------*/
/* LMP */


#ifdef ENABLE_LMP

void ActionInfoLMP(char *filename, opt_t *opt)
{
  LMP_t l;
  char buffer[10];
  int i;

  LMP_init(&l,filename,"rb");
  LMP_self_determine_type(&l);
  if (opt->option&opDefineGame) {
    if (l.game&opt->game)
      l.game&=opt->game;
    else
      l.game=opt->game;
  }
  LMP_readheader(&l); 
  fprintf(stderr,"%s: ",l.filename);
  fprintf(stderr,"%s %s ", l.gs, l.ns);
  if (l.game==DOOM2 || l.game==STRIFE || l.game==HEXEN_old || l.game==HEXEN_new)
    fprintf(stderr,"Map%02d", l.map); 
  else
    fprintf(stderr,"E%dM%d", l.episode, l.map);
  fprintf(stderr," skill %d, ", l.skill+1);
  if (l.playernum==1)
    fprintf(stderr,"single");
  else {
    fprintf(stderr,"%d ", l.playernum);
    if (l.game&(DOOM_new|DOOM2|STRIFE))
      switch (l.multirule) {
	case ruleCOOP:  fprintf(stderr,"cooperative "); break;
	case ruleDEATH: fprintf(stderr,"deathmatch ");  break;
	case ruleALT:   fprintf(stderr,"altdeath ");    break;
	default:   fprintf(stderr,"unknown rule "); break;
      }
    fprintf(stderr,"players"); 
  }    
  if (l.game&(HEXEN_old|HEXEN_new)) {
    fprintf(stderr," (");
    for (i=0;i<l.playernum;i++) {
      fprintf(stderr,"%s", ClassName[l.playerclass[i]]);
      if (l.playernum>1 && i<l.playernum-1) fprintf(stderr,",");
    }
    fprintf(stderr,")");
  }
  if (l.respawn) fprintf(stderr,", respawn ");
  if (l.fast)    fprintf(stderr,", fast ");
  if (l.nomonsters) fprintf(stderr,", nomonsters "); 
  fprintf(stderr,"\n");
  if (l.playernum>1) fprintf(stderr,"rec. by player %d, ", l.mainplayer);
  fprintf(stderr,"%li game tics, ", l.tics); 
  fprintf(stderr,"%s.\n", Time2String(l.time,buffer));
  LMP_done(&l);
}


void ActionLMP2LS(char *lmpfilename, char *lsfilename, opt_t *opt)
{
  LMP_t l;
  LS_t s;
  TIC_t t;
  STAT_t GF[8], GB[8], 
	 SL[8], SR[8], 
	 TL[8], TR[8];
  unsigned long wait_sum[8];
  int dms[8];
  char buffer[10];
  unsigned char p;
  int pause, empty_t, empty_c;
  double time;
  char ts[200];
  char cs[200];
  long tic, stic;
  unsigned char b;
  int wb[8], ab, cb, wn;
  int KeyOld, KeyNew; 
  int m;
  char ss[1000];
  
  LMP_init(&l,lmpfilename,"rb");
  LMP_self_determine_type(&l);
  if (opt->option&opDefineGame) {
    if (l.game&opt->game)
      l.game&=opt->game;
    else
      l.game=opt->game;
  }
  LMP_readheader(&l);
  LS_init(&s,lsfilename,"wb");
  fprintf(stderr,"%s (LMP bin) -> %s (LMP txt)\n", l.filename, s.filename);
  ab = cb = 0; wn = 8;
  switch (l.game) {
    case DOOM_new : 
    case DOOM_old : 
                     for (p=0;p<l.playernum;p++) wb[p]=0; 
                     cb = 0;
                     wn = 8;
    break;
    case DOOM2    : 
    case DOOM_new|DOOM2 : 
                     for (p=0;p<l.playernum;p++) wb[p]=8; 
                     cb = 0;
                     wn = 8;
    break;
    case HERETIC  : 
                     for (p=0;p<l.playernum;p++) wb[p]=16; 
                     ab = 0;
                     cb = 4;
                     wn = 8;
    break;
    case HEXEN_old: 
    case HEXEN_new:
                     for (p=0;p<l.playernum;p++) wb[p]=24+l.playerclass[p]*4; 
                     ab = 10; 
                     cb = 8;
                     wn = 4;
    break;
    case STRIFE   :
                     for (p=0;p<l.playernum;p++) wb[p]=36;
                     cb = 12;
                     wn = 9;
    break;
  }
  sprintf(ss,"# %s %.*s %s", l.gs, (int)(24-strlen(l.gs)), "LMP file:                ", l.filename); WriteLine(s.file,ss);
  sprintf(ss,"# Number of players:        %d", l.playernum); WriteLine(s.file,ss);
  sprintf(ss,"# Total play time:          %s", Time2String(l.time,buffer)); WriteLine(s.file,ss);
  sprintf(ss,"# Number of game tics:      %ld", l.tics); WriteLine(s.file,ss);
  WriteLine(s.file,"");
  sprintf(ss,"HeaderStart"); WriteLine(s.file,ss);
  sprintf(ss," Game:         %s", l.gs); WriteLine(s.file,ss);
  if (l.game!=HERETIC) {
    sprintf(ss," Version:      %s", l.ns); 
    WriteLine(s.file,ss);
  }
  sprintf(ss," Skill:        %d", l.skill+1); WriteLine(s.file,ss);
  if (l.game!=STRIFE) {
    sprintf(ss," Episode:      %d", l.episode); WriteLine(s.file,ss);
  }
  sprintf(ss," Map:          %d", l.map); WriteLine(s.file,ss);
  if (l.playernum>1) {
    for (p=0;p<l.playernum;p++) {
      sprintf(ss," Player%d:      %d        # %s player",p,l.num[p],ColorName[cb+l.num[p]]); WriteLine(s.file,ss);
      if (l.game&(HEXEN_old|HEXEN_new)) {
        sprintf(ss," Class%d:       %d        # %s",p,l.playerclass[p],ClassName[l.playerclass[p]]); WriteLine(s.file,ss);
      }
    }
  }
  else {
    if (l.game&(HEXEN_old|HEXEN_new)) {
      sprintf(ss," Class:        %d        # %s",l.playerclass[0],ClassName[l.playerclass[0]]); WriteLine(s.file,ss);
    }
  }
  if (l.game&DOOM_new || l.game&DOOM2 || l.game&STRIFE) {
    if (l.playernum>1) {
      switch (l.multirule) {
	case ruleCOOP : sprintf(ss," MultiRule:    Cooperative")   ; break;
	case ruleDEATH: sprintf(ss," MultiRule:    DeathMeatch")   ; break;
	case ruleALT  : sprintf(ss," MultiRule:    AltDeath")      ; break;
	default       : sprintf(ss," MultiRule:    %d",l.multirule); break;
      }
      WriteLine(s.file,ss);
    }
    if (l.respawn   !=0) {
      sprintf(ss," Respawn:      %d", l.respawn); WriteLine(s.file,ss);
    }
    if (l.fast      !=0) {
      sprintf(ss," Fast:         %d", l.fast); WriteLine(s.file,ss);
    }
    if (l.nomonsters!=0) { 
      sprintf(ss," NoMonsters:   %d", l.nomonsters); WriteLine(s.file,ss);
    }
    if (l.playernum>1 || (l.playernum==1 && l.mainplayer!=0)) {
      sprintf(ss, "ViewOfPlayer: %d", l.mainplayer); WriteLine(s.file,ss);
    }
  }
  sprintf(ss,"HeaderEnd"); WriteLine(s.file,ss);
  WriteLine(s.file,"");
  sprintf(ss,"DataStart"); WriteLine(s.file,ss);
  
  for (p=0;p<l.playernum;p++) {
    wait_sum[p] = 0;
    dms[p] = FALSE;
    STAT_init(&GF[p],"GF", 2);
    STAT_init(&GB[p],"GB", 2);
    STAT_init(&SL[p],"SL", 2);
    STAT_init(&SR[p],"SR", 2);
    STAT_init(&TL[p],"TL", 3);
    STAT_init(&TR[p],"TR", 3);
  }
  
  if (opt->tic_start==0) opt->tic_start=1;
  if (opt->tic_end==0) opt->tic_end=l.tics;
  pause=FALSE;
  for (stic=1,tic=1,p=0;stic<=l.stics;stic++,p++) {
      if (p==l.playernum) { 
        p=0;
        tic++;
      }
      time=tic*TICTIME;
      LMP_getgametic(&l,&t);
      strcpy(ts,"");
      strcpy(cs,"");
      empty_t=TRUE;
      empty_c=TRUE;
      if (opt->tic_step!=0) { 
        if (p==0 && ((tic%opt->tic_step==1)||(opt->tic_step==1))) {
          sprintf(cs, "%5ld (%s)", tic, Time2String(time,buffer));
          empty_c=FALSE;
        } 
      }
      if (t.go!=0) {
	if (!empty_t) strcat(ts," ");
	if (t.go<=0x7F) {
	  sprintf(ts+strlen(ts),"GF%d",t.go);
	  GF[p].hist[t.go]++;
	} 
	if (0x81<=t.go) {
	  sprintf(ts+strlen(ts),"GB%d",256-t.go);
	  GB[p].hist[256-t.go]++;
	} 
	if (t.go==0x80) {
	  sprintf(ts+strlen(ts),"G0x%02x",t.go);
	  if (!empty_c) strcat(cs," ");
	  strcat(cs,"Unknown Go");
	  empty_c=FALSE;
	}
	empty_t=FALSE; 
      }
      if (t.strafe!=0) {
	if (!empty_t) strcat(ts," ");
	if (t.strafe<=0x7F) {
	  sprintf(ts+strlen(ts),"SR%d",t.strafe);
	  SR[p].hist[t.strafe]++;
	}
	if (0x81<=t.strafe) { 
	  sprintf(ts+strlen(ts),"SL%d",256-t.strafe);
	  SL[p].hist[256-t.strafe]++;
	}
	if (t.strafe==0x80) {
	  sprintf(ts+strlen(ts),"S0x%02x",t.strafe);
	  if (!empty_c) strcat(cs," ");
	  strcat(cs,"Unknown Strafe");
	  empty_c=FALSE;
	}
	empty_t=FALSE;
      }
      if (t.turn!=0) {
	if (!empty_t) strcat(ts," ");
	if (t.turn<=0x7F) {
	  sprintf(ts+strlen(ts),"TL%d",t.turn);
	  TL[p].hist[t.turn]++;
	}
	if (0x81<=t.turn) {
	  sprintf(ts+strlen(ts),"TR%d",256-t.turn);
	  TR[p].hist[256-t.turn]++;
	} 
	if (t.turn==0x80) {
	  sprintf(ts+strlen(ts),"T0x%02x",t.turn);
	  if (!empty_c) strcat(cs," ");
	  strcat(cs,"DOOM mouse spinner Turn");
          dms[p]=TRUE;
	  empty_c=FALSE;
	}
	empty_t=FALSE;
      }
      if (t.use!=0) {
	if (!empty_t) strcat(ts," ");
	if (t.use & 0x80) {
	  t.use ^= 0x80;
	  if (t.use==0x01) {
	    if (pause) {
	      strcat(ts,"PE");
	      pause=FALSE;
	    }
	    else {
	      strcat(ts,"PS");
	      pause=TRUE;
	    }
	  } /* t.use==0x01 */
	  else if ((t.use & 0x03) == 0x02) {
	    sprintf(ts+strlen(ts),"SG%d",(t.use>>2) & 0x07);
	  }
	  else {
	    sprintf(ts+strlen(ts),"U0x%02x",t.use | 0x80);
	    if (!empty_c) strcat(cs," ");
	    strcat(cs,"Unknown Use");
	    empty_c=FALSE;
	  }
	} /* t.use & 0x80 */
	else {
	  switch (t.use & 0x03) {
	    case 0x01: strcat(ts,"FW ");    break;
	    case 0x02: strcat(ts,"UT ");    break;
	    case 0x03: strcat(ts,"FW UT "); break;
	  }
	  if (t.use & 0x04) {
	    b = t.use >> 3;
	    if (b>=wn) {
	      sprintf(ts+strlen(ts),"U0x%02x",t.use);
	      if (!empty_c) strcat(cs," ");
	      strcat(cs,"Unknown Weapon");
	      empty_c=FALSE;
	    }
	    else {  
	      sprintf(ts+strlen(ts),"NW%d",b+1);
	      if (!empty_c) strcat(cs," ");
              strcat(cs,WeaponName[wb[p]+b]);
	      empty_c=FALSE;
	    }
	  }
	  else {
	    if ((t.use & (~0x03)) !=0) {
	      sprintf(ts+strlen(ts),"U0x%02x",t.use);
	      if (!empty_c) strcat(cs," ");
	      strcat(cs,"Unknown Use");
	      empty_c=FALSE;
	    }
	  }
	}
	empty_t=FALSE;
      }
      if ((l.game&(HERETIC|HEXEN_old|HEXEN_new)) && t.fl!=0) {
	b=t.fl & 0x0F;
	if (b!=0) {
	  if (!empty_t) strcat(ts," ");
	  if (b<=0x07) {
	    sprintf(ts+strlen(ts),"LU%d",b);
	  }
	  if (b>=0x09) {
	    sprintf(ts+strlen(ts),"LD%d",16-b);
	  }
	  if (b==0x08) {
	    strcat(ts,"LC");
	  }
	  empty_t=FALSE;
	} 
	b=t.fl >> 4;
	if (b!=0) {
	  if (!empty_t) strcat(ts," ");
	  if (b<=0x07) {
	    sprintf(ts+strlen(ts),"FU%d",b);
	  }
	  if (b>=0x09) {
	    sprintf(ts+strlen(ts),"FD%d",16-b);
	  }
	  if (b==0x08) {
	    strcat(ts,"FC");
	  }
	  empty_t=FALSE;
	} 
      }
      if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && t.art) {
        if (l.game&(HEXEN_old|HEXEN_new) && t.art&0x80) {
	  if (!empty_t) strcat(ts," ");
          sprintf(ts+strlen(ts),"JU");
          empty_t=FALSE;
          t.art^=0x80;
        }
        if (l.game&(HEXEN_old|HEXEN_new) && t.art==0x21) {
	  if (!empty_t) strcat(ts," ");
          sprintf(ts+strlen(ts),"UA");
          empty_t=FALSE;
          if (!empty_c) strcat(cs," ");
          sprintf(ts+strlen(cs),"Use All");
          empty_c=FALSE;
        }
        else {
  	  if (((l.game==HERETIC) && (t.art<=10)) || 
              ((l.game&(HEXEN_old|HEXEN_new)) && (t.art<=15))) {
	    if (!empty_t) strcat(ts," ");
	    sprintf(ts+strlen(ts),"AT%c", t.art-1+'a');
            empty_t=FALSE;
	    if (!empty_c) strcat(cs," ");
	    sprintf(cs+strlen(cs),"%s",ArtifactName[ab+t.art-1]);
	    empty_c=FALSE; 
	  }
          else {
	    if (!empty_t) strcat(ts," ");
	    sprintf(ts+strlen(ts),"A0x%02x",t.art); 
            empty_t=FALSE;
	    if (!empty_c) strcat(cs," ");
	    strcat(cs,"Unknown Artifact");
	    empty_c=FALSE; 
	  } 
	}
      }
      if (l.game==STRIFE && t.fl!=0) {
        for (m=0;m<8;m++,t.fl>>=1) {
          if (t.fl&1) {
            if (!empty_t) strcat(ts," ");
            sprintf(ts+strlen(ts),"%s", STRIFEActionName[m]);
          }
        }
        empty_t=FALSE;
      }
      if (l.game==STRIFE && t.art!=0) {
	if (!empty_t) strcat(ts," ");
        sprintf(ts+strlen(ts),"A0x%02x",t.art); 
        empty_t=FALSE;
        if (!empty_c) strcat(cs," ");
        for (m=0;m<=STRIFEArtifacts;m++) {
          if (m==STRIFEArtifacts) {
            strcat(cs,"Unknown Artifact");
            break;
          }
          if (t.art==STRIFEArtifactName[m].value) {
            strcat(cs,STRIFEArtifactName[m].name);
            break;
          }
        }
        empty_c=FALSE; 
      }
      if (empty_t) {
	strcat(ts,"WT");
	wait_sum[p]++;
      }
      if ((tic>=opt->tic_start) && (tic<=opt->tic_end)) {
        if (empty_c) 
          sprintf(ss,"%s", ts);
        else 
   	  sprintf(ss,"%s%.*s# %s", ts, (int)(24-strlen(ts)), "                        ", cs);
        WriteLine(s.file,ss);
      }
  } /* end for stic */
  sprintf(ss,"DataEnd"); WriteLine(s.file,ss);
  if (l.tics>0) {
    m=0;
    for(p=0;p<l.playernum;p++) {
      STAT_calc(&GF[p]);
      STAT_calc(&GB[p]);
      STAT_calc(&SL[p]);
      STAT_calc(&SR[p]);
      STAT_calc(&TL[p]);
      STAT_calc(&TR[p]);
      WriteLine(s.file,"");
      sprintf(ss,"# Statistics for player %d", p); WriteLine(s.file,ss);
      if (TL[p].control==C_KEY && TR[p].control==C_KEY) {
	KeyOld=STAT_turncheck(&TL[p],1,2,5) && STAT_turncheck(&TR[p],2,3,5);
	KeyNew=STAT_turncheck(&TL[p],1,3,5) && STAT_turncheck(&TR[p],1,2,5);
	/* the following code isn't totally save. What happens, if 
	   KeyOld and KeyNew are both true ? */
	if (!KeyOld && !KeyNew) 
	  TL[p].control=C_MOUSE;
	else 
	  if (KeyOld)
	    TL[p].control=C_KEY_OLD;
	  else
	   TL[p].control=C_KEY_NEW;
	TR[p].control=TL[p].control;
      }
      STAT_writehist(&GF[p],cs); if (strcmp(cs,"")!=0) { 
        sprintf(ss,"# %s",cs); WriteLine(s.file,ss);
      }
      STAT_writehist(&GB[p],cs); if (strcmp(cs,"")!=0) {
        sprintf(ss,"# %s",cs); WriteLine(s.file,ss);
      }
      STAT_writehist(&SL[p],cs); if (strcmp(cs,"")!=0) {
        sprintf(ss,"# %s",cs); WriteLine(s.file,ss);
      }
      STAT_writehist(&SR[p],cs); if (strcmp(cs,"")!=0) {
        sprintf(ss,"# %s",cs); WriteLine(s.file,ss);
      }
      STAT_writehist(&TL[p],cs); if (strcmp(cs,"")!=0) {
        sprintf(ss,"# %s",cs); WriteLine(s.file,ss);
      }
      STAT_writehist(&TR[p],cs); if (strcmp(cs,"")!=0) {
        sprintf(ss,"# %s",cs); WriteLine(s.file,ss);
      }
      if (    GF[p].control==C_KEY && GB[p].control==C_KEY
	   && SL[p].control==C_KEY && SR[p].control==C_KEY
	   && (TL[p].control & C_KEY)
	   && (TR[p].control & C_KEY)
	 ) {
	sprintf(ss,"# This player plays with keyboard/joystick only. Give him a mouse.");
        WriteLine(s.file,ss);
      } 
      if (SR[p].sum+SL[p].sum==0) {
	sprintf(ss,"# This player plays without STRAFE. Show him the README.");
        WriteLine(s.file,ss);
      }
      if (dms[p]) {
        sprintf(ss,"# This player plays with the DOOM mouse spinner.");
        WriteLine(s.file,ss);
      }
      sprintf(ss,"# %lu * WT in %lu tics (%.1f%%)",wait_sum[p],l.tics,100.0*wait_sum[p]/l.tics);
      WriteLine(s.file,ss);
      if (TL[p].control==C_KEY_OLD && (l.game&DOOM_new || l.game&DOOM2)) m=OLD_NEW;
      if (TL[p].control==C_KEY_NEW && l.game==DOOM_old) m=NEW_OLD; 
    }
    if (m==NEW_OLD) {
      sprintf(ss,"# This LMP was originally recorded with an old version (<1.4)");
      WriteLine(s.file,ss);
    }
    if (m==OLD_NEW) {
      sprintf(ss,"# This LMP was originally recorded with a new version (>=1.4)");
      WriteLine(s.file,ss);
    }
  }
  LMP_done(&l);
  LS_done(&s);
}


#define MAXLINELENGTH 200

void ActionLS2LMP(char *lsfilename, char *lmpfilename, int action)
{
  static LMP_t l;
  LS_t s;
  TIC_t t;
  int state;
  unsigned long linenumber;
  char lsline[MAXLINELENGTH];
  char *gt, *temp, *res, *token, *value;
  int i, pause;
  int notdecoded, numberused;
  long number;
  unsigned long repeatcounter, repeati;
  char *gtp;

  LS_init(&s,lsfilename,"rb");
  if (action&LMP_INIT) LMP_init(&l,lmpfilename,"wb");
  fprintf(stderr, "%s ", s.filename);

  state=HEADER_START;
  linenumber=0;
  pause=FALSE;
  while (fgets(lsline,MAXLINELENGTH,s.file)==lsline) {
    linenumber++;
    if (lsline[strlen(lsline)-1]!='\n') {
      syntaxerror(linenumber,"line too long");
    }
    gt=lsline;
    temp=strchr(gt,'#');
    if (temp!=NULL) *temp='\0';
    for(gtp=gt;*gtp;gtp++) *gtp = toupper(*gtp); 
    gt=delspaces(gt);
    if (strlen(gt)>0) switch (state) {
      case HEADER_START:
	l.game=UNKNOWN;
	l.versionbyte=0;
	l.skill=0;
	l.episode=0;
	l.map=0;
	l.multirule=0;
	l.respawn=0;
	l.fast=0;
	l.nomonsters=0;
	l.playernum=1;
	l.mainplayer=0;
	for (i=0;i<MAXPLAYER_GLOBAL;i++) { 
          l.num[i]=0; 
          l.playerclass[i]=0;
        }
	if (strcmp(gt,"HEADERSTART")) syntaxerror(linenumber,"HeaderStart expected");
	state=HEADER;
      break; /* end HEADER_START */
      case HEADER:
	if (strcmp(gt,"HEADEREND")==0)
	  state=HEADER_END;
	else {
	  temp=strchr(gt,':');
	  if (temp==NULL) syntaxerror(linenumber,"a colon separates token and value in the header");
	  if (temp==gt) syntaxerror(linenumber,"token expected");
	  if (temp==gt+strlen(gt)-1) syntaxerror(linenumber,"value expected");
	  *temp='\0';
	  token=gt; 
	  token=delspaces(token);
	  value=temp+1;
	  value=delspaces(value);
	  notdecoded=TRUE;
	  numberused=FALSE;
	  number=strtol(value,&res,0);
	  if (notdecoded && strcmp(token,"GAME")==0) {
	    if (notdecoded && strcmp(value,"DOOM")==0) {
	      l.game=DOOM_old|DOOM_new;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    } 
	    if (notdecoded && strcmp(value,"DOOM ][")==0) {
	      l.game=DOOM2;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    } 
	    if (notdecoded && strcmp(value,"DOOM OR DOOM ][")==0) {
	      l.game=DOOM_new|DOOM2;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    } 
	    if (notdecoded && strcmp(value,"HERETIC")==0) {
	      l.game=HERETIC;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    } 
	    if (notdecoded && strcmp(value,"HEXEN")==0) {
	      l.game=HEXEN_old|HEXEN_new;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    } 
	    if (notdecoded && strcmp(value,"STRIFE")==0) {
	      l.game=STRIFE;
              l.episode=1;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    } 
	    if (notdecoded && strcmp(value,"DUKE NUKEM 3D")==0) {
              syserror(NOTSUP,"DUKE NUKEM 3D");
	    } 
	  }
	  if (notdecoded && strcmp(token,"VERSION")==0) {
            l.versionbyte=strtoversionbyte(value,l.game);
	    numberused=FALSE;
	    notdecoded=FALSE;
	  } 
	  if (notdecoded && strcmp(token,"SKILL")==0) {
	    l.skill=number-1;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"EPISODE")==0) {
	    l.episode=number;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"MAP")==0) {
	    l.map=number;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER0")==0) {
	    l.num[0]=number;
	    if (l.playernum<1) l.playernum=1;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER1")==0) {
	    l.num[1]=number;
	    if (l.playernum<2) l.playernum=2;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER2")==0) {
	    l.num[2]=number;
	    if (l.playernum<3) l.playernum=3;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER3")==0) {
	    l.num[3]=number;
	    if (l.playernum<4) l.playernum=4;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER4")==0) {
	    l.num[4]=number;
	    if (l.playernum<5) l.playernum=5;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER5")==0) {
	    l.num[5]=number;
	    if (l.playernum<6) l.playernum=6;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER6")==0) {
	    l.num[6]=number;
	    if (l.playernum<7) l.playernum=7;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"PLAYER7")==0) {
	    l.num[7]=number;
	    if (l.playernum<8) l.playernum=8;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"CLASS")==0) {
	    l.playerclass[0]=number;
	    if (l.playernum<1) l.playernum=1;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"CLASS0")==0) {
	    l.playerclass[0]=number;
	    if (l.playernum<1) l.playernum=1;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"CLASS1")==0) {
	    l.playerclass[1]=number;
	    if (l.playernum<1) l.playernum=1;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"CLASS2")==0) {
	    l.playerclass[2]=number;
	    if (l.playernum<1) l.playernum=1;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"CLASS3")==0) {
	    l.playerclass[3]=number;
	    if (l.playernum<1) l.playernum=1;
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
          if (notdecoded && strcmp(token,"CLASS4")==0) {
            l.playerclass[4]=number;
            if (l.playernum<1) l.playernum=1;
            numberused=TRUE;
            notdecoded=FALSE;
          }
          if (notdecoded && strcmp(token,"CLASS5")==0) {
            l.playerclass[5]=number;
            if (l.playernum<1) l.playernum=1;
            numberused=TRUE;
            notdecoded=FALSE;
          }
          if (notdecoded && strcmp(token,"CLASS6")==0) {
            l.playerclass[6]=number;
            if (l.playernum<1) l.playernum=1;
            numberused=TRUE;
            notdecoded=FALSE;
          }
          if (notdecoded && strcmp(token,"CLASS7")==0) {
            l.playerclass[7]=number;
            if (l.playernum<1) l.playernum=1;
            numberused=TRUE;
            notdecoded=FALSE;
          }
	  if (notdecoded && strcmp(token,"MULTIRULE")==0) {
	    if (notdecoded && strcmp(value,"COOPERATIVE")==0) {
	      l.multirule=ruleCOOP;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    }
	    if (notdecoded && strcmp(value,"DEATHMATCH")==0) {
	      l.multirule=ruleDEATH;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    }
	    if (notdecoded && strcmp(value,"ALTDEATH")==0) {
	      l.multirule=ruleALT;
	      numberused=FALSE;
	      notdecoded=FALSE;
	    }
	    if (notdecoded) { 
	      l.multirule=number;
	      numberused=TRUE;
	      notdecoded=FALSE;
	    }
	  }
	  if (notdecoded && strcmp(token,"RESPAWN")==0) {
	    l.respawn=number; 
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"FAST")==0) {
	    l.fast=number; 
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"NOMONSTERS")==0) {
	    l.nomonsters=number; 
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded && strcmp(token,"VIEWOFPLAYER")==0) {
	    l.mainplayer=number; 
	    numberused=TRUE;
	    notdecoded=FALSE;
	  }
	  if (notdecoded) syntaxerror(linenumber,"token expected");
	  if (numberused && res[0]!='\0') syntaxerror(linenumber,"number expected");
	}
      break; /* end HEADER */
      case HEADER_END:
      case DATA_START:
	if (l.game==UNKNOWN) syntaxerror(linenumber,"Game is not defined");
	if (l.game==(DOOM_old|DOOM_new)) {
	  if (l.versionbyte==0) 
	    l.game=DOOM_old;
	  else
	    l.game=DOOM_new;
	}
        if (l.game==(HEXEN_old|HEXEN_new)) {
          if (l.versionbyte==HEXEN_old)
            l.game=HEXEN_old;
          if (l.versionbyte==HEXEN_new)
            l.game=HEXEN_new;
        }
        if (l.game&(STRIFE|HEXEN_new)) 
          l.maxplayer=8;
        else 
          l.maxplayer=4;

	if (action&LMP_INIT) LMP_writeheader(&l);
	l.ticsize=((l.game&(HERETIC|HEXEN_old|HEXEN_new|STRIFE))?LONG_TIC:SHORT_TIC);
	state=DATA_START;
	if (strcmp(gt,"DATASTART")) syntaxerror(linenumber,"DataStart expected");
	state=DATA;
      break;
      case DATA:
	if (strcmp(gt,"DATAEND")==0) {
	  state=DATA_END;
	  if (action&LMP_END) LMP_writequitbyte(&l);
	  state++;
	}
	else {
	  t.go=0;
	  t.strafe=0;
	  t.turn=0;
	  t.use=0;
	  if (l.game&(HERETIC|HEXEN_old|HEXEN_new|STRIFE)) {
	    t.fl=0; 
	    t.art=0;
	  }
	  repeatcounter=1;
	  token=strtok(gt," ");
	  while (token!=NULL) {
	    notdecoded=TRUE;
	    if (notdecoded && strncmp(token,"*",1)==0) {
	      number=strtol(token+1,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"*number expected");
	      if (0<number) 
		repeatcounter=number; 
	      else
		syntaxerror(linenumber,"*number out of range");
	      notdecoded=FALSE;
	    }
	    if (notdecoded && strncmp(token,"GF",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"GFnumber expected");
	      if (0<number && number<128)
		t.go=number; 
	      else
		syntaxerror(linenumber,"GFnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"GB",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"GBnumber expected");
	      if (0<number && number<128)
		t.go=256-number;
	      else
		syntaxerror(linenumber,"GBnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"SR",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"SRnumber expected");
	      if (0<number && number<128)
		t.strafe=number; 
	      else
		syntaxerror(linenumber,"SRnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"SL",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"SLnumber expected");
	      if (0<number && number<128)
		t.strafe=256-number;
	      else
		syntaxerror(linenumber,"SLnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"TR",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"TRnumber expected");
	      if (0<number && number<128) 
		t.turn=256-number; 
	      else
		syntaxerror(linenumber,"TRnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"TL",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"TLnumber expected");
	      if (0<number && number<128)
		t.turn=number;
	      else
		syntaxerror(linenumber,"TLnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"PS",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"PS expected");
	      if (pause) syntaxwarning(linenumber,"Pause is activ already");
              pause=TRUE;
	      if (t.use!=0 && t.use!=0x81) syntaxerror(linenumber,"PS can't be used with other use things");
	      t.use=0x81; 
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"PE",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"PE expected");
	      if (!pause) syntaxwarning(linenumber,"Pause is off already");
              pause=FALSE;
	      if (t.use!=0 && t.use!=0x81) syntaxerror(linenumber,"PE can't be used with other use things");
	      t.use=0x81; 
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"SG",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"SGnumber expected");
	      if (0<=number && number<=7) {
		if (t.use!=0) syntaxerror(linenumber,"SG can't be used with other use things");
		t.use=0x82 | (number<<2);
	      }
	      else
		syntaxerror(linenumber,"SGnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"FW",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"FW expected");
	      if (t.use & 0x80) syntaxerror(linenumber,"FW can't be used with PS, PE and SG");
	      t.use |= 0x01;
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"UT",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"UT expected");
	      if (t.use & 0x80) syntaxerror(linenumber,"UT can't be used with PS, PE and SG");
	      t.use |= 0x02;
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"NW",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"NWnumber expected");
	      if (1<=number && number<=8) {
		if (t.use & 0x80) syntaxerror(linenumber,"NW can't be used with PS, PE and SG"); 
		t.use = (t.use & 0x03) | 0x04 | ((number-1)<<3);
	      }
	      else
		syntaxerror(linenumber,"NWnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"WT",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"WT expected");
	      notdecoded=FALSE;
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"LU",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"LUnumber expected");
	      if (0<number && number<16)
		t.fl=(t.fl & 0xF0) | number; 
	      else
		syntaxerror(linenumber,"LUnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"LD",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"LDnumber expected");
	      if (0<number && number<16)
		t.fl=(t.fl & 0xF0) | (16-number);
	      else
		syntaxerror(linenumber,"LDnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"LC",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"LC expected");
	      t.fl=(t.fl & 0xF0) | 0x08; 
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"FU",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"FUnumber expected");
	      if (0<number && number<16)
		t.fl=(t.fl & 0x0F) | (number<<4); 
	      else
		syntaxerror(linenumber,"FUnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"FD",2)==0) {
	      number=strtol(token+2,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"FDnumber expected");
	      if (0<number && number<16)
		t.fl=(t.fl & 0x0F) | ((16-number)<<4);
	      else
		syntaxerror(linenumber,"FDnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"FC",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"FC expected");
	      t.fl=(t.fl & 0x0F) | 0x80; 
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"AT",2)==0) {
	      if (strlen(token)!=3) syntaxerror(linenumber,"ATchar expected");
	      number=token[2]-'A'+1; 
	      if (1<=number && number<=10) 
		t.art|=number;
	      else
		syntaxerror(linenumber,"ATchar out of range");
	      notdecoded=FALSE; 
	    }
            if (l.game&(HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"JU",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"JU expected");
	      t.art |= 0x80;
	      notdecoded=FALSE; 
            }
            if (l.game&(HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"UA",2)==0) {
	      if (strlen(token)>2) syntaxerror(linenumber,"UA expected");
	      t.art |= 0x21;
	      notdecoded=FALSE; 
            }
	    if (notdecoded && strncmp(token,"G",1)==0) {
	      number=strtol(token+1,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"Gnumber expected");
	      if (0<number && number<256) {
		if (t.go!=0) syntaxerror(linenumber,"Gnumber overwrites go-byte");
		t.go=number;
	      }
	      else
		syntaxerror(linenumber,"Gnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"S",1)==0) {
	      number=strtol(token+1,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"Snumber expected");
	      if (0<number && number<256) {
		if (t.strafe!=0) syntaxerror(linenumber,"Snumber overwrites strafe-byte");
		t.strafe=number;
	      }
	      else
		syntaxerror(linenumber,"Snumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"T",1)==0) {
	      number=strtol(token+1,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"Tnumber expected");
	      if (0<number && number<256) {
		if (t.turn!=0) syntaxerror(linenumber,"Tnumber overwrites turn-byte");
		t.turn=number;
	      }
	      else
		syntaxerror(linenumber,"Tnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (notdecoded && strncmp(token,"U",1)==0) {
	      number=strtol(token+1,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"Unumber expected");
	      if (0<number && number<256) {
		if (t.use!=0) syntaxerror(linenumber,"Unumber overwrites use-byte");
		t.use=number;
	      }
	      else
		syntaxerror(linenumber,"Unumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new) && notdecoded && strncmp(token,"F",1)==0) {
	      number=strtol(token+1,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"Fnumber expected");
	      if (0<number && number<256) {
		if (t.fl!=0) syntaxerror(linenumber,"Fnumber overwrites flight/look-byte");
		t.fl=number;
	      }
	      else
		syntaxerror(linenumber,"Fnumber out of range");
	      notdecoded=FALSE; 
	    }
	    if (l.game&(HERETIC|HEXEN_old|HEXEN_new|STRIFE) && notdecoded && strncmp(token,"A",1)==0) {
	      number=strtol(token+1,&res,0);
	      if (res[0]!='\0') syntaxerror(linenumber,"Anumber expected");
	      if (0<number && number<256) {
		if (t.art!=0) syntaxerror(linenumber,"Anumber overwrites use artifact-byte");
		t.art=number;
	      }
	      else
		syntaxerror(linenumber,"Anumber out of range");
	      notdecoded=FALSE; 
	    }
            if (l.game&STRIFE && notdecoded) {
              if (strlen(token)==2) {
                for (i=0;i<8;i++) {
                  if (strncmp(token,STRIFEActionName[i],2)==0) {
                    t.fl|=(1<<i);
                    notdecoded=FALSE; 
                    break;
                  }
                }
              }
            }
	    if (notdecoded) syntaxerror(linenumber,"Data token expected");       
	    token=strtok(NULL," ");
	  }
	  for (repeati=0;repeati<repeatcounter;repeati++) LMP_putgametic(&l, &t);
	}
      break;
      case DATA_END:
        /* ignore all the rest */
      break;
    } /* end switch (state) */
  } /* end while not end of file */
  LS_done(&s);
  if (action&LMP_END) {
    fprintf(stderr," (LMP txt) -> %s (LMP bin)\n", l.filename);
    LMP_done(&l); 
  }
}


void ActionConvert(char *infilename, char *outfilename, opt_t *opt)
{
  LMP_t li, lo;
  int NewFile, NewTics, NewHeader; 
  int o_game;
  char *gs, *o_version;
  long wttics=0, tic, stic, ps=0;
  int p;
  TIC_t t;
  TIC_t WT = {0,0,0,0,0,0};
  int currentpause=FALSE;
  int newpause=FALSE;
  #define TMP_FILE "lmpclmpc.tmp"

  LMP_init(&li, infilename, "rb");
  LMP_self_determine_type(&li);
  if (opt->option&opDefineGame) {
    if (li.game&opt->game)
      li.game&=opt->game;
    else
      li.game=opt->game;
  }
  LMP_readheader(&li);
  fprintf(stderr,"%s (LMP bin) ", li.filename);
  NewFile=FALSE;
  NewTics=FALSE;
  NewHeader=FALSE;
  o_game=li.game;
  if (strcmp(infilename,outfilename)) NewFile=TRUE;
  if (opt->option&opVersion) {
    if (li.versionbyte==opt->versionbyte) {
      opt->option^=opVersion; /* nothing to do */
    }
    else {
      NewHeader=TRUE;
      if (li.game&(HERETIC)) syserror(IMPVCH,li.filename);
      if (li.game&(HEXEN_old|HEXEN_new)) {
        if (opt->versionbyte==0) 
          opt->versionbyte=strtoversionbyte(opt->versionstring,li.game);
        if (opt->versionbyte!=HEXEN_old && opt->versionbyte!=HEXEN_new) 
          syserror(IMPVCH,li.filename);
        if (li.game==HEXEN_old && opt->versionbyte==HEXEN_new) {
          o_game=HEXEN_new;
          NewFile=TRUE;
        }
        if (li.game==HEXEN_new && opt->versionbyte==HEXEN_old) {
          if (li.playernum>MAXPLAYER_HEXEN_old) syserror(IMPVCH,li.filename);
          o_game=HEXEN_old;
          NewFile=TRUE;
        }
      }
      else if (opt->versionbyte==0) {
        if (li.game==DOOM2||li.game==STRIFE) syserror(IMPVCH,li.filename);
        if (li.game&DOOM_new) {
          o_game=DOOM_old; 
          NewFile=TRUE;
        }
      }
      else {
        if (li.game==DOOM_old) {
          o_game=DOOM_new;
          NewFile=TRUE;
        }
      } 
      getversion(opt->versionbyte,&o_game,&gs,&o_version); 
      fprintf(stderr,"version %s -> %s ", li.ns, o_version);
    }
  }
  if (opt->option&opViewPlayer) {
    if (li.game&(DOOM_old|HERETIC|HEXEN_old|HEXEN_new)) syserror(IMPVCH,li.filename);
    if (li.mainplayer==opt->mainplayer) {
      opt->option^=opViewPlayer; /* nothing to do */
    }
    else {
      NewHeader=TRUE;
    }
    fprintf(stderr,"player %i -> %i ", li.mainplayer, opt->mainplayer);
  }
  if (opt->option&opAddWT) {
    if (opt->addwt==0) {
      opt->option^=opAddWT; /* nothing to do */
    }
    wttics = opt->addwt * 35 * li.playernum;   
    fprintf(stderr,"+%lisec ", opt->addwt);
  }
  if (opt->option&opRemovePause) {
    NewFile=TRUE;
    NewTics=TRUE;
    ps = 0;
    fprintf(stderr,"-PS -PE ");
  }
  if (opt->option&opClearSave) {
    NewFile=TRUE;
    NewTics=TRUE;
    fprintf(stderr,"-SG ");
  }
  fprintf(stderr,"%s (LMP bin)\n", outfilename);
  if (NewFile) {
    lo=li;
    LMP_init(&lo, TMP_FILE, "wb");
  }
  else {
    if (fclose(li.file)!=0) syserror(errno,li.filename);
    if ((li.file=fopen(li.filename, "r+b"))==NULL) syserror(errno,li.filename);
    lo=li;
  }
  lo.game=o_game;
  if (opt->option&opVersion) {
    lo.versionbyte=opt->versionbyte;
  }
  if (opt->option&opViewPlayer) {
    lo.mainplayer=opt->mainplayer;
  }

  /* header */
  if (NewHeader || NewFile) LMP_writeheader(&lo);

  /* game tics */
  if (NewFile) {
    if (NewTics) {
      currentpause = FALSE, newpause=FALSE;
      for (stic=1,tic=1,p=0;stic<=li.stics;stic++,p++) {
        if (p==li.playernum) {
          p=0;
          tic++;
          currentpause = newpause;
        } 
        LMP_getgametic(&li,&t);
        
        if (opt->option&opClearSave) {
          if ((t.use&0x82)==0x82) {
            t.use=0x00;
          }
        } /* end ClearSave */

        if (opt->option&opRemovePause) {
          if (t.use==0x81) {
            if (currentpause) {
              newpause=FALSE; /* PE */
              /* the PE game tic will not be stored */
              fprintf(stderr,", PE at %lu (%lu tics removed)\n",tic, tic-ps);
            }
            else { /* not pause */
              newpause=TRUE; /* PS */
              t.use=0x00;
              /* the PS game tic will be stored (use=empty) */
              fprintf(stderr,"PS at %lu",tic);
              ps=tic;
            }
          }
        } /* end RemovePause */

        if (!currentpause) LMP_putgametic(&lo,&t); 

      } /* end for stics */
    } /* not NewTics */
    else {
      copyopenfiles(li.file,lo.file);
      fseek(lo.file, -1, SEEK_END);  /* seek after the last tic */
    } /* end if NewTics */ 
  }
  else { /* not NewFile */
    fseek(lo.file, -1, SEEK_END);    /* seek after the last tic */
  } /* end if NewFile */

  if (opt->option&opAddWT) {
    for (tic=0;tic<wttics;tic++) LMP_putgametic(&lo, &WT);
  } 
  
  if (NewFile || (opt->option&opAddWT)) LMP_writequitbyte(&lo);
  LMP_done(&lo);
  if (NewFile) {
    LMP_done(&li);
    if (rename(TMP_FILE, outfilename)==-1) syserror(errno, outfilename);
  }
}


#else /* !ENABLE_LMP */


void ActionInfoLMP(char *filename, opt_t *opt)
{ syserror(NOTCOM,"LMP"); }

void ActionLMP2LS(char *lmpfilename, char *lsfilename, opt_t *opt)
{ syserror(NOTCOM,"LMP"); }

void ActionLS2LMP(char *lsfilename, char *lmpfilename, int action)
{ syserror(NOTCOM,"LMP"); }

void ActionConvert(char *infilename, char *outfilename, opt_t *opt)
{ syserror(NOTCOM,"LMP"); }


#endif /* ENABLE_LMP */



/*----------------------------------------------------------------------------*/
/* DMO */


#ifdef ENABLE_DMO


void ActionInfoDMO(char *filename, opt_t *opt)
{
  DMO_t d;
  char buffer[10];

  DMO_init(&d,filename,"rb");
  if (opt->option&opDefineGame) {
    d.game=isDMO(filename);
    if (!(d.game&opt->game))
      d.game=opt->game;
  }
  else {
    if (opt->gamecheck!=UNKNOWN)
      d.game=opt->gamecheck;
    else
      d.game=isDMO(filename);
  }
  DMO_readheader(&d); 
  fprintf(stderr,"%s: %s %s ", d.filename, d.gs, d.vs);
  fprintf(stderr,"E%dM%d", d.episode, d.map);
  fprintf(stderr," skill %d, ", d.skill);
  if (d.playernum==1)
    fprintf(stderr,"single");
  else {
    fprintf(stderr,"%d players", d.playernum);  
  }    
  if (d.game==DUKE_new) {
    if (d.t1) fprintf(stderr, " /t1");
    if (d.t2) fprintf(stderr, " /t2");
    if (d.t3) fprintf(stderr, " /t3");
    if (d.m) fprintf(stderr, " /m");
    if (d.playernum>1) {
      fprintf(stderr, " /c%i", d.multirule);
    }
  }
  if (d.game==REDNECK) {
    if (d.t1) fprintf(stderr, " /t1");
    if (d.t2) fprintf(stderr, " /t2");
    if (d.t3) fprintf(stderr, " /t3");
    if (d.a) fprintf(stderr, " /a");
    if (d.m) fprintf(stderr, " /m");
    if (d.playernum>1) {
      fprintf(stderr, " /c%i", d.multirule);
    }
    if (strcmp(d.name,"LEONARD")!=0) {
      fprintf(stderr, " -name %s", d.name);
    }
  }
  fprintf(stderr,"\n");
  fprintf(stderr,"%li game tics, ", d.tics);
  fprintf(stderr,"%s.\n", Time2String(d.time,buffer));
  DMO_done(&d);
}


void ActionDMObin2DMOtxt(char *dmofilename, char *dsfilename, opt_t *opt)
{
  DMO_t d;
  CHU_t chu;
  unsigned long i;
  int j;
  long tics;
  TEXT_t s;
  DMO_TIC_t t;
  char buffer[10];
  unsigned char p;
  int empty_t, empty_c;
  double time;
  char ts[200];
  char cs[200];
  long tic;
  char ss[1000];
  int weapon;

  DMO_init(&d,dmofilename,"rb");
  if (opt->option&opDefineGame) {
    d.game=isDMO(dmofilename);
    if (!(d.game&opt->game))
      d.game=opt->game;
  }
  else {
    if (opt->game!=UNKNOWN)
      d.game=opt->game;
    else
      d.game=isDMO(dmofilename);
  }
  DMO_readheader(&d); 

  TEXT_init(&s,dsfilename,"wb");
  fprintf(stderr,"%s (DMO bin) -> %s (DMO txt)\n", d.filename, s.filename);
  sprintf(ss,"# %s %.*s %s", d.gs, (int)(24-strlen(d.gs)), "DMO file:            ", d.filename); WriteLine(s.file,ss);
  sprintf(ss,"# Total play time:          %s", Time2String(d.time,buffer)); WriteLine(s.file,ss);
  sprintf(ss,"# Number of game tics:      %lu", d.tics); WriteLine(s.file,ss);
  WriteLine(s.file,"");
  sprintf(ss,"HeaderStart"); WriteLine(s.file,ss);
  sprintf(ss," Game:              %s", d.gs); WriteLine(s.file,ss);
  sprintf(ss," Version:           %s", d.vs); WriteLine(s.file,ss);
  sprintf(ss," Skill:             %d", d.skill); WriteLine(s.file,ss);
  sprintf(ss," Episode:           %d", d.episode); WriteLine(s.file,ss);
  sprintf(ss," Map:               %d", d.map); WriteLine(s.file,ss);
  sprintf(ss," Player:            %d", d.playernum); WriteLine(s.file,ss);
  if (d.game&DUKE_new) {
    if (d.playernum>1) {
      switch (d.multirule) {
        case 1:  sprintf(ss, " MultiRule:         DukeMatch(spawn)")   ; break;
        case 2:  sprintf(ss, " MultiRule:         Coop")                 ; break;
        case 3:  sprintf(ss, " MultiRule:         Dukematch(no spawn)") ; break;
        default: sprintf(ss, " MultiRule:         %d", d.multirule)      ; break;
      }
      WriteLine(s.file,ss);
    }
  }
  if (d.game&REDNECK) {
    if (d.playernum>1) {
      switch (d.multirule) {
        case 1:  sprintf(ss, " MultiRule:         RedneckMatch(spawn)")    ; break;
        case 2:  sprintf(ss, " MultiRule:         Coop")                   ; break;
        case 3:  sprintf(ss, " MultiRule:         Redneckmatch(no spawn)") ; break;
        default: sprintf(ss, " MultiRule:         %d", d.multirule)        ; break;
      }
      WriteLine(s.file,ss);
    }
  }
  if (d.game&(DUKE_new|REDNECK)) {
    if (d.t1) {
      sprintf(ss," Respawn Monsters:  %ld", d.t1); WriteLine(s.file,ss);
    }
    if (d.t2) {
      sprintf(ss," Respawn Items:     %ld", d.t2); WriteLine(s.file,ss);
    }
    if (d.t3) {
      sprintf(ss," Respawn Inventory: %ld", d.t3); WriteLine(s.file,ss);
    }
    if (d.m) {
      sprintf(ss," NoMonsters:        %ld", d.m); WriteLine(s.file,ss);
    }
  }
  if (d.game&REDNECK) {
    if (d.a) {
      sprintf(ss," FakePlayerAI:      %ld", d.a); WriteLine(s.file,ss);
    }
    sprintf(ss,  " Name:              \"%s\"", d.name); WriteLine(s.file,ss);
  }

  /*
  sprintf(ss," Header-Raw:   ");
  for (i=0;i<d.headersize;i++) {
    sprintf(buffer,"%02X ", (unsigned) d.header[i]);
    strcat(ss,buffer);
  }
  WriteLine(s.file,ss);
  */
  WriteLine(s.file,"HeaderEnd");

  WriteLine(s.file,""); 

  WriteLine(s.file,"DataStart");
  fseek(d.file, d.headersize, SEEK_SET);
  tics=0;
  if (opt->tic_start==0) opt->tic_start=1;
  if (opt->tic_end==0) opt->tic_end=d.tics;
  tic=1; p=0; 
  
  while (tics<d.tics) {
    DMO_readmacroblock(&d,&chu);
    tics+=chu.tics;
    for (i=0;i<chu.tics*d.playernum;i++,p++) {
      if (p==d.playernum) {
        p=0;
        tic++;
      }
      time=tic*DMO_TICTIME;
      /* sign ok ??? */
      t.go_x = ((short)chu.u_buffer[i*DMO_TIC+0]     )  |
               ((short)chu.u_buffer[i*DMO_TIC+1] << 8);
      t.go_y = ((short)chu.u_buffer[i*DMO_TIC+2]     )  | 
               ((short)chu.u_buffer[i*DMO_TIC+3] << 8);
      t.turn = ((short)chu.u_buffer[i*DMO_TIC+4]     )  |
               ((short)chu.u_buffer[i*DMO_TIC+5] << 8);
      t.use = ((unsigned long)chu.u_buffer[i*DMO_TIC+6]     )  |
              ((unsigned long)chu.u_buffer[i*DMO_TIC+7] << 8)  |
              ((unsigned long)chu.u_buffer[i*DMO_TIC+8] << 16) |
              ((unsigned long)chu.u_buffer[i*DMO_TIC+9] << 24);  
      strcpy(ts,"");
      strcpy(cs,"");
      empty_t=TRUE;
      empty_c=TRUE;
      if (opt->tic_step!=0) { 
        if (p==0 && ((tic%opt->tic_step==1)||(opt->tic_step==1))) {
          sprintf(cs, "%5ld (%s)", tic, Time2String(time,buffer));
          empty_c=FALSE;
        } 
      }
      if (t.go_x!=0) {
        if (!empty_t) strcat(ts," ");
        sprintf(ts+strlen(ts),"GX%hd",t.go_x);
        empty_t=FALSE;
      }
      if (t.go_y!=0) {
        if (!empty_t) strcat(ts," ");
        sprintf(ts+strlen(ts),"GY%hd",t.go_y);
        empty_t=FALSE;
      }
      if (t.turn!=0) {
        if (!empty_t) strcat(ts," ");
        if (t.turn>0) 
          sprintf(ts+strlen(ts),"TR%hd",t.turn);
        if (t.turn<0)
          sprintf(ts+strlen(ts),"TL%hd",-t.turn);
        /* should never occur ! */
        /*
        if (t.turn>200 || t.turn<-200) {
          if (!empty_c) strcat(cs," ");
          sprintf(cs+strlen(cs),"T0x%02X%02X", 
            chu.u_buffer[i*DMO_TIC+5],  chu.u_buffer[i*DMO_TIC+4]);
          empty_c=FALSE;
        }
        */
        empty_t=FALSE;
      }
      weapon = (t.use >> 8) & 0x0F; 
      t.use &= 0xFFFFF0FFL;
      if (weapon) {
        if (!empty_t) strcat(ts," ");
        sprintf(ts+strlen(ts),"NW%i", weapon);
        empty_t=FALSE;
      }
      if (t.use!=0) {
        for (j=0;j<32;j++,t.use>>=1) {
          if (t.use&1) {
            if (!empty_t) strcat(ts," ");
            sprintf(ts+strlen(ts),"%s", DMOActionName[j]);
            empty_t=FALSE;
          }
        }
      }
      if (empty_t) {
        strcat(ts,"WT");
      }
      if ((tic>=opt->tic_start) && (tic<=opt->tic_end)) {
        if (empty_c) 
          sprintf(ss,"%s", ts);
        else 
          sprintf(ss,"%s%.*s# %s", ts, (int)(40-strlen(ts)), "                                      ", cs);
        WriteLine(s.file,ss);
      }
    }
  }
  if ((unsigned long)ftell(d.file)!=d.filesize) syserror(FIREAD,d.filename);
  WriteLine(s.file,"DataEnd");
  DMO_done(&d);
  TEXT_done(&s);
}


#else /* !ENABLE_DMO */


void ActionInfoDMO(char *filename, opt_t *opt)
{ syserror(NOTCOM,"DMO"); }

void ActionDMObin2DMOtxt(char *dmofilename, char *dsfilename, opt_t *opt)
{ syserror(NOTCOM,"DMO"); }


#endif /* ENABLE_DMO */


/*----------------------------------------------------------------------------*/
/* DEM */


#ifdef ENABLE_DEM


void ActionInfoDEM(char *filename, opt_t *opt)
{
  unsigned long tics;

  /* I check here again for text or bin, text check is much faster */
  if (isDEMtxt(filename)) {
    fprintf(stderr,"%s: Quake (DEM txt)\n", filename);
  }
  else {
    if ((tics=isDEMbin(filename))!=0) {
      fprintf(stderr,"%s: Quake (DEM bin), %li frames\n", filename, tics);
    }
    else {
      syswarning(WDEM,filename);
    }
  }
}


void ActionInplaceDEM(char *filename, opt_t *opt)
{
  DEM_t d;
  node *n;
  unsigned char buffer[DEM_MAX_MESSAGE_SIZE];
  size_t blockbase;
  size_t blocksize;
  unsigned char *bufferend; 
  int changed;
  unsigned char *bufferbase;
  unsigned char *bufferpos;
   
  #define FixMultiString "\011reconnect\n"
  #define FixMultiStringLength (strlen(FixMultiString)+1)
     
  if (isDEMbin(filename)) {   
    DEM_init(&d, filename, "r+b");
  
    n=do_track_read_bin(&d);
    node_delete(n);
    fprintf(stderr,"%s (DEM bin) -> %s (DEM bin)%s\n", d.filename, d.filename,
                   opt_string(opt));
    while ((blockbase=ftell(d.file)) < d.filesize) {
      /*
      fprintf(stdout,"blockbase=0x%lx, ", (unsigned long)blockbase);
      fflush(stdout);
      */
      /* I don't check return-values because of the successful isDEMbin */
      fread(buffer+0,1,4,d.file);
      /* blocksize = *(unsigned long *)(buffer); */
      /* endian-independent variant */
      blocksize = ((buffer[0])    ) +
                  ((buffer[1])<< 8) +
                  ((buffer[2])<<16) +
                  ((buffer[3])<<24);
      fread(buffer+4,1,12+blocksize,d.file);
      bufferend = buffer + 4 + 12 + blocksize; 
      changed = 0;

      if (opt->option&opFixMulti) {
        for (bufferbase=buffer + 4 + 12;
             bufferbase < bufferend;
             bufferbase++) {
          /* Note the parameter order! 
             The Linux memmem(3) man page is wrong */
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FixMultiString, FixMultiStringLength))!=NULL) {
            /*
            fprintf(stdout,"found: ");
            fprintf(stdout,"0x%02x ",*bufferpos); 
            fprintf(stdout,"L=%i, value=%i\n", FixMultiStringLength, c_nop); 
            fprintf(stdout,"b=0x%x, b=0x%x, p=0x%x\n", buffer, bufferbase, bufferpos);
            fflush(stdout);
            */
            /* overwrite the reconnect command with some nop commands */
            memset(bufferpos,c_nop,FixMultiStringLength); 
            changed++;
            bufferbase+=FixMultiStringLength;
          }
          else {
            break; /* stop the inner-most for-loop */
          }
        } /* end for */       
      } /* end opFixMulti */
      /* here may come some other change or repair code */
      if (changed) {
        /* something has changed, let's write it back in the file */
        fseek(d.file, blockbase, SEEK_SET);
        fwrite(buffer,1,4+12+blocksize,d.file);
      }
      /* fprintf(stdout,"\n"); fflush(stdout); */
    } 
    DEM_done(&d);
  }
  else {
    syswarning(WDEM,filename);
  }
}

void ActionDEMbin2DEMbin(char *dembinfilename1, char *dembinfilename2,
     opt_t *opt)
{
  DEM_t d1, d2;
  BB_t m;
  node *n;
  int o;

  DEM_init(&d1,dembinfilename1,"rb");
  DEM_init(&d2,dembinfilename2,"wb");
  DEM_prepare_write_bin(&d2);
  node_token_init(DEM_token); /* for token routines */
  glob_opt = opt; /* for DEM_block_edit */
  do_block_edit = DEM_block_edit;
  do_block_output = DEM_block_write_bin;
  
  fprintf(stderr, "%s (DEM bin) -> %s (DEM bin)%s\n", 
                   dembinfilename1, dembinfilename2, opt_string(opt));
  
  n=do_track_read_bin(&d1);
  do_block(n);
  node_delete(n);
  if (opt->option & opMarkStep) o=0x01; else o=0x00;

  d1.frame=0;
  while ((unsigned long)ftell(d1.file)<d1.filesize) {
    DEM_block_read_bin(&d1, &m);
    n=DEM_bin_to_node(&m,o);
    do_block(n);
    node_delete(n);
    d1.frame++;
  }

  DEM_done(&d1);
  DEM_done(&d2);
}

void ActionDEMbin2DEMtxt(char *dembinfilename, char *demtxtfilename, opt_t *opt)
{
  DEM_t d;
  TEXT_t s;
  char ts[1000];
  BB_t m;
  node *n;
  int o;

  DEM_init(&d,dembinfilename,"rb");
  TEXT_init(&s,demtxtfilename,"wb");
  node_write_text_init(&s); /* for DEM_block_write_text */
  node_token_init(DEM_token); /* for token routines */
  glob_opt = opt; /* for DEM_block_edit */
  do_block_edit = DEM_block_edit;
  do_block_output = DEM_block_write_text;

  fprintf(stderr,"%s (DEM bin) -> %s (DEM txt)%s\n", d.filename, s.filename,
                 opt_string(opt));

  sprintf(ts,"// source: DEM binary file %s", d.filename); WriteLine(s.file, ts);
  WriteLine(s.file,"");

  n=do_track_read_bin(&d);
  do_block(n);
  node_delete(n);
  if (opt->option & opMarkStep) o=0x01; else o=0x00;
  
  d.frame=0;
  while ((unsigned long)ftell(d.file)<d.filesize) {
    DEM_block_read_bin(&d, &m);
    n=DEM_bin_to_node(&m,o);
    do_block(n);
    node_delete(n);
    d.frame++;
  }
  DEM_done(&d);
  TEXT_done(&s);
}

void ActionDEMtxt2DEMtxt(char *demtxtfilename1, char *demtxtfilename2, 
     opt_t *opt)
{
  TEXT_t s1;
  TEXT_t s2;
  char ts[1000];

  TEXT_init(&s1,demtxtfilename1,"rb");
  TEXT_init(&s2,demtxtfilename2,"wb");
  node_write_text_init(&s2); /* for DEM_block_write_text */
  node_token_init(DEM_token); /* for token routines */
  glob_opt = opt; /* for DEM_block_edit */
  do_block_edit = DEM_block_edit;
  do_block_output = DEM_block_write_text;
      
  fprintf(stderr, "%s (DEM txt) -> %s (DEM txt)%s\n", 
                   demtxtfilename1, demtxtfilename2,opt_string(opt));

  sprintf(ts,"// source: DEM text file %s", demtxtfilename1); WriteLine(s2.file, ts);
  WriteLine(s2.file,"");

  yyin = s1.file;
  yyparse();

  TEXT_done(&s1);
  TEXT_done(&s2);
}

void ActionDEMtxt2DEMbin(char *demtxtfilename, char *dembinfilename,
     opt_t *opt)
{
  TEXT_t s;
  DEM_t d;

  TEXT_init(&s,demtxtfilename,"rb");
  DEM_init(&d,dembinfilename,"wb");
  DEM_prepare_write_bin(&d);
  node_token_init(DEM_token); /* for token routines */
  glob_opt = opt; /* for DEM_block_edit */
  do_block_edit = DEM_block_edit;
  do_block_output = DEM_block_write_bin;

  fprintf(stderr, "%s (DEM txt) -> %s (DEM bin)%s\n",
                   demtxtfilename, dembinfilename, opt_string(opt));

  yyin = s.file;
  yyparse();

  TEXT_done(&s);
  DEM_done(&d);
}


#else /* !ENABLE_DEM */


void ActionInfoDEM(char *filename, opt_t *opt)
{ syserror(NOTCOM,"DEM"); }

void ActionInplaceDEM(char *filename, opt_t *opt)
{ syserror(NOTCOM,"DEM"); }

void ActionDEMbin2DEMbin(char *dembinfilename1, char *dembinfilename2, 
       opt_t *opt)
{ syserror(NOTCOM,"DEM"); }

void ActionDEMbin2DEMtxt(char *dembinfilename, char *demtxtfilename, 
       opt_t *opt)
{ syserror(NOTCOM,"DEM"); }
       
void ActionDEMtxt2DEMbin(char *demtxtfilename, char *dembinfilename, 
       opt_t *opt)
{ syserror(NOTCOM,"DEM"); }
       
void ActionDEMtxt2DEMtxt(char *demtxtfilename1, char *demtxtfilename2, 
       opt_t *opt)
{ syserror(NOTCOM,"DEM"); }
       

#endif /* ENABLE_DEM */


/*---------------------------------------------------------------------------*/
/* QWD */


#ifdef ENABLE_QWD


void ActionInfoQWD(char *filename, opt_t *opt)
{
  unsigned long tics;

  /* I check here again for text or bin, text check is much faster */
  if (isQWDtxt(filename)) {
    fprintf(stderr,"%s: QuakeWorld (QWD txt)\n", filename);
  }
  else {
    if ((tics=isQWDbin(filename))!=0) {
      fprintf(stderr,"%s: QuakeWorld (QWD bin), %li frames\n", filename, tics);
    }
    else {
      syswarning(WQWD,filename);
    }
  }
}

void ActionQWDbin2QWDbin(char *qwdbinfilename1, char *qwdbinfilename2,
       opt_t *opt)
{
  QWD_t d1, d2;
  BB_t m;
  node *n;
  int o;

  QWD_init(&d1,qwdbinfilename1,"rb");
  QWD_init(&d2,qwdbinfilename2,"wb");
  QWD_prepare_write_bin(&d2);
  node_token_init(QWD_token); /* for token routines */
  glob_opt = opt; /* for QWD_block_edit */
  do_block_edit = QWD_block_edit;
  do_block_output = QWD_block_write_bin;
  
  fprintf(stderr, "%s (QWD bin) -> %s (QWD bin)%s\n",
                  qwdbinfilename1, qwdbinfilename2, opt_string(opt));

  if (opt->option & opMarkStep) o=0x01; else o=0x00;

  d1.frame = 0;
  while ((unsigned long)ftell(d1.file)<d1.filesize) {
    QWD_block_read_bin(&d1, &m);
    n=QWD_bin_to_node(&m,o);
    do_block(n);
    node_delete(n);
    d1.frame++;
  }
  
  QWD_done(&d1);
  QWD_done(&d2);
}

void ActionQWDbin2QWDtxt(char *qwdbinfilename, char *qwdtxtfilename, 
       opt_t *opt)
{
  QWD_t d;
  TEXT_t s;
  char ts[1000];
  BB_t m;
  node *n;
  int o;

  QWD_init(&d,qwdbinfilename,"rb");
  TEXT_init(&s,qwdtxtfilename,"wb");
  node_write_text_init(&s); /* for node_write_text */
  node_token_init(QWD_token); /* for token routines */

  fprintf(stderr,"%s (QWD bin) -> %s (QWD txt)\n", d.filename, s.filename);

  sprintf(ts,"// source: QWD binary file %s", d.filename); WriteLine(s.file, ts);
  WriteLine(s.file,"");

  if (opt->option & opMarkStep) o=0x01; else o=0x00;
  
  d.frame=0;
  while ((unsigned long)ftell(d.file)<d.filesize) {
    QWD_block_read_bin(&d, &m);
    n=QWD_bin_to_node(&m,o);
    node_write_text(n,0);
    node_delete(n);
    d.frame++;
  }
  QWD_done(&d);
  TEXT_done(&s);
}

void ActionQWDtxt2QWDbin(char *qwdtxtfilename, char *qwdbinfilename,
       opt_t *opt)
{
  TEXT_t s;
  QWD_t d;

  TEXT_init(&s,qwdtxtfilename,"rb");
  QWD_init(&d,qwdbinfilename,"wb");
  QWD_prepare_write_bin(&d);
  node_token_init(QWD_token); /* for token routines */
  glob_opt = opt; /* for QWD_block_edit */
  do_block_edit = QWD_block_edit;
  do_block_output = QWD_block_write_bin;

  fprintf(stderr, "%s (QWD txt) -> %s (QWD bin)%s\n", 
                   qwdtxtfilename, qwdbinfilename, opt_string(opt));

  yyin = s.file;
  yyparse();

  TEXT_done(&s);
  QWD_done(&d);
}

void ActionQWDtxt2QWDtxt(char *qwdtxtfilename1, char *qwdtxtfilename2, 
       opt_t *opt)
{
  TEXT_t s1;
  TEXT_t s2;
  char ts[1000];

  TEXT_init(&s1,qwdtxtfilename1,"rb");
  TEXT_init(&s2,qwdtxtfilename2,"wb");
  node_write_text_init(&s2); /* for DM2_block_write_text */
  node_token_init(QWD_token); /* for token routines */
  glob_opt = opt; /* for QWD_block_edit */
  do_block_edit = QWD_block_edit;
  do_block_output = QWD_block_write_text;

  fprintf(stderr, "%s (QWD txt) -> %s (QWD txt)%s\n", 
                   qwdtxtfilename1, qwdtxtfilename2, opt_string(opt));

  sprintf(ts,"// source: QWD text file %s", qwdtxtfilename1); WriteLine(s2.file, ts);
  WriteLine(s2.file,"");

  yyin = s1.file;
  yyparse();

  TEXT_done(&s1);
  TEXT_done(&s2);
}


#else /* !ENABLE_QWD */


void ActionInfoQWD(char *filename, opt_t *opt)
{ syserror(NOTCOM,"QWD"); }

void ActionQWDbin2QWDbin(char *qwdbinfilename1, char *qwdbinfilename2,
       opt_t *opt)
{ syserror(NOTCOM,"QWD"); }

void ActionQWDbin2QWDtxt(char *qwdbinfilename, char *qwdtxtfilename, 
       opt_t *opt)
{ syserror(NOTCOM,"QWD"); }

void ActionQWDtxt2QWDbin(char *qwdtxtfilename, char *qwdbinfilename,
       opt_t *opt)
{ syserror(NOTCOM,"QWD"); }

void ActionQWDtxt2QWDtxt(char *qwdtxtfilename1, char *qwdtxtfilename2, 
       opt_t *opt)
{ syserror(NOTCOM,"QWD"); }


#endif /* ENABLE_QWD */



/*---------------------------------------------------------------------------*/
/* DM2 */


#ifdef ENABLE_DM2


void ActionInfoDM2(char *filename, opt_t *opt)
{
  unsigned long tics;

  udm2_init();

  /* I check here again for text or bin, text check is much faster */
  if (isDM2txt(filename)) {
    fprintf(stderr,"%s: Quake II (DM2 txt)\n", filename);
  }
  else {
    if ((tics=isDM2bin(filename))!=0) {
      fprintf(stderr,"%s: Quake II (DM2 bin), %li frames\n", filename, tics);
    }
    else {
      syswarning(WDM2,filename);
    }
  }
  udm2_done();

}

void ActionInplaceDM2(char *filename, opt_t *opt)
{
  DM2_t d;
  unsigned char buffer[DM2_MAX_MESSAGE_SIZE];
  size_t blockbase;
  size_t blocksize;
  unsigned char *bufferend; 
  int changed;
  unsigned char *bufferbase;
  unsigned char *bufferpos;
  int lastwasbetweenblock = 0;

  #define FM_reconnect		"\013reconnect\n"
  #define FMS_reconnect		(strlen(FM_reconnect)+1)
  #define FM_configstrings	"\013cmd configstrings "
  #define FMS_configstrings	(strlen(FM_configstrings))
  #define FM_baselines		"\013cmd baselines "
  #define FMS_baselines		(strlen(FM_baselines))
  #define FM_precache		"\013precache "
  #define FMS_precache		(strlen(FM_precache))
  char FM_reconnect_m[]="\012\002Server restarted\n\001";
  #define FMS_reconnect_m	sizeof(FM_reconnect_m)
  *(FM_reconnect_m + FMS_reconnect_m - 2) = 0x00;
  *(FM_reconnect_m + FMS_reconnect_m - 1) = 0x08;
  #define FM_record		"\013record "
  #define FMS_record		(strlen(FM_record))
  #define FM_download		"\020\377\377"
  #define FMS_download		(strlen(FM_download)+1)

  if (isDM2bin(filename)) {   
    DM2_init(&d, filename, "r+b");
  
    fprintf(stderr,"%s (DM2 bin) -> %s (DM2 bin)%s\n", d.filename, d.filename,
                   opt_string(opt));
    while ((blockbase=ftell(d.file)) < d.filesize) {
      /*
      fprintf(stderr,"blockbase=0x%lx, ", (unsigned long)blockbase);
      */
      /* I don't check return-values because of the successful isDM2bin */
      /* read the block size from file */
      fread(buffer+0,1,4,d.file);
      /* get the block size */
      blocksize = ((buffer[0])    ) +
                  ((buffer[1])<< 8) +
                  ((buffer[2])<<16) +
                  ((buffer[3])<<24);
      /* don't confuse with the endblock */
      if (blocksize != 0xFFFFFFFF) {
        /* read in the block */
        fread(buffer+4,1,blocksize,d.file);
        /* set the bufferend pointer at the end */
        bufferend = buffer + 4 + blocksize;
      }
      else {
        bufferend = buffer + 4;
      }
      changed = 0;

      if (opt->option&opFixMulti) {
        for (bufferbase = buffer + 4;
             bufferbase < bufferend;
             bufferbase++) {
          /* Note the parameter order!
             The Linux memmem(3) man page is wrong */
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FM_reconnect, FMS_reconnect))!=NULL) {
            memset(bufferpos,0x06,FMS_reconnect); 
            changed++;
            bufferbase+=FMS_reconnect;
          }
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FM_configstrings, FMS_configstrings))!=NULL) {
            size_t l;
            memset(bufferpos,0x06,l = strlen((char*)bufferpos) + 1);
            changed++;
            bufferbase+=l;
          }
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FM_baselines, FMS_baselines))!=NULL) {
            size_t l;
            memset(bufferpos,0x06,l = strlen((char*)bufferpos) + 1);
            changed++;
            bufferbase+=l;
          }
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FM_record, FMS_record))!=NULL) {
            size_t l;
            memset(bufferpos,0x06,l = strlen((char*)bufferpos) + 1);
            changed++;
            bufferbase+=l;
          }
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FM_download, FMS_download))!=NULL) {
            size_t l;
            memset(bufferpos,0x06,l = FMS_download);
            changed++;
            bufferbase+=l;
          }
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FM_precache, FMS_precache))!=NULL) {
            size_t l;
            memset(
                   /* after the new string end */
                   bufferpos + FMS_precache + 1,
                   /* NOP bytes */
                   0x06,
                   /* strlen + 1 times */
                   l = strlen((char*)bufferpos + FMS_precache + 1) + 1
                  );
            *(bufferpos + FMS_precache - 1) = '\n'; /* \n instead of ' ' */ 
            *(bufferpos + FMS_precache    ) = '\0'; /* \0 instead of anything */
            changed++;
            bufferbase+=l;
          }
          if ((bufferpos=(unsigned char*)memmem(bufferbase, bufferend - bufferbase,
                                                FM_reconnect_m, FMS_reconnect_m))!=NULL) {
            /* replace reconnect (0x08) with nop (0x06) */
            *(bufferpos + FMS_reconnect_m - 1) = 0x06;
            changed++;
            bufferbase+=FMS_reconnect_m;
          }
          if (!changed) break;
        } /* end for */
        /* buffer[0..3] is the length */
        /* buffer[4] is the first message id */
        /* if it is reconnect, chang it to nop */
        if ((blocksize > 4) && (buffer[4] == 0x08)) {
          buffer[4] = 0x06;
          changed++;
        }
        /* buffer[5..8] is serverversion */
        /* buffer[9..12] is key */
        /* buffer[13] is isdemo */
        /* if it is serverdata, set isdemo to, if it was 0 before */
        if ((blocksize > 13) && (buffer[4] == 0x0C) && (buffer[13] == 0x00)) {
          buffer[13] = 0x01;
          changed++;
        }
        if (lastwasbetweenblock) {
          /* move the whole block (size plus data) further by 4 */
          memmove(buffer + 4, buffer, blocksize + 4);
          /* fill the old size bytes with NOPs */
          memset(buffer + 4, 0x06, 4);
          /* increase the current blocksize by 4 to include the betweenblock */
          blocksize += 4;
          /* put the new increased blocksize at the start: lowest byte first */
          buffer[0] = ( blocksize      ) & 0xFF;
          buffer[1] = ( blocksize >>  8) & 0xFF;
          buffer[2] = ( blocksize >> 16) & 0xFF;
          buffer[3] = ( blocksize >> 24) & 0xFF;
          /* move the position in the file down by 4 */
          blockbase -= 4;
          /* switch this state off */
          lastwasbetweenblock = 0;
          /* write this block out */
          changed++; 
        }
        if (blocksize == 0) {
          lastwasbetweenblock = 1;
        }
      } /* end opFixMulti */
      
      /* here may come some other change or repair code */
      
      if (changed) {
        /* something has changed, let's write it back in the file */
        /* move back to the blockbase */
        fseek(d.file, blockbase, SEEK_SET);
        if (blocksize == 0xFFFFFFFF) {
        /* endblock: 4 bytes */
          fwrite(buffer, 1, 4, d.file);
        }
        else {
          /* normal block: 4 + blocksize bytes */
          fwrite(buffer, 1, 4 + blocksize, d.file);
        }
      }
      /* fprintf(stdout,"\n"); fflush(stdout); */
    } 
    DM2_done(&d);
  }
  else {
    syswarning(WDM2,filename);
  }
}

void ActionDM2bin2DM2bin(char *dm2binfilename1, char *dm2binfilename2,
       opt_t *opt)
{
  DM2_t d1, d2;
  BB_t m;
  node *n;
  int o;

  udm2_init();
  
  DM2_init(&d1,dm2binfilename1,"rb");
  DM2_init(&d2,dm2binfilename2,"wb");
  DM2_prepare_write_bin(&d2);
  node_token_init(DM2_token); /* for token routines */
  glob_opt = opt; /* for DM2_block_edit */
  do_block_edit = DM2_block_edit;
  do_block_output = DM2_block_write_bin;
  
  fprintf(stderr, "%s (DM2 bin) -> %s (DM2 bin)%s\n",
                  dm2binfilename1, dm2binfilename2, opt_string(opt));

  if (opt->option & opMarkStep) o=0x01; else o=0x00;

  d1.frame = 0;
  while ((unsigned long)ftell(d1.file)<d1.filesize) {
    DM2_block_read_bin(&d1, &m);
    if (m.size > DM2_MAX_MESSAGE_SIZE) {
      /* clean the record type hint */
      m.hint &= ~(DM2_RECORD_CLIENT|DM2_RECORD_SERVER|DM2_RECORD_RELAY);
      /* set serverrecord variant */
      m.hint |= DM2_RECORD_SERVER;
    }
    n=DM2_bin_to_node(&m,o);
    do_block(n);
    node_delete(n);
    d1.frame++;
  }
  
  DM2_done(&d1);
  DM2_done(&d2);
  
  udm2_done();
}

void ActionDM2bin2DM2txt(char *dm2binfilename, char *dm2txtfilename, 
       opt_t *opt)
{
  DM2_t d;
  TEXT_t s;
  char ts[1000];
  BB_t m;
  node *n;
  int o;

  udm2_init();

  DM2_init(&d,dm2binfilename,"rb");
  TEXT_init(&s,dm2txtfilename,"wb");
  node_write_text_init(&s); /* for node_write_text */
  node_token_init(DM2_token); /* for token routines */
  glob_opt = opt; /* for DM2_block_edit */
  do_block_edit = DM2_block_edit;
  do_block_output = DM2_block_write_text;

  fprintf(stderr,"%s (DM2 bin) -> %s (DM2 txt)%s\n", 
                 d.filename, s.filename, opt_string(opt));

  sprintf(ts,"// source: DM2 binary file %s", d.filename); WriteLine(s.file, ts);
  WriteLine(s.file,"");

  if (opt->option & opMarkStep) o=0x01; else o=0x00;
  
  d.frame=0;
  while ((unsigned long)ftell(d.file)<d.filesize) {
    DM2_block_read_bin(&d, &m);
    if (m.size > DM2_MAX_MESSAGE_SIZE) {
      /* clean the record type hint */
      m.hint &= ~(DM2_RECORD_CLIENT|DM2_RECORD_SERVER|DM2_RECORD_RELAY);
      /* set serverrecord variant */
      m.hint |= DM2_RECORD_SERVER;
    }
    n=DM2_bin_to_node(&m,o);
    do_block(n);
    node_delete(n);
    d.frame++;
  }
  DM2_done(&d);
  TEXT_done(&s);

  udm2_done(); 
}

void ActionDM2txt2DM2bin(char *dm2txtfilename, char *dm2binfilename,
       opt_t *opt)
{
  TEXT_t s;
  DM2_t d;

  TEXT_init(&s,dm2txtfilename,"rb");
  DM2_init(&d,dm2binfilename,"wb");
  DM2_prepare_write_bin(&d);
  node_token_init(DM2_token); /* for token routines */
  glob_opt = opt; /* for DM2_block_edit */
  do_block_edit = DM2_block_edit;
  do_block_output = DM2_block_write_bin;

  fprintf(stderr, "%s (DM2 txt) -> %s (DM2 bin)%s\n", 
                   dm2txtfilename, dm2binfilename, opt_string(opt));

  yyin = s.file;
  yyparse();

  TEXT_done(&s);
  DM2_done(&d);
}

void ActionDM2txt2DM2txt(char *dm2txtfilename1, char *dm2txtfilename2, 
       opt_t *opt)
{
  TEXT_t s1;
  TEXT_t s2;
  char ts[1000];

  TEXT_init(&s1,dm2txtfilename1,"rb");
  TEXT_init(&s2,dm2txtfilename2,"wb");
  node_write_text_init(&s2); /* for DM2_block_write_text */
  node_token_init(DM2_token); /* for token routines */
  glob_opt = opt; /* for DM2_block_edit */
  do_block_edit = DM2_block_edit;
  do_block_output = DM2_block_write_text;

  fprintf(stderr, "%s (DM2 txt) -> %s (DM2 txt)%s\n", 
                   dm2txtfilename1, dm2txtfilename2, opt_string(opt));

  sprintf(ts,"// source: DM2 text file %s", dm2txtfilename1); WriteLine(s2.file, ts);
  WriteLine(s2.file,"");

  yyin = s1.file;
  yyparse();

  TEXT_done(&s1);
  TEXT_done(&s2);
}


#else /* !ENABLE_DM2" */


void ActionInfoDM2(char *filename, opt_t *opt)
{ syserror(NOTCOM,"DM2"); }

void ActionDM2bin2DM2bin(char *dm2binfilename1, char *dm2binfilename2,
       opt_t *opt)
{ syserror(NOTCOM,"DM2"); }

void ActionDM2bin2DM2txt(char *dm2binfilename, char *dm2txtfilename, 
       opt_t *opt)
{ syserror(NOTCOM,"DM2"); }

void ActionDM2txt2DM2bin(char *dm2txtfilename, char *dm2binfilename,
       opt_t *opt)
{ syserror(NOTCOM,"DM2"); }

void ActionDM2txt2DM2txt(char *dm2txtfilename1, char *dm2txtfilename2, 
       opt_t *opt)
{ syserror(NOTCOM,"DM2"); }


#endif /* ENABLE_DM2 */



/*---------------------------------------------------------------------------*/
/* DM3 */


#ifdef ENABLE_DM3


void ActionInfoDM3(char *filename, opt_t *opt)
{
  unsigned long tics;

  udm3_init();

  /* I check here again for text or bin, text check is much faster */
  if (isDM3txt(filename)) {
    fprintf(stderr,"%s: Quake III Arena (DM3 txt)\n", filename);
  }
  else {
    if ((tics=isDM3bin(filename))!=0) {
      fprintf(stderr,"%s: Quake III Arena (DM3 bin), %li frames\n", filename, tics);
    }
    else {
      syswarning(WDM3,filename);
    }
  }
  udm3_done();
}

void ActionDM3bin2DM3bin(char *dm3binfilename1, char *dm3binfilename2,
       opt_t *opt)
{ syserror(NOTCOM,"DM3"); }

void ActionDM3bin2DM3txt(char *dm3binfilename, char *dm3txtfilename, 
       opt_t *opt)
{
  DM3_t d;
  TEXT_t s;
  char ts[1000];
  DM3_binblock m;
  node *n;
  int o;

  udm3_init();

  DM3_init(&d,dm3binfilename,"rb");
  TEXT_init(&s,dm3txtfilename,"wb");
  node_write_text_init(&s); /* for node_write_text */
  node_token_init(DM3_token); /* for token routines */
  glob_opt = opt; /* for DM3_block_edit */
  do_block_edit = DM3_block_edit;
  do_block_output = DM3_block_write_text;

  fprintf(stderr,"%s (DM3 bin) -> %s (DM3 txt)%s\n", 
                 d.filename, s.filename, opt_string(opt));

  sprintf(ts,"// source: DM3 binary file %s", d.filename); WriteLine(s.file, ts);
  WriteLine(s.file,"");

  if (opt->option & opMarkStep) o=0x01; else o=0x00;
  
  d.frame=0;
  while ((unsigned long)ftell(d.file)<d.filesize) {
    DM3_block_read_bin(&d, &m);
    n=DM3_bin_to_node(&m,o);
    do_block(n);
    node_delete(n);
    d.frame++;
  }
  DM3_done(&d);
  TEXT_done(&s);

  udm3_done(); 
}

void ActionDM3txt2DM3bin(char *dm3txtfilename, char *dm3binfilename,
       opt_t *opt)
{ syserror(NOTCOM,"DM3"); }

void ActionDM3txt2DM3txt(char *dm3txtfilename1, char *dm3txtfilename2, 
       opt_t *opt)
{ syserror(NOTCOM,"DM3"); }


#else /* !ENABLE_DM3" */


void ActionInfoDM3(char *filename, opt_t *opt)
{ syserror(NOTCOM,"DM3"); }

void ActionDM3bin2DM3bin(char *dm3binfilename1, char *dm2binfilename2,
       opt_t *opt)
{ syserror(NOTCOM,"DM3"); }

void ActionDM3bin2DM3txt(char *dm3binfilename, char *dm3txtfilename, 
       opt_t *opt)
{ syserror(NOTCOM,"DM3"); }

void ActionDM3txt2DM3bin(char *dm3txtfilename, char *dm3binfilename,
       opt_t *opt)
{ syserror(NOTCOM,"DM3"); }

void ActionDM3txt2DM3txt(char *dm3txtfilename1, char *dm3txtfilename2, 
       opt_t *opt)
{ syserror(NOTCOM,"DM3"); }


#endif /* ENABLE_DM3 */



/*****************************************************************************/
/* special general routines **************************************************/
/*****************************************************************************/


char *opt_string(opt_t *opt)
{
  char os[1000];

  *os='\0';

  if (opt->option&opFixMulti) {
    sprintf(os+strlen(os)," multi-level fixed");
  }

  return strdup(os);
}


/*- file end lmpc.c ----------------------------------------------------------*/
