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
|  demcut.c  -  main source code                                             |
\****************************************************************************/

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#ifdef HAVE_MKFIFO
  #define WITH_INTERACTIVE 1
#else
  #undef WITH_INTERACTIVE
#endif

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "tools.h"
#include "record.h"
#include "udem.h"

typedef struct {
  float timestart;
  float timelength;
  float timeend;
  size_t seekpos; 
  int multientry;
} FRAME_t;

typedef struct {
  DEM_t dem;
  long framei;
  FRAME_t *frames;
} DEMFILE_t;

#define MAX_LINE 100
typedef struct {
  char code;
  int argi1;
  int argi2;
  char args[MAX_LINE];
  float argf;
  int done;
} COMMAND_t;

typedef struct {
  int frame;
  int range_start;
  int range_end;
  int range_direction;
  int range_running;
  int multi_start;
  int multi_end;
  float speed;
  float scale;
  DEM_t *output_dem;
  char output_filename[MAX_LINE];
  int demfile;
  int demfilei;
  DEMFILE_t **demfiles;
  char cdtrack[MAX_LINE];
  float time;
  int loop;
  float a1;
  float a2;
  float a3;
  int inter;
  int prompt;
  float realtime;
  int repeatable_frame;
  int allow_output;
} STATE_t;

#define C_COMMENT       '#'
#define C_DEMFILE       'd'
#define C_FRAME         'f'
#define C_QUIT          'q'
#define C_RANGE         'r'
#define C_RANGE_RUNNING 'R'
#define C_OUTPUT        'o'
#define C_SPEED         's'
#define C_CDTRACK       'c'
#define C_TIME          't'
#define C_HELP          'h'
#define C_LOOP          'l'
#define C_MULTI         'm'
#ifdef WITH_INTERACTIVE 
  #define C_INTER         'i'
#endif /* WITH_INTERACTIVE */
#define C_PROMPT        'p'
#define C_WARRANTY      'w' 

/* prototypes */
#ifndef HAVE_STRSEP
  extern char * strsep(char **pp, const char *delim);
#endif

void syntax(void);
void version(void);
DEMFILE_t *prepare_demfile(char *filename);
COMMAND_t *get_new_command(STATE_t *state);
void do_command(STATE_t *state, COMMAND_t *command);
void print_post_command(STATE_t *state, char cc);
void write_next_range_frame(STATE_t *state);
void write_frame(STATE_t *state);
node* do_read_frame(STATE_t *state, long frameno);
node* do_write_frame(STATE_t *state, node* n);
void free_demfile(DEMFILE_t *demfile);
void open_output(STATE_t *state);
void close_output(STATE_t *state);
void stop_running_range(STATE_t *state);

