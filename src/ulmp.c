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
|  ulmp.c - implementation, general LMP routines                             |
\****************************************************************************/


#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#if defined(HAVE_SYS_TIME_H) && defined(TM_IN_SYS_TIME)
#include <sys/time.h>
#if defined(TIME_WITH_SYS_TIME)
#include <time.h>
#endif /* defined(TIME_WITH_SYS_TIME) */
#else /* !defined(TM_IN_SYS_TIME) || !defined(TM_IN_SYS_TIME) */
#include <time.h>
#endif /* !defined(TM_IN_SYS_TIME) */
#include <unistd.h>
#include "tools.h"
#include "ulmp.h"


char *ColorName[] = {
/*  0: DOOM, DOOM ][ */  "green", "indigo", "brown", "red",
/*  4: HERETIC       */  "green", "yellow", "red", "blue",
/*  8: HEXEN         */  "blue", "red", "yellow", "green", "jade", "white", "hazel", "purple",
/* 12: STRIFE        */  "brown", "red", "redbrown", "grey", "dark green", "gold", "bright green", "super-hero blue"
                      };

char *WeaponName[] = {
/*  0: DOOM           */ "Fist/Chainsaw", "Pistol", "Shotgun", "Chaingun", "Rocket Launcher", "Plasma Rifle", "BFG 9000", "Chainsaw",
/*  8: DOOM ][        */ "Fist/Chainsaw", "Pistol", "Shotgun/Super Shotgun", "Chaingun", "Rocket Launcher", "Plasma Rifle", "BFG 9000", "Chainsaw",
/* 16: HERETIC        */ "Staff/Gauntlets of the Necromancer", "Crystal", "Ethereal Crossbow", "Dragon Claw", "Hellstaff", "Phoenix Rod", "Mace", "Gauntlets of the Necromancer",
/* 24: HEXEN: fighter */ "Spiked Gauntlets", "Timon's Axe", "Hammer of Retribution", "Quietus",
/* 28: HEXEN: cleric  */ "Mace of Contrition", "Serpent Staff", "Firestorm", "Justifieri",
/* 32: HEXEN: mage    */ "Sapphire Wand", "Frost Shards", "Arc of Death", "Bloodscourge",
/* 36: STRIFE         */ "Punch Dagger", "Crossbow", "Pulse Rifle", "Missle Launcher", "Grenade Launcher", "Flame Thrower", "Blaster", "SIGIL", "Punch Dagger"
                     };

char *ArtifactName[] = {
/*  0: HERETIC        */ "Ring of Invincibility",
                         "Shadowsphere",
                         "Quartz Flask",
                         "Chaos Device",
                         "Tome of Power",
                         "Torch",
                         "Time Bomb of the Ancients",
                         "Morph Ovum",
                         "Wings of Wrath",
                         "Mystic Urn",
/* 10: HEXEN          */ "icon of the defender",
                         "quartz flask",
                         "mystic urn",
                         "clerical healing key",
                         "dark servant",
                         "torch",
                         "porkalator",
                         "wings of wrath",
                         "chaos device",
                         "flechette",
                         "bunishment device",
                         "boots of speed",
                         "krater of might",
                         "dragonskin bracers",
                         "disc of repulsion"
                       };

char *ClassName[] = { "fighter", "cleric", "mage" };

VK VersionKey[] = {
                  { "1.0"     , HERETIC,                     0, 1},
		  { "1.1"     , DOOM_old,                    0, 1},
                  { "1.2"     , DOOM_old,                    0, 2},
		  { "<1.4"    , DOOM_old,                    0, 4},
                  { "1.0"     , STRIFE,	                   101, 1},
		  { "1.4beta" , DOOM_new,                  104, 1},
                  { "1.5beta" , DOOM_new,                  105, 1},
		  { "1.6beta" , DOOM_new,                  106, 1},
		  { "1.666"   , DOOM_new,                  106, 2},
		  { "1.666"   , DOOM2,                     106, 2},
		  { "1.7"     , DOOM2,                     107, 1},
		  { "1.7a"    , DOOM_new,                  107, 2},
		  { "1.7a"    , DOOM2,                     107, 2},
		  { "1.8"     , DOOM_new,                  108, 1},
		  { "1.8"     , DOOM2,                     108, 1},
		  { "1.9"     , DOOM_new,                  109, 1},
		  { "1.9"     , DOOM2,                     109, 1},
		  { "1.10"    , DOOM_new,                  110, 1},
		  { "1.10"    , DOOM2,                     110, 1},
		  { "1.0"     , HEXEN_old,           HEXEN_old, 1},
		  { "1.0"     , HEXEN_old|HEXEN_new, HEXEN_old, 1},
		  { "1.1"     , HEXEN_new,           HEXEN_new, 1},
		  { "1.1"     , HEXEN_old|HEXEN_new, HEXEN_new, 1}
                };

