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
    E-mail: Uwe.Girlich@itp.uni-leipzig.de */

/****************************************************************************\
|  udm2.c - implementation, general DM2 routines                             |
\****************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include "tools.h"
#include "record.h"
#include "udm2.h"


cs_t cs_info[] = {
  { DM2_CS_NAME,		"map name" },
  { DM2_CS_CDTRACK,		"CD track" },
  { DM2_CS_SKY,			"sky" },
  { DM2_CS_SKYAXIS,		"skyaxis" },
  { DM2_CS_SKYROTATE,		"sky rotate" },
  { DM2_CS_STATUSBAR,		"statusbar" },
  { DM2_CS_MAXCLIENTS,		"max clients" },
  { DM2_CS_MAPCHECKSUM,		"map checksum" },
  { DM2_CS_MODELS,		"model" },
  { DM2_CS_SOUNDS,		"sound" },
  { DM2_CS_IMAGES,		"image" },
  { DM2_CS_LIGHTS,		"lightstyle" },
  { DM2_CS_ITEMS,		"item" },
  { DM2_CS_PLAYERSKINS,		"playerskin" },
  { DM2_MAX_CONFIGSTRINGS,	NULL }
};

stat_t stat_info[] = {
  { DM2_STAT_HEALTH_ICON  , "HEALTH_ICON"  , 2 },
  { DM2_STAT_HEALTH       , "HEALTH"       , 1 },
  { DM2_STAT_AMMO_ICON    , "AMMO_ICON"    , 2 },
  { DM2_STAT_AMMO         , "AMMO"         , 1 },
  { DM2_STAT_ARMOR_ICON   , "ARMOR_ICON"   , 2 },
  { DM2_STAT_ARMOR        , "ARMOR"        , 1 },
  { DM2_STAT_SELECTED_ICON, "SELECTED_ICON", 2 },
  { DM2_STAT_PICKUP_ICON  , "PICKUP_ICON"  , 2 },
  { DM2_STAT_PICKUP_STRING, "PICKUP_STRING", 3 },
  { DM2_STAT_TIMER_ICON   , "TIMER_ICON"   , 2 },
  { DM2_STAT_TIMER        , "TIMER"        , 1 },
  { DM2_STAT_HELPICON     , "HELPICON"     , 2 },
  { DM2_STAT_SELECTED_ITEM, "SELECTED_ITEM", 6 },
  { DM2_STAT_LAYOUTS      , "LAYOUTS"      , 5 },
  { DM2_STAT_FRAGS        , "FRAGS"        , 1 },
  { DM2_STAT_FLASHES      , "FLASHES"      , 4 },
  { 0                     , NULL           , 0 }
};

char** cs_each;

token_t DM2_token[]={
	{ "block",			TOKEN_BLOCK,			0 },
	{ "endblock",			TOKEN_ENDBLOCK,			0 },
	{ "betweenblock",		TOKEN_BETWEENBLOCK,		0 },
	{ "unknown",			TOKEN_UNKNOWN,			0 },
	{ "id",				TOKEN_ID,			0 },
	{ "data",			TOKEN_DATA,			0 },
	{ "",				GEN_NOTHING,			0 },
	{ "frame",			TOKEN_FRAME,			0x14 },
	{ "deltapacketentities",	TOKEN_DELTAPACKETENTITIES,	0x13 },
	{ "packetentities", 		TOKEN_PACKETENTITIES,		0x12 },
	{ "playerinfo",			TOKEN_PLAYERINFO,		0x11 },
	{ "download",			TOKEN_DOWNLOAD,			0x10 },
	{ "centerprint",		TOKEN_CENTERPRINT,		0x0F },
	{ "spawnbaseline",		TOKEN_SPAWNBASELINE,		0x0E },
	{ "configstring",		TOKEN_CONFIGSTRING,		0x0D },
	{ "serverdata",			TOKEN_SERVERDATA,		0x0C },
	{ "stufftext",			TOKEN_STUFFTEXT,		0x0B },
	{ "print",			TOKEN_PRINT,			0x0A },
	{ "sound",			TOKEN_SOUND,			0x09 },
	{ "reconnect",			TOKEN_RECONNECT,		0x08 },
	{ "disconnect",			TOKEN_DISCONNECT,		0x07 },
	{ "nop",			TOKEN_NOP,			0x06 },
	{ "inventory",			TOKEN_INVENTORY,		0x05 },
	{ "layout",			TOKEN_LAYOUT,			0x04 },
	{ "temp_entity",		TOKEN_TEMP_ENTITY,		0x03 },
	{ "muzzlflash2",		TOKEN_MUZZLFLASH2,		0x02 },
	{ "muzzleflash",		TOKEN_MUZZLEFLASH,		0x01 },
	{ "bad",			TOKEN_BAD,			0x00 },

	{ "unknown_mask_bits",		TOKEN_UNKNOWN_MASK_BITS,	0x00 },
	{ "serverversion",		TOKEN_SERVERVERSION,		0x00 },
	{ "mapname",			TOKEN_MAPNAME,			0x00 },
	{ "index",			TOKEN_INDEX,			0x00 },
	{ "value",			TOKEN_VALUE,			0x00 },
	{ "seq1",			TOKEN_SEQ1,			0x00 },
	{ "seq2",			TOKEN_SEQ2,			0x00 },
	{ "origin",			TOKEN_ORIGIN,			0x00 },
	{ "entity",			TOKEN_ENTITY,			0x00 },
	{ "trace_endpos",		TOKEN_TRACE_ENDPOS,		0x00 },
	{ "movedir",			TOKEN_MOVEDIR,			0x00 },
	{ "count",			TOKEN_COUNT,			0x00 },
	{ "color",			TOKEN_COLOR,			0x00 },
	{ "entitytype",			TOKEN_ENTITYTYPE,		0x00 },
	{ "vol",			TOKEN_VOL,			0x00 },
	{ "attenuation",		TOKEN_ATTENUATION,		0x00 },
	{ "channel",			TOKEN_CHANNEL,			0x00 },
	{ "timeofs",			TOKEN_TIMEOFS,			0x00 },
	{ "soundnum",			TOKEN_SOUNDNUM,			0x00 },
	{ "level",			TOKEN_LEVEL,			0x00 },
	{ "string",			TOKEN_STRING,			0x00 },
	{ "key",			TOKEN_KEY,			0x00 },
	{ "client",			TOKEN_CLIENT,			0x00 },
	{ "uk_b1",			TOKEN_UK_B1,			0x00 },
	{ "game",			TOKEN_GAME,			0x00 },
	{ "modelindex",			TOKEN_MODELINDEX,		0x00 },
	{ "modelindex2",		TOKEN_MODELINDEX2,		0x00 },
	{ "modelindex3",		TOKEN_MODELINDEX3,		0x00 },
	{ "modelindex4",		TOKEN_MODELINDEX4,		0x00 },
	{ "skin",			TOKEN_SKIN,			0x00 },
	{ "vwep",			TOKEN_VWEP,			0x00 },
	{ "effects",			TOKEN_EFFECTS,			0x00 },
	{ "renderfx",			TOKEN_RENDERFX,			0x00 },
	{ "origin_x",			TOKEN_ORIGIN_X,			0x00 },
	{ "origin_y",			TOKEN_ORIGIN_Y,			0x00 },
	{ "origin_z",			TOKEN_ORIGIN_Z,			0x00 },
	{ "angles_1",			TOKEN_ANGLES_1,			0x00 },
	{ "angles_2",			TOKEN_ANGLES_2,			0x00 },
	{ "angles_3",			TOKEN_ANGLES_3,			0x00 },
	{ "old_origin",			TOKEN_OLD_ORIGIN,		0x00 },
	{ "event",			TOKEN_EVENT,			0x00 },
	{ "solid",			TOKEN_SOLID,			0x00 },
	{ "pm_type",			TOKEN_PM_TYPE,			0x00 },
	{ "velocity",			TOKEN_VELOCITY,			0x00 },
	{ "pm_time",			TOKEN_PM_TIME,			0x00 },
	{ "pm_flags",			TOKEN_PM_FLAGS,			0x00 },
	{ "gravity",			TOKEN_GRAVITY,			0x00 },
	{ "delta_angles",		TOKEN_DELTA_ANGLES,		0x00 },
	{ "viewoffset",			TOKEN_VIEWOFFSET,		0x00 },
	{ "viewangles",			TOKEN_VIEWANGLES,		0x00 },
	{ "kick_angles",		TOKEN_KICK_ANGLES,		0x00 },
	{ "gunindex",			TOKEN_GUNINDEX,			0x00 },
	{ "gunframe",			TOKEN_GUNFRAME,			0x00 },
	{ "gunoffset",			TOKEN_GUNOFFSET,		0x00 },
	{ "gunangles",			TOKEN_GUNANGLES,		0x00 },
	{ "blend",			TOKEN_BLEND,			0x00 },
	{ "fov",			TOKEN_FOV,			0x00 },
	{ "rdflags",			TOKEN_RDFLAGS,			0x00 },
	{ "stat",			TOKEN_STAT,			0x00 },
	{ "stats",			TOKEN_STATS,			0x00 },
	{ "packetentity",		TOKEN_PACKETENTITY,		0x00 },
	{ "remove",			TOKEN_REMOVE,			0x00 },
	{ "areas",			TOKEN_AREAS,			0x00 },
	{ "isdemo",			TOKEN_ISDEMO,			0x00 },
	{ "size",			TOKEN_SIZE,			0x00 },
	{ "percent",			TOKEN_PERCENT,			0x00 },
	{ "style",			TOKEN_STYLE,			0x00 },
	{ "dest_entity",		TOKEN_DEST_ENTITY,		0x00 },
	{ "nextid",			TOKEN_NEXTID,			0x00 },
	{ "start",			TOKEN_START,			0x00 },
	{ "dest_origin",		TOKEN_DEST_ORIGIN,		0x00 },
	{ "type",			TOKEN_TYPE,			0x00 },
	{ "flash_entity",		TOKEN_FLASH_ENTITY,		0x00 },
	{ "pos1",			TOKEN_POS1,			0x00 },
	{ "pos2",			TOKEN_POS2,			0x00 },
	{ "pos3",			TOKEN_POS3,			0x00 },
	{ "pos4",			TOKEN_POS4,			0x00 },
	{ "plat2flags",			TOKEN_PLAT2FLAGS,		0x00 },
	{ "wait",			TOKEN_WAIT,			0x00 },
	{ "connected",			TOKEN_CONNECTED,		0x00 },
	{ "unicast",			TOKEN_UNICAST,			0x00 }
};

void udm2_init(void)
{
  int i, j;
  char buffer[1000];

  cs_each = (char**) malloc(DM2_MAX_CONFIGSTRINGS * sizeof(char*));
  for (j=0;cs_info[j].note!=NULL;j++) {
    if (cs_info[j+1].index-cs_info[j].index == 1) {
      cs_each[cs_info[j].index]= strdup(cs_info[j].note);
    } else {
      for (i=cs_info[j].index;i<cs_info[j+1].index;i++) {
        sprintf(buffer, "%s[%d]", cs_info[j].note, i-cs_info[j].index);
        cs_each[i] = strdup(buffer);
      } /* end for */
    } /* end else */
  } /* end for */
}

void udm2_done(void)
{
  int i;
  for (i=0;i<DM2_MAX_CONFIGSTRINGS;i++) free(cs_each[i]);
  free(cs_each);
}