int main(int argc, char **argv)
{
  int i;
  int c;
  char c_str[2]=" "; 
  COMMAND_t *command;
  STATE_t state;
  int argf;
  int argvf_base;
  static struct option long_options[] = {
    {"help",           0, 0, 'h'},
    {"version",        0, 0, 'V'},
    {0,                0, 0, 0}
  };
  int option_index = 0;

  progname = strdup(argv[0]);
  basetime_init(realtime());

  /* getopt_long shouldn't print any error messages. I will do for myself */
  opterr=0;
  /* scan the command line */
  while (1) {
    c = getopt_long(argc, argv, "hV", long_options, &option_index);

    if (c==-1) break;
    *c_str = (char) c; 
    switch (c) {
      case ':': 
        /* a bit senseless, since there are no command-line options which
           require an additional argument */
        syserror(ARGMIS, c_str); 
      break;
      case 'h':
        syntax();
      break;
      case 'V':
        version(); 
      break;
      case '?': {
        char s[2];
        s[0]=optopt; s[1]='\0';
        syserror(ILLOPT, s);
      }
      break;
      default:
        syserror(EINVAL, c_str);
      break; 
    }
  } 

  /* how many files ? */
  argf = argc-optind;   /* number of files on the command line */
  argvf_base = optind;  /* this is the first file */

  /* there are no DEMFILE_t structures */
  state.demfiles=NULL;
  /* really nothing */
  state.demfilei=0;

  printf("\tDEMcut version %s, Copyright (C) 1996-99 Uwe Girlich\n", VERSION);
  printf("\tDEMcut comes with ABSOLUTELY NO WARRANTY; for details type `w'.\n");
  printf("\tThis is free software, and you are welcome to redistribute it\n");
  printf("\tunder certain conditions;  See the GNU General Public License\n");
  printf("\t(COPYING) for more details.\n");
  printf("\n");
  printf("\t%s version %s, last modified %s\n", HOST, VERSION, DATE);
  printf("\t%s\n", COMMENT);
  printf("\tType `h' for help.\n");
  fflush(stdout);

  node_token_init(DEM_token); /* for token routines */
  /* this prepares all DEM files from the command line */
  if ((command = (COMMAND_t*)malloc(sizeof(COMMAND_t)))==NULL)
    syserror(ENOMEM,"command malloc");
  state.allow_output=0;
  for (i=0;i<argf;i++) {
    command->done=0;
    strcpy(command->args,argv[argvf_base + i]);
    command->code='d'; /* new DEM file command */
    do_command(&state,command);
  }

  /* print out what you did from now on */
  state.allow_output=1;
  /* if there was at least one good DEM file: reactivate it */
  if (state.demfilei) {
    command->done=0;
    strcpy(command->args,state.demfiles[0]->dem.filename);
    command->code='d'; /* new DEM file command */
    do_command(&state,command);
  }
  free(command);

  state.speed=1.0;
  state.scale=1.0/state.speed;
  strcpy(state.output_filename,"demcut.dem");
  state.output_dem=NULL;
  state.demfile=0;
  state.frame=0;
  state.range_start=0;
  if (state.demfilei) {
    state.range_end=state.demfiles[state.demfile]->framei-1;
  }
  else {
    state.range_end=state.range_start;
  }
  state.range_direction = 0;
  state.range_running = 0;
  strcpy(state.cdtrack,"-1\n");
  state.time=0.0;
  state.realtime=basetime();
  state.loop=1;
  state.multi_start=state.range_start;
  state.multi_end=state.range_end;
  state.inter=0;
  state.prompt=1;
  state.repeatable_frame = 0;

  while(1) {
    if ((command=get_new_command(&state))!=NULL) {
      /* do a new command */
      if (!command->done) do_command(&state,command);
      if (command->code==C_QUIT) { free(command); break; }
      free(command);
    }
    else {
      /* nothing important to do */ 
      /* handle the running range command */ 
      if (state.inter && 
          state.range_running &&
          state.realtime <= basetime()) {
        /* write the frame */
        write_next_range_frame(&state);
      }
      else {
        /* in interactive mode: repeat last frame */
        if (state.inter && 
            state.repeatable_frame && 
            state.realtime <= basetime()) {
          write_frame(&state);
        }
      }
    }
  }
    
  for (i=0;i<state.demfilei;i++) {
    DEM_done(&(state.demfiles[i]->dem));
    free_demfile(state.demfiles[i]); 
  }
  free(state.demfiles);
  close_output(&state);
  return 0;
}

void syntax(void)
{
  fprintf(stderr,"DEM file cutter tool\n");
  fprintf(stderr,"DEMcut (c) U. Girlich, 1996-99, Release %s %s (%s)\n",
    VERSION, DATE, COMMENT);
  fprintf(stderr,"demcut [option ...] [demfile ...]\n");
  fprintf(stderr,"-h, --help     display this help and exit.\n");
  fprintf(stderr,"-V, --version  output version information and exit.\n");
  fprintf(stderr,"DEMcut works with the DEM files from the command line.\n");
  fprintf(stderr,"All commands will be read in from stdin.\n");
  fprintf(stderr,"The command `h' gives you a short online help.\n");
  exit(1);
}

void version(void)
{
  fprintf(stderr,"DEMcut %s, %s (%s)\n", VERSION, DATE, COMMENT);
  exit(0);
}

