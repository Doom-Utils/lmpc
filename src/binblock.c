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
|  binblock.c - implementation, binary block operations                      |
\****************************************************************************/


#include "binblock.h"


unsigned char get_next_unsigned_char(BB_t* m)
{
  unsigned char a;

  a = *(unsigned char *)(m->p);
  m->p += 1;
  return a;
}

void put_unsigned_char(BB_t* m, unsigned char a)
{
  *(unsigned char*)(m->p) = a;
  m->p += 1;
}

char get_next_signed_char(BB_t* m)
{
  char a;

  a = *(char *)(m->p);
  m->p += 1;
  return a;
}

void put_signed_char(BB_t* m, char a)
{
  *(char *)(m->p) = a;
  m->p += 1;
}

short get_next_short(BB_t* m)
{
  long a;

  a =  (((unsigned char*)(m->p))[0]) + 
      ((((  signed char*)(m->p))[1])<<8);
  m->p += 2;
  return a;
}

void put_short(BB_t* m, short a)
{
  put_unsigned_char (m,((a     ) & 0xFF));
  put_signed_char   (m,((a >> 8) & 0xFF));
}

long get_next_long(BB_t* m)
{
  long a;

  a =  (((unsigned char*)(m->p))[0])      + 
      ((((unsigned char*)(m->p))[1])<<8 ) + 
      ((((unsigned char*)(m->p))[2])<<16) + 
      ((((  signed char*)(m->p))[3])<<24);
  m->p += 4;
  return a;
}

void put_long(BB_t* m, long a)
{
  put_unsigned_char (m,((a      ) & 0xFF));
  put_unsigned_char (m,((a >>  8) & 0xFF));
  put_unsigned_char (m,((a >> 16) & 0xFF));
  put_signed_char   (m,((a >> 24) & 0xFF));
}

float get_next_float(BB_t* m)
{
  /* endian-save float read, it works only with 32 bit long and 32 bit float */
  union {long l; float f;} a;
  a.l =  (((unsigned char*)(m->p))[0])      +
        ((((unsigned char*)(m->p))[1])<<8 ) +
        ((((unsigned char*)(m->p))[2])<<16) +
        ((((unsigned char*)(m->p))[3])<<24);
  m->p += 4;
  return a.f;
}

void put_float(BB_t* m, float a)
{
  union {long l; float f;} aa;

  aa.f = a;
  put_long(m,aa.l);  
}

char *get_next_string(BB_t* m, char *buffer)
{
  long i;
  char *b;

  for (i=0,b=buffer;i<0x7FF && *(m->p) ;i++,m->p++) {
    *b++ = *(m->p);
  };
  *b = '\0';
  m->p++;
  return buffer;
}

void put_string(BB_t* m, char* a)
{
  strcpy((char*)m->p, a);
  m->p += strlen((char*)m->p) + 1;
}


long ReadByte(BB_t* m)
{
  return (long) get_next_unsigned_char(m);
}

void WriteByte(BB_t* m, unsigned char a)
{
  put_unsigned_char(m, a);
}

long ReadChar(BB_t* m)
{
  return (long) get_next_signed_char(m);
}

void WriteChar(BB_t* m, char a)
{
  put_signed_char(m, a);
}

long ReadShort(BB_t* m)
{
  return (long) get_next_short(m);
}

void WriteShort(BB_t* m, short a)
{
  put_short(m, a);
}

long ReadLong(BB_t* m)
{
  return get_next_long(m);
}

void WriteLong(BB_t* m, long a)
{
  put_long(m, a);
}

long ReadLongBE(BB_t* m)
{
  return  (ReadByte(m)<<24)+
          (ReadByte(m)<<16)+
          (ReadByte(m)<< 8)+
          (ReadByte(m)    );
}
                                  
void WriteLongBE(BB_t* m, long a)
{
  WriteByte(m,(a>>24) & 0xFF);
  WriteByte(m,(a>>16) & 0xFF);
  WriteByte(m,(a>> 8) & 0xFF);
  WriteByte(m,(a    ) & 0xFF);
}

float ReadFloat(BB_t* m)
{
  return get_next_float(m);
}

void WriteFloat(BB_t* m, float a)
{
  put_float(m, a);
}

float ReadAngle(BB_t* m)
{
  return (float) ReadChar(m) / 256.0 * 360.0;
}

void WriteAngle(BB_t* m, float a)
{
  put_signed_char(m, (signed char)(a * 256.0 / 360.0));
}

