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

/* udm2-te2.c */

/* read binary into variables */

entitytype = ReadByte(m);
switch (entitytype) {
  /* version problems */
  /* case DM2_TE_PLASMATRAIL: */
  case DM2_TE_GREENBLOOD_new:
    if (SERVERVERSION >= 32) /* game version >= 3.15 */
      goto impact_entity_1;
    else
      goto line_entity_1;
  break;
  /* case DM2_TE_GREENBLOOD_old: */
  case DM2_TE_BLUEHYPERBLASTER:
    if (SERVERVERSION >= 32) /* game version >= 3.15 */
      goto line_entity_1;
    else
      goto impact_entity_1;
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
    ReadPosition(m, origin);
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
  impact_entity_1:
    ReadPosition(m, origin);
    ReadDir(m, movedir);
  break;
  /* line entity */
  case DM2_TE_RAILTRAIL:
  case DM2_TE_BUBBLETRAIL:
  case DM2_TE_BFG_LASER:
  /* case DM2_TE_PLASMATRAIL: */
  /* case DM2_TE_BLUEHYPERBLASTER: */
  case DM2_TE_DEBUGTRAIL:
  case DM2_TE_BUBBLETRAIL2:
  line_entity_1:
    ReadPosition(m, origin);
    ReadPosition(m, trace_endpos);
  break;
  /* special entity */
  case DM2_TE_SPLASH:
  case DM2_TE_LASER_SPARKS:
  case DM2_TE_WELDING_SPARKS:
  case DM2_TE_TUNNEL_SPARKS:
    count = ReadByte(m);
    ReadPosition(m, origin);
    ReadDir(m, movedir);
    style = ReadByte(m);
  break;
  case DM2_TE_PARASITE_ATTACK:
  case DM2_TE_MEDIC_CABLE_ATTACK:
  case DM2_TE_HEATBEAM:
  case DM2_TE_MONSTER_HEATBEAM:
    entity = ReadShort(m);
    ReadPosition(m, origin);
    ReadPosition(m, trace_endpos);
  break;  
  case DM2_TE_GRAPPLE_CABLE:
    entity = ReadShort(m);
    ReadPosition(m, origin);
    ReadPosition(m, trace_endpos);
    ReadPosition(m, pos1);
  break;
  case DM2_TE_FLAME: /* Quake2 can't parse this! */
    entity = ReadShort(m);
    count = ReadShort(m);
    ReadPosition(m, start);
    ReadPosition(m, origin);
    ReadPosition(m, pos1);
    ReadPosition(m, pos2);
    ReadPosition(m, pos3);
    ReadPosition(m, pos4);
  break;
  case DM2_TE_LIGHTNING:
    dest_entity = ReadShort(m);
    entity = ReadShort(m);
    ReadPosition(m, dest_origin);
    ReadPosition(m, origin);
  break;
  case DM2_TE_FLASHLIGHT:
    ReadPosition(m, origin);
    flash_entity = ReadShort(m);
  break;
  case DM2_TE_FORCEWALL:
    ReadPosition(m, origin);
    ReadPosition(m, trace_endpos);
    style = ReadShort(m);
  break;
  case DM2_TE_STEAM:
    nextid = ReadShort(m);
    count = ReadByte(m);
    ReadPosition(m, origin);
    ReadDir(m, movedir);
    style = ReadByte(m);
    plat2flags = ReadShort(m);
    if (nextid != -1)
      wait = ReadLong(m);
  break;
  case DM2_TE_WIDOWBEAMOUT:
    type = ReadShort(m);
    ReadPosition(m, origin);
  break;
  case DM2_TE_RAILTRAIL2: /* senseless, I know */
  default:
    syserror(WDM2, DM2TOP->filename);
  break;
}