DEMFILE_t *prepare_demfile(char *filename)
{
  DEMFILE_t *demfile;
  BB_t m;
  float oldtime;
  int allocframe;
  node *n, *in;
  long i;

  if (!isDEMbin(filename)) return NULL; 

  if ((demfile=(DEMFILE_t*) malloc(sizeof(DEMFILE_t)))==NULL)
    syserror(ENOMEM,"demfile malloc");
  DEM_init(&(demfile->dem), filename, "rb");
  /* cd track is not interesting but it fills some variables */
  node_delete(do_track_read_bin(&(demfile->dem)));
  demfile->framei = 0;
  allocframe = 0;
  demfile->frames=NULL;
  oldtime = 0.0;
  while (ftell(demfile->dem.file)<(long)demfile->dem.filesize) {
    if (demfile->framei==allocframe) {
      allocframe=allocframe*2+1;
      if ((demfile->frames=(FRAME_t*)realloc(demfile->frames,sizeof(FRAME_t)*allocframe))
         ==NULL) syserror(ENOMEM,"frames realloc");
    }
    demfile->frames[demfile->framei].seekpos = ftell(demfile->dem.file);
    DEM_block_read_bin(&(demfile->dem), &m);
    n=DEM_bin_to_node(&m,0);
    demfile->frames[demfile->framei].timestart = oldtime;
    demfile->frames[demfile->framei].multientry= 0;
    for (in=n->down;in!=NULL;in=in->next) {
      if (in->type==TOKEN_TIME) {
        demfile->frames[demfile->framei].timestart = *(float*)in->down->down; 
      }
      if (in->type==TOKEN_UPDATECOLORS || in->type==TOKEN_UPDATENAME) 
        demfile->frames[demfile->framei].multientry = 1;
    }
    oldtime = demfile->frames[demfile->framei].timestart;
    demfile->framei++;
    node_delete(n);
  }
  if ((demfile->frames=(FRAME_t*)realloc(demfile->frames,sizeof(FRAME_t)*demfile->framei))     ==NULL) syserror(ENOMEM, "frames realloc");
  for (i=0;i<demfile->framei-1;i++) {
    demfile->frames[i].timeend = demfile->frames[i+1].timestart;
    demfile->frames[i].timelength = demfile->frames[i].timeend -
                                    demfile->frames[i].timestart;
  }
  demfile->frames[demfile->framei-1].timeend = demfile->frames[demfile->framei-1].timestart;
  demfile->frames[demfile->framei-1].timelength = 0.0; 
  return demfile;
}

