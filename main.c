/* system includes */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* iXhash includes */
#include "ixhash.h"

#define BUFSZ (4096)

int
main (int argc, char *argv[])
{
  FILE *fh;
  char *tmp_str, *str;
  int err;
  size_t tmp_len, len, off, size;
  ssize_t cnt;

  ixhash_result_t res;

  cnt = 0;
  len = 0;
  off = 0;
  str = NULL;

  if (argc != 2) {
    fprintf (stderr, "Usage: %s FILE\n", argv[0]);
    return (1);
  }

  if (! (fh = fopen (argv[1], "rb"))) {
    fprintf (stderr, "fopen: %s\n", strerror (errno));
    return (1);
  }

  /* read content from file handle */
  err = errno;
  errno = 0;
  for (;;) {
    if ((len - off) == 0) {
      /* allocate memory */
      tmp_len = len + BUFSZ;
      size = (tmp_len + 1) * sizeof (char);
      if (! (tmp_str = realloc (str, size))) {
        fprintf (stderr, "realloc: %s\n", strerror (errno));
        return (1);
      }
      /* initialize newly allocated memory */
      size = ((tmp_len - off) + 1) * sizeof (char);
      memset (tmp_str + off, '\0', size);
      /* restore pointers */
      str = tmp_str;
      len = tmp_len;
    }

    cnt = fread (str + off, sizeof (char), len - off, fh);

    if (cnt > 0) {
      off += cnt;
    }

    if ((len - off) > 0) {
      if (errno) {
        if (errno == EINTR) {
          errno = 0;
        } else {
          fprintf (stderr, "fread: %s\n", strerror (errno));
          return (1);
        }
      } else {
        break;
      }
    }
  }
  errno = err;

  //printf ("stdin: %s\n", str);

  (void)ixhash_check (&res, str, off);
free (str);
fclose (fh);
  return (0);
}

