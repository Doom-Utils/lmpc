/*
 * This code implements the LZW compression algorithm as used in
 * DUKE NUKEM 3D.
 *
 * The code in this file is directly derived from the public domain 'compress'
 * written by Spencer Thomas, Joe Orost, James Woods, Jim McKie, Steve Davies,
 * Ken Turkowski, Dave Mack and Peter Jannesen.
 *
 * The algorithm is very similar but not quite identical to
 * the LZW (de)compression method used by COMPRESS.
 *
 * This implementation relies on int having at least 32 bit (I think).
 *
 * Steffen Winterfeldt
 * Steffen.Winterfeldt@itp.uni-leipzig.de
 *
 * Revision 1.1 - Feb 13 1996, 14:30
 * Revision 1.2 - June 14 1998, some glitches removed for SunOS compilers (UG)
 * Revision 1.3 - May 7 1999, make it C++ compiler ready (UG)
 *
 */


#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif


/*
 * define to have game tics subtracted
 */
/* #define SUM_TICS */

/*
 * if you want lots of debug info, define PRINTCODES
 */
#undef PRINTCODES

/*
 * maximum code size that can be handled; 14 bits should be enough
 */
#define MAXBITS 15
#define HSIZE 69001

#ifdef PRINTCODES
  #include <stdio.h>    /* fprintf() */
#endif
#include <stdlib.h>	/* malloc(), free() */
#include <string.h>	/* memcpy(), memset() */
#ifdef HAVE_MEMORY_H
  #include <memory.h>   /* memset(), memcpy() on BSD */
#endif

static int free_ent;

static unsigned char *out_buf;
static int out_buf_size, out_buf_ptr;

static int putcode(int);

/*
 * returns number of compressed bytes or
 * a negative number (-6 .. -1) indicating some error
 * -1 : malloc failed
 * -2 : code size > 15 bits
 * -5 : output buffer too small
 * -6 : uncompressed size is not a multiple of tic_size
 *
 */

int compress(unsigned char *o_buf, long o_buf_size,
             unsigned char *i_buf, long i_buf_size,
             int last_code, int tic_size, int *used_last_code)
{
  int *t_hash;
  unsigned *t_code;		/* short would suffice, ... */

  int i = 0, c, ent, fcode;

  int i_buf_ptr = 0;
#ifdef SUM_TICS
  int j, k;
#endif

  t_hash = (int*)malloc(HSIZE * sizeof(*t_hash));
  if(t_hash == NULL) return -1;
  t_code = (unsigned*)malloc(HSIZE * sizeof(*t_code));
  if(t_code == NULL) return free(t_hash), -1;

  out_buf_ptr = 0;
  out_buf_size = o_buf_size;
  out_buf = o_buf;

#ifdef PRINTCODES
  fprintf(stderr, "++ compress ++\n");
  fprintf(stderr, "i_buf_size: %d, o_buf_size: %d\n", (int) i_buf_size, (int) o_buf_size);
  fprintf(stderr, "last code: %d\n", last_code);
#endif

#ifdef SUM_TICS
  if(!tic_size || i_buf_size % tic_size) return -6;

  if(i_buf_size > tic_size) {
    j = (i_buf_size / tic_size) - 1;
    k = j * tic_size;
    while(j--) {
      for(i = 0; i < tic_size; i++) {
        i_buf[k + i] -= i_buf[k + i - tic_size];
      }
      k -= tic_size;
    }
  }
  i = 0;
#endif

  if(!last_code) {
    /* last_code == 0 --> no compression */

    if(i_buf_size > o_buf_size) return -5;
    memcpy(o_buf, i_buf, i_buf_size);

#ifdef PRINTCODES
    fprintf(stderr, "++ compressed size %d ++\n", (int) i_buf_size);
#endif

    return i_buf_size;
  }

  free_ent = 256;
  memset(t_hash, -1, sizeof(*t_hash) * HSIZE);
    
  ent = i_buf[i_buf_ptr++];

  while(i_buf_ptr < i_buf_size) {
    c = i_buf[i_buf_ptr++];

    fcode = (c << MAXBITS) + ent;
    i = (c << 8) ^ ent;		/* hash index */

    while(t_hash[i] != fcode && t_hash[i] != -1) {
      i = (i + 5003) % HSIZE;
    }

    if(t_hash[i] == fcode) {
      ent = t_code[i];
    }
    else {
      if(putcode(ent)) return -5;
      ent = c;
      t_code[i] = free_ent++;
      t_hash[i] = fcode;
      if(free_ent >= (1 << MAXBITS)) return -2;
    }
  }

  if(putcode(ent)) return -5;
  if(putcode(-1)) return -5;

#ifdef PRINTCODES
  fprintf(stderr, "++ compressed size %d ++\n", out_buf_ptr);
#endif

  free(t_code); free(t_hash);

  *used_last_code = free_ent + 1;
  return out_buf_ptr;
}

static int putcode(int code)
{
  static int n_bits = 9, max_code = 1 << 9;
  static unsigned int bits = 0, buf = 0;

  if(out_buf_ptr >= out_buf_size) {

#ifdef PRINTCODES
    fprintf(stderr, "++ output buffer full ++\n");
#endif

    return -1;
  }
  
  if(code < 0) {
    if(bits) out_buf[out_buf_ptr++] = buf;
    n_bits = 9; max_code = 1 << 9;
    return buf = bits = 0;
  }

  if(free_ent >= max_code) {
    max_code = 1 << ++n_bits;
                  
#ifdef PRINTCODES
    fprintf(stderr, "++ buffer: %d bits ++\n", bits);
    fprintf(stderr, "++ change to %d bits ++\n", n_bits);
#endif

    if(n_bits > MAXBITS) {

#ifdef PRINTCODES
      fprintf(stderr, "++ code size > 15 bits ++\n");
#endif

      return -2;
    }
  }
 
  buf |= code << bits; bits += n_bits;

  do {
    if(out_buf_ptr >= out_buf_size) {

#ifdef PRINTCODES
      fprintf(stderr, "++ output buffer full ++\n");
#endif

      return -1;
    }
    out_buf[out_buf_ptr++] = buf & 0xff;
    buf >>= 8; bits -= 8;
  } while(bits >= 8);

#ifdef PRINTCODES
  if(free_ent < code) fprintf(stderr, "++ uups ++");
  fprintf(stderr, "%5d: %5d\n", (int) (free_ent), (int) code);
#endif

  if(free_ent < code) return -1;
  
  return 0;
}

