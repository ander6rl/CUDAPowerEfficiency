/**
 * main-cuda.cpp: This file is part of the mixbench GPU micro-benchmark suite.
 *
 * Contact: Elias Konstantinidis <ekondis@gmail.com>
 * Edited by: Rebecca Anderson
 **/

#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <string.h>
#include "lcutil.h"
#include "mix_kernels_cuda.h"
#include "version_info.h"

#define VECTOR_SIZE (32 * 1024 * 1024)

int main(int argc, char* argv[]) {
    printf("mixbench (%s)\n", VERSION_INFO);

    int blockSize = 256; // default block size
    bool runWarmup = false;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            blockSize = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-w") == 0) {
            runWarmup = true;
        }
    }

    unsigned int datasize = VECTOR_SIZE * sizeof(double);

    cudaSetDevice(0);
    StoreDeviceInfo(stdout);

    size_t freeCUDAMem, totalCUDAMem;
    cudaMemGetInfo(&freeCUDAMem, &totalCUDAMem);
    printf("Total GPU memory %lu, free %lu\n", totalCUDAMem, freeCUDAMem);
    printf("Buffer size:          %dMB\n", datasize / (1024 * 1024));
    printf("Block size:           %d\n", blockSize);
    printf("Running warmup:       %s\n", runWarmup ? "yes" : "no");

    double* c;
    c = (double*)malloc(datasize);

    mixbenchGPU(c, VECTOR_SIZE, blockSize, runWarmup);

    free(c);

    return 0;
}