/* check, if file is DM2 */
unsigned long isDM2bin(char *filename)
{
  FILE *file;
  struct stat buf;
  size_t filesize;
  unsigned char buffer[10];
  unsigned long blocksize;
  unsigned long tics;

  if ((file=fopen(filename, "rb"))==NULL) return 0;
  if (stat(filename,&buf)==-1) return 0;
  filesize=buf.st_size;
  rewind(file);
  tics=0;
  while (ftell(file)<(long)filesize) {
    tics++;
    if (fread(buffer, 1, 4, file)!=4) return 0;
    blocksize = ((buffer[0])    ) +
                ((buffer[1])<< 8) +
                ((buffer[2])<<16) +
                ((buffer[3])<<24);
    if (blocksize==0xFFFFFFFF) break; /* not necessary in q2test */
    if (fseek(file, blocksize, SEEK_CUR)!=0) return 0;
  }
  if (ftell(file)!=(long)filesize) return 0;
  fclose(file);
  return tics;
}

/* check, if file is DM2 source */
int isDM2txt(char* filename)
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
      case 0: if (strncmp (&(buffer[p]), "block", 5)==0) return 1;
              if (strncmp (&(buffer[p]), "endblock", 8)==0) return 1;
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


void DM2_init(DM2_t* d, char *filename, char *mode)
{
  struct stat buf;
  long i;
  
  if ((d->filename = strdup(filename))==NULL) syserror(errno,"strdup");
  if (strcmp(d->filename,"-")==0) {
    d->file=stdout;
  }
  else {
    DM2_open(d,mode);
    if (stat(d->filename,&buf)==-1) syserror(errno,d->filename);
    d->filesize=buf.st_size;
  }
  for (i=0;i<DM2_MAX_CONFIGSTRINGS;i++) {
    d->config_string[i]=NULL;
  }
  /* we need a rewind here because some fopen modes may seek at the end */
  rewind(d->file);
  d->serverversion = 26; /* standard setting, retail CD version 3.05 */
  d->demotype = DM2_RECORD_CLIENT|DM2_RECORD_SERVER|DM2_RECORD_RELAY; /* all possible ways */
}

void DM2_prepare_write_bin(DM2_t* d)
{
  output_file = d->file;
  output_filename = d->filename;
} 

void DM2_open(DM2_t *d, char *mode)
{
  if ((d->file=fopen(d->filename, mode))==NULL) syserror(errno,d->filename);
}
  
void DM2_close(DM2_t *d)
{
  if (fclose(d->file)) syserror(errno,d->filename);
  d->file=NULL;
}

void DM2_done(DM2_t* d)
{
  long i;
  
  DM2_close(d);
  free(d->filename);
  for (i=0;i<DM2_MAX_CONFIGSTRINGS;i++) {
    if (d->config_string[i]!=NULL) {
      free(d->config_string[i]);
      d->config_string[i]=NULL;
    }
  }
}

void DM2_block_read_bin(DM2_t *d, BB_t* m)
{
  m->base = ftell(d->file);
  if (fread(m->buffer,1,4,d->file)!=4) syserror(FIREAD,d->filename);
  m->p = m->buffer;
  m->end = &(m->buffer[4]);
  m->size = ReadLong(m);
  if (m->size == (long)0xFFFFFFFF) { /* end block */
    m->size = 0;
  }
  else {
    if (m->size > BBB_MAX)
      syserror(WDM2,d->filename);
    if (fread(m->buffer+4,1,m->size,d->file)!=(size_t)m->size) 
      syserror(FIREAD,d->filename);
  }  
  m->size+=4;
  m->p = m->buffer;
  m->end = &(m->buffer[m->size]);
  m->top = (void *) d;
}

#define DM2TOP ((DM2_t*)(m->top))
#define MODELNAME(x) ((x)>=1 && (x)<DM2_MAX_MODELS  ? DM2TOP->config_string[DM2_CS_MODELS      + (x)] : (char*)NULL)
#define SOUNDNAME(x) ((x)>=1 && (x)<DM2_MAX_SOUNDS  ? DM2TOP->config_string[DM2_CS_SOUNDS      + (x)] : (char*)NULL)
#define IMAGENAME(x) ((x)>=1 && (x)<DM2_MAX_IMAGES  ? DM2TOP->config_string[DM2_CS_IMAGES      + (x)] : (char*)NULL)
#define LIGHTNAME(x) ((x)>=1 && (x)<DM2_MAX_LIGHTS  ? DM2TOP->config_string[DM2_CS_LIGHTS      + (x)] : (char*)NULL) 
#define ITEMNAME(x)  ((x)>=1 && (x)<DM2_MAX_ITEMS   ? DM2TOP->config_string[DM2_CS_ITEMS       + (x)] : (char*)NULL)
#define SKINNAME(x)  ((x)>=1 && (x)<DM2_MAX_CLIENTS ? DM2TOP->config_string[DM2_CS_PLAYERSKINS + (x)] : (char*)NULL)
/*
#define SERVERVERSION (DM2TOP->serverversion)
*/
#define DEMOTYPE (DM2TOP->demotype)