COMMAND_t *get_new_command(STATE_t *state)
{
  int stdin_fd;
  char buffer[MAX_LINE]; 
  COMMAND_t *command;
  char *result;
  char *bufferp;

  command = NULL; /* no command there */
  stdin_fd = fileno(stdin); /* should be 1 but I prefer to check it first */
  #ifdef WITH_INTERACTIVE
    if (fcntl(stdin_fd,F_SETFL,O_NONBLOCK)==-1) syserror(errno,"fcntl");
  #endif /* WITH_INTERACTIVE */
  result = fgets(buffer,MAX_LINE,stdin);
  if (result!=NULL && result[0]!='\0') {
    while (strlen(buffer)>0) {
      if (buffer[strlen(buffer)-1]=='\n' || buffer[strlen(buffer)-1]=='\r')
        buffer[strlen(buffer)-1]='\0';
      else break;
    }
    if ((command = (COMMAND_t*)malloc(sizeof(COMMAND_t)))==NULL)
      syserror(ENOMEM,"command malloc");
    command->done=1; /* will be set to 0 if all args are parsed and something
                        remains to to */
    command->code=buffer[0];
    switch (command->code) {
      case '\r':
      case '\n':
      case C_COMMENT:
        /* do nothing */
      break;
      case C_HELP:
        command->done=0;
      break;
      case C_QUIT:
        command->done=0;
      break;
      case C_DEMFILE:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          if (state->demfilei) {
            print_post_command(state,command->code);
          }
          else {
            printf("ERROR: no DEM file opened yet\n"); fflush(stdout);
          }
        } 
        else {
          /* has one string parameter */
          bufferp=buffer+2;
          result=strsep(&bufferp," \r\n");
          strcpy(command->args,result);
          command->done=0;
        }
      break;
      case C_FRAME:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        } 
        else { 
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (state->demfilei) {
            if (sscanf(result,"%i",&command->argi1)!=1) {
              printf("ERROR: can't interpret \"%s\"\n", result); fflush(stdout);
            }
            else {
              if (command->argi1<0 || 
                  command->argi1>=state->demfiles[state->demfile]->framei) {
                printf("ERROR: frame %i out of range\n", command->argi1); 
                fflush(stdout);
              }
              else {
                command->done=0;
              }
            }
          }
          else {
            printf("ERROR: no DEM file opened for reading frame\n");
            fflush(stdout);
          }
        }
      break;
      case C_RANGE:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        } 
        else { 
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (state->demfilei) {
            if (sscanf(result,"%i",&command->argi1)!=1) {
              printf("ERROR: can't interpret \"%s\"\n", result); fflush(stdout);
            }
            else {
              if (command->argi1<0 || 
                  command->argi1>=state->demfiles[state->demfile]->framei) {
                printf("ERROR: range start %i out of range\n", command->argi1); 
                fflush(stdout);
              }
              else {
                result = strsep(&bufferp," \r\n");
                if (sscanf(result,"%i",&command->argi2)!=1) {
                  printf("ERROR: can't interpret \"%s\"\n", result); 
                  fflush(stdout);
                }
                else {
                  if (command->argi2<0 ||
                      command->argi2>=state->demfiles[state->demfile]->framei) {
                    printf("ERROR: range end %i out of range\n", command->argi2);   
                    fflush(stdout);
                  }
                  else {
                    command->done=0;
                  }
                }
              }
            }
          }
          else {
            printf("ERROR: no DEM file opened for reading range\n");
            fflush(stdout);
          }
        }
      break;
      case C_OUTPUT:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp = buffer+2;
          result = strsep(&bufferp," \r\n");
          if (result==NULL || result[0]=='\0') {
            printf("ERROR: can't read string\n"); fflush(stdout);
          }
          else {
            strcpy(command->args,result);
            command->done=0;
          }
        }
      break;
      case C_SPEED:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (sscanf(result,"%f",&command->argf)!=1) {
            printf("ERROR: can't interpret \"%s\"\n", result); fflush(stdout);
          }
          else {
            if (command->argf<=0.0) {
              printf("ERROR: speed out of range\n"); fflush(stdout);
            }
            else {
              command->done=0;
            }
          }
        }
      break;
      case C_CDTRACK:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp = buffer+2;
          result = strsep(&bufferp," \r\n");
          if (result==NULL || result[0]=='\0') {
            printf("ERROR: can't read string\n"); fflush(stdout);
          }
          else {
            char *b;
            
            /* don't parse anything */              
            /* strcpy(command->args,result); */

            /* parse \t \r \n \f in result to the internal repr. */
            for (b=command->args; *result!='\0'; result++,b++) {
              if (strncmp(result,"\\t",2)==0) { *b = '\t'; result++; continue; }
              if (strncmp(result,"\\r",2)==0) { *b = '\r'; result++; continue; }
              if (strncmp(result,"\\n",2)==0) { *b = '\n'; result++; continue; }
              if (strncmp(result,"\\f",2)==0) { *b = '\f'; result++; continue; }
              *b=*result;
            }
            *b--='\0';

            /* old syntax compatible: we need a whitespace at the end */
            if (*b!='\t' && *b!='\r' && *b!='\n' && *b!='\f') 
              strcat(command->args,"\n"); 

            command->done = 0;
          }
        }
      break;
      case C_TIME:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (sscanf(result,"%f",&command->argf)!=1) {
            printf("ERROR: can't interpret \"%s\"\n", result); fflush(stdout);
          }
          else {
            if (command->argf<=0.0) {
              printf("ERROR: time out of range\n"); fflush(stdout);
            }
            else {
              command->done=0;
            }
          }
        }
      break;
      case C_LOOP:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (sscanf(result,"%i",&command->argi1)!=1) {
            printf("ERROR: can't interpret \"%s\"\n", result); fflush(stdout);
          }
          else {
            if (command->argi1<=0) {
              printf("ERROR: loop %i out of range\n", command->argi1);
              fflush(stdout);
            }
            else {
              command->done=0;
            }
          }
        }
      break;
      case C_MULTI:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (state->demfilei) {
            if (sscanf(result,"%i",&command->argi1)!=1) {
              printf("ERROR: can't interpret \"%s\"\n", result); fflush(stdout);
            }
            else {
              if (command->argi1<0 ||
                  command->argi1>=state->demfiles[state->demfile]->framei) {
                printf("ERROR: multi start %i out of range\n", command->argi1);
                fflush(stdout);
              }
              else {
                result = strsep(&bufferp," \r\n");
                if (sscanf(result,"%i",&command->argi2)!=1) {
                  printf("ERROR: can't interpret \"%s\"\n", result); 
                  fflush(stdout);
                }
                else {
                  if (command->argi2<0 ||
                      command->argi2>=state->demfiles[state->demfile]->framei) {
                    printf("ERROR: multi end %i out of range\n", command->argi2);
                    fflush(stdout);
                  }
                  else {
                    command->done=0;
                  }
                }
              }
            }
          } 
          else {
            printf("ERROR: no DEM file opened for reading multi range\n");
            fflush(stdout);
          }
        }
      break;
      #ifdef WITH_INTERACTIVE
      case C_INTER:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (sscanf(result,"%i",&command->argi1)!=1) {
            printf("ERROR: can't interpret \"%s\"\n", result); 
            fflush(stdout);
          }
          else {
            if (command->argi1!=0 && command->argi1!=1) {
              printf("ERROR: %i should be 0 or 1\n", command->argi1);
              fflush(stdout);
            }
            else {
              command->done=0;
            }
          }
        }
      break;
      #endif /* WITH_INTERACTIVE */
      case C_PROMPT:
        /* without anything else: question */
        if (strlen(buffer)==1) {
          print_post_command(state,command->code);
        }
        else {
          bufferp=buffer+2;
          result = strsep(&bufferp," \r\n");
          if (sscanf(result,"%i",&command->argi1)!=1) {
            printf("ERROR: can't interpret \"%s\"\n", result); fflush(stdout);
          }
          else {
            if (command->argi1!=0 && command->argi1!=1) {
              printf("ERROR: %i should be 0 or 1\n", command->argi1);
              fflush(stdout);
            }
            else {
              command->done=0;
            }
          }
        }
      break;
      case C_WARRANTY:
        command->done=0;
      break;
    }
  }
  return command;
}

