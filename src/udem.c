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
|  udem.c - implementation, general DEM routines                             |
\****************************************************************************/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "tools.h"
#include "record.h"
#include "udem.h"


token_t DEM_token[]={
        { "",                   GEN_NOTHING, 0},

	{ "track",              TOKEN_TRACK, 0 },
	{ "block",              TOKEN_BLOCK, 0 },

	{ "camera",             TOKEN_CAMERA, 0 },

	{ "unknown",            TOKEN_UNKNOWN, 0 },

	{ "bad",                TOKEN_BAD, c_bad },
	{ "nop",                TOKEN_NOP, c_nop },
	{ "disconnect",         TOKEN_DISCONNECT, c_disconnect },
	{ "updatestat",         TOKEN_UPDATESTAT, c_updatestat },
	{ "version",            TOKEN_VERSION, c_version },
	{ "setview",            TOKEN_SETVIEW, c_setview },
	{ "sound",              TOKEN_SOUND, c_sound },
	{ "time",               TOKEN_TIME, c_time },
	{ "print",              TOKEN_PRINT, c_print },
	{ "stufftext",          TOKEN_STUFFTEXT, c_stufftext },
	{ "setangle",           TOKEN_SETANGLE, c_setangle },
	{ "serverinfo",         TOKEN_SERVERINFO, c_serverinfo },
	{ "lightstyle",         TOKEN_LIGHTSTYLE, c_lightstyle },
	{ "updatename",         TOKEN_UPDATENAME, c_updatename },
	{ "updatefrags",        TOKEN_UPDATEFRAGS, c_updatefrags },
	{ "clientdata",         TOKEN_CLIENTDATA, c_clientdata },
	{ "stopsound",          TOKEN_STOPSOUND, c_stopsound },
	{ "updatecolors",       TOKEN_UPDATECOLORS, c_updatecolors },
	{ "particle",           TOKEN_PARTICLE, c_particle },
	{ "damage",             TOKEN_DAMAGE, c_damage },
	{ "spawnstatic",        TOKEN_SPAWNSTATIC, c_spawnstatic },
	{ "spawnbinary",        TOKEN_SPAWNBINARY, c_spawnbinary },
	{ "spawnbaseline",      TOKEN_SPAWNBASELINE, c_spawnbaseline },
	{ "temp_entity",        TOKEN_TEMP_ENTITY, c_temp_entity },
	{ "setpause",           TOKEN_SETPAUSE, c_setpause },
	{ "signonnum",          TOKEN_SIGNONNUM, c_signonnum },
	{ "centerprint",        TOKEN_CENTERPRINT, c_centerprint },
	{ "killedmonster",      TOKEN_KILLEDMONSTER, c_killedmonster },
	{ "foundsecret",        TOKEN_FOUNDSECRET, c_foundsecret },
	{ "spawnstaticsound",   TOKEN_SPAWNSTATICSOUND, c_spawnstaticsound },
	{ "intermission",       TOKEN_INTERMISSION, c_intermission },
	{ "finale",             TOKEN_FINALE, c_finale },
	{ "cdtrack",            TOKEN_CDTRACK, c_cdtrack },
	{ "sellscreen",         TOKEN_SELLSCREEN, c_sellscreen },
	{ "cutscene",		TOKEN_CUTSCENE, c_cutscene },
	{ "updateentity",       TOKEN_UPDATEENTITY, 0 },

	{ "id",                 TOKEN_ID, 0 },
	{ "data",               TOKEN_DATA, 0 },
	{ "index",              TOKEN_INDEX, 0 },
	{ "value",              TOKEN_VALUE, 0 },
	{ "vol",                TOKEN_VOL, 0 },
	{ "attenuation",        TOKEN_ATTENUATION, 0 },
	{ "channel",            TOKEN_CHANNEL, 0 },
	{ "entity",             TOKEN_ENTITY, 0 },
	{ "soundnum",           TOKEN_SOUNDNUM, 0 },
	{ "origin",             TOKEN_ORIGIN, 0 },
	{ "serverversion",      TOKEN_SERVERVERSION, 0 },
	{ "maxclients",         TOKEN_MAXCLIENTS, 0 },
	{ "multi",              TOKEN_MULTI, 0 },
	{ "mapname",            TOKEN_MAPNAME, 0 },
	{ "models",             TOKEN_MODELS, 0 },
	{ "sounds",             TOKEN_SOUNDS, 0 },
	{ "style",              TOKEN_STYLE, 0 },
	{ "string",             TOKEN_STRING, 0 },
	{ "player",             TOKEN_PLAYER, 0 },
	{ "netname",            TOKEN_NETNAME, 0 },
	{ "frags",              TOKEN_FRAGS, 0 },
	{ "view_ofs_z",         TOKEN_VIEW_OFS_Z, 0 },
	{ "punchangle_x",       TOKEN_PUNCHANGLE_X, 0 },
	{ "angles_1",           TOKEN_ANGLES_1, 0 },
	{ "angles_2",           TOKEN_ANGLES_2, 0 },
	{ "angles_3",           TOKEN_ANGLES_3, 0 },
	{ "vel_x",              TOKEN_VEL_X, 0 },
	{ "vel_y",              TOKEN_VEL_Y, 0 },
	{ "vel_z",              TOKEN_VEL_Z, 0 },
	{ "weapons",            TOKEN_WEAPONS, 0 },
	{ "ammotype",           TOKEN_AMMOTYPE, 0 },
	{ "armortype",          TOKEN_ARMORTYPE, 0 },
	{ "megahealth",         TOKEN_MEGAHEALTH, 0},
	{ "keys",               TOKEN_KEYS, 0 },
	{ "power_ups",          TOKEN_POWER_UPS, 0 },
	{ "uk_items_b23",       TOKEN_UK_ITEMS_B23, 0 },
	{ "uk_items_b24",       TOKEN_UK_ITEMS_B24, 0 },
	{ "uk_items_b25",       TOKEN_UK_ITEMS_B25, 0 },
	{ "uk_items_b26",       TOKEN_UK_ITEMS_B26, 0 },
	{ "uk_items_b27",       TOKEN_UK_ITEMS_B27, 0 },
	{ "runes",              TOKEN_RUNES, 0 },
	{ "uk_bit_b10",         TOKEN_UK_BIT_B10, 0 },
	{ "uk_bit_b11",         TOKEN_UK_BIT_B11, 0 },
	{ "weaponframe",        TOKEN_WEAPONFRAME, 0 },
	{ "armorvalue",         TOKEN_ARMORVALUE, 0 },
	{ "weaponmodel",        TOKEN_WEAPONMODEL, 0 },
	{ "health",             TOKEN_HEALTH, 0 },
	{ "currentammo",        TOKEN_CURRENTAMMO, 0 },
	{ "ammo_shells",        TOKEN_AMMO_SHELLS, 0 },
	{ "ammo_nails",         TOKEN_AMMO_NAILS, 0 },
	{ "ammo_rockets",       TOKEN_AMMO_ROCKETS, 0 },
	{ "ammo_cells",         TOKEN_AMMO_CELLS, 0 },
	{ "weapon",             TOKEN_WEAPON, 0 },
	{ "colors",             TOKEN_COLORS, 0 },
	{ "vel",                TOKEN_VEL, 0 },
	{ "color",              TOKEN_COLOR, 0 },
	{ "count",              TOKEN_COUNT, 0 },
	{ "save",               TOKEN_SAVE, 0 },
	{ "take",               TOKEN_TAKE, 0 },
	{ "default_modelindex", TOKEN_DEFAULT_MODELINDEX, 0 },
	{ "default_frame",      TOKEN_DEFAULT_FRAME, 0 },
	{ "default_colormap",   TOKEN_DEFAULT_COLORMAP, 0 },
	{ "default_skin",       TOKEN_DEFAULT_SKIN, 0 },
	{ "default_origin",     TOKEN_DEFAULT_ORIGIN, 0 },
	{ "default_angles",     TOKEN_DEFAULT_ANGLES, 0 },
	{ "entitytype",         TOKEN_ENTITYTYPE, 0 },
	{ "trace_endpos",       TOKEN_TRACE_ENDPOS, 0 },
	{ "range",		TOKEN_RANGE, 0 },
	{ "fromtrack",          TOKEN_FROMTRACK, 0 },
	{ "totrack",            TOKEN_TOTRACK, 0 },
	{ "modelindex",         TOKEN_MODELINDEX, 0 },
	{ "frame",              TOKEN_FRAME, 0 },
	{ "colormap",           TOKEN_COLORMAP, 0 },
	{ "skin",               TOKEN_SKIN, 0 },
	{ "effects",            TOKEN_EFFECTS, 0 },
	{ "origin_x",           TOKEN_ORIGIN_X, 0 },
	{ "origin_y",           TOKEN_ORIGIN_Y, 0 },
	{ "origin_z",           TOKEN_ORIGIN_Z, 0 },
	{ "new",                TOKEN_NEW, 0 },
		
        { NULL, 0, 0}
};

/******************************************************************************/
/** DEM ***********************************************************************/
/******************************************************************************/

long analyzeDEM(char *filename, size_t *datastart, long *tics);

void DEM_init(DEM_t* d, char *filename, char *mode)
{
  struct stat buf;
 
  if ((d->filename = strdup(filename))==NULL) syserror(errno,"strdup");
  if (strcmp(d->filename,"-")==0) {
    d->file=stdout;
  }
  else {
    if ((d->file=fopen(d->filename, mode))==NULL) syserror(errno,d->filename);
    if (stat(d->filename,&buf)==-1) syserror(errno,d->filename);
    d->filesize=buf.st_size;
  }
  d->nummodels = 0;
  d->precache_models=NULL;
  d->numsounds = 0;
  d->precache_sounds=NULL;
  d->lastHealth = 100;
  d->lastArmor = 0;
  d->lastMinusHealth = 0;
  d->lastMinusArmor = 0;
  d->game = QUAKE;

  /* we need a rewind here because some fopen modes may seek at the end */
  rewind(d->file);

  /* don't check output files or pipes */
  if (strchr(mode,'w') || buf.st_mode & S_IFIFO) {
    d->datasize = 0;
  }
  else {
    if (analyzeDEM(d->filename, &(d->datastart), &(d->tics)) < 0)
      syserror(errno, d->filename);
    d->datasize = d->filesize - d->datastart;
  } 
}

void DEM_prepare_write_bin(DEM_t* d)
{
  output_file = d->file;
  output_filename = d->filename;
}

void DEM_open(DEM_t *d, char *mode)
{
  if ((d->file=fopen(d->filename, mode))==NULL) syserror(errno,d->filename);
}

void DEM_open_sure(DEM_t *d, char *mode)
{
  if (d->file==NULL) DEM_open(d,mode);
}

void DEM_close(DEM_t *d)
{
  if (fclose(d->file)) syserror(errno,d->filename);
  d->file=NULL;
}

node* do_track_read_bin(DEM_t* d)
{
  node *n;
  char buffer[MAX_TRACK_SIZE+1];

  rewind(d->file);
  if (d->datastart) {
    if (fread(buffer,1,d->datastart,d->file)!=d->datastart) syserror(FIREAD,d->filename);
  }   
  buffer[d->datastart]='\0';
  fseek(d->file, d->datastart, SEEK_SET); 

  n=node_add_hint(node_command_init(TOKEN_TRACK,V_STRING,H_STRING,NODE_VALUE_STRING_dup(buffer),0),
                  H_SIMPLE); 
  if (strcmp(buffer,"-1\n")==0) node_add_comment(n,NODE_VALUE_STRING_dup("default track"));
  if (!d->datastart) node_add_comment(n,NODE_VALUE_STRING_dup("warning: empty CD track"));
  return n;
}