node* DM2_bin_to_node(BB_t *m, int opt)
{
  node *n, *tn, *ttn, *cn;
  unsigned long size;
  char ts[1000];
  long id;
  node *messagenode;
  node *blocknode;

  size = ReadLong(m); 
  if (size == 0) {
    return node_init(TOKEN_BETWEENBLOCK,NULL,0);
  }
  if (size == 0xFFFFFFFF) {
    return node_init(TOKEN_ENDBLOCK,NULL,0);
  }

  /*
  fprintf(stderr,"block start, base=0x%08lX, end=0x%08lX\n",
                  m->base + (m->p-m->buffer),
                  m->base + (m->p-m->buffer) + size - 1);
  */
  messagenode=NULL;
  while (m->p < m->end) {
    /* fprintf(stderr, "pos=0x%08lX\n", m->base + (m->p-m->buffer)); */
    if (opt & 0x01) {
      tn=node_init_all(GEN_NOTHING,H_NOTHING,NULL,0);
      sprintf(ts, "0x%08lX", m->base + (m->p-m->buffer));
      node_add_comment(tn,NODE_VALUE_STRING_dup(ts));
      messagenode=node_link(messagenode,tn);
    }
    id = ReadByte(m);
    /* fprintf(stderr,"id=0x%02lX\n", id); */ 
    if (id & MASK_UNICAST) {
      long unicast;
      node* unicastnode;

      unicast = ReadByte(m);
      id &= ~MASK_UNICAST;
      unicastnode = node_add_hint(
                      node_command_init(TOKEN_UNICAST, V_INT, H_BYTE,
                                        NODE_VALUE_INT_dup(unicast), 0),
                      H_UNICAST);
                                     
      messagenode = node_link(messagenode, unicastnode);
    }
    n=NULL;
    switch(id) {
      case 0x00: { /* bad */
        /* node tree */
        n=node_link(n,
            node_add_comment(node_init_all(TOKEN_BAD, H_SIMPLE, NULL, 0),
                             NODE_VALUE_STRING_dup("something is wrong")
                            )
                   ); 
      }
      break;
      case 0x01: { /* muzzleflash */
        long entity;
        long value;
        
        /* binary in */
        entity = ReadShort(m);
        value = ReadByte(m);
        /* node tree */
        tn = node_add_next (
               node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,NODE_VALUE_INT_dup(entity),0),
               node_command_init(TOKEN_VALUE,V_INT,H_BYTE,NODE_VALUE_INT_dup(value),0)
             );
        n=node_link(n,node_init_all(TOKEN_MUZZLEFLASH,H_SIMPLE,tn,0));
      }
      break;
      case 0x02: { /* muzzlflash2 */
        long entity;
        long value;
        
        /* binary in */
        entity = ReadShort(m);
        value = ReadByte(m);
        /* node tree */
        tn = node_add_next (
               node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,NODE_VALUE_INT_dup(entity),0),
               node_command_init(TOKEN_VALUE,V_INT,H_BYTE,NODE_VALUE_INT_dup(value),0)
             );
        n=node_link(n,node_init_all(TOKEN_MUZZLFLASH2,H_SIMPLE,tn,0));
      }
      break;
      case 0x03: { /* temp_entity */
        /* variables */
        #include "udm2-te1.c"
        
        /* binary in */
        #include "udm2-te2.c"

        tn = NULL;
        /* construct node tree */
        #include "udm2-te3.c"

        n=node_link(n,node_init_all(TOKEN_TEMP_ENTITY,H_SIMPLE,tn,0));
      }
      break;
      case 0x04: { /* layout */
        /* variable */
        char text[DM2_MAX_MESSAGE_SIZE];
        
        /* binary in */
        ReadString(m,text);

        /* node tree */
        tn=node_command_init(TOKEN_LAYOUT,V_STRING,H_STRING,
                             NODE_VALUE_STRING_dup(text),0);
        node_add_hint(tn,H_SIMPLE);                     
        n=node_link(n,tn);
      }
      break;
      case 0x05: { /* inventory */
        int i;

        /* variables */
        int inventory[DM2_MAX_ITEMS];

        /* binary in */
        for (i=0 ; i<DM2_MAX_ITEMS ; i++) {
          inventory[i] = ReadShort(m);
        }

        /* node tree */
        tn = NULL;
        for (tn=NULL , i=0 ; i<DM2_MAX_ITEMS ; i++) {
          tn=node_link(tn,node_init_all(V_INT,H_SHORT,
                                          NODE_VALUE_INT_dup(inventory[i]),0));
        }
        n=node_link(n,node_init_all(TOKEN_INVENTORY,H_SIMPLE,tn,0));
      }
      break;
      case 0x06: { /* nop */
        /* node tree */
        n=node_link(n, node_init_all(TOKEN_NOP,H_SIMPLE,NULL,0));
      }
      break;
      case 0x07: { /* disconnect */
        /* node tree */
        n=node_link(n, node_init_all(TOKEN_DISCONNECT,H_SIMPLE,NULL,0));
      }
      break;
      case 0x08: { /* reconnect */
        /* node tree */
        n=node_link(n, node_init_all(TOKEN_RECONNECT,H_SIMPLE,NULL,0));
      }
      break;
      case 0x09: { /* sound */
        /* variables */
        long mask;
        long soundnum;
        float vol;
        float attenuation;
        float timeofs;
        long entity;
        long channel;
        vec3_t origin;

        /* binary in */
        long mix;

        mask = ReadByte(m);
        soundnum = ReadByte(m);
        vol = (mask & MASK_DM2_SOUND_VOL) ? ((float)ReadByte(m) / 255.0) : (1.0);
        attenuation = (mask & MASK_DM2_SOUND_ATTENUATION) ? ((float)ReadByte(m) / 64.0) : (1.0);
        timeofs = (mask & MASK_DM2_SOUND_TIMEOFS) ? ((float)ReadByte(m) * 0.001) : (0.0);
        if (mask & MASK_DM2_SOUND_ENTITY) {
          mix = ReadShort(m);
          channel = mix & 0x07;
          entity = (mix >> 3);
        } else {
          channel = 0;
          entity = 0;
        }
        if (mask & MASK_DM2_SOUND_ORIGIN) {
          ReadPosition(m,origin);
        }

        /* node tree */
        tn=NULL;

        if (mask & MASK_DM2_SOUND_UNKNOWNBITS) 
          tn = node_link(
                 tn,
                 node_init(
                   TOKEN_UNKNOWN_MASK_BITS, 
                   bitlist_to_nodes(mask & MASK_DM2_SOUND_UNKNOWNBITS),
                   0
                 )
               );

        cn = node_command_init(TOKEN_SOUNDNUM, V_INT, H_BYTE,
                               NODE_VALUE_INT_dup(soundnum),0);
        if (soundnum >= 1 &&
            soundnum < DM2_MAX_SOUNDS &&
            SOUNDNAME(soundnum) != NULL) {
          node_add_comment(cn, NODE_VALUE_STRING_dup(SOUNDNAME(soundnum)));
        }
        tn = node_link(tn,cn);
        if (mask & MASK_DM2_SOUND_VOL) {
          tn=node_link(tn,node_command_init(TOKEN_VOL,V_FLOAT,H_VOL,
                       NODE_VALUE_FLOAT_dup(vol),0));
        }
        if (mask & MASK_DM2_SOUND_ATTENUATION) {
          tn=node_link(tn,node_command_init(TOKEN_ATTENUATION,V_FLOAT,H_ATTENUATION,
                       NODE_VALUE_FLOAT_dup(attenuation),0));
        }
        if (mask & MASK_DM2_SOUND_TIMEOFS) {
          tn=node_link(tn,node_command_init(TOKEN_TIMEOFS,V_FLOAT,H_TIMEOFS,
                       NODE_VALUE_FLOAT_dup(timeofs),0));
        }
        if (mask & MASK_DM2_SOUND_ENTITY) {
          tn=node_link(tn,node_command_init(TOKEN_CHANNEL,V_INT,0,
                       NODE_VALUE_INT_dup(channel),0));
          tn=node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,0,
                       NODE_VALUE_INT_dup(entity),0));
        }
        if (mask & MASK_DM2_SOUND_ORIGIN) {
          tn=node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                          NODE_VALUE_FLOAT_dup(origin[0]),
                          NODE_VALUE_FLOAT_dup(origin[1]),
                          NODE_VALUE_FLOAT_dup(origin[2]),0));
        }
        n=node_link(n,node_init_all(TOKEN_SOUND,H_DM2_SOUND,tn,0));
      }
      break;
      case 0x0A: { /* print */
        /* variables */
        long level;
        char string[DM2_MAX_MESSAGE_SIZE];

        /* binary in */
        level = ReadByte(m);
        ReadString(m,string);
        
        /* node tree */
        n=node_link(n,
            node_add_hint(
              node_init_all(TOKEN_PRINT, H_SIMPLE, 
                node_add_next(
                   node_command_init(TOKEN_LEVEL, V_INT, H_BYTE, 
                                     NODE_VALUE_INT_dup(level),0),
                   node_command_init(TOKEN_STRING, V_STRING, H_STRING, 
                                     NODE_VALUE_STRING_dup(string),0)
                ), 
              0),
            H_SIMPLE)
          );
      }
      break;
      case 0x0B: { /* stufftext */
        /* variable */
        char text[DM2_MAX_MESSAGE_SIZE];

        /* binary in */
        ReadString(m,text);

        /* node tree */
        tn=node_command_init(TOKEN_STUFFTEXT, V_STRING, H_STRING,
                             NODE_VALUE_STRING_dup(text),0);
	node_add_hint(tn,H_SIMPLE);
        n=node_link(n, tn);
      }
      break;
      case 0x0C: { /* serverdata */
        /* variables */
        long serverversion;
        long key;
        long isdemo;
        char game[DM2_MAX_MESSAGE_SIZE];
        long client;
        char mapname[DM2_MAX_MESSAGE_SIZE];
        long i;

        /* binary in */
        serverversion = ReadLong(m);
        key = ReadLong(m);
        isdemo = ReadByte(m);
        ReadString(m, game);
        client = ReadShort(m);
        ReadString(m, mapname);

	/* memorize the protocol version */
	/* in a global variable: bad */
        SERVERVERSION = serverversion;
        /* in the DM2 structure: not usable any more */
        (DM2TOP->serverversion) = serverversion;
        
	/* memorize the demo type */
	if (isdemo) {
	  DEMOTYPE = isdemo;
	}
	else { /* zero is a network recorded client format */
	  DEMOTYPE = DM2_RECORD_CLIENT;
	}

        /* remove the old stuff */
        for (i=0;i<DM2_MAX_CONFIGSTRINGS;i++) {
          if (DM2TOP->config_string[i] != NULL) {
            free(DM2TOP->config_string[i]); 
            DM2TOP->config_string[i]=NULL;
          }
        }

        /* node tree */
        tn = node_command_init(TOKEN_SERVERVERSION, V_INT, H_LONG, 
                               NODE_VALUE_INT_dup(serverversion),0);
        tn = node_link(tn,
                       node_command_init(TOKEN_KEY, V_INT, H_LONG, 
                                         NODE_VALUE_INT_dup(key),0)
                      );
        tn = node_link(tn,
                       node_command_init(TOKEN_ISDEMO, V_INT, H_BYTE, 
                                         NODE_VALUE_INT_dup(isdemo),0)
                      );
        tn = node_link(tn,
                       node_command_init(TOKEN_GAME, V_STRING, H_STRING, 
                                         NODE_VALUE_STRING_dup(game),0)
                      );
        tn = node_link(tn,
                       node_command_init(TOKEN_CLIENT, V_INT, H_SHORT, 
                                         NODE_VALUE_INT_dup(client),0)
                      );
        tn = node_link(tn, 
                       node_command_init(TOKEN_MAPNAME, V_STRING, H_STRING,
                                         NODE_VALUE_STRING_dup(mapname),0)
                      );
        n=node_link(n,node_init_all(TOKEN_SERVERDATA, H_SIMPLE, tn, 0));
      }
      break;
      case 0x0D: { /* configstring */
        long index;
        char string[DM2_MAX_MESSAGE_SIZE];

        /* binary in */
        index = ReadShort(m);
        ReadString(m,string);

        /* prepare stuff */
        if (index>=1 && index<DM2_MAX_CONFIGSTRINGS) {
          if (DM2TOP->config_string[index] != NULL) free(DM2TOP->config_string[index]);
          DM2TOP->config_string[index] = strdup(string);
        }

        /* node tree */
        tn = node_command_init(TOKEN_INDEX, V_INT, H_SHORT,
                               NODE_VALUE_INT_dup(index),0);
        if (index>=0 && index<DM2_MAX_CONFIGSTRINGS) {
          node_add_comment(tn,NODE_VALUE_STRING_dup(cs_each[index]));
        }  
        tn = node_link(tn,
                       node_command_init(TOKEN_STRING, V_STRING, H_STRING,
                                         NODE_VALUE_STRING_dup(string),0)
                      );
        n=node_link(n,node_init_all(TOKEN_CONFIGSTRING, H_SIMPLE, tn, 0));
      }
      break;
      case 0x0E: { /* spawnbaseline */
        /* variables */
        long mask;
        long entity;
        vec3_t origin;
        vec3_t angles;
        vec3_t old_origin;
        long modelindex=0;
        long modelindex2=0;
        long modelindex3=0;
        long modelindex4=0;
        long frame=0;
        long skin=0;
        long vwep=0;
        long effects=0;
        long renderfx=0;
        long solid=0;
        long sound=0;
        long event=0;

        /* binary in */
        mask = ReadByte(m);
        if (mask & 0x00000080) mask |= (ReadByte(m) <<  8);
        if (mask & 0x00008000) mask |= (ReadByte(m) << 16);
        if (mask & 0x00800000) mask |= (ReadByte(m) << 24);
        /* fprintf(stderr,"spawnmask=0x%x\n", mask);*/ 
        entity = (mask & 0x00000100) ? ReadShort(m) : ReadByte(m);
        if (mask & 0x00000800) modelindex = ReadByte(m);
        if (mask & 0x00100000) modelindex2 = ReadByte(m);
        if (mask & 0x00200000) modelindex3 = ReadByte(m);
        if (mask & 0x00400000) modelindex4 = ReadByte(m);
        if (mask & MASK_DM2_PACKET_FRAME_BYTE) frame = ReadByte(m);
        if (mask & MASK_DM2_PACKET_FRAME_SHORT) frame = ReadShort(m);
        if (mask & MASK_DM2_PACKET_SKIN) {
          if ((mask & MASK_DM2_PACKET_SKIN_LONG) == MASK_DM2_PACKET_SKIN_LONG)
            skin = ReadLong(m);
          else {
            if (mask & MASK_DM2_PACKET_SKIN_SHORT) skin = ReadShort(m);
            if (mask & MASK_DM2_PACKET_SKIN_BYTE) skin = ReadByte(m);
          }
          vwep = (skin >> 8) & 0xFFFFFF;
          skin &= 0xFF;
        }
        if (mask & 0x00004000) {
          if (mask & 0x00080000) effects = ReadLong(m);
          else effects = ReadByte(m);
        }
        else {
          if (mask & 0x00080000) effects = ReadShort(m);
        }
        if (mask & 0x00001000) {
          if (mask & 0x00040000) renderfx = ReadLong(m);
          else renderfx = ReadByte(m);
        }
        else {
          if (mask & 0x00040000) renderfx = ReadShort(m);
        }
        if (mask & 0x00000001) origin[0] = ReadCoord(m);
        if (mask & 0x00000002) origin[1] = ReadCoord(m);
        if (mask & 0x00000200) origin[2] = ReadCoord(m);
        if (mask & 0x00000400) angles[0] = ReadAngle(m);
        if (mask & 0x00000004) angles[1] = ReadAngle(m);
        if (mask & 0x00000008) angles[2] = ReadAngle(m);
        if (mask & 0x01000000) ReadPosition(m,old_origin);
        if (mask & 0x04000000) sound = ReadByte(m);
        event = (mask & 0x00000020) ? ReadByte(m) : 0;
        if (mask & 0x08000000) solid = ReadShort(m);

        /* node tree */
        tn = NULL;
        
        if (mask & MASK_DM2_PACKET_UNKNOWNBITS) 
          tn = node_link(
                 tn,
                 node_init(
                   TOKEN_UNKNOWN_MASK_BITS, 
                   bitlist_to_nodes(mask & MASK_DM2_PACKET_UNKNOWNBITS),
                   0
                 )
               );

        tn = node_link(tn,node_command_init(TOKEN_ENTITY, V_INT, H_SB, 
                                         NODE_VALUE_INT_dup(entity),0));
        if (mask & 0x00000800) {
          cn = node_command_init(TOKEN_MODELINDEX, V_INT, H_BYTE,
                                 NODE_VALUE_INT_dup(modelindex),0);
          if (modelindex >= 1 && 
              modelindex < DM2_MAX_MODELS &&
              MODELNAME(modelindex) != NULL) 
            node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex)));
          node_add_next(tn,cn);
        }
        if (mask & 0x00100000) {
          cn = node_command_init(TOKEN_MODELINDEX2, V_INT, H_BYTE, 
                                 NODE_VALUE_INT_dup(modelindex2),0);
          if (modelindex2 >= 1 &&
              modelindex2 < DM2_MAX_MODELS &&
              MODELNAME(modelindex2) != NULL) 
                node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex2)));
          node_add_next(tn,cn);
        }
        if (mask & 0x00200000) {
          cn = node_command_init(TOKEN_MODELINDEX3, V_INT, H_BYTE, 
                                 NODE_VALUE_INT_dup(modelindex3),0);
          if (modelindex3 >= 1 &&
              modelindex3 < DM2_MAX_MODELS &&
              MODELNAME(modelindex3) != NULL) 
                node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex3)));
          node_add_next(tn,cn);
        }
        if (mask & 0x00400000) {
          cn = node_command_init(TOKEN_MODELINDEX4, V_INT, H_BYTE, 
                                 NODE_VALUE_INT_dup(modelindex4),0);
          if (modelindex4 >= 1 &&
              modelindex4 < DM2_MAX_MODELS &&
              MODELNAME(modelindex4) != NULL) 
                node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex4)));
          node_add_next(tn,cn);
        }
        if (mask & MASK_DM2_PACKET_FRAME)
          tn = node_link(tn,node_command_init(TOKEN_FRAME, V_INT, H_SB, 
                                           NODE_VALUE_INT_dup(frame),0));
        if (mask & MASK_DM2_PACKET_SKIN) {
          tn = node_link(tn,node_command_init(TOKEN_SKIN, V_INT, H_BYTE, 
                                              NODE_VALUE_INT_dup(skin),0));
          if (vwep) 
            tn = node_link(tn,node_command_init(TOKEN_VWEP, V_INT, H_LSB, 
                                                NODE_VALUE_INT_dup(vwep),0));
        }
        if (mask & 0x00084000)
          tn = node_link(tn,node_command_init(TOKEN_EFFECTS, V_INT, H_LSB, 
                                           NODE_VALUE_INT_dup(effects),0));
        if (mask & 0x00041000)
          tn = node_link(tn,node_command_init(TOKEN_RENDERFX, V_INT, H_LSB, 
                                           NODE_VALUE_INT_dup(renderfx),0));
        if (mask & 0x00000001)
          tn = node_link(tn,node_command_init(TOKEN_ORIGIN_X, V_FLOAT, H_COORD, 
                                           NODE_VALUE_FLOAT_dup(origin[0]),0));
        if (mask & 0x00000002)
          tn = node_link(tn,node_command_init(TOKEN_ORIGIN_Y, V_FLOAT, H_COORD, 
                                           NODE_VALUE_FLOAT_dup(origin[1]),0));
        if (mask & 0x00000200)
          tn = node_link(tn,node_command_init(TOKEN_ORIGIN_Z, V_FLOAT, H_COORD, 
                                           NODE_VALUE_FLOAT_dup(origin[2]),0));
        if (mask & 0x00000400)
          tn = node_link(tn,node_command_init(TOKEN_ANGLES_1, V_FLOAT, H_ANGLE, 
                                           NODE_VALUE_FLOAT_dup(angles[0]),0));
        if (mask & 0x00000004)
          tn = node_link(tn,node_command_init(TOKEN_ANGLES_2, V_FLOAT, H_ANGLE, 
                                           NODE_VALUE_FLOAT_dup(angles[1]),0));
        if (mask & 0x00000008)
          tn = node_link(tn,node_command_init(TOKEN_ANGLES_3, V_FLOAT, H_ANGLE, 
                                           NODE_VALUE_FLOAT_dup(angles[2]),0));
        if (mask & 0x01000000) 
          tn = node_link(tn, 
            node_triple_command_init(TOKEN_OLD_ORIGIN, V_FLOAT, H_COORD, 
                                     NODE_VALUE_FLOAT_dup(old_origin[0]),
                                     NODE_VALUE_FLOAT_dup(old_origin[1]),
                                     NODE_VALUE_FLOAT_dup(old_origin[2]),
                                     0));
        if (mask & 0x04000000) {
          cn = node_command_init(TOKEN_SOUND, V_INT, H_BYTE,
                                 NODE_VALUE_INT_dup(sound),0);
          if (sound >= 1 &&
              sound < DM2_MAX_SOUNDS &&
              SOUNDNAME(sound) != NULL) {
            node_add_comment(cn, NODE_VALUE_STRING_dup(SOUNDNAME(sound)));
          }
          node_add_next(tn,cn);
        }
        if (mask & 0x00000020) 
          tn = node_link(tn,node_command_init(TOKEN_EVENT, V_INT, H_BYTE, 
                                           NODE_VALUE_INT_dup(event),0));
        if (mask & 0x08000000) 
          tn = node_link(tn,node_command_init(TOKEN_SOLID, V_INT, H_SHORT, 
                                           NODE_VALUE_INT_dup(solid),0));
        n=node_link(n,node_init_all(TOKEN_SPAWNBASELINE, H_DM2_SPAWNBASELINE, tn, 0));
      }
      break;
      case 0x0F: { /* centerprint */
        /* variable */
        char text[DM2_MAX_MESSAGE_SIZE];
        
        /* binary in */
        ReadString(m,text);

        /* node tree */
        tn=node_command_init(TOKEN_CENTERPRINT,V_STRING,H_STRING,
                             NODE_VALUE_STRING_dup(text),0);
        node_add_hint(tn,H_SIMPLE);
        n = node_link(n,tn);
      }
      break;
      case 0x10: { /* download */
        /* variables */
        long size;
        long percent;
        unsigned char data[65536];
        
        /* binary in */
        size = ReadShort(m);
        percent=ReadByte(m);
        if (SERVERVERSION >= 32) {
          int i;
          for (i=0 ; i<size ; i++) {
            data[i] = ReadByte(m);
          }
        }
        
        /* node tree */
        tn = node_command_init(TOKEN_SIZE, V_INT, H_SHORT, NODE_VALUE_INT_dup(size), 0);
        tn = node_link(tn, node_command_init(TOKEN_PERCENT, V_INT, H_BYTE, NODE_VALUE_INT_dup(percent), 0)); 
        if (SERVERVERSION >= 32 && size>0) {
          int i;
          for (ttn=NULL, i=0 ; i<size ; i++) {
            ttn=node_link(ttn,node_init(V_BYTEHEX, NODE_VALUE_INT_dup(data[i]), 0));
          }
          tn=node_link(tn, node_init(TOKEN_DATA,ttn,0));
        }
        n=node_link(n,node_init_all(TOKEN_DOWNLOAD, H_SIMPLE, tn, 0));
      }
      break;
      case 0x11: { /* playerinfo */
        long mask, mask2;
        long pm_type=0;
        vec3_t origin;
        vec3_t velocity;
        unsigned char pm_flags=0;
        unsigned char pm_time=0;
        short gravity=0;
        vec3_t delta_angles;
        vec3_t viewangles;
        vec3_t viewoffset;
        vec3_t kick_angles;
        vec3_t gunangles;
        vec3_t gunoffset;
        int gunindex=0;
        int gunframe=0;
        float blend[4];
        long fov=0;
        int rdflags=0;
        short stats[DM2_MAX_STATS];

        /* binary in */
        int i;

        mask = ReadShort(m);
        if (mask & 0x0001) pm_type = ReadByte(m);
        if (mask & 0x0002) {
          ReadPosition(m, origin);
        }
        if (mask & 0x0004) {
          ReadPosition(m, velocity);
        }
        if (mask & 0x0008) pm_time = ReadByte(m);
        if (mask & 0x0010) pm_flags = ReadByte(m);
        if (mask & 0x0020) gravity = ReadShort(m);
        if (mask & 0x0040) {
          delta_angles[0] = ReadAngle16(m);
          delta_angles[1] = ReadAngle16(m);
          delta_angles[2] = ReadAngle16(m);
        }
        if (mask & 0x0080) {
          ReadOffsetVec(m, viewoffset);
        }
        if (mask & 0x0100) {
          viewangles[0] = ReadAngle16(m);
          viewangles[1] = ReadAngle16(m);
          viewangles[2] = ReadAngle16(m);
        }
        if (mask & 0x0200) {
          ReadOffsetVec(m, kick_angles);
        }
        if (mask & 0x1000) gunindex = ReadByte(m);
        if (mask & 0x2000) {
          gunframe = ReadByte(m);
          ReadOffsetVec(m, gunoffset);
          ReadOffsetVec(m, gunangles);
        }
        if (mask & 0x0400) ReadBlendVec(m, blend);
        if (mask & 0x0800) fov = ReadByte(m);
        if (mask & 0x4000) rdflags = ReadByte(m);
        mask2 = ReadLong(m);
        for (i=0;i<DM2_MAX_STATS;i++) {
          if (mask2 & (0x00000001 << i)) stats[i] = ReadShort(m);
        }

        /* node tree */
        tn =NULL;

        if (mask & 0x0001) 
          tn = node_link(tn,node_command_init(TOKEN_PM_TYPE,V_INT,H_BYTE,
                          NODE_VALUE_INT_dup(pm_type),0));
        if (mask & 0x0002)
          tn=node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                            NODE_VALUE_FLOAT_dup(origin[0]),
                            NODE_VALUE_FLOAT_dup(origin[1]),
                            NODE_VALUE_FLOAT_dup(origin[2]),0));
        if (mask & 0x0004)
          tn=node_link(tn,node_triple_command_init(TOKEN_VELOCITY,V_FLOAT,H_COORD,
                            NODE_VALUE_FLOAT_dup(velocity[0]),
                            NODE_VALUE_FLOAT_dup(velocity[1]),
                            NODE_VALUE_FLOAT_dup(velocity[2]),0));
        if (mask & 0x0008)
          tn = node_link(tn,node_command_init(TOKEN_PM_TIME,V_INT,H_BYTE,
                          NODE_VALUE_INT_dup(pm_time),0));
        if (mask & 0x0010)
          tn = node_link(tn,node_command_init(TOKEN_PM_FLAGS,V_INT,H_BYTE,
                          NODE_VALUE_INT_dup(pm_flags),0));
        if (mask & 0x0020)
          tn = node_link(tn,node_command_init(TOKEN_GRAVITY,V_INT,H_SHORT,
                          NODE_VALUE_INT_dup(gravity),0));
        if (mask & 0x0040)
          tn=node_link(tn,node_triple_command_init(TOKEN_DELTA_ANGLES,V_FLOAT,H_ANGLE16,
                            NODE_VALUE_FLOAT_dup(delta_angles[0]),
                            NODE_VALUE_FLOAT_dup(delta_angles[1]),
                            NODE_VALUE_FLOAT_dup(delta_angles[2]),0));
        if (mask & 0x0080)
          tn=node_link(tn,node_triple_command_init(TOKEN_VIEWOFFSET,V_FLOAT,H_OFFSET,
                            NODE_VALUE_FLOAT_dup(viewoffset[0]),
                            NODE_VALUE_FLOAT_dup(viewoffset[1]),
                            NODE_VALUE_FLOAT_dup(viewoffset[2]),0));
        if (mask & 0x0100)
          tn=node_link(tn,node_triple_command_init(TOKEN_VIEWANGLES,V_FLOAT,H_ANGLE16,
                            NODE_VALUE_FLOAT_dup(viewangles[0]),
                            NODE_VALUE_FLOAT_dup(viewangles[1]),
                            NODE_VALUE_FLOAT_dup(viewangles[2]),0));
        if (mask & 0x0200)
          tn=node_link(tn,node_triple_command_init(TOKEN_KICK_ANGLES,V_FLOAT,H_OFFSET,
                            NODE_VALUE_FLOAT_dup(kick_angles[0]),
                            NODE_VALUE_FLOAT_dup(kick_angles[1]),
                            NODE_VALUE_FLOAT_dup(kick_angles[2]),0));
        if (mask & 0x1000)
          tn = node_link(tn,node_command_init(TOKEN_GUNINDEX,V_INT,H_BYTE,
                            NODE_VALUE_INT_dup(gunindex),0));
        if (mask & 0x2000) {
          tn = node_link(tn,node_command_init(TOKEN_GUNFRAME,V_INT,H_BYTE,
                            NODE_VALUE_INT_dup(gunframe),0));
          tn=node_link(tn,node_triple_command_init(TOKEN_GUNOFFSET,V_FLOAT,H_OFFSET,
                            NODE_VALUE_FLOAT_dup(gunoffset[0]),
                            NODE_VALUE_FLOAT_dup(gunoffset[1]),
                            NODE_VALUE_FLOAT_dup(gunoffset[2]),0));
          tn=node_link(tn,node_triple_command_init(TOKEN_GUNANGLES,V_FLOAT,H_OFFSET,
                            NODE_VALUE_FLOAT_dup(gunangles[0]),
                            NODE_VALUE_FLOAT_dup(gunangles[1]),
                            NODE_VALUE_FLOAT_dup(gunangles[2]),0));
        }
        if (mask & 0x0400) {
          ttn=NULL;
          for (i=0;i<4;i++) {
            ttn=node_link(ttn,node_init_all(V_FLOAT,H_BLEND,NODE_VALUE_FLOAT_dup(blend[i]),0));
          }
          tn=node_link(tn,node_init_all(TOKEN_BLEND, 0, ttn, 0));
        }
        if (mask & 0x0800) 
          tn = node_link(tn,node_command_init(TOKEN_FOV,V_INT,H_BYTE,
                            NODE_VALUE_INT_dup(fov),0));
        if (mask & 0x4000) 
          tn = node_link(tn,node_command_init(TOKEN_RDFLAGS,V_INT,H_BYTE,
                            NODE_VALUE_INT_dup(rdflags),0));

        if (mask2) {
          ttn = NULL;
          for (i=0;i<DM2_MAX_STATS;i++) {
            if (mask2 & (0x00000001 << i)) {
              node* in;
              node* vn;
              long j;
              char* s;
              char buffer[100];

              in=node_command_init(TOKEN_INDEX,V_INT,H_BYTE,
                                   NODE_VALUE_INT_dup(i),0);
              vn=node_command_init(TOKEN_VALUE,V_INT,H_SHORT,
                                   NODE_VALUE_INT_dup(stats[i]),0);
              for (j=0;stat_info[j].type;j++) {
                if (i==stat_info[j].index) {
                  node_add_comment(in,NODE_VALUE_STRING_dup(stat_info[j].name));
                  s=NULL;
                  switch (stat_info[j].type) {
                    case 1:
                    break;
                    case 2:
                      if (stats[i]==0)
                        s="image off";
                      else if (stats[i]>0 && 
                               stats[i]<DM2_MAX_IMAGES && 
                               DM2TOP->config_string[DM2_CS_IMAGES + stats[i]] != NULL)
                        s=DM2TOP->config_string[DM2_CS_IMAGES + stats[i]];
                    break;
                    case 3:
                      if (stats[i]==0)
                        s="string off";
                      else if (stats[i]>0 &&
                               stats[i]<DM2_MAX_CONFIGSTRINGS &&
                               DM2TOP->config_string[stats[i]] != NULL) {
                        sprintf(buffer,"\"%s\"",DM2TOP->config_string[stats[i]]);
                        s=buffer;
                      }
                    break;
                    case 4:
                      switch (stats[i]) {
                        case 0: s="flash off"; break;
                        case 1: s="health"   ; break;
                        case 2: s="armor"    ; break;
                      }
                    break;
                    case 5:
                      switch (stats[i]) {
                        case 0: s="popup off"; break;
                        case 1: s="help"     ; break;
                        case 2: s="inventory"; break;
                      }
                    break;
                    case 6:
                      if (stats[i]==0)
                        s="item off";
                      else if (stats[i]>0 &&
                               stats[i]<DM2_MAX_ITEMS &&
                               DM2TOP->config_string[DM2_CS_ITEMS + stats[i]] != NULL) 
                        s=DM2TOP->config_string[DM2_CS_ITEMS + stats[i]];
                    break;
                  } /* switch type */
                  if (s!=NULL) node_add_comment(vn,NODE_VALUE_STRING_dup(s));
                } /* if i */
              } /* for j */
              ttn=node_link(ttn,node_init_all(TOKEN_STAT, 0, 
                              node_add_next(in,vn), 0));
            } /* mask2 */
          } /* end for i */
          tn = node_link(tn,node_init_all(TOKEN_STATS, 0, ttn, 0));
        } /* if (mask2) */ 
        n=node_link(n,node_init_all(TOKEN_PLAYERINFO, H_DM2_PLAYERINFO, tn, 0));
      }
      break;
      case 0x12: { /* packetentities */
        /* variables */
        long mask;
        long entity;
        long remove;
        vec3_t origin;
        vec3_t angles;
        vec3_t old_origin;
        long modelindex=0;
        long modelindex2=0;
        long modelindex3=0;
        long modelindex4=0;
        long frame=0;
        long skin=0;
        long vwep=0;
        long effects=0;
        long renderfx=0;
        long solid=0;
        long sound=0;
        long event=0;
	
        /* binary in & node tree */
        ttn=NULL;
        for (;;) {
          mask = ReadByte(m);
          if (mask & 0x00000080) mask |= (ReadByte(m) <<  8);
          if (mask & 0x00008000) mask |= (ReadByte(m) << 16);
          if (mask & 0x00800000) mask |= (ReadByte(m) << 24);
          /* fprintf(stderr,"inmask=0x%x\n", mask); */ 
          entity = (mask & 0x00000100) ? ReadShort(m) : ReadByte(m);
          
          if (entity == 0) break;

          remove = (mask & 0x00000040) ? 1 : 0;
          if (mask & 0x00000800) modelindex = ReadByte(m);
          if (mask & 0x00100000) modelindex2 = ReadByte(m);
          if (mask & 0x00200000) modelindex3 = ReadByte(m);
          if (mask & 0x00400000) modelindex4 = ReadByte(m);
          if (mask & MASK_DM2_PACKET_FRAME_BYTE) frame = ReadByte(m);
          if (mask & MASK_DM2_PACKET_FRAME_SHORT) frame = ReadShort(m);
          if (mask & MASK_DM2_PACKET_SKIN) {
            if ((mask & MASK_DM2_PACKET_SKIN_LONG) == MASK_DM2_PACKET_SKIN_LONG)
              skin = ReadLong(m);
            else {
              if (mask & MASK_DM2_PACKET_SKIN_SHORT) skin = ReadShort(m);
              if (mask & MASK_DM2_PACKET_SKIN_BYTE) skin = ReadByte(m);
            }
            vwep = (skin >> 8) & 0xFFFFFF;
            skin &= 0xFF;
          }
          if (mask & 0x00004000) {
            if (mask & 0x00080000) effects = ReadLong(m);
            else effects = ReadByte(m);
          }
          else {
            if (mask & 0x00080000) effects = ReadShort(m);
          }
          if (mask & 0x00001000) {
            if (mask & 0x00040000) renderfx = ReadLong(m);
            else renderfx = ReadByte(m);
          }
          else {
            if (mask & 0x00040000) renderfx = ReadShort(m);
          }
          if (mask & 0x00000001) origin[0] = ReadCoord(m);
          if (mask & 0x00000002) origin[1] = ReadCoord(m);
          if (mask & 0x00000200) origin[2] = ReadCoord(m);
          if (mask & 0x00000400) angles[0] = ReadAngle(m);
          if (mask & 0x00000004) angles[1] = ReadAngle(m);
          if (mask & 0x00000008) angles[2] = ReadAngle(m);
          if (mask & 0x01000000) ReadPosition(m,old_origin);
          if (mask & 0x04000000) sound = ReadByte(m);
          event = (mask & 0x00000020) ? ReadByte(m) : 0;
          if (mask & 0x08000000) solid = ReadShort(m);
          
          /* node tree */
          tn = NULL;
         
          if (mask & MASK_DM2_PACKET_UNKNOWNBITS) 
            tn = node_link(
                   tn,
                   node_init(
                     TOKEN_UNKNOWN_MASK_BITS, 
                     bitlist_to_nodes(mask & MASK_DM2_PACKET_UNKNOWNBITS),
                     0
                   )
                 );

          tn = node_link(tn,node_command_init(TOKEN_ENTITY, V_INT, H_SB, 
                                           NODE_VALUE_INT_dup(entity),0));
          if (mask & 0x00000040) {
            tn = node_link(tn, node_init_all(TOKEN_REMOVE, H_SIMPLE, NULL, 0));
          }
          if (mask & 0x00000800) {
            cn = node_command_init(TOKEN_MODELINDEX, V_INT, H_BYTE,
                                   NODE_VALUE_INT_dup(modelindex),0);
            if (modelindex >= 1 && 
                modelindex < DM2_MAX_MODELS &&
                MODELNAME(modelindex) != NULL) 
              node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex)));
            node_add_next(tn,cn);
          }
          if (mask & 0x00100000) {
            cn = node_command_init(TOKEN_MODELINDEX2, V_INT, H_BYTE, 
                                   NODE_VALUE_INT_dup(modelindex2),0);
            if (modelindex2 >= 1 &&
                modelindex2 < DM2_MAX_MODELS &&
                MODELNAME(modelindex2) != NULL) 
                  node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex2)));
            node_add_next(tn,cn);
          }
          if (mask & 0x00200000) {
            cn = node_command_init(TOKEN_MODELINDEX3, V_INT, H_BYTE, 
                                   NODE_VALUE_INT_dup(modelindex3),0);
            if (modelindex3 >= 1 &&
                modelindex3 < DM2_MAX_MODELS &&
                MODELNAME(modelindex3) != NULL) 
                  node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex3)));
            node_add_next(tn,cn);
          }
          if (mask & 0x00400000) {
            cn = node_command_init(TOKEN_MODELINDEX4, V_INT, H_BYTE, 
                                   NODE_VALUE_INT_dup(modelindex4),0);
            if (modelindex4 >= 1 &&
                modelindex4 < DM2_MAX_MODELS &&
                MODELNAME(modelindex4) != NULL) 
                  node_add_comment(cn, NODE_VALUE_STRING_dup(MODELNAME(modelindex4)));
            node_add_next(tn,cn);
          }
          if (mask & MASK_DM2_PACKET_FRAME)
            tn = node_link(tn,node_command_init(TOKEN_FRAME, V_INT, H_SB, 
                                             NODE_VALUE_INT_dup(frame),0));
          if (mask & MASK_DM2_PACKET_SKIN) {
            tn = node_link(tn,node_command_init(TOKEN_SKIN, V_INT, H_BYTE, 
                                                NODE_VALUE_INT_dup(skin),0));
            if (vwep) 
              tn = node_link(tn,node_command_init(TOKEN_VWEP, V_INT, H_LSB, 
                                                  NODE_VALUE_INT_dup(vwep),0));
          }
          if (mask & 0x00084000)
            tn = node_link(tn,node_command_init(TOKEN_EFFECTS, V_INT, H_LSB, 
                                             NODE_VALUE_INT_dup(effects),0));
          if (mask & 0x00041000)
            tn = node_link(tn,node_command_init(TOKEN_RENDERFX, V_INT, H_LSB, 
                                             NODE_VALUE_INT_dup(renderfx),0));
          if (mask & 0x00000001)
            tn = node_link(tn,node_command_init(TOKEN_ORIGIN_X, V_FLOAT, H_COORD, 
                                             NODE_VALUE_FLOAT_dup(origin[0]),0));
          if (mask & 0x00000002)
            tn = node_link(tn,node_command_init(TOKEN_ORIGIN_Y, V_FLOAT, H_COORD, 
                                             NODE_VALUE_FLOAT_dup(origin[1]),0));
          if (mask & 0x00000200)
            tn = node_link(tn,node_command_init(TOKEN_ORIGIN_Z, V_FLOAT, H_COORD, 
                                             NODE_VALUE_FLOAT_dup(origin[2]),0));
          if (mask & 0x00000400)
            tn = node_link(tn,node_command_init(TOKEN_ANGLES_1, V_FLOAT, H_ANGLE, 
                                             NODE_VALUE_FLOAT_dup(angles[0]),0));
          if (mask & 0x00000004)
            tn = node_link(tn,node_command_init(TOKEN_ANGLES_2, V_FLOAT, H_ANGLE, 
                                             NODE_VALUE_FLOAT_dup(angles[1]),0));
          if (mask & 0x00000008)
            tn = node_link(tn,node_command_init(TOKEN_ANGLES_3, V_FLOAT, H_ANGLE, 
                                             NODE_VALUE_FLOAT_dup(angles[2]),0));
          if (mask & 0x01000000) 
            tn = node_link(tn, 
              node_triple_command_init(TOKEN_OLD_ORIGIN, V_FLOAT, H_COORD, 
                                       NODE_VALUE_FLOAT_dup(old_origin[0]),
                                       NODE_VALUE_FLOAT_dup(old_origin[1]),
                                       NODE_VALUE_FLOAT_dup(old_origin[2]),
                                       0));
          if (mask & 0x04000000) {
            cn = node_command_init(TOKEN_SOUND, V_INT, H_BYTE,
                                   NODE_VALUE_INT_dup(sound),0);
            if (sound >= 1 &&
                sound < DM2_MAX_SOUNDS &&
                SOUNDNAME(sound) != NULL) {
              node_add_comment(cn, NODE_VALUE_STRING_dup(SOUNDNAME(sound)));
            }
            node_add_next(tn,cn);
          }
          if (mask & 0x00000020) 
            tn = node_link(tn,node_command_init(TOKEN_EVENT, V_INT, H_BYTE, 
                                             NODE_VALUE_INT_dup(event),0));
          if (mask & 0x08000000) 
            tn = node_link(tn,node_command_init(TOKEN_SOLID, V_INT, H_SHORT, 
                                             NODE_VALUE_INT_dup(solid),0));
          
          ttn=node_link(ttn,node_init_all(TOKEN_PACKETENTITY, 0, tn, 0));
        }
        n=node_link(n,node_init_all(TOKEN_PACKETENTITIES, H_DM2_PACKETENTITIES, ttn, 0));
      }
      break;
      case 0x13: { /* deltapacketentities */
        for (ttn=NULL; m->p < m->end;) {
          ttn=node_link(ttn,node_init_all(V_BYTEHEX,H_SIMPLE,
                                          NODE_VALUE_BYTEHEX_dup((unsigned char)ReadByte(m)),0));
        }
        tn = NULL;
        if (ttn!=NULL)
          tn=node_link(tn,node_init_all(TOKEN_DATA,H_SIMPLE,ttn,0));
        n=node_link(n,node_init_all(TOKEN_DELTAPACKETENTITIES, H_SIMPLE, tn, 0));
      }
      break;
      case 0x14: { /* frame */
        unsigned char* p_base;
        unsigned char* p_client = 0;
        unsigned char* p_server = 0;
        unsigned char* p_relay  = 0;
        unsigned char id_client;
        unsigned char id_server;
        unsigned char id_relay;
        int rectype;
              
        int i;
        long seq1 = 0;
        long seq2 = 0 ;
        long uk_b1 = 0;
        long count = 0;
        long areas[32];
        long connected_count = 0;
        long connected[DM2_MAX_CLIENTS];
        long frame = 0;

        p_base = m->p; /* base position */
        /* all is possible and DEMOTYPE reduces it again */
        rectype = (DM2_RECORD_CLIENT|DM2_RECORD_SERVER|DM2_RECORD_RELAY) & DEMOTYPE;

        /* nothing at all */
        if (!rectype) 
          syserror(WDM2,DM2TOP->filename);

	if (rectype & DM2_RECORD_CLIENT) {
	  /* client recording is possible */
          m->p = p_base; /* back on base position */
          /* binary in: client */
          seq1 = ReadLong(m);
          seq2 = ReadLong(m);
          if (SERVERVERSION != 26) {
            uk_b1 = ReadByte(m);
          }
          count = ReadByte(m);
          if (count > 32) {
            rectype &= ~DM2_RECORD_CLIENT;
          }
          else {
            for (i=0;i<count;i++) {
              areas[i] = ReadByte(m);
            }
          }
          p_client = m->p; /* after client */
          id_client = ReadByte(m);
          if (id_client != 0x11) { /* playerinfo */
            rectype &= ~DM2_RECORD_CLIENT;
          }
        }

        if (rectype & DM2_RECORD_SERVER) {
          /* server recording is possible */
          m->p = p_base; /* back on base position */
          /* binary in: server */
          frame = ReadLong(m);
          p_server = m->p; /* after server */
          id_server = ReadByte(m);
          if (id_server != 0x12) { /* packetentities */
            rectype &= ~DM2_RECORD_SERVER;
          }
        }

	if (rectype & DM2_RECORD_RELAY) {
	  /* relay recording is possible */
          m->p = p_base; /* back on base position */
          /* binary in: relay */
          seq1 = ReadLong(m);
          seq2 = ReadLong(m);
          if (SERVERVERSION != 26) {
            uk_b1 = ReadByte(m);
          }
          count = ReadByte(m);
          if (count > 32) {
            rectype &= ~DM2_RECORD_RELAY;
          }
          else {
            for (i=0;i<count;i++) {
              areas[i] = ReadByte(m);
            }
          }
          connected_count = ReadByte(m);
          if (connected_count > 32) {
            rectype &= ~DM2_RECORD_RELAY;
          }
          else {
            for (i=0;i<connected_count;i++) {
              connected[i] = ReadByte(m);
            }
          }
          p_relay = m->p; /* after relay */
          id_relay = ReadByte(m) & ~MASK_UNICAST;
          if (id_relay != 0x11 && id_relay != 0x12) { /* playerinfo, packetentities */
            rectype &= ~DM2_RECORD_RELAY;
          }
        }

        /* node tree */
        tn = NULL;
        /* Now comes the tricky bit. There are many possible cases for rectype */
        switch (rectype) {
          case DM2_RECORD_CLIENT:                    /* sure a client */
          case DM2_RECORD_CLIENT|DM2_RECORD_SERVER:  /* may be both, I choose client */
          {
            /* point after client parsed message */
            m->p = p_client;
            
            tn = node_link(tn,node_command_init(TOKEN_SEQ1,V_INT,H_LONG,
                              NODE_VALUE_INT_dup(seq1),0));
            tn = node_link(tn,node_command_init(TOKEN_SEQ2,V_INT,H_LONG,
                              NODE_VALUE_INT_dup(seq2),0));
            if (SERVERVERSION != 26) {
              tn = node_link(tn,node_command_init(TOKEN_UK_B1, V_INT, H_BYTE, NODE_VALUE_INT_dup(uk_b1),0));
            }
            ttn = NULL;
            for (i=0;i<count;i++) {
              ttn=node_link(ttn,node_init_all(V_BYTEHEX,H_SIMPLE,
                                NODE_VALUE_BYTEHEX_dup(areas[i]),0));
            }
            tn = node_link(tn, node_init_all(TOKEN_AREAS, H_SIMPLE, ttn, 0));
            n=node_link(n,node_init_all(TOKEN_FRAME, H_DM2_FRAME, tn, 0));
          }
          break;
          case DM2_RECORD_SERVER:
          {
            /* point after server parsed message */
            m->p = p_server;
            
            n = node_link(n,node_add_hint(node_command_init(TOKEN_FRAME,V_INT,H_LONG,
                            NODE_VALUE_INT_dup(frame),0),H_SIMPLE));                  
          }
          break;
          case DM2_RECORD_RELAY:
          {
            /* point after relay parsed message */
            m->p = p_relay;
            
            tn = node_link(tn,node_command_init(TOKEN_SEQ1,V_INT,H_LONG,
                              NODE_VALUE_INT_dup(seq1),0));
            tn = node_link(tn,node_command_init(TOKEN_SEQ2,V_INT,H_LONG,
                              NODE_VALUE_INT_dup(seq2),0));
            if (SERVERVERSION != 26) {
              tn = node_link(tn,node_command_init(TOKEN_UK_B1, V_INT, H_BYTE, NODE_VALUE_INT_dup(uk_b1),0));
            }
            ttn = NULL;
            for (i=0;i<count;i++) {
              ttn=node_link(ttn,node_init_all(V_BYTEHEX,H_SIMPLE,
                                NODE_VALUE_BYTEHEX_dup(areas[i]),0));
            }
            tn = node_link(tn, node_init_all(TOKEN_AREAS, H_SIMPLE, ttn, 0));
            ttn = NULL;
            for (i=0;i<connected_count;i++) {
              ttn=node_link(ttn,node_init_all(V_BYTEHEX,H_SIMPLE,
                                NODE_VALUE_BYTEHEX_dup(connected[i]),0));
            }
            tn = node_link(tn, node_init_all(TOKEN_CONNECTED, H_SIMPLE, ttn, 0));
            n=node_link(n,node_init_all(TOKEN_FRAME, H_DM2_FRAME, tn, 0));
          }
          break;
          case 0: /* all record types are wrong, a wrong DM2 file */
          {
            syserror(WDM2,DM2TOP->filename);
          }
          break;
          default: /* shouldn't happen */
          {
            syserror(GENERR,DM2TOP->filename);
          }
          break;
        }
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
      } /* end default */
      break;
    } /* end switch */
    messagenode = node_link(messagenode,n);
  } /* end while */
  blocknode = node_init_all(TOKEN_BLOCK,0,messagenode,0);
  if (opt & 0x01) {
    sprintf(ts,"%i",DM2TOP->frame);
    node_add_comment(blocknode,NODE_VALUE_STRING_dup(ts));
  }
  return blocknode;
}

