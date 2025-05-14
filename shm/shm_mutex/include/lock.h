#ifndef __LOCK_H__
#define __LOCK_H__

#include <stdbool.h>
#include <pthread.h>

// This stucture essentially describes the
// synchronization header which includes
// the mutex, mutex attribute and a boolean flag
// indicating whether the mutex has already been initialized
typedef struct header {
    pthread_mutex_t     lock_;
    pthread_mutexattr_t lock_attr_;
    bool                initialized_;
} sync_header_t;

// Gets the process-shared mutex header.
sync_header_t* get_shared_mutex();

// Enters the critical section.
void enter_critical_section(sync_header_t* header);

// Does the clean-up of all kernel resources
// which we have previously allocated or initialized.
void cleanup(sync_header_t* header, bool destroy);

#endif