char *STRIFEActionName[] = {
/* Look Up */       "LU",
/* Look Down */     "LD",
/* Run */           "RU",
/* Inventory Use */ "IN",
/* Drop */          "DR",
/* Jump */          "JU",
/* Unknown */       "AC6",
/* Health */        "HE"
};

SA STRIFEArtifactName[] = {
  { 0x74, "toughness"          },
  { 0x75, "accuracy"           },
  { 0x76, "full health"        },
  { 0x7B, "teleportor beacon"  },
  { 0x7C, "metal armor"        },
  { 0x7D, "leather armor"      },
  { 0xA1, "med patch"          },
  { 0xA2, "medical kit"        },
  { 0xA3, "coin"               },
  { 0xA7, "shadow armor"       },
  { 0xA8, "environmental suit" },
  { 0xB7, "offering chalice"   }
};


#define MYBUFSIZE 65536


/******************************************************************************/
/** LMP ***********************************************************************/
/******************************************************************************/

void LMP_init(LMP_t *l, char *filename, char *mode)
{
  struct stat buf;
  struct tm   *ft;

  if ((l->filename = strdup(filename))==NULL) syserror(errno,"strdup");
  if ((l->file=fopen(l->filename, mode))==NULL) syserror(errno,l->filename);
  if (SETVBUF(l->file,NULL,_IOFBF,BUFSIZ>MYBUFSIZE?BUFSIZ:MYBUFSIZE)!=0) syserror(errno,"setvbuf");
  if (stat(l->filename,&buf)==-1) syserror(errno,l->filename);
  l->filesize=buf.st_size;
  ft=localtime(&buf.st_mtime);
  l->year=ft->tm_year;
}