/*----------------------------------------------------------------------------*/
/* end of specific message decoding ------------------------------------------*/
/*----------------------------------------------------------------------------*/

/******************************************************************************/
/* block output: text version *************************************************/
/******************************************************************************/

node* DM2_block_write_text(node* b)
{
  /* b may be NULL (something was wrong or removed) */
  if (b == NULL) return b;
  /* type is TOKEN_BETWEENBLOCK, TOKEN_ENDBLOCK or 
     TOKEN_BLOCK with down as first message */
  if ( b->type == TOKEN_BETWEENBLOCK || b->type == TOKEN_ENDBLOCK ||
      (b->type == TOKEN_BLOCK && b->down!=NULL)) {
    node_write_text(b,0);
  }
  return b;
}

/******************************************************************************/
/* block output: binary version ***********************************************/
/******************************************************************************/

node* DM2_block_write_bin(node* b)
{
  BB_t m;
  node* tn;
  long datasize = 0;
  unicast_t unicast;

  /* b may be NULL (something was wrong or removed) */
  if (b == NULL) return b;

  /* we start from the beginning */
  m.p = m.buffer;

  /* fprintf(stderr,"block out\n"); */ 
  switch (b->type) {
    case GEN_NOTHING:
      /* empty blocks */
    break;
    case TOKEN_BETWEENBLOCK:
      /* calculate the size info */
      datasize = 4;
      m.size = 0;
      /* write the size info */
      m.p = m.buffer;
      WriteLong(&m, m.size);
    break;
    case TOKEN_ENDBLOCK:
      /* calculate the size info */
      datasize = 4;
      m.size = 0xFFFFFFFF;
      /* write the size info */
      m.p = m.buffer;
      WriteLong(&m, m.size);
    break;
    case TOKEN_BLOCK:
      tn=b->down;
      if (tn != NULL) { /* we really need something in the block */
        m.p += 4; /* after size */
        for (; tn!=NULL; tn=tn->next) {
          /* do something useful with the tn */
          /* fprintf(stderr,"m %d, h %d\n", tn->type, tn->hint); */
          if (unicast.use == UC_NEXT) 
            unicast.use = UC_THIS;
          switch(tn->hint) {
            case H_UNICAST:
              unicast.client = *(int*)tn->down->down;
              unicast.use = UC_NEXT;
            break;
            case H_NOTHING:
            break;
            case H_SIMPLE: 
              do_simple_message_write_bin_unicast(tn,&m,&unicast);
            break;
            case H_UNKNOWN:
              /* TODO: unicast does not work here */
              do_unknown_message_write_bin(tn,&m);
            break;
            case H_DM2_SOUND:
              do_dm2_sound_message_write_bin(tn,&m,&unicast);
            break;
            case H_DM2_PLAYERINFO:
              do_dm2_playerinfo_message_write_bin(tn,&m,&unicast);
            break;
            case H_DM2_SPAWNBASELINE:
              do_dm2_spawnbaseline_message_write_bin(tn,&m,&unicast);
            break;
            case H_DM2_PACKETENTITIES:
              do_dm2_packetentities_message_write_bin(tn,&m,&unicast);		
            break;
            case H_DM2_FRAME:
              do_dm2_frame_message_write_bin(tn,&m,&unicast);
            break;
            default: /* this creates a bad message */
              *m.p='\0';
              m.p++;
            break;
          } /* end switch */
          if (unicast.use == UC_THIS) 
            unicast.use = UC_NOT;
        } /* end while */
        /* calculate the size info */
        datasize = m.p - m.buffer; 
        m.size = (datasize) - 4;
        /* write the size info */
        m.p = m.buffer;
        WriteLong(&m, m.size);
      } /* end (tn->next != NULL) */
    break;
    default: 
      syserror(DM2INTE,"bad block type %d", b->type);
    break;
  } 
  
  if (datasize) { /* if there was something in the block */
    if (fwrite(m.buffer, 1, datasize, output_file) != (size_t)datasize) 
      syserror(FIWRITE, output_filename);
    if (fflush(output_file) != 0)
      syserror(errno, output_filename);
  }

  return b;
}

