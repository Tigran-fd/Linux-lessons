#include <stdlib.h>
#include <string.h>

#include "lock.h"

int main(int argc, char** argv)
{
    // Getting synchonization header.
    sync_header_t* header = get_shared_mutex();
    if (header == NULL)
        return 1;
           
    // Entering critical section.
    enter_critical_section(header);

    // Doing the clean up if it is requested.
    cleanup(header, argc == 2 && strcmp(argv[1], "-c") == 0);

    return 0;
}
