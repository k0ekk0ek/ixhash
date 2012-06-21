/* system includes */
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

#include "ixhash.h"
#include "ixhash_ascii.h"

#define IXHASH_BUFSZ (1024)

/* prototypes */
static int ixhash_md5hex (char *, size_t, const char *, size_t);

static int ixhash_i (char *, size_t, const char *, size_t);
static int ixhash_ii (char *, size_t, const char *, size_t);
static int ixhash_3rd (char **, const char *, size_t);

static int
ixhash_md5hex (char *hex, size_t hex_len, const char *md5, size_t md5_len)
{
  char chrs[] = "0123456789abcdef";
  int cnt, pos;

  if (hex_len < (md5_len * 2))
    return (ENOBUFS);

  for (cnt = 0, pos = 0; cnt < MD5_DIGEST_LENGTH; cnt++) {
    hex[pos++] = chrs[ (0xf0 & md5[cnt]) >> 4 ];
    hex[pos++] = chrs[ (0x0f & md5[cnt])      ];
  }

  if (pos < hex_len)
    hex[pos] = '\0';

  return (0);
}

static int
ixhash_i (char *hex, size_t hex_len, const char *str, size_t str_len)
{
  char buf[IXHASH_BUFSZ + 1];
  char md5[MD5_DIGEST_LENGTH];
  int nls, pos, spc;
  size_t cnt, cur;

  MD5_CTX md5_ctx;

  assert (hex);
  assert (str);

  if (hex_len < (MD5_DIGEST_LENGTH * 2))
    return (ENOBUFS);

  /* body must contain either twenty or more whitespace characters, or two or
     more newline characters */
  nls = 0;
  spc = 0;

  for (cur = 0; cur < str_len && spc < 20 && nls < 2; cur++) {
    if ((str[cur] == 0x20 || str[cur] == 0x09) ||
        (str[cur] == 0x0a || str[cur] == 0x0d && (++nls)))
      spc++;
  }

  if (spc < 20 && nls < 2)
    return (EINVAL);

  MD5_Init (&md5_ctx);

  /* remove carriage return character if followed by newline character */
  /* remove repeating whitespace characters */
  /* remove visible characters */
  for (cur = 0, pos = -1, cnt = 0; ; cur++) {
    if (cur == str_len) {
      MD5_Update (&md5_ctx, buf, pos + 1);
      MD5_Final (md5, &md5_ctx);
      break;
    }

    if (! ixhash_isgraph (str[cur]) || ixhash_isspace (str[cur])) {
      /* carriage return characters should be removed if followed by a
         newline character, but if there's a sequence of carriage returns
         other rules apply */
      if (str[cur] == 0x12 && pos >= 0 && buf[pos] == 0x14 && cnt == 1) {
        pos--;
      }

      if (ixhash_isspace (str[cur])) {
        if (pos < 0 || ! cnt || buf[pos] != str[cur]) {
          buf[++pos] = str[cur];
          cnt  = 1;
        } else {
          cnt += 1;
        }
      } else {
        buf[++pos] = str[cur];
      }

      if (pos == IXHASH_BUFSZ) {
        MD5_Update (&md5_ctx, buf, pos);
        buf[0] = buf[pos];
        pos = 0;
      }
    } else if (cnt) {
      cnt = 0;
    }
  }

  ixhash_md5hex (hex, hex_len, md5, MD5_DIGEST_LENGTH);

  return (0);
}

#define ixhash_ii_isforget(c) \
  (ixhash_iscntrl (c) ||      \
   ixhash_isalnum (c) ||      \
   /* # */  c == 0x23 ||      \
   /* % */  c == 0x25 ||      \
   /* & */  c == 0x26 ||      \
   /* ; */  c == 0x3b ||      \
   /* = */  c == 0x3d)

static int
ixhash_ii (char *hex, size_t hex_len, const char *str, size_t str_len)
{
  char buf[IXHASH_BUFSZ];
  char md5[MD5_DIGEST_LENGTH];
  int cnt, pos;
  size_t cur, first;

  MD5_CTX md5_ctx;

  assert (hex);
  assert (str);

  if (hex_len < (MD5_DIGEST_LENGTH * 2))
    return (ENOBUFS);

  /* body must contain at least three of the following characters
     "<>()|@*'!?," or a combination of ":/" */
  for (cur = 0, first = 0, cnt = 0; cur < str_len && cnt < 3; cur++) {
    switch (str[cur]) {
      case 0x21: /* ! */
      case 0x27: /* ' */
      case 0x28: /* ( */
      case 0x29: /* ) */
      case 0x2a: /* * */
      case 0x2c: /* , */
      case 0x3c: /* < */
      case 0x3e: /* > */
      case 0x3f: /* ? */
      case 0x40: /* @ */
      case 0x7c: /* | */
        cnt++;
        break;
      case 0x2f: /* / */
        if (str[first] == 0x3a) {
          cnt++;
          first = cur;
        }
        break;
      case 0x3a: /* : */
        first = cur;
        break;
      default:
        break;
    }
  }

  if (cnt < 3)
    return (EINVAL);

  MD5_Init (&md5_ctx);

  /* remove any control characters */
  /* remove any alpha numeric character */
  /* remove any of %, &, #, ;, and = */
  /* convert _ to . */
  /* remove repeating printable characters */
  for (cur = 0, pos = -1; ; cur++) {
    if (cur == str_len) {
      MD5_Update (&md5_ctx, buf, pos + 1);
      MD5_Final (md5, &md5_ctx);
      break;
    }

    if (! ixhash_ii_isforget (str[cur])) {
      if (str[cur] == 0x5f) {
        if (pos == 0 || buf[pos] != 0x2e)
          buf[++pos] = 0x2e;
      } else if (! ixhash_isprint (str[cur]) || (pos < 0 || buf[pos] != str[cur])) {
        buf[++pos] = str[cur];
      }

      if (pos == IXHASH_BUFSZ) {
        MD5_Update (&md5_ctx, buf, pos);
        buf[0] = buf[pos];
        pos = 0;
      }
    }
  }

  ixhash_md5hex (hex, hex_len, md5, MD5_DIGEST_LENGTH);

  return (0);
}