#define NEXT(x) (x)=(x)->next
#define CHECKRET(x) { if ((x)==NULL) { return; } }
#define NEXTRET(x) { NEXT(x) ; CHECKRET(x); }
#define NEXTGOTO(x,g) { NEXT(x) ; if ((x)==NULL) { goto g; } }

void
do_dm2_sound_message_write_bin(node* n, BB_t* m, unicast_t* u)
{
  node* cn;
  unsigned char mask;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */

  /* at first: the message id */
  if (u->use == UC_THIS) {
    WriteByte(m,node_token_id(n->type) | MASK_UNICAST);
    WriteByte(m,u->client);
  }
  else
    WriteByte(m,node_token_id(n->type));

  /* collect the mask bits */
  mask = MASK_DM2_SOUND_EMPTY;
  for (cn=n->down ; cn != NULL ; NEXT(cn)) {
    switch (cn->type) {
      case TOKEN_UNKNOWN_MASK_BITS: mask |= nodes_to_bitlist(cn->down); break;
      case TOKEN_VOL:		mask |= MASK_DM2_SOUND_VOL;		break;
      case TOKEN_ATTENUATION:	mask |= MASK_DM2_SOUND_ATTENUATION;	break;
      case TOKEN_TIMEOFS:	mask |= MASK_DM2_SOUND_TIMEOFS;		break;
      case TOKEN_ENTITY:	mask |= MASK_DM2_SOUND_ENTITY;		break;
      case TOKEN_CHANNEL:	mask |= MASK_DM2_SOUND_CHANNEL;		break;
      case TOKEN_ORIGIN:	mask |= MASK_DM2_SOUND_ORIGIN;		break;
    } /* end switch */
  } /* end for */

  /* output */
  WriteByte(m,mask); cn=n->down;
  /* unknown bits for the bit mask */
  if (cn->type == TOKEN_UNKNOWN_MASK_BITS) NEXTRET(cn);
  /* soundnum */
  do_simple_argument_write_bin(cn->down, m); NEXTRET(cn);
  if (mask & MASK_DM2_SOUND_VOL) {
    do_simple_argument_write_bin(cn->down, m); NEXTRET(cn);
  }
  if (mask & MASK_DM2_SOUND_ATTENUATION) {
    do_simple_argument_write_bin(cn->down, m); NEXTRET(cn);
  }
  if (mask & MASK_DM2_SOUND_TIMEOFS) {
    do_simple_argument_write_bin(cn->down, m); NEXTRET(cn);
  }
  if (mask & (MASK_DM2_SOUND_CHANNEL | MASK_DM2_SOUND_ENTITY)) {
    long mix = 0;
    
    if (cn->type == TOKEN_CHANNEL) {
      /* cn   is TOKEN_CHANNEL
         down is V_INT
         down is the channel itself */
      mix = (*(long*)cn->down->down) & 0x07; NEXT(cn);
    }
    if (cn->type == TOKEN_ENTITY) {
      /* cn   is TOKEN_ENTITY
         down is V_INT
         down is the entity itself */
      mix |= ((*(long*)cn->down->down) & 0x07FF) << 3; NEXT(cn);
    }
    WriteShort(m, mix);
  }
  CHECKRET(cn);
  if (mask & MASK_DM2_SOUND_ORIGIN) {
    do_simple_arguments_write_bin(cn->down, m); NEXTRET(cn);
  }
}

