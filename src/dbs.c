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
    E-mail: Uwe.Girlich@itp.uni-leipzig */
                                                                    
/****************************************************************************\
|  dbs.c  -  main source code                                                |
\****************************************************************************/
                                                                    

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_SYS_IPC_H
  #include <sys/ipc.h>
#endif
#ifdef HAVE_SYS_SHM_H
  #include <sys/shm.h>
#endif
#include <sys/types.h>
#ifdef HAVE_SYS_TIME_H
  #include <sys/time.h>
#endif
#include <sys/socket.h>
#include <pwd.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#ifdef HAVE_MEMORY_H
  #include <memory.h>
#endif
#include <signal.h>
#include <math.h>
#include <getopt.h>

#include "tools.h"
#include "record.h"
#include "binblock.h"
#include "udem.h"

#define NET_PROTOCOL_VERSION 0x03

#define CCREQ_CONNECT     0x01
#define CCREQ_SERVER_INFO 0x02    
#define CCREQ_PLAYER_INFO 0x03
#define CCREQ_RULE_INFO   0x04

#define CCREP_ACCEPT      0x81
#define CCREP_REJECT      0x82
#define CCREP_SERVER_INFO 0x83
#define CCREP_PLAYER_INFO 0x84
#define CCREP_RULE_INFO   0x85
           
#define KEEP_ALIVE        0x01
#define CONSOLE_ORDER     0x04

/* messsage types */
#define CONTROL           0x8000
#define PART_REL          0x0001
#define ACK_REL           0x0002
#define LAST_REL          0x0009
#define NON_REL           0x0010

#define MAX_BLOCKSIZE     0x0400

typedef struct {
  char* rule;
  char* value;
} rule_t;

typedef struct {
  pid_t pid;
  int used;
  unsigned long int sequence;
  unsigned long int cseq;
  int cseqused;
  struct sockaddr_in caddr;
  int clen;
  unsigned short int port;
  char name[100];
  long colors;
  long frags;
  double logintime;
  char address[100];
} shm_t;

typedef struct {
  char *name;
  char *address;
  int max_players;
  char *demfilename;
  unsigned short int port;
  int sd;
  int shmid;
  shm_t *shm;
} qsdb_t;

rule_t rules[] = {
  { "sv_maxspeed", "320" },
  { "sv_friction", "4"   },
  { "sv_gravity",  "800" },
  { "noexit",      "0"   },
  { "teamplay",    "0"   },
  { "timelimit",   "0"   },
  { "fraglimit",   "0"   },
  { "cmdline",     ""    }
};

qsdb_t qsdb;
  
char hostname[100];  
char hostip[100];
int maxretry;
int timeout;
                          
static struct sockaddr_in INET_ZERO = { AF_INET };

int SendUDPPacket(BB_t* m, unsigned short type, 
                  int fromsd, struct sockaddr_in *toaddr, int tolen);
int SendAckPacket(unsigned long sequence, int sd, struct sockaddr_in *toaddr, int tolen);
int ReceiveUDPPacket(BB_t* m, unsigned short *type,
                     int sd, struct sockaddr_in *fromaddr, socklen_t *fromlen);
int EqualAddrIn(struct sockaddr_in* a, struct sockaddr_in* b);
void ControlServer(void);
pid_t SpawnServer(char* demfilename, unsigned short int port);
pid_t PlayServer(int player);
int SendInitDEMPacket(BB_t *pd, unsigned long *sequence, 
                  int sd, struct sockaddr_in *toaddr, int tolen);
int SendRuntimeDEMPacket(BB_t *pd, unsigned long *sequence, 
                  int sd, struct sockaddr_in *toaddr, int tolen);
int count_players(void);
int find_free(void);
void syntax(void);
void version(void);

