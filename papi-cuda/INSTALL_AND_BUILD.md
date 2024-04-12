# How to install and build PAPI with it's dependencies from source

## Installing PAPI

1. Download the source code from this github: https://github.com/icl-utk-edu/papi

2. Either unzip or use tar -xvzf on the folder, and cd into papi/src

3. Run ./configure --prefix=/nfs/home/YOUR_ACCOUNT_NAME/papi --with-components=cuda

4. Run make

5. Run make install

6. Export your path: export PATH=/nfs/home/YOUR_ACCOUNT_NAME/papi/bin:$PATH

7. Reload your shell; likely using bash, in which case run the command: source ~/.bashrc

8. Run papi_avail and papi_component_avail to ensure PAPI itself is working.

## Installing libpfm

PAPI also requires the library libpfm to interact with certain things.

1. Go to https://perfmon2.sourceforge.net/ and install the newest version available (this was originally written using libpfm-4.13.0)

2. Tar the file; tar -xvzf libpfm-VERSION.tar.gz

3. Cd into libpfm-version.

4. Run make

5. Run make PREFIX=~/libpfm install

6. Export libpfm to your library path, i.e., export LD_LIBRARY_PATH=~/libpfm/lib:$LD_LIBRARY_PATH

7. Re-launch your shell as before.

8. Everything should work now, to make sure go to papi/src/components/cuda/tests and run the shell script with a gpu; srun --gres=gpu ./runtest.sh (make sure runtest is executable first)

You should see a majority of the tests pass; some will fail due to non-sudo priveleges, but you SHOULD NOT get gpu not found errors. 

If you do, try exporting your cuda path: export PATH=/usr/local/cuda/bin:$PATH , relaunch your terminal again and try again.

## Upon Re-opening the cluster

You will need to re-export the PAPI path and libpfm library path everytime you re-launch the cluster prior to using papi again.

1. export PATH=/nfs/home/YOUR_ACCOUNT_NAME/papi/bin:$PATH

2. export LD_LIBRARY_PATH=~/libpfm/lib:$LD_LIBRARY_PATH
2a. export LD_LIBRARY_PATH=/usr/local/cuda/lib64:$LD_LIBRARY_PATH

3. export CPATH=/usr/local/cuda/include:$CPATH

4. export PATH=/usr/local/cuda/bin:$PATH

5. Re-run the runtest shell script, and also re-run papi_component_avail as a slurm job using --gres=gpu to ensure papi works.
