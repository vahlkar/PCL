
#ifdef __PCL_MACOSX

#include <AppKit/AppKit.h>

namespace pcl
{

/*
 * Get available storage space on volume, including purgeable space:
 * https://developer.apple.com/documentation/foundation/nsurlresourcekey/checking_volume_storage_capacity
 * https://developer.apple.com/documentation/foundation/nsurl/1408874-getresourcevalue?language=objc
 */

bool GetVolumeAvailableCapacity( const uint16_t* pathUTF16,
                                 size_t pathLength,
                                 uint64_t* argTotalBytes,
                                 uint64_t* argAvailableBytes,
                                 uint64_t* argAvailableBytesForImportantUsage )
{
   NSString* path = [[NSString alloc] initWithCharacters:(const unichar*)pathUTF16 length:(NSUInteger)pathLength];
   NSURL* url = [NSURL fileURLWithPath:path];
   NSError* error = nil;
   bool valid = true;

   if ( argTotalBytes )
   {
      NSNumber* totalSpace = nil;
      if ( [url getResourceValue:&totalSpace forKey:NSURLVolumeTotalCapacityKey error:&error] == YES )
         *argTotalBytes = [totalSpace unsignedLongLongValue];
      else
         valid = false;
   }

   if ( valid && argAvailableBytes )
   {
      NSNumber* availableSpace = nil;
      if ( [url getResourceValue:&availableSpace forKey:NSURLVolumeAvailableCapacityKey error:&error] == YES )
         *argAvailableBytes = [availableSpace unsignedLongLongValue];
      else
         valid = false;
   }

   if ( valid && argAvailableBytesForImportantUsage )
   {
      NSNumber* availableSpaceForImportant = nil;
      if ( [url getResourceValue:&availableSpaceForImportant forKey:NSURLVolumeAvailableCapacityForImportantUsageKey error:&error] == YES )
         *argAvailableBytesForImportantUsage = [availableSpaceForImportant unsignedLongLongValue];
      else
         valid = false;
   }

   if ( valid )
      return true;

   if ( argTotalBytes )
      *argTotalBytes = 0;
   if ( argAvailableBytes )
      *argAvailableBytes = 0;
   if ( argAvailableBytesForImportantUsage )
      *argAvailableBytesForImportantUsage = 0;

   return false;
}

} // pcl

#endif   // __PCL_MACOSX
