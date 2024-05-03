#/bin/bash

# papi path
export PATH="$HOME/papi/bin:$PATH"

# libpfm, cuda, papi paths
export LD_LIBRARY_PATH="$HOME/libpfm/lib:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="/usr/local/cuda/lib64:$LD_LIBRARY_PATH"
export LD_LIBRARY_PATH="$HOME/papi/lib:$LD_LIBRARY_PATH"

# cuda include
export CPATH="/usr/local/cuda/include:$CPATH"

# path for cuda
export PATH="/usr/local/cuda/bin:$PATH"

# AI
cd "$(dirname "$0")"

# clean and build makefile
make clean
make