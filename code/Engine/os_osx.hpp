#ifndef engine_os_osx_h
#define engine_os_osx_h
#include "engine.hpp"

#define MAX_PATH_LENGTH 300 // ??


// Threads
// https://github.com/itfrombit/osx_handmade_minimal/blob/master/code/osx_handmade.h
#include <dispatch/dispatch.h>

struct WorkQueue;
#define PLATFORM_WORK_QUEUE_CALLBACK(name) void name(WorkQueue *queue, void *data)
typedef PLATFORM_WORK_QUEUE_CALLBACK(WorkQueueCallback);

struct WorkQueueEntry {
    WorkQueueCallback* callback;
    void* data;
};


struct WorkQueue {
    u32 volatile completion_goal;
    u32 volatile completion_count;
    
    u32 volatile next_entry_to_write;
    u32 volatile next_entry_to_read;
    dispatch_semaphore_t semaphore_handle;
    
    WorkQueueEntry entries[256];
    
    bool NeedsOpenGL;
};


struct ThreadStartUp
{
    //int LogicalThreadIndex;
    //CGLContextObj OpenGLContext;
    WorkQueue *queue;
};


void make_queue(WorkQueue* queue, uint32 thread_count, ThreadStartUp* startups);
void add_entry(WorkQueue* queue, WorkQueueCallback* callback, void* data);
bool do_next_work_queue_entry(WorkQueue* queue);
void complete_all_work(WorkQueue *queue);

#endif
