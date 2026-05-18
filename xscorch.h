/* xscorch.h - generated for macOS GTK3 build */
#ifndef __xscorch_h_included
#define __xscorch_h_included

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <libj/libj.h>

#include <sys/time.h>
#include <time.h>

#define TIME_WITH_SYS_TIME   1
#define HAVE_SYS_TIME_H      1
#define HAVE_GETTIMEOFDAY    1

/*** Profile SUBSYSTEM ***/
#define  SC_PROFILE_BEGIN(desc)     {  struct timeval profile_timer1, profile_timer2; \
                                       long profile_sec, profile_usec; \
                                       const char *profile_desc = (desc); \
                                       gettimeofday(&profile_timer1, NULL);
#define  SC_PROFILE_END                gettimeofday(&profile_timer2, NULL); \
                                       profile_usec = profile_timer2.tv_usec - profile_timer1.tv_usec; \
                                       profile_sec  = profile_timer2.tv_sec  - profile_timer1.tv_sec;  \
                                       while(profile_usec < 0) profile_usec += 1000000, profile_sec--; \
                                       printf("%s: %ld.%06ld\n", profile_desc, profile_sec, profile_usec); }

/* Debugging constants */
#define __debugging_macros    0
#if __debugging_macros
#define  SC_DEBUG_PRINTF(args...)      { printf(args); fflush(stdout); }
#define  SC_DEBUG_HEADING()            SC_DEBUG_PRINTF("%s(%d): %s:  ", __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define  SC_DEBUG_MSG(fmt, args...)    { SC_DEBUG_HEADING(); SC_DEBUG_PRINTF(fmt "\n", args); }
#define  SC_DEBUG_ENTER(fmt, args...)  { SC_DEBUG_HEADING(); SC_DEBUG_PRINTF("enter " fmt "\n", args); }
#define  SC_DEBUG_ENTER_()             SC_DEBUG_ENTER("%s", "")
#define  SC_DEBUG_EXIT(fmt, args...)   { SC_DEBUG_HEADING(); SC_DEBUG_PRINTF("exit " fmt "\n", args); }
#define  SC_DEBUG_EXIT_()              SC_DEBUG_EXIT("%s", "")
#endif /* __debugging_macros */

#ifndef M_PI
   #define  M_PI                 3.14159265358979323846
#endif
#ifndef M_SQRT2
   #define  M_SQRT2              1.41421356237309504880
#endif

#ifndef SQR
   #define  SQR(a)               ((a) * (a))
#endif
#ifndef DIST
   #define  DIST(a,b)            (sqrt(SQR(a) + SQR(b)))
#endif
#ifndef SGN
   #define  SGN(a)               ((a) < 0 ? -1 : 1)
#endif
#ifndef INT_ROUND_DIV
   #define  INT_ROUND_DIV(a,b)   ((a)/(b) + (((a)%(b)) << 1) / (b))
#endif

/* Feature flags */
#define  USE_SOUND               0
#define  USE_NETWORK             0
#define  USE_READLINE            0
#define  GTK_ENABLED             1
#define  GTK12_ENABLED           0
#define  GTK20_ENABLED           1
#define  GNOME_ENABLED           0

/* Filenames, paths */
#define  SC_FILENAME_LENGTH      0x1000
#define  SC_FONT_LENGTH          0x1000
#define  SC_GLOBAL_DIR           SC_INSTALL_DATADIR "/xscorch"
#define  SC_LOCAL_CONFIG_DIR     ".xscorch"
#define  SC_LOCAL_CONFIGURATION  "config"
#define  SC_SOUND_DIR            "sounds"
#define  SC_IMAGE_DIR            "images"
#define  SC_ACCESSORY_FILE       "accessories.def"
#define  SC_TANK_PROFILE_FILE    "profiles.def"
#define  SC_SCORING_FILE         "scorings.def"
#define  SC_WEAPON_FILE          "weapons.def"

/* Player and round limits */
#define  SC_MAX_PLAYERS          10
#define  SC_MAX_ROUNDS           1000

/* General field definitions */
#define  SC_DEF_FIELD_WIDTH      800
#define  SC_DEF_FIELD_HEIGHT     600
#define  SC_DEF_MAX_HEIGHT       550

#define  SC_MIN_FIELD_WIDTH      320
#define  SC_MIN_FIELD_HEIGHT     240
#define  SC_MAX_FIELD_WIDTH      1024
#define  SC_MAX_FIELD_HEIGHT     768

/* Miscellaneous */
#define  SC_SLEEP_TIME           5
#define  SC_TIME_ETERNITY        10

#define  SC_DATAROOTDIR          SC_INSTALL_DATADIR


#endif /* __xscorch_h_included */
