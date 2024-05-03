# CUDAPowerEfficiency

This is a research project focusing on CUDA power efficiency using CUDA benchmarks.

## Running the Powerreader

To run the powerreader that records instantaneous power over the execution of the mixbench benchmark for the block sizes that we chose, follow these steps:

1. In your terminal, make the script executable:
    ```bash
    chmod +x run_powermonitor.sh
    ```

2. Run the automatic power reader for each of the block sizes by executing:
    ```bash
    srun --gres=gpu ./run_powermonitor.sh directory_to_put_files
    ```
    Replace `directory_to_put_files` with the desired name of the directory where output files will be stored.

3. After execution, locate the output directory. Inside this directory, you'll find files named according to the block sizes. Each directory contains two files:
   - `gpu_sensor_log.txt`: Records data from `nvidia-smi` during both idle time and execution of mixbench.
   - `times.txt`: Provides the exact starting and ending times of the benchmark execution.

## Contributors

- Rebecca Anderson
- Sarah Pinkstaff
- Ahron Pollak
