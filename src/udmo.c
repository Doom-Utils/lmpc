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
|  udmo.c - implementation, general DMO routines                             |
\****************************************************************************/


#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "tools.h"
#include "udmo.h"


char *DMOActionName[] = {
/* Jump */               "JU",
/* Crouch */             "CR",
/* Fire */               "FW",
/* Aim_Up */             "AU",
/* Aim_Down */           "AD",
/* Run */                "RU",
/* ?? */                 "AC6",
/* ?? */                 "AC7", 
/* Weapon */             "", "", "", "",
/* ?? */                 "AC12",
/* Look_Up */            "LU",
/* Look_Down */          "LD",
/* NightVision */        "VI",
/* MedKit */             "MK",
/* AutoRun */            "AR",
/* Center_View */        "LC",
/* Holster_Weapon */     "HW",
/* Inventory_Left */     "IL",
/* Pause */              "PA",
/* ?? */                 "AC22",
/* ?? */                 "AC23",
/* Holo_Duke */          "HD",
/* Jetpack */            "JP",
/* ?? */                 "AC26",
/* Inventory_Right */    "IR",
/* TurnAround */         "TA",
/* Open */               "UT",
/* Inventory */          "IN",
/* ?? */                 "AC31"
                       };

/******************************************************************************/
/** DMO ***********************************************************************/
/******************************************************************************/

#define DMO_BUFFER 65536L

void DMO_init(DMO_t *d, char *filename, char *mode)
{
  struct stat buf;

  if ((d->filename = strdup(filename))==NULL) syserror(errno,"strdup");
  if ((d->file=fopen(d->filename, mode))==NULL) syserror(errno,d->filename);
  if (SETVBUF(d->file,NULL,_IOFBF,BUFSIZ>DMO_BUFFER?BUFSIZ:DMO_BUFFER)!=0) syserror(errno,"setvbuf");
  if (stat(d->filename,&buf)==-1) syserror(errno,d->filename);
  d->filesize=buf.st_size;
}

void DMO_readheader(DMO_t *d)
{
  unsigned long newheadersize;

  rewind(d->file);
  d->headersize=HEADER_DUKE_old;
  if ((unsigned long)fread(d->header,1,d->headersize,d->file)!=d->headersize) 
    syserror(FIREAD,d->filename);
  d->tics= ((unsigned long)d->header[0]      ) |
           ((unsigned long)d->header[1] <<  8) |
           ((unsigned long)d->header[2] << 16) |
           ((unsigned long)d->header[3] << 24); 
  /* game must be set before calling */
  switch (d->game) {
    case DUKE_old:
      d->game=DUKE_old;
      d->gs="Duke Nukem 3D";
      d->vs="1.0 or 1.1";
      d->episode=d->header[4]+1;
      d->map=d->header[5]+1;
      d->skill=d->header[6];
      d->playernum=((unsigned short)d->header[7]     ) |
                   ((unsigned short)d->header[8] << 8);
    break;
    case DUKE_new:
      newheadersize = HEADER_DUKE_new;
      if (fread(d->header+d->headersize,1,newheadersize-d->headersize,d->file)!=newheadersize-d->headersize) syserror(FIREAD,d->filename);
      d->headersize = newheadersize;
      d->game=DUKE_new;
      d->gs="Duke Nukem 3D";
      d->vs="1.3D";
      d->episode=d->header[0x04] + 1;
      d->map=d->header[0x05] + 1;
      d->skill=d->header[0x06];
      d->multirule=d->header[0x07];
      d->playernum=((unsigned short)d->header[0x08]     ) |
                   ((unsigned short)d->header[0x09] << 8);
      d->m=((unsigned short)d->header[0x0A]     ) |
           ((unsigned short)d->header[0x0B] << 8);
      d->t1 = ((unsigned long)d->header[0x0C]      ) |
              ((unsigned long)d->header[0x0D] <<  8) |
              ((unsigned long)d->header[0x0E] << 16) |
              ((unsigned long)d->header[0x0F] << 24);
      d->t2 = ((unsigned long)d->header[0x10]      ) |
              ((unsigned long)d->header[0x11] <<  8) |
              ((unsigned long)d->header[0x12] << 16) |
              ((unsigned long)d->header[0x13] << 24);
      d->t3 = ((unsigned long)d->header[0x14]      ) |
              ((unsigned long)d->header[0x15] <<  8) |
              ((unsigned long)d->header[0x16] << 16) |
              ((unsigned long)d->header[0x17] << 24);
    break;
    case REDNECK:
      newheadersize = HEADER_REDNECK_BASE;
      if (fread(d->header+d->headersize,1,newheadersize-d->headersize,d->file)!=newheadersize-d->headersize) syserror(FIREAD,d->filename);
      d->headersize = newheadersize;
      d->game=REDNECK;
      d->gs="Redneck Rampage";
      d->vs="0.7 alpha";
      d->headersize=HEADER_REDNECK_BASE;
      d->episode=1; /* don't know any better yet */
      d->skill=d->header[0x07];
      d->multirule=d->header[0x08]+1;
      d->map=d->header[0x09];
      d->playernum=((unsigned short)d->header[0x0A]     ) |
                   ((unsigned short)d->header[0x0B] << 8);
      d->m=((unsigned short)d->header[0x0C]     ) |
           ((unsigned short)d->header[0x0D] << 8);
      d->t1 = ((unsigned long)d->header[0x0E]      ) |
              ((unsigned long)d->header[0x0F] <<  8) |
              ((unsigned long)d->header[0x10] << 16) |
              ((unsigned long)d->header[0x11] << 24);
      d->t2 = ((unsigned long)d->header[0x12]      ) |
              ((unsigned long)d->header[0x13] <<  8) |
              ((unsigned long)d->header[0x14] << 16) |
              ((unsigned long)d->header[0x15] << 24);
      d->t3 = ((unsigned long)d->header[0x16]      ) |
              ((unsigned long)d->header[0x17] <<  8) |
              ((unsigned long)d->header[0x18] << 16) |
              ((unsigned long)d->header[0x19] << 24);
      d->a  = ((unsigned long)d->header[0x1A]      ) |
              ((unsigned long)d->header[0x1B] <<  8) |
              ((unsigned long)d->header[0x1C] << 16) |
              ((unsigned long)d->header[0x1D] << 24);
      d->name=(char*)d->header+0x1E;
      newheadersize = d->headersize + d->playernum;
      if (fread(d->header+d->headersize,1,newheadersize-d->headersize,d->file)!=newheadersize-d->headersize) syserror(FIREAD,d->filename);
      d->headersize = newheadersize;
    break;
  }

  d->incomplete = d->tics % d->playernum;
  d->tics =       d->tics / d->playernum;
  d->time = d->tics*DMO_TICTIME;
  d->datasize=d->filesize-d->headersize;

}