void DEM_block_read_bin(DEM_t* d, BB_t* m)
{
  m->base = ftell(d->file);
  if (fread(m->buffer,1,16,d->file)!=16) syserror(FIREAD,d->filename);
  m->p = m->buffer;
  m->end = &(m->buffer[16]);
  m->size = ReadLong (m);
  if (m->size>DEM_MAX_MESSAGE_SIZE) syserror(WDEM,d->filename);
  if (fread(m->buffer+16,1,m->size,d->file)!=(size_t)m->size) syserror(FIREAD,d->filename);
  m->size += 16; /* total size */
  m->p = m->buffer;
  m->end = &(m->buffer[m->size]);
  m->top = (void*) d;
}

node* DEM_bin_to_node(BB_t* m, int opt)
{
  node *n, *tn;
  char ts[100];
  float ca1, ca2, ca3;
  /* long dummy; */
  
  /* dummy = */ ReadLong(m); /* size is not important any more */
  ca1   = ReadFloat(m);
  ca2   = ReadFloat(m);
  ca3   = ReadFloat(m);
  n=node_triple_command_init(TOKEN_CAMERA,V_FLOAT,H_FLOAT,
                             NODE_VALUE_FLOAT_dup(ca1),
                             NODE_VALUE_FLOAT_dup(ca2),
                             NODE_VALUE_FLOAT_dup(ca3),
                             0);
  while (m->p < m->end) {
    if (opt & 0x01) {
      tn=node_init_all(GEN_NOTHING,H_NOTHING,NULL,0);
      sprintf(ts, "0x%08lX", m->base + (m->p-m->buffer));
      node_add_comment(tn,NODE_VALUE_STRING_dup(ts));
      node_add_next(n,tn);
    }
    node_add_next(n,do_message_read_bin(m));
  }
  n=node_init_all(TOKEN_BLOCK,0,n,0);
  if (opt & 0x01) { 
    sprintf(ts,"%i",((DEM_t*)(m->top))->frame);
    node_add_comment(n,NODE_VALUE_STRING_dup(ts)); 
  }
  return n;
}

node* do_message_read_bin(BB_t* m)
{
  unsigned char id;
  node* n=NULL;

  id = ReadByte(m);
  switch (id) {
    /* 0x00 */
    case c_bad:              n=do_bad_message_read_bin();                break;
    /* 0x01 */
    case c_nop:              n=do_nop_message_read_bin();                break;
    /* 0x02 */
    case c_disconnect:       n=do_disconnect_message_read_bin();         break;
    /* 0x03 */
    case c_updatestat:       n=do_updatestat_message_read_bin(m);        break;
    /* 0x04 */
    case c_version:          n=do_version_message_read_bin(m);           break;
    /* 0x05 */
    case c_setview:          n=do_setview_message_read_bin(m);           break;
    /* 0x06 */
    case c_sound:            n=do_sound_message_read_bin(m);             break;
    /* 0x07 */
    case c_time:             n=do_time_message_read_bin(m);              break;
    /* 0x08 */
    case c_print:            n=do_print_message_read_bin(m);             break;
    /* 0x09 */
    case c_stufftext:        n=do_stufftext_message_read_bin(m);         break;
    /* 0x0A */
    case c_setangle:         n=do_setangle_message_read_bin(m);          break;
    /* 0x0B */
    case c_serverinfo:       n=do_serverinfo_message_read_bin(m);        break; 
    /* 0x0C */
    case c_lightstyle:       n=do_lightstyle_message_read_bin(m);        break;
    /* 0x0D */
    case c_updatename:       n=do_updatename_message_read_bin(m);        break;
    /* 0x0E */
    case c_updatefrags:      n=do_updatefrags_message_read_bin(m);       break;
    /* 0x0F */
    case c_clientdata:       n=do_clientdata_message_read_bin(m);        break;
    /* 0x10 */
    case c_stopsound:        n=do_stopsound_message_read_bin(m);         break;
    /* 0x11 */
    case c_updatecolors:     n=do_updatecolors_message_read_bin(m);      break;
    /* 0x12 */
    case c_particle:         n=do_particle_message_read_bin(m);          break;
    /* 0x13 */
    case c_damage:           n=do_damage_message_read_bin(m);            break;
    /* 0x14 */
    case c_spawnstatic:      n=do_spawnstatic_message_read_bin(m);       break;
    /* 0x15 */
    case c_spawnbinary:      n=do_spawnbinary_message_read_bin();        break;
    /* 0x16 */
    case c_spawnbaseline:    n=do_spawnbaseline_message_read_bin(m);     break;
    /* 0x17 */
    case c_temp_entity:      n=do_temp_entity_message_read_bin(m);       break;
    /* 0x18 */
    case c_setpause:         n=do_setpause_message_read_bin(m);          break;
    /* 0x19 */
    case c_signonnum:        n=do_signonnum_message_read_bin(m);         break;
    /* 0x1A */
    case c_centerprint:      n=do_centerprint_message_read_bin(m);       break;
    /* 0x1B */
    case c_killedmonster:    n=do_killedmonster_message_read_bin();      break;
    /* 0x1C */
    case c_foundsecret:      n=do_foundsecret_message_read_bin();        break;
    /* 0x1D */
    case c_spawnstaticsound: n=do_spawnstaticsound_message_read_bin(m);  break;
    /* 0x1E */
    case c_intermission:     n=do_intermission_message_read_bin();       break;
    /* 0x1F */
    case c_finale:           n=do_finale_message_read_bin(m);            break;
    /* 0x20 */
    case c_cdtrack:          n=do_cdtrack_message_read_bin(m);           break;
    /* 0x21 */
    case c_sellscreen:       n=do_sellscreen_message_read_bin() ;        break;
    /* 0x22 */
    case c_cutscene:         n=do_cutscene_message_read_bin(m);          break;
    default: if (id>=0x80) 
               n=do_updateentity_message_read_bin(m,id & 0x7F);
             else 
               n=do_unknown_message_read_bin(m,id); 
             break;
  }
  return n;
}

void DEM_done(DEM_t* d)
{
  DEM_close(d);
  free(d->filename);
}

/*----------------------------------------------------------------------------*/
/*- end of DEM ---------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* check, if file is DEM */
unsigned long isDEMbin(char *filename)
{
  size_t datastart;
  long tics;

  if (analyzeDEM(filename, &datastart, &tics)<0) 
    return 0;
  else
    return tics;
}

long analyzeDEM(char *filename, size_t *datastart, long *tics)
{
  FILE *file;
  struct stat buf;

  long areDEMblocks(FILE *file, long fromoffset, long tooffset);
    
  /*
   * TODO:
   * look the filename up in the list 
   */
   
  if ((file=fopen(filename, "rb"))==NULL)
    return -1;
  if (stat(filename,&buf)==-1)
    return -1;
  for (*datastart=0, *tics=-1 ; *datastart<MAX_TRACK_SIZE ; (*datastart)++) 
    if ((*tics = areDEMblocks(file, *datastart, buf.st_size)) >= 0) 
      break;
  fclose(file);
  if (*tics<0)
    return -1;

  /* 
   * TODO:
   * to speed up things: 
   * memorize the filename/datastart/tics combination
   */

  return 1;
}

#define NOTDEM { fseek(file, oldoffset, SEEK_SET); return -1; }

long areDEMblocks(FILE *file, long fromoffset, long tooffset)
{
  long oldoffset;
  unsigned long tics;
  unsigned char buffer[10];
  long blocksize;
    
  oldoffset = ftell(file);
  
  if (fseek(file, fromoffset, SEEK_SET) != 0) {
    /* fprintf(stderr, "couldn't seek to the start\n"); */
    NOTDEM
  }
  
  tics=0;
  while (ftell(file)<tooffset) {
    /* fprintf(stderr, "p=0x%08lx", ftell(file)); */
    tics++;
    if (fread(buffer,1,4,file)!=4) {
      /* fprintf(stderr, "couldn't read the blocksize\n"); */
      NOTDEM
    }
    /* endian-dependent */
    /* blocksize = *(unsigned long *)(buffer); */
    /* endian independent */
    blocksize = ((buffer[0])    ) +
                ((buffer[1])<< 8) +
                ((buffer[2])<<16) +
                ((buffer[3])<<24);
    /* fprintf(stderr,",b=%ld\n", blocksize); */
    if (blocksize>DEM_MAX_MESSAGE_SIZE) {
      /* fprintf(stderr,"bs too big\n"); */
      NOTDEM
    }
    if (fseek(file, 12+blocksize, SEEK_CUR)!=0) {
      /* fprintf(stderr,"impossible seek"); */
      NOTDEM
    }  
  } 
  if (ftell(file)!=tooffset) {
    /* fprintf(stderr, "not at the tooffset position after all\n"); */
    NOTDEM
  }    

  fseek(file, oldoffset, SEEK_SET);
  return tics;
}

/* check, if file if DEM source */
int isDEMtxt(char* filename)
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
      case 0: if (strncmp (&(buffer[p]), "track", 5)==0) return 1;
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
  /* If it comes to this line it may be a DEM txt file. */
  return 0;
}

/******************************************************************************/
/* decode a specific message **************************************************/
/******************************************************************************/

/* unknown -------------------------------------------------------------------*/
node* do_unknown_message_read_bin(BB_t* m, unsigned char id)
{
  node *n, *tn;

  n=node_command_init(TOKEN_ID,V_BYTEHEX,H_SIMPLE,NODE_VALUE_BYTEHEX_dup(id),0);
  if (m->p < m->end) {
    for (tn=NULL; m->p < m->end;) {
      tn=node_link(tn,node_init_all(V_BYTEHEX,H_SIMPLE,NODE_VALUE_BYTEHEX_dup((unsigned char)ReadByte(m)),0));
    }
    node_add_next(n,node_init_all(TOKEN_DATA,H_SIMPLE,tn,0));
  }
  return node_add_comment(node_init_all(TOKEN_UNKNOWN,H_UNKNOWN,n,0),NODE_VALUE_STRING_dup("something is wrong"));
}

/* 0x00 ----------------------------------------------------------------------*/
node* do_bad_message_read_bin(void)
{
  node* n;

  /* construct node tree and return the root of it */
  n=node_init_all(TOKEN_BAD,H_SIMPLE,NULL,0);
  return node_add_comment(n,NODE_VALUE_STRING_dup("something is wrong")); 
}

/* 0x01 ----------------------------------------------------------------------*/
node* do_nop_message_read_bin(void)
{
  /* construct node tree and return the root of it */
  return node_init_all(TOKEN_NOP,H_SIMPLE,NULL,0);
}

/* 0x02 ----------------------------------------------------------------------*/
node* do_disconnect_message_read_bin(void)
{ 
  /* construct node tree and return the root of it */
  return node_init_all(TOKEN_DISCONNECT,H_SIMPLE,NULL,0);
}