int CreateUDPSocket(unsigned short int  * port)
{
  struct sockaddr_in me ;
  struct sockaddr_in sin;
  int sd, zz;
  socklen_t len;
  
  me = sin = INET_ZERO;
  me.sin_port = htons((unsigned short) *port);
  me.sin_family = AF_INET;

  if ((sd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) syserror(NETGEN, "socket");
                
  if (setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(char *)&zz,sizeof(zz)) < 0)
    syserror(NETGEN, "setsockopt");
  if (bind(sd,(struct sockaddr *) &me,(len = sizeof(me))) < 0)
    syserror(NETGEN, "bind");
  if (getsockname(sd,(struct sockaddr *)&sin,&len) < 0)
    syserror(NETGEN, "getsockname");
  
  if(!*port) *port = ntohs((unsigned short) sin.sin_port);

  return sd;
}

int SendUDPPacket(BB_t* m, unsigned short type, 
                  int sd, struct sockaddr_in *toaddr, int tolen)
{
  int sendbytes;
  
  m->p = m->buffer;
  WriteByte(m, (type >> 8) & 0xFF);
  WriteByte(m, (type) & 0xFF);
  WriteByte(m, (m->size>>8) & 0xFF);
  WriteByte(m, (m->size) & 0xFF);
  /* fprintf(stderr, "sending: type=0x%04x, size=%li\n", type, m->size); */
            
  sendbytes = sendto(sd, m->buffer, m->size, 0, 
                     (struct sockaddr*) toaddr, tolen);
  if (sendbytes != m->size) {
    syswarning(NETGEN,"sendto");
    if (sendbytes==-1) {
      syswarning(errno,"sendto");
    }
    return -1;
  }
  return sendbytes;
}

int SendAckPacket(unsigned long sequence, int sd, struct sockaddr_in *toaddr, 
                  int tolen)
{      
  BB_t m;
  
  m.p = m.buffer + 4;
  WriteLongBE(&m, sequence);
  m.size = m.p - m.buffer;
  return SendUDPPacket(&m, ACK_REL, sd, toaddr, tolen);
}

int ReceiveUDPPacket(BB_t* m, unsigned short *type,
                     int sd, struct sockaddr_in *fromaddr, socklen_t *fromlen)
{        
  int receivebytes;
               
  receivebytes = recvfrom(sd ,m->buffer, DEM_MAX_MESSAGE_SIZE, 0,
                          (struct sockaddr*)fromaddr, fromlen);
  if (receivebytes == -1) {
    syswarning(NETGEN, "recvfrom");
    return -1;
  }        
  if (fromaddr->sin_family != AF_INET) {
    syswarning(NETGEN, "wrong network family\n");
    return -1;
  }
  m->p = m->buffer;
  *type = (ReadByte(m) << 8) + ReadByte(m);
  m->size = (ReadByte(m) << 8) + ReadByte(m);
  /* fprintf(stderr,"receiving: type=0x%04x, size=%li\n", *type, m->size); */
  if (m->size != receivebytes) {
    fprintf(stderr, "Problem: UDP packet len=%i, id packet len=%li\n",
                                             receivebytes,     m->size);
  }
  m->end = m->buffer + m->size;
  return receivebytes;
}

int EqualAddrIn(struct sockaddr_in* a, struct sockaddr_in* b)
{
  if (memcmp(&a->sin_addr, &b->sin_addr, sizeof(struct in_addr)) == 0
      &&
      a->sin_port == b->sin_port)
    return 1;
  else
    return 0;                                                                            
}

