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
|  udm2.h - header file of udm2.c, general DM2 routines                      |
\****************************************************************************/


#ifndef __UDM2_H
#define __UDM2_H


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif


#define QUAKE2 8192
#define H_DM2_HINT (1<<20)


#define H_DM2_SOUND		( H_DM2_HINT | 1)
#define H_DM2_PLAYERINFO	( H_DM2_HINT | 2)
#define H_DM2_SPAWNBASELINE	( H_DM2_HINT | 3)
#define H_DM2_PACKETENTITIES	( H_DM2_HINT | 4)
#define H_DM2_FRAME		( H_DM2_HINT | 5)


#define DM2_MAX_MESSAGE_SIZE 1400


#define DM2_RECORD_CLIENT 0x00000001
#define DM2_RECORD_SERVER 0x00000002
#define DM2_RECORD_RELAY  0x00000080


/*
   All of the following constans are taken form q_shared.h and they got a
   DM2_ prefix because LMPC is a multi-protocol program.
*/
#define DM2_MAX_CLIENTS         256
#define DM2_MAX_LIGHTSTYLES     256
#define DM2_MAX_MODELS          256
#define DM2_MAX_SOUNDS          256
#define DM2_MAX_IMAGES          256
#define DM2_MAX_ITEMS           256
#define DM2_CS_NAME             0
#define DM2_CS_CDTRACK          1
#define DM2_CS_SKY              2
#define DM2_CS_SKYAXIS          3
#define DM2_CS_SKYROTATE        4
#define DM2_CS_STATUSBAR        5
#define DM2_CS_MAXCLIENTS	30
#define DM2_CS_MAPCHECKSUM      31
#define DM2_CS_MODELS           32
#define DM2_CS_SOUNDS           (DM2_CS_MODELS+DM2_MAX_MODELS)
#define DM2_CS_IMAGES           (DM2_CS_SOUNDS+DM2_MAX_SOUNDS)
#define DM2_CS_LIGHTS           (DM2_CS_IMAGES+DM2_MAX_IMAGES)
#define DM2_CS_ITEMS            (DM2_CS_LIGHTS+DM2_MAX_LIGHTSTYLES)
#define DM2_CS_PLAYERSKINS      (DM2_CS_ITEMS+DM2_MAX_ITEMS)
#define DM2_MAX_CONFIGSTRINGS   (DM2_CS_PLAYERSKINS+DM2_MAX_CLIENTS)


#define DM2_TE_GUNSHOT 0
#define DM2_TE_BLOOD 1
#define DM2_TE_BLASTER 2
#define DM2_TE_RAILTRAIL 3
#define DM2_TE_SHOTGUN 4
#define DM2_TE_EXPLOSION1 5
#define DM2_TE_EXPLOSION2 6
#define DM2_TE_ROCKET_EXPLOSION 7
#define DM2_TE_GRENADE_EXPLOSION 8
#define DM2_TE_SPARKS 9
#define DM2_TE_SPLASH 10
#define DM2_TE_BUBBLETRAIL 11
#define DM2_TE_SCREEN_SPARKS 12
#define DM2_TE_SHIELD_SPARKS 13
#define DM2_TE_BULLET_SPARKS 14
#define DM2_TE_LASER_SPARKS 15
#define DM2_TE_PARASITE_ATTACK 16
#define DM2_TE_ROCKET_EXPLOSION_WATER 17
#define DM2_TE_GRENADE_EXPLOSION_WATER 18
#define DM2_TE_MEDIC_CABLE_ATTACK 19
#define DM2_TE_BFG_EXPLOSION 20
#define DM2_TE_BFG_BIGEXPLOSION 21
#define DM2_TE_BOSSTPORT 22
#define DM2_TE_BFG_LASER 23
#define DM2_TE_GRAPPLE_CABLE 24
#define DM2_TE_WELDING_SPARKS 25
#define DM2_TE_PLASMATRAIL 26
#define DM2_TE_GREENBLOOD_new 26
#define DM2_TE_GREENBLOOD_old 27
#define DM2_TE_BLUEHYPERBLASTER 27
#define DM2_TE_PLASMA_EXPLOSION 28
#define DM2_TE_TUNNEL_SPARKS 29
#define DM2_TE_BLASTER2 30
#define DM2_TE_RAILTRAIL2 31
#define DM2_TE_FLAME 32
#define DM2_TE_LIGHTNING 33
#define DM2_TE_DEBUGTRAIL 34
#define DM2_TE_PLAIN_EXPLOSION 35
#define DM2_TE_FLASHLIGHT 36
#define DM2_TE_FORCEWALL 37
#define DM2_TE_HEATBEAM 38
#define DM2_TE_MONSTER_HEATBEAM 39
#define DM2_TE_STEAM 40
#define DM2_TE_BUBBLETRAIL2 41
#define DM2_TE_MOREBLOOD 42
#define DM2_TE_HEATBEAM_SPARKS 43
#define DM2_TE_HEATBEAM_STEAM 44
#define DM2_TE_CHAINFIST_SMOKE 45
#define DM2_TE_ELECTRIC_SPARKS 46
#define DM2_TE_TRACKER_EXPLOSION 47
#define DM2_TE_TELEPORT_EFFECT 48
#define DM2_TE_DBALL_GOAL 49
#define DM2_TE_WIDOWBEAMOUT 50
#define DM2_TE_NUKEBLAST 51
#define DM2_TE_WIDOWSPLASH 52
#define DM2_TE_EXPLOSION1_BIG 53
#define DM2_TE_EXPLOSION1_NP 54
#define DM2_TE_FLECHETTE 55