void LMP_self_determine_type(LMP_t *l)
{
  int game, i;
  signed char *buffer;
  #define MINBUF 2000
  #define MAXBUF 60000
  int filled;
  double av, av_d, av_h, av_o, av_n;
  int n;

  game=UNKNOWN;
  /* file size test */
  if (l->filesize<HEADER_DOOM_old+1) syserror(WLMP,l->filename);
  if (((l->filesize-1-HEADER_DOOM_old  )%SHORT_TIC)==0) game|=DOOM_old;
  if (((l->filesize-1-HEADER_DOOM_new  )%SHORT_TIC)==0) game|=(DOOM_new|DOOM2);
  if (((l->filesize-1-HEADER_HERETIC   )%LONG_TIC )==0) game|=HERETIC;
  if (((l->filesize-1-HEADER_HEXEN_old )%LONG_TIC )==0) game|=HEXEN_old;
  if (((l->filesize-1-HEADER_HEXEN_new )%LONG_TIC )==0) game|=HEXEN_new;
  if (((l->filesize-1-HEADER_STRIFE    )%LONG_TIC )==0) game|=STRIFE;
  rewind(l->file);
  if (fread(l->header,1,HEADER_DOOM_old,l->file)==0) syserror(FIREAD,l->filename);
  if (l->header[0]>=104) {
    /* DOOM_new or DOOM2 */
    if (!(game&(DOOM_new|DOOM2))) syserror(WLMP,l->filename);
    if (l->filesize<HEADER_DOOM_new+1) syserror(WLMP,l->filename);
    rewind(l->file);
    if (fread(l->header,1,HEADER_DOOM_new,l->file)==0) syserror(FIREAD,l->filename);
    if (l->header[2]>1) 
      game=DOOM_new;
    else 
      if (l->header[3]>9)
        game=DOOM2;
      else
        game=DOOM_new|DOOM2;
  }
  else {
    /* STRIFE, DOOM_old, HERETIC, HEXEN_old or HEXEN_new */
    if (l->header[0]==101) {
      /* STRIFE */
      if (!(game&STRIFE)) syserror(WLMP,l->filename);
      if (l->filesize<HEADER_STRIFE-1) syserror(WLMP,l->filename);
      rewind(l->file);
      if (fread(l->header,1,HEADER_STRIFE,l->file)==0) syserror(FIREAD,l->filename);
      game=STRIFE;
    }
    else { 
      /* DOOM_old, HERETIC, HEXEN_old or HEXEN_new */
      game &= ((~DOOM_new)&(~DOOM2)&(~STRIFE));
      if (game==UNKNOWN) syserror(WLMP,l->filename);
      l->episode     = l->header[1];
      l->map         = l->header[2];
      /* HEXEN uses episode 1 only */
      if (l->episode>1) game&=((~HEXEN_old)&(~HEXEN_new));
      /* DOOM_old and HERETIC don't use maps>9 */
      if (l->map>9) game&=((~DOOM_old)&(~HERETIC));
      /* 2 at 4,6 means playerclass=mage -> not DOOM_old or HERETIC */
      if (l->header[4]==2 || l->header[6]==2) game&=((~DOOM_old)&(~HERETIC));
      if (game==UNKNOWN) syserror(WLMP,l->filename);
      /* there are now only 9 possibilities for the bits: 
         D, H, O, N, 
         DH, DO, DN, HN,
         DHN */
      if ((game!=DOOM_old ) && (game!=HERETIC  ) && 
          (game!=HEXEN_old) && (game!=HEXEN_new)) {
        /* 5 variants deserve further research: 
           DH, DO, DN, HN, DHN */
        buffer=(signed char *) malloc(sizeof(signed char)*MAXBUF); 
        if (buffer==NULL) syserror(ENOMEM, "buffer");
        if (l->filesize<MINBUF) syserror(UKTYPE, l->filename);
        filled=(l->filesize>MAXBUF?MAXBUF:l->filesize);
        rewind(l->file);
        if (fread(buffer,1,filled,l->file)==0) syserror(FIREAD,l->filename);
        av_d=av_h=av_o=av_n=0;
        if (game&DOOM_old ) { 
          for (i=0x09,n=0,av=0;i<filled;i+=4,n++) av+=buffer[i]; av_d=fabs(av/n); }
        if (game&HERETIC  ) {
          for (i=0x09,n=0,av=0;i<filled;i+=6,n++) av+=buffer[i]; av_h=fabs(av/n); }
        if (game&HEXEN_old) {
          for (i=0x0D,n=0,av=0;i<filled;i+=6,n++) av+=buffer[i]; av_o=fabs(av/n); }
        if (game&HEXEN_new) {
          for (i=0x15,n=0,av=0;i<filled;i+=6,n++) av+=buffer[i]; av_n=fabs(av/n); }
        switch (game) {
          case DOOM_old|HERETIC:
            game=(av_d<av_h?DOOM_old:HERETIC);
          break;
          case DOOM_old|HEXEN_old:
            game=(av_d<av_o?DOOM_old:HEXEN_old);
          break;
          case DOOM_old|HEXEN_new:
            game=(av_d<av_n?DOOM_old:HEXEN_new);
          break;
          case HERETIC|HEXEN_new:
            game=(l->year>=96?HEXEN_new:HERETIC);
          break;
          case DOOM_old|HERETIC|HEXEN_new:
            if (av_d<av_n)
              game=DOOM_old;
            else
              game=(l->year>=96?HEXEN_new:HERETIC);
          break;
          default:
              syserror(WLMP,l->filename);
          break; 
        } /* end switch */
        free(buffer);
      }
    }
  }
  l->game = game;
}