/* 0x03 ----------------------------------------------------------------------*/
node* do_updatestat_message_read_bin(BB_t* m)
{
  node *n;

  /* variables */
  long index;
  long value;

  /* binary in */
  index = ReadByte(m);
  value = ReadLong(m);

  /* construct node tree and return the root of it */
  n = node_add_next (
    node_command_init(TOKEN_INDEX,V_INT,H_BYTE,NODE_VALUE_INT_dup(index),0),
    node_command_init(TOKEN_VALUE,V_INT,H_LONG,NODE_VALUE_INT_dup(value),0)
  );
  return node_init_all(TOKEN_UPDATESTAT,H_SIMPLE,n,0);
}

/* 0x04 ----------------------------------------------------------------------*/
node* do_version_message_read_bin(BB_t* m)
{
  /* variables */
  long version;

  /* binary in */
  version = ReadLong(m);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_VERSION,V_INT,H_LONG,NODE_VALUE_INT_dup(version),0),
                       H_SIMPLE);
}

/* 0x05 ----------------------------------------------------------------------*/
node* do_setview_message_read_bin(BB_t* m)
{
  /* variables */
  long entity;

  /* binary in */
  entity = ReadShort(m);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_SETVIEW,V_INT,H_SHORT,NODE_VALUE_INT_dup(entity),0),
                       H_SIMPLE);
}

#define DEMTOP ((DEM_t*)(m->top))

/* 0x06 ----------------------------------------------------------------------*/
node* do_sound_message_read_bin(BB_t* m)
{
  node *n, *tn;

  /* variables */
  long mask;
  float vol;
  float attenuation;
  long channel;
  long entity;
  long soundnum;
  vec3_t origin;

  int i;

  /* binary in */
  long entity_channel; /* combined variable */

  /* init */
  vol = attenuation = 0.0;

  mask = ReadByte(m);
  if (mask & 0x01) vol = (float) ReadByte(m) / 255.0;
  if (mask & 0x02) attenuation = (float) ReadByte(m) / 64.0;
  entity_channel = ReadShort(m);
  channel = entity_channel & 0x07;
  entity = (entity_channel >> 3) & 0x1FFF;
  soundnum = ReadByte(m);
  for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);

  /* construct node tree and return the root of it */
  n = NULL;
  if (mask & 0x01) {
    n = node_link(n,node_command_init(TOKEN_VOL,V_FLOAT,H_VOL,NODE_VALUE_FLOAT_dup(vol),0));
  }
  if (mask & 0x02) {
    n = node_link(n,node_command_init(TOKEN_ATTENUATION,V_FLOAT,H_ATTENUATION,NODE_VALUE_FLOAT_dup(attenuation),0));
  }
  n=node_link(n,node_command_init(TOKEN_CHANNEL,V_INT,0,NODE_VALUE_INT_dup(channel),0));
  n=node_link(n,node_command_init(TOKEN_ENTITY,V_INT,0,NODE_VALUE_INT_dup(entity),0));
  tn = node_command_init(TOKEN_SOUNDNUM,V_INT,H_BYTE,NODE_VALUE_INT_dup(soundnum),0);
  if (soundnum >= 1 && 
      soundnum <= DEMTOP->numsounds) {
    node_add_comment(tn,NODE_VALUE_STRING_dup(DEMTOP->precache_sounds[soundnum]));
  }
  n=node_link(n,tn);
  n=node_link(n,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                  NODE_VALUE_FLOAT_dup(origin[0]),NODE_VALUE_FLOAT_dup(origin[1]),NODE_VALUE_FLOAT_dup(origin[2]),0));
  return node_init_all(TOKEN_SOUND,H_DEM_SOUND,n,0);
}

/* 0x07 ----------------------------------------------------------------------*/
node* do_time_message_read_bin(BB_t* m)
{
  /* variables */
  float time;

  /* binary in */
  time = ReadFloat(m);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_TIME,V_TIME,H_SIMPLE,
                                         NODE_VALUE_TIME_dup(time),0),
                       H_SIMPLE);
}

/* 0x08 ----------------------------------------------------------------------*/
node* do_print_message_read_bin(BB_t* m)
{ 
  /* variables */
  char text[MAX_STRING_SIZE];

  /* binary in */
  ReadString(m, text);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_PRINT,V_STRING,H_STRING,NODE_VALUE_STRING_dup(text),0),
                       H_SIMPLE);
}

/* 0x09 ----------------------------------------------------------------------*/
node* do_stufftext_message_read_bin(BB_t* m)
{
  /* variables */
  char text[MAX_STRING_SIZE];

  /* binary in */
  ReadString(m, text);

  /* construct the node tree */
  return node_add_hint(node_command_init(TOKEN_STUFFTEXT,V_STRING,H_STRING,NODE_VALUE_STRING_dup(text),0),
                       H_SIMPLE);
}

/* 0x0A ----------------------------------------------------------------------*/
node* do_setangle_message_read_bin(BB_t* m)
{
  node *tn[3];

  /* variables */
  vec3_t angles;

  int i;

  /* binary in */
  for (i=0 ; i<3 ; i++) angles[i] = ReadAngle(m);

  /* construct node tree */
  for (i=0 ; i<3 ; i++) {
    tn[i] = node_init(V_FLOAT,NODE_VALUE_FLOAT_dup(angles[i]),0);
    tn[i]->hint=H_ANGLE;
    if (i>0) node_add_next(tn[0],tn[i]);
  }
  return node_init_all(TOKEN_SETANGLE,H_SIMPLE,tn[0],0);
}

/* 0x0B ----------------------------------------------------------------------*/ 
node* do_serverinfo_message_read_bin(BB_t* m)
{
  node *n, *tn;

  long i;
  char buf[MAX_STRING_SIZE];

  /* variables */
  long serverversion;
  long maxclients;
  long multi;
  char mapname[MAX_STRING_SIZE];

  /* binary in */
  serverversion = ReadLong(m);
  maxclients = ReadByte(m);
  multi = ReadByte(m);
  ReadString(m, mapname);
  /* free the old strings (multiple level recordings) */
  if (DEMTOP->nummodels) {
    for (i=1;i<=DEMTOP->nummodels;i++) { 
      free(DEMTOP->precache_models[i]); 
    }
    free(DEMTOP->precache_models);
  } 
  DEMTOP->precache_models=(char**)malloc(sizeof(char*));
  DEMTOP->nummodels = 0;
  do {
    ReadString(m, buf);
    if (strlen(buf)==0) break;
    DEMTOP->nummodels++;
    DEMTOP->precache_models = (char**)realloc(DEMTOP->precache_models,
                                      (1+DEMTOP->nummodels)*sizeof(char*));
    if (DEMTOP->precache_models==NULL) 
      syserror(ENOMEM,"realloc precache_models");
    DEMTOP->precache_models[DEMTOP->nummodels] = strdup(buf);
    if (DEMTOP->precache_models[DEMTOP->nummodels]==NULL) syserror(errno, "strdup");
  } while (*buf);
  /* fprintf(stderr,"nummodels=%ld\n",DEMTOP->nummodels); */
  /* free the old strings (multiple level recordings) */
  if (DEMTOP->numsounds) {
    for (i=1;i<=DEMTOP->numsounds;i++) {
      free(DEMTOP->precache_sounds[i]);
    }
    free(DEMTOP->precache_sounds);
  }
  DEMTOP->precache_sounds=(char**)malloc(sizeof(char*));
  DEMTOP->numsounds = 0;
  do {
    ReadString(m, buf);
    if (strlen(buf)==0) break;
    DEMTOP->numsounds++;
    DEMTOP->precache_sounds = (char**)realloc(DEMTOP->precache_sounds,
                                      (1+DEMTOP->numsounds)*sizeof(char*));
    if (DEMTOP->precache_sounds==NULL)
      syserror(ENOMEM,"realloc precache_sounds");
    DEMTOP->precache_sounds[DEMTOP->numsounds] = strdup(buf);
    if (DEMTOP->precache_sounds[DEMTOP->numsounds]==NULL) syserror(errno, "strdup");
  } while (*buf);
  /* fprintf(stderr,"numsounds=%ld\n",DEMTOP->numsounds); */

  /* construct node tree and return the root of it */
  n = node_command_init(TOKEN_SERVERVERSION,V_INT,H_LONG,NODE_VALUE_INT_dup(serverversion),0);
  node_add_next(n,node_command_init(TOKEN_MAXCLIENTS,V_INT,H_BYTE,NODE_VALUE_INT_dup(maxclients),0));
  node_add_next(n,node_command_init(TOKEN_MULTI,V_INT,H_BYTE,NODE_VALUE_INT_dup(multi),0));
  node_add_next(n,node_command_init(TOKEN_MAPNAME,V_STRING,H_STRING,NODE_VALUE_STRING_dup(mapname),0));
  for (i=1,tn=NULL; i<=DEMTOP->nummodels; i++) {
    if (tn==NULL)
      tn=node_init_all(V_STRING,H_STRINGLIST,NODE_VALUE_STRING_dup(DEMTOP->precache_models[i]),0);
    else 
      node_add_next(tn,node_init_all(V_STRING,H_STRINGLIST,NODE_VALUE_STRING_dup(DEMTOP->precache_models[i]),0));
  }
  node_add_next(n,node_init_all(TOKEN_MODELS,0,tn,0));
  for (i=1,tn=NULL; i<=DEMTOP->numsounds; i++) {
    if (tn==NULL)
      tn=node_init_all(V_STRING,H_STRINGLIST,NODE_VALUE_STRING_dup(DEMTOP->precache_sounds[i]),0);
    else 
      node_add_next(tn,node_init_all(V_STRING,H_STRINGLIST,NODE_VALUE_STRING_dup(DEMTOP->precache_sounds[i]),0));
  }
  node_add_next(n,node_init_all(TOKEN_SOUNDS,0,tn,0));
  return node_init_all(TOKEN_SERVERINFO,H_SIMPLE,n,0);
}

/* 0x0C ----------------------------------------------------------------------*/
node* do_lightstyle_message_read_bin(BB_t* m)
{
  node *n;

  /* variables */
  long style;
  char string[MAX_STRING_SIZE]; 

  /* binary in */
  style = ReadByte(m);
  ReadString(m, string);

  /* construct node tree and return the root of it */
  n = node_add_next(
    node_command_init(TOKEN_STYLE,V_INT,H_BYTE,NODE_VALUE_INT_dup(style),0),
    node_command_init(TOKEN_STRING,V_STRING,H_STRING,NODE_VALUE_STRING_dup(string),0)
  );
  return node_init_all(TOKEN_LIGHTSTYLE,H_SIMPLE,n,0);
}

/* 0x0D ----------------------------------------------------------------------*/
node* do_updatename_message_read_bin(BB_t* m)
{
  node *n, *tn;

  /* variables */
  long player;
  char netname[MAX_STRING_SIZE];

  /* binary in */
  player = ReadByte(m);
  ReadString(m, netname);

  /* construct node tree */
  node_add_next(
    tn = node_command_init(TOKEN_PLAYER,V_INT,H_BYTE,NODE_VALUE_INT_dup(player),0),
    node_command_init(TOKEN_NETNAME,V_STRING,H_STRING,NODE_VALUE_STRING_dup(netname),0)
  );
  n = node_init(TOKEN_UPDATENAME,tn,0);
  n->hint=H_SIMPLE;

  /* return node */
  return n;
}