#define DM2_STAT_HEALTH_ICON 0
#define DM2_STAT_HEALTH 1
#define DM2_STAT_AMMO_ICON 2
#define DM2_STAT_AMMO 3
#define DM2_STAT_ARMOR_ICON 4
#define DM2_STAT_ARMOR 5
#define DM2_STAT_SELECTED_ICON 6
#define DM2_STAT_PICKUP_ICON 7
#define DM2_STAT_PICKUP_STRING 8
#define DM2_STAT_TIMER_ICON 9
#define DM2_STAT_TIMER 10
#define DM2_STAT_HELPICON 11
#define DM2_STAT_SELECTED_ITEM 12
#define DM2_STAT_LAYOUTS 13
#define DM2_STAT_FRAGS 14
#define DM2_STAT_FLASHES 15
#define DM2_MAX_STATS 32


#define MASK_DM2_SOUND_EMPTY		0x00
#define MASK_DM2_SOUND_VOL		0x01
#define MASK_DM2_SOUND_ATTENUATION	0x02
#define MASK_DM2_SOUND_TIMEOFS		0x10
#define MASK_DM2_SOUND_ENTITY		0x08
#define MASK_DM2_SOUND_CHANNEL		0x08
#define MASK_DM2_SOUND_ORIGIN		0x04
#define MASK_DM2_SOUND_KNOWNBITS	0x1F
#define MASK_DM2_SOUND_UNKNOWNBITS	0xE0


#define MASK_DM2_PLAYERINFO_EMPTY	0x0000
#define MASK_DM2_PLAYERINFO_PM_TYPE	0x0001
#define MASK_DM2_PLAYERINFO_ORIGIN	0x0002
#define MASK_DM2_PLAYERINFO_VELOCITY	0x0004
#define MASK_DM2_PLAYERINFO_PM_TIME	0x0008
#define MASK_DM2_PLAYERINFO_PM_FLAGS	0x0010
#define MASK_DM2_PLAYERINFO_GRAVITY	0x0020
#define MASK_DM2_PLAYERINFO_DELTA_ANGLES	0x0040
#define MASK_DM2_PLAYERINFO_VIEWOFFSET	0x0080
#define MASK_DM2_PLAYERINFO_VIEWANGLES	0x0100
#define MASK_DM2_PLAYERINFO_KICK_ANGLES	0x0200
#define MASK_DM2_PLAYERINFO_GUNINDEX	0x1000
#define MASK_DM2_PLAYERINFO_GUNFRAME	0x2000
#define MASK_DM2_PLAYERINFO_GUNOFFSET	0x2000
#define MASK_DM2_PLAYERINFO_GUNANGLES	0x2000
#define MASK_DM2_PLAYERINFO_BLEND	0x0400
#define MASK_DM2_PLAYERINFO_FOV		0x0800
#define MASK_DM2_PLAYERINFO_RDFLAGS	0x4000