static int
ixhash_3rd (char **digest, const char *str, size_t len)
{
  char buf[IXHASH_BUFSZ];
  char *md5;
  int cnt, err;
  size_t cur, first, pos;

  MD5_CTX md5_ctx;

  assert (digest);
  assert (str);

  err = 0;

  /* body must contain eight or more non-whitespace characters and 1st and 2nd
     hash haven't been computed */
  for (cur = 0, cnt = 0; cur < len && cnt < 8; cur++) {
    if (ixhash_isspace (str[cur]))
      cnt = 0;
    else
      cnt++;
  }

  if (cnt >= 8) {
    /* remove any control characters */
    /* remove any whitespace characters */
    /* remove equal signs */
    /* remove repeating characters */

    MD5_Init (&md5_ctx);

    if (! (md5 = calloc (MD5_DIGEST_LENGTH + 1, sizeof (char)))) {
      err = errno;
      goto error;
    }

    for (cur = 0, first = 0, pos = 0; cur < len; cur++) {
      //if (str[cur] == 0x3d) { printf ("0x3d: %c\n", str[cur]); }
      if (ixhash_isspace (str[cur]) ||
          ixhash_iscntrl (str[cur]) ||
          str[cur] == 0x3d) // =
      {
        if (first != cur && str[first] != 0x3d && ixhash_isgraph (str[first])) {
          if (pos == 0 || buf[(pos-1)] != str[first])
            buf[pos++] = str[first];
          first = cur;
          goto update;
        }
      } else if (ixhash_isgraph (str[cur])) {
        if (str[first] != str[cur]) {
          if (str[first] != 0x3d && ixhash_isgraph (str[first])) {
            if (pos == 0 || buf[(pos-1)] != str[first])
              buf[pos++] = str[first];
            first = cur;
            goto update;
          }
          first = cur;
        }
      } else {
        if (first != cur && str[first] != 0x3d && ixhash_isgraph (str[first])) {
          if (pos == 0 || buf[(pos-1)] != str[first])
            buf[pos++] = str[first];
          first = cur;
        }
        if (pos == 0 || buf[(pos-1)] != str[cur])
          buf[pos++] = str[cur];
update:
        if (pos >= IXHASH_BUFSZ) {
finish:
          MD5_Update (&md5_ctx, buf, pos);
          pos = 0;
        }
      }
    }

    if (pos > 0)
      goto finish;

    MD5_Final (md5, &md5_ctx);
    *digest = md5;

  } else {
    err = EINVAL;
  }

error:
  return (err);
}

#undef IXHASH_BUFSZ

int
ixhash_check (ixhash_result_t *res, const char *str, size_t len)
{
  /* currently this function's sole purpose is to verify the hashing
     algorithms */

  char digest[35], *hex;
  int err, ret;

  /*switch ((err = ixhash_1st (&digest, str, len))) {
    case 0:
      hex = md5ashex (digest);
      printf ("1st digest: %s\n", hex);
      free (digest);
      free (hex);
      break;
    case EINVAL:
      printf ("1st digest: did not match criteria\n");
      break;
    default:
      //printf ("error\n");
      goto error;
  }*/
//size_t i;
//for (i = 0; i < 1000; i++) {
  switch ((err = ixhash_i (digest, 34, str, len))) {
    case 0:
      //hex = md5ashex (digest);
      printf ("\n\n1st digest: %s\n", digest);
      //free (digest);
      //free (hex);
      break;
    case EINVAL:
      //printf ("1st digest: did not match criteria\n");
      break;
    default:
      //printf ("error\n");
      goto error;
  }
//}


  /*switch ((err = ixhash_3rd (&digest, str, len))) {
    case 0:
      hex = md5ashex (digest);
      //printf ("3rd digest: %s\n", hex);
      free (digest);
      free (hex);
      break;
    case EINVAL:
      //printf ("3rd digest: did not match criteria\n");
      break;
    default:
      //printf ("error\n");
      goto error;
  }*/

  *res = ixhash_result_none;
  return (0);
error:
  printf ("error: %s\n", strerror (err));
  return (err);
}







































































