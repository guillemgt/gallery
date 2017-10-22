#import "osx.h"

#include "../code/Engine/engine.hpp"
#include "../code/Engine/os.hpp"
#include "../code/Engine/os_osx.hpp"

// File reading & writing

OsFile openGameFile(const char *name, const char *mode){
    NSString *path = [[[[NSBundle mainBundle] resourcePath] stringByAppendingString:@"/"] stringByAppendingString:[NSString stringWithUTF8String:name]];
    FILE *fp = fopen([path UTF8String], mode);
    return fp;
}
OsFile openUserFile(const char *name, const char *mode){
    NSString *path = [[applicationSupportDirectory() stringByAppendingString:@"/"] stringByAppendingString:[NSString stringWithUTF8String:name]];
    FILE *fp = fopen([path UTF8String], mode);
    return fp;
}

OsFile openGameFile(String name, const char *mode){
    return openGameFile((char *)name.text, mode);
}
OsFile openUserFile(String name, const char *mode){
    return openUserFile((char *)name.text, mode);
}

void getGameFilePath(const char *name, char *dst){
    const char *path = [[[NSBundle mainBundle] resourcePath] UTF8String];
    sprintf(dst, "%s/%s", path, name);
    
}
void getUserFilePath(const char *name, char *dst){
    const char *path = [applicationSupportDirectory() UTF8String];
    sprintf(dst, "%s/%s", path, name);
}

void readFile(void *dest, unsigned int size, unsigned int amount, OsFile fp){
    fread(dest, size, amount, fp);
}
void writeFile(void *dest, unsigned int size, unsigned int amount, OsFile fp){
    fwrite(dest, size, amount, fp);
}
void closeFile(OsFile fp){
    fclose(fp);
}

// Directories
// https://www.cocoawithlove.com/2010/05/finding-or-creating-application-support.html
NSString* findOrCreateDirectory(NSSearchPathDirectory searchPathDirectory, NSSearchPathDomainMask domainMask, NSString* appendComponent, NSError **errorOut){
    // Search for the path
    NSArray* paths = NSSearchPathForDirectoriesInDomains(
                                                         searchPathDirectory,
                                                         domainMask,
                                                         YES);
    if ([paths count] == 0)
    {
        // *** creation and return of error object omitted for space
        return nil;
    }
    
    // Normally only need the first path
    NSString *resolvedPath = [paths objectAtIndex:0];
    
    if (appendComponent)
    {
        resolvedPath = [resolvedPath
                        stringByAppendingPathComponent:appendComponent];
    }
    
    // Create the path if it doesn't exist
    NSError *error;
    BOOL success = [[NSFileManager defaultManager]
                    createDirectoryAtPath:resolvedPath
                    withIntermediateDirectories:YES
                    attributes:nil
                    error:&error];
    if (!success)
    {
        if (errorOut)
        {
            *errorOut = error;
        }
        return nil;
    }
    
    // If we've made it this far, we have a success
    if (errorOut)
    {
        *errorOut = nil;
    }
    return resolvedPath;
}

NSString* applicationSupportDirectory(){
    NSString *executableName =
    [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleExecutable"];
    NSError *error;
    NSString *result = findOrCreateDirectory(NSApplicationSupportDirectory, NSUserDomainMask, executableName, &error);
    if (error)
    {
        NSLog(@"Unable to find or create application support directory:\n%@", error);
    }
    return result;
}

void createUserDirectory(const char *name){
    NSString *path = [[applicationSupportDirectory() stringByAppendingString:@"/"] stringByAppendingString:[NSString stringWithUTF8String:name]];
    NSError *error;
    [[NSFileManager defaultManager]
        createDirectoryAtPath: path
        withIntermediateDirectories:YES
        attributes:nil
     error:&error];
    if (error)
    {
        NSLog(@"Unable to create directory:\n%@", error);
    }
}


//
// Watching for changes in file
//

void monitorFile(const char *cpath, void (*callback)()) {
    NSString *path = [NSString stringWithUTF8String:cpath];
    dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    int fildes = open([path UTF8String], O_EVTONLY);
    
    __block dispatch_source_t source = dispatch_source_create(DISPATCH_SOURCE_TYPE_VNODE, fildes,
                                                              DISPATCH_VNODE_DELETE | DISPATCH_VNODE_WRITE | DISPATCH_VNODE_EXTEND |
                                                              DISPATCH_VNODE_ATTRIB | DISPATCH_VNODE_LINK | DISPATCH_VNODE_RENAME |
                                                              DISPATCH_VNODE_REVOKE, queue);
    dispatch_source_set_event_handler(source, ^{
        unsigned long flags = dispatch_source_get_data(source);
        if(flags & DISPATCH_VNODE_WRITE)
        {
            //dispatch_source_cancel(source);
            sleep(1);
            printf("START CALL\n");
            callback();
            printf("END CALL\n");
            //monitorFile([path UTF8String], callback);
        }else if(flags & DISPATCH_VNODE_DELETE) {
            printf("Deleted %s\n", cpath);
            dispatch_source_cancel(source);
        }
    });
    dispatch_source_set_cancel_handler(source, ^(void) {
        close(fildes);
    });
    dispatch_resume(source);
}

OsTime fileModificationDate(const char *cpath){
    NSString *path = [NSString stringWithUTF8String:cpath];
    
    NSError *error = nil;
    NSDictionary *attrs = [[NSFileManager defaultManager] attributesOfItemAtPath:path error:&error];
    if(attrs && !error){
        return [[attrs fileModificationDate] timeIntervalSince1970];
    }
    return 0;
}