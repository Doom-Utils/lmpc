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
|  lmpc.h  -  some definitions                                               |
\****************************************************************************/


#ifndef __LMPC_H
#define __LMPC_H


#define acInfo          0x01
#define ac2TXT          0x02
#define ac2BIN          0x04
#define acConvert       0x08
#define acInplace       0x10

#define opVersion       0x0001
#define opViewPlayer    0x0002
#define opAddWT         0x0004
#define opRemovePause   0x0008
#define opClearSave     0x0010
#define opSelectTics	0x0020
#define opDefineGame	0x0040
#define opMarkStep      0x0080
#define opFixMulti      0x0100

#define OLD_NEW         1
#define NEW_OLD         2

#define HEADER_START          0
#define HEADER                1
#define HEADER_END            2
#define DATA_START            3
#define DATA                  4
#define DATA_END              5

#define LMP_INIT              1
#define LMP_END               2

typedef struct {
                 unsigned char	action;
                 long          	option;
                 unsigned char	versionbyte;
                 char*          versionstring;
                 unsigned char	mainplayer;
                 long		tic_start;
                 long		tic_end;
                 long		addwt;
                 int	        game;
		 int            gamecheck;
                 int		tic_step;
	           } opt_t;


#endif /* __LMPC_H */


/*- file end lmpc.h ----------------------------------------------------------*/