float ReadCoord(BB_t* m)
{
  return (float) ReadShort(m) * 0.125;
}

void WriteCoord(BB_t* m, float a)
{
  put_short(m, (short)(a * 8.0));
}   

void WriteVel(BB_t* m, float a)
{
  put_signed_char(m, (signed char)(a * 16.0));
}

void WriteVol(BB_t* m, float a)
{
  put_unsigned_char(m, (unsigned char)(a * 255.0 + 0.5));
}

void WriteAttenuation(BB_t* m, float a)
{
  put_unsigned_char(m, (unsigned char)(a * 64.0));
}

void WriteTimeofs(BB_t* m, float a)
{
  put_unsigned_char(m, (unsigned char)(a * 1000.0));
}

char* ReadString(BB_t* m, char* buffer)
{
  return get_next_string(m, buffer); 
}

void WriteString(BB_t* m, char* a)
{
  put_string(m, a);
}

float ReadAngle16(BB_t* m)
{
  return (float) ReadShort(m) / 65536.0 * 360.0;
}
  
void WriteAngle16(BB_t* m, float a)
{
  put_short(m, (short)(a * 65536.0 / 360.0));
}

float* ReadPosition(BB_t* m, float* a)
{
  int i;
  
  for (i=0 ; i<3 ; i++) a[i] = ReadCoord(m);
  return a;
}

void WritePosition(BB_t* m, float* a)
{
  int i;
  
  for (i=0 ; i<3 ; i++) WriteCoord(m, a[i]);
}

/* 
 * I may remove the array and the following functions from here. 
 * They contain too many explicite Quake2 references 
 */

