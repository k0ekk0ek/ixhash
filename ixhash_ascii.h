#ifndef IXHASH_ASCII_H_INCLUDED
#define IXHAHS_ASCII_H_INCLUDED

// most of this stuff can actually be copied from glib
// most of this stuff is actually copied from glib :)

/* Functions like the ones in <ctype.h> that are not affected by locale. */
typedef enum {
  IXHASH_ASCII_ALNUM  = 1 << 0,
  IXHASH_ASCII_ALPHA  = 1 << 1,
  IXHASH_ASCII_CNTRL  = 1 << 2,
  IXHASH_ASCII_DIGIT  = 1 << 3,
  IXHASH_ASCII_GRAPH  = 1 << 4,
  IXHASH_ASCII_LOWER  = 1 << 5,
  IXHASH_ASCII_PRINT  = 1 << 6,
  IXHASH_ASCII_PUNCT  = 1 << 7,
  IXHASH_ASCII_SPACE  = 1 << 8,
  IXHASH_ASCII_UPPER  = 1 << 9,
  IXHASH_ASCII_XDIGIT = 1 << 10
} ixhash_ascii_type_t;

//GLIB_VAR const guint16 * const g_ascii_table;
extern unsigned int *const ixhash_ascii_table;

#define ixhash_isalnum(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_ALNUM) != 0)

#define ixhash_isalpha(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_ALPHA) != 0)

#define ixhash_iscntrl(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_CNTRL) != 0)

#define ixhash_isdigit(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_DIGIT) != 0)

#define ixhash_isgraph(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_GRAPH) != 0)

#define ixhash_islower(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_LOWER) != 0)

#define ixhash_isprint(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_PRINT) != 0)

#define ixhash_ispunct(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_PUNCT) != 0)

#define ixhash_isspace(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_SPACE) != 0)

#define ixhash_isupper(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_UPPER) != 0)

#define ixhash_isxdigit(c) \
  ((ixhash_ascii_table[(unsigned char) (c)] & IXHASH_ASCII_XDIGIT) != 0)

#endif

