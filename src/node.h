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
|  node.h  -  header file of node.c, general node tree routines              |
\****************************************************************************/


#ifndef __NODE_H
#define __NODE_H


#include <stdlib.h> 
#include "tools.h"
#include "lmpc.h"

typedef struct node {
                      struct node* next;
                      char* comment;
                      int type;
                      int hint;
                      struct node* down;
                      int pos;
} node;

typedef struct {
  char* string; 
  int type;
  unsigned char id;
} token_t;

#define NODE_VALUE 0x2000

#define V_INT     (NODE_VALUE | 0x01)
#define V_BYTEHEX (NODE_VALUE | 0x02)
#define V_FLOAT   (NODE_VALUE | 0x03)
#define V_TIME    (NODE_VALUE | 0x04)
#define V_STRING  (NODE_VALUE | 0x05)
#define V_TOKEN   (NODE_VALUE | 0x06)

#define NODE_TOKEN 0x4000

#define GEN_NOTHING (NODE_TOKEN | 0x01)

typedef node* nodep;

/* general routines, which are called even from C code */
/* C code means in this connection the yacc/lex created stuff */
#ifdef __cplusplus
extern "C" {
#endif
extern node* node_init(int type, void* down, int pos);
extern node* node_init_all(int type, int hint, void* down, int pos);
extern node* node_add_down(node* n, node* down);
extern node* node_add_next(node* n, node* next);
extern void node_delete(node* n);
extern node* node_add_hint_next(node* n, int hint);
extern char *node_token_string(int type);
extern int node_count_next(node *base);
extern node* node_add_type_next(node* n, int type);
extern node* do_block(node* b);

#ifdef __cplusplus
}
#endif

extern node* node_malloc(void);
extern void node_free(node* n);
extern node* node_command_init(int type, int down_type, int down_hint, void* down_down, int pos);
extern node* node_triple_command_init(int type, int down_type, int down_hint,
                                      void* down_down0,
                                      void* down_down1,
                                      void* down_down2,
                                      int pos);
extern node* node_fourfold_command_init(int type, int down_type, int down_hint,
                                        void* down_down0,
                                        void* down_down1,
                                        void* down_down2,
                                        void* down_down3,
                                        int pos);
extern node* node_link(node* n, node* next);
extern node* node_combine3(node* n1, node* n2, node* n3);
extern node* node_add_comment(node* n, char* comment);
extern node* node_add_hint(node* n, int hint);
extern node* node_add_type(node* n, int type);
extern node* node_unlink(node *base, node *n);

extern node* bitlist_to_nodes(unsigned long field);
extern unsigned long nodes_to_bitlist(node* n);

extern long int *NODE_VALUE_INT_dup(long int value);
extern long int *NODE_VALUE_BYTEHEX_dup(long int value);
extern float    *NODE_VALUE_FLOAT_dup(float value);
extern double   *NODE_VALUE_DOUBLE_dup(double value);
extern float    *NODE_VALUE_TIME_dup(float value);
extern char*    NODE_VALUE_STRING_dup(char *value);

extern void node_token_init(token_t *t);
extern unsigned char node_token_id(int type);

extern void node_write_text_init(TEXT_t* t);
extern void node_write_text(node* b, int depth);

extern opt_t *glob_opt;

extern void default_block_edit(node* b);
extern void (*do_block_edit)(node* b);
extern node* default_block_output(node* b);
extern node* (*do_block_output)(node* b);


#endif /* __NODE_H */


/*-- file end node.h ---------------------------------------------------------*/