/* 0x0E ----------------------------------------------------------------------*/
node* do_updatefrags_message_read_bin(BB_t* m)
{
  node *n, *tn;

  /* variables */
  long player;
  long frags;

  /* binary in */
  player = ReadByte(m);
  frags = ReadShort(m);

  /* construct node tree */
  node_add_next(
    tn = node_command_init(TOKEN_PLAYER,V_INT,H_BYTE,NODE_VALUE_INT_dup(player),0),
    node_command_init(TOKEN_FRAGS,V_INT,H_SHORT,NODE_VALUE_INT_dup(frags),0)
  );
  n = node_init(TOKEN_UPDATEFRAGS,tn,0);
  n->hint=H_SIMPLE;

  /* return node */
  return n;
}

/* 0x0F ----------------------------------------------------------------------*/
node* do_clientdata_message_read_bin(BB_t* m)
{
  node *n, *tn;
  long i, j;

  /* variables */
  long mask;
  float view_ofs_z;
  float punchangle_x;
  vec3_t angles;
  vec3_t vel;
  long items;
  long weapons, ammotype, armortype, megahealth, keys, power_ups, runes;
  long uk_items_b23, uk_items_b24, uk_items_b25, uk_items_b26, uk_items_b27;
  long weaponframe;
  long armorvalue;
  long weaponmodel;
  long health;
  long currentammo;
  long ammo_shells;
  long ammo_nails;
  long ammo_rockets;
  long ammo_cells;
  long weapon;

  long uk_bit_b10, uk_bit_b11; /* unknown (unused ??) */

  /* init */
  armorvalue=weaponmodel=weaponframe=items=0;

  /* binary in */
  mask = ReadShort(m);
  view_ofs_z = mask & 0x0001 ? (float) ReadChar(m) : 22.0;
  punchangle_x = mask & 0x0002 ? (float) ReadChar(m) : 0.0;
  angles[0] = mask & 0x0004 ? (vec_t) ReadChar(m) : 0.0;
  vel[0] = mask & 0x0020 ? (vec_t) ReadChar(m) : 0.0;
  angles[1] = mask & 0x0008 ? (vec_t) ReadChar(m) : 0.0;
  vel[1] = mask & 0x0040 ? (vec_t) ReadChar(m) : 0.0;
  angles[2] = mask & 0x0010 ? (vec_t) ReadChar(m) : 0.0;
  vel[2] = mask & 0x0080 ? (vec_t) ReadChar(m) : 0.0;
  #ifdef ONLY_106
    items = mask & 0x0200 ? ReadLong(m) : 0x4001;
  #else
    items = ReadLong(m);
  #endif
  uk_bit_b10 = mask & 0x0400 ? 1 : 0; /* bit 10 */
  uk_bit_b11 = mask & 0x0800 ? 1 : 0; /* bit 11 */
  weaponframe = mask & 0x1000 ? ReadByte(m) : 0;
  armorvalue = mask & 0x2000 ? ReadByte(m) : 0;
  weaponmodel = mask & 0x4000 ? ReadByte(m) : 0;
  health = ReadShort(m);
  currentammo = ReadByte(m);
  ammo_shells = ReadByte(m);
  ammo_nails = ReadByte(m);
  ammo_rockets = ReadByte(m);
  ammo_cells = ReadByte(m);
  weapon = ReadByte(m);

  /* construct node tree and return the root of it */
  n=NULL; 
  if (mask & 0x0001) {
    n=node_link(n,node_command_init(TOKEN_VIEW_OFS_Z,V_FLOAT,H_BYTEFLOAT,NODE_VALUE_FLOAT_dup(view_ofs_z),0));
  }
  if (mask & 0x0002) {
    n=node_link(n,node_command_init(TOKEN_PUNCHANGLE_X,V_FLOAT,H_BYTEFLOAT,NODE_VALUE_FLOAT_dup(punchangle_x),0));
  }
  if (mask & 0x0004) {
    n=node_link(n,node_command_init(TOKEN_ANGLES_1,V_FLOAT,H_ANGLE,NODE_VALUE_FLOAT_dup(angles[0]),0));
  }
  if (mask & 0x0008) {
    n=node_link(n,node_command_init(TOKEN_ANGLES_2,V_FLOAT,H_ANGLE,NODE_VALUE_FLOAT_dup(angles[1]),0));
  }
  if (mask & 0x0010) {
    n=node_link(n,node_command_init(TOKEN_ANGLES_3,V_FLOAT,H_ANGLE,NODE_VALUE_FLOAT_dup(angles[2]),0));
  }
  if (mask & 0x0020) {
    n=node_link(n,node_command_init(TOKEN_VEL_X,V_FLOAT,H_BYTEFLOAT,NODE_VALUE_FLOAT_dup(vel[0]),0));
  }
  if (mask & 0x0040) {
    n=node_link(n,node_command_init(TOKEN_VEL_Y,V_FLOAT,H_BYTEFLOAT,NODE_VALUE_FLOAT_dup(vel[1]),0));
  }
  if (mask & 0x0080) {
    n=node_link(n,node_command_init(TOKEN_VEL_Z,V_FLOAT,H_BYTEFLOAT,NODE_VALUE_FLOAT_dup(vel[2]),0));
  }

  #ifdef ONLY_106
    if (mask & 0x0200) { 
  #else
    {
  #endif
              
    weapons = ((items <<  1) & 0x1FE) | /*  0 ..  7 */
              ((items >> 12) & 0x001);  /*  12 */
    ammotype = (items>>8) & 0x0F;       /*  8 .. 11 */
    armortype = (items>>13) & 0x07;     /* 13 .. 15 */
    megahealth = (items>>16) & 0x01;    /* 16 */
    keys = (items>>17) & 0x03;          /* 17 .. 18 */
    power_ups = (items>>19) & 0x0F;     /* 19 .. 22 */
    uk_items_b23 = (items>>23) & 0x01;  /* 23 ? */
    uk_items_b24 = (items>>24) & 0x01;  /* 24 ? */
    uk_items_b25 = (items>>25) & 0x01;  /* 25 ? */
    uk_items_b26 = (items>>26) & 0x01;  /* 26 ? */
    uk_items_b27 = (items>>27) & 0x01;  /* 27 ? */
    runes = (items>>28) & 0x0F;         /* 28 .. 31 */

    if (weapons) {
      for (i=1, j=weapons, tn=NULL; j; j>>=1, i++) {
        if (j&0x01) tn=node_link(tn,node_init_all(V_INT,H_BYTE,NODE_VALUE_INT_dup(i),0)); 
      }
      n=node_link(n,node_init_all(TOKEN_WEAPONS,H_SIMPLE,tn,0));
    }

    if (ammotype) {
      for (i=1, j=ammotype, tn=NULL; j; j>>=1, i++) {
        if (j&0x01) tn=node_link(tn,node_init_all(V_INT,H_BYTE,NODE_VALUE_INT_dup(i),0));
      }
      n=node_link(n,node_init_all(TOKEN_AMMOTYPE,H_SIMPLE,tn,0));
    }

    if (armortype) {
      for (i=1, j=armortype, tn=NULL; j; j>>=1, i++) {
        if (j&0x01) tn=node_link(tn,node_init_all(V_INT,H_BYTE,NODE_VALUE_INT_dup(i),0));
      }
      n=node_link(n,node_init_all(TOKEN_ARMORTYPE,H_SIMPLE,tn,0));
    }

    if (megahealth) n=node_link(n,node_init_all(TOKEN_MEGAHEALTH,H_SIMPLE,NULL,0));

    if (keys) {
      for (i=1, j=keys, tn=NULL; j; j>>=1, i++) {
        if (j&0x01) tn=node_link(tn,node_init_all(V_INT,H_BYTE,NODE_VALUE_INT_dup(i),0));
      }
      n=node_link(n,node_init_all(TOKEN_KEYS,H_SIMPLE,tn,0));
    }

    if (power_ups) {
      for (i=1, j=power_ups, tn=NULL; j; j>>=1, i++) {
        if (j&0x01) tn=node_link(tn,node_init_all(V_INT,H_BYTE,NODE_VALUE_INT_dup(i),0));
      }
      n=node_link(n,node_init_all(TOKEN_POWER_UPS,H_SIMPLE,tn,0));
    }

    if (uk_items_b23) n=node_link(n,node_init_all(TOKEN_UK_ITEMS_B23,0,NULL,0));
    if (uk_items_b24) n=node_link(n,node_init_all(TOKEN_UK_ITEMS_B24,0,NULL,0));
    if (uk_items_b25) n=node_link(n,node_init_all(TOKEN_UK_ITEMS_B25,0,NULL,0));
    if (uk_items_b26) n=node_link(n,node_init_all(TOKEN_UK_ITEMS_B26,0,NULL,0));
    if (uk_items_b27) n=node_link(n,node_init_all(TOKEN_UK_ITEMS_B27,0,NULL,0));

    if (runes) {
      for (i=1, j=runes, tn=NULL; j; j>>=1, i++) {
        if (j&0x01) tn=node_link(tn,node_init_all(V_INT,H_BYTE,NODE_VALUE_INT_dup(i),0));
      }
      n=node_link(n,node_init_all(TOKEN_RUNES,H_SIMPLE,tn,0));
    }

  }

  if (uk_bit_b10) n=node_link(n,node_init_all(TOKEN_UK_BIT_B10,0,NULL,0));
  if (uk_bit_b11) n=node_link(n,node_init_all(TOKEN_UK_BIT_B11,0,NULL,0));
  if (mask & 0x1000) n=node_link(n,node_command_init(TOKEN_WEAPONFRAME,V_INT,H_BYTE,NODE_VALUE_INT_dup(weaponframe),0));
  if (mask & 0x2000) {
    tn=node_command_init(TOKEN_ARMORVALUE,V_INT,H_BYTE,NODE_VALUE_INT_dup(armorvalue),0);
    if (DEMTOP->lastMinusArmor && 
        DEMTOP->lastArmor - DEMTOP->lastMinusArmor != armorvalue) {
      node_add_comment(tn,NODE_VALUE_STRING_dup("unexpected armorvalue: do you cheat?"));
    }
    n=node_link(n,tn);
    DEMTOP->lastArmor=armorvalue;
    DEMTOP->lastMinusArmor=0;
  }

  if (mask & 0x4000) {
    tn=node_command_init(TOKEN_WEAPONMODEL,V_INT,H_BYTE,NODE_VALUE_INT_dup(weaponmodel),0);
    if (weaponmodel>=1 && 
        weaponmodel<=DEMTOP->nummodels &&
        DEMTOP->precache_models[weaponmodel][0]!='*') {
      node_add_comment(tn,NODE_VALUE_STRING_dup(DEMTOP->precache_models[weaponmodel]));
    }
    n=node_link(n,tn);
  }
  
  tn=node_command_init(TOKEN_HEALTH,V_INT,H_SHORT,NODE_VALUE_INT_dup(health),0);
  if (DEMTOP->lastMinusHealth && 
      DEMTOP->lastHealth - DEMTOP->lastMinusHealth != health) {
    node_add_comment(tn,NODE_VALUE_STRING_dup("unexpected health: do you cheat?"));
  }
  n=node_link(n,tn);
  DEMTOP->lastHealth=health;
  DEMTOP->lastMinusHealth=0;

  node_add_next(n,node_command_init(TOKEN_CURRENTAMMO,V_INT,H_BYTE,NODE_VALUE_INT_dup(currentammo),0));
  node_add_next(n,node_command_init(TOKEN_AMMO_SHELLS,V_INT,H_BYTE,NODE_VALUE_INT_dup(ammo_shells),0));
  node_add_next(n,node_command_init(TOKEN_AMMO_NAILS,V_INT,H_BYTE,NODE_VALUE_INT_dup(ammo_nails),0));
  node_add_next(n,node_command_init(TOKEN_AMMO_ROCKETS,V_INT,H_BYTE,NODE_VALUE_INT_dup(ammo_rockets),0));
  node_add_next(n,node_command_init(TOKEN_AMMO_CELLS,V_INT,H_BYTE,NODE_VALUE_INT_dup(ammo_cells),0));
  
  if (weapon) {
    for (i=2, j=weapon, tn=NULL; j; j>>=1, i++) {
      if (j&0x01) tn=node_link(tn,node_init_all(V_INT,H_BYTE,NODE_VALUE_INT_dup(i),0));
    }
    n=node_link(n,node_init_all(TOKEN_WEAPON,H_SIMPLE,tn,0));
  }
  else {
    node_add_next(n,node_command_init(TOKEN_WEAPON,V_INT,H_BYTE,NODE_VALUE_INT_dup(1),0));  
  }
                                  
  return node_init_all(TOKEN_CLIENTDATA,H_DEM_CLIENTDATA,n,0);
}

/* 0x10 ----------------------------------------------------------------------*/
node* do_stopsound_message_read_bin(BB_t* m)
{
  node *n;

  /* variables */
  long channel;
  long entity;
  
  long entity_channel;

  /* binary in */
  entity_channel = ReadShort(m); /* combined variable */

  channel = entity_channel & 0x07;
  entity = (entity_channel >> 3) & 0x1FFF;

  /* construct node tree */
  n = node_add_next (
    node_command_init(TOKEN_CHANNEL,V_INT,0,NODE_VALUE_INT_dup(channel),0),
    node_command_init(TOKEN_ENTITY,V_INT,0,NODE_VALUE_INT_dup(entity),0)
  );
  n = node_init_all(TOKEN_STOPSOUND,H_DEM_STOPSOUND,n,0);

  /* return node */
  return n;
}

/* 0x11 ----------------------------------------------------------------------*/
node* do_updatecolors_message_read_bin(BB_t* m)
{

  /* variables */
  long player;
  long colors;
  long shirtcolor;
  long pantscolor;

  /* binary in */
  player = ReadByte(m);
  colors = ReadByte(m);
  shirtcolor = (colors>>4) & 0x0F;
  pantscolor = colors & 0x0F;

  /* construct node tree and return the root of it */
  return node_init_all(TOKEN_UPDATECOLORS, H_DEM_UPDATECOLORS,
           node_add_next (
             node_init_all(TOKEN_PLAYER, H_SIMPLE, 
               node_init_all(V_INT, H_BYTE, NODE_VALUE_INT_dup(player), 0),
             0)
             ,
             node_init_all(TOKEN_COLORS, 0,
               node_add_next(
                 node_init_all(V_INT, 0, NODE_VALUE_INT_dup(shirtcolor), 0),
                 node_init_all(V_INT, 0, NODE_VALUE_INT_dup(pantscolor), 0)
               ),
             0)
           ),
         0);
}

/* 0x12 ----------------------------------------------------------------------*/
node* do_particle_message_read_bin(BB_t* m)
{
  /* variables */
  vec3_t origin;
  vec3_t vel;
  long count;
  long color;

  int i;

  /* binary in */
  for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
  for (i=0 ; i<3 ; i++) vel[i] = (vec_t) ReadChar(m) * 0.0625;
  count = ReadByte(m);
  color = ReadByte(m);

  /* construct node tree and return the root of it */
  return node_init_all(TOKEN_PARTICLE, H_SIMPLE,
           node_add_next(
             node_triple_command_init(TOKEN_ORIGIN, V_FLOAT, H_COORD,
               NODE_VALUE_FLOAT_dup(origin[0]), 
               NODE_VALUE_FLOAT_dup(origin[1]),
               NODE_VALUE_FLOAT_dup(origin[2]),
             0)
           ,
             node_add_next(
               node_triple_command_init(TOKEN_VEL, V_FLOAT, H_VEL,
                 NODE_VALUE_FLOAT_dup(vel[0]),
                 NODE_VALUE_FLOAT_dup(vel[1]),
                 NODE_VALUE_FLOAT_dup(vel[2]),
               0)
             , 
               node_add_next(
                 node_command_init(TOKEN_COUNT,V_INT,H_BYTE,NODE_VALUE_INT_dup(count),0)
               ,
                 node_command_init(TOKEN_COLOR,V_INT,H_BYTE,NODE_VALUE_INT_dup(color),0)
               )
             )
           ),
         0);
}

/* 0x13 ----------------------------------------------------------------------*/
node* do_damage_message_read_bin(BB_t* m)
{
  /* variables */
  long save;
  long take;
  vec3_t origin;

  int i;

  /* binary in */
  save = ReadByte(m);
  take = ReadByte(m);
  for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);

  /* add */
  DEMTOP->lastMinusArmor = save;
  DEMTOP->lastMinusHealth = take;

  /* construct node tree and return the root of it */
  return node_init_all(TOKEN_DAMAGE, H_SIMPLE, 
           node_add_next(
             node_init_all(TOKEN_SAVE, 0,
               node_init_all(V_INT, H_BYTE, NODE_VALUE_INT_dup(save), 0),
               0)
             ,
             node_add_next(
               node_init_all(TOKEN_TAKE, 0,
                 node_init_all(V_INT, H_BYTE, NODE_VALUE_INT_dup(take), 0),
               0)
               ,
               node_triple_command_init(TOKEN_ORIGIN, V_FLOAT, H_COORD,
                   NODE_VALUE_FLOAT_dup(origin[0]),
                   NODE_VALUE_FLOAT_dup(origin[1]),
                   NODE_VALUE_FLOAT_dup(origin[2]),
               0)
             )
           ),
           0
         );
}

