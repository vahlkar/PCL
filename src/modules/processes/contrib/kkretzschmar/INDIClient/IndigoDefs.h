
#ifndef __IndigoDefs_h
#define __IndigoDefs_h

#include <pcl/Defs.h>

#ifdef __PCL_FREEBSD
# define INDIGO_FREEBSD 1
#endif

#ifdef __PCL_LINUX
# define INDIGO_LINUX 1
#endif

#if defined( __PCL_MACOS ) || defined( __PCL_MACOSX )
# define INDIGO_MACOS 1
#endif

#ifdef __PCL_WINDOWS
# define INDIGO_WINDOWS 1
#endif

#endif   // __IndigoDefs_h
