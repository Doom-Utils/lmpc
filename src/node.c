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

/****************************************************************************\
|  node.c  -  implementation, general node routines                          |
\****************************************************************************/


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include "binblock.h"
#include "node.h"
#include "tools.h"
#include "lmpc.h"
#include <string.h>
#ifdef HAVE_MEMORY_H
  #include <memory.h>
#endif

token_t *node_token_field;
TEXT_t *node_text;
opt_t *glob_opt;


void (*do_block_edit)(node* b) = NULL;
node* (*do_block_output)(node* b) = NULL;


node* node_malloc(void)
{
  node* t;

  t = (node*) malloc(sizeof(node));
  if (t==NULL) syserror(ENOMEM, "node");

  return t;
}

void node_free(node* n)
{
  if (n!= NULL) free(n);
}

node* node_init(int type, void* down, int pos)
{
  node* t;

  t = node_malloc();
  t->next = NULL;
  t->comment = NULL;
  t->type = type;
  t->hint = 0;
  t->down = (node*) down;
  t->pos = pos; 

  return t;
}

node* node_command_init(int type, int down_type, int down_hint, void* down_down, int pos)
{
  node* down;

  down       = node_init(down_type,down_down, pos); 
  down->hint = down_hint;

  return node_init(type, down, pos);
}

node* node_triple_command_init(int type, int down_type, int down_hint,
                               void* down_down0, 
                               void* down_down1, 
                               void* down_down2, 
                               int pos)
{
  return node_init_all(type,0,
           node_add_next(
             node_init_all(down_type,down_hint,down_down0,pos),
             node_add_next(
               node_init_all(down_type,down_hint,down_down1,pos),
               node_init_all(down_type,down_hint,down_down2,pos))),
         pos);
}

node* node_fourfold_command_init(int type, int down_type, int down_hint,
                               void* down_down0, 
                               void* down_down1, 
                               void* down_down2, 
                               void* down_down3,
                               int pos)
{
  return node_init_all(type,0,
           node_add_next(
             node_init_all(down_type,down_hint,down_down0,pos),
             node_add_next(
               node_init_all(down_type,down_hint,down_down1,pos),
               node_add_next(
                 node_init_all(down_type,down_hint,down_down2,pos),
                 node_init_all(down_type,down_hint,down_down3,pos)))),
         pos);
}

node* node_init_all(int type, int hint, void* down, int pos)
{
  node* n;

  n = node_init(type, down, pos);
  if (hint) n->hint = hint;

  return n;
}

node* node_add_down(node* n, node* down)
{
  n->down = down;
  return n;
}

node* node_add_next(node* n, node* next)
{
  node* nbase=n;
  for (;n->next!=NULL;n=n->next) ;
  n->next = next;
  return nbase;
}

node* node_link(node* n, node* next)
{
  return (n==NULL ? next : node_add_next(n,next));
}

node* node_combine3(node* n1, node* n2, node* n3)
{
  return node_add_next(node_add_next(n1,n2),n3);
}

node* node_add_comment(node* n, char* comment)
{
char a;
  /* do only anything, if the comment is not NULL */
  if (comment != NULL) {
    /* the latest survives, I should maybe concatenate both strings ?! */
    if (n->comment!=NULL) {
      free(n->comment);
      n->comment=NULL;
    }
    n->comment=comment;
    a = n->comment[0];
    /* fprintf(stderr,"n=0x%08lx *0x%08lx=0x%02x\n",(long)n,(long)n->comment,a); */
  }
  return n;
}

node* node_add_hint(node* n, int hint)
{
  n->hint = hint;
  return n;
}

node* node_add_hint_next(node* n, int hint)
{
  node* tn;
  for ( tn = n ; tn != NULL ; tn = tn->next ) node_add_hint(tn, hint);
  return n;
}

node* node_add_type(node* n, int type)
{
  n->type = type;
  return n;
}

node* node_add_type_next(node* n, int type)
{
  for ( ; n != NULL ; n = n->next ) node_add_type(n, type);
  return n;
}

void node_delete(node* n)
{
  node* next;
  int type;
  node* down;

  while (n!=NULL) {
    next = n->next;
    type = n->type;
    down = n->down;
    if(n->comment!=NULL) {
      free(n->comment);
      n->comment=NULL;
    }
    if (type & NODE_VALUE) {
      free(down);
    }
    else {
      node_delete(down);
    }
    node_free(n);
    n=next;
  }
}
 