/* 0x14 ----------------------------------------------------------------------*/
node* do_spawnstatic_message_read_bin(BB_t* m)
{
  node *n;

  /* variables */
  long default_modelindex;
  long default_frame;
  long default_colormap;
  long default_skin;
  vec3_t default_origin;
  vec3_t default_angles;

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
  n = node_command_init(TOKEN_DEFAULT_MODELINDEX,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_modelindex),0);
  if (default_modelindex >= 1 &&
      default_modelindex <= DEMTOP->nummodels &&
      DEMTOP->precache_models[default_modelindex][0]!='*') {
    node_add_comment(n,NODE_VALUE_STRING_dup(DEMTOP->precache_models[default_modelindex]));
  }
  node_add_next(n,node_command_init(TOKEN_DEFAULT_FRAME,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_frame),0));
  node_add_next(n,node_command_init(TOKEN_DEFAULT_COLORMAP,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_colormap),0));
  node_add_next(n,node_command_init(TOKEN_DEFAULT_SKIN,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_skin),0));
  node_add_next(n,node_triple_command_init(TOKEN_DEFAULT_ORIGIN, V_FLOAT, H_COORD,
                  NODE_VALUE_FLOAT_dup(default_origin[0]),
                  NODE_VALUE_FLOAT_dup(default_origin[1]),
                  NODE_VALUE_FLOAT_dup(default_origin[2]), 0));
  node_add_next(n,node_triple_command_init(TOKEN_DEFAULT_ANGLES, V_FLOAT, H_ANGLE,
                  NODE_VALUE_FLOAT_dup(default_angles[0]),
                  NODE_VALUE_FLOAT_dup(default_angles[1]),
                  NODE_VALUE_FLOAT_dup(default_angles[2]), 0));
  return node_init_all(TOKEN_SPAWNSTATIC, H_DEM_SPAWNSTATIC, n, 0);
}

/* 0x15 ----------------------------------------------------------------------*/
node* do_spawnbinary_message_read_bin(void)
{
  node* n;

  /* construct node tree */
  n=node_init(TOKEN_SPAWNBINARY,NULL,0);
  n->hint=H_SIMPLE;
  node_add_comment(n,NODE_VALUE_STRING_dup("Illegible server message"));

  /* return node */
  return n;
}

/* 0x16 ----------------------------------------------------------------------*/
node* do_spawnbaseline_message_read_bin(BB_t* m)
{
  node *n, *tn;

  /* variables */
  long entity;
  long default_modelindex;
  long default_frame;
  long default_colormap;
  long default_skin;
  vec3_t default_origin;
  vec3_t default_angles;

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
  n = node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,NODE_VALUE_INT_dup(entity),0);
  tn = node_command_init(TOKEN_DEFAULT_MODELINDEX,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_modelindex),0);
  if (default_modelindex >= 1 && 
      default_modelindex <= DEMTOP->nummodels && 
      DEMTOP->precache_models[default_modelindex][0]!='*') {
    node_add_comment(tn,NODE_VALUE_STRING_dup(DEMTOP->precache_models[default_modelindex]));
  }
  node_add_next(n,tn);
  node_add_next(n,node_command_init(TOKEN_DEFAULT_FRAME,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_frame),0));
  node_add_next(n,node_command_init(TOKEN_DEFAULT_COLORMAP,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_colormap),0));
  node_add_next(n,node_command_init(TOKEN_DEFAULT_SKIN,V_INT,H_BYTE,NODE_VALUE_INT_dup(default_skin),0));
  node_add_next(n,node_triple_command_init(TOKEN_DEFAULT_ORIGIN, V_FLOAT, H_COORD,
                  NODE_VALUE_FLOAT_dup(default_origin[0]),
                  NODE_VALUE_FLOAT_dup(default_origin[1]),
                  NODE_VALUE_FLOAT_dup(default_origin[2]), 0));
  node_add_next(n,node_triple_command_init(TOKEN_DEFAULT_ANGLES, V_FLOAT, H_ANGLE,
                  NODE_VALUE_FLOAT_dup(default_angles[0]),
                  NODE_VALUE_FLOAT_dup(default_angles[1]),
                  NODE_VALUE_FLOAT_dup(default_angles[2]), 0));
  return node_init_all(TOKEN_SPAWNBASELINE, H_DEM_SPAWNBASELINE, n, 0);
}

