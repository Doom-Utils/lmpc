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
|  uqwd.c - implementation, general QWD routines                             |
\****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "tools.h"
#include "record.h"
#include "uqwd.h"


token_t QWD_token[]={
        { "sblock",              TOKEN_SBLOCK,         0 },
        { "cblock",              TOKEN_CBLOCK,         0 },
        { "fblock",              TOKEN_FBLOCK,         0 },
	{ "",                    GEN_NOTHING,          0 },
        { "time",                TOKEN_TIME,           0 },
        { "seq1",                TOKEN_SEQ1,           0 },
        { "seq2",                TOKEN_SEQ2,           0 },
        { "reliable",            TOKEN_RELIABLE,       0 },
        { "connless",            TOKEN_CONNLESS,       0 },
        { "data",                TOKEN_DATA,           0 },
        { "angles",              TOKEN_ANGLES,         0 },
        { "uk_angles",           TOKEN_UK_ANGLES,      0 },
        { "speed",               TOKEN_SPEED,          0 },
        { "impulse",             TOKEN_IMPULSE,        0 },
        { "fire",                TOKEN_FIRE,           0 },
        { "jump",                TOKEN_JUMP,           0 },
        { "flag",                TOKEN_FLAG,           0 },
        { "game",                TOKEN_GAME,           0 },
        { "load",                TOKEN_LOAD,           0 },
        { "connect",             TOKEN_CONNECT,        0 },
        { "ping",                TOKEN_PING,           0 },
        { "console",             TOKEN_CONSOLE,        0 },
        { "id",                  TOKEN_ID,             0 },
        { "unknown",             TOKEN_UNKNOWN,        0 },
        { "serverversion",       TOKEN_SERVERVERSION,  0 },
        { "client",              TOKEN_CLIENT,         0 },
        { "age",                 TOKEN_AGE,            0 },
        { "mapname",             TOKEN_MAPNAME,        0 },
        { "uk_floats",           TOKEN_UK_FLOATS,      0 },
        { "level",               TOKEN_LEVEL,          0 },
        { "origin",              TOKEN_ORIGIN,         0 },
        { "vol",                 TOKEN_VOL,            0 },
        { "attenuation",         TOKEN_ATTENUATION,    0 },
        { "soundnum",            TOKEN_SOUNDNUM,               0 },
        { "default_modelindex",  TOKEN_DEFAULT_MODELINDEX,     0 },
        { "default_frame",       TOKEN_DEFAULT_FRAME,          0 },
        { "default_colormap",    TOKEN_DEFAULT_COLORMAP,       0 },
        { "default_skin",        TOKEN_DEFAULT_SKIN,           0 },
        { "default_origin",      TOKEN_DEFAULT_ORIGIN,         0 },
        { "default_angles",      TOKEN_DEFAULT_ANGLES,         0 },
        { "entity",              TOKEN_ENTITY,                 0 },
        { "player",              TOKEN_PLAYER,                 0 },
        { "frags",               TOKEN_FRAGS,                  0 },
        { "entertime",           TOKEN_ENTERTIME,              0 },
        { "user",                TOKEN_USER,                   0 },
        { "style",               TOKEN_STYLE,                  0 },
        { "string",              TOKEN_STRING,                 0 },
        { "index",               TOKEN_INDEX,                  0 },
        { "value",               TOKEN_VALUE,                  0 },
        { "camera",              TOKEN_CAMERA,                 0 },
        { "save",                TOKEN_SAVE,                   0 },
        { "take",                TOKEN_TAKE,                   0 },
        { "cspeed_x",            TOKEN_CSPEED_X,               0 },
        { "cspeed_y",            TOKEN_CSPEED_Y,               0 },
        { "cspeed_z",            TOKEN_CSPEED_Z,               0 },
        { "uk_byte6",            TOKEN_UK_BYTE6,               0 },
        { "modelindex",          TOKEN_MODELINDEX,             0 },
        { "speed_x",             TOKEN_SPEED_X,                0 },
        { "speed_y",             TOKEN_SPEED_Y,                0 },
        { "speed_z",             TOKEN_SPEED_Z,                0 },
        { "angles_1",            TOKEN_ANGLES_1,               0 },
        { "angles_2",            TOKEN_ANGLES_2,               0 },
        { "angles_3",            TOKEN_ANGLES_3,               0 },
        { "frame",               TOKEN_FRAME,                  0 },
        { "packetentity",        TOKEN_PACKETENTITY,           0 },
        { "colormap",            TOKEN_COLORMAP,               0 },
        { "skin",                TOKEN_SKIN,                   0 },
        { "effects",             TOKEN_EFFECTS,                0 },
        { "origin_x",            TOKEN_ORIGIN_X,               0 },
        { "origin_y",            TOKEN_ORIGIN_Y,               0 },
        { "origin_z",            TOKEN_ORIGIN_Z,               0 },
        { "entitytype",          TOKEN_ENTITYTYPE,             0 },
        { "trace_endpos",        TOKEN_TRACE_ENDPOS,           0 },
        { "count",               TOKEN_COUNT,                  0 },
        { "channel",             TOKEN_CHANNEL,                0 },
        { "nail",                TOKEN_NAIL,                   0 },
        { "weapon",              TOKEN_WEAPON,                 0 },
        { "weaponframe",         TOKEN_WEAPONFRAME,            0 },
        { "deltapacketentity",   TOKEN_DELTAPACKETENTITY,      0 },
        { "game",                TOKEN_GAME,                   0 },
        { "size",                TOKEN_SIZE,                   0 },
        { "percent",             TOKEN_PERCENT,                0 },
        { "name",                TOKEN_NAME,                   0 },
        { "models",              TOKEN_MODELS,                 0 },
        { "sounds",              TOKEN_SOUNDS,                 0 },
        { "next",		 TOKEN_NEXT,		       0 },
        { "first",               TOKEN_FIRST,                  0 },
	{ "challenge",		 TOKEN_CHALLENGE,	       0 },
	{ "client_command",	 TOKEN_CLIENT_COMMAND,	       0 },
	{ "remove",		 TOKEN_REMOVE,		       0 },
	{ "unknown_mask_bits",	 TOKEN_UNKNOWN_MASK_BITS,      0 },
	{ "loss",		 TOKEN_LOSS,		       0 },

        { "disconnect",          TOKEN_DISCONNECT,          0x02 },
        { "updatestat",          TOKEN_UPDATESTAT,          0x03 },
        { "sound",               TOKEN_SOUND,               0x06 },
        { "print",               TOKEN_PRINT,               0x08 },
        { "stufftext",           TOKEN_STUFFTEXT,           0x09 },
        { "setangle",            TOKEN_SETANGLE,            0x0A },
        { "serverdata",          TOKEN_SERVERDATA,          0x0B },
        { "lightstyle",          TOKEN_LIGHTSTYLE,          0x0C },
        { "updatefrags",         TOKEN_UPDATEFRAGS,         0x0E },
        { "stopsound",           TOKEN_STOPSOUND,           0x10 },
        { "damage",              TOKEN_DAMAGE,              0x13 },
        { "spawnstatic",         TOKEN_SPAWNSTATIC,         0x14 },
        { "spawnbaseline",       TOKEN_SPAWNBASELINE,       0x16 },
        { "temp_entity",         TOKEN_TEMP_ENTITY,         0x17 },
        { "setpause",            TOKEN_SETPAUSE,            0x18 },
        { "centerprint",         TOKEN_CENTERPRINT,         0x1A },
        { "killedmonster",       TOKEN_KILLEDMONSTER,       0x1B },
        { "foundsecret",         TOKEN_FOUNDSECRET,         0x1C },
        { "spawnstaticsound",    TOKEN_SPAWNSTATICSOUND,    0x1D },
        { "intermission",        TOKEN_INTERMISSION,        0x1E },
        { "finale",              TOKEN_FINALE,              0x1F },
        { "cdtrack",             TOKEN_CDTRACK,             0x20 },
        { "sellscreen",          TOKEN_SELLSCREEN,          0x21 },
        { "smallkick",           TOKEN_SMALLKICK,           0x22 },
        { "bigkick",             TOKEN_BIGKICK,             0x23 },
        { "updateping",          TOKEN_UPDATEPING,          0x24 },
        { "updateentertime",     TOKEN_UPDATEENTERTIME,     0x25 },
        { "updatestatlong",      TOKEN_UPDATESTATLONG,      0x26 },
        { "muzzleflash",         TOKEN_MUZZLEFLASH,         0x27 },
        { "updateuserinfo",      TOKEN_UPDATEUSERINFO,      0x28 },
        { "download",            TOKEN_DOWNLOAD,            0x29 },
        { "playerinfo",          TOKEN_PLAYERINFO,          0x2A },
        { "nails",               TOKEN_NAILS,               0x2B },
        { "choke",               TOKEN_CHOKE,               0x2C },
        { "modellist",           TOKEN_MODELLIST,           0x2D },
        { "soundlist",           TOKEN_SOUNDLIST,           0x2E },
        { "packetentities",      TOKEN_PACKETENTITIES,      0x2F },
        { "deltapacketentities", TOKEN_DELTAPACKETENTITIES, 0x30 },
        { "maxspeed",            TOKEN_MAXSPEED,            0x31 },
        { "entgravity",          TOKEN_ENTGRAVITY,          0x32 },
        { "setinfo",             TOKEN_SETINFO,             0x33 },
        { "serverinfo",          TOKEN_SERVERINFO,          0x34 },
        { "updatepl",            TOKEN_UPDATEPL,            0x35 },
        { NULL,                  0,                         0 }
};

#define NEXT(x) (x)=(x)->next
#define CHECKRET(x) { if ((x)==NULL) { return; } }
#define NEXTRET(x) { NEXT(x) ; CHECKRET(x); }
#define NEXTGOTO(x,g) { NEXT(x) ; if ((x)==NULL) { goto g; } }
/* #define cmdcheck(n,t,e,s) if(n->type!=t) syserror(QWDINTE,"expected %s, found %d in %s",e,t,s) */

#define cmdcheck(n,t,e,s) { \
if (n == NULL) syserror(QWDINTE,"expected %s, found NULL in %s",e,s); \
else if(n->type!=t) syserror(QWDINTE,"expected %s, found %d in %s",e,t,s); \
}

typedef float fvector[3];
typedef short svector[3];

/* check, if file is QWD */
unsigned long isQWDbin(char *filename)
{
  FILE *file;
  struct stat buf;
  size_t filesize;
  unsigned char buffer[10];
  unsigned long blocksize;
  unsigned long tics;
  unsigned long code;
  
  if ((file=fopen(filename, "rb"))==NULL) return 0;
  if (stat(filename,&buf)==-1) return 0;
  filesize=buf.st_size;
  rewind(file);
  tics=0;
  while (ftell(file)<(long)filesize) {
    tics++;
    /* 4 bytes float (time stamp) */
    if (fseek(file, 4, SEEK_CUR)!=0) return 0;
    /* code */
    if (fread(buffer, 1, 1, file)!=1) return 0;
    code=buffer[0];
    switch (code) {
      case 0:
        if (fseek(file, 0x24, SEEK_CUR) != 0) return 0;
      break;
      case 1:
        if (fread(buffer, 1, 4, file)!=4) return 0;
        blocksize = ((buffer[0])    ) +
                    ((buffer[1])<< 8) +
                    ((buffer[2])<<16) +
                    ((buffer[3])<<24);
        if (fseek(file, blocksize, SEEK_CUR)!=0) return 0;
      break;
      case 2:
        if (fseek(file, 8, SEEK_CUR) != 0) return 0;
      break;
      default:
        return 0;
    }
  } 
  if (ftell(file)!=(long)filesize) return 0;
  fclose(file);
  return tics;
}

/* check, if file is QWD source */
int isQWDtxt(char* filename)
{
  FILE *file;
  /*
  struct stat buf;
  size_t filesize;
  */
  #define BUFFER_SIZE 1000
  char buffer[BUFFER_SIZE];
  long blocksize;
  long p;
  int state;

  if ((file=fopen(filename, "rb"))==NULL) return 0;
  /*
  if (stat(filename,&buf)==-1) return 0;
  filesize=buf.st_size;
  */
  rewind(file);
  if ((blocksize=fread(buffer,1,BUFFER_SIZE,file))==0) return 0;
  if(fclose(file)) return 0;
  if (blocksize<20) return 0;
  state = 0;
  for (p=0;p<blocksize;p++) {
    switch (state) {
      /* not a comment */
      case 0: if (strncmp (&(buffer[p]), "sblock", 6)==0) return 1;
              if (strncmp (&(buffer[p]), "cblock", 6)==0) return 1;
              if (strncmp (&(buffer[p]), "fblock", 6)==0) return 1;
              if (strncmp (&(buffer[p]), "//", 2)==0) { state=1; break; }
              if (buffer[p]!=' ' && buffer[p]!='\t' &&
                  buffer[p]!='\r' && buffer[p]!='\n' &&
                  buffer[p]!='\f') return 0;
              break;
      /* a comment */
      case 1: if (buffer[p]=='\n') state=0;
              break;
    }
  }
  /* If it comes to this line it may be a DM2 txt file but we don't
     beleave it. */
  return 0;
}

void QWD_init(QWD_t* d, char *filename, char *mode)
{
  struct stat buf;
  int i;
    
  if ((d->filename = strdup(filename))==NULL) syserror(errno,"strdup");
  if (strcmp(d->filename,"-")==0) {
    d->file=stdout;
  }
  else {
    QWD_open(d,mode);
    if (stat(d->filename,&buf)==-1) syserror(errno,d->filename);
    d->filesize=buf.st_size;
  }
  d->nummodels = 0;
  d->numsounds = 0;
  d->lastHealth = 100;
  d->lastArmor = 0;
  d->lastMinusHealth = 0;
  d->lastMinusArmor = 0;
  d->playermodel = 0;
  /* we need a rewind here because some fopen modes may seek at the end */
  rewind(d->file);
  d->serverversion = 24; /* standard setting, 1.64, 1st demo-capable version */
  for ( i=0 ; i<QWD_MAX_MODELS ; i++) 
    d->precache_models[i] = NULL;
  for ( i=0 ; i<QWD_MAX_SOUNDS ; i++) 
    d->precache_sounds[i] = NULL;
}

void QWD_prepare_write_bin(QWD_t* d)
{
  output_file = d->file;
  output_filename = d->filename;
}

void QWD_open(QWD_t *d, char *mode)
{
  if ((d->file=fopen(d->filename, mode))==NULL) syserror(errno,d->filename);
}
  
void QWD_close(QWD_t *d)
{
  if (fclose(d->file)) syserror(errno,d->filename);
  d->file=NULL;
}

void QWD_done(QWD_t* d)
{
  QWD_close(d);
  free(d->filename);
}

