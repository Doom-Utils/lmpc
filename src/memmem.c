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
|  memmem.c  -  implementation, missing standard C routine                    |
\****************************************************************************/


/*
 *  Some cross-compilers don't have all standard C library functions.
 *  I copied the following code from the GNU C Library source.
 *  Look in COPYING.LIB for distribution hints of this code.
 */                      


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif


#include <string.h>


void *memmem(const void *haystack, size_t haystack_len,
             const void* needle, size_t needle_len)
{
  register const char *begin;
  register const char *const last_possible
    = (const char *) haystack + haystack_len - needle_len;
    
  if (needle_len == 0)
    return (void *) &((const char *) haystack)[needle_len - 1];
    
  for (begin = (const char *) haystack; begin <= last_possible; ++begin)
    if (begin[0] == ((const char *) needle)[0] &&
        !memcmp ((const void *) &begin[1],
                 (const void *) ((const char *) needle + 1),
                 needle_len - 1))
      return (void *) begin;
  return NULL;
}


/*-- file end memmem.c -----------------------------------------------------*/