void
do_dm2_playerinfo_message_write_bin(node* n, BB_t* m, unicast_t* u)
{
  node *cn;
  unsigned long mask;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */

  /* at first: the message id */
  if (u->use == UC_THIS) {
    WriteByte(m,node_token_id(n->type) | MASK_UNICAST);
    WriteByte(m,u->client);
  }
  else
    WriteByte(m,node_token_id(n->type));
#define aaa(x) fprintf(stderr,"%d %d %s\n", x, cn->type, node_token_string(cn->type)); 
#define bbb(x) fprintf(stderr,"%d mask = 0x%0x\n", x, mask);
  
  /* collect the mask bits */
  mask = MASK_DM2_PLAYERINFO_EMPTY;
  for (cn=n->down ; cn != NULL ; NEXT(cn)) {
    switch (cn->type) {
      case TOKEN_PM_TYPE:	mask |= MASK_DM2_PLAYERINFO_PM_TYPE; break;
      case TOKEN_ORIGIN:	mask |= MASK_DM2_PLAYERINFO_ORIGIN; break;
      case TOKEN_VELOCITY:	mask |= MASK_DM2_PLAYERINFO_VELOCITY; break;
      case TOKEN_PM_TIME:	mask |= MASK_DM2_PLAYERINFO_PM_TIME; break;
      case TOKEN_PM_FLAGS:	mask |= MASK_DM2_PLAYERINFO_PM_FLAGS; break;
      case TOKEN_GRAVITY:	mask |= MASK_DM2_PLAYERINFO_GRAVITY; break;
      case TOKEN_DELTA_ANGLES:	mask |= MASK_DM2_PLAYERINFO_DELTA_ANGLES; break;
      case TOKEN_VIEWOFFSET:	mask |= MASK_DM2_PLAYERINFO_VIEWOFFSET; break;
      case TOKEN_VIEWANGLES:	mask |= MASK_DM2_PLAYERINFO_VIEWANGLES; break;
      case TOKEN_KICK_ANGLES:	mask |= MASK_DM2_PLAYERINFO_KICK_ANGLES; break;
      case TOKEN_GUNINDEX:	mask |= MASK_DM2_PLAYERINFO_GUNINDEX; break;
      case TOKEN_GUNFRAME:	mask |= MASK_DM2_PLAYERINFO_GUNFRAME; break;
      case TOKEN_GUNOFFSET:	mask |= MASK_DM2_PLAYERINFO_GUNOFFSET; break;
      case TOKEN_GUNANGLES:	mask |= MASK_DM2_PLAYERINFO_GUNANGLES; break;
      case TOKEN_BLEND:		mask |= MASK_DM2_PLAYERINFO_BLEND; break;
      case TOKEN_FOV:		mask |= MASK_DM2_PLAYERINFO_FOV; break;
      case TOKEN_RDFLAGS:	mask |= MASK_DM2_PLAYERINFO_RDFLAGS; break;
    } /* end switch */
  } /* end for */


  /* output */
  WriteShort(m,mask); cn=n->down;
  if (mask & MASK_DM2_PLAYERINFO_PM_TYPE) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_ORIGIN) {
    do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_VELOCITY) {
    do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_PM_TIME) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_PM_FLAGS) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_GRAVITY) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_DELTA_ANGLES) {
    do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_VIEWOFFSET) {
    do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_VIEWANGLES) {
    do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_KICK_ANGLES) {
    do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_GUNINDEX) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & (MASK_DM2_PLAYERINFO_GUNFRAME  | 
              MASK_DM2_PLAYERINFO_GUNOFFSET | 
              MASK_DM2_PLAYERINFO_GUNANGLES)  ) {
    if (cn!=NULL && cn->type == TOKEN_GUNFRAME) {
      do_simple_argument_write_bin(cn->down, m); NEXT(cn);
    }
    else {
      WriteByte(m, 0);
    }
    if (cn!=NULL && cn->type == TOKEN_GUNOFFSET) {
      do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
    }
    else {
      vec3_t gunoffset;
      int i;
      
      for (i=0;i<3;i++) gunoffset[i] = 0.0;
      WriteOffsetVec(m, gunoffset);
    }
    if (cn!=NULL && cn->type == TOKEN_GUNANGLES) {
      do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
    }
    else {
      vec3_t gunangles;
      int i;
      
      for (i=0;i<3;i++) gunangles[i] = 0.0;
      WriteOffsetVec(m, gunangles);
    }
  }
  if (mask & MASK_DM2_PLAYERINFO_BLEND) {
    do_simple_arguments_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_FOV) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  if (mask & MASK_DM2_PLAYERINFO_RDFLAGS) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  if (cn!=NULL && cn->type == TOKEN_STATS) {
    node* ccn;
    unsigned long mask2 = 0;
    
    for (ccn = cn->down ; ccn != NULL ; NEXT(ccn) ) {
      if (ccn->type != TOKEN_STAT) syserror(DM2INTE,"no stat token in stats");
      /* ccn  is TOKEN_STAT 
         down is TOKEN_INDEX
         down is V_INT
         down is the index itself */
      mask2 |= (0x00000001 << *(int*)(ccn->down->down->down));
    } /* end for */ 
    WriteLong(m, mask2);
    for (ccn = cn->down ; ccn != NULL ; NEXT(ccn) ) {
      /* ccn  is TOKEN_STAT 
         down is TOKEN_INDEX 
         next is TOKEN_VALUE
         down is V_INT
         down is the value itself */
      WriteShort(m, *(int*)(ccn->down->next->down->down));
    } /* end for */
  } /* end stats */
  else {
    /* empty stats mask */
    WriteLong(m, 0);
  }
}