void QWD_block_read_bin(QWD_t *d, BB_t* m)
{
  int code;
  
  m->base = ftell(d->file);
  /* 9 bytes are always possible */
  if (fread(m->buffer,1,9,d->file)!=9) syserror(FIREAD,d->filename);
  /* after the time stamp */
  m->p = m->buffer+4;
  /* there is the end for now */
  m->end = &(m->buffer[9]);
  code = ReadByte(m);
  switch (code) {
    case 0: /* cblock */
      m->size = 36; /* real cblock data */
      if (fread(m->buffer+9,1,m->size - 9 + 5,d->file)!=(size_t)(m->size - 9 + 5))
        syserror(FIREAD,d->filename);
      m->size += 5; /* standard header */
    break;
    case 1: /* sblock */
      m->size = ReadLong(m);
      if (fread(m->buffer+9,1,m->size,d->file)!=(size_t)(m->size)) 
        syserror(FIREAD,d->filename);
      m->size += 9;
    break;
    case 2: /* fblock */
      m->size = 8; /* real fblock data */
      if (fread(m->buffer+9,1,m->size - 9 + 5,d->file)!=(size_t)(m->size - 9 + 5))
        syserror(FIREAD,d->filename);
      m->size += 5; /* standard header */
    break;
    default:
      syserror(FIREAD,d->filename);
    break;
  }
  if (m->size > QWD_MAX_MESSAGE_SIZE) syserror(WQWD,d->filename);
  m->p = m->buffer;
  m->end = &(m->buffer[m->size]);
  m->top = (void *) d;
}

#define QWDTOP ((QWD_t*)(m->top))
/* #define SERVERVERSION (QWDTOP->serverversion) */

