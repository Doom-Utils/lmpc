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

/*
 * This file was created automatically out of the DM2 specs.
 * This ensures the necessary equvivalence and simplifies my task of writing
 * corresponding specs and programs.
 */

/* udm2-te3.c */

/* create tree from the variables */

tn = node_link(tn,node_command_init(TOKEN_ENTITYTYPE,V_INT,H_BYTE,
                  NODE_VALUE_INT_dup(entitytype),0));
switch (entitytype) {
  /* version problems */
  /* case DM2_TE_PLASMATRAIL: */
  case DM2_TE_GREENBLOOD_new:
    if (SERVERVERSION >= 32) /* game version >= 3.15 */
      goto impact_entity_2;
    else
      goto line_entity_2;
  break;
  /* case DM2_TE_GREENBLOOD_old: */
  case DM2_TE_BLUEHYPERBLASTER:
    if (SERVERVERSION >= 32) /* game version >= 3.15 */
      goto line_entity_2;
    else
      goto impact_entity_2;
  break;
  /* point entity */
  case DM2_TE_EXPLOSION1:
  case DM2_TE_EXPLOSION2:
  case DM2_TE_ROCKET_EXPLOSION:
  case DM2_TE_GRENADE_EXPLOSION:
  case DM2_TE_ROCKET_EXPLOSION_WATER:
  case DM2_TE_GRENADE_EXPLOSION_WATER:
  case DM2_TE_BFG_EXPLOSION:
  case DM2_TE_BFG_BIGEXPLOSION:
  case DM2_TE_BOSSTPORT:
  case DM2_TE_PLASMA_EXPLOSION:
  case DM2_TE_PLAIN_EXPLOSION:
  case DM2_TE_CHAINFIST_SMOKE:
  case DM2_TE_TRACKER_EXPLOSION:
  case DM2_TE_TELEPORT_EFFECT:
  case DM2_TE_DBALL_GOAL:
  case DM2_TE_NUKEBLAST:
  case DM2_TE_WIDOWSPLASH:
  case DM2_TE_EXPLOSION1_BIG:
  case DM2_TE_EXPLOSION1_NP:
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
  break;
  /* impact entity */
  case DM2_TE_GUNSHOT:
  case DM2_TE_BLOOD:
  case DM2_TE_BLASTER:
  case DM2_TE_SHOTGUN:
  case DM2_TE_SPARKS:
  case DM2_TE_SCREEN_SPARKS:
  case DM2_TE_SHIELD_SPARKS:
  case DM2_TE_BULLET_SPARKS:
  /* case DM2_TE_GREENBLOOD_new: */
  /* case DM2_TE_GREENBLOOD_old: */
  case DM2_TE_BLASTER2:
  case DM2_TE_MOREBLOOD:
  case DM2_TE_HEATBEAM_SPARKS:
  case DM2_TE_HEATBEAM_STEAM:
  case DM2_TE_ELECTRIC_SPARKS:
  case DM2_TE_FLECHETTE:
  impact_entity_2:
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_MOVEDIR,V_FLOAT,H_DIR,
                      NODE_VALUE_FLOAT_dup(movedir[0]),
                      NODE_VALUE_FLOAT_dup(movedir[1]),
                      NODE_VALUE_FLOAT_dup(movedir[2]),0));
  break;
  /* line entity */
  case DM2_TE_RAILTRAIL:
  case DM2_TE_BUBBLETRAIL:
  case DM2_TE_BFG_LASER:
  /* case DM2_TE_PLASMATRAIL: */
  /* case DM2_TE_BLUEHYPERBLASTER: */
  case DM2_TE_DEBUGTRAIL:
  case DM2_TE_BUBBLETRAIL2:
  line_entity_2:
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_TRACE_ENDPOS,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(trace_endpos[0]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[1]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[2]),0));
  break;
  /* special entity */
  case DM2_TE_SPLASH:
  case DM2_TE_LASER_SPARKS:
  case DM2_TE_WELDING_SPARKS:
  case DM2_TE_TUNNEL_SPARKS:
    tn = node_link(tn,node_command_init(TOKEN_COUNT,V_INT,H_BYTE,
                      NODE_VALUE_INT_dup(count),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_MOVEDIR,V_FLOAT,H_DIR,
                      NODE_VALUE_FLOAT_dup(movedir[0]),
                      NODE_VALUE_FLOAT_dup(movedir[1]),
                      NODE_VALUE_FLOAT_dup(movedir[2]),0));
    tn = node_link(tn,node_command_init(TOKEN_STYLE,V_INT,H_BYTE,
                      NODE_VALUE_INT_dup(style),0));
  break;
  case DM2_TE_PARASITE_ATTACK:
  case DM2_TE_MEDIC_CABLE_ATTACK:
  case DM2_TE_HEATBEAM:
  case DM2_TE_MONSTER_HEATBEAM:
    tn = node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(entity),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_TRACE_ENDPOS,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(trace_endpos[0]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[1]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[2]),0));
  break;  
  case DM2_TE_GRAPPLE_CABLE:
    tn = node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(entity),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_TRACE_ENDPOS,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(trace_endpos[0]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[1]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_POS1,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(pos1[0]),
                      NODE_VALUE_FLOAT_dup(pos1[1]),
                      NODE_VALUE_FLOAT_dup(pos1[2]),0));
  break;
  case DM2_TE_FLAME: /* Quake2 can't parse this! */
    tn = node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(entity),0));
    tn = node_link(tn,node_command_init(TOKEN_COUNT,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(count),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_START,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(start[0]),
                      NODE_VALUE_FLOAT_dup(start[1]),
                      NODE_VALUE_FLOAT_dup(start[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_POS1,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(pos1[0]),
                      NODE_VALUE_FLOAT_dup(pos1[1]),
                      NODE_VALUE_FLOAT_dup(pos1[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_POS2,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(pos2[0]),
                      NODE_VALUE_FLOAT_dup(pos2[1]),
                      NODE_VALUE_FLOAT_dup(pos2[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_POS3,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(pos3[0]),
                      NODE_VALUE_FLOAT_dup(pos3[1]),
                      NODE_VALUE_FLOAT_dup(pos3[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_POS4,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(pos4[0]),
                      NODE_VALUE_FLOAT_dup(pos4[1]),
                      NODE_VALUE_FLOAT_dup(pos4[2]),0));
  break;
  case DM2_TE_LIGHTNING:
    tn = node_link(tn,node_command_init(TOKEN_DEST_ENTITY,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(dest_entity),0));
    tn = node_link(tn,node_command_init(TOKEN_ENTITY,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(entity),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_DEST_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(dest_origin[0]),
                      NODE_VALUE_FLOAT_dup(dest_origin[1]),
                      NODE_VALUE_FLOAT_dup(dest_origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
  break;
  case DM2_TE_FLASHLIGHT:
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_command_init(TOKEN_FLASH_ENTITY,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(flash_entity),0));
  break;
  case DM2_TE_FORCEWALL:
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_TRACE_ENDPOS,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(trace_endpos[0]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[1]),
                      NODE_VALUE_FLOAT_dup(trace_endpos[2]),0));
    tn = node_link(tn,node_command_init(TOKEN_STYLE,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(style),0));
  break;
  case DM2_TE_STEAM:
    tn = node_link(tn,node_command_init(TOKEN_NEXTID,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(nextid),0));
    tn = node_link(tn,node_command_init(TOKEN_COUNT,V_INT,H_BYTE,
                      NODE_VALUE_INT_dup(count),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_MOVEDIR,V_FLOAT,H_DIR,
                      NODE_VALUE_FLOAT_dup(movedir[0]),
                      NODE_VALUE_FLOAT_dup(movedir[1]),
                      NODE_VALUE_FLOAT_dup(movedir[2]),0));
    tn = node_link(tn,node_command_init(TOKEN_STYLE,V_INT,H_BYTE,
                      NODE_VALUE_INT_dup(style),0));
    tn = node_link(tn,node_command_init(TOKEN_PLAT2FLAGS,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(plat2flags),0));
    if (nextid != -1)
      tn = node_link(tn,node_command_init(TOKEN_WAIT,V_INT,H_LONG,
                        NODE_VALUE_INT_dup(wait),0));
  break;
  case DM2_TE_WIDOWBEAMOUT:
    tn = node_link(tn,node_command_init(TOKEN_TYPE,V_INT,H_SHORT,
                      NODE_VALUE_INT_dup(type),0));
    tn = node_link(tn,node_triple_command_init(TOKEN_ORIGIN,V_FLOAT,H_COORD,
                      NODE_VALUE_FLOAT_dup(origin[0]),
                      NODE_VALUE_FLOAT_dup(origin[1]),
                      NODE_VALUE_FLOAT_dup(origin[2]),0));
  break;
  case DM2_TE_RAILTRAIL2: /* senseless, I know */
  default:
    syserror(WDM2, DM2TOP->filename);
  break;
}