int
do_dm2_packetmask_write_bin(node* n, BB_t* m)
{
  node* cn;
  unsigned long mask;
  unsigned long skin = 0;
    
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */
  
  /* collect the mask bits */
  mask = MASK_DM2_PACKET_EMPTY;
  for ( cn = n->down ; cn != NULL ; cn = cn->next ) {
    switch (cn->type) {
      case TOKEN_UNKNOWN_MASK_BITS: mask |= nodes_to_bitlist(cn->down); break;
      case TOKEN_ENTITY:
        if (*(long*)cn->down->down > 255)
          mask |= MASK_DM2_PACKET_ENTITY_SHORT;
      break;
      case TOKEN_REMOVE: mask |= MASK_DM2_PACKET_REMOVE; break;
      case TOKEN_MODELINDEX: mask |= MASK_DM2_PACKET_MODELINDEX; break;
      case TOKEN_MODELINDEX2: mask |= MASK_DM2_PACKET_MODELINDEX2; break;
      case TOKEN_MODELINDEX3: mask |= MASK_DM2_PACKET_MODELINDEX3; break;
      case TOKEN_MODELINDEX4: mask |= MASK_DM2_PACKET_MODELINDEX4; break;
      case TOKEN_FRAME:
      {
        if (*(long*)cn->down->down > 255)
          mask |= MASK_DM2_PACKET_FRAME_SHORT;
        else
          mask |= MASK_DM2_PACKET_FRAME_BYTE;
      }
      break;
      case TOKEN_SKIN:
      {
        skin = *(long*)cn->down->down;
        if (skin < 256)
          mask |= MASK_DM2_PACKET_SKIN_BYTE;
        else {
          if (skin<0x8000)
            mask |= MASK_DM2_PACKET_SKIN_SHORT;
          else
            mask |= MASK_DM2_PACKET_SKIN_LONG;
        }
      }
      break;
      case TOKEN_VWEP:
      {
        skin |= (*(long*)cn->down->down) << 8;
        /* skin is now totally different. 
           We have to clean the skin bits first */
        mask &= ~MASK_DM2_PACKET_SKIN;
        if (skin < 256)
          mask |= MASK_DM2_PACKET_SKIN_BYTE;
        else {
          if (skin<0x8000)
            mask |= MASK_DM2_PACKET_SKIN_SHORT;
          else
            mask |= MASK_DM2_PACKET_SKIN_LONG;
        }
      }
      break;
      case TOKEN_EFFECTS:
      {
        long effects = *(long*)cn->down->down;
        if (effects<256) 
          mask |= MASK_DM2_PACKET_EFFECTS_BYTE;
        else 
          if (effects<0x8000) 
            mask |= MASK_DM2_PACKET_EFFECTS_SHORT;
          else
            mask |= MASK_DM2_PACKET_EFFECTS_LONG;
      }
      break;
      case TOKEN_RENDERFX:
      {
        long renderfx = *(long*)cn->down->down;
        if (renderfx<256) 
          mask |= MASK_DM2_PACKET_RENDERFX_BYTE;
        else 
          if (renderfx<0x8000) 
            mask |= MASK_DM2_PACKET_RENDERFX_SHORT;
          else
            mask |= MASK_DM2_PACKET_RENDERFX_LONG;
      }
      break;
      case TOKEN_ORIGIN_X: mask |= MASK_DM2_PACKET_ORIGIN_X; break;
      case TOKEN_ORIGIN_Y: mask |= MASK_DM2_PACKET_ORIGIN_Y; break;
      case TOKEN_ORIGIN_Z: mask |= MASK_DM2_PACKET_ORIGIN_Z; break;
      case TOKEN_ANGLES_1: mask |= MASK_DM2_PACKET_ANGLES_1; break;
      case TOKEN_ANGLES_2: mask |= MASK_DM2_PACKET_ANGLES_2; break;
      case TOKEN_ANGLES_3: mask |= MASK_DM2_PACKET_ANGLES_3; break;
      case TOKEN_OLD_ORIGIN: mask |= MASK_DM2_PACKET_OLD_ORIGIN; break;
      case TOKEN_SOUND: mask |= MASK_DM2_PACKET_SOUND; break;
      case TOKEN_EVENT: mask |= MASK_DM2_PACKET_EVENT; break;
      case TOKEN_SOLID: mask |= MASK_DM2_PACKET_SOLID; break;
      default: syserror(DM2INTE,"wrong command %d in packet",cn->type);
    } /* end switch cn->type */
  } /* end for cn */
  
  /* calculate the used mask length */
  if (mask & MASK_DM2_PACKET_PART4_USED)
    mask |= MASK_DM2_PACKET_PART2 | MASK_DM2_PACKET_PART3 | MASK_DM2_PACKET_PART4;
  else {
    if (mask & MASK_DM2_PACKET_PART3_USED)
      mask |= MASK_DM2_PACKET_PART2 | MASK_DM2_PACKET_PART3;
    else {
      if (mask & MASK_DM2_PACKET_PART2_USED)
        mask |= MASK_DM2_PACKET_PART2;
    }  
  }

  /* write the mask */
  WriteByte(m, 0x000000FF & mask);
  if (mask & MASK_DM2_PACKET_PART2) WriteByte(m, 0x000000FF & (mask >>  8));
  if (mask & MASK_DM2_PACKET_PART3) WriteByte(m, 0x000000FF & (mask >> 16));
  if (mask & MASK_DM2_PACKET_PART4) WriteByte(m, 0x000000FF & (mask >> 24));

  /*
  if (skin) {
    printf("mask=0x%08lx, skin=%li\n",mask,skin);
  }
  */

  /* return it, so we can simpler continue with the data */
  return mask;
}