void LMP_readheader(LMP_t *l)
{
  int i;
  
  /* totally rubbish code but I had no better idea */
  if (l->game==(DOOM_old|DOOM_new)) {
    rewind(l->file);
    if (fread(l->header,1,1,l->file)==0) syserror(FIREAD,l->filename);
    l->game=(l->header[0]>=104?DOOM_new:DOOM_old);
  }
  if (l->game==(HEXEN_old|HEXEN_new)) {
    l->game=((((l->filesize-1-HEADER_HEXEN_old)%LONG_TIC)==0)?HEXEN_old:HEXEN_new);
  }
  /* end of rubbish */
  switch (l->game) {
    case DOOM_old:       
           l->headersize = HEADER_DOOM_old;
           l->ticsize    = SHORT_TIC;
           l->maxplayer  = MAXPLAYER_DOOM_old;
    break;
    case HERETIC: 
           l->headersize = HEADER_HERETIC;
           l->ticsize    = LONG_TIC;
           l->maxplayer  = MAXPLAYER_HERETIC;
    break;
    case HEXEN_old:     
           l->headersize = HEADER_HEXEN_old;
           l->ticsize    = LONG_TIC;
           l->maxplayer  = MAXPLAYER_HEXEN_old;
    break;
    case HEXEN_new:  
           l->headersize = HEADER_HEXEN_new;
           l->ticsize    = LONG_TIC;
           l->maxplayer  = MAXPLAYER_HEXEN_new;
    break;
    case DOOM_new: 
           l->headersize = HEADER_DOOM_new;
           l->ticsize    = SHORT_TIC;
           l->maxplayer  = MAXPLAYER_DOOM_new;
    break;
    case DOOM2:     
           l->headersize = HEADER_DOOM_new;
           l->ticsize    = SHORT_TIC;
           l->maxplayer  = MAXPLAYER_DOOM_new;
    break;
    case DOOM_new|DOOM2: 
           l->headersize = HEADER_DOOM_new;
           l->ticsize    = SHORT_TIC;
           l->maxplayer  = MAXPLAYER_DOOM_new;
    break;
    case STRIFE:	 
           l->headersize = HEADER_STRIFE;
           l->ticsize    = LONG_TIC;
           l->maxplayer  = MAXPLAYER_STRIFE; 
    break;
  }
  rewind(l->file);
  if (fread(l->header,1,l->headersize,l->file)==0) syserror(FIREAD,l->filename);
  fseek(l->file,l->headersize,SEEK_SET);
  switch (l->headersize) {
    case HEADER_DOOM_old:
    case HEADER_HEXEN_old:
    case HEADER_HEXEN_new:
      l->versionbyte = ((l->game==DOOM_old || l->game==HERETIC)?0:l->game);
      l->skill       = l->header[0];
      l->episode     = l->header[1];
      l->map         = l->header[2]; 
      l->multirule   = 0;
      l->respawn     = 0;
      l->fast        = 0;
      l->nomonsters  = 0;
      l->mainplayer  = 0;
    break;
    case HEADER_DOOM_new:
      l->versionbyte = l->header[0];
      l->skill       = l->header[1];
      l->episode     = l->header[2];
      l->map         = l->header[3];
      l->multirule   = l->header[4];
      l->respawn     = l->header[5];
      l->fast        = l->header[6];
      l->nomonsters  = l->header[7];
      l->mainplayer  = l->header[8];
    break;
    case HEADER_STRIFE:
      l->versionbyte = l->header[0];
      l->skill       = l->header[1];
      l->episode     = 1;
      l->map         = l->header[2];
      l->multirule   = l->header[3];
      l->respawn     = l->header[4];
      l->fast        = l->header[5];
      l->nomonsters  = l->header[6];
      l->mainplayer  = l->header[7];
    break;
  }
  if (l->game&(HEXEN_old|HEXEN_new)) {
    for (i=0, l->playernum=0;i<l->maxplayer;i++) {
      if (l->header[l->headersize-l->maxplayer*2+i*2]) {
        l->num[l->playernum] = i;
        l->playerclass[l->playernum++] = l->header[l->headersize-l->maxplayer*2+i*2+1];
      }
    }
  }
  else {
    for (i=0, l->playernum=0;i<l->maxplayer;i++) {
      if (l->header[l->headersize-l->maxplayer+i]) {
        l->num[l->playernum++] = i;
      }
    }
  }
  if (!l->playernum) syserror(WLMP,l->filename);
  getversion(l->versionbyte,&(l->game),&l->gs,&l->ns);  
  l->datasize=l->filesize-l->headersize-1;
  if (l->datasize % l->ticsize) syserror(WLMP,l->filename);
  l->stics=l->datasize / l->ticsize;            /* number of single tics */
  l->tics=l->stics / l->playernum;              /* complete multi tics */
  l->incomplete=l->stics % l->playernum;        /* incomplete single tics */
  l->time=l->tics*TICTIME;
}

