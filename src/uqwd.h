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
    Erika-von-Brockdorff-Strasse 2
    04159 Leipzig
    Deutschland / Germany
    E-mail: Uwe.Girlich@itp.uni-leipzig.de */

/****************************************************************************\
|  uqwd.h - header file of uqwd.c, general QWD routines                      |
\****************************************************************************/


#ifndef __UQWD_H
#define __UQWD_H


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif


#define QUAKEWORLD 4096
#define H_QWD_HINT (1<<19)


#define H_QWD_SOUND			( H_QWD_HINT | 1 )
#define H_QWD_STOPSOUND			( H_QWD_HINT | 2 )
#define H_QWD_SPAWNSTATIC		( H_QWD_HINT | 3 )
#define H_QWD_SPAWNBASELINE		( H_QWD_HINT | 4 )
#define H_QWD_PLAYERINFO		( H_QWD_HINT | 5 )
#define H_QWD_NAILS			( H_QWD_HINT | 6 )
#define H_QWD_PACKETENTITIES		( H_QWD_HINT | 7 )
#define H_QWD_DELTAPACKETENTITIES	( H_QWD_HINT | 8 )

#define QWD_MAX_MODELS 256
#define QWD_MAX_SOUNDS 256
#define QWD_MAX_MESSAGE_SIZE 0x800

#include "node.h"
#include "binblock.h"


#define MASK_QWD_PACKET_EMPTY		0x0000
#define MASK_QWD_PACKET_REMOVE		0x4000
#define MASK_QWD_PACKET_MASK2		0x8000
#define MASK_QWD_PACKET_MODELINDEX	0x0004
#define MASK_QWD_PACKET_FRAME		0x2000
#define MASK_QWD_PACKET_COLORMAP	0x0008
#define MASK_QWD_PACKET_SKIN		0x0010
#define MASK_QWD_PACKET_EFFECTS		0x0020
#define MASK_QWD_PACKET_ORIGIN_X	0x0200
#define MASK_QWD_PACKET_ANGLES_1	0x0001
#define MASK_QWD_PACKET_ORIGIN_Y	0x0400
#define MASK_QWD_PACKET_ANGLES_2	0x1000
#define MASK_QWD_PACKET_ORIGIN_Z	0x0800
#define MASK_QWD_PACKET_ANGLES_3	0x0002

#define MASK_QWD_PLAYERINFO_EMPTY	0x0000
#define MASK_QWD_PLAYERINFO_PING	0x0001
#define MASK_QWD_PLAYERINFO_MASK2	0x0002
#define MASK_QWD_PLAYERINFO_ANGLES_1	0x01
#define MASK_QWD_PLAYERINFO_ANGLES_2_n	0x80
#define MASK_QWD_PLAYERINFO_ANGLES_3	0x02
#define MASK_QWD_PLAYERINFO_SPEED_X	0x04
#define MASK_QWD_PLAYERINFO_SPEED_Y	0x08
#define MASK_QWD_PLAYERINFO_SPEED_Z	0x10
#define MASK_QWD_PLAYERINFO_FLAG	0x20
#define MASK_QWD_PLAYERINFO_IMPULSE	0x40
#define MASK_QWD_PLAYERINFO_LOAD_o	0x80
#define MASK_QWD_PLAYERINFO_CSPEED_X	0x0004
#define MASK_QWD_PLAYERINFO_CSPEED_Y	0x0008
#define MASK_QWD_PLAYERINFO_CSPEED_Z	0x0010
#define MASK_QWD_PLAYERINFO_MODELINDEX	0x0020
#define MASK_QWD_PLAYERINFO_UK_BYTE6	0x0040
#define MASK_QWD_PLAYERINFO_WEAPON	0x0080
#define MASK_QWD_PLAYERINFO_WEAPONFRAME	0x0100
#define MASK_QWD_PLAYERINFO_UNKNOWNBITS 0xFE00


typedef struct {
  char* filename;
  size_t filesize;
  FILE* file;
  char* precache_models[QWD_MAX_MODELS+1];
  long nummodels;
  char* precache_sounds[QWD_MAX_SOUNDS+1];
  long numsounds;
  long lastHealth;
  long lastArmor;
  long lastMinusHealth;
  long lastMinusArmor;
  unsigned char playermodel;
  int frame;
  int serverversion;
} QWD_t;
  
extern token_t QWD_token[];

extern unsigned long isQWDbin(char *filename);
extern int isQWDtxt(char *filename);

extern void QWD_init(QWD_t* d, char *filename, char *mode);
extern void QWD_prepare_write_bin(QWD_t* d);
extern void QWD_open(QWD_t *d, char *mode);
extern void QWD_close(QWD_t *d);
extern void QWD_done(QWD_t* d);
extern void QWD_block_read_bin(QWD_t *d, BB_t* m);
extern node* QWD_bin_to_node(BB_t* m, int opt);

extern node* QWD_block_write_text(node* b);

extern node* QWD_block_write_bin(node* b);
extern void do_qwd_sound_message_write_bin(node* n, BB_t* m);
extern void do_qwd_stopsound_message_write_bin(node* n, BB_t* m);
extern void do_qwd_spawnstatic_message_write_bin(node* n, BB_t* m);
extern void do_qwd_spawnbaseline_message_write_bin(node* n, BB_t* m);
extern void do_qwd_playerinfo_message_write_bin(node* n, BB_t* m);
extern void do_qwd_nails_message_write_bin(node* n, BB_t* m);
extern void do_qwd_packetentities_message_write_bin(node* n, BB_t* m);
extern void do_qwd_deltapacketentities_message_write_bin(node* n, BB_t* m);

extern void QWD_block_edit(node *n);

      
#endif /* __UQWD_H */


/*-- file end uqwd.h --------------------------------------------------------*/