#ifdef aaa
#undef aaa
#endif
#define aaa(a)
/* #define aaa(a) { fprintf(stderr,"%d ", a); } */

void
do_dm2_packetdata_write_bin(node* n, BB_t* m, unsigned long mask)
{
  node* cn;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */
  
  cn=n->down;

  /* unknown bits for the bit mask */
  if (cn->type == TOKEN_UNKNOWN_MASK_BITS) NEXT(cn);
  /* write the entity */
  /* fprintf(stderr,"entity=%d\n", *(long*)cn->down->down); */
  aaa(1);
  /* fprintf(stderr,"enthint=%d\n",cn->down->hint); */
  do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  aaa(2);
  if (mask & MASK_DM2_PACKET_REMOVE) {
    NEXT(cn);
  }
  if (mask & MASK_DM2_PACKET_MODELINDEX) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  aaa(3);
  if (mask & MASK_DM2_PACKET_MODELINDEX2) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  aaa(4);
  if (mask & MASK_DM2_PACKET_MODELINDEX3) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  aaa(5);
  if (mask & MASK_DM2_PACKET_MODELINDEX4) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  aaa(6);
  if (mask & (MASK_DM2_PACKET_FRAME)) {
    do_simple_argument_write_bin(cn->down, m); NEXT(cn);
  }
  aaa(7);
  if (mask & (MASK_DM2_PACKET_SKIN)) {
    /* cn is skin */
    long skin = *(long*)cn->down->down;
    NEXT(cn);
    
    if (cn != NULL && cn->type == TOKEN_VWEP) {
      long vwep = *(long*)cn->down->down;
      NEXT(cn);
      skin |= vwep << 8;
    }
    
    if ((mask & MASK_DM2_PACKET_SKIN_LONG) == MASK_DM2_PACKET_SKIN_LONG)
      WriteLong(m, skin);
    else {
      if (mask & MASK_DM2_PACKET_SKIN_SHORT) WriteShort(m, skin);
      if (mask & MASK_DM2_PACKET_SKIN_BYTE) WriteByte(m, skin);
    }
  }
  if (mask & (MASK_DM2_PACKET_EFFECTS_LONG|MASK_DM2_PACKET_EFFECTS_SHORT|MASK_DM2_PACKET_EFFECTS_BYTE)) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & (MASK_DM2_PACKET_RENDERFX_LONG|MASK_DM2_PACKET_RENDERFX_SHORT|MASK_DM2_PACKET_RENDERFX_BYTE)) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_ORIGIN_X) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_ORIGIN_Y) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_ORIGIN_Z) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_ANGLES_1) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_ANGLES_2) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_ANGLES_3) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_OLD_ORIGIN) {
    do_simple_arguments_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_SOUND) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_EVENT) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (mask & MASK_DM2_PACKET_SOLID) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
}

void
do_dm2_spawnbaseline_message_write_bin(node* n, BB_t* m, unicast_t* u)
{
  unsigned long mask;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */

  /* at first: the message id */
  if (u->use == UC_THIS) {
    WriteByte(m,node_token_id(n->type) | MASK_UNICAST);
    WriteByte(m,u->client);
  }
  else
    WriteByte(m,node_token_id(n->type));
  
  mask = do_dm2_packetmask_write_bin(n, m);
  do_dm2_packetdata_write_bin(n,m,mask);
}

void
do_dm2_packetentities_message_write_bin(node* n, BB_t* m, unicast_t* u)
{
  node* cn;
  unsigned long mask;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */

  /* at first: the message id */
  if (u->use == UC_THIS) {
    WriteByte(m,node_token_id(n->type) | MASK_UNICAST);
    WriteByte(m,u->client);
  }
  else
    WriteByte(m,node_token_id(n->type));

  for (cn = n->down ; cn != NULL ; cn = cn->next ) {
    mask = do_dm2_packetmask_write_bin(cn,m);
    /* fprintf(stderr,"mask=0x%0x\n", mask); */
    do_dm2_packetdata_write_bin(cn,m,mask);
  }
  WriteByte(m,0); /* empty mask */
  WriteByte(m,0); /* empty entity as the end mark */
}

void
do_dm2_frame_message_write_bin(node* n, BB_t* m, unicast_t* u)
{
  node *cn;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.
  */

  /* at first: the message id */
  if (u->use == UC_THIS) {
    WriteByte(m,node_token_id(n->type) | MASK_UNICAST);
    WriteByte(m,u->client);
  }
  else
    WriteByte(m,node_token_id(n->type));

  cn = n->down;

  /* client recording only */
  /* seq1 */
  do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  /* seq2 */
  do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  /* uk_b1 */
  if (cn->type == TOKEN_UK_B1) {
    do_simple_argument_write_bin(cn->down, m); cn=cn->next;
  }
  if (cn == NULL) return;
  /* areas */
  if (cn->type == TOKEN_AREAS) {
    /* count at the beginning */
    {
      node* c = cn->down;
      int i;
      
      for (i=0 ; c!=NULL ; c=c->next, i++) ;
      WriteByte(m, i);
    }
    do_simple_arguments_write_bin(cn->down, m); cn=cn->next;
  }
  else { /* no areas entry at all */
    WriteByte(m, 0);
  }
  if (cn == NULL) return;
  /* connected */
  if (cn->type == TOKEN_CONNECTED) {
    /* count at the beginning */
    {
      node* c = cn->down;
      int i;
      
      for (i=0 ; c!=NULL ; c=c->next, i++) ;
      WriteByte(m, i);
    }
    do_simple_arguments_write_bin(cn->down, m); cn=cn->next;
  }
  else { /* no connected entry at all */
    WriteByte(m, 0);
  }
}  



/******************************************************************************/
/** DM2 block editing *********************************************************/
/******************************************************************************/

void DM2_block_edit(node *n)
{
  node *ni;
  opt_t* opt;

  opt = glob_opt;

  /* block manipulation */
  if (opt->option & opFixMulti) {
    /* it repairs the multi-level play-back problem */
    
    START_opFixMulti:
    if (n->type == TOKEN_BETWEENBLOCK) {
      /* it should be NULL anyway but it doesnt hurt */
      node_delete(n->down);
      n->down = NULL;
      n->type = GEN_NOTHING;
      n->hint = H_NOTHING;
      /* go to the next option */
      /* goto END_opFixMulti; */
      return;
    }

    for (ni=n->down;ni!=NULL;ni=ni->next) {
      /* is this message important ? */
      if (ni->type==TOKEN_STUFFTEXT) {
        /* ni->down is V_STRING, ni->down->down is the string itself */
        if ((strcmp ((char*)(ni->down->down),"reconnect\n"         )==0) ||
            (strncmp((char*)(ni->down->down),"cmd configstrings",17)==0) ||
            (strncmp((char*)(ni->down->down),"cmd baselines"    ,13)==0) ||
            (strncmp((char*)(ni->down->down),"record "          , 7)==0)) {
          /* the whole ni must disappear */
          /* unlink this node, correct the start */
          n->down = node_unlink(n->down,ni);
          /* remove the node */
          node_delete(ni);
          /* do it again */
          /* to contine from here is possible but totally confusing */
          goto START_opFixMulti;
        } /* end if strcmp */
        if (strncmp((char*)(ni->down->down),"precache ",9) == 0) {
          /* the stuff after precache has to disappear */
          free(ni->down->down);
          ni->down->down = (node*)NODE_VALUE_STRING_dup("precache\n");
          /* we continue right here, no need to goto restart */
        }
      } /* end if type */
      if (ni->type == TOKEN_SERVERDATA) {
        long* nt;
        /*
           down is TOKEN_SERVERVERSION
           down->next is TOKEN_KEY
           down->next->next is TOKEN_ISDEMO
           down->next->next->down V_INT
           down->next->next->down->down is the actual value pointer 
        */
        if ((*(nt = (long*)(ni->down->next->next->down->down))) == 0) *nt = 1;
      }
      if (ni->type == TOKEN_RECONNECT ||
          ni->type == TOKEN_DOWNLOAD) {
        /* the whole ni must disappear */
        /* unlink this node, correct the start */
        n->down = node_unlink(n->down,ni);
        /* remove the node */
        node_delete(ni);
        /* do it again */
        /* to contine from here is possible but totally confusing */
        goto START_opFixMulti;
      }
    } /* end for ni */
  } /* end opFixMulti */

  /*
  nothing comes after this, so 
  "ANSI C forbids label at end of compound statement"
  END_opFixMulti:
  */
  
  /* other optional manipulations may come here */
  
}


/*- file end udm2.c ---------------------------------------------------------*/