void LMP_writeheader(LMP_t *l)
{
  int i;

  switch (l->game) {
    case DOOM_old:       
      l->headersize=HEADER_DOOM_old; 
      l->maxplayer =MAXPLAYER_DOOM_old;
    break;
    case HERETIC:
      l->headersize=HEADER_HERETIC;
      l->maxplayer =MAXPLAYER_HERETIC;
    break;
    case HEXEN_old:
      l->headersize=HEADER_HEXEN_old;
      l->maxplayer =MAXPLAYER_HEXEN_old;
    break;
    case HEXEN_new:
      l->headersize=HEADER_HEXEN_new;
      l->maxplayer =MAXPLAYER_HEXEN_new;
    break;
    case DOOM_new:
      l->headersize=HEADER_DOOM_new;
      l->maxplayer =MAXPLAYER_DOOM_new;
    break;
    case DOOM2:
      l->headersize=HEADER_DOOM_new;
      l->maxplayer =MAXPLAYER_DOOM_new;
    break;
    case DOOM_new|DOOM2:
      l->headersize=HEADER_DOOM_new;
      l->maxplayer =MAXPLAYER_DOOM_new;
    break;
    case STRIFE:
      l->headersize=HEADER_STRIFE;
      l->maxplayer =MAXPLAYER_STRIFE;
    break;
  }
  for (i=0;i<l->headersize;i++) l->header[i]=0; 
  switch (l->headersize) {
    case HEADER_DOOM_old:
    case HEADER_HEXEN_old:
    case HEADER_HEXEN_new:
      l->header[0] = l->skill;
      l->header[1] = l->episode;
      l->header[2] = l->map;
    break;
    case HEADER_DOOM_new:
      l->header[0] = l->versionbyte;
      l->header[1] = l->skill;
      l->header[2] = l->episode;
      l->header[3] = l->map;
      l->header[4] = l->multirule;
      l->header[5] = l->respawn;
      l->header[6] = l->fast;
      l->header[7] = l->nomonsters;
      l->header[8] = l->mainplayer;
    break;
    case HEADER_STRIFE:
      l->header[0] = l->versionbyte;
      l->header[1] = l->skill;
      l->header[2] = l->map;
      l->header[3] = l->multirule;
      l->header[4] = l->respawn;
      l->header[5] = l->fast;
      l->header[6] = l->nomonsters;
      l->header[7] = l->mainplayer;
    break;
  }
  if (l->game&(HEXEN_old|HEXEN_new)) {
    for (i=0;i<l->maxplayer;i++) {
      l->header[l->headersize-l->maxplayer*2+i*2  ] = 0;
      l->header[l->headersize-l->maxplayer*2+i*2+1] = 0;
    }
    for (i=0;i<l->playernum;i++) {
      l->header[l->headersize-l->maxplayer*2+l->num[i]*2]   = 1;
      l->header[l->headersize-l->maxplayer*2+l->num[i]*2+1] = l->playerclass[i];
    }
  }
  else {
    for (i=0;i<l->maxplayer;i++) 
      l->header[l->headersize-l->maxplayer+i] = 0;
    for (i=0;i<l->playernum;i++) 
      l->header[l->headersize-l->maxplayer+l->num[i]] = 1;
  }
  if (fwrite(l->header,1,l->headersize,l->file)==0) syserror(FIWRITE,l->filename);
}  

void LMP_writequitbyte(LMP_t *l)
{
  unsigned char a=0x80;

  if (fwrite(&a,1,1,l->file)==0) syserror(FIWRITE,l->filename);
}

void LMP_putgametic(LMP_t *l, TIC_t *t)
{
  if (fwrite(t,1,l->ticsize,l->file)==0) syserror(FIWRITE,l->filename);
}

void LMP_getgametic(LMP_t *l, TIC_t *t)
{
  if (fread(t,1,l->ticsize,l->file)==0) syserror(FIREAD,l->filename);
}

void LMP_done(LMP_t *l)
{
  if (fclose(l->file)!=0) syserror(errno,l->filename);
  free(l->filename);
}


/******************************************************************************/
/** LS ************************************************************************/
/******************************************************************************/

void LS_init(LS_t *l, char *filename, char *mode)
{
  struct stat buf;

  if ((l->filename = strdup(filename))==NULL) syserror(errno,"strdup");
  if ((l->file=fopen(l->filename, mode))==NULL) syserror(errno,l->filename);
  if (SETVBUF(l->file,NULL,_IOFBF,BUFSIZ>MYBUFSIZE?BUFSIZ:MYBUFSIZE)!=0) syserror(errno,"setvbuf");
  if (stat(l->filename,&buf)==-1) syserror(errno,l->filename);
  l->filesize=buf.st_size;
}


void LS_done(LS_t *l)
{
  if (fclose(l->file)!=0) syserror(errno,l->filename);
  free(l->filename);
}


/******************************************************************************/
/** STAT **********************************************************************/
/******************************************************************************/

void STAT_init(STAT_t *s, char *command, int maxm)
{
  unsigned char i;

  for (i=0;i<128;i++) s->hist[i]=0;
  s->sum=0;
  if ((s->command=strdup(command))==NULL) syserror(errno,"strdup");
  s->maxm=maxm;
}


