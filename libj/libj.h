/* libj.h - generated for macOS 64-bit build */
#ifndef __libj_included
#define __libj_included

#include <stdlib.h>

#define  LIBJ_STANDALONE      0
#define  LIBJ_C99_STANDARD    1
#define  LIBJ_LIBC_STRING     1

#define  TIME_WITH_SYS_TIME   1
#define  HAVE_SYS_TIME_H      1
#define  HAVE_GETTIMEOFDAY    1

/* Miscellaneous constants */
#define  LIBJ_RK_PRIME     32749
#define  LIBJ_READBUF      0x4000

/* Data types for 64-bit macOS (LP64) */
#ifndef LIBJ_TYPES
#define  LIBJ_TYPES              1
typedef  unsigned char           ubyte;
typedef  unsigned short          uword;
typedef  unsigned int            udword;
typedef  unsigned long long      uqword;

typedef  signed   char           sbyte;
typedef  signed   short          sword;
typedef  signed   int            sdword;
typedef  signed   long long      sqword;

typedef  unsigned char           byte;
typedef  unsigned short          word;
typedef  unsigned int            dword;
typedef  unsigned long long      qword;
typedef  unsigned int            sizea;
typedef  signed   int            indexa;
typedef  unsigned short          wchar;
typedef  signed   int            dimensiona;

#define  DWORD_MAX               0xffffffff
#define  WORD_MAX                0xffff
#define  BYTE_MAX                0xff
#endif /* LIBJ_TYPES */

#ifndef LIBJ_BOOL
#define  LIBJ_BOOL               1
#define  false                   0
#define  true                    (!false)
#ifndef __cplusplus
typedef  ubyte                   bool;
#endif
#endif /* LIBJ_BOOL */

#define  clamp(x, a, b)          ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define  min(x, a)               ((x) < (a) ? (x) : (a))
#define  max(x, a)               ((x) > (a) ? (x) : (a))

typedef  void (*destructorfn)(void *data);
typedef  bool (*comparefn)(const void *a, const void *b);
typedef  bool (*iteratorfn)(void *a, void *params);

#ifndef __libj_unused
#if __GNUC__ && LIBJ_C99_STANDARD
#define  __libj_unused                  __attribute__((unused))
#else
#define  __libj_unused
#endif
#endif

#endif /* __libj_included */