node* QWD_bin_to_node(BB_t *m, int opt)
{
  node *n, *tn, *ttn;
  float time;
  int code;
  char ts[1000];
   
  /* binary in */
  time = ReadFloat(m);
  n = node_add_hint(
        node_command_init(TOKEN_TIME,V_TIME,H_SIMPLE,NODE_VALUE_TIME_dup(time),0),
        H_SIMPLE
      );
  code = ReadByte(m);
  /* fprintf(stderr,"block %d code %d\n", QWDTOP->frame, code); */
  switch (code) {
    case 0: /* cblock */ {
      int i;
      long load;
      fvector angles;
      svector speed;
      fvector uk_angles;
      long flag, impulse;
      
      /* binary in */
      load = ReadLong(m);
      for (i=0;i<3;i++) angles[i] = ReadFloat(m);
      for (i=0;i<3;i++) speed[i] = ReadShort(m);
      flag = ReadByte(m);
      impulse = ReadByte(m);
      for (i=0;i<3;i++) uk_angles[i] = ReadFloat(m);

      /* construct node tree */
      node_add_next(n, 
        node_command_init(TOKEN_LOAD, V_INT, H_LONG, NODE_VALUE_INT_dup(load), 0)
      );
      node_add_next(n,
        node_triple_command_init(TOKEN_ANGLES, V_FLOAT, H_FLOAT, 
                                 NODE_VALUE_FLOAT_dup(angles[0]),
                                 NODE_VALUE_FLOAT_dup(angles[1]),
                                 NODE_VALUE_FLOAT_dup(angles[2]),
                                 0)
      );
      node_add_next(n,
        node_triple_command_init(TOKEN_SPEED, V_INT, H_SHORT, 
                                 NODE_VALUE_INT_dup(speed[0]),
                                 NODE_VALUE_INT_dup(speed[1]),
                                 NODE_VALUE_INT_dup(speed[2]),
                                 0)
      );
      if (flag) {
        if (flag & 0x01) {
          node_add_next(n, node_init_all(TOKEN_FIRE, H_SIMPLE, NULL, 0));
        }
        if (flag & 0x02) {
          node_add_next(n, node_init_all(TOKEN_JUMP, H_SIMPLE, NULL, 0));
        }
        for (i=2,tn=NULL;i<8;i++) {
          if ((flag>>i) & 0x01) {
            tn=node_link(tn,node_init_all(V_INT, 0, 
                                          NODE_VALUE_INT_dup(i), 0));
          }
        }
        if (tn!=NULL) {
          node_add_next(n,node_init_all(TOKEN_FLAG,H_SIMPLE,tn,0));
        }
      }
      if (impulse) {
        node_add_next(n, node_command_init(TOKEN_IMPULSE, V_INT, H_BYTE, 
                                           NODE_VALUE_INT_dup(impulse),0)
                     );
      }
      node_add_next(n,
        node_triple_command_init(TOKEN_UK_ANGLES, V_FLOAT, H_FLOAT, 
                                 NODE_VALUE_FLOAT_dup(uk_angles[0]),
                                 NODE_VALUE_FLOAT_dup(uk_angles[1]),
                                 NODE_VALUE_FLOAT_dup(uk_angles[2]),
                                 0)
      );
      n = node_init_all(TOKEN_CBLOCK,0,n,0);
      if (opt & 0x01) {
        sprintf(ts,"%i",QWDTOP->frame);
        node_add_comment(n,NODE_VALUE_STRING_dup(ts));
      }       
    }
    break;
    case 1: /* sblock */ {
      /* unsigned long size; */
      unsigned long seq1, seq2;
      int rel1, rel2;
      long id;
      
      /* size = */ ReadLong(m);
      seq1 = ReadLong(m);
      if (seq1 == 0xFFFFFFFF) {
        /* connectionless block */
        id = ReadByte(m);
        switch (id) {
          case '\002': {
            char text[MAX_STRING_SIZE];
             
            ReadString(m,text);

            tn=node_command_init(TOKEN_DISCONNECT, V_STRING, H_STRING, 
                                 NODE_VALUE_STRING_dup(text),0);
          }
          break;
          case 0x42: { /* 'B' */
            char text[MAX_STRING_SIZE];
             
            ReadString(m,text);

            tn=node_command_init(TOKEN_CLIENT_COMMAND, V_STRING, H_STRING, 
                                 NODE_VALUE_STRING_dup(text),0);
          }
          break;
          case 0x63: { /* 'c' */
             char text[MAX_STRING_SIZE];
             
             ReadString(m,text);
                         
             tn=node_command_init(TOKEN_CHALLENGE, V_STRING, H_STRING,
                                  NODE_VALUE_STRING_dup(text),0);   
          }
          case 0x6a: { /* 'j' */
            tn=node_init(TOKEN_CONNECT, NULL, 0);
          }
          break;
          case 0x6b: { /* 'k' */
            tn=node_init(TOKEN_PING, NULL, 0);
          }
          break;
          case 0x6e: { /* 'n' */
            char text[MAX_STRING_SIZE];
             
            ReadString(m,text);

            tn=node_command_init(TOKEN_CONSOLE, V_STRING, H_STRING, 
                                 NODE_VALUE_STRING_dup(text),0);
          }
          break;
          default: {
            tn=node_command_init(TOKEN_ID, V_BYTEHEX, H_SIMPLE, 
                                 NODE_VALUE_BYTEHEX_dup(id),0);
            for (ttn=NULL; m->p < m->end;) {
              ttn=node_link(ttn,
                   node_init_all(V_BYTEHEX,H_SIMPLE,
                                 NODE_VALUE_BYTEHEX_dup((unsigned char)ReadByte(m)),0));
            }
            if (ttn!=NULL) 
              node_add_next(tn,node_init_all(TOKEN_DATA,H_SIMPLE,ttn,0));
          }
          break;
        }
        n=node_link(n,
           node_init_all(TOKEN_CONNLESS,H_SIMPLE,tn,0)
          ); 
      } /* end connectionless block */
      else {
        /* game block */
        seq2 = ReadLong(m);
        rel1 = (seq1 >> 31);
        rel2 = (seq2 >> 31);
        seq1 &= 0x7FFFFFFF;          
        seq2 &= 0x7FFFFFFF;
        n = node_link(n,node_command_init(TOKEN_SEQ1, V_INT, H_LONG, 
                                        NODE_VALUE_INT_dup(seq1),0));
        n = node_link(n,node_command_init(TOKEN_SEQ2, V_INT, H_LONG, 
                                        NODE_VALUE_INT_dup(seq2),0));
        if (rel1 || rel2) {
          tn = NULL;
          if (rel1) {
            tn = node_link(tn, node_init_all(V_INT,0,
                                             NODE_VALUE_INT_dup(1),0));
          }
          if (rel2) {
            tn = node_link(tn, node_init_all(V_INT,0,
                                             NODE_VALUE_INT_dup(2),0));
          }
          node_add_next(n, node_init_all(TOKEN_RELIABLE, 0, tn, 0));
        }
        while (m->p < m->end) {
          if (opt & 0x01) {
            tn=node_init_all(GEN_NOTHING,H_NOTHING,NULL,0);
            sprintf(ts, "0x%08lX", m->base + (m->p-m->buffer));
            node_add_comment(tn,NODE_VALUE_STRING_dup(ts));
            n=node_link(n,tn);
          }
          id = ReadByte(m);
          switch(id) {
            case 0x02: { /* disconnect */
              /* construct node tree */
              n=node_link(n,node_init_all(TOKEN_DISCONNECT, H_SIMPLE, NULL, 0));
            }
            break;
            case 0x03: { /* updatestat */
              /* variables */
              long index; 
              long value;
              
              /* binary in */
              index = ReadByte(m);
              value = ReadByte(m);

              /* construct node tree */
              n=node_link(n,
                  node_init_all(TOKEN_UPDATESTAT, H_SIMPLE, 
                    node_add_next(
                      node_command_init(TOKEN_INDEX, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(index),0),
                      node_command_init(TOKEN_VALUE, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(value),0)
                    ), 
                    0)
                );
            }
            break;
            case 0x06: { /* sound */
              /* variables */
              float vol;
              float attenuation;
              long channel;
              long entity;
              long soundnum;
              fvector origin;

              int i;
     
              /* binary in */
              long entity_channel;

              /* init */
              vol = attenuation = 0.0;

              entity_channel = ReadShort(m);
              channel = entity_channel & 0x07;
              entity = (entity_channel >> 3) & 0x03FF;
              if (entity_channel & 0x8000) vol = (float) ReadByte(m) /255.0;
              if (entity_channel & 0x4000) attenuation = (float) ReadByte(m) / 64.0;
              soundnum = ReadByte(m);
              for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);

              /* construct node tree */
              tn = NULL;
              tn=node_link(tn,node_command_init(TOKEN_CHANNEL,V_INT,0,
                              NODE_VALUE_INT_dup(channel),0));
              tn=node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,0,
                              NODE_VALUE_INT_dup(entity),0));
              if (entity_channel & 0x8000) {
                tn=node_link(tn,node_command_init(TOKEN_VOL,V_FLOAT,H_VOL,
                                                  NODE_VALUE_FLOAT_dup(vol),0));
              }           
              if (entity_channel & 0x4000) {
                tn=node_link(tn,node_command_init(TOKEN_ATTENUATION,V_FLOAT,H_ATTENUATION,
                                NODE_VALUE_FLOAT_dup(attenuation),0));
              }
              ttn=node_command_init(TOKEN_SOUNDNUM,V_INT,H_BYTE,
                  NODE_VALUE_INT_dup(soundnum),0);
              if (soundnum >= 1 && soundnum <= QWDTOP->numsounds) {
                node_add_comment(ttn,NODE_VALUE_STRING_dup(QWDTOP->precache_sounds[soundnum]));
              }
              tn=node_link(tn,ttn);
              tn=node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                              NODE_VALUE_FLOAT_dup(origin[0]),
                              NODE_VALUE_FLOAT_dup(origin[1]),
                              NODE_VALUE_FLOAT_dup(origin[2]),0));
              n=node_link(n,node_init_all(TOKEN_SOUND,H_QWD_SOUND,tn,0));
            }
            break;
            case 0x08: { /* print */
              long level; 
              char string[MAX_STRING_SIZE];
              
              level = ReadByte(m);;
              ReadString(m,string);
              
              n=node_link(n,
                  node_init_all(TOKEN_PRINT, H_SIMPLE, 
                    node_add_next(
                      node_command_init(TOKEN_LEVEL, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(level),0),
                      node_command_init(TOKEN_STRING, V_STRING, H_STRING, 
                                        NODE_VALUE_STRING_dup(string),0)
                    ), 
                    0)
                );
            }
            break;
            case 0x09: { /* stufftext */
              char text[MAX_STRING_SIZE];
              
              ReadString(m,text);
              
              n=node_link(n,node_add_hint(node_command_init(TOKEN_STUFFTEXT, V_STRING, H_STRING,
                                              NODE_VALUE_STRING_dup(text),0),H_SIMPLE)
                         );
            }
            break;
            case 0x0A: { /* setangle */ 
              /* variables */
              fvector angles;
              
              int i;
              
              /* binary in */
              for (i=0;i<3;i++) angles[i]=ReadAngle(m);
              
              /* construct node tree */              
              n=node_link(n,
                  node_add_hint(
                    node_triple_command_init(TOKEN_SETANGLE, V_FLOAT, H_ANGLE, 
                                       NODE_VALUE_FLOAT_dup(angles[0]),
                                       NODE_VALUE_FLOAT_dup(angles[1]),
                                       NODE_VALUE_FLOAT_dup(angles[2]),0),
                    H_SIMPLE
                  )
                );
            }
            break;
            case 0x0B: { /* serverdata */
              
              long serverversion;
              long age;
              long client;
              char game[MAX_STRING_SIZE];
              char mapname[MAX_STRING_SIZE];
              float uk_floats[10];
              
              int i;
              
              serverversion = ReadLong(m);
              age = ReadLong(m);
              ReadString(m, game);
              client = ReadByte(m);
              ReadString(m, mapname);
              if (serverversion >= 25) { /* from 2.00 on */
                for (i=0;i<10;i++) {
                  uk_floats[i] = ReadFloat(m);
                }
              }
              
              /* memorize the protocol version */
              /* in a global variable: bad */
              SERVERVERSION = serverversion;
              /* in the QWD structure: not usable any more */
              (QWDTOP->serverversion) = serverversion;
              
              tn = node_command_init(TOKEN_SERVERVERSION, V_INT, H_LONG, 
                                     NODE_VALUE_INT_dup(serverversion),0);
              tn = node_link(tn, 
                     node_command_init(TOKEN_AGE, V_INT, H_LONG, 
                                       NODE_VALUE_INT_dup(age),0)
                   );
              tn = node_link(tn, 
                     node_command_init(TOKEN_GAME, V_STRING, H_STRING,
                                       NODE_VALUE_STRING_dup(game),0)
                   );
              tn = node_link(tn, 
                     node_command_init(TOKEN_CLIENT, V_INT, H_BYTE, 
                                       NODE_VALUE_INT_dup(client),0)
                   );
              tn = node_link(tn, 
                     node_command_init(TOKEN_MAPNAME, V_STRING, H_STRING,
                                       NODE_VALUE_STRING_dup(mapname),0)
                   );
              if (SERVERVERSION >= 25) { /* from 2.00 on */
                for (i=0,ttn=0;i<10;i++) {
                  ttn=node_link(ttn,node_init_all(V_FLOAT, H_FLOAT,
                                                  NODE_VALUE_FLOAT_dup(uk_floats[i]),0));
                }
                tn=node_link(tn,node_init_all(TOKEN_UK_FLOATS,H_SIMPLE,ttn,0));
              }
              n=node_link(n,node_init_all(TOKEN_SERVERDATA, H_SIMPLE, tn, 0));
            }
            break;
            case 0x0C: { /* lightstyle */
              /* variables */
              long style; 
              char string[MAX_STRING_SIZE];
              
              /* binary in */
              style = ReadByte(m);
              ReadString(m,string);

              /* construct node tree */
              n=node_link(n,
                  node_init_all(TOKEN_LIGHTSTYLE, H_SIMPLE, 
                    node_add_next(
                      node_command_init(TOKEN_STYLE, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(style),0),
                      node_command_init(TOKEN_STRING, V_STRING, H_STRING, 
                                        NODE_VALUE_STRING_dup(string),0)
                    ), 
                    0)
                );
            }
            break;
            case 0x0E: { /* updatefrags */  
              /* variables */
              long player;
              long frags;

              /* binary in */
              player = ReadByte(m);
              frags = ReadShort(m);

              /* construct node tree */
              n=node_link(n,
                  node_init_all(TOKEN_UPDATEFRAGS,H_SIMPLE,
                    node_add_next(  
                      node_command_init(TOKEN_PLAYER,V_INT,H_BYTE,
                                        NODE_VALUE_INT_dup(player),0),
                      node_command_init(TOKEN_FRAGS,V_INT,
                                        H_SHORT,NODE_VALUE_INT_dup(frags),0)
                    ),
                    0)
                );
            }
            break;
            case 0x10: { /* stopsound */
              /* variables */
              long channel;
              long entity;

              long entity_channel;

              /* binary in */
              entity_channel = ReadShort(m);

              channel = entity_channel & 0x07;
              entity = (entity_channel >> 3) & 0x01FFF;

              /* construct node tree */
              tn = node_add_next (
                node_command_init(TOKEN_CHANNEL,V_INT,0,NODE_VALUE_INT_dup(channel),0),
                node_command_init(TOKEN_ENTITY,V_INT,0,NODE_VALUE_INT_dup(entity),0)
              );
              n = node_link(n,node_init_all(TOKEN_STOPSOUND,H_QWD_STOPSOUND,tn,0));
            }
            break;
            case 0x13: { /* damage */
              /* variables */
              long save;
              long take;
              fvector origin;
                    
              int i;
                      
              /* binary in */
              save = ReadByte(m);
              take = ReadByte(m);
              for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
            
              /* add */
              QWDTOP->lastMinusArmor = save;
              QWDTOP->lastMinusHealth = take;

              /* construct node tree */
              n=node_link(n,
              node_init_all(TOKEN_DAMAGE,H_SIMPLE,
                node_link(
                  node_link(
                    node_command_init(TOKEN_SAVE, V_INT, H_BYTE, 
                                      NODE_VALUE_INT_dup(save),0)
                  ,
                    node_command_init(TOKEN_TAKE, V_INT, H_BYTE, 
                                      NODE_VALUE_INT_dup(take),0)
                  )
                ,
                  node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                                           NODE_VALUE_FLOAT_dup(origin[0]),
                                           NODE_VALUE_FLOAT_dup(origin[1]),
                                           NODE_VALUE_FLOAT_dup(origin[2]),
                                           0)
                ),0
              ));
            }
            break;
            case 0x14: { /* spawnstatic */
              /* variables */
              long default_modelindex;
              long default_frame;
              long default_colormap;
              long default_skin;
              fvector default_origin;
              fvector default_angles;

              int i;

              /* binary in */
              default_modelindex = ReadByte(m);
              default_frame = ReadByte(m);
              default_colormap = ReadByte(m);
              default_skin = ReadByte(m);
              for (i=0 ; i<3 ; i++) {
                default_origin[i] = ReadCoord(m);
                default_angles[i] = ReadAngle(m); 
              }

              /* construct node tree and return the root of it */
              tn = NULL;              
              ttn=node_command_init(TOKEN_DEFAULT_MODELINDEX,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(default_modelindex),0);
              if (default_modelindex >= 1 &&
                  default_modelindex <= QWDTOP->nummodels &&
                  QWDTOP->precache_models[default_modelindex][0]!='*') {
                node_add_comment(ttn,NODE_VALUE_STRING_dup(QWDTOP->precache_models[default_modelindex]));
              }
              tn=node_link(tn,ttn);
              tn=node_link(tn,node_command_init(TOKEN_DEFAULT_FRAME,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_frame),0));
              tn=node_link(tn,node_command_init(TOKEN_DEFAULT_COLORMAP,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_colormap),0));
              tn=node_link(tn,node_command_init(TOKEN_DEFAULT_SKIN,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_skin),0));
              tn=node_link(tn,
                   node_triple_command_init(TOKEN_DEFAULT_ORIGIN,V_FLOAT,H_COORD,
                     NODE_VALUE_FLOAT_dup(default_origin[0]),
                     NODE_VALUE_FLOAT_dup(default_origin[1]),
                     NODE_VALUE_FLOAT_dup(default_origin[2]),0));
              tn=node_link(tn,
                   node_triple_command_init(TOKEN_DEFAULT_ANGLES,V_FLOAT,H_ANGLE,
                     NODE_VALUE_FLOAT_dup(default_angles[0]),
                     NODE_VALUE_FLOAT_dup(default_angles[1]),
                     NODE_VALUE_FLOAT_dup(default_angles[2]),0));
              n=node_link(n,node_init_all(TOKEN_SPAWNSTATIC,H_QWD_SPAWNSTATIC,tn,0));
            }
            break;
            case 0x16: { /* spawnbaseline */
              /* variables */
              long entity;
              long default_modelindex;
              long default_frame;
              long default_colormap;
              long default_skin;
              fvector default_origin;
              fvector default_angles;

              int i;

              /* binary in */
              entity = ReadShort(m);
              default_modelindex = ReadByte(m);
              default_frame = ReadByte(m);
              default_colormap = ReadByte(m);
              default_skin = ReadByte(m);
              for (i=0 ; i<3 ; i++) {
                default_origin[i] = ReadCoord(m);
                default_angles[i] = ReadAngle(m); 
              }

              /* construct node tree and return the root of it */
              tn = NULL;
              tn=node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,
                              NODE_VALUE_INT_dup(entity),0));
              ttn=node_command_init(TOKEN_DEFAULT_MODELINDEX,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(default_modelindex),0);
              if (default_modelindex >= 1 &&
                  default_modelindex <= QWDTOP->nummodels &&
                  QWDTOP->precache_models[default_modelindex][0]!='*') {
                node_add_comment(ttn,NODE_VALUE_STRING_dup(QWDTOP->precache_models[default_modelindex]));
              }
              tn=node_link(tn,ttn);
              tn=node_link(tn,node_command_init(TOKEN_DEFAULT_FRAME,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_frame),0));
              tn=node_link(tn,node_command_init(TOKEN_DEFAULT_COLORMAP,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_colormap),0));
              tn=node_link(tn,node_command_init(TOKEN_DEFAULT_SKIN,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_skin),0));
              tn=node_link(tn,
                   node_triple_command_init(TOKEN_DEFAULT_ORIGIN,V_FLOAT,H_COORD,
                     NODE_VALUE_FLOAT_dup(default_origin[0]),
                     NODE_VALUE_FLOAT_dup(default_origin[1]),
                     NODE_VALUE_FLOAT_dup(default_origin[2]),0));
              tn=node_link(tn,
                   node_triple_command_init(TOKEN_DEFAULT_ANGLES,V_FLOAT,H_ANGLE,
                     NODE_VALUE_FLOAT_dup(default_angles[0]),
                     NODE_VALUE_FLOAT_dup(default_angles[1]),
                     NODE_VALUE_FLOAT_dup(default_angles[2]),0));
              n=node_link(n,node_init_all(TOKEN_SPAWNBASELINE,H_QWD_SPAWNBASELINE,tn,0));
            }
            break;
            case 0x17: { /* temp_entity */
              /* variables */
              long entitytype;
              long entity;
              vec3_t origin;
              vec3_t trace_endpos;
              long count;
              
              int i;
              
              /* init */
              entity = 0; count = 0;

              /* binary in */
              entitytype = ReadByte(m);
              switch (entitytype) {
                case 0:
                case 1:
                case 3:
                case 4:
                case 7:
                case 8:
                case 10:
                case 11:
                case 13:
                  for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
                break; 
                case 5:
                case 6:
                case 9:
                  entity = ReadShort(m);
                  for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
                  for (i=0 ; i<3 ; i++) trace_endpos[i] = ReadCoord(m);
                break;
                case 2:
                case 12:
                  count = ReadByte(m);
                  for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
                break;
              }          
              
              /* construct node tree */
              tn = NULL;              
              tn = node_link(tn,node_command_init(TOKEN_ENTITYTYPE,V_INT,H_BYTE,
                                NODE_VALUE_INT_dup(entitytype),0));
              switch (entitytype) {
                case 0:
                case 1:
                case 3:
                case 4:
                case 7:
                case 8:
                case 10:
                case 11:
                case 13:
                  tn=node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                                  NODE_VALUE_FLOAT_dup(origin[0]),
                                  NODE_VALUE_FLOAT_dup(origin[1]),
                                  NODE_VALUE_FLOAT_dup(origin[2]),0));
                break;
                case 5:
                case 6:
                case 9:
                  tn=node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,
                                  NODE_VALUE_INT_dup(entity),0));
                  tn=node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                                  NODE_VALUE_FLOAT_dup(origin[0]),
                                  NODE_VALUE_FLOAT_dup(origin[1]),
                                  NODE_VALUE_FLOAT_dup(origin[2]),0));
                  tn=node_link(tn,node_triple_command_init(TOKEN_TRACE_ENDPOS,V_FLOAT,H_COORD,
                                  NODE_VALUE_FLOAT_dup(trace_endpos[0]),
                                  NODE_VALUE_FLOAT_dup(trace_endpos[1]),
                                  NODE_VALUE_FLOAT_dup(trace_endpos[2]),0));
                break;
                case 2:
                case 12:
                  tn=node_link(tn,node_command_init(TOKEN_COUNT,V_INT,H_BYTE,
                                  NODE_VALUE_INT_dup(count),0));
                  tn=node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                                  NODE_VALUE_FLOAT_dup(origin[0]),
                                  NODE_VALUE_FLOAT_dup(origin[1]),
                                  NODE_VALUE_FLOAT_dup(origin[2]),0));
                break;
              }
              n=node_link(n,node_init_all(TOKEN_TEMP_ENTITY,H_SIMPLE,tn,0));
            }
            break;
            case 0x18: { /* setpause */
              long pausestate;

              pausestate = ReadByte(m);

              n=node_link(n,
                  node_add_hint(
                    node_command_init(TOKEN_SETPAUSE,
                                      V_INT,
                                      H_BYTE,
                                      NODE_VALUE_INT_dup(pausestate),
                                      0),
                    H_SIMPLE)
                );
            }
            break;
            case 0x1A: { /* centerprint */
              char text[MAX_STRING_SIZE];
              
              ReadString(m,text);
              
              n=node_link(n,
                  node_add_hint(
                    node_command_init(TOKEN_CENTERPRINT, V_STRING, H_STRING,
                                      NODE_VALUE_STRING_dup(text),0),
                  H_SIMPLE)
                );
            }
            break;
            case 0x1B: { /* killedmonster */
              /* construct node tree */
              n=node_link(n,node_init_all(TOKEN_KILLEDMONSTER, H_SIMPLE, NULL,0));
            }
            break;
            case 0x1C: { /* foundsecret */
              /* construct node tree */
              n=node_link(n,node_init_all(TOKEN_FOUNDSECRET, H_SIMPLE, NULL,0));
            }
            break;
            case 0x1D: { /* spawnstaticsound */
              /* variables */
              fvector origin;
              long soundnum;
              float vol;
              float attenuation;

              int i;

              /* binary in */
              for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
              soundnum = ReadByte(m);
              vol = (float) ReadByte(m) / 255.0;
              attenuation = (float) ReadByte(m) / 64.0;

              /* construct node tree and return the root of it */
              tn = node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                     NODE_VALUE_FLOAT_dup(origin[0]),
                     NODE_VALUE_FLOAT_dup(origin[1]),
                     NODE_VALUE_FLOAT_dup(origin[2]),0);
              ttn = node_command_init(TOKEN_SOUNDNUM,V_INT,H_BYTE,
                                      NODE_VALUE_INT_dup(soundnum),0);
              if (soundnum >= 1 && soundnum <= QWDTOP->numsounds) {
                node_add_comment(ttn,NODE_VALUE_STRING_dup(QWDTOP->precache_sounds[soundnum]));
              }
              tn=node_link(tn,ttn);
              tn=node_link(tn,node_command_init(TOKEN_VOL,V_FLOAT,H_VOL,
                              NODE_VALUE_FLOAT_dup(vol),0));
              tn=node_link(tn,node_command_init(TOKEN_ATTENUATION,V_FLOAT,
                              H_ATTENUATION,NODE_VALUE_FLOAT_dup(attenuation),0));
              n=node_link(n,node_init_all(TOKEN_SPAWNSTATICSOUND,H_SIMPLE,tn,0));
            }
            break;
            case 0x1E: { /* intermission */
              /* variables */

              fvector origin;
              fvector angles;

              int i;

              /* binary in */
              for ( i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
              for ( i=0 ; i<3 ; i++) angles[i] = ReadAngle(m);

              /* construct node tree */
              n=node_link(n,
              node_init_all(TOKEN_INTERMISSION,H_SIMPLE,
                node_link(
                  node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                                           NODE_VALUE_FLOAT_dup(origin[0]),
                                           NODE_VALUE_FLOAT_dup(origin[1]),
                                           NODE_VALUE_FLOAT_dup(origin[2]),
                                           0)
                ,
                  node_triple_command_init(TOKEN_ANGLES,V_FLOAT,H_ANGLE,
                                           NODE_VALUE_FLOAT_dup(angles[0]),
                                           NODE_VALUE_FLOAT_dup(angles[1]),
                                           NODE_VALUE_FLOAT_dup(angles[2]),
                                           0)
                ),0));                                             
            }
            break;
            case 0x1F: { /* finale */
              char text[MAX_STRING_SIZE];
              
              ReadString(m,text);
              
              n=node_link(n,
                  node_add_hint(
                    node_command_init(TOKEN_FINALE, V_STRING, H_STRING,
                                      NODE_VALUE_STRING_dup(text),0),
                    H_SIMPLE)
                );
            }
            break;
            case 0x20: { /* cdtrack */
              long track;
              
              track = ReadByte(m);
              
              n = node_link(n,
                    node_add_hint(
                      node_command_init(TOKEN_CDTRACK, V_INT, H_BYTE,
                                        NODE_VALUE_INT_dup(track), 0),
                      H_SIMPLE)
                  );
            }
            break;
            case 0x21: { /* sellscreen */
              /* construct node tree */
              n=node_link(n,node_init_all(TOKEN_SELLSCREEN, H_SIMPLE, NULL,0));
            }
            break;
            case 0x22: { /* smallkick */
              /* construct node tree */
              n=node_link(n,node_init_all(TOKEN_SMALLKICK, H_SIMPLE, NULL,0));
            }
            break;
            case 0x23: { /* bigkick */
              /* construct node tree */
              n=node_link(n,node_init_all(TOKEN_BIGKICK, H_SIMPLE, NULL,0));
            }
            break;
            case 0x24: { /* updateping */
              /* variables */
              long player;
              long ping;

              /* binary in */
              player = ReadByte(m);
              ping = ReadShort(m);

              tn=NULL;
              tn=node_link(tn,
                   node_command_init(TOKEN_PLAYER, V_INT, H_BYTE, 
                                     NODE_VALUE_INT_dup(player), 0));
              tn=node_link(tn,
                   node_command_init(TOKEN_PING, V_INT, H_SHORT, 
                                     NODE_VALUE_INT_dup(ping), 0));
              n=node_link(n,node_init_all(TOKEN_UPDATEPING, H_SIMPLE, tn, 0));
            }
            break;
            case 0x25: { /* updateentertime */
              /* variables */
              long player;
              float entertime;

              /* binary in */
              player = ReadByte(m);
              entertime = ReadFloat(m);

              /* construct node tree */
              n=node_link(n,
                  node_init_all(TOKEN_UPDATEENTERTIME, H_SIMPLE, 
                    node_add_next(
                      node_command_init(TOKEN_PLAYER, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(player),0),
                      node_command_init(TOKEN_ENTERTIME, V_TIME, H_SIMPLE, 
                                        NODE_VALUE_TIME_dup(entertime),0)
                    ), 
                    0)
                );
            }
            break;
            case 0x26: { /* updatestatlong */
              /* variables */
              long index; 
              long value;
              
              /* binary in */
              index = ReadByte(m);
              value = ReadLong(m);

              /* construct node tree */
              n=node_link(n,
                  node_init_all(TOKEN_UPDATESTATLONG, H_SIMPLE, 
                    node_add_next(
                      node_command_init(TOKEN_INDEX, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(index),0),
                      node_command_init(TOKEN_VALUE, V_INT, H_LONG, 
                                        NODE_VALUE_INT_dup(value),0)
                    ), 
                    0)
                );
            }
            break;
            case 0x27: { /* muzzleflash */
              /* variables */
              long entity;

              /* binary in */
              entity = ReadShort(m);

              /* construct node tree */
              n = node_link(
                    n,
                    node_add_hint(
                      node_command_init(TOKEN_MUZZLEFLASH, V_INT, H_SHORT,
                                        NODE_VALUE_INT_dup(entity), 0),
                      H_SIMPLE)
                  );
            }
            break;
            case 0x28: { /* updateuserinfo */
              /* variables */
              long player;
              long user;
              char string[MAX_STRING_SIZE];

              /* binary in */
              player = ReadByte(m);
              user = ReadLong(m);
              ReadString(m,string);
              
              tn=NULL;
              tn=node_link(tn,
                   node_command_init(TOKEN_PLAYER, V_INT, H_BYTE, 
                                     NODE_VALUE_INT_dup(player), 0));
              tn=node_link(tn,
                   node_command_init(TOKEN_USER, V_INT, H_LONG, 
                                     NODE_VALUE_INT_dup(user), 0));
              tn=node_link(tn,
                   node_command_init(TOKEN_STRING, V_STRING, H_STRING, 
                                     NODE_VALUE_STRING_dup(string), 0));
              n=node_link(n,node_init_all(TOKEN_UPDATEUSERINFO, H_SIMPLE, tn, 0));
            }
            break;
            case 0x29: { /* download */
              long size;
              long percent;
              unsigned char data[65536];

              int i;
 
              size = ReadShort(m);
              percent = ReadByte(m);
              for (i=0 ; i<size ; i++) {
                data[i] = ReadByte(m);
              }

              tn = NULL;
              tn = node_link(tn,node_command_init(TOKEN_SIZE,V_INT,H_SHORT,NODE_VALUE_INT_dup(size),0));
              tn = node_link(tn, node_command_init(TOKEN_PERCENT, V_INT, H_BYTE, NODE_VALUE_INT_dup(percent),0));
              for (ttn=NULL, i=0 ; i<size ; i++) {
                ttn=node_link(ttn,node_init(V_BYTEHEX, NODE_VALUE_INT_dup(data[i]),0));
              }
              tn=node_link(tn, node_init(TOKEN_DATA,ttn,0));
              n=node_link(n,node_init_all(TOKEN_DOWNLOAD, H_SIMPLE, tn, 0)); 
            }
            break;
            case 0x2A: { /* playerinfo */
              /* variables */
              long player;
              long mask;
              fvector origin;
              long frame;
              float ping = 0.0;
              fvector cspeed;
              long modelindex;
              long uk_byte6;
              long weapon;
              long weaponframe;
              
              long mask2=0;
              long load=0;
              fvector angles;
              svector speed;
              long flag=0;
              long impulse=0;
                            
              int i, j;
              
              /* binary in */
              player = ReadByte(m);
              mask = ReadShort(m);
              for (i=0;i<3;i++) origin[i] = ReadCoord(m);
              frame = ReadByte(m);
              /* bit 0 */
              if (mask & 0x0001) {
                ping=(ReadByte(m) * 0.001);
              }
              /* bit 1 */
              if (mask & 0x0002) {
                mask2 = ReadByte(m);
                
                if (SERVERVERSION>=27) { /* from game version 2.29BETA on */
                  angles[0] = (mask2 & 0x01) ? ReadAngle16(m) : 0;
                  angles[1] = (mask2 & 0x80) ? ReadAngle16(m) : 0;
                  angles[2] = (mask2 & 0x02) ? ReadAngle16(m) : 0;
                  speed[0] = (mask2 & 0x04) ? ReadShort(m) : 0;
                  speed[1] = (mask2 & 0x08) ? ReadShort(m) : 0;
                  speed[2] = (mask2 & 0x10) ? ReadShort(m) : 0;
                  flag = (mask2 & 0x20) ? ReadByte(m): 0;
                  impulse = (mask2 & 0x40) ? ReadByte(m): 0;
                  load = ReadByte(m);
                }
                else { /* SERVERVERSION<=26, game version up to 2.21 */
                  angles[0] = (mask2 & 0x01) ? ReadAngle16(m) : 0;
                  angles[1] = ReadAngle16(m);
                  angles[2] = (mask2 & 0x02) ? ReadAngle16(m) : 0;
                  speed[0] = (mask2 & 0x04) ? ReadByte(m) : 0;
                  speed[1] = (mask2 & 0x08) ? ReadByte(m) : 0;
                  speed[2] = (mask2 & 0x10) ? ReadByte(m) : 0;
                  flag = (mask2 & 0x20) ? ReadByte(m): 0;
                  impulse = (mask2 & 0x40) ? ReadByte(m): 0;
                  load = (mask2 & 0x80) ? ReadByte(m): 0;
                }
                
              }
              /* bit 2 */
              cspeed[0] = (mask & 0x0004) ? ReadCoord(m) : 0 ;
              /* bit 3 */
              cspeed[1] = (mask & 0x0008) ? ReadCoord(m) : 0 ;
              /* bit 4 */
              cspeed[2] = (mask & 0x0010) ? ReadCoord(m) : 0 ;
              /* bit 5 */
              /* so is it right but QWDTOP->playermodel will never be visible,
                 because modelindex comes in the node tree only if 
                 mask & 0x0020 */
              modelindex = (mask & 0x0020) ? ReadByte(m) : QWDTOP->playermodel;
              /* bit 6 */
              uk_byte6 = (mask & 0x0040) ? ReadByte(m) : 0;
              /* bit 7 */
              weapon = (mask & 0x0080) ? ReadByte(m) : 0;
              /* bit 8 */
              weaponframe = (mask & 0x0100) ? ReadByte(m) : 0;

              /* construct node tree */
              tn=NULL;
              if (mask & MASK_QWD_PLAYERINFO_UNKNOWNBITS)
                tn = node_link(
                       tn,
                       node_init(
                         TOKEN_UNKNOWN_MASK_BITS,
                         bitlist_to_nodes(mask & MASK_QWD_PLAYERINFO_UNKNOWNBITS),
                         0
                       )
                     );
              tn=node_link(tn,node_command_init(TOKEN_PLAYER, V_INT, H_BYTE, 
                                                NODE_VALUE_INT_dup(player), 0));
              tn=node_link(tn,node_triple_command_init(TOKEN_ORIGIN, V_FLOAT, H_COORD,
                                          NODE_VALUE_FLOAT_dup(origin[0]),
                                          NODE_VALUE_FLOAT_dup(origin[1]),
                                          NODE_VALUE_FLOAT_dup(origin[2]),
                                          0));
              tn=node_link(tn,node_command_init(TOKEN_FRAME, V_INT, H_BYTE, 
                                                NODE_VALUE_INT_dup(frame), 0));
              /* bit 0 */
              if (mask & 0x0001) {
                tn=node_link(tn,node_command_init(TOKEN_PING, V_FLOAT, 0, 
                                                  NODE_VALUE_FLOAT_dup(ping), 0));
              }
              /* bit 1 */
              if (mask & 0x0002) {

                ttn=NULL;
                
                if (SERVERVERSION >= 27) { /* from game version 2.29BETA on */
                  if (mask2 & 0x01) {
                    ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_1, V_FLOAT, H_ANGLE16,
                                      NODE_VALUE_FLOAT_dup(angles[0]),0));
                  }
                  if (mask2 & 0x80) {
                    ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_2, V_FLOAT, H_ANGLE16,
                                      NODE_VALUE_FLOAT_dup(angles[1]),0));
                  }
                  if (mask2 & 0x02) {
                    ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_3, V_FLOAT, H_ANGLE16,
                                      NODE_VALUE_FLOAT_dup(angles[2]),0));
                  }
                  if (mask2 & 0x04) {
                    ttn=node_link(ttn,node_command_init(TOKEN_SPEED_X, V_INT, H_SHORT,
                                      NODE_VALUE_INT_dup(speed[0]),0));
                  }
                  if (mask2 & 0x08) {
                    ttn=node_link(ttn,node_command_init(TOKEN_SPEED_Y, V_INT, H_SHORT,
                                      NODE_VALUE_INT_dup(speed[1]),0));
                  }
                  if (mask2 & 0x10) {
                    ttn=node_link(ttn,node_command_init(TOKEN_SPEED_Z, V_INT, H_SHORT,
                                      NODE_VALUE_INT_dup(speed[2]),0));
                  }
                  if (mask2 & 0x20) {
                    if (flag & 0x01) {
                      ttn=node_link(ttn, node_init_all(TOKEN_FIRE, H_SIMPLE, NULL, 0));
                    }
                    if (flag & 0x02) {
                      ttn=node_link(ttn, node_init_all(TOKEN_JUMP, H_SIMPLE, NULL, 0));
                    }
                    { 
                      node* tttn;
                    
                      for (i=2,tttn=NULL;i<8;i++) {
                        if ((flag>>i) & 0x01) {
                          tttn=node_link(tttn,node_init_all(V_INT, H_SIMPLE, 
                                                      NODE_VALUE_INT_dup(i), 0));
                        }
                      }
                      if (tttn!=NULL) {
                        ttn=node_link(ttn,node_init_all(TOKEN_FLAG,H_SIMPLE,tttn,0));
                      }
                    }
                  }
                  if (mask2 & 0x40) {
                    ttn=node_link(ttn,
                      node_command_init(TOKEN_IMPULSE, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(impulse),0));
                  }
                  ttn=node_link(ttn, 
                    node_command_init(TOKEN_LOAD, V_INT, H_BYTE, 
                                      NODE_VALUE_INT_dup(load), 0));
                }
                else { /* SERVERDATA <= 26, game version up to 2.21 */
                  if (mask2 & 0x01) {
                    ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_1, V_FLOAT, H_ANGLE16,
                                      NODE_VALUE_FLOAT_dup(angles[0]),0));
                  }
                  ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_2, V_FLOAT, H_ANGLE16,
                                      NODE_VALUE_FLOAT_dup(angles[1]),0));
                  if (mask2 & 0x02) {
                    ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_3, V_FLOAT, H_ANGLE16,
                                      NODE_VALUE_FLOAT_dup(angles[2]),0));
                  }
                  if (mask2 & 0x04) {
                    ttn=node_link(ttn,node_command_init(TOKEN_SPEED_X, V_INT, H_BYTE,
                                      NODE_VALUE_INT_dup(speed[0]),0));
                  }
                  if (mask2 & 0x08) {
                    ttn=node_link(ttn,node_command_init(TOKEN_SPEED_Y, V_INT, H_BYTE,
                                      NODE_VALUE_INT_dup(speed[1]),0));
                  }
                  if (mask2 & 0x10) {
                    ttn=node_link(ttn,node_command_init(TOKEN_SPEED_Z, V_INT, H_BYTE,
                                      NODE_VALUE_INT_dup(speed[2]),0));
                  }
                  if (mask2 & 0x20) {
                    if (flag & 0x01) {
                      ttn=node_link(ttn, node_init_all(TOKEN_FIRE, H_SIMPLE, NULL, 0));
                    }
                    if (flag & 0x02) {
                      ttn=node_link(ttn, node_init_all(TOKEN_JUMP, H_SIMPLE, NULL, 0));
                    }
                    { 
                      node* tttn;
                    
                      for (i=2,tttn=NULL;i<8;i++) {
                        if ((flag>>i) & 0x01) {
                          tttn=node_link(tttn,node_init_all(V_INT, H_SIMPLE, 
                                                      NODE_VALUE_INT_dup(i), 0));
                        }
                      }
                      if (tttn!=NULL) {
                        ttn=node_link(ttn,node_init_all(TOKEN_FLAG,H_SIMPLE,tttn,0));
                      }
                    }
                  }
                  if (mask2 & 0x40) {
                    ttn=node_link(ttn,
                      node_command_init(TOKEN_IMPULSE, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(impulse),0));
                  }
                  if (mask2 & 0x80) {
                    ttn=node_link(ttn, 
                      node_command_init(TOKEN_LOAD, V_INT, H_BYTE, 
                                        NODE_VALUE_INT_dup(load), 0));
                  }
                }
                /* sub command */
                /* tn=node_link(tn,node_init_all(TOKEN_PLAYER, 0, ttn, 0)); */
                /* everything as commands */
                tn=node_link(tn,ttn); 
              }
              /* bit 2 */
              if (mask & 0x0004) {
                tn=node_link(tn,node_command_init(TOKEN_CSPEED_X, V_FLOAT, H_COORD, 
                                                  NODE_VALUE_FLOAT_dup(cspeed[0]), 0));
              }
              /* bit 3 */
              if (mask & 0x0008) {
                tn=node_link(tn,node_command_init(TOKEN_CSPEED_Y, V_FLOAT, H_COORD, 
                                                  NODE_VALUE_FLOAT_dup(cspeed[1]), 0));
              }
              /* bit 4 */
              if (mask & 0x0010) {
                tn=node_link(tn,node_command_init(TOKEN_CSPEED_Z, V_FLOAT, H_COORD, 
                                                  NODE_VALUE_FLOAT_dup(cspeed[2]), 0));
              }
              /* bit 5 */
              if (mask & 0x0020) {
                tn=node_link(tn,node_command_init(TOKEN_MODELINDEX, V_INT, H_BYTE, 
                                                  NODE_VALUE_INT_dup(modelindex), 0));
              }
              /* bit 6 */
              if (mask & 0x0040) {
                tn=node_link(tn,node_command_init(TOKEN_UK_BYTE6, V_INT, H_BYTE, 
                                                  NODE_VALUE_INT_dup(uk_byte6), 0));
              }
              /* bit 7 */
              if (mask & 0x0080) {
                if (weapon) {
                  for (i=2, j=weapon, ttn=NULL; j; j>>=1, i++) {
                    if (j&0x01) ttn=node_link(ttn,node_init_all(V_INT,H_BYTE,
                                                  NODE_VALUE_INT_dup(i),0));
                  }
                  tn=node_link(tn,node_init_all(TOKEN_WEAPON,H_SIMPLE,ttn,0));
                }
                else {
                  tn=node_link(tn, node_command_init(TOKEN_WEAPON,V_INT,H_BYTE,
                                                     NODE_VALUE_INT_dup(1),0));  
                }                                    
              }
              /* bit 8 */
              if (mask & 0x0100) {
                tn=node_link(tn,node_command_init(TOKEN_WEAPONFRAME, V_INT, H_BYTE,
                                                  NODE_VALUE_INT_dup(weaponframe), 0));
              }
              n=node_link(n,node_init_all(TOKEN_PLAYERINFO, H_QWD_PLAYERINFO, tn, 0));
            }
            break;
            case 0x2B: { /* nails */
              /* variables */
              long count;
              svector origin;
              float angle_1;
              float angle_2;
              
              int i, j;
              unsigned char b[5];
              
              /* binary in and construct node tree */
              count = ReadByte(m);
              tn = NULL;
              for (j=0;j<count;j++) {
                ttn = NULL;
                for (i=0;i<5;i++) b[i] = ReadByte(m);
                origin[0] = (b[0] & 0xFF) | ((b[1] & 0x0F) << 8);
                origin[1] = ((b[1] & 0xF0) >> 4) | (b[2] << 4);
                origin[2] = (b[3] & 0xFF) | ((b[4] & 0x0F) << 8);
                for (i=0;i<3;i++) origin[i] = (origin[i] - 2048) * 2;
                angle_1 = (b[4] & 0xF0) >> 4;
                if (angle_1>=8) angle_1 = angle_1 - 16;
                angle_1 *= 180.0 / 8.0;
                angle_2 = ReadAngle(m);
                ttn = node_link(ttn, node_triple_command_init(TOKEN_ORIGIN, V_INT, H_SHORT,
                                     NODE_VALUE_INT_dup(origin[0]),
                                     NODE_VALUE_INT_dup(origin[1]),
                                     NODE_VALUE_INT_dup(origin[2]),0));
                ttn = node_link(ttn, node_command_init(TOKEN_ANGLES_1, V_FLOAT, H_FLOAT, 
                                     NODE_VALUE_FLOAT_dup(angle_1),0));
                ttn = node_link(ttn, node_command_init(TOKEN_ANGLES_2, V_FLOAT, H_ANGLE, 
                                     NODE_VALUE_FLOAT_dup(angle_2),0));
                tn = node_link(tn, node_init_all(TOKEN_NAIL, 0 , ttn, 0));
              }
              n = node_link(n,node_init_all(TOKEN_NAILS, H_QWD_NAILS, tn, 0)); 
            }
            break;
            case 0x2C: { /* choke */
              /* variables */
              long choke;
              
              /* binaray in */
              choke = ReadByte(m);
              
              /* construct node tree */
              n=node_link(n,
                  node_add_hint(
                    node_command_init(TOKEN_CHOKE,V_INT, H_BYTE, 
                                      NODE_VALUE_INT_dup(choke), 0),
                    H_SIMPLE)
                ); 
            }
            break;
            case 0x2D: { /* modellist */
              char text[MAX_STRING_SIZE];
              long m_index;
              long m_last = 0;
              long m_next;
              int i;

              if (SERVERVERSION >= 26) {
                /* binary in */
                m_index = ReadByte(m);
                for ( i=m_index ; i<QWD_MAX_MODELS ; i++ ) {
                  ReadString(m, text);
                  if (strlen(text) == 0) break;
                  if (QWDTOP->precache_models[i+1] != NULL) {
                    free(QWDTOP->precache_models[i+1]);
                  }
                  QWDTOP->precache_models[i+1] = strdup(text);
                  if ((i+1)>QWDTOP->nummodels) QWDTOP->nummodels = (i+1);
                  m_last = (i+1);
                }
                m_next = ReadByte(m);

                /* construct node tree */
                tn = node_command_init(TOKEN_FIRST,V_INT,H_BYTE,
                     NODE_VALUE_INT_dup(m_index),0);
                for (i=m_index, ttn=NULL; (i+1)<=m_last ; i++) {
                  ttn=node_link(ttn,
                       node_init_all(V_STRING,H_STRINGLIST,
                         NODE_VALUE_STRING_dup(QWDTOP->precache_models[i+1]),0)
                     );
                }
                ttn = node_init_all(TOKEN_MODELS, 0, ttn, 0);
                tn = node_link(tn,ttn);
                tn = node_link(tn,node_command_init(TOKEN_NEXT,V_INT,H_BYTE,
                     NODE_VALUE_INT_dup(m_next),0));
                n=node_link(n,node_init_all(TOKEN_MODELLIST, H_SIMPLE, tn, 0)); 
              }
              else {
                /* free the old strings (multiple level recordings) */
                for (i=1;i<=QWDTOP->nummodels;i++) free(QWDTOP->precache_models[i]); 
                QWDTOP->nummodels = 0;

                /* binary in */
                do {
                  ReadString(m, text);
                  if (strlen(text)==0) break;
                  if (QWDTOP->nummodels<QWD_MAX_MODELS)
                    QWDTOP->nummodels++;
                  else
                    syserror(WQWD, QWDTOP->filename);
                  QWDTOP->precache_models[QWDTOP->nummodels] = strdup(text);
                  if (QWDTOP->precache_models[QWDTOP->nummodels]==NULL) 
                    syserror(errno, "strdup");
                  if (strcmp(text,"progs/player.mdl")==0) {
                    QWDTOP->playermodel=QWDTOP->nummodels;
                  }
                } while (*text);
                
                /* construct node tree */
                for (i=1,tn=NULL; i<=QWDTOP->nummodels; i++) {
                  tn=node_link(tn,
                       node_init_all(V_STRING, H_STRINGLIST,
                         NODE_VALUE_STRING_dup(QWDTOP->precache_models[i]),0)
                     );
                }
                n=node_link(n,node_init_all(TOKEN_MODELLIST, H_SIMPLE, tn, 0)); 
              }
            }
            break;
            case 0x2E: { /* soundlist */
              char text[MAX_STRING_SIZE];
              long s_index;
              long s_last = 0;
              long s_next;
              int i;

              if (SERVERVERSION >= 26) {
                /* binary in */
                s_index = ReadByte(m);
                for ( i=s_index ; i<QWD_MAX_SOUNDS ; i++ ) {
                  ReadString(m, text);
                  if (strlen(text) == 0) break;
                  if (QWDTOP->precache_sounds[i+1] != NULL)
                    free(QWDTOP->precache_sounds[i+1]);
                  QWDTOP->precache_sounds[i+1] = strdup(text);
                  if ((i+1)>QWDTOP->numsounds) QWDTOP->numsounds = (i+1);
                  s_last = (i+1);
                }
                s_next = ReadByte(m);
                
                /* construct node tree */
                tn = node_command_init(TOKEN_FIRST,V_INT,H_BYTE,
                     NODE_VALUE_INT_dup(s_index),0);
                for (i=s_index, ttn=NULL; (i+1)<=s_last ; i++) {
                  ttn=node_link(ttn,
                       node_init_all(V_STRING,H_STRINGLIST,
                         NODE_VALUE_STRING_dup(QWDTOP->precache_sounds[i+1]),0)
                     );
                }
                ttn = node_init_all(TOKEN_SOUNDS, 0, ttn, 0);
                tn = node_link(tn,ttn);
                tn = node_link(tn,node_command_init(TOKEN_NEXT,V_INT,H_BYTE,
                                  NODE_VALUE_INT_dup(s_next),0));
                n=node_link(n,node_init_all(TOKEN_SOUNDLIST, H_SIMPLE, tn, 0)); 
              }
              else {
                /* free the old strings (multiple level recordings) */
                for (i=1;i<=QWDTOP->numsounds;i++) free(QWDTOP->precache_sounds[i]); 
                QWDTOP->numsounds = 0;

                /* binary in */
                do {
                  ReadString(m, text);
                  if (strlen(text)==0) break;
                  if (QWDTOP->numsounds<QWD_MAX_SOUNDS)
                    QWDTOP->numsounds++;
                  else
                    syserror(WQWD, QWDTOP->filename);
                  QWDTOP->precache_sounds[QWDTOP->numsounds] = strdup(text);
                  if (QWDTOP->precache_sounds[QWDTOP->numsounds]==NULL) 
                    syserror(errno, "strdup");
                } while (*text);
                
                /* construct node tree */
                for (i=1,tn=NULL; i<=QWDTOP->numsounds; i++) {
                  tn=node_link(tn,
                       node_init_all(V_STRING,H_STRINGLIST,
                         NODE_VALUE_STRING_dup(QWDTOP->precache_sounds[i]),0)
                     );
                }
                n=node_link(n,node_init_all(TOKEN_SOUNDLIST, H_SIMPLE, tn, 0)); 
              }
            }
            break;
            case 0x2F: { /* packetentities */
              /* variables */
              long entity;
              long remove;
              long mask;
              long modelindex;
              long frame;
              long colormap;
              long skin;
              long effects;
              vec3_t origin;
              vec3_t angles;
                
              node *q1;
              /*
              unsigned char *pold, *pnew;
              */
              
              /* binary in and node tree */              
              tn = NULL;
              
              /* pold=m->p; */ 
              while ((mask=ReadShort(m)) != 0x0000) {
                entity = mask & 0x01FF;
                mask &= 0xFE00;
                remove = (mask & 0x4000) ? 1 : 0;
                if (mask & 0x8000) mask |= ReadByte(m);
                modelindex = mask & 0x0004 ? ReadByte(m) : 0;
                frame = mask & 0x2000 ? ReadByte(m) : 0;
                colormap = mask & 0x0008 ? ReadByte(m) : 0;
                skin = mask & 0x0010 ? ReadByte(m) : 0;
                effects = mask & 0x0020 ? ReadByte(m) : 0;
                origin[0] = mask & 0x0200 ? ReadCoord(m) : 0;
                angles[0] = mask & 0x0001 ? ReadAngle(m) : 0;
                origin[1] = mask & 0x0400 ? ReadCoord(m) : 0;
                angles[1] = mask & 0x1000 ? ReadAngle(m) : 0;
                origin[2] = mask & 0x0800 ? ReadCoord(m) : 0;
                angles[2] = mask & 0x0002 ? ReadAngle(m) : 0;

                ttn=NULL;
                
                ttn=node_link(ttn,node_command_init(TOKEN_ENTITY,V_INT,H_BYTE,
                                       NODE_VALUE_INT_dup(entity),0));
                if (mask & 0x4000) {
                  ttn=node_link(ttn,node_init_all(TOKEN_REMOVE, H_SIMPLE, NULL, 0));
                }
                if (mask & 0x0004) {
                  q1=node_command_init(TOKEN_MODELINDEX,V_INT,H_BYTE,NODE_VALUE_INT_dup(modelindex),0);
                  if (modelindex>=1 && modelindex<=QWDTOP->nummodels &&
                      QWDTOP->precache_models[modelindex][0]!='*') {
                    node_add_comment(q1,
                    NODE_VALUE_STRING_dup(QWDTOP->precache_models[modelindex]));
                  }
                  ttn=node_link(ttn,q1);
                }
                if (mask & 0x2000) {
                  ttn=node_link(ttn,node_command_init(TOKEN_FRAME,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(frame),0));
                }
                if (mask & 0x0008) {
                  ttn=node_link(ttn,node_command_init(TOKEN_COLORMAP,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(colormap),0));
                }
                if (mask & 0x0010) {
                  ttn=node_link(ttn,node_command_init(TOKEN_SKIN,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(skin),0));
                }
                if (mask & 0x0020) {
                  ttn=node_link(ttn,node_command_init(TOKEN_EFFECTS,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(effects),0));
                }
                if (mask&0x0200) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ORIGIN_X,V_FLOAT,H_COORD,
                                    NODE_VALUE_FLOAT_dup(origin[0]),0));
                }
                if (mask&0x0400) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ORIGIN_Y,V_FLOAT,H_COORD,
                                    NODE_VALUE_FLOAT_dup(origin[1]),0));
                }
                if (mask&0x0800) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ORIGIN_Z,V_FLOAT,H_COORD,
                                    NODE_VALUE_FLOAT_dup(origin[2]),0));
                }
                if (mask&0x0001) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_1,V_FLOAT,H_ANGLE,
                                    NODE_VALUE_FLOAT_dup(angles[0]),0));
                }
                if (mask&0x1000) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_2,V_FLOAT,H_ANGLE,
                                    NODE_VALUE_FLOAT_dup(angles[1]),0));
                }
                if (mask&0x0002) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_3,V_FLOAT,H_ANGLE,
                                    NODE_VALUE_FLOAT_dup(angles[2]),0));
                }
                if (ttn!=NULL)
                  tn = node_link(tn,node_init_all(TOKEN_PACKETENTITY, 0, ttn, 0));
              }
              /* finish node tree */
              n = node_link(n,node_init_all(TOKEN_PACKETENTITIES, H_QWD_PACKETENTITIES, tn, 0));
            }
            break;
            case 0x30: { /* deltapacketentities */
              /* variables */
              long index;
              long entity;
              long mask;
              long remove;
              long modelindex;
              long frame;
              long colormap;
              long skin;
              long effects;
              vec3_t origin;
              vec3_t angles;
                
              node *q1;
              /*
              unsigned char *pold, *pnew;
              */

              /* binary in and node tree */              
              tn = NULL;
              
              /* pold=m->p; */
              index = ReadByte(m);
              tn = node_link(tn, node_command_init(TOKEN_FRAME, V_INT, H_BYTE, 
                                 NODE_VALUE_INT_dup(index),0));
              while ((mask=ReadShort(m)) != 0x0000) {
                entity = mask & 0x01FF;
                mask &= 0xFE00;
                remove = (mask & 0x4000) ? 1 : 0;
                if (mask & 0x8000) mask |= ReadByte(m);
                modelindex = mask & 0x0004 ? ReadByte(m) : 0;
                frame = mask & 0x2000 ? ReadByte(m) : 0;
                colormap = mask & 0x0008 ? ReadByte(m) : 0;
                skin = mask & 0x0010 ? ReadByte(m) : 0;
                effects = mask & 0x0020 ? ReadByte(m) : 0;
                origin[0] = mask & 0x0200 ? ReadCoord(m) : 0;
                angles[0] = mask & 0x0001 ? ReadAngle(m) : 0;
                origin[1] = mask & 0x0400 ? ReadCoord(m) : 0;
                angles[1] = mask & 0x1000 ? ReadAngle(m) : 0;
                origin[2] = mask & 0x0800 ? ReadCoord(m) : 0;
                angles[2] = mask & 0x0002 ? ReadAngle(m) : 0;

                ttn=NULL;
                
                ttn=node_link(ttn,node_command_init(TOKEN_ENTITY,V_INT,H_BYTE,
                                       NODE_VALUE_INT_dup(entity),0));
                if (mask & 0x4000) {
                  ttn=node_link(ttn,node_init_all(TOKEN_REMOVE, H_SIMPLE, NULL, 0));
                }
                if (mask & 0x0004) {
                  q1=node_command_init(TOKEN_MODELINDEX,V_INT,H_BYTE,NODE_VALUE_INT_dup(modelindex),0);
                  if (modelindex>=1 && modelindex<=QWDTOP->nummodels &&
                      QWDTOP->precache_models[modelindex][0]!='*') {
                    node_add_comment(q1,
                    NODE_VALUE_STRING_dup(QWDTOP->precache_models[modelindex]));
                  }
                  ttn=node_link(ttn,q1);
                }
                if (mask & 0x2000) {
                  ttn=node_link(ttn,node_command_init(TOKEN_FRAME,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(frame),0));
                }
                if (mask & 0x0008) {
                  ttn=node_link(ttn,node_command_init(TOKEN_COLORMAP,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(colormap),0));
                }
                if (mask & 0x0010) {
                  ttn=node_link(ttn,node_command_init(TOKEN_SKIN,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(skin),0));
                }
                if (mask & 0x0020) {
                  ttn=node_link(ttn,node_command_init(TOKEN_EFFECTS,V_INT,H_BYTE,
                                    NODE_VALUE_INT_dup(effects),0));
                }
                if (mask&0x0200) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ORIGIN_X,V_FLOAT,H_COORD,
                                    NODE_VALUE_FLOAT_dup(origin[0]),0));
                }
                if (mask&0x0400) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ORIGIN_Y,V_FLOAT,H_COORD,
                                    NODE_VALUE_FLOAT_dup(origin[1]),0));
                }
                if (mask&0x0800) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ORIGIN_Z,V_FLOAT,H_COORD,
                                    NODE_VALUE_FLOAT_dup(origin[2]),0));
                }
                if (mask&0x0001) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_1,V_FLOAT,H_ANGLE,
                                    NODE_VALUE_FLOAT_dup(angles[0]),0));
                }
                if (mask&0x1000) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_2,V_FLOAT,H_ANGLE,
                                    NODE_VALUE_FLOAT_dup(angles[1]),0));
                }
                if (mask&0x0002) { 
                  ttn=node_link(ttn,node_command_init(TOKEN_ANGLES_3,V_FLOAT,H_ANGLE,
                                    NODE_VALUE_FLOAT_dup(angles[2]),0));
                }
                if (ttn!=NULL)
                  tn = node_link(tn,node_init_all(TOKEN_DELTAPACKETENTITY, 0, ttn, 0));
              }
              /* finish node tree */
              n = node_link(n,node_init_all(TOKEN_DELTAPACKETENTITIES, H_QWD_DELTAPACKETENTITIES, tn, 0));
            }
            break;
            case 0x31: { /* maxspeed */
              float maxspeed;

              maxspeed = ReadFloat(m);

              n=node_link(n,
                  node_add_hint(
                    node_command_init(TOKEN_MAXSPEED,
                                      V_FLOAT,
                                      H_FLOAT,
                                      NODE_VALUE_FLOAT_dup(maxspeed),
                                      0),
                    H_SIMPLE)
                );
            }
            break;
            case 0x32: { /* entgravity */
              float entgravity;

              entgravity = ReadFloat(m);

              n=node_link(n,
                  node_add_hint(
                    node_command_init(TOKEN_ENTGRAVITY,
                                      V_FLOAT,
                                      H_FLOAT,
                                      NODE_VALUE_FLOAT_dup(entgravity),
                                      0),
                    H_SIMPLE)
                );
            }
            break;
            case 0x33: { /* setinfo */
              long player;
              char name[MAX_STRING_SIZE];
              char string[MAX_STRING_SIZE];
              
              player = ReadByte(m);
              ReadString(m,name);
              ReadString(m,string);

              tn = NULL;
              tn = node_link(tn, node_command_init(TOKEN_PLAYER,V_INT,H_BYTE,
                                 NODE_VALUE_INT_dup(player),0));
              tn = node_link(tn, node_command_init(TOKEN_NAME,V_STRING,H_STRING,
                                 NODE_VALUE_STRING_dup(name),0));
              tn = node_link(tn, node_command_init(TOKEN_STRING,V_STRING,H_STRING,
                                 NODE_VALUE_STRING_dup(string),0));
              n = node_link(n, node_init_all(TOKEN_SETINFO,H_SIMPLE,tn,0));
            }
            break;
            case 0x34: { /* serverinfo */
              char name[MAX_STRING_SIZE];
              char string[MAX_STRING_SIZE];
              
              ReadString(m,name);
              ReadString(m,string);

              n=node_link(n,
                  node_add_hint(
                    node_init_all(TOKEN_SERVERINFO, H_SIMPLE, 
                      node_add_next(
                        node_command_init(TOKEN_NAME, V_STRING, H_STRING, 
                                          NODE_VALUE_STRING_dup(name),0),
                        node_command_init(TOKEN_STRING, V_STRING, H_STRING, 
                                          NODE_VALUE_STRING_dup(string),0)
                      ), 
                      0),
                  H_SIMPLE)
                );
            }
            break;
            case 0x35: { /* updatepl */
              long player;
              long loss;

              player = ReadByte(m);
              loss = ReadByte(m);

              tn = NULL;
              tn = node_link(tn, node_command_init(TOKEN_PLAYER,V_INT,H_BYTE,
                                 NODE_VALUE_INT_dup(player),0));
              tn = node_link(tn, node_command_init(TOKEN_LOSS,V_INT,H_BYTE,
                                 NODE_VALUE_INT_dup(loss),0));
              n = node_link(n, node_init_all(TOKEN_UPDATEPL,H_SIMPLE,tn,0));
            }
            break;
            default: {
              tn=node_command_init(TOKEN_ID,V_BYTEHEX,H_SIMPLE,
                                   NODE_VALUE_BYTEHEX_dup(id),0);
              for (ttn=NULL; m->p < m->end;) {
                ttn=node_link(ttn,node_init_all(V_BYTEHEX,H_SIMPLE,
                                                NODE_VALUE_BYTEHEX_dup((unsigned char)ReadByte(m)),0));
              }
              if (ttn!=NULL)
                tn=node_link(tn,node_init_all(TOKEN_DATA,H_SIMPLE,ttn,0));

              n=node_link(n,
                  node_add_comment(
                    node_init_all(TOKEN_UNKNOWN,H_UNKNOWN,tn,0),
                    NODE_VALUE_STRING_dup("something is wrong")
                  )
                );
            }
            break;
          } /* end switch(id) */
        } /* end while (m->p) */
      } /* end game block */
      n = node_init_all(TOKEN_SBLOCK,0,n,0);
      if (opt & 0x01) {
        sprintf(ts,"%i",QWDTOP->frame);
        node_add_comment(n,NODE_VALUE_STRING_dup(ts));
      }
    }
    break;
    case 2: { /* fblock */
      /* variables */
      long seq1;
      long seq2;
      
      /* binary in */
      seq1 = ReadLong(m);
      seq2 = ReadLong(m);

      /* contruct node tree */
      node_link(n,
        node_command_init(TOKEN_SEQ1,V_INT,H_LONG,NODE_VALUE_INT_dup(seq1),0));
      node_link(n,
        node_command_init(TOKEN_SEQ2,V_INT,H_LONG,NODE_VALUE_INT_dup(seq2),0));
      n = node_init_all(TOKEN_FBLOCK,0,n,0);
      if (opt & 0x01) {
        sprintf(ts,"%i",QWDTOP->frame);
        node_add_comment(n,NODE_VALUE_STRING_dup(ts));
      }       
    }
    break;
    default:
      syserror(WQWD, QWDTOP->filename);
  }
  return n;
}