void do_command(STATE_t *state, COMMAND_t *command)
{
  int direction;
  int afterlast;
  DEMFILE_t *demfile;
  node *multi, *n, *ni;
  long frame;
  /* TEXT_t output_text; */
  
  if (!command->done) {
    switch (command->code) {
      case C_DEMFILE:
        {
        int i;
        int found=0;

        stop_running_range(state);
        for (i=0;i<state->demfilei;i++) {
          if (strcmp(command->args,state->demfiles[i]->dem.filename)==0) {
            /* an old one */
            found=1; 
            state->demfile=i;
            print_post_command(state,command->code);
            break;
          }
        }
        if (!found) {
          /* a new one */
          demfile=prepare_demfile(command->args);
          if (demfile!=NULL) {
            /* a good new one */
            state->demfile=state->demfilei;
            state->demfilei++;
            state->demfiles=(DEMFILE_t **)realloc(state->demfiles,sizeof(DEMFILE_t*)*state->demfilei);
            if (state->demfiles==NULL) syserror(ENOMEM,"state->demfiles realloc");
            state->demfiles[state->demfile]=demfile;
            print_post_command(state,command->code);
          }
          else {
            /* something is bad */
            printf("ERROR: bad DEM file \"%s\"\n",command->args);
          }
        }
        }
      break;
      case C_FRAME:
        stop_running_range(state);
        state->frame=command->argi1;
        write_frame(state);
        print_post_command(state,command->code);
      break;
      case C_QUIT:
        stop_running_range(state);
        close_output(state);
        print_post_command(state,command->code);
      break;
      case C_RANGE:
        stop_running_range(state);
        state->range_start = command->argi1;
        state->range_end   = command->argi2;
        state->range_direction=(state->range_start<=state->range_end?+1:-1);
        state->range_running = state->range_direction;
        state->frame = state->range_start;
        if (state->inter) 
          print_post_command(state,C_RANGE_RUNNING);
        else
          while (state->range_running) write_next_range_frame(state);
      break;
      case C_OUTPUT:
        stop_running_range(state);
        if (strcmp(state->output_filename,command->args)!=0) {
          close_output(state);
          strcpy(state->output_filename,command->args);
        } 
        print_post_command(state,command->code);
      break;
      case C_SPEED:
        state->speed=command->argf;
        state->scale=1.0/state->speed;
        print_post_command(state,command->code);
      break;
      case C_CDTRACK:
        stop_running_range(state);
        open_output(state);
        strcpy(state->cdtrack,command->args);
        node_delete(
          DEM_block_write_bin(
            node_add_hint(
              node_command_init(TOKEN_TRACK,V_STRING,H_STRING,strdup(state->cdtrack),0),
              H_SIMPLE
            )
          )
        );
        print_post_command(state,command->code);
      break;
      case C_TIME:
        state->time=command->argf;
        print_post_command(state,command->code);
      break;
      case C_LOOP:
        state->loop=command->argi1;
        print_post_command(state,command->code);
      break;
      case C_HELP:
        printf("h\t\t\tPrints this help.\n");
        printf("q\t\t\tQuits the program.\n");
        printf("o filename\t\tSelects a DEM file for writing.\n");
        printf("d filename\t\tSelects a DEM file for reading.\n");
        printf("s floating-number\tSpeeds up further output by this factor.\n");
        printf("t floating-number\tSets the current time.\n");
        printf("l loop\t\t\tLoops every further frame loop times.\n");
        printf("c string\t\tWrites this cd track string to the output.\n");
        printf("f framenumber\t\tWrites the selected frame to the output.\n"); 
        printf("r start end\t\tWrites the selected frame range to the output.\n");
        printf("m start end\t\tWrites multiplayer info from the range to the output.\n");
        #ifdef WITH_INTERACTIVE
          printf("i mode\t\t\tSwitches interactive mode on (1) and off (0).\n");
        #endif /* WITH_INTERACTIVE */
        printf("w\t\t\tPrints the NO WARRANTY message (GPL chapter 11 and 12).\n");
        printf("All commands prints their current value, when used without parameters.\n");
        print_post_command(state,command->code);
      break;
      case C_MULTI:
        stop_running_range(state);
        /* it's easier with short variables */
        demfile=state->demfiles[state->demfile];
        /* read the arguments */
        state->multi_start = command->argi1;
        state->multi_end   = command->argi2;
        direction=(state->multi_start<=state->multi_end?+1:-1);
        afterlast=state->multi_end+direction;
        /* the first message is the camera */
        multi=node_triple_command_init(TOKEN_CAMERA,V_FLOAT,H_ANGLE,
                                       NODE_VALUE_FLOAT_dup(state->a1),
                                       NODE_VALUE_FLOAT_dup(state->a2),
                                       NODE_VALUE_FLOAT_dup(state->a3),
                                       0);
        for (frame=state->multi_start;frame!=afterlast;frame+=direction) {
          if ((demfile->frames+frame)->multientry) {
            /* fprintf(stderr,"%li ",frame); */
            n=do_read_frame(state,frame);
            /* unlink and grab all the `multi' messages */
            restart: ;
            for (ni=n->down;ni!=NULL;ni=ni->next) {
              /* is this message important ? */
              if (ni->type==TOKEN_UPDATENAME || ni->type==TOKEN_UPDATECOLORS) {
                /* unlink this node, correct the start */
                n->down = node_unlink(n->down,ni);
                /* link this node to all the other multi nodes */
                node_add_next(multi,ni);
                /* do it again */
                goto restart;
              }
            }
            /* remove the (non-multi) rest */
            node_delete(n);
          }
        }
        /* make a full block */
        multi=node_init_all(TOKEN_BLOCK,0,multi,0);
        /* write it out and delete it */
        node_delete(do_write_frame(state,multi));
        print_post_command(state,command->code);
      break;
      #ifdef WITH_INTERACTIVE
      case C_INTER:
        if (state->inter != command->argi1)
          stop_running_range(state);
        state->inter=command->argi1;
        print_post_command(state,command->code);
      break;
      #endif /* WITH_INTERACTIVE */
      case C_PROMPT:
        state->prompt=command->argi1;
        print_post_command(state,command->code);
      break;
      case C_WARRANTY:
        printf("                    NO WARRANTY\n\
\n\
  11. BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY\n\
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN\n\
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES\n\
PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED\n\
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF\n\
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS\n\
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE\n\
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,\n\
REPAIR OR CORRECTION.\n\
\n\
  12. IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING\n\
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR\n\
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,\n\
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING\n\
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED\n\
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY\n\
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER\n\
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE\n\
POSSIBILITY OF SUCH DAMAGES.\n\n");
        print_post_command(state,command->code);
      break;
      default:
        printf("ERROR: unknown command %c\n",command->code);
      break;
    }
    fflush(stdout);
    command->done=1;
  }
}