void ControlServer(void)
{
  fd_set fds;
  BB_t m, r;
  unsigned short packet_type;
  long opcode;
  struct sockaddr_in from;
  socklen_t fromlen;
  
  qsdb.sd = CreateUDPSocket(&(qsdb.port));
  
  while (1) {
    FD_ZERO(&fds);
    FD_SET(qsdb.sd, &fds); 
    if (select(qsdb.sd+1, &fds, NULL, NULL, NULL) < 0) syserror(errno,"select");

    if (!(FD_ISSET(qsdb.sd, &fds))) syserror(NETGEN, "select sd");

    fromlen = sizeof(from);
    ReceiveUDPPacket(&m, &packet_type, qsdb.sd, &from, &fromlen);

    switch (packet_type) {
      case CONTROL:
      {
        opcode = ReadByte(&m);
        fprintf(stderr, "receive control packet opcode=0x%02lx\n", opcode);
        switch (opcode) {
          case CCREQ_CONNECT: 
          {
            char game_name[DEM_MAX_MESSAGE_SIZE];
            long net_protocol_version;
            int ignore, pi;
            
            ReadString (&m, game_name);
            net_protocol_version = ReadByte(&m);
            
            if (strcmp(game_name,"QUAKE")) {
              fprintf(stderr, "unknown game type `%s'\n", game_name);
              break;
            }
            if (net_protocol_version != NET_PROTOCOL_VERSION) {
              fprintf(stderr, "unknown protocol version: 0x%02lx\n", net_protocol_version);
              break;
            }
            ignore = 0;
            
            /* look for an already connected player */
            for (pi = 0; pi<qsdb.max_players; pi++) {
              if (qsdb.shm[pi].used) {
                if (EqualAddrIn(&from, &qsdb.shm[pi].caddr)) ignore=1;
              }
            }
            
            if (!ignore) {
              int new_player;
              
              if ((new_player=find_free()) == -1) {
                r.p = r.buffer + 4;
                WriteByte(&r, CCREP_REJECT);
                WriteString(&r, "too many watchers out there");
                r.size = r.p - r.buffer;
                SendUDPPacket(&r, 0x8000, qsdb.sd, &from, fromlen);
              }
              else {
                shm_t *pls;
              
                pls = &(qsdb.shm[new_player]);
                memcpy(&(pls->caddr), &from, fromlen);
                pls->clen = fromlen;
                pls->logintime = realtime();
                pls->port = ntohs(pls->caddr.sin_port); 
                sprintf(pls->address, "%s:%i", 
                                       inet_ntoa(pls->caddr.sin_addr),
                                          pls->port);  
  
                switch (pls->pid=fork()) {
                  case -1:
                  { 
                    syswarning(errno, "control: couldn't fork child");
                    pls->used = 0;
                  }
                  break;
                  case 0: 
                  {
                    pls->pid = getpid();
                    pls->used = 1;
                    PlayServer(new_player);
                    pls->used = 0;
                    exit(0);
                  }
                  default:
                  {
                    fprintf(stderr,"control: starting child pid=%i\n", pls->pid);
                  }
                  break;
                } /* end switch fork-pid */  
              } /* end do connect */
            } /* not ignore */
          } 
          break; /* end case opcode == CCREQ_CONNECT */
          /****************************************************/
          case CCREQ_SERVER_INFO: 
          {
            char game_name[DEM_MAX_MESSAGE_SIZE];
            long net_protocol_version;
            
            ReadString (&m, game_name);
            net_protocol_version = ReadByte(&m);
                                                            
            if (strcmp(game_name,"QUAKE")) {
              fprintf(stderr, "unknown game type %s'\n", game_name);
              break;
            }
            if (net_protocol_version != NET_PROTOCOL_VERSION) {
              fprintf(stderr, "unknown protocol version: 0x%02lx\n", net_protocol_version);
            }
            
            r.p = r.buffer + 4;
            WriteByte(&r, CCREP_SERVER_INFO);
            WriteString(&r, qsdb.address);
            WriteString(&r, qsdb.name);
            WriteString(&r, qsdb.demfilename);
            WriteByte(&r,count_players());
            WriteByte(&r, qsdb.max_players);
            WriteByte(&r, NET_PROTOCOL_VERSION);
            r.size = r.p - r.buffer;
            
            SendUDPPacket(&r, 0x8000, qsdb.sd, &from, fromlen); 

          }  
          break; /* end case opcode == CCREQ_SERVER_INFO */
          /***********************************************/
          case CCREQ_PLAYER_INFO: 
          {
            long player_number;
            
            player_number = ReadByte(&m);
            
            if (player_number<0 || 
                player_number>qsdb.max_players ||
                !qsdb.shm[player_number].used) {
              fprintf(stderr, "unknown player number: %li\n", player_number);
              break;
            }
            
            /* response */
            r.p = r.buffer + 4;
            WriteByte(&r, CCREP_PLAYER_INFO);
            WriteByte(&r, player_number);
            WriteString(&r, qsdb.shm[player_number].name);
            WriteLong(&r, qsdb.shm[player_number].colors); 
            WriteLong(&r, qsdb.shm[player_number].frags); 
            WriteLong(&r, (long)(realtime() - qsdb.shm[player_number].logintime));
            WriteString(&r, qsdb.shm[player_number].address); 
            r.size = r.p - r.buffer;

            SendUDPPacket(&r, 0x8000, qsdb.sd, &from, fromlen); 
          
          } 
          break; /* end of case CCREQ_PLAYER_INFO */
          /********************************************************/
          case CCREQ_RULE_INFO: 
          {
            char rule[DEM_MAX_MESSAGE_SIZE];
            int rn;
            int i;
              
            ReadString(&m, rule);

            /* response */
            
            rn = sizeof(rules) / sizeof(rules[0]);
            
            r.p = r.buffer + 4;
            WriteByte(&r, CCREP_RULE_INFO);
            /* first rule */
            if (!*rule) {
              WriteString(&r, rules[0].rule);
              WriteString(&r, rules[0].value);
            } /* end first rule */ 
            else {
              /* normal new rule */
              for (i=0;i<rn-1;i++) {
                if (!strcmp(rule,rules[i].rule)) {
                  WriteString(&r, rules[i+1].rule);
                  WriteString(&r, rules[i+1].value);
                }
              }
              /* wrong rule string: nothing */
              if (i==rn) break;
            } 
            r.size = r.p - r.buffer;
            SendUDPPacket(&r, 0x8000, qsdb.sd, &from, fromlen); 

          } 
          break; /* end of case opcode == CCREQ_RULE_INFO */
          /*************************************************/
          default:
          {
            fprintf(stderr, "unknown control packet opcode: 0x%02lx\n", opcode);
          } 
          break;
          /*************************************************/
        } /* end switch opcode */
      }
      break; /* end case packet_type == CONTROL */
      default: 
      {
        fprintf (stderr, "unknown packet type: 0x%04x\n", packet_type);
      }
      break;
    } /* switch packet_type */         
  } /* endless while */
} /* end ControlServer */