/* 0x17 ----------------------------------------------------------------------*/
node* do_temp_entity_message_read_bin(BB_t* m)
{
  node *n;

  /* variables */
  long entitytype;
  long entity;
  vec3_t origin;
  vec3_t trace_endpos;
  long color;
  long range;
  
  int i;

  /* init */
  entity=0;
  color=0;
  range=0;

  /* binary in */
  entitytype = ReadByte(m);
  switch (entitytype) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
    case 8:
    case 10:
    case 11:
             for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
    break;
    case 5:
    case 6:
    case 9:
    case 13:
             entity = ReadShort(m);
             for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
             for (i=0 ; i<3 ; i++) trace_endpos[i] = ReadCoord(m);
    break;
    case 12:
             for (i=0 ; i<3 ; i++) origin[i] = ReadCoord(m);
             color = ReadByte(m);
             range = ReadByte(m);
    break;             
  }

  /* construct node tree and return the root of it */
  n = node_command_init(TOKEN_ENTITYTYPE,V_INT,H_BYTE,NODE_VALUE_INT_dup(entitytype),0);
  switch (entitytype) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 7:
    case 8:
    case 10:
    case 11:
             node_add_next(n,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                             NODE_VALUE_FLOAT_dup(origin[0]),
                             NODE_VALUE_FLOAT_dup(origin[1]),
                             NODE_VALUE_FLOAT_dup(origin[2]),0));
    break;
    case 5:
    case 6:
    case 9:
    case 13:
             node_add_next(n,node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,NODE_VALUE_INT_dup(entity),0));
             node_add_next(n,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                             NODE_VALUE_FLOAT_dup(origin[0]),NODE_VALUE_FLOAT_dup(origin[1]),NODE_VALUE_FLOAT_dup(origin[2]),0));
             node_add_next(n,node_triple_command_init(TOKEN_TRACE_ENDPOS,V_FLOAT,H_COORD,
                             NODE_VALUE_FLOAT_dup(trace_endpos[0]),
                             NODE_VALUE_FLOAT_dup(trace_endpos[1]),
                             NODE_VALUE_FLOAT_dup(trace_endpos[2]),0));
    break;
    case 12:
             node_add_next(n,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                             NODE_VALUE_FLOAT_dup(origin[0]),
                             NODE_VALUE_FLOAT_dup(origin[1]),
                             NODE_VALUE_FLOAT_dup(origin[2]),0));
             node_add_next(n,node_command_init(TOKEN_COLOR,V_INT,H_BYTE,NODE_VALUE_INT_dup(color),0));
             node_add_next(n,node_command_init(TOKEN_RANGE,V_INT,H_BYTE,NODE_VALUE_INT_dup(range),0));
    break;
  }
  return node_init_all(TOKEN_TEMP_ENTITY,H_SIMPLE,n,0);
}

/* 0x18 ----------------------------------------------------------------------*/
node* do_setpause_message_read_bin(BB_t* m)
{
  /* variables */
  long pausestate;

  /* binary in */
  pausestate = ReadByte(m);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_SETPAUSE,V_INT,H_BYTE,NODE_VALUE_INT_dup(pausestate),0),
                       H_SIMPLE);
}

/* 0x19 ----------------------------------------------------------------------*/
node* do_signonnum_message_read_bin(BB_t* m)
{
  /* variables */
  long signon;

  /* binary in */
  signon = ReadByte(m);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_SIGNONNUM,V_INT,H_BYTE,NODE_VALUE_INT_dup(signon),0),
                       H_SIMPLE);
}

/* 0x1A ----------------------------------------------------------------------*/
node* do_centerprint_message_read_bin(BB_t* m)
{
  /* variables */
  char text[MAX_STRING_SIZE];

  /* binary in */
  ReadString(m, text);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_CENTERPRINT,V_STRING,H_STRING,NODE_VALUE_STRING_dup(text),0),
                       H_SIMPLE);
}

/* 0x1B ----------------------------------------------------------------------*/
node* do_killedmonster_message_read_bin(void)
{
  node* n;

  /* construct node tree */
  n=node_init(TOKEN_KILLEDMONSTER,NULL,0);
  n->hint=H_SIMPLE;

  /* return node */
  return n;
}

/* 0x1C ----------------------------------------------------------------------*/
node* do_foundsecret_message_read_bin(void)
{
  node* n;

  /* construct node tree */
  n=node_init(TOKEN_FOUNDSECRET,NULL,0);
  n->hint=H_SIMPLE;

  /* return node */
  return n;
}

/* 0x1D ----------------------------------------------------------------------*/
node* do_spawnstaticsound_message_read_bin(BB_t* m)
{
  node *n, *tn;

  /* variables */
  vec3_t origin;
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
  n = node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
      NODE_VALUE_FLOAT_dup(origin[0]),NODE_VALUE_FLOAT_dup(origin[1]),NODE_VALUE_FLOAT_dup(origin[2]),0);
  tn = node_command_init(TOKEN_SOUNDNUM,V_INT,H_BYTE,NODE_VALUE_INT_dup(soundnum),0);
  if (soundnum >= 1 &&
      soundnum <= DEMTOP->numsounds) {
    node_add_comment(tn,NODE_VALUE_STRING_dup(DEMTOP->precache_sounds[soundnum]));
  }
  node_add_next(n,tn);
  node_add_next(n,node_command_init(TOKEN_VOL,V_FLOAT,H_VOL,NODE_VALUE_FLOAT_dup(vol),0));
  node_add_next(n,node_command_init(TOKEN_ATTENUATION,V_FLOAT,H_ATTENUATION,NODE_VALUE_FLOAT_dup(attenuation),0));
  return node_init_all(TOKEN_SPAWNSTATICSOUND,H_SIMPLE,n,0);
}

/* 0x1E ----------------------------------------------------------------------*/
node* do_intermission_message_read_bin(void)
{
  /* construct node tree and return the root of it */
  return node_init_all(TOKEN_INTERMISSION,H_SIMPLE,NULL,0);
}

/* 0x1F ----------------------------------------------------------------------*/
node* do_finale_message_read_bin(BB_t* m)
{
  /* variables */
  char text[MAX_STRING_SIZE];

  /* binary in */
  ReadString(m, text);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_FINALE,V_STRING,H_STRING,NODE_VALUE_STRING_dup(text),0),
                       H_SIMPLE);
}

/* 0x20 ----------------------------------------------------------------------*/
node* do_cdtrack_message_read_bin(BB_t* m)
{
  node *n;

  /* variables */
  long fromtrack;
  long totrack;

  /* binary in */
  fromtrack = ReadByte(m);
  totrack = ReadByte(m);

  /* construct node tree and return the root of it */
  n = node_add_next(
    node_command_init(TOKEN_FROMTRACK,V_INT,H_BYTE,NODE_VALUE_INT_dup(fromtrack),0),
    node_command_init(TOKEN_TOTRACK,V_INT,H_BYTE,NODE_VALUE_INT_dup(totrack),0)
  );
  return node_init_all(TOKEN_CDTRACK,H_SIMPLE,n,0);
}

/* 0x21 ----------------------------------------------------------------------*/
node* do_sellscreen_message_read_bin(void)
{
  /* construct node tree and return the root of it */
  return node_init_all(TOKEN_SELLSCREEN,H_SIMPLE,NULL,0);
}

/* 0x22 ----------------------------------------------------------------------*/
node* do_cutscene_message_read_bin(BB_t* m)
{
  /* variables */
  char text[MAX_STRING_SIZE];

  /* binary in */
  ReadString(m, text);

  /* construct node tree and return the root of it */
  return node_add_hint(node_command_init(TOKEN_CUTSCENE,V_STRING,H_STRING,NODE_VALUE_STRING_dup(text),0),
                       H_SIMPLE);
}

/* >= 0x80 -------------------------------------------------------------------*/
node* do_updateentity_message_read_bin(BB_t* m, long mask)
{
  node *n, *tn;

  /* variables */
  long entity;
  long modelindex;
  long frame;
  long colormap;
  long skin;
  long effects;
  vec3_t origin;
  vec3_t angles;
  long new_;

  /* init */
  origin[0]=origin[1]=origin[2]=angles[0]=angles[1]=angles[2]=0.0;

  /* binary in */
  if (mask & 0x0001) mask |= (ReadByte(m)) << 8;
  entity = mask & 0x4000 ? ReadShort(m) : ReadByte(m);
  modelindex = mask & 0x0400 ? ReadByte(m) : 0;
  frame = mask & 0x0040 ? ReadByte(m) : 0;
  colormap = mask & 0x0800 ? ReadByte(m) : 0;
  skin = mask & 0x1000 ? ReadByte(m) : 0;
  effects = mask & 0x2000 ? ReadByte(m) : 0;
  origin[0] = mask & 0x0002 ? ReadCoord(m) : 0;
  angles[0] = mask & 0x0100 ? ReadAngle(m) : 0;
  origin[1] = mask & 0x0004 ? ReadCoord(m) : 0;
  angles[1] = mask & 0x0010 ? ReadAngle(m) : 0;
  origin[2] = mask & 0x0008 ? ReadCoord(m) : 0;
  angles[2] = mask & 0x0200 ? ReadAngle(m) : 0;
  new_ = mask & 0x0020 ? 1 : 0;

  /* construct node tree and return the root of it */
  n=node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,NODE_VALUE_INT_dup(entity),0);
  if (mask & 0x0400) {
    tn=node_command_init(TOKEN_MODELINDEX,V_INT,H_BYTE,NODE_VALUE_INT_dup(modelindex),0);
    if (modelindex>=1 && 
        modelindex<=DEMTOP->nummodels &&
        DEMTOP->precache_models[modelindex][0]!='*') {
      node_add_comment(tn,NODE_VALUE_STRING_dup(DEMTOP->precache_models[modelindex]));
    }
    node_add_next(n,tn);
  }
  if (mask & 0x0040) {
    node_add_next(n,node_command_init(TOKEN_FRAME,V_INT,H_BYTE,NODE_VALUE_INT_dup(frame),0));
  }
  if (mask & 0x0800) {
    node_add_next(n,node_command_init(TOKEN_COLORMAP,V_INT,H_BYTE,NODE_VALUE_INT_dup(colormap),0));
  }
  if (mask & 0x1000) {
    node_add_next(n,node_command_init(TOKEN_SKIN,V_INT,H_BYTE,NODE_VALUE_INT_dup(skin),0));
  }
  if (mask & 0x2000) {
    node_add_next(n,node_command_init(TOKEN_EFFECTS,V_INT,H_BYTE,NODE_VALUE_INT_dup(effects),0));
  }
  if (mask&0x0002) { 
    node_add_next(n,node_command_init(TOKEN_ORIGIN_X,V_FLOAT,H_COORD,NODE_VALUE_FLOAT_dup(origin[0]),0));
  }
  if (mask&0x0004) { 
    node_add_next(n,node_command_init(TOKEN_ORIGIN_Y,V_FLOAT,H_COORD,NODE_VALUE_FLOAT_dup(origin[1]),0));
  }
  if (mask&0x0008) { 
    node_add_next(n,node_command_init(TOKEN_ORIGIN_Z,V_FLOAT,H_COORD,NODE_VALUE_FLOAT_dup(origin[2]),0));
  }
  if (mask&0x0100) { 
    node_add_next(n,node_command_init(TOKEN_ANGLES_1,V_FLOAT,H_ANGLE,NODE_VALUE_FLOAT_dup(angles[0]),0));
  }
  if (mask&0x0010) { 
    node_add_next(n,node_command_init(TOKEN_ANGLES_2,V_FLOAT,H_ANGLE,NODE_VALUE_FLOAT_dup(angles[1]),0));
  }
  if (mask&0x0200) { 
    node_add_next(n,node_command_init(TOKEN_ANGLES_3,V_FLOAT,H_ANGLE,NODE_VALUE_FLOAT_dup(angles[2]),0));
  }
  if (new_) {
    node_add_next(n,node_init_all(TOKEN_NEW,H_SIMPLE,NULL,0));
  }
  return node_init_all(TOKEN_UPDATEENTITY,H_DEM_UPDATEENTITY,n,0);
}