void print_post_command(STATE_t *state, char cc)
{
  DEMFILE_t *demfile;

  if (state->allow_output) {
    switch (cc) {
      case C_DEMFILE:
        demfile=state->demfiles[state->demfile];
        printf("DEMFILE \"%s\", frames: 0 ... %li, time: 0.0s ... %fs\n",
          demfile->dem.filename,
          demfile->framei-1, demfile->frames[demfile->framei-1].timeend);
      break;
      case C_FRAME:
        printf("FRAME %i\n", state->frame);
      break;
      case C_QUIT:
        printf("QUIT\n");
      break;
      case C_RANGE:
        printf("RANGE %i %i\n", state->range_start, state->range_end);
      break;
      case C_RANGE_RUNNING:
        printf("RANGE_RUNNING %i\n", state->range_running);
      break;
      case C_OUTPUT:
        printf("OUTPUT \"%s\"\n", state->output_filename);
      break;
      case C_SPEED:
        printf("SPEED %f\n", state->speed);
      break;
      case C_CDTRACK:
        /* don't parse anything */
        /* printf("CDTRACK \"%s\"\n", state->cdtrack); */
        /* parse \t \r \n \f in state->cdtrack back to the text repr. */
        {
          char buffer[100];
          char *b, *c;
          
          /* 
          for (c=state->cdtrack; *c!='\0'; c++) {
            printf(" '%c' (0x%02x), ", *c, *c);
          }
          */
          for (c=state->cdtrack, b=buffer; *c!='\0'; c++, b++) {
            if (*c=='\t') { *b++ = '\\'; *b='t'; continue; }
            if (*c=='\r') { *b++ = '\\'; *b='r'; continue; }
            if (*c=='\n') { *b++ = '\\'; *b='n'; continue; }
            if (*c=='\f') { *b++ = '\\'; *b='f'; continue; }
            *b = *c;
          }
          *b = '\0';
          printf("CDTRACK \"%s\"\n", buffer); 
        }
      break;
      case C_TIME:
        printf("TIME %f\n", state->time);
      break;
      case C_LOOP:
        printf("LOOP %i\n", state->loop);
      break;
      case C_HELP:
        printf("HELP\n");
      break;
      case C_MULTI:
        printf("MULTI %i %i\n", state->multi_start, state->multi_end);
      break;
      #ifdef WITH_INTERACTIVE
      case C_INTER:
        printf("INTER %i\n", state->inter);
      break;
      #endif /* WITH_INTERACTIVE */
      case C_PROMPT:
        printf("PROMPT %i\n", state->prompt);
      break;
      case C_WARRANTY:
        printf("WARRANTY\n");
      break;
      default:
        printf("ERROR: unknown command %c\n",cc);
      break;
    }
    fflush(stdout);
  }
}