void exitchild(int player)
{
  fprintf(stderr, "[%i] stopping server for player %i\n", 
    qsdb.shm[player].pid, player);
}

                
pid_t PlayServer(int player)
{
  int sd;
  unsigned short int port;
  BB_t pd, pn;
  DEM_t d;

  BB_t am;
  unsigned short atype;
  struct sockaddr_in aaddr;
  socklen_t alen;
  unsigned long aseq;
  int runtime;
  shm_t *pls;
  int gotit, retry;
  
  #define CA (pls->caddr)
  #define CL (pls->clen)
  #define EXITCHILD exitchild(player) ; goto PLAYEND

  pls = &(qsdb.shm[player]);

  /* we need a new port */
  port = 0;
  sd = CreateUDPSocket(&port);

  /* tell the client: accept */
  pn.p = pn.buffer + 4;
  WriteByte(&pn, CCREP_ACCEPT);
  WriteLong(&pn, port);
  pn.size = pn.p - pn.buffer;
  SendUDPPacket(&pn, 0x8000, qsdb.sd, &CA, CL);

  /* prepare DEM file */
  pls->sequence = 0; 
  pls->cseq = 0;
  pls->cseqused = 0;
  DEM_init(&d, qsdb.demfilename, "rb");
  /* cd track is not interesting but it fills some variables */
  node_delete(do_track_read_bin(&d));
      
  /* send precache */
  DEM_block_read_bin(&d, &pd);
  SendInitDEMPacket(&pd, &(pls->sequence), sd, &CA, CL);

  /* now wait for the client and hope the best */
      
  runtime = 0;
  while(!runtime) {
    for (gotit=0, retry=0; !gotit && retry<maxretry; retry++) {
      fd_set fds;
      struct timeval tv;

          FD_ZERO(&fds);
          FD_SET(sd, &fds); 
          tv.tv_sec = timeout; 
          tv.tv_usec = 0;
          fprintf(stderr, "wait for something happening\n");
          if (select(sd+1, &fds, NULL, NULL, &tv) < 0) syserror(errno,"select");
          if (FD_ISSET(sd, &fds)) {
            int data; 
           
            fprintf(stderr, "got a packet\n");
            alen = sizeof(aaddr);
            ReceiveUDPPacket(&am, &atype, sd, &aaddr, &alen);
            if (EqualAddrIn(&aaddr,&CA)) {
              if (atype == LAST_REL) {
                aseq = ReadLongBE(&am);
                /* send an ack just now */
                SendAckPacket(aseq, sd, &aaddr, alen);
                if (!pls->cseqused || 
                     (
                       (aseq>pls->cseq) && 
                       pls->cseqused
                     )
                   ) { 
                  pls->cseqused = 1;
                  pls->cseq = aseq;
                  while (am.p < am.end) {
                    data = ReadByte(&am);
                    switch (data) {
                      case KEEP_ALIVE:
                        /* ok, do nothing */
                        gotit = 1;
                      break;
                      case CONSOLE_ORDER: {
                        char ts[0x800];
                      
                        gotit = 1;
                        
                        /* read the console string */
                        ReadString(&am, ts);
                        
                        if (strncmp(ts, "name", 4) == 0) {
                          sscanf(ts, "name %s\n", pls->name);
                        } 
                        if (strncmp(ts, "color", 5) == 0) {
                          int col_s, col_p;
                          
                          sscanf(ts, "color %i %i\n", &col_s, &col_p);
                          pls->colors = col_p + (col_s << 4);
                        }
     
                        if (strcmp(ts, "prespawn") == 0) {
                          /* send entity init */
                          fprintf(stderr, "entity init\n");
                          DEM_block_read_bin(&d, &pd);
                          if (SendInitDEMPacket(&pd,&(pls->sequence),sd,&CA, CL)==-1) {
                            syswarning(NETGEN, "SendInitDEMPacket");
                            EXITCHILD;
                          }
                        }
                        if (strcmp(ts, "spawn ") == 0) {
                          /* send lighting */
                          fprintf(stderr, "lighting\n");
                          DEM_block_read_bin(&d, &pd);
                          if (SendInitDEMPacket(&pd,&(pls->sequence),sd,
                              &CA, CL)==-1) {
                            syswarning(NETGEN, "SendInitDEMPacket");
                            EXITCHILD;
                          }
                        }
                        if (strcmp(ts, "begin") == 0) {
                          /* stop init phase, start runtime phase */
                          runtime = 1;
                        }
                        fprintf(stderr, "console `%s'\n", ts);
                      }
                      break;
                      default: 
                        fprintf(stderr, "problem: client parse\n");
                      break;  
                    }
                  }
                } /* end: new packet needs ack */
              } /* end: packet needs ack */
            } /* end: right address */         
          } /* end: select found a packet */
        } /* end: for retry */
        if (retry == maxretry) {
          syswarning(NETGEN, "client does not respond");
          EXITCHILD;
        }  
      } /* while(!runtime) */
      
  fprintf(stderr, "starting runtime mode\n");

  {
    double ofs_time;
    double cur_dem_time;

    ofs_time = 0.0; /* we don't know the offset yet */
    cur_dem_time = 0.0; /* the DEM time starts at 0 */
    while (ftell(d.file) < (long)d.filesize) {
      double wait_time; 
      fd_set fds;
      struct timeval tv;
      node *no, *b;

      /* do not wait */
      wait_time = 0.0;
      /* read the block */
      DEM_block_read_bin(&d, &pd);
      /* fprintf(stderr, "@"); */
      /* parse block set the wait time */ 
      no=DEM_bin_to_node(&pd, 0);
      /* down is camera, down->next is the next message */
      if (no->type==TOKEN_BLOCK) {
        for (b=no->down; b!=NULL; b=b->next) {
          /* find the time message */
          if (b->type==TOKEN_TIME) {
            cur_dem_time = *(float*)b->down->down;
            if (ofs_time==0.0) {
              /* fprintf(stderr, "base dem time is %f\n", cur_dem_time); */
              ofs_time = realtime() - cur_dem_time; 
            }
            break; /* break the parse loop after TOKEN_TIME */
          } /* TOKEN_TIME message */
        } /* end for all messages */
      } /* end if good block */
      node_delete(no); /* parsed block is of no use any more */

                  /*   amount   of   time   to   wait   */
            /* moment of the block */
                                /* real time in DEM format */
      while ((wait_time = cur_dem_time - ( realtime() - ofs_time) ) >= 0) {
        /* fprintf(stderr, "wait_time=%f\n", wait_time); */
        if (wait_time>0.0) {
          FD_ZERO(&fds);
          FD_SET(sd, &fds); 
          tv.tv_sec = (int) floor(wait_time);
          tv.tv_usec = (int) (1e6 * (wait_time-tv.tv_sec));
          if (select(sd+1, &fds, NULL, NULL, &tv) < 0) syserror(errno,"select");
          if (FD_ISSET(sd, &fds)) {
            ReceiveUDPPacket(&am, &atype, sd, &aaddr, &alen);
            if (EqualAddrIn(&aaddr,&CA)) {
              /* do something with the client packet */
              /* fprintf(stderr, "got a client packet type=%i\n", atype); */
            }
          }
        }
      }

      if (SendRuntimeDEMPacket(&pd,&(pls->sequence),sd,&CA,CL)==-1) {
        syswarning(NETGEN, "SendRuntimeDEMPacket");
        EXITCHILD;
      }
    }
  
  }
  DEM_done(&d);
  fprintf(stderr, "end of DEM file\n");
  EXITCHILD;

  PLAYEND:    
  return 0;
}