#define DM2_NUMVERTEXNORMALS 162
float avertexnormals[3*DM2_NUMVERTEXNORMALS] = {
  -0.525731027, 0, 0.850651026,
  -0.442862988, 0.238856003, 0.864188015,
  -0.295242012, 0, 0.955422997,
  -0.309017003, 0.5, 0.809017003,
  -0.162459999, 0.26286599, 0.951056004,
  0, 0, 1,
  0, 0.850651026, 0.525731027,
  -0.147621006, 0.71656698, 0.681717992,
  0.147621006, 0.71656698, 0.681717992,
  0, 0.525731027, 0.850651026,
  0.309017003, 0.5, 0.809017003,
  0.525731027, 0, 0.850651026,
  0.295242012, 0, 0.955422997,
  0.442862988, 0.238856003, 0.864188015,
  0.162459999, 0.26286599, 0.951056004,
  -0.681717992, 0.147621006, 0.71656698,
  -0.809017003, 0.309017003, 0.5,
  -0.587785006, 0.425325006, 0.688190997,
  -0.850651026, 0.525731027, 0,
  -0.864188015, 0.442862988, 0.238856003,
  -0.71656698, 0.681717992, 0.147621006,
  -0.688190997, 0.587785006, 0.425325006,
  -0.5, 0.809017003, 0.309017003,
  -0.238856003, 0.864188015, 0.442862988,
  -0.425325006, 0.688190997, 0.587785006,
  -0.71656698, 0.681717992, -0.147621006,
  -0.5, 0.809017003, -0.309017003,
  -0.525731027, 0.850651026, 0,
  0, 0.850651026, -0.525731027,
  -0.238856003, 0.864188015, -0.442862988,
  0, 0.955422997, -0.295242012,
  -0.26286599, 0.951056004, -0.162459999,
  0, 1, 0,
  0, 0.955422997, 0.295242012,
  -0.26286599, 0.951056004, 0.162459999,
  0.238856003, 0.864188015, 0.442862988,
  0.26286599, 0.951056004, 0.162459999,
  0.5, 0.809017003, 0.309017003,
  0.238856003, 0.864188015, -0.442862988,
  0.26286599, 0.951056004, -0.162459999,
  0.5, 0.809017003, -0.309017003,
  0.850651026, 0.525731027, 0,
  0.71656698, 0.681717992, 0.147621006,
  0.71656698, 0.681717992, -0.147621006,
  0.525731027, 0.850651026, 0,
  0.425325006, 0.688190997, 0.587785006,
  0.864188015, 0.442862988, 0.238856003,
  0.688190997, 0.587785006, 0.425325006,
  0.809017003, 0.309017003, 0.5,
  0.681717992, 0.147621006, 0.71656698,
  0.587785006, 0.425325006, 0.688190997,
  0.955422997, 0.295242012, 0,
  1, 0, 0,
  0.951056004, 0.162459999, 0.26286599,
  0.850651026, -0.525731027, 0,
  0.955422997, -0.295242012, 0,
  0.864188015, -0.442862988, 0.238856003,
  0.951056004, -0.162459999, 0.26286599,
  0.809017003, -0.309017003, 0.5,
  0.681717992, -0.147621006, 0.71656698,
  0.850651026, 0, 0.525731027,
  0.864188015, 0.442862988, -0.238856003,
  0.809017003, 0.309017003, -0.5,
  0.951056004, 0.162459999, -0.26286599,
  0.525731027, 0, -0.850651026,
  0.681717992, 0.147621006, -0.71656698,
  0.681717992, -0.147621006, -0.71656698,
  0.850651026, 0, -0.525731027,
  0.809017003, -0.309017003, -0.5,
  0.864188015, -0.442862988, -0.238856003,
  0.951056004, -0.162459999, -0.26286599,
  0.147621006, 0.71656698, -0.681717992,
  0.309017003, 0.5, -0.809017003,
  0.425325006, 0.688190997, -0.587785006,
  0.442862988, 0.238856003, -0.864188015,
  0.587785006, 0.425325006, -0.688190997,
  0.688190997, 0.587785006, -0.425325006,
  -0.147621006, 0.71656698, -0.681717992,
  -0.309017003, 0.5, -0.809017003,
  0, 0.525731027, -0.850651026,
  -0.525731027, 0, -0.850651026,
  -0.442862988, 0.238856003, -0.864188015,
  -0.295242012, 0, -0.955422997,
  -0.162459999, 0.26286599, -0.951056004,
  0, 0, -1,
  0.295242012, 0, -0.955422997,
  0.162459999, 0.26286599, -0.951056004,
  -0.442862988, -0.238856003, -0.864188015,
  -0.309017003, -0.5, -0.809017003,
  -0.162459999, -0.26286599, -0.951056004,
  0, -0.850651026, -0.525731027,
  -0.147621006, -0.71656698, -0.681717992,
  0.147621006, -0.71656698, -0.681717992,
  0, -0.525731027, -0.850651026,
  0.309017003, -0.5, -0.809017003,
  0.442862988, -0.238856003, -0.864188015,
  0.162459999, -0.26286599, -0.951056004,
  0.238856003, -0.864188015, -0.442862988,
  0.5, -0.809017003, -0.309017003,
  0.425325006, -0.688190997, -0.587785006,
  0.71656698, -0.681717992, -0.147621006,
  0.688190997, -0.587785006, -0.425325006,
  0.587785006, -0.425325006, -0.688190997,
  0, -0.955422997, -0.295242012,
  0, -1, 0,
  0.26286599, -0.951056004, -0.162459999,
  0, -0.850651026, 0.525731027,
  0, -0.955422997, 0.295242012,
  0.238856003, -0.864188015, 0.442862988,
  0.26286599, -0.951056004, 0.162459999,
  0.5, -0.809017003, 0.309017003,
  0.71656698, -0.681717992, 0.147621006,
  0.525731027, -0.850651026, 0,
  -0.238856003, -0.864188015, -0.442862988,
  -0.5, -0.809017003, -0.309017003,
  -0.26286599, -0.951056004, -0.162459999,
  -0.850651026, -0.525731027, 0,
  -0.71656698, -0.681717992, -0.147621006,
  -0.71656698, -0.681717992, 0.147621006,
  -0.525731027, -0.850651026, 0,
  -0.5, -0.809017003, 0.309017003,
  -0.238856003, -0.864188015, 0.442862988,
  -0.26286599, -0.951056004, 0.162459999,
  -0.864188015, -0.442862988, 0.238856003,
  -0.809017003, -0.309017003, 0.5,
  -0.688190997, -0.587785006, 0.425325006,
  -0.681717992, -0.147621006, 0.71656698,
  -0.442862988, -0.238856003, 0.864188015,
  -0.587785006, -0.425325006, 0.688190997,
  -0.309017003, -0.5, 0.809017003,
  -0.147621006, -0.71656698, 0.681717992,
  -0.425325006, -0.688190997, 0.587785006,
  -0.162459999, -0.26286599, 0.951056004,
  0.442862988, -0.238856003, 0.864188015,
  0.162459999, -0.26286599, 0.951056004,
  0.309017003, -0.5, 0.809017003,
  0.147621006, -0.71656698, 0.681717992,
  0, -0.525731027, 0.850651026,
  0.425325006, -0.688190997, 0.587785006,
  0.587785006, -0.425325006, 0.688190997,
  0.688190997, -0.587785006, 0.425325006,
  -0.955422997, 0.295242012, 0,
  -0.951056004, 0.162459999, 0.26286599,
  -1, 0, 0,
  -0.850651026, 0, 0.525731027,
  -0.955422997, -0.295242012, 0,
  -0.951056004, -0.162459999, 0.26286599,
  -0.864188015, 0.442862988, -0.238856003,
  -0.951056004, 0.162459999, -0.26286599,
  -0.809017003, 0.309017003, -0.5,
  -0.864188015, -0.442862988, -0.238856003,
  -0.951056004, -0.162459999, -0.26286599,
  -0.809017003, -0.309017003, -0.5,
  -0.681717992, 0.147621006, -0.71656698,
  -0.681717992, -0.147621006, -0.71656698,
  -0.850651026, 0, -0.525731027,
  -0.688190997, 0.587785006, -0.425325006,
  -0.587785006, 0.425325006, -0.688190997,
  -0.425325006, 0.688190997, -0.587785006,
  -0.425325006, -0.688190997, -0.587785006,
  -0.587785006, -0.425325006, -0.688190997,
  -0.688190997, -0.587785006, -0.425325006,
};

