#ifndef IXHASH_H_INCLUDED
#define IXHASH_H_INCLUDED

typedef enum {
  ixhash_result_none = 0, /* not listed */
  ixhash_result_1st, /* first iXhash digest listed */
  ixhash_result_2nd, /* second iXhash digest listed */
  ixhash_result_3rd /* third iXhash digest listed */
} ixhash_result_t;

int ixhash_check (ixhash_result_t *, const char *, size_t);

#endif