int SendInitDEMPacket(BB_t *pd, unsigned long *sequence, 
                  int sd, struct sockaddr_in *toaddr, int tolen)
{
  BB_t pn;
  int bytesleft;
  int blocksize;
  unsigned short int code;
  int gotit, retry;

  pd->p = pd->buffer + 16;   /* without the first 16 bytes */
  bytesleft = pd->size - 16; /* size was total */

  fprintf(stderr, "sending DEM packet\n");
  while (bytesleft>0) {
    fprintf(stderr,"sending reliable packet\n");
    if (bytesleft>MAX_BLOCKSIZE) {
      blocksize = MAX_BLOCKSIZE;
      code = PART_REL;
    }
    else {
      blocksize = bytesleft;
      code = LAST_REL;
    }

    pn.p = pn.buffer + 4;
    WriteLongBE(&pn, *sequence);
    memcpy(pn.p, pd->p, blocksize);
    bytesleft -= blocksize;
    pd->p += blocksize;
    pn.p += blocksize;
    pn.size = blocksize + 8;
      
    gotit = 0;
    for (gotit=0, retry=0; !gotit && retry<maxretry; retry++) {
      fd_set fds;
      struct timeval tv;
        
      fprintf(stderr, "try %i\n", retry);
      if (SendUDPPacket(&pn, code, sd, toaddr, tolen) == -1) return -1;
      fprintf(stderr,"wait for ack\n");
      FD_ZERO(&fds);
      FD_SET(sd, &fds); 
      tv.tv_sec = timeout; 
      tv.tv_usec = 0;
      if (select(sd+1, &fds, NULL, NULL, &tv) < 0) {
        syswarning(errno,"select");
        return -1;
      }
      if (FD_ISSET(sd, &fds)) {
        BB_t am;
        unsigned short atype;
        struct sockaddr_in aaddr;
        socklen_t alen;
        unsigned long aseq;
        int data;
 
        alen = sizeof(aaddr);
        ReceiveUDPPacket(&am, &atype, sd, &aaddr, &alen);
        if (EqualAddrIn(&aaddr,toaddr)) {
          aseq = ReadLongBE(&am);
          switch (atype) {
            case LAST_REL:
              /* something important */
              data=ReadByte(&am);
              switch(data) {
                case KEEP_ALIVE:
                  fprintf(stderr, "sending an ack\n");
                  SendAckPacket(aseq, sd, &aaddr, alen);
                  retry=0;
                break;
                default:
                  fprintf(stderr, "problem; received type 0x%02x packet, code=0x%02x\n",
                                                            atype,              data);
                break;
              } /* end switch(data) */
            break;
            case ACK_REL:
              fprintf(stderr, "got a ack\n");
              if (aseq == *sequence) {
                gotit = 1;
                fprintf(stderr, "got the right sequence\n");
              }
            break;
            default:
              fprintf(stderr, "problem: received type 0x%02x packet\n", atype);
            break;
          } /* end switch(atype) */
        } /* the right address */
      } /* end select finds a packet */
      else {
        /* else timeout */ 
        fprintf(stderr, "problem: timeout while waiting for response\n");
      }
    } /* for retry */
    if (retry==maxretry) {
      fprintf(stderr,"severe problem, client timeout\n");
      syswarning(NETGEN, "client does not respond");
      return -1;
    } /* end retry out */
    
    (*sequence)++;
  } /* end while bytesleft */
  fprintf(stderr, "DEM packet sended\n");
  return 1; 
}