float* ReadDir(BB_t* m, float* a)
{
  int code;
  code = ReadByte(m);
  if (code >= DM2_NUMVERTEXNORMALS)
    /* no way to show an error in this routine */
    /* syserror(WDM2,"MSF_ReadDir: out of range"); */
    code=0;
  a[0] = avertexnormals[3 * code + 0];
  a[1] = avertexnormals[3 * code + 1];
  a[2] = avertexnormals[3 * code + 2];
  
  return a;
}


/* 
 * The routine is really shit! 
 * 486 float multiplications, 
 * 324 float summations
 * 162 float comparisons
 * But anyway: even Quake 2 does it the same way.
 */
void WriteDir(BB_t* m, float* a)
{
  int j;
  float maxdot;
  int maxdotindex;
    
  maxdot = -999999.0;
  maxdotindex = -1;
  
  for (j=0 ; j<DM2_NUMVERTEXNORMALS ; j++) {
    float dot;
    
    dot = avertexnormals[3*j + 0] * a[0] +
          avertexnormals[3*j + 1] * a[1] +
          avertexnormals[3*j + 2] * a[2];
    if (dot > maxdot) {
      maxdot = dot;
      maxdotindex = j;
    }
  }
  
  WriteByte(m, maxdotindex);
}

float ReadBlend(BB_t* m)
{
  return ReadByte(m) / 255.0;
}

void WriteBlend(BB_t* m, float a)
{
  WriteByte(m, (unsigned char)(a * 255.0));
}

float* ReadBlendVec(BB_t* m, float* a)
{
  int i;
  
  for ( i=0 ; i<4 ; i++ ) a[i] = ReadBlend(m);
  return a;
}

void WriteBlendVec(BB_t* m, float* a)
{
  int i;
  
  for ( i=0 ; i<4 ; i++ ) WriteBlend(m, a[i]);
}

float ReadOffset(BB_t* m)
{
  return ReadChar(m) / 4.0;
}

void WriteOffset(BB_t* m, float a)
{
  WriteChar(m, (char)(a * 4.0) );
}

float* ReadOffsetVec(BB_t* m, float* a)
{
  int i;
  
  for ( i=0 ; i<3 ; i++ ) a[i] = ReadOffset(m);
  return a;
}

void WriteOffsetVec(BB_t* m, float* a)
{
  int i;
  
  for ( i=0 ; i<3 ; i++ ) WriteOffset(m, a[i] );
}


void
do_simple_message_write_bin(node* n, BB_t* m)
{
  unicast_t u;
  
  u.use = UC_NOT;
  do_simple_message_write_bin_unicast(n,m,&u);
}


