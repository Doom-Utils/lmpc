/*
 * This code implements the LZW decompression algorithm as used in
 * DUKE NUKEM 3D.
 * I added lots of range checks that are _all_ omitted in DUKE3D's code.
 *
 * The algorithm is very similar but not quite identical to
 * the LZW (de)compression method used by COMPRESS.
 *
 * This implementation relies on int having at least 32 bit (I think).
 *
 * Steffen Winterfeldt
 * Steffen.Winterfeldt@itp.uni-leipzig.de
 *
 * Revision 1.1 - Feb 13 1996, 14:33
 * Revision 1.2 - June 24 1998, some glitches removed for SunOS compilers (UG)
 * Revision 1.3 - May 7 1999, make it C++ compiler ready (UG)
 *
 */


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif


/*
 * define to have game tics added up
 */
/* #define SUM_TICS */

/*
 * if you want lots of debug info, define PRINTCODES
 */
#undef PRINTCODES
/* #define PRINTCODES */

/*
 * maximum code size that can be handled; 14 bits should be enough
 */
#define MAXBITS 15

#ifdef PRINTCODES
  #include <stdio.h>	/* fprintf() */
#endif
#include <stdlib.h>	/* malloc() */
#include <string.h>	/* memcpy() */
#ifdef HAVE_MEMORY_H
  #include <memory.h>   /* memset(), memcpy() on BSD */
#endif

/*
 * returns number of decompressed bytes or
 * a negative number (-6 .. -1) indicating some error
 * -1 : malloc failed
 * -2 : code size > 15 bits
 * -3 : input buffer too small
 * -4 : undefined code found
 * -5 : output buffer too small
 * -6 : uncompressed size is not a multiple of tic_size
 *
 */

int decompress(unsigned char *i_buf, long i_buf_size,
               unsigned char *o_buf, long o_buf_size,
               int last_code, int tic_size)
{
  static int init = 1;
  static unsigned *t_pred;		/* short would suffice, but who cares... */
  static unsigned char *t_suff, *stack;

  int i_buf_ptr = 0, o_buf_ptr = 0, stack_ptr;
  unsigned bits = 0, buf = 0;
  unsigned code, free_ent = 256, n_bits = 9, max_code = 1 << 9;
#ifdef SUM_TICS
  int i, j, k;
#endif

  if(init) {
    t_suff = (unsigned char*)malloc((1 << MAXBITS) * sizeof(*t_suff));
    if(t_suff == NULL) return -1;
    stack = (unsigned char*)malloc((1 << MAXBITS) * sizeof(*stack));
    if(stack == NULL) return -1;
    t_pred = (unsigned *)malloc((1 << MAXBITS) * sizeof(*t_pred));
    if(t_pred == NULL) return -1;

    init = 0;
  }

#ifdef PRINTCODES
  fprintf(stderr, "** uncompress **\n");
  fprintf(stderr, "i_buf_size: %d, o_buf_size: %d\n", (int) i_buf_size, (int) o_buf_size);
  fprintf(stderr, "last code: %d\n", last_code);
#endif

  if(last_code) {
    do {
      do {
        buf |= i_buf[i_buf_ptr++] << bits; bits += 8;
      } while(bits < n_bits);

      code = buf & ((1 << n_bits) - 1);
      buf >>= n_bits;
      bits -= n_bits;
      
      if(i_buf_ptr <= i_buf_size) {
      
#ifdef PRINTCODES
        if(free_ent < code) fprintf(stderr, "** uups **");
        fprintf(stderr, "%5d: %5d --> ", (int) free_ent, (int) code);
#endif

        if(free_ent <= code) return -4;		/* really = ? */

        t_pred[free_ent] = code;
        stack_ptr = 0;

        while(code >= 256) {
          stack[stack_ptr++] = t_suff[code];
          code = t_pred[code];
        }

        stack_ptr--;
        if(o_buf_ptr >= o_buf_size) return -5;
        o_buf[o_buf_ptr++] = code;

#ifdef PRINTCODES
        fprintf(stderr, "%02x ", (int) (o_buf[o_buf_ptr - 1] & 0xff));
#endif
        
        while(stack_ptr >= 0) {
          if(o_buf_ptr >= o_buf_size) return -5;
          o_buf[o_buf_ptr++] = stack[stack_ptr--];

#ifdef PRINTCODES
          fprintf(stderr, "%02x ", (int) (o_buf[o_buf_ptr - 1] & 0xff));
#endif

        }
        
        t_suff[free_ent - 1] = code;
        t_suff[free_ent++] = code;

#ifdef PRINTCODES
        fprintf(stderr, "\n");
#endif
        
        if(free_ent >= max_code) {
          max_code = 1 << ++n_bits;

#ifdef PRINTCODES
          fprintf(stderr, "** buffer: %d bits **\n", bits);
          fprintf(stderr, "** change to %d bits **\n", n_bits);
#endif
  	if(n_bits > 15) return -2;
        }
      }
      else {
        return -3;
      }
    }
    while(i_buf_ptr < i_buf_size && free_ent < (unsigned)last_code);

#ifdef PRINTCODES
    fprintf(stderr, "** bits left: %d **\n", bits);
#endif

  }
  else {
    /* last_code == 0 --> no compression */

    if(i_buf_size > o_buf_size) return -5;
    memcpy(o_buf, i_buf, i_buf_size);
    o_buf_ptr = i_buf_size;
  }

#ifdef SUM_TICS
  if(!tic_size || o_buf_ptr % tic_size) return -6;

  if(o_buf_ptr > tic_size) {
    j = (o_buf_ptr / tic_size) - 1;
    k = 0;
    while(j--) {
      for(i = 0; i < tic_size; i++) {
        o_buf[k + tic_size + i] += o_buf[k + i];
      }
      k += tic_size;
    }
  }
#endif

  return o_buf_ptr;
}

