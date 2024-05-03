/*
 * This file is used to measure Power in milliwats using a combination of the PAPI library and Nvidia's NVML library.
 *
 * Author: Ahron Pollak
 */

#define _POSIX_C_SOURCE 200809L 
#include "papi.h"
#include <nvml.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

int handle_error(int retval, const char* message);

/*
 * Measures GPU power draw with NVML and PAPI.
 * https://www.vi-hps.org/cms/upload/material/tw39/PAPI.pdf (For me), additional
 * research: https://icl.utk.edu/projects/papi/files/documentation/PAPI-C.html
 */
int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    // This nvml structure is largely copied from here: https://github.com/erik/cudamon/blob/master/lib/nvml/example/example.c
    // Initialize nvml library
    nvmlReturn_t result;
    unsigned int device_count;
    result = nvmlInit();
    if (result != NVML_SUCCESS) {
        handle_error(result, "Failure initializing NVML");
    }
    
    // query device count
    result = nvmlDeviceGetCount(&device_count);
    if (result != NVML_SUCCESS) {
        handle_error(result, "Failed to query device count");
    }
    // Various device info per device
    nvmlDevice_t device;
    char name[NVML_DEVICE_NAME_BUFFER_SIZE];
    nvmlPciInfo_t pci;
        
    // query for the device handle to perform operations on a device; just query the 0th indexed gpu (the one alloc'd)
    result = nvmlDeviceGetHandleByIndex(0, &device);
    if (result != NVML_SUCCESS) {
        handle_error(result, "Failed to get handle for gpu");
    }
        
    // name of gpu
    result = nvmlDeviceGetName(device, name, NVML_DEVICE_NAME_BUFFER_SIZE);
    if (result != NVML_SUCCESS) {
        handle_error(result, "Failed to get name of device");
    }
    printf("Device name: %s\n", name);
        
    // pci stuff of actual gpu we're working with
    result = nvmlDeviceGetPciInfo(device, &pci);
    if (result != NVML_SUCCESS) {
        handle_error(result, "Failed to get pci info for device");
    }
    printf("Device physically talking to: %d. %s [%s]\n", 0, name, pci.busId);

    // Now initialize the PAPI library
    int retval;
    retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT && retval > 0) {
        handle_error(retval, "PAPI library doesn't match!\n");
        exit(EXIT_FAILURE);
    }
    if (retval < 0) {
        handle_error(retval, "Failed to initialize PAPI");
    }

    // create an event set
    int EventSet = PAPI_NULL;
    if (PAPI_create_eventset(&EventSet) != PAPI_OK) {
        handle_error(1, "Failure creating event set");
    }
    
    // Add NVML event to monitor.
    // https://github.com/NERSC/timemory/issues/181
    long long num_components = PAPI_num_components();
    int nvml_component_index = -1;
    for (long long i = 0; i < num_components; i++) {
        const PAPI_component_info_t * component = PAPI_get_component_info(i);
        if (component && strcmp(component->name, "nvml") == 0) {
            nvml_component_index = i;
            break;
        }
    }
    if (nvml_component_index == -1) {
        fprintf(stderr, "NVML component not found\n");
        return -1;
    }

    //cudaSetDevice(0);


    // AI generated
    // List all events for the NVML component
    int device_index = 0;
    char target_event_name[PAPI_MAX_STR_LEN];
    snprintf(target_event_name, sizeof(target_event_name), "nvml:::NVIDIA_A2:device_%d:power", device_index);

    int code = PAPI_NATIVE_MASK;
    int event_count = 0;
    retval = PAPI_enum_cmp_event(&code, PAPI_ENUM_FIRST, nvml_component_index);
    char event_name[PAPI_MAX_STR_LEN];

    while (retval == PAPI_OK) {
        if (PAPI_event_code_to_name(code, event_name) == PAPI_OK) {
            //printf("Event %d: %s\n", event_count, event_name);
            // Check if this is the event we're interested in
            if (strcmp(event_name, target_event_name) == 0) {
                //printf("Found the power event!\n");
                break;
            }
        }
        retval = PAPI_enum_cmp_event(&code, PAPI_ENUM_EVENTS, nvml_component_index);
        event_count++;
    }
    retval = PAPI_add_named_event(EventSet, event_name);
    if (retval != PAPI_OK) {
        printf("ERROR HERE WITH CODE: %d\n", retval);
        exit(EXIT_FAILURE);
    }
    
    // start monitoring
    retval = PAPI_start(EventSet);

    char log_file[256];
    snprintf(log_file, sizeof(log_file), "gpu_sensor_papi_log.txt");
    FILE *fp = fopen(log_file, "w");
    if (!fp) {
        fprintf(stderr, "Failed to open file\n");
        exit(EXIT_FAILURE);
    }
    //TODO: convert to cuda program or script to check block size as well
    //TODO: run cuda benchmarks here (turn into cuda file?)
    fprintf(fp, "Timestamp (s) Power (W)\n");

    //TODO: CALL THE MIXBENCH FUNCTION HERE TO RUN THE BENCHMARK
    char *end_p;
    long duration = strtol(argv[1], &end_p, 10); // total seconds to monitor gpu
    struct timespec start, end, request;
    int sampling_rate_ms = 5;
    long long values[1]; // number of events to monitor

    request.tv_sec = 0;
    request.tv_nsec = sampling_rate_ms * 1000000L;

    clock_gettime(CLOCK_MONOTONIC, &start); // ai
    long time_elapsed = 0;

    while (time_elapsed < duration) {
        clock_gettime(CLOCK_MONOTONIC, &end); //ai 
        time_elapsed = (end.tv_sec - start.tv_sec);

        // read event
        //TODO: call mixbenchGPU(DATA_SIZE, VECTOR_SIZE);
        retval = PAPI_read(EventSet, values);
        if (retval != PAPI_OK) {
            fclose(fp);
            handle_error(retval, "Failed to read event");
        }

        fprintf(fp, "%ld                     %lld\n", time_elapsed, values[0]);
        fflush(fp);
        nanosleep(&request, NULL);
    }
    fclose(fp);

    // stop monitoring, store results in values
    if (PAPI_stop(EventSet, values) != PAPI_OK) {
        handle_error(1, "Failed stopping event set");
    }

    // https://web.eece.maine.edu/~vweaver/projects/papi/papi_simple.html
    // remove events in the eventset
    if (PAPI_cleanup_eventset(EventSet) != PAPI_OK) {
        handle_error(1, "Failed cleaning up event set");
    }

    // free memory and ds; eventset MUST be empty
    if (PAPI_destroy_eventset(&EventSet) != PAPI_OK) {
        handle_error(1, "Failed to destroy event set");
    }

    result = nvmlShutdown();
    PAPI_shutdown();
    return EXIT_SUCCESS;
}

/*
 * Basic error handle function. Largely AI-generated.
 */
int handle_error(int retval, const char* message) {
    if (retval != NVML_SUCCESS) {
        fprintf(stderr, "NVML error occurred: %s\n", nvmlErrorString(retval));
        exit(EXIT_FAILURE);
    } else {
        fprintf(stderr, "PAPI error occurred: %d: %s\n", retval, PAPI_strerror(retval));
        exit(EXIT_FAILURE);
    }
}