void STAT_calc(STAT_t *s)
{
  unsigned char i;

  s->m[0]=s->m[1]=s->m[2]=0;
  s->sum = 0;
  for (i=0;i<128;i++) {
    s->sum+=s->hist[i];
    if (s->hist[i]>s->hist[s->m[0]]) {
      s->m[2]=s->m[1]; s->m[1]=s->m[0]; s->m[0]=i;
    }
    else if (s->hist[i]>s->hist[s->m[1]]) {
      s->m[2]=s->m[1]; s->m[1]=i;
    }
    else if (s->hist[i]>s->hist[s->m[2]]) {
      s->m[2]=i;
    }  
    if (s->hist[i]>0) s->last=i;
  }
  if (s->hist[s->m[0]]+s->hist[s->m[1]]+s->hist[s->m[2]]==s->sum) 
    s->control = C_KEY;
  else
    s->control = C_MOUSE;
}


int STAT_turncheck(STAT_t *s, unsigned char t0, unsigned char t1, unsigned char t2)
{
  return    (s->m[0]==t0 || s->m[0]==t1 || s->m[0]==t2 || s->m[0]==0)
         && (s->m[1]==t0 || s->m[1]==t1 || s->m[1]==t2 || s->m[1]==0)
         && (s->m[2]==t0 || s->m[2]==t1 || s->m[2]==t2 || s->m[2]==0);
}


char *STAT_writehist(STAT_t *s, char *buf)
{
  int empty;
  unsigned char i;

  empty=TRUE;
  strcpy(buf,"");
   
  for (i=0;i<s->maxm;i++) {
    if (s->hist[s->m[i]]!=0) {
      if (!empty) strcat(buf,", ");
      sprintf(buf+strlen(buf),"%lu * %s%d",s->hist[s->m[i]],s->command,s->m[i]);
      empty=FALSE;
    }
  }
  return buf;
}


void STAT_done(STAT_t *s)
{
  free(s->command);
}

/******************************************************************************/
/** Outside *******************************************************************/
/******************************************************************************/

void getversion(unsigned char versionbyte, int *game, char **gs, char **ns) 
{
  int i;
  unsigned int n;

  *gs=(char*)malloc(100*sizeof(char));
  *gs[0]='\0';
  *ns=(char*)malloc(100*sizeof(char));
  *ns[0]='\0';

  /* calculate version number string */
  n=0; /* no version strings */
  for (i=0;i<VersionKeys;i++) {
    if (VersionKey[i].versionbyte==versionbyte) {
      if (VersionKey[i].game & (*game)) {
        if (!(n & VersionKey[i].string_no)) {
          if(strlen(*ns)) strcat(*ns," or ");
          strcat(*ns, VersionKey[i].versionstring);
          n|=VersionKey[i].string_no;
        } /* string not used */
      } /* game ok */
    } /* versionbyte ok */
  } /* for i */

  /* calculate game string */
  if (*game & (DOOM_old|DOOM_new)) {
    if (strlen(*gs)) strcat(*gs," or ");
    strcat(*gs,"DOOM");
  }
  if (*game & DOOM2) {
    if (strlen(*gs)) strcat(*gs," or ");
    strcat(*gs,"DOOM ][");
  }
  if (*game & HERETIC) {
    if (strlen(*gs)) strcat(*gs," or ");
    strcat(*gs,"HERETIC");
  }
  if (*game & (HEXEN_old|HEXEN_new)) {
    if (strlen(*gs)) strcat(*gs," or ");
    strcat(*gs,"HEXEN");
  }
  if (*game & STRIFE) {
    if (strlen(*gs)) strcat(*gs," or ");
    strcat(*gs,"STRIFE");
  }
}


unsigned char strtoversionbyte(char *s, int game)
{
  long int i,l;
  char *res;

  for (i=0;i<VersionKeys;i++) {
    l=strlen(VersionKey[i].versionstring)>strlen(s)?strlen(VersionKey[i].versionstring):strlen(s);
    if (l>3) l=3;
    if (game==0) {
      if (!strncmp(VersionKey[i].versionstring,s,l))
        return VersionKey[i].versionbyte;
    }
    else {
      if (!strncmp(VersionKey[i].versionstring,s,l) && VersionKey[i].game&game)
        return VersionKey[i].versionbyte;
    }
  }   
  i=strtol(s,&res,0)%256;
  if (res[0]!='\0') syserror(EINVAL,res);
  return i;
}


/*- file end ulmp.c ----------------------------------------------------------*/