#define MASK_DM2_PACKET_EMPTY          0x00000000
#define MASK_DM2_PACKET_PART2_USED     0x0000FF00
#define MASK_DM2_PACKET_PART2          0x00000080
#define MASK_DM2_PACKET_PART3_USED     0x00FF0000
#define MASK_DM2_PACKET_PART3          0x00008000
#define MASK_DM2_PACKET_PART4_USED     0xFF000000
#define MASK_DM2_PACKET_PART4          0x00800000
#define MASK_DM2_PACKET_ENTITY_SHORT   0x00000100
#define MASK_DM2_PACKET_REMOVE         0x00000040
#define MASK_DM2_PACKET_MODELINDEX     0x00000800
#define MASK_DM2_PACKET_MODELINDEX2    0x00100000
#define MASK_DM2_PACKET_MODELINDEX3    0x00200000
#define MASK_DM2_PACKET_MODELINDEX4    0x00400000
#define MASK_DM2_PACKET_FRAME_BYTE     0x00000010
#define MASK_DM2_PACKET_FRAME_SHORT    0x00020000
#define MASK_DM2_PACKET_FRAME          (MASK_DM2_PACKET_FRAME_BYTE|MASK_DM2_PACKET_FRAME_SHORT)
#define MASK_DM2_PACKET_SKIN_BYTE      0x00010000
#define MASK_DM2_PACKET_SKIN_SHORT     0x02000000
#define MASK_DM2_PACKET_SKIN_LONG      (MASK_DM2_PACKET_SKIN_BYTE|MASK_DM2_PACKET_SKIN_SHORT)
#define MASK_DM2_PACKET_SKIN           (MASK_DM2_PACKET_SKIN_BYTE|MASK_DM2_PACKET_SKIN_SHORT|MASK_DM2_PACKET_SKIN_LONG)
#define MASK_DM2_PACKET_EFFECTS_BYTE   0x00004000
#define MASK_DM2_PACKET_EFFECTS_SHORT  0x00080000
#define MASK_DM2_PACKET_EFFECTS_LONG   (MASK_DM2_PACKET_EFFECTS_BYTE|MASK_DM2_PACKET_EFFECTS_SHORT)
#define MASK_DM2_PACKET_RENDERFX_BYTE  0x00001000
#define MASK_DM2_PACKET_RENDERFX_SHORT 0x00040000
#define MASK_DM2_PACKET_RENDERFX_LONG  (MASK_DM2_PACKET_RENDERFX_BYTE|MASK_DM2_PACKET_RENDERFX_SHORT)
#define MASK_DM2_PACKET_ORIGIN_X       0x00000001
#define MASK_DM2_PACKET_ORIGIN_Y       0x00000002
#define MASK_DM2_PACKET_ORIGIN_Z       0x00000200
#define MASK_DM2_PACKET_ANGLES_1       0x00000400
#define MASK_DM2_PACKET_ANGLES_2       0x00000004
#define MASK_DM2_PACKET_ANGLES_3       0x00000008
#define MASK_DM2_PACKET_OLD_ORIGIN     0x01000000
#define MASK_DM2_PACKET_SOUND          0x04000000
#define MASK_DM2_PACKET_EVENT          0x00000020
#define MASK_DM2_PACKET_SOLID          0x08000000
#define MASK_DM2_PACKET_KNOWNBITS      0x0FFFDFFF
#define MASK_DM2_PACKET_UNKNOWNBITS    0xF0002000


#include "node.h"
#include "binblock.h"

typedef struct {
  int index;
  char* note;
} cs_t;

typedef struct {
  char* filename;
  size_t filesize;
  FILE* file;
  char* config_string[DM2_MAX_CONFIGSTRINGS];
  int frame;
  int serverversion;
  int demotype;
} DM2_t;

typedef struct {
  long index;
  char* name;
  int type;
} stat_t;

extern token_t DM2_token[];

extern void udm2_init(void);
extern void udm2_done(void);

extern unsigned long isDM2bin(char *filename);
extern int isDM2txt(char *filename);

extern void DM2_init(DM2_t* d, char *filename, char *mode);
extern void DM2_prepare_write_bin(DM2_t* d);
extern void DM2_open(DM2_t *d, char *mode);
extern void DM2_close(DM2_t *d);
extern void DM2_done(DM2_t* d);
extern void DM2_block_read_bin(DM2_t *d, BB_t* m);
extern node* DM2_bin_to_node(BB_t *m, int opt);
extern node* DM2_block_write_text(node* b);
extern node* DM2_block_write_bin(node* b);

extern void do_dm2_sound_message_write_bin(node* n, BB_t* m, unicast_t* u);
extern void do_dm2_playerinfo_message_write_bin(node* n, BB_t* m, unicast_t* u);
extern void do_dm2_spawnbaseline_message_write_bin(node* n, BB_t* m, unicast_t* u);
extern void do_dm2_packetentities_message_write_bin(node* n, BB_t* m, unicast_t* u);
extern int do_dm2_packetmask_write_bin(node* n, BB_t* m);
extern void do_dm2_packetdata_write_bin(node* n, BB_t* m, unsigned long mask);
extern void do_dm2_frame_message_write_bin(node* n, BB_t* m, unicast_t* u);

extern void DM2_block_edit(node *n);

      
#endif /* __UDM2_H */


/*-- file end udm2.h --------------------------------------------------------*/