/******************************************************************************/
/* block output: text version *************************************************/
/******************************************************************************/

node* QWD_block_write_text(node* b)
{
  /* b may be NULL (something was wrong or removed) */
  if (b == NULL) return b;
  /* type is TOKEN_CBLOCK or TOKEN_SBLOCK or TOKEN_FBLOCK, 
     down is first message */
  if ( ( b->type == TOKEN_CBLOCK ||
           b->type == TOKEN_SBLOCK ||
           b->type == TOKEN_FBLOCK ) &&
         b->down!=NULL ) {
    node_write_text(b,0);
  }
  return b;
}

/******************************************************************************/
/* block output: binary version ***********************************************/
/******************************************************************************/

node* QWD_block_write_bin(node* b)
{
  BB_t m;
  node *tn, *ttn;
  unsigned long datasize = 0;

  /* b may be NULL (something was wrong or removed) */
  if (b == NULL) return b;

  /* we start from the beginning */
  m.p = m.buffer;

  /* fprintf(stderr,"block out\n"); */ 
  tn = b->down;
  m.p = m.buffer;
  if (tn->type != TOKEN_TIME) syserror(QWDINTE,"no time token");
  do_simple_argument_write_bin(tn->down,&m); tn = tn->next;
  switch (b->type) {
    case TOKEN_CBLOCK: {
      long flag;
      
      /* 0 == client block */
      WriteByte(&m,0);
      /* collect some info */
      flag = 0;
      for (ttn=tn ; ttn != NULL ; ttn=ttn->next) {
        switch (ttn->type) {
          case TOKEN_FIRE: flag |= 0x01; break;
          case TOKEN_JUMP: flag |= 0x02; break;
          case TOKEN_FLAG: {
            node *tttn;
            for (tttn = ttn->down ; tttn != NULL ; tttn = tttn->next) {
              long flagbit = *(long*)tttn->down;
              if ((flagbit >=0) && (flagbit < 8)) 
                flag |= 0x01 << flagbit;
              else
                syserror(QWDINTE,"wrong flag bit value");
            } /* end for tttn */          
          } /* end case TOKEN_FLAG */
          break;
        } /* end switch ttn->type */
      } /* end for ttn */
      /* now write it out */
      if (tn->type != TOKEN_LOAD) syserror(QWDINTE,"no load token");
      do_simple_argument_write_bin(tn->down,&m); tn = tn->next;
      if (tn->type != TOKEN_ANGLES) syserror(QWDINTE, "no angles token");
      do_simple_arguments_write_bin(tn->down,&m); tn = tn->next;
      if (tn->type != TOKEN_SPEED) syserror(QWDINTE, "no speed token");
      do_simple_arguments_write_bin(tn->down,&m); tn = tn->next;
      if (tn->type == TOKEN_FIRE) tn = tn->next;
      if (tn->type == TOKEN_JUMP) tn = tn->next;
      if (tn->type == TOKEN_FLAG) tn = tn->next;
      WriteByte(&m,flag);
      if (tn->type == TOKEN_IMPULSE) {
        do_simple_argument_write_bin(tn->down,&m); tn = tn->next ;
      }
      else 
        WriteByte(&m,0);
      if (tn->type != TOKEN_UK_ANGLES) syserror(QWDINTE,"no uk_angles token");
      do_simple_arguments_write_bin(tn->down,&m); tn = tn->next;
      if (tn != NULL) syserror(QWDINTE,"too many tokens in cblock");
      /* calculate the size info */
      datasize = m.p - m.buffer; 
    } /* end TOKEN_CBLOCK */
    break;
    case TOKEN_SBLOCK: {
      /* 1 == server block */
      WriteByte(&m,1);
      WriteLong(&m,0); /* here comes at the end the blocksize entry */
      
      /* 2 cases: connectionless or game block */
      if (tn->type == TOKEN_CONNLESS) {
        /* there is nothing after the connless message */
        if (tn->next != NULL) syserror(QWDINTE,"too many connectionless frames");
        /* write the special sequence number */
        WriteLong(&m,0xFFFFFFFF);
        /* go down to the specific message */
        tn = tn->down;
        /* there is nothing after a specific connless message */
        if (tn->next != NULL) syserror(QWDINTE,"too many connectionless messages");
        switch(tn->type) {
          case TOKEN_UNKNOWN:
            do_unknown_message_write_bin(tn,&m);
          break;
          case TOKEN_DISCONNECT:
            WriteByte(&m,2);
            do_simple_argument_write_bin(tn->down,&m);
          break;
          case TOKEN_CLIENT_COMMAND:
            WriteByte(&m,0x42); /* 'B' */
            do_simple_argument_write_bin(tn->down,&m);
          break;
          case TOKEN_CHALLENGE:
            WriteByte(&m,0x63); /* 'c' */
            do_simple_argument_write_bin(tn->down,&m);
          break;
          case TOKEN_CONNECT:
            WriteByte(&m,0x6a); /* 'j' */
          break;
          case TOKEN_PING:
            WriteByte(&m,0x6b); /* 'k' */
          break;
          case TOKEN_CONSOLE:
            WriteByte(&m,0x6e); /* 'n' */
            do_simple_argument_write_bin(tn->down,&m);
          break;
          default:
            syserror(QWDINTE,"wrong connectionless message type");
        } /* end switch tn->type */
      } /* end TOKEN_CONNLESS */
      else { /* server game block */
        long seq1;
        long seq2;

        /* first the sequence numbers */
        cmdcheck(tn,TOKEN_SEQ1,"seq1","sblock");
        seq1 = *(long*)tn->down->down; NEXT(tn);
        cmdcheck(tn,TOKEN_SEQ2,"seq2","sblock");
        seq2 = *(long*)tn->down->down; NEXT(tn);
        /* fprintf(stderr,"aseq type=%d hint=%d\n",tn->type, tn->hint); */
        if (tn != NULL && tn->type==TOKEN_RELIABLE) {
          for (ttn=tn->down ; ttn != NULL ; ttn=ttn->next) {
            long rel = *(long*)ttn->down;
            switch (rel) {
              case 1: seq1 |= 0x80000000L; break;
              case 2: seq2 |= 0x80000000L; break;
              default: syserror(QWDINTE,"wrong reliable value %d",rel);
            } /* end switch rel */
          } /* end for ttn */
          NEXT(tn);
        } /* end tn != NULL && reliable */
        WriteLong(&m,seq1);
        WriteLong(&m,seq2);
        /* end of sequence numbers in the server game block */

        /* now the game messages */
        for ( ; tn != NULL ; tn = tn->next ) {
          /* fprintf(stderr,"m %d, h %d\n", tn->type, tn->hint); */ 
          switch (tn->hint) {
            case H_NOTHING:
              /* good for comment inclusion */
            break;
            case H_SIMPLE:
              do_simple_message_write_bin(tn,&m);
            break;
            case H_UNKNOWN:
              do_unknown_message_write_bin(tn,&m);
            break;
            /* here comes all the special QWD game messages */
            case H_QWD_SOUND:
              do_qwd_sound_message_write_bin(tn,&m);
            break;
            case H_QWD_STOPSOUND:
              do_qwd_stopsound_message_write_bin(tn,&m);
            break;
            case H_QWD_SPAWNSTATIC:
              do_qwd_spawnstatic_message_write_bin(tn,&m);
            break;
            case H_QWD_SPAWNBASELINE:
              do_qwd_spawnbaseline_message_write_bin(tn,&m);
            break;
            case H_QWD_PLAYERINFO:
              do_qwd_playerinfo_message_write_bin(tn,&m);
            break;
            case H_QWD_NAILS:
              do_qwd_nails_message_write_bin(tn,&m);
            break;
            case H_QWD_PACKETENTITIES:
              do_qwd_packetentities_message_write_bin(tn,&m);
            break;
            case H_QWD_DELTAPACKETENTITIES:
              do_qwd_deltapacketentities_message_write_bin(tn,&m);
            break;
            /* end of special QWD game messages */
            default:
              syserror(QWDINTE, "wrong server game message hint %d", tn->hint);
          } /* end switch tn->hint */        
        } /* end for tn */

        /* end of game messages in the server game block */
      } /* end server game block */
      /* calculate the size info */
      datasize = m.p - m.buffer;
      /* write the size info at 5 = sizeof(float) + sizeof(char) */
      m.p = m.buffer + 5;
      /* 9 = sizeof(float) + sizeof(char)+ sizeof(long) */ 
      WriteLong(&m, datasize - 9);
    } /* end case SBLOCK */
    break;
    case TOKEN_FBLOCK: {
      long seq1;
      long seq2;
      
      WriteByte(&m,2);
      if (tn->type != TOKEN_SEQ1) syserror(QWDINTE,"no seq1 token");
      seq1 = *(long*)tn->down->down; tn = tn->next;
      if (tn->type != TOKEN_SEQ2) syserror(QWDINTE,"no seq2 token");
      seq2 = *(long*)tn->down->down; tn = tn->next;
      if (tn != NULL) {
        if (tn->type != TOKEN_RELIABLE) syserror(QWDINTE,"no reliable token");
        for (ttn=tn->down ; ttn != NULL ; ttn=ttn->next) {
          long rel = *(long*)ttn->down;
          switch (rel) {
            case 1: seq1 |= 0x80000000L; break;
            case 2: seq2 |= 0x80000000L; break;
            default: syserror(QWDINTE,"wrong reliable value");
          } /* end switch rel */
        } /* end for ttn */
      } /* end tn != NULL */
      WriteLong(&m,seq1);
      WriteLong(&m,seq2);
      /* calculate the size info */
      datasize = m.p - m.buffer; 
    } /* end TOKEN_FBLOCK */
    break;
    default: 
      syserror(QWDINTE,"bad block type %d", b->type);
    break;
  } /* end switch b->type */
  
  if (datasize) { /* if there was something in the block */
    if (fwrite(m.buffer, 1, datasize, output_file) != datasize) 
      syserror(FIWRITE, output_filename);
    if (fflush(output_file) != 0)
      syserror(errno, output_filename);
  }

  return b;
}