void DMO_readblock(DMO_t *d, CHU_t *c)
{
  long count;

  if (fread(c->header,1,CHUNK_HEADER_SIZE,d->file)!=CHUNK_HEADER_SIZE) 
    syserror(FIREAD,d->filename);
  c->c_size = (((unsigned short) c->header[0]     ) | 
               ((unsigned short) c->header[1] << 8)  ) -4;
  c->u_size =  ((unsigned short) c->header[2]     ) | 
               ((unsigned short) c->header[3] << 8);
  c->tics = c->u_size/( DMO_TIC * d->playernum);
  c->time = c->tics * DMO_TICTIME;
  c->incomplete = c->u_size % (DMO_TIC * d->playernum);
  c->comp_number = ((unsigned short) c->header[4]     ) |
                   ((unsigned short) c->header[5] << 8);
  if (c->u_size > DMO_TIC*MAXTICS) syserror(WDMO,d->filename);

  /* 
  fprintf(stderr, "c=0x%x, u=0x%x, l=0x0%x\n", c->c_size, c->u_size, 
                                               c->comp_number);
  */
    
  if (fread(c->c_buffer,1,c->c_size,d->file)!=c->c_size) 
    syserror(FIREAD,d->filename);
  count = decompress(c->c_buffer,c->c_size,c->u_buffer,c->u_size,
                     c->comp_number,DMO_TIC*d->playernum);
  if (count!=c->u_size) {
    switch (count) {
      case -1: 
        fprintf(stderr,"malloc failed"); break;
      case -2:
        fprintf(stderr,"code size > 16 bits"); break;
      case -3:
        fprintf(stderr,"input buffer too small"); break;
      case -4:
        fprintf(stderr,"undefined code found"); break;
      case -5:
        fprintf(stderr,"output buffer too small"); break;
      case -6:
        fprintf(stderr,"uncompressed size is not a multiple of %i",DMO_TIC*d->playernum); break;
      default:
        fprintf(stderr,"decompression error code=%li",count); 
    }      
    fprintf(stderr,"\n");
    fflush(stderr);
    syserror(DECOMP,d->filename); 
  }
}

