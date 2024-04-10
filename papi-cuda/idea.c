/*
 * This does NOT compile. However, this is what I think a basic idea
 * for measuring power consumption of the GPU will look like with PAPI.
 *
 * Author: Ahron Pollak
 */

#include "papi.h"
#include <stdio.h>
#include <stdlib.h>

int handle_error(int retval);

/*
 * A general idea of all the PAPI measurement should do. This follows a basic
 * idea of some resources found online (linked in relevant lines). Much is still
 * to be implemented.
 * General structure copied from here:
 * https://www.vi-hps.org/cms/upload/material/tw39/PAPI.pdf (For me), additional
 * research: https://icl.utk.edu/projects/papi/files/documentation/PAPI-C.html
 */
int main() {
    // copied from man page (mostly)
    // https://linux.die.net/man/3/papi_library_init
    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT && retval > 0) {
        fprintf(stderr, "PAPI library doesn't match.\n");
        exit(EXIT_FAILURE);
    }
    if (retval < 0) {
        handle_error(retval);
    }

    retval = PAPI_is_initialized();
    if (retval != PAPI_LOW_LEVEL_INITED) {
        handle_error(retval);
    }

    int gpu_component = PAPI_NULL;
    int i;
    for (i = 0; i < PAPI_num_components(); i++) {
        // papi_get_component_info
        const PAPI_component_info_t *cmpinfo = NULL;
        if (cmpinfo = PAPI_get_component_info(i) == NULL) {
            exit(EXIT_FAILURE);
        }
        if (cmpinfo && strcmp(cmpinfo->name, "cuda") == 0) {
            // found a gpu component
            gpu_component = i;
            break;
        }
    }
    // no nvidia gpu found
    if (gpu_component == PAPI_NULL) {
        printf("A CUDA component was not found.\n");
        exit(EXIT_FAILURE);
    }

    // set of hardware events; we want the native events of the gpu
    int EventSet = PAPI_NULL;
    // alloc space for new eventset
    retval = PAPI_create_eventset(&EventSet);
    // add gpu component to the event set
    retval = PAPI_assign_eventset_component(EventSet, gpu_component);

    // TODO: find the power consumption event name
    int event_code;
    PAPI_event_code_to_name("SOME_POWER_CONSUMPTION_EVENT", &event_code);
    PAPI_add_event(EventSet, event_code);

    // start counters
    retval = PAPI_start(EventSet);

    dowork();

    // stop monitoring, store results in values
    // TODO: implement performance report of measured values
    retval = PAPI_stop(EventSet, values);

    // https://web.eece.maine.edu/~vweaver/projects/papi/papi_simple.html
    // remove events in the eventset
    if (PAPI_cleanup_eventset(EventSet) != PAPI_OK) {
        handle_error(1);
    }
    // free memory and ds; eventset MUST be empty
    if (PAPI_destroy_eventset(&EventSet) != PAPI_OK) {
        handle_error(1);
    }

    // now shutdown papi
    PAPI_shutdown();

    return EXIT_SUCCESS;
}

/*
 * Basic error handling.
 */
int handle_error(int retval) {
    fprintf("An error has occurred with code: %d\n", retval);
    exit(EXIT_FAILURE);
}
