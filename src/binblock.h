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
|  binblock.h - header file of binblock.c, binary block operations           | 
\****************************************************************************/


#ifndef __BINBLOCK_H
#define __BINBLOCK_H


/*
 * The size of a block should be dynamically determined. Maybe next time.
 */
#define BBB_MAX 65536


#include "node.h"


#define H_SIMPLE 1 
#define H_BYTE 2
#define H_SHORT 3
#define H_LONG 4
#define H_COORD 5
#define H_FLOAT 6
#define H_ANGLE 8
#define H_STRING 10
#define H_STRINGLIST 11
#define H_VEL 12
#define H_VOL 13
#define H_ATTENUATION 14
#define H_UNKNOWN 15
#define H_BYTEFLOAT 22
#define H_NOTHING 24
#define H_LSB 25
#define H_TIMEOFS 26
#define H_ANGLE16 27
#define H_DIR 28
#define H_OFFSET 29
#define H_BLEND 30
#define H_SB 31
#define H_UNICAST 32

#define MASK_UNICAST 0x80

/* the size of a string (stored as in the files) */
#define MAX_STRING_SIZE 0x800

/* the size of a string (stored with non-printable chars octal expanded) */
#define MAX_STRING_SPACE (4 * MAX_STRING_SIZE)


typedef struct {
         long size;
         unsigned char buffer[BBB_MAX];
         unsigned char* p;
         unsigned char* end;
         long base;
         void* top;
         long hint;
} BB_t;


typedef struct {
	int	client;
	int	use;
#define UC_NOT 0
#define UC_THIS 1
#define UC_NEXT 2
} unicast_t;


extern unsigned char get_next_unsigned_char (BB_t* m);
extern void          put_unsigned_char      (BB_t* m, unsigned char a);
extern char          get_next_signed_char   (BB_t* m);
extern void          put_signed_char        (BB_t* m, char a);
extern short         get_next_short         (BB_t* m);
extern void          put_short              (BB_t* m, short a);
extern long          get_next_long          (BB_t* m);
extern void          put_long               (BB_t* m, long a);
extern float         get_next_float         (BB_t* m);
extern void          put_float              (BB_t* m, float a);
extern char*         get_next_string        (BB_t* m, char* buffer);
extern void          put_string             (BB_t* m, char* a);


extern long ReadByte(BB_t* m);
extern void WriteByte(BB_t* m, unsigned char a);
extern long ReadChar(BB_t* m);
extern void WriteChar(BB_t* m, char a);
extern long ReadShort(BB_t* m);
extern void WriteShort(BB_t* m, short a);
extern long ReadLong(BB_t* m);
extern void WriteLong(BB_t* m, long a);
extern long ReadLongBE(BB_t* m);
extern void WriteLongBE(BB_t* m, long a);
extern float ReadFloat(BB_t* m);
extern void WriteFloat(BB_t* m, float a);
extern float ReadAngle(BB_t* m);
extern void WriteAngle(BB_t* m, float a);
extern float ReadCoord(BB_t* m);
extern void WriteCoord(BB_t* m, float a);
extern void WriteVel(BB_t* m, float a);
extern void WriteVol(BB_t* m, float a);
extern void WriteAttenuation(BB_t* m, float a);
extern void WriteTimeofs(BB_t* m, float a);
extern char* ReadString(BB_t* m, char* buffer);
extern char* ReadUnparsedString(BB_t* m, char* buffer);
extern void WriteString(BB_t* m, char* a);
extern float ReadAngle16(BB_t* m);
extern void WriteAngle16(BB_t* m, float a);
extern float* ReadPosition(BB_t* m, float* a);
extern void WritePosition(BB_t* m, float* a);
extern float* ReadDir(BB_t* m, float* a);
extern void WriteDir(BB_t* m, float* a);
extern float ReadBlend(BB_t* m);
extern void WriteBlend(BB_t* m, float a);
extern float* ReadBlendVec(BB_t* m, float* a);
extern void WriteBlendVec(BB_t* m, float* a);
extern float ReadOffset(BB_t* m);
extern void WriteOffset(BB_t* m, float a);
extern float* ReadOffsetVec(BB_t* m, float* a);
extern void WriteOffsetVec(BB_t* m, float* a);

extern void do_simple_message_write_bin(node* n, BB_t* m);
extern void do_simple_message_write_bin_unicast(node* n, BB_t* m, unicast_t* u);
extern void do_unknown_message_write_bin(node* n, BB_t* m);
extern void do_simple_arguments_write_bin(node* a, BB_t* m);
extern void do_simple_argument_write_bin(node* a, BB_t* m);

#endif /* __BINBLOCK_H */


/*-- file end binblock.h ----------------------------------------------------*/