void write_next_range_frame(STATE_t *state)
{
  /* write the frame */
  write_frame(state);
  if (state->frame == state->range_end) {
    /* stop the running range */
    state->range_running = 0;
    if (state->inter)
      print_post_command(state,C_RANGE_RUNNING);
    /* the last frame */
    print_post_command(state,C_RANGE);
  }
  else {
    /* a normal frame */
    state->frame += state->range_direction;
  }
}

void write_frame(STATE_t *state)
{
  DEMFILE_t *demfile;
  FRAME_t *frame;
  /* unsigned char buffer[MAX_MESSAGE_SIZE]; */
  int l;
  node *n, *in;

  /* it's easier with short variables */
  demfile=state->demfiles[state->demfile];
  frame=demfile->frames+state->frame;
  /* read frame */
  n=do_read_frame(state, state->frame);
  /* we'll write a frame, so we have something to repeat afterwards */
  state->repeatable_frame = 1;

  /* manipulate everything */
  for (l=0;l<state->loop;l++) {
    for (in=n->down;in!=NULL;in=in->next) {
      /* memorize the last output camera angles */
      if (in->type == TOKEN_CAMERA) {
        state->a1 = *(float*)(in->down->down);
        state->a2 = *(float*)(in->down->next->down);
        state->a3 = *(float*)(in->down->next->next->down);
      }
      /* manipulate time */
      if (in->type == TOKEN_TIME) {
        *(float*)(in->down->down) = state->time;
      }
      /* don't allow to repeat a block with a signonnum message */
      /* Quake doesn't like it at all  */
      if (in->type == TOKEN_SIGNONNUM) {
        state->repeatable_frame = 0;
      }
      /* don't allow to repeat a block with a disconnect message */
      /* Quake closes the pipe and DEMcut gets a `broken pipe' message. */
      if (in->type == TOKEN_DISCONNECT) {
        state->repeatable_frame = 0;
      }
    }
    /* write it out */
    do_write_frame(state,n);
    /* new time */
    state->time+=state->scale*frame->timelength;
    state->realtime=basetime() + state->scale*frame->timelength;;
    /*
    fprintf(stderr,"frame %i will end at %f\n",state->frame,state->realtime);
    fprintf(stderr,"current time %f\n",basetime()); 
    */
  }

  /* remove it again */
  node_delete(n);
}