node* node_unlink(node *base, node *n)
{
  node *ni, *no;

  for (no=NULL,ni=base;ni!=NULL;ni=ni->next) {
    if (n==ni) {
      if (no==NULL)
        base = ni->next;
      else
        no->next = ni->next;
      n->next = NULL;
      return base;
    }
    no=ni;
  }
  syserror(DINTE,"node_unlink");
  return base;
}

int node_count_next(node *base)
{
  int count = 0;
  for ( ; base!=NULL ; base = base->next ) count++;
  return count;
}


/* complex structures in nodes and vide versa */
node* bitlist_to_nodes(unsigned long field)
{
  int i;
  node* n = NULL;
    
  for ( i=0 ; field ; i++, field>>=1 ) {
    if (field & 0x01)
      n=node_link(n,node_init(V_INT, NODE_VALUE_INT_dup(i),0));
  }  
  return n;
}
                    
                    
unsigned long nodes_to_bitlist(node* n)
{
  unsigned long field = 0; 
                      
  for ( ; n!=NULL ; n=n->next ) {
    field |= 0x01 << *(long*)(n->down);
    /* printf("%ld ", *(long*)(n->down)); */ 
  }
  /* printf("= %lu\n", field); */ 
  return field;
}
                                

/*----------------------------------------------------------------------------*/

long int *NODE_VALUE_INT_dup(long int value)
{
  long int *p;

  if ((p=(long int*)malloc(sizeof(value)))==NULL) {
    syserror(ENOMEM,"NODE_VALUE_INT_dup");
  }
  return (long int*) memcpy(p,&value,sizeof(value));
}

long int *NODE_VALUE_BYTEHEX_dup(long int value)
{
  long int *p;

  if ((p=(long int*)malloc(sizeof(value)))==NULL) {
    syserror(ENOMEM,"NODE_VALUE_BYTEHEX_dup");
  }
  return (long int*) memcpy(p,&value,sizeof(value));
}

float *NODE_VALUE_FLOAT_dup(float value)
{
  float *p;

  if ((p=(float *)malloc(sizeof(value)))==NULL) {
    syserror(ENOMEM,"NODE_VALUE_FLOAT_dup");
  }
  return (float*) memcpy(p,&value,sizeof(value));
}

double *NODE_VALUE_DOUBLE_dup(double value)
{
  double *p;

  if ((p=(double *)malloc(sizeof(value)))==NULL) {
    syserror(ENOMEM,"NODE_VALUE_DOUBLE_dup");
  }
  return (double*) memcpy(p,&value,sizeof(value));
}

float *NODE_VALUE_TIME_dup(float value)
{
  float *p;

  if ((p=(float *)malloc(sizeof(value)))==NULL) {
    syserror(ENOMEM,"NODE_VALUE_TIME_dup");
  }
  return (float*) memcpy(p,&value,sizeof(value));
}

char *NODE_VALUE_STRING_dup(char *value)
{
  char *p;
  
  if (value!=NULL) {
    if ((p=strdup(value)) == NULL) {
      syserror(ENOMEM, "NODE_VALUE_STRING_dup");
    }
    return p;
  }
  else {
    return NULL;
  }
}

/*----------------------------------------------------------------------------*/

void node_token_init(token_t *t)
{
  node_token_field = t;
}

char *node_token_string(int type)
{
  token_t *i;

  for (i=node_token_field;i->string!=NULL;i++) {
    if (type==i->type) return i->string;
  }
  return NULL;
}

unsigned char node_token_id(int type)
{
  token_t *i;

  for (i=node_token_field;i->string!=NULL;i++) {
    if (type==i->type) return i->id;
  }
  return 0;
}

/*----------------------------------------------------------------------------*/

void node_write_text_init(TEXT_t* t)
{
  node_text = t;
}

#define startnewline(x) sprintf(ts, "%.*s", x, "                     "); cs[0]='\0'

void node_write_text_recursive(int depth, char* ts, char* cs, 
                               node* n, int around);

void node_write_text(node* b, int depth)
{
  /*
   * MAX_STRING_SPACE + max(command length) should be enough but who 
   * cares about some bytes anyway?
   */
  char ts[MAX_STRING_SPACE * 2];
  char cs[MAX_STRING_SPACE * 2];
    
  startnewline(depth);
  node_write_text_recursive(depth, ts, cs, b, 1); 
}
        