void do_qwd_sound_message_write_bin(node* n, BB_t* m)
{
  long entity_channel;
  node* c;
    
  /*
      I don't do any kind of check in here. If the internal structure is
      corrupt this gives a total crash.
          
      sound: combining of two values, bit mask
   */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* collect some combining info */
  entity_channel = 0;
  for ( c = n->down ; c != NULL ; c = c->next ) {
    switch (c->type) {
      case TOKEN_CHANNEL:
        entity_channel |= 0x07 & (*(long*)c->down->down);
      break;
      case TOKEN_ENTITY:
        entity_channel |= (0x3ff & (*(long*)c->down->down)) << 3;
      break;
      case TOKEN_VOL:
        entity_channel |= 0x8000;
      break;
      case TOKEN_ATTENUATION:
        entity_channel |= 0x4000;
      break;
      case TOKEN_SOUNDNUM:
      case TOKEN_ORIGIN:
        /* do nothing */
      break;
      default:
        syserror(QWDINTE,"unknown sound command value %d", c->type);
      break;
    } /* end switch c->type */
  } /* end for c */
  
  /* now write it out */
  /* message -> commands */
  c = n->down;
  
  WriteShort(m,entity_channel);
  /* c is entity */
  NEXT(c);
  /* c is channel */
  NEXT(c);
  /* c is vol or attenuation or soundnum */
  if (c->type == TOKEN_VOL) {
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  /* c is attenuation or soundnum */
  if (c->type == TOKEN_ATTENUATION) {
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  /* c is soundnum */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is origin */
  do_simple_arguments_write_bin(c->down,m); NEXT(c);
  /* c is emtpy */
  if (c != NULL) syserror(QWDINTE,"additional command %d in sound", c->type);
}


void do_qwd_stopsound_message_write_bin(node* n, BB_t* m)
{
  long entity_channel;
  node* c;
    
  /*
      I don't do any kind of check in here. If the internal structure is
      corrupt this gives a total crash.
          
      sound: combining of two values, bit mask
   */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* collect some combining info */
  entity_channel = 0;
  /* message -> commands */
  c = n->down;
  /* c is channel */
  entity_channel |= 0x7 & (*(long*)c->down->down);
  NEXT(c);
  /* c is entity */
  entity_channel |= (0x1fff & (*(long*)c->down->down)) << 3;
  NEXT(c);

  /* write it out */
  WriteShort(m,entity_channel);

  /* c is emtpy */
  if (c != NULL) syserror(QWDINTE,"additional command %d in stopsound", c->type);
}


void do_qwd_spawnstatic_message_write_bin(node* n, BB_t* m)
{
  node* c;
  node* c2;
  node* a;
  node* a2;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    spawnstatic is very easy: all commands have to be there
  */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* message -> commands */
  c = n->down;
  /* c is default_modelindex */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_frame */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_colormap */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_skin */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_origin */
  c2 = c->next;
  /* c2 is default_angles */

  /* default_origin and default_angles are combined */
  a=c->down;
  a2=c2->down;

  do_simple_argument_write_bin(a, m); NEXT(a);
  do_simple_argument_write_bin(a2,m); NEXT(a2);
  do_simple_argument_write_bin(a, m); NEXT(a);
  do_simple_argument_write_bin(a2,m); NEXT(a2);
  do_simple_argument_write_bin(a, m);
  do_simple_argument_write_bin(a2,m);

  NEXT(c2);
  /* 2c is emtpy */
  if (c2 != NULL) syserror(QWDINTE,"additional command %d in spawnstatic", c2->type);
}


void do_qwd_spawnbaseline_message_write_bin(node* n, BB_t* m)
{
  node* c;
  node* c2;
  node* a;
  node* a2;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    spawnbaseline is very easy: all commands have to be there
  */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* message -> commands */
  c = n->down;
  /* c is entity */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_modelindex */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_frame */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_colormap */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_skin */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is default_origin */
  c2 = c->next;
  /* c2 is default_angles */

  /* default_origin and default_angles are combined */
  a=c->down;
  a2=c2->down;

  do_simple_argument_write_bin(a, m); NEXT(a);
  do_simple_argument_write_bin(a2,m); NEXT(a2);
  do_simple_argument_write_bin(a, m); NEXT(a);
  do_simple_argument_write_bin(a2,m); NEXT(a2);
  do_simple_argument_write_bin(a, m);
  do_simple_argument_write_bin(a2,m);

  NEXT(c2);
  /* 2c is emtpy */
  if (c2 != NULL) syserror(QWDINTE,"additional command %d in spawnbaseline", c2->type);
}


void do_qwd_playerinfo_message_write_bin(node* n, BB_t* m)
{
  node* c;
  long mask, mask2;
  long flag;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    playerinfo is fairly complicated: double masks and reordering 
  */
  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* collect some info, combine the masks */
  for ( c = n->down , mask = mask2 = MASK_QWD_PLAYERINFO_EMPTY , flag = 0;
        c != NULL ; 
        c = c->next) {
    /* fprintf(stderr,"type=%d\n",c->type); */
    switch(c->type) {
      case TOKEN_UNKNOWN_MASK_BITS: mask |= nodes_to_bitlist(c->down); break;
      case TOKEN_PLAYER:
      case TOKEN_ORIGIN:
      case TOKEN_FRAME:
        /* do nothing, the are obligatory commands */
      break;
      case TOKEN_PING: mask |= MASK_QWD_PLAYERINFO_PING; break;
      case TOKEN_ANGLES_1: 
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_ANGLES_1;
        c->down->hint = H_ANGLE16;
      break;
      case TOKEN_ANGLES_2: 
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        if (SERVERVERSION>=27) { /* from 2.29BETA on */
          mask2 |= MASK_QWD_PLAYERINFO_ANGLES_2_n;
        }
        c->down->hint = H_ANGLE16;
      break;
      case TOKEN_ANGLES_3: 
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_ANGLES_3;
        c->down->hint = H_ANGLE16;
      break;
      case TOKEN_SPEED_X:
        if (SERVERVERSION>=27) c->down->hint=H_SHORT;
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_SPEED_X;
      break;
      case TOKEN_SPEED_Y:
        if (SERVERVERSION>=27) c->down->hint=H_SHORT;
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_SPEED_Y;
      break;
      case TOKEN_SPEED_Z:
        if (SERVERVERSION>=27) c->down->hint=H_SHORT;
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_SPEED_Z;
      break;
      case TOKEN_FIRE:
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_FLAG;
        flag |= 0x01;
      case TOKEN_JUMP:
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_FLAG;
        flag |= 0x02;
      case TOKEN_FLAG: {
        node *tc;
        
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_FLAG;
        for (tc = c->down ; tc != NULL ; tc = tc->next) {
          long flagbit = *(long*)tc->down;
          if ((flagbit >=0) && (flagbit < 8)) 
            flag |= 0x01 << flagbit;
          else
            syserror(QWDINTE,"wrong flag bit value %d", flagbit);
        } /* end for tc */
      }  
      break;
      case TOKEN_IMPULSE:
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        mask2 |= MASK_QWD_PLAYERINFO_IMPULSE;
      break;
      case TOKEN_LOAD:
        mask |= MASK_QWD_PLAYERINFO_MASK2;
        if (SERVERVERSION<=26) { /* up to 2.21 */
          mask2 |= MASK_QWD_PLAYERINFO_LOAD_o;
        }
      break;
      case TOKEN_CSPEED_X: mask |= MASK_QWD_PLAYERINFO_CSPEED_X; break;
      case TOKEN_CSPEED_Y: mask |= MASK_QWD_PLAYERINFO_CSPEED_Y; break;
      case TOKEN_CSPEED_Z: mask |= MASK_QWD_PLAYERINFO_CSPEED_Z; break;
      case TOKEN_MODELINDEX: mask |= MASK_QWD_PLAYERINFO_MODELINDEX; break;
      case TOKEN_UK_BYTE6: mask |= MASK_QWD_PLAYERINFO_UK_BYTE6; break;
      case TOKEN_WEAPON: mask |= MASK_QWD_PLAYERINFO_WEAPON; break;
      case TOKEN_WEAPONFRAME: mask |= MASK_QWD_PLAYERINFO_WEAPONFRAME; break;
      default: syserror(QWDINTE,"wrong command %d in playerinfo", c->type);
    } /* end switch c->type */
  } /* end for c */

  /* now write it out */
  /* message -> commands */
  c = n->down;
  /* c may be unknown_mask_bits */
  if (c->type == TOKEN_UNKNOWN_MASK_BITS) NEXT(c);
  /* c is player */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  /* c is origin */
  WriteShort(m,mask);
  do_simple_arguments_write_bin(c->down,m); NEXT(c);
  /* c is frame */
  do_simple_argument_write_bin(c->down,m); NEXT(c);
  if (mask & MASK_QWD_PLAYERINFO_PING) {
    unsigned char uc;
    
    cmdcheck(c,TOKEN_PING,"ping","playerinfo");
    uc = (unsigned char)(*(float*)c->down->down * 1000.0 + 0.5);
    /*
    if (uc>=106 && uc<=162) {
      fprintf(stderr,"uc=%i float=%#.9g\n",uc,*(float*)c->down->down);
    }
    */
    /* WriteByte(m,*(float*)c->down->down * 1000.0); */ 
    WriteByte(m,uc);
    NEXT(c);
  }
  /* was there a second mask ? */
  if (mask & MASK_QWD_PLAYERINFO_MASK2) {
    WriteByte(m,mask2);
    if (mask2 & MASK_QWD_PLAYERINFO_ANGLES_1) {
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
    if ( ( (SERVERVERSION>=27) && 
           (mask2 & MASK_QWD_PLAYERINFO_ANGLES_2_n) )
         ||
         (SERVERVERSION<=26) ) {
      /* c is angles_2 */ 
      cmdcheck(c,TOKEN_ANGLES_2,"angles_2","playerinfo");
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
    if (mask2 & MASK_QWD_PLAYERINFO_ANGLES_3) {
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
    if (mask2 & MASK_QWD_PLAYERINFO_SPEED_X) {
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
    if (mask2 & MASK_QWD_PLAYERINFO_SPEED_Y) {
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
    if (mask2 & MASK_QWD_PLAYERINFO_SPEED_Z) {
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
    if (c->type == TOKEN_FIRE) NEXT(c);
    if (c->type == TOKEN_JUMP) NEXT(c);
    if (c->type == TOKEN_FLAG) NEXT(c);
    if (mask2 & MASK_QWD_PLAYERINFO_FLAG) WriteByte(m,flag);
    if (mask2 & MASK_QWD_PLAYERINFO_IMPULSE) {
      cmdcheck(c,TOKEN_IMPULSE,"impulse","playerinfo");
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
    if ( (SERVERVERSION >= 27) ||
         (mask2 & MASK_QWD_PLAYERINFO_LOAD_o) ) {
      cmdcheck(c,TOKEN_LOAD,"load","playerinfo");
      do_simple_argument_write_bin(c->down,m); NEXT(c);
    }
  } /* end mask & MASK_QWD_PLAYERINFO_MASK2 */
  if (mask & MASK_QWD_PLAYERINFO_CSPEED_X) {
    cmdcheck(c,TOKEN_CSPEED_X,"cspeed_x","playerinfo");
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  if (mask & MASK_QWD_PLAYERINFO_CSPEED_Y) {
    cmdcheck(c,TOKEN_CSPEED_Y,"cspeed_y","playerinfo");
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  if (mask & MASK_QWD_PLAYERINFO_CSPEED_Z) {
    cmdcheck(c,TOKEN_CSPEED_Z,"cspeed_z","playerinfo");
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  if (mask & MASK_QWD_PLAYERINFO_MODELINDEX) {
    cmdcheck(c,TOKEN_MODELINDEX,"modelindex","playerinfo");
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  if (mask & MASK_QWD_PLAYERINFO_UK_BYTE6) {
    cmdcheck(c,TOKEN_UK_BYTE6,"uk_byte6","playerinfo");
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  if (mask & MASK_QWD_PLAYERINFO_WEAPON) {
    node* tc;
    long weapon;
    long bit;

    cmdcheck(c,TOKEN_WEAPON,"weapon","playerinfo");
    for ( tc = c->down , weapon = 0 ; tc != NULL ; tc = tc->next ) {
      bit = *(long*)tc->down;
      if (bit<1 || bit>9) 
        syserror(QWDINTE,"wrong weapon %d in playerinfo\n",bit);
      if (bit != 1) weapon |= 0x01 << (bit-2);
    }
    WriteByte(m,weapon);
    NEXT(c);
  }
  if (mask & MASK_QWD_PLAYERINFO_WEAPONFRAME) {
    cmdcheck(c,TOKEN_WEAPONFRAME,"weaponframe","playerinfo");
    do_simple_argument_write_bin(c->down,m); NEXT(c);
  }
  if (c != NULL) syserror(QWDINTE,"additional command %d is playerinfo",c->type);
  /* fprintf(stderr,"playerinfo end\n");*/ 
}


void do_qwd_nails_message_write_bin(node* n, BB_t* m)
{
  node* c;
  long count;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    nails is fairly complicated: count at the start, bit shifts etc.
  */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  for ( c = n->down , count = 0 ; 
        c != NULL ; 
        c = c->next , count++ )
          cmdcheck(c,TOKEN_NAIL,"nail","nails");

  /* write count */
  WriteByte(m,count);
  /* fprintf(stderr,"count = %ld\n", count); */ 

  /* now the nails */
  for ( c = n->down ; 
        c != NULL ; 
        c = c->next ) {
    node* tn;
    long origin[3];
    long angles_1_int;
    float angles_1;
    int i;
    unsigned char b[5];

    /* c is nail */
    tn = c->down;
    /* tn is origin */
    /* fprintf(stderr,"AAAAAAAAAAAAAAAAAAAAAA\n"); */
    cmdcheck(tn,TOKEN_ORIGIN,"origin","nail");
    /* fprintf(stderr,"BBBBBBBBBBBBBBBBBBBBB\n"); */ 
    origin[0] = *(long*)tn->down->down;
    origin[1] = *(long*)tn->down->next->down;
    origin[2] = *(long*)tn->down->next->next->down;
    NEXT(tn);
    /* tn is angles_1 */
    /* fprintf(stderr,"CCCCCCCCCCCCCCCCCCCCC\n"); */ 
    cmdcheck(tn,TOKEN_ANGLES_1,"angles_1","nail");
    /* fprintf(stderr,"DDDDDDDDDDDDDDDDDDDDDDDDDD\n"); */
    angles_1 = *(float*)tn->down->down; NEXT(tn);
    /* fprintf(stderr,"EEEEEEEEEEEEEEEEEEE\n"); */

    /* combine origin and angles_1 in 5 bytes */
    /* shift it right */
    for (i = 0 ; i < 3 ; i++) {
      origin[i] = origin[i] / 2 + 2048;
    }
    b[0] =   origin[0]       & 0xFF    ;
    b[1] = ((origin[0] >> 8) & 0x0F) |
           ((origin[1] << 4) & 0xF0)   ;
    b[2] =  (origin[1] >> 4) & 0xFF    ;
    b[3] =   origin[2]       & 0xFF    ;
    b[4] =  (origin[2] >> 8) & 0x0F    ;
    /* rescale the angle */
    angles_1 *= 8.0 / 180.0;
    /* make it positive */
    if (angles_1 < 0) angles_1 = angles_1 + 16;
    /* angles_1 should be now a integer number 0 .. 15 */
    angles_1_int = (long)angles_1;
    b[4] |= (angles_1_int << 4) & 0xF0;

    /* write the 5 bytes */
    for (i = 0 ; i<5 ; i++) {
      WriteByte(m,b[i]);
    }

    /* tn is angles_2 */
    /* fprintf(stderr,"FFFFFFFFFFFFFFFFF\n"); */
    cmdcheck(tn,TOKEN_ANGLES_2,"angles_2","nails");
    /* fprintf(stderr,"GGGGGGGGGGGGGGGGGGGGGG\n"); */
    do_simple_argument_write_bin(tn->down,m); NEXT(tn);
    /* fprintf(stderr,"HHHHHHHHHHHHHHHHHHHHH\n"); */ 
    /* tn is empty */
    if (tn != NULL) syserror(QWDINTE,"additional command %d in nail",tn->type);
    /* fprintf(stderr,"IIIIIIIIIIIIIIIIIIIIIII\n"); */ 
  }
}


int do_qwd_packetmask_write_bin(node* n, BB_t* m)
{
  node* cn;
  unsigned long mask, mask2;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */

  /* collect the mask bits */
  mask  = MASK_QWD_PACKET_EMPTY;
  mask2 = MASK_QWD_PACKET_EMPTY;
  for ( cn = n->down ; cn != NULL ; cn = cn->next ) {
    switch (cn->type) {
      case TOKEN_ENTITY:	mask |= *(long*)cn->down->down & 0x1FF; break;
      case TOKEN_REMOVE:	mask |= MASK_QWD_PACKET_REMOVE; break;
      case TOKEN_MODELINDEX:	mask2 |= MASK_QWD_PACKET_MODELINDEX; break;
      case TOKEN_FRAME:		mask |= MASK_QWD_PACKET_FRAME; break;
      case TOKEN_COLORMAP:	mask2 |= MASK_QWD_PACKET_COLORMAP; break;
      case TOKEN_SKIN:		mask2 |= MASK_QWD_PACKET_SKIN; break;
      case TOKEN_EFFECTS:	mask2 |= MASK_QWD_PACKET_EFFECTS; break;
      case TOKEN_ORIGIN_X:	mask |= MASK_QWD_PACKET_ORIGIN_X; break;
      case TOKEN_ANGLES_1:	mask2 |= MASK_QWD_PACKET_ANGLES_1; break;
      case TOKEN_ORIGIN_Y:	mask |= MASK_QWD_PACKET_ORIGIN_Y; break;
      case TOKEN_ANGLES_2:	mask |= MASK_QWD_PACKET_ANGLES_2; break;
      case TOKEN_ORIGIN_Z:	mask |= MASK_QWD_PACKET_ORIGIN_Z; break;
      case TOKEN_ANGLES_3:	mask2 |= MASK_QWD_PACKET_ANGLES_3; break;
      default: syserror(QWDINTE,"wrong command %d in packet",cn->type);
    } /* end switch cn->type */
  } /* end for cn */
  if (mask2) mask |= MASK_QWD_PACKET_MASK2;
  WriteShort(m,mask);
  if (mask & MASK_QWD_PACKET_MASK2)
    WriteByte(m,mask2);

  /* return the full bits, so we can simpler continue with the data */
  return (mask & 0xFE00) | mask2;
}


void do_qwd_packetdata_write_bin(node* n, BB_t* m, unsigned long mask)
{
  node* cn;
  float origin_x=0.0;
  float origin_y=0.0;
  float origin_z=0.0;
  float angles_1=0.0;
  float angles_2=0.0;
  float angles_3=0.0;
    
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */

  /* message -> commands */
  cn = n->down;
  /* cn is entity */
  cmdcheck(cn,TOKEN_ENTITY,"entity","packet");
  NEXT(cn);
  if (mask & MASK_QWD_PACKET_REMOVE) {
    /* cn is remove */
    cmdcheck(cn,TOKEN_REMOVE,"remove","packet");
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_MODELINDEX) {
    /* cn is modelindex */
    cmdcheck(cn,TOKEN_MODELINDEX,"modelindex","packet");
    do_simple_argument_write_bin(cn->down, m);
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_FRAME) {
    /* cn is frame */
    cmdcheck(cn,TOKEN_FRAME,"frame","packet");
    do_simple_argument_write_bin(cn->down, m);
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_COLORMAP) {
    /* cn is colormap */
    cmdcheck(cn,TOKEN_COLORMAP,"colormap","packet");
    do_simple_argument_write_bin(cn->down, m);
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_SKIN) {
    /* cn is skin */
    cmdcheck(cn,TOKEN_SKIN,"skin","packet");
    do_simple_argument_write_bin(cn->down, m);
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_EFFECTS) {
    /* cn is effects */
    cmdcheck(cn,TOKEN_EFFECTS,"effects","packet");
    do_simple_argument_write_bin(cn->down, m);
    NEXT(cn);
  }

  /* we have to reorder origin and angles */
  if (mask & MASK_QWD_PACKET_ORIGIN_X) {
    origin_x = *(float*)cn->down->down;
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_ORIGIN_Y) {
    origin_y = *(float*)cn->down->down;
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_ORIGIN_Z) {
    origin_z = *(float*)cn->down->down;
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_ANGLES_1) {
    angles_1 = *(float*)cn->down->down;
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_ANGLES_2) {
    angles_2 = *(float*)cn->down->down;
    NEXT(cn);
  }
  if (mask & MASK_QWD_PACKET_ANGLES_3) {
    angles_3 = *(float*)cn->down->down;
    NEXT(cn);
  }

  /* now write it out */
  if (mask & MASK_QWD_PACKET_ORIGIN_X) WriteCoord(m,origin_x);
  if (mask & MASK_QWD_PACKET_ANGLES_1) WriteAngle(m,angles_1);
  if (mask & MASK_QWD_PACKET_ORIGIN_Y) WriteCoord(m,origin_y);
  if (mask & MASK_QWD_PACKET_ANGLES_2) WriteAngle(m,angles_2);
  if (mask & MASK_QWD_PACKET_ORIGIN_Z) WriteCoord(m,origin_z);
  if (mask & MASK_QWD_PACKET_ANGLES_3) WriteAngle(m,angles_3);

  /* empty check */
  if (cn != NULL) syserror(QWDINTE,"additional command %d in packet",cn->type);
}              


void do_qwd_packetentities_message_write_bin(node* n, BB_t* m)
{
  node* cn;
  unsigned long mask;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */
              
  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));
                  
  for (cn = n->down ; cn != NULL ; cn = cn->next ) {
    /* cn is packetentity */
    cmdcheck(cn,TOKEN_PACKETENTITY,"packetentity","packetentities");
    /* first the mask */
    /* fprintf(stderr,"pre mask\n"); */
    mask = do_qwd_packetmask_write_bin(cn,m);
    /* fprintf(stderr,"post mask\n"); */
    /* fprintf(stderr,"mask=0x%0lx\n", mask); */
    /* now the data */
    /* fprintf(stderr,"pre data\n"); */
    do_qwd_packetdata_write_bin(cn,m,mask);
    /* fprintf(stderr,"post data\n"); */

  }
  WriteShort(m,0); /* empty mask and entity */
}


void do_qwd_deltapacketentities_message_write_bin(node* n, BB_t* m)
{
  node* cn;
  unsigned long mask;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */
              
  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));
                  
  /* message -> command */
  cn = n->down;
  /* cn is frame */
  do_simple_argument_write_bin(cn->down,m); NEXT(cn);
  /* cn is packetentity */
  for ( ; cn != NULL ; NEXT(cn) ) {
    /* cn is packetentity */
    cmdcheck(cn,TOKEN_PACKETENTITY,"packetentity","packetentities");
    /* first the mask */
    mask = do_qwd_packetmask_write_bin(cn,m);
    /* fprintf(stderr,"mask=0x%0x\n", mask); */
    /* now the data */
    do_qwd_packetdata_write_bin(cn,m,mask);
  }
  WriteShort(m,0); /* empty mask and entity */
}



/******************************************************************************/
/** QWD block editing *********************************************************/
/******************************************************************************/

void QWD_block_edit(node *n)
{
  node *ni;
  opt_t* opt;

  opt = glob_opt;

  /* data block manipulation */
  if (opt->option & opFixMulti) {

    /* should repair the multi-level play-back problem but it doesn't*/
    restart:
    for (ni=n->down;ni!=NULL;ni=ni->next) {
      /* is this message important ? */
      if (ni->type==TOKEN_STUFFTEXT) {
        /* ni->down is V_STRING, ni->down->down is the string itself */
        if (strcmp((char*)(ni->down->down),"reconnect\n")==0) {
          /* the whole ni must disappear */
          /* unlink this node, correct the start */
          n->down = node_unlink(n->down,ni);
          /* remove the node */
          node_delete(ni);
          /* do it again */
          /* to contine from here is possible but totally confusing */
          goto restart;
        } /* end if strcmp */
      } /* end if type */
    } /* end for ni */
  } /* end opFixMulti */

  /* other manipulations may come here */

}


/*- file end uqwd.c ---------------------------------------------------------*/