int SendRuntimeDEMPacket(BB_t *pd, unsigned long *sequence, 
                  int sd, struct sockaddr_in *toaddr, int tolen)
{
  BB_t pn;
  int bytesleft;
  float a1, a2, a3;
 
  /* fprintf(stderr,"sending runtime packet\n"); */

  pn.p = pn.buffer + 4;
  WriteLongBE(&pn, *sequence);

  /* if there are some good camera values, place an setangle message */
  pd->p = pd->buffer + 4;
  a1=ReadFloat(pd);
  a2=ReadFloat(pd);
  a3=ReadFloat(pd);
  if (a1!=0.0 || a2!=0.0 ||a3!=0.0) {
    WriteByte(&pn, c_setangle);
    WriteAngle(&pn,a1);
    WriteAngle(&pn,a2);
    WriteAngle(&pn,a3);
  }

  bytesleft=pd->size - 16; /* skip 16 bytes from the DEM format */
  memcpy(pn.p, pd->buffer + 16, bytesleft);
  pn.p += bytesleft;
  pn.size = pn.p - pn.buffer;
  if (SendUDPPacket(&pn, NON_REL, sd, toaddr, tolen) == -1) return -1;
  (*sequence)++;
  
  return 1;
}

int count_players(void) 
{
  int pi, p;
  
  for (p=0, pi=0; pi< qsdb.max_players; pi++) 
    if (qsdb.shm[pi].used) p++;
  return p;
}

