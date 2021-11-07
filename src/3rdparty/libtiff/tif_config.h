// ----------------------------------------------------------------------------
// Custom configuration file for PCL builds
// ----------------------------------------------------------------------------

#ifndef __TIFF_CONFIG_H
#define __TIFF_CONFIG_H

#include "tiffconf.h"

/* Support C++ stream API (requires C++ compiler) */
#define CXX_SUPPORT 1

/* Define to 1 if you have the <assert.h> header file. */
#define HAVE_ASSERT_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Name of package */
#define PACKAGE "tiff"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "tiff@lists.maptools.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "LibTIFF Software"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "LibTIFF Software 4.3.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "tiff"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.3.0"

/* The size of `size_t', as computed by sizeof. */
#define SIZEOF_SIZE_T 8

/* Default size of the strip in bytes (when strip chopping enabled) */
#define STRIP_SIZE_DEFAULT 8192

/* Version number of package */
#define VERSION "4.3.0"

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

#if !defined(__MINGW32__)
#  define TIFF_SIZE_FORMAT "zu"
#endif
#if SIZEOF_SIZE_T == 8
#  define TIFF_SSIZE_FORMAT PRId64
#  if defined(__MINGW32__)
#    define TIFF_SIZE_FORMAT PRIu64
#  endif
#elif SIZEOF_SIZE_T == 4
#  define TIFF_SSIZE_FORMAT PRId32
#  if defined(__MINGW32__)
#    define TIFF_SIZE_FORMAT PRIu32
#  endif
#else
#  error "Unsupported size_t size; please submit a bug report"
#endif

// ----------------------------------------------------------------------------
#ifndef __WIN32__
// ----------------------------------------------------------------------------

/* Define to 1 if you have the declaration of `optarg', and to 0 if you don't.
   */
#define HAVE_DECL_OPTARG 1

/* Define to 1 if fseeko (and presumably ftello) exists and is declared. */
#define HAVE_FSEEKO 1

/* Define to 1 if you have the `getopt' function. */
#define HAVE_GETOPT 1

/* Define to 1 if you have the `mmap' function. */
#define HAVE_MMAP 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

// ----------------------------------------------------------------------------
#endif // !__WIN32__
// ----------------------------------------------------------------------------

#endif   // __TIFF_CONFIG_H
