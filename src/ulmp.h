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
|  ulmp.h - header file of ulmp.c, general LMP routines                      |
\****************************************************************************/


#ifndef __ULMP_H
#define __ULMP_H


#include <stdio.h>

#define C_MOUSE         1
#define C_KEY_OLD       2
#define C_KEY_NEW       4
#define C_KEY           (C_KEY_OLD | C_KEY_NEW)
#define ruleCOOP       0
#define ruleDEATH      1
#define ruleALT        2
#define MAXPLAYER_GLOBAL    8
#define MAXPLAYER_DOOM_old  4
#define MAXPLAYER_DOOM_new  4
#define MAXPLAYER_HERETIC   4
#define MAXPLAYER_HEXEN_old 4
#define MAXPLAYER_HEXEN_new 8
#define MAXPLAYER_STRIFE    8
#define HEADER_DOOM_old    7
#define HEADER_DOOM_new   13
#define HEADER_HERETIC     7
#define HEADER_HEXEN_old  11
#define HEADER_HEXEN_new  19
#define HEADER_STRIFE     16
#define SHORT_TIC      4
#define LONG_TIC       6

#define UNKNOWN     0
#define DOOM_old    1
#define DOOM_new    2
#define DOOM2       4
#define HERETIC     8
#define HEXEN_old  16
#define HEXEN_new  32
#define STRIFE     64
#define TICTIME  1.0/35.0


extern char *ColorName[];
extern char *WeaponName[];
extern char *ArtifactName[];
extern char *ClassName[];
extern char *STRIFEActionName[];

typedef struct {
                 unsigned char go;
                 unsigned char strafe;
                 unsigned char turn;
                 unsigned char use;
                 unsigned char fl;
                 unsigned char art;
               } TIC_t;


typedef struct {
                 unsigned char header[19];
                 int           headersize;
                 int           game;
                 unsigned char versionbyte;
                 unsigned char skill;
                 unsigned char episode;
                 unsigned char map;
                 unsigned char multirule;
                 unsigned char respawn;
                 unsigned char fast;
                 unsigned char nomonsters;
                 unsigned char mainplayer;
                 int           playernum;
                 unsigned char maxplayer;
                 int           num[MAXPLAYER_GLOBAL];
                 int           playerclass[MAXPLAYER_GLOBAL];
                 char          *filename;
                 FILE          *file;
                 long          filesize;
                 long          datasize;
                 int           year;
                 long          tics;
                 long          stics;
                 long          incomplete;
                 int           ticsize;
                 double        time;
		 char          *gs;
		 char          *ns;
               } LMP_t;


typedef struct {
                 char          *filename;
                 FILE          *file;
                 long          filesize;
               } LS_t;


typedef struct {
                 unsigned long hist[128];
                 unsigned long sum;
                 char *command;
                 int maxm;
                 unsigned char m[3];
                 unsigned char control;
                 unsigned char last;
               } STAT_t; 


typedef struct {
                 char*         versionstring;
		 int           game;
                 unsigned char versionbyte;
		 unsigned int  string_no;
               } VK;
extern VK VersionKey[];
#define VersionKeys 23

typedef struct {
                 unsigned char value;
                 char*         name;
               } SA;
extern SA STRIFEArtifactName[];
#define STRIFEArtifacts 12
		
/* LMP */
void LMP_init(LMP_t *l, char *filename, char *mode);
void LMP_self_determine_type(LMP_t *l);
void LMP_readheader(LMP_t *l);
void LMP_writeheader(LMP_t *l);
void LMP_init(LMP_t *l, char *filename, char *mode);
void LMP_readheader(LMP_t *l);
void LMP_writeheader(LMP_t *l);
void LMP_writequitbyte(LMP_t *l);
void LMP_putgametic(LMP_t *l, TIC_t *t);
void LMP_getgametic(LMP_t *l, TIC_t *t);
void LMP_done(LMP_t *l);

/* LS */
void LS_init(LS_t *l, char *filename, char *mode);
void LS_done(LS_t *l);

/* STAT */
void STAT_init(STAT_t *s, char *command, int maxm);
void STAT_calc(STAT_t *s);
int STAT_turncheck(STAT_t *s, unsigned char t1, unsigned char t2, unsigned char t3);
char *STAT_writehist(STAT_t *s, char *buf);
void STAT_done(STAT_t *s);

void getversion(unsigned char versionbyte, int *game, char **gs, char **ns);
unsigned char strtoversionbyte(char *s, int game);


#endif /* __ULMP_H */


/*-- file end ulmp.h ---------------------------------------------------------*/