/*----------------------------------------------------------------------------*/
/* end of specific message decoding ------------------------------------------*/
/*----------------------------------------------------------------------------*/

/******************************************************************************/
/* block output: text version *************************************************/
/******************************************************************************/

node* DEM_block_write_text(node* b)
{
  /* b may be NULL (something was wrong or removed) */
  if (b == NULL) return b;
  /* type is TOKEN_TRACK or TOKEN_*/
  /* down is camera, down->next is the next message */
  if ( ( b->type==TOKEN_TRACK ) || 
       ( b->type==TOKEN_BLOCK && b->down->next!=NULL ) ) {
    node_write_text(b,0);
  }
  return b;
}

/******************************************************************************/
/* block output: binary version ***********************************************/
/******************************************************************************/

node* DEM_block_write_bin(node* b)
{
  BB_t m;
  node* tn;
  node* tn1;
  long datasize = 0;

  /* b may be NULL (something was wrong or removed) */
  if (b == NULL) return b;

  /* we start from the beginning */
  m.p = m.buffer;

  switch (b->type) {
    case TOKEN_TRACK: 
      WriteString(&m,(char*)b->down->down); /* with \0 */
      m.p--;                                /* without \0 */
      /* calculate the size info */
      datasize = m.p - m.buffer;
    break;
    case TOKEN_BLOCK:
      tn=b->down;
      if (tn->type != TOKEN_CAMERA) 
        syserror(DINTE, "block doesn't start with camera");
      if (tn->next != NULL) { /* we really need something after camera */
        m.p += 4; /* after size */
        tn1 = tn->down; /* tn1 is V_FLOAT */
        WriteFloat(&m,*(float*)tn1->down);
        tn1 = tn1->next; /* tn1 is the 2nd V_FLOAT */
        WriteFloat(&m,*(float*)tn1->down);
        tn1 = tn1->next; /* tn1 is the 3rd V_FLOAT */
        WriteFloat(&m,*(float*)tn1->down);
        tn = tn->next; /* tn is the first "real" message */
        for (; tn!=NULL; tn=tn->next) {
          /* do something useful with the tn */
          /* fprintf(stderr,"m %i\n", tn->type); */
          switch(tn->hint) {
            case H_NOTHING:
            break;
            case H_SIMPLE: 
              do_simple_message_write_bin(tn,&m);
            break;
            case H_UNKNOWN:
              do_unknown_message_write_bin(tn,&m);
            break;
            case H_DEM_SOUND:
              do_dem_sound_message_write_bin(tn,&m);
            break;
            case H_DEM_CLIENTDATA:
              do_dem_clientdata_message_write_bin(tn,&m);
            break;
            case H_DEM_STOPSOUND:
              do_dem_stopsound_message_write_bin(tn,&m);
            break;
            case H_DEM_UPDATECOLORS:
              do_dem_updatecolors_message_write_bin(tn,&m);
            break;
            case H_DEM_SPAWNSTATIC:
              do_dem_spawnstatic_message_write_bin(tn,&m);		
            break;
            case H_DEM_SPAWNBASELINE:
              do_dem_spawnbaseline_message_write_bin(tn,&m);		
            break;
            case H_DEM_UPDATEENTITY:
              do_dem_updateentity_message_write_bin(tn,&m);		
            break;
            default: /* this creates a bad message */
              *m.p='\0';
              m.p++;
            break;
          }
        }
        /* calculate the size info */
        datasize = m.p - m.buffer; 
        m.size = (datasize) - 16;
        /* write the size info */
        m.p = m.buffer;
        WriteLong(&m, m.size);
      } /* end (tn->next != NULL) */
    break;
    default: 
      syserror(DINTE,"bad block type");
    break;
  } 
  
  if (datasize) { /* if there was something after camera */
    if (fwrite(m.buffer, 1, datasize, output_file) != (size_t)datasize) 
      syserror(FIWRITE, output_filename);
    if (fflush(output_file) != 0) 
      syserror(errno, output_filename);
  }

  return b;
}

void do_dem_sound_message_write_bin(node* n, BB_t* m)
{
  node* c;
  node* a;
  unsigned char mask;
  long entity_channel;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    sound: combining of two values, bit mask
  */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  mask = 0;

  /* message -> commands */
  c=n->down; /* 1st command: vol, attenuation, channel */
  if (c->type==TOKEN_VOL) mask|=0x01;
  if (c->type==TOKEN_ATTENUATION) mask|=0x02;
  c=c->next; /* 2nd command: attenuation, channel */
  if (c->type==TOKEN_ATTENUATION) mask|=0x02;

  /* now output */
  WriteByte(m,mask);
  c=n->down; /* c may be vol, attenuation or channel */
  if (mask&0x01) { /* vol */
    do_simple_argument_write_bin(c->down,m);
    c=c->next;
  }
  /* c may be attenuation or channel */
  if (mask&0x02) { /* attenuation */
    do_simple_argument_write_bin(c->down,m);
    c=c->next;
  }
  /* c is now channel */
  a=c->down; /* argument */
  entity_channel = (*(long*)a->down) & 0x07;
  c=c->next; /* entity */
  a=c->down; /* argument */
  entity_channel |= ((*(long*)(a->down)) & 0x1FFF ) << 3;
  WriteShort(m,entity_channel);

  c=c->next; /* soundnum */
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* origin */
  do_simple_arguments_write_bin(c->down,m); /* plural s */
}

void do_dem_clientdata_message_write_bin(node* n, BB_t* m)
{
  node* c;
  node* a;
  long mask;
  int lastitem;
  unsigned long items;
  float angles_1, angles_2, angles_3;
  float vel_x, vel_y, vel_z;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    clientdata: combining many values, bit mask
  */

  angles_1 = angles_2 = angles_3 = 0.0;
  vel_x = vel_y = vel_z = 0.0;

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* mask */
  items = 0;
  lastitem = 0;
  /* mask = 0; */
  /* use always an items entry: 
   * this is the standard behaviour in WinQuake 1.07 and it doesn't
   * hurt in all other cases. I never found a DEM file without the items
   * entry.
   */
  mask =0x0200;
  for (c=n->down; c!=NULL ; c=c->next) {
    /* fprintf(stderr,"t=%i\n",c->type); */
    switch (c->type) {
      case TOKEN_VIEW_OFS_Z:       mask |= 0x0001; break;
      case TOKEN_PUNCHANGLE_X:     mask |= 0x0002; break;
      case TOKEN_ANGLES_1:         mask |= 0x0004; break;
      case TOKEN_ANGLES_2:         mask |= 0x0008; break;
      case TOKEN_ANGLES_3:         mask |= 0x0010; break;
      case TOKEN_VEL_X:            mask |= 0x0020; break;
      case TOKEN_VEL_Y:            mask |= 0x0040; break;
      case TOKEN_VEL_Z:            mask |= 0x0080; break;
      case TOKEN_WEAPONS:      mask |= 0x0200; lastitem=TOKEN_WEAPONS;
                           /* ((items <<  1) & 0x1FE) |   0 ..  7
                              ((items >> 12) & 0x001);    12 */
                           for (a=c->down;a!=NULL;a=a->next) {
                             if (*(long*)a->down == 1) {
                               items|= 0x1000;
                             }
                             else {
                               items|= ( 0x1 << (*(long*)a->down - 2)) & 0x00FF;
                             }
                           }
                           break; 
      case TOKEN_AMMOTYPE:     mask |= 0x0200; lastitem=TOKEN_AMMOTYPE;
                           /* (items>>8) & 0x0F        8 .. 11 */
                           for (a=c->down;a!=NULL;a=a->next) {
                             items|= ( 0x80L << *(long*)a->down) & 0x0F00L;
                           }
                           break;
      case TOKEN_ARMORTYPE:    mask |= 0x0200; lastitem=TOKEN_ARMORTYPE;
                           /* (items>>13) & 0x07       13 .. 15 */
                           for (a=c->down;a!=NULL;a=a->next) {
                             items|= ( 0x1000L << *(long*)a->down) & 0xE000L;
                           }
                           break;
      case TOKEN_MEGAHEALTH:   mask |= 0x0200; lastitem=TOKEN_MEGAHEALTH;
                           items |= 0x00010000L;
                           break;
      case TOKEN_KEYS:         mask |= 0x0200; lastitem=TOKEN_KEYS;
                           /* (items>>17) & 0x03      17 .. 18 */
                           for (a=c->down;a!=NULL;a=a->next) {
                             items|= ( 0x10000L << *(long*)a->down) & 0x60000L;
                           }
                           break;
      case TOKEN_POWER_UPS:    mask |= 0x0200; lastitem=TOKEN_POWER_UPS;
                           /* (items>>19) & 0x0F     19 .. 22 */
                           for (a=c->down;a!=NULL;a=a->next) {
                             items|= ( 0x40000L << *(long*)a->down) & 0x780000L;
                           }
                           break;
      case TOKEN_UK_ITEMS_B23: mask |= 0x0200; lastitem=TOKEN_UK_ITEMS_B23;
                           items |= 0x00800000L;
                           break;
      case TOKEN_UK_ITEMS_B24: mask |= 0x0200; lastitem=TOKEN_UK_ITEMS_B24;
                           items |= 0x01000000L;
                           break;
      case TOKEN_UK_ITEMS_B25: mask |= 0x0200; lastitem=TOKEN_UK_ITEMS_B25;
                           items |= 0x02000000L;
                           break;
      case TOKEN_UK_ITEMS_B26: mask |= 0x0200; lastitem=TOKEN_UK_ITEMS_B26;
                           items |= 0x04000000L;
                           break;
      case TOKEN_UK_ITEMS_B27: mask |= 0x0200; lastitem=TOKEN_UK_ITEMS_B27;
                           items |= 0x08000000L;
                           break;
      case TOKEN_RUNES:        mask |= 0x0200; lastitem=TOKEN_RUNES;
                           /* (items>>28) & 0x0F        28 .. 31 */
                           for (a=c->down;a!=NULL;a=a->next) {
                             items|= ( 0x8000000L << *(long*)a->down) & 0xF0000000L;
                           }
                           break;
      case TOKEN_UK_BIT_B10:   mask |= 0x0400; break;
      case TOKEN_UK_BIT_B11:   mask |= 0x0800; break;
      case TOKEN_WEAPONFRAME:  mask |= 0x1000; break;
      case TOKEN_ARMORVALUE:   mask |= 0x2000; break;
      case TOKEN_WEAPONMODEL:  mask |= 0x4000; break;
    }
  }

  c=n->down; /* start again */

  WriteShort(m,mask); /* mask */

  if (mask&0x0001) { /* view_ofs_z */
    WriteChar(m,(char)(*(float*)c->down->down));
    c=c->next;
  }

  if (mask&0x0002) { /* punchangle_x */
    WriteChar(m,(char)(*(float*)c->down->down));
    c=c->next;
  }

  if (mask&0x0004) { /* angels_1 */
    angles_1 = *(float*)c->down->down;
    c=c->next;
  }

  if (mask&0x0008) { /* angels_2 */
    angles_2 = *(float*)c->down->down;
    c=c->next;
  }

  if (mask&0x0010) { /* angels_3 */
    angles_3 = *(float*)c->down->down;
    c=c->next;
  }

  if (mask&0x0020) { /* vel_x */
    vel_x = *(float*)c->down->down;
    c=c->next;
  }

  if (mask&0x0040) { /* vel_y */
    vel_y = *(float*)c->down->down;
    c=c->next;
  }

  if (mask&0x0080) { /* vel_z */
    vel_z = *(float*)c->down->down;
    c=c->next;
  }

  if (mask&0x0004) WriteChar(m,(char)angles_1); 
  if (mask&0x0020) WriteChar(m,(char)vel_x); 
  if (mask&0x0008) WriteChar(m,(char)angles_2); 
  if (mask&0x0040) WriteChar(m,(char)vel_y); 
  if (mask&0x0010) WriteChar(m,(char)angles_3); 
  if (mask&0x0080) WriteChar(m,(char)vel_z); 

  if (mask&0x0200) { /* items (many poss.) */
    WriteLong(m,items);
    if (items!=0) { /* go over all items makes only sense if there are some */
      for ( ; c->type!=lastitem ; c=c->next) ;
      c=c->next;
    }
  }
  
  if (mask&0x0400) { /* uk_bit_b10 (in mask) */
    c=c->next;
  }

  if (mask&0x0800) { /* uk_bit_b11 (in mask) */
    c=c->next;
  }

  if (mask&0x1000) { /* weaponframe */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }

  if (mask&0x2000) { /* armorvalue */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }

  if (mask&0x4000) { /* weaponmodel */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }

  do_simple_argument_write_bin(c->down, m); /* health */
  c=c->next;

  do_simple_argument_write_bin(c->down, m); /* currentammo */
  c=c->next;

  do_simple_argument_write_bin(c->down, m); /* ammo_shells */
  c=c->next;

  do_simple_argument_write_bin(c->down, m); /* ammo_nails */
  c=c->next;

  do_simple_argument_write_bin(c->down, m); /* ammo_rockets */
  c=c->next;

  do_simple_argument_write_bin(c->down, m); /* ammo_cells */
  c=c->next;

  {
    unsigned char value;
    
    value = 0;
    for (a=c->down;a!=NULL;a=a->next) {
      if (*(long*)a->down != 1) {
        value |= ( 0x1 << (*(long*)a->down - 2)) & 0x00FF;
      }
    }
    WriteByte(m,value);
  }

}