void output_text_and_comment(FILE* f, char* ts, char* cs)
{
  if (strlen(cs)>0) {
    strcat(ts," // "); 
    strcat(ts, cs);
  }
  if (WriteLine(f,ts)==0) syserror(errno,"output file");
  if (fflush(f) != 0) syserror(errno,"output file");
}

void node_write_text_recursive(int depth, char* ts, char* cs, 
                               node* n, int around)
{
  char* string;
  node* i;
  /* the space for an (octal expanded) string */
  char self[MAX_STRING_SPACE];
  char* sp;

  for (i=n; i!=NULL; i=i->next) {
    /* fprintf(stderr, "type=%i,", i->type); */ 
    if ((string=node_token_string(i->type))!=NULL) {
      /* fprintf(stderr,"=%s\n", string); */ 
      /* token */
      sprintf(ts+strlen(ts), "%s", string);
      if (i->comment!=NULL) {
        sprintf(cs+strlen(cs), "%s", i->comment);
      }
      if (i->down!=NULL) {
        if (node_token_string(i->down->type)!=NULL) {
          sprintf(ts+strlen(ts), " {");
          output_text_and_comment(node_text->file, ts, cs);
          startnewline(depth+1);
          node_write_text_recursive(depth+1, ts, cs, i->down, 1);
          startnewline(depth);
          sprintf(ts+strlen(ts), "}");
        }
        else {
          sprintf(ts+strlen(ts), " ");
          node_write_text_recursive(depth+1, ts, cs, i->down, 0);
          sprintf(ts+strlen(ts), ";");
        }
      }
      else {
        if (i->type!=GEN_NOTHING)
          sprintf(ts+strlen(ts), ";");
      }
    }
    else {
      /* value */
      /* fprintf(stderr,"value\n"); */ 
      if (i->down==NULL) {
        fprintf(stderr, "no value in node type %i, hint %i\n", i->type, i->hint); 
        syserror(GENERR, "wrong internal structure");
      }
      switch (i->type) {
        case V_INT:
          sprintf(ts+strlen(ts), "%li", *(long*)i->down);
        break;
        case V_BYTEHEX:
          if (strlen(ts)+4>79) {
            output_text_and_comment(node_text->file, ts, cs);
            startnewline(depth);
          }
          sprintf(ts+strlen(ts), "0x%02X", (unsigned char)(*(long*)i->down));
        break;
        case V_FLOAT:
          sprintf(ts+strlen(ts), "%#.9g", *(float*)i->down);
        break;
        case V_TIME:
          Time2StringLong(*(float*)i->down,ts+strlen(ts));
        break;        
        case V_STRING:
          /* fprintf(stderr,"V_STRING start\n"); */
          for (string=(char*)i->down, sp=self; *string; string++) {
            if ((unsigned char)*string >= '\040') {
              /* normal characters */
              switch (*string) {
                case '"':
                  *sp++ = '\\';
                  *sp++ = '"';
                break;
                case '\\':
                  *sp++ = '\\';
                  *sp++ = '\\';
                break;
                default:
                  *sp++ = *string;
                break;
              }
            }
            else {
              /* special characters */
              switch (*string) {
                case '\n':
                  *sp++ = '\\'; *sp++ = 'n';
                break;
                case '\r':
                  *sp++ = '\\'; *sp++ = 'r';
                break;
                default:
                  sprintf(sp, "\\%03o", (unsigned char)*string);
                  sp+=strlen(sp);
                break;
              }
            }
          }
          *sp = '\0';
          if (strlen(ts)+1+strlen(self)+1>79) {
            output_text_and_comment(node_text->file, ts, cs);
            startnewline(depth);
          }
          sprintf(ts+strlen(ts), "\"%s\"", self);
          /* fprintf(stderr,"V_STRING end"); */ 
        break;
        default:
          fprintf(stderr,"wrong node type: %i\n", i->type);
          syserror(GENERR, "wrong internal structure");
        break;
      }
    }
    if (around==0 && i->next!=NULL) sprintf(ts+strlen(ts), " ");
    if (around==1) {
      output_text_and_comment(node_text->file, ts, cs);
      startnewline(depth);
    }
  }
}

/******************************************************************************/


node* do_block(node* b)
{
  /* I may insert here some more (optional) hooks */
  if (do_block_edit != NULL) {
    do_block_edit(b);
  }
  if (do_block_output == NULL) {
    syserror(GENERR, "do_block_output == NULL");
  }  
  return do_block_output(b);
}


/*-- file end node.c ---------------------------------------------------------*/
