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

/* udm2-te1.c */

/* variable definition */

long entitytype;
long dest_entity = 0;
long entity = 0;
long nextid = 0;
long count = 0;
vec3_t start;
vec3_t dest_origin;
long type = 0;
vec3_t origin;
long flash_entity = 0;
vec3_t trace_endpos;
vec3_t pos1, pos2, pos3, pos4;
vec3_t movedir;
long style = 0;
long plat2flags = 0;
long wait = 0;