void do_dem_stopsound_message_write_bin(node* n, BB_t* m)
{
  node* c;
  long entity_channel;
  
  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    stopsound: combining of two values
  */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  c=n->down; /* c is the channel command */
  entity_channel = (*(long*)c->down->down) & 0x07;
  c=c->next; /* c is the entity command */
  entity_channel |= ((*(long*)(c->down->down)) & 0x1FFF ) << 3;
  WriteShort(m,entity_channel);
}

void do_dem_updatecolors_message_write_bin(node* n, BB_t* m)
{
  node* c;
  node* a;
  unsigned char colors;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    updatecolors: combining of two values
  */

  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* message -> commands */
  c=n->down; /* player */
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* colors */
  a=c->down; /* argument */
  colors = ((*(long*)a->down) << 4) & 0xF0;
  a=a->next;
  colors |= (*(long*)a->down) & 0x0F;
  WriteByte(m,colors);
}

void do_dem_spawnstatic_message_write_bin(node* n, BB_t* m)
{
  node* c;
  node* c2;
  node* a;
  node* a2;
  char ts[1000];

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    spawnstatic is very easy: all commands have to be there
  */

  /* fprintf(stderr,"s"); */
  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* message -> commands */
  c=n->down; /* default_modelindex */
  if (c==NULL) syntaxerror(n->pos, "command default_modelindex missing");
  if (c->type!=TOKEN_DEFAULT_MODELINDEX) {
    sprintf(ts, "command is %s, should be %s", 
                 node_token_string(c->type), 
                 node_token_string(TOKEN_DEFAULT_MODELINDEX));
    syntaxerror(c->pos, ts);
  }
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_frame */
  if (c->type!=TOKEN_DEFAULT_FRAME) {
    syntaxerror(c->pos,"default_frame expected");
  }
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_colormap */
  if (c->type!=TOKEN_DEFAULT_COLORMAP) {
    syntaxerror(c->pos,"default_colormap expected");
  }
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_skin */
  if (c->type!=TOKEN_DEFAULT_SKIN) {
    syntaxerror(c->pos,"default_skin expected");
  } 
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_origin */
  c2=c->next; /* default_angles */

  /* default_origin and default_angles are combined */
  a=c->down;
  a2=c2->down;

  do_simple_argument_write_bin(a, m); a=a->next;
  do_simple_argument_write_bin(a2,m); a2=a2->next;
  do_simple_argument_write_bin(a, m); a=a->next;
  do_simple_argument_write_bin(a2,m); a2=a2->next;
  do_simple_argument_write_bin(a, m);
  do_simple_argument_write_bin(a2,m);
}

void do_dem_spawnbaseline_message_write_bin(node* n, BB_t* m)
{
  node* c;
  node* c2;
  node* a;
  node* a2;
  char ts[1000];

  /* 
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    spawnbaseline is very easy: all commands have to be there
  */

  /* fprintf(stderr,"b"); */
  /* at first: the message id */
  WriteByte(m,node_token_id(n->type));

  /* message -> commands */
  c=n->down; /* entity */
  if (c==NULL) syntaxerror(n->pos, "command entity missing");
  if (c->type!=TOKEN_ENTITY) {
    sprintf(ts, "command is %s, should be %s", 
                node_token_string(c->type), 
                node_token_string(TOKEN_ENTITY));
    syntaxerror(c->pos, ts);
  }
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_modelindex */
  if (c==NULL) syntaxerror(n->pos, "command default_modelindex missing");
  if (c->type!=TOKEN_DEFAULT_MODELINDEX) {
    syntaxerror(c->pos, "default_modelindex expected");
  }
  do_simple_argument_write_bin(c->down,m);
 
  c=c->next; /* default_frame */
  if (c==NULL) syntaxerror(n->pos, "command default_frame missing");
  if (c->type!=TOKEN_DEFAULT_FRAME) {
    syntaxerror(c->pos, "default_frame expected");
  }
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_colormap */
  if (c==NULL) syntaxerror(n->pos, "command default_colormap missing");
  if (c->type!=TOKEN_DEFAULT_COLORMAP) {
    syntaxerror(c->pos, "default_colormap expected");
  }
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_skin */
  if (c==NULL) syntaxerror(n->pos, "command default_skin missing");
  if (c->type!=TOKEN_DEFAULT_SKIN) {
    syntaxerror(c->pos, "default_skin expected");
  }
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* default_origin */
  if (c==NULL) syntaxerror(n->pos, "command default_origin missing");
  if (c->type!=TOKEN_DEFAULT_ORIGIN) {
    syntaxerror(c->pos, "default_origin expected");
  }

  c2=c->next; /* default_angles */
  if (c2==NULL) syntaxerror(n->pos, "command default_angles missing");
  if (c2->type!=TOKEN_DEFAULT_ANGLES) {
    syntaxerror(c->pos, "default_angles expected");
  }

  /* default_origin and default_angles are combined */
  a=c->down;
  a2=c2->down;

  do_simple_argument_write_bin(a, m); a=a->next;
  do_simple_argument_write_bin(a2,m); a2=a2->next;
  do_simple_argument_write_bin(a, m); a=a->next;
  do_simple_argument_write_bin(a2,m); a2=a2->next;
  do_simple_argument_write_bin(a, m); 
  do_simple_argument_write_bin(a2,m);
}

void do_dem_updateentity_message_write_bin(node* n, BB_t* m)
{
  node* c;
  long mask;
  float origin_x, origin_y, origin_z;
  float angles_1, angles_2, angles_3;
  short entity;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    updateentity: bit mask
  */

  origin_x = origin_y = origin_z = 0.0;
  angles_1 = angles_2 = angles_3 = 0.0;

  /* mask */
  for (c=n->down, mask=0; c!=NULL ; c=c->next) {
    switch (c->type) {
      case TOKEN_MODELINDEX:   mask |= 0x0400; break;
      case TOKEN_FRAME:        mask |= 0x0040; break;
      case TOKEN_COLORMAP:     mask |= 0x0800; break;
      case TOKEN_SKIN:         mask |= 0x1000; break;
      case TOKEN_EFFECTS:      mask |= 0x2000; break;
      case TOKEN_ORIGIN_X:     mask |= 0x0002; break;
      case TOKEN_ORIGIN_Y:     mask |= 0x0004; break;
      case TOKEN_ORIGIN_Z:     mask |= 0x0008; break;
      case TOKEN_ANGLES_1:     mask |= 0x0100; break;
      case TOKEN_ANGLES_2:     mask |= 0x0010; break;
      case TOKEN_ANGLES_3:     mask |= 0x0200; break;
      case TOKEN_NEW:          mask |= 0x0020; break;
    }
  }
  c=n->down;
  entity = (short)(*(long*)c->down->down);
  if (entity>0xFF) mask |= 0x4000;

  if (mask & 0xFF00) mask |= 0x0001;

  /* ID */
  WriteByte (m, (mask&0x7F) | 0x80);
  /* mask */
  if (mask & 0x0001) WriteByte(m,(mask>>8)&0xFF);

  if (entity>0xFF) {
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }
  else {
    WriteByte(m,(unsigned char)entity);
    c=c->next;
  }
  if (mask&0x0400) { /* modelindex */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }
  if (mask&0x0040) { /* frame */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }
  if (mask&0x0800) { /* colormap */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }
  if (mask&0x1000) { /* skin */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }
  if (mask&0x2000) { /* effects */
    do_simple_argument_write_bin(c->down, m);
    c=c->next;
  }

  if (mask&0x0002) { /* origin_x */
    origin_x = *(float*)c->down->down;
    c=c->next;
  }
  if (mask&0x0004) { /* origin_y */
    origin_y = *(float*)c->down->down;
    c=c->next;
  }
  if (mask&0x0008) { /* origin_z */
    origin_z = *(float*)c->down->down;
    c=c->next;
  }
  if (mask&0x0100) { /* angles_1 */
    angles_1 = *(float*)c->down->down;
    c=c->next;
  }
  if (mask&0x0010) { /* angles_2 */
    angles_2 = *(float*)c->down->down;
    c=c->next;
  }
  if (mask&0x0200) { /* angles_3 */
    angles_3 = *(float*)c->down->down;
    c=c->next;
  }

  if (mask&0x0002) WriteCoord(m,origin_x); 
  if (mask&0x0100) WriteAngle(m,angles_1);
  if (mask&0x0004) WriteCoord(m,origin_y);
  if (mask&0x0010) WriteAngle(m,angles_2);
  if (mask&0x0008) WriteCoord(m,origin_z);
  if (mask&0x0200) WriteAngle(m,angles_3);

  /* Here may come a new command. It is in the bit mask already */ 
}

/******************************************************************************/
/** DEM block editing *********************************************************/
/******************************************************************************/

void DEM_block_edit(node *n)
{
  node *ni;
  opt_t* opt;

  opt = glob_opt;

  if (n->type==TOKEN_TRACK) {
    /* CD track manipulation */
    /* nothing to do up to now */
  }
  else {
    /* data block manipulation */
    if (opt->option & opFixMulti) {

      /* repairs the multi-level play-back problem */
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
  } /* end block */
  
}

/*- file end udem.c ----------------------------------------------------------*/
