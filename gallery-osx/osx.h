#ifndef osx_h
#define osx_h

#import <Cocoa/Cocoa.h>

NSString* applicationSupportDirectory();
NSString* findOrCreateDirectory(NSSearchPathDirectory searchPathDirectory, NSSearchPathDomainMask domainMask, NSString* appendComponent, NSError **errorOut);

#endif /* osx_h */