node* do_read_frame(STATE_t *state, long frameno)
{
  DEMFILE_t *demfile;
  FRAME_t *frame;
  BB_t m;

  /* it's easier with short variables */
  demfile=state->demfiles[state->demfile];
  frame=demfile->frames + frameno;

  /* seek */
  if (fseek(demfile->dem.file, frame->seekpos, SEEK_SET)==-1)
    syserror(errno,demfile->dem.filename);

  /* read block in */
  DEM_block_read_bin(&(demfile->dem), &m);

  /* construct the node tree and return the root of it */
  return DEM_bin_to_node(&m,0x00);
}

node* do_write_frame(STATE_t *state, node* n)
{
  /* write only, if there is something after the camera message */
  if (n->down->next != NULL) {
    /* make sure output is open */
    open_output(state);

    /* write it out */
    DEM_block_write_bin(n);
  }
  return n;
}

void free_demfile(DEMFILE_t *demfile)
{
  free(demfile->frames);
  free(demfile);
}

void open_output(STATE_t *state)
{
  if (state->output_dem == NULL) {
    if ((state->output_dem = (DEM_t*)malloc(sizeof(DEM_t)))==NULL)
      syserror(ENOMEM,"output_dem malloc");
    DEM_init(state->output_dem,state->output_filename,"wb");
    DEM_prepare_write_bin(state->output_dem);
    /* the camera angles are 0.0 at first */
    state->a1 = state->a2 = state->a3 = 0.0;
    /* new file, nothing written */
    state->repeatable_frame = 0;
  }
}

void close_output(STATE_t *state)
{
  if (state->output_dem != NULL) {
    DEM_done(state->output_dem);
    free(state->output_dem);
    state->repeatable_frame = 0;
  }
  state->output_dem=NULL;
}

void stop_running_range(STATE_t *state)
{
  if (state->range_running) {
    state->range_running = 0;
    if (state->inter) 
      print_post_command(state,C_RANGE_RUNNING);
    state->range_end = state->frame - state->range_direction;
    print_post_command(state,C_RANGE);
    state->frame = state->range_end;
  }
}

/*- file end demcut.c --------------------------------------------------------*/
