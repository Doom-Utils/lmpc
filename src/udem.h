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
|  udem.h - header file of udem.c, general DEM routines                      |
\****************************************************************************/


#ifndef __UDEM_H
#define __UDEM_H


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif


#define QUAKE 1024
#define H_DEM_HINT (1<<18)


#define H_DEM_SOUND		( H_DEM_HINT | 1 )
#define H_DEM_CLIENTDATA	( H_DEM_HINT | 2 )
#define H_DEM_STOPSOUND		( H_DEM_HINT | 3 )
#define H_DEM_UPDATECOLORS	( H_DEM_HINT | 4 )
#define H_DEM_SPAWNSTATIC	( H_DEM_HINT | 5 )
#define H_DEM_SPAWNBASELINE	( H_DEM_HINT | 6 )
#define H_DEM_UPDATEENTITY	( H_DEM_HINT | 7 )


#include "node.h"
#include "binblock.h"


#define MAX_TRACK_SIZE 10
#define DEM_MAX_MESSAGE_SIZE BBB_MAX
#define DEM_MAX_MSGLEN 8000


typedef struct {
         int game;
         char* filename;
         size_t filesize;
         size_t datasize;
         FILE* file;
         size_t datastart;
         char** precache_models;
         long nummodels;
         char** precache_sounds;
         long numsounds;
         long lastHealth;
         long lastArmor;
         long lastMinusHealth;
         long lastMinusArmor;
         int frame;
         long tics;
} DEM_t;

extern token_t DEM_token[];

extern void DEM_init(DEM_t* d, char* filename, char* mode);
extern void DEM_prepare_write_bin(DEM_t* d);
extern node* do_track_read_bin(DEM_t* d);
extern void DEM_block_read_bin(DEM_t* d, BB_t* m);
extern node* DEM_bin_to_node(BB_t* m, int opt);
extern node* do_message_read_bin(BB_t* m);
extern void DEM_done(DEM_t* d);

extern unsigned long isDEMbin(char* filename);
extern int isDEMtxt(char* filename);

extern node* do_unknown_message_read_bin(BB_t* m, unsigned char id);
#define     c_bad              0x00
extern node* do_bad_message_read_bin(void);
#define     c_nop              0x01
extern node* do_nop_message_read_bin(void);
#define     c_disconnect       0x02
extern node* do_disconnect_message_read_bin(void);
#define     c_updatestat       0x03
extern node* do_updatestat_message_read_bin(BB_t* m);
#define     c_version          0x04
extern node* do_version_message_read_bin(BB_t* m);
#define     c_setview          0x05
extern node* do_setview_message_read_bin(BB_t* m);
#define     c_sound            0x06
extern node* do_sound_message_read_bin(BB_t* m);
#define     c_time             0x07
extern node* do_time_message_read_bin(BB_t* m);
#define     c_print            0x08
extern node* do_print_message_read_bin(BB_t* m);
#define     c_stufftext        0x09
extern node* do_stufftext_message_read_bin(BB_t* m);
#define     c_setangle         0x0A
extern node* do_setangle_message_read_bin(BB_t* m);
#define     c_serverinfo       0x0B
extern node* do_serverinfo_message_read_bin(BB_t* m);
#define     c_lightstyle       0x0C
extern node* do_lightstyle_message_read_bin(BB_t* m);
#define     c_updatename       0x0D
extern node* do_updatename_message_read_bin(BB_t* m);
#define     c_updatefrags      0x0E
extern node* do_updatefrags_message_read_bin(BB_t* m);
#define     c_clientdata       0x0F
extern node* do_clientdata_message_read_bin(BB_t* m);
#define     c_stopsound        0x10
extern node* do_stopsound_message_read_bin(BB_t* m);
#define     c_updatecolors     0x11
extern node* do_updatecolors_message_read_bin(BB_t* m);
#define     c_particle         0x12
extern node* do_particle_message_read_bin(BB_t* m);
#define     c_damage           0x13
extern node* do_damage_message_read_bin(BB_t* m);
#define     c_spawnstatic      0x14
extern node* do_spawnstatic_message_read_bin(BB_t* m);
#define     c_spawnbinary      0x15
extern node* do_spawnbinary_message_read_bin(void);
#define     c_spawnbaseline    0x16
extern node* do_spawnbaseline_message_read_bin(BB_t* m);
#define     c_temp_entity      0x17
extern node* do_temp_entity_message_read_bin(BB_t* m);
#define     c_setpause         0x18
extern node* do_setpause_message_read_bin(BB_t* m);
#define     c_signonnum        0x19
extern node* do_signonnum_message_read_bin(BB_t* m);
#define     c_centerprint      0x1A
extern node* do_centerprint_message_read_bin(BB_t* m);
#define     c_killedmonster    0x1B
extern node* do_killedmonster_message_read_bin(void);
#define     c_foundsecret      0x1C
extern node* do_foundsecret_message_read_bin(void);
#define     c_spawnstaticsound 0x1D
extern node* do_spawnstaticsound_message_read_bin(BB_t* m);
#define     c_intermission     0x1E
extern node* do_intermission_message_read_bin(void);
#define     c_finale           0x1F
extern node* do_finale_message_read_bin(BB_t* m);
#define     c_cdtrack          0x20
extern node* do_cdtrack_message_read_bin(BB_t* m);
#define     c_sellscreen       0x21
extern node* do_sellscreen_message_read_bin(void);
#define     c_cutscene         0x22
extern node* do_cutscene_message_read_bin(BB_t* m);

extern node* do_updateentity_message_read_bin(BB_t* m, long mask);

extern void do_node_write_text(node* b, int depth);
extern node* DEM_block_write_text(node* b);

extern node* DEM_block_write_bin(node* b);

extern void do_dem_sound_message_write_bin(node* n, BB_t* m);
extern void do_dem_clientdata_message_write_bin(node* n, BB_t* m);
extern void do_dem_stopsound_message_write_bin(node* n, BB_t* m);
extern void do_dem_updatecolors_message_write_bin(node* n, BB_t* m);
extern void do_dem_spawnstatic_message_write_bin(node* n, BB_t* m);
extern void do_dem_spawnbaseline_message_write_bin(node* n, BB_t* m);
extern void do_dem_updateentity_message_write_bin(node* n, BB_t* m);


extern void DEM_block_edit(node *n);

#endif /* __UDEM_H */


/*-- file end udem.h --------------------------------------------------------*/