int find_free(void) 
{
  int pi;
  
  for (pi=0 ; pi< qsdb.max_players ; pi++) {
    /* fprintf(stderr, "player %i = %i\n", pi, qsdb.shm[pi].used); */
    if (!qsdb.shm[pi].used) return pi;
  }
  return -1;
}

void syntax(void)
{
  fprintf(stderr,"Demo Broadcasting Server\n");
  fprintf(stderr,"DBS (c) U. Girlich, 1997-99, Release %s %s (%s)\n", VERSION, DATE, COMMENT);
  fprintf(stderr,"dbs [option ...] demfile\n");
  fprintf(stderr,"-h, --help                   display this help and exit.\n");
  fprintf(stderr,"-v, --version                output version information and exit.\n");
  fprintf(stderr,"-p, --port                   defines an UDP port for the server.\n");
  fprintf(stderr,"-m, --max-player             defines the max. number of players.\n");
  exit(1);
}

void version(void)
{
  fprintf(stderr,"dbs %s, %s (%s)\n", VERSION, DATE, COMMENT);
  exit(0);
}

int main(int argc, char ** argv)
{
  struct hostent *he;
  int pi;
  char ts[100];
  int c;
  char c_str[2]=" "; 
  
  #define DEFAULT_PORT 26000
  #define DEFAULT_MAX_PLAYERS 16

  static struct option long_options[] = {
    {"help",           0, 0, 'h'},
    {"version",        0, 0, 'v'},
    {"port",           1, 0, 'p'},
    {"max-players",    1, 0, 'm'},
    {0,                0, 0,   0}
  };
  int option_index = 0;
    
  maxretry = 20; /* max. number of retries */
  timeout = 2;   /* seconds before next retry */
  
  progname=strdup(argv[0]);

  /* define default values */
  qsdb.port = DEFAULT_PORT;
  qsdb.max_players = DEFAULT_MAX_PLAYERS;

  /* scan the command line */
  while (1) {
    c = getopt_long(argc, argv, "hvp:m:", long_options, &option_index);

    if (c==-1) break;
    *c_str = (char) c; 
    switch (c) {
      case ':': 
        syserror(ARGMIS,c_str); 
      break;
      case 'h':
        syntax();
      break;
      case 'v':
        version(); 
      break;
      case 'p': {
        int a, i;
        a = sscanf(optarg,"%i",&i);
        if (a!=1 || i<0 || i>65535) syserror(INVOPT, optarg);
        qsdb.port = i;
      }
      break;
      case 'm': {
        int a, i;
        a = sscanf(optarg,"%i",&i);
        if (a!=1 || i<1 || i>255) syserror(INVOPT, optarg);
        qsdb.max_players = i;
      }
      break;
      case '?':
        exit(ILLOPT);
      break;
      default:
        syserror(EINVAL, c_str);
      break; 
    } /* end switch */
  } /* end while */

  /* how many files left ? */
  if (argc-optind!=1) syntax();

  /* set the DEM file */
  qsdb.demfilename = strdup(argv[optind]);
  if (!isDEMbin(qsdb.demfilename)) syserror(WDEM,qsdb.demfilename);
  
  fprintf(stderr, "port=%i, DEM=\"%s\", max_players=%i\n", 
                  qsdb.port, qsdb.demfilename, qsdb.max_players);

  if (gethostname(hostname, sizeof(hostname)-1)==-1) 
    syserror(errno, "gethostname");
  if ((he = gethostbyname(hostname)) == NULL) 
    syserror(NETGEN, "gethostbyname");
  if (he->h_addrtype != AF_INET || he->h_length != 4)
    syserror(NETGEN, "unsupported gethostbyname"); 
  strcpy(hostip, inet_ntoa(*(struct in_addr*)(he->h_addr_list[0])));  

  qsdb.name = strdup(hostname);
  sprintf(ts, "%s:%i", hostip, qsdb.port);
  qsdb.address = strdup(ts);
    
  if ((qsdb.shmid=shmget(IPC_PRIVATE, qsdb.max_players * sizeof(shm_t),
                         IPC_CREAT | S_IRUSR | S_IWUSR))==-1) 
    syserror(SHMGEN, "shmget");
  qsdb.shm=(shm_t*)shmat(qsdb.shmid,0,0);
  if ((long int) qsdb.shm == -1) syserror(SHMGEN, "shmat"); 
  /* 
     Special trick: remove the SHM here, so the program can be killed
     with any signal and the SHM will be removed.
  */
  if (shmctl(qsdb.shmid,IPC_RMID,(struct shmid_ds*) 0) == -1) 
    syserror(SHMGEN, "shmctl (remove shm)");

  for (pi=0;pi<qsdb.max_players;pi++) {
    qsdb.shm[pi].used = 0;
    qsdb.shm[pi].port = 0;
    qsdb.shm[pi].logintime = 0.0;
    qsdb.shm[pi].frags = 0;
    qsdb.shm[pi].colors = 0;
    strcpy(qsdb.shm[pi].address, "(none)");
    strcpy(qsdb.shm[pi].name, "(none)");
  }

  ControlServer();

  return 0;
}