void DMO_readmacroblock(DMO_t *d, CHU_t *m)
{
  CHU_t c;
  unsigned long i, j, tic_size;

  m->tics = 0;
  do {
    DMO_readblock(d, &c);
    memmove(&(m->c_buffer[m->tics*d->playernum*DMO_TIC]), c.u_buffer, 
            c.tics*d->playernum*DMO_TIC);
    m->tics+=c.tics;
    if (m->tics*d->playernum>MAXTICS) syserror(WDMO,d->filename);
  } while (((unsigned long)ftell(d->file)<d->filesize) && m->tics*d->playernum<MAXTICS);
  tic_size=d->playernum*DMO_TIC;
  for (j=0;j<tic_size;j++) {
    m->u_buffer[j]=m->c_buffer[j]; 
  }
  for (i=1;i<m->tics;i++) {
    for (j=0;j<tic_size;j++) {
      m->u_buffer[i*tic_size+j] = 
        m->u_buffer[(i-1)*tic_size+j] + m->c_buffer[i*tic_size+j]; 
    }
  } 
  m->time = m->tics * DMO_TICTIME;
}

void DMO_done(DMO_t *d)
{
  if (fclose(d->file)!=0) syserror(errno,d->filename);
  free(d->filename);
}


#define creturn(x) { fclose(file); return x; }

/* outside */
/* Checks, if a file exists and is a proper DMO file, return the game code */
int isDMO(char *filename)
{
  struct stat buf;
  FILE *file;
  off_t filesize;
  unsigned long tics;
  unsigned char buffer[HEADER_REDNECK_BASE+8];
  unsigned long headersize;
  unsigned short playernum;

  /* fprintf(stderr,"checking %s\n",filename); */
  if ((file=fopen(filename, "rb"))==NULL) { return 0; }
  /* fprintf(stderr,"file open ok\n"); */
  if (stat(filename,&buf)==-1) creturn(0); 
  filesize=buf.st_size;
  /* fprintf(stderr,"filesize=%li\n", filesize); */

  headersize=HEADER_DUKE_old;
  if (fread(buffer,1,headersize,file)!=headersize) creturn(0);
  tics=((unsigned long)buffer[0]      ) |
       ((unsigned long)buffer[1] <<  8) |
       ((unsigned long)buffer[2] << 16) |
       ((unsigned long)buffer[3] << 24); 
  /* fprintf(stderr,"tics=%li\n",tics); */

  /* try DUKE_old */
  /* fprintf(stderr,"try Duke 3D old\n"); */
  if (checkDMOtail(file, filesize, tics)) creturn(DUKE_old);

  /* try DUKE_new */  
  /* fprintf(stderr,"try Duke 3D new\n"); */
  headersize = HEADER_DUKE_new;
  if (fseek(file, headersize, SEEK_SET)!=0) creturn(0);
  if (checkDMOtail(file, filesize, tics)) creturn(DUKE_new);

  /* try Redneck Rampage */
  /* fprintf(stderr,"try Redneck Rampage\n"); */
  rewind(file);
  headersize = HEADER_REDNECK_BASE;
  if (fread(buffer,1,headersize,file) != headersize) creturn(0);
  playernum=((unsigned short)buffer[0x0A]     ) |
            ((unsigned short)buffer[0x0B] << 8);
  if (playernum==0 || playernum>8) creturn(0);
  headersize += playernum;
  if (fseek(file, headersize, SEEK_SET)!=0) return 0;
  if (checkDMOtail(file, filesize, tics)) creturn(REDNECK);

  /* nothing at all */
  creturn(0);
}

int checkDMOtail(FILE* f, off_t filesize, unsigned long tics)
{
  char unsigned buffer[4];
  unsigned short chunk_size_compressed, chunk_size_uncompressed;
  
  while (tics>0) {
    /* fprintf(stderr,"pos is %li\n", ftell(f)); */
    if (fread(buffer,1,4,f)!=4) return 0;
    chunk_size_compressed=((unsigned short)buffer[0]     ) |
                          ((unsigned short)buffer[1] << 8);
    chunk_size_uncompressed=((unsigned short)buffer[2]     ) |
                            ((unsigned short)buffer[3] << 8);
    /*
    fprintf(stderr,"c=%i, u=%i\n",chunk_size_compressed, chunk_size_uncompressed);
    */
    if (chunk_size_compressed<CHUNK_HEADER_SIZE) return 0;
    if (fseek(f, chunk_size_compressed-2, SEEK_CUR)!=0) return 0;
    if (chunk_size_uncompressed%DMO_TIC) return 0;
    tics-=chunk_size_uncompressed/DMO_TIC;
  }
  return (ftell(f)==filesize);
}

/*- file end udmo.c ----------------------------------------------------------*/