void
do_simple_message_write_bin_unicast(node* n, BB_t* m, unicast_t* u)
{
  node* c;

  /* at first: the message id */
  if (u->use == UC_THIS) {
    WriteByte(m,node_token_id(n->type) | MASK_UNICAST);
    WriteByte(m,u->client);
  }
  else
    WriteByte(m,node_token_id(n->type));

  if ((c = n->down) != NULL) {
    /* c is the first command or the first argument */
    /* are there commands ? */
    if (node_token_string(c->type)==NULL) {
      /* only arguments */
      do_simple_arguments_write_bin(c, m);
    }
    else {
      /* loop over all commands */
      for ( ; c!=NULL ; c=c->next) {
        /* do the single command */
        do_simple_arguments_write_bin(c->down, m);
      }
    }
  }
}


void do_unknown_message_write_bin(node* n, BB_t* m)
{
  node* c;

  /*
    I don't do any kind of check in here. If the internal structure is
    corrupt this gives a total crash.

    unknown, message id itself is an argument
  */

  /* message -> commands */
  c=n->down; /* id */
  do_simple_argument_write_bin(c->down,m);

  c=c->next; /* data */
  do_simple_arguments_write_bin(c->down,m); /* plural s is important */
}


void do_simple_arguments_write_bin(node* a, BB_t* m)
{
  for ( ; a!=NULL; a=a->next) {
    do_simple_argument_write_bin( a, m);
  }
}


void do_simple_argument_write_bin(node* a, BB_t* m)
{
  if (a==NULL) {
    syntaxerror(0,"NULL pointer");
  }
  switch (a->type) {
    case V_INT:
      switch(a->hint) {
        case H_BYTE:
          WriteByte(m,*(long*)a->down);
        break;
        case H_SHORT:
          WriteShort(m,*(long*)a->down);
        break;
        case H_LONG:
          WriteLong(m,*(long*)a->down);
        break; 
        case H_SB:
          if (*(long*)a->down < 256) {
            WriteByte(m,*(long*)a->down);
          }
          else {
            WriteShort(m,*(long*)a->down);
          }
        break;
        case H_LSB: {
          long value = *(long*)a->down;

          if (value < 256) {
            WriteByte(m,value);
          }
          else {
            if (value < 0x8000) {
              WriteShort(m,value);
            }
            else {
              WriteLong(m,value);
            }
          }
        }
        break;
        default: 
          syserror(DINTE, "wrong int hint type %d",a->hint);
      }
    break;
    case V_BYTEHEX:
      WriteByte(m,*(long*)a->down);
    break;    
    case V_FLOAT:
      switch(a->hint) {
        case H_NOTHING:
        break;
        case H_FLOAT:
          WriteFloat(m,*(float*)a->down);
        break;
        case H_COORD: 
          WriteCoord(m,*(float*)a->down);
        break;
        case H_ANGLE: 
          WriteAngle(m,*(float*)a->down);
        break;
        case H_ANGLE16: 
          WriteAngle16(m,*(float*)a->down);
        break;
        case H_VEL:
          WriteVel(m,*(float*)a->down);
        break;
        case H_VOL:
          WriteVol(m,*(float*)a->down);
        break;
        case H_ATTENUATION:
          WriteAttenuation(m,*(float*)a->down);
        break;
        case H_TIMEOFS:
          WriteTimeofs(m,*(float*)a->down);
        break;
        case H_BYTEFLOAT:
          WriteChar(m,(char)*(float*)a->down);
        break;
        case H_BLEND:
          WriteBlend(m,*(float*)a->down);
        break;
        case H_OFFSET:
          WriteOffset(m,*(float*)a->down);
        break;
        case H_DIR:
        {
          float aa[3];
          aa[0] = *(float*)a->down;
          aa[1] = *(float*)a->next->down;
          aa[2] = *(float*)a->next->next->down;
          WriteDir(m,aa);
          /* did it already */
          a->next->hint=H_NOTHING;
          a->next->next->hint=H_NOTHING;
        }
        break;
        default:
          syserror(DINTE, "wrong float hint type %d", a->hint);
      }
    break;
    case V_TIME:
      WriteFloat(m,*(float*)a->down);
    break;
    case V_STRING:
      switch(a->hint) {
        case H_STRING:
          WriteString(m,(char*)a->down);
        break;
        case H_STRINGLIST:
          WriteString(m,(char*)a->down);
          if (a->next==NULL) put_signed_char(m,'\0'); 
        break;
        default:
          syserror(DINTE, "wrong string hint type %d", a->hint);
      }
    break;
    default:
      syserror(DINTE, "wrong argument type at pos=%d, type=%d", a->pos, a->type);
  } /* switch a->type */
}


/*- file end binblock.c -----------------------------------------------------*/
