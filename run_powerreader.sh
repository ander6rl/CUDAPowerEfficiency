#!/bin/bash

# This bash script runs the mixbench-cuda benchmark for different GPU sizes and 
# logs the power consumption and performance state of the GPU to a file.

# is output directory arg provided?
if [ $# -ne 1 ]; then
    echo "Usage: $0 <output_directory>"
    exit 1
fi

output_dir=$1

mkdir -p "$output_dir"
echo "Output directory: $output_dir"
sleep 1

# loop through specified GPU sizes

for size in 32 64 128 256 512 1024; do
    echo "Running benchmark for GPU size: $size"

    
    # This shell scipt just reads the power level in watts fron nvidia-smi and writes it to a file.

    # 10 milliseconds sampling rate
    sampling_rate_ms=10

    delay=$(echo "scale=3; $sampling_rate_ms / 1000" | bc)

    gpu_dir="$output_dir/$size"
    mkdir -p "$gpu_dir"

    log_file="$gpu_dir/gpu_sensor_log.txt"
    echo "Logging GPU sensor data to: $log_file"
    sleep 0.5

    times_file="$gpu_dir/times.txt"
    echo "Logging benchmark start and end times to: $times_file"
    sleep 0.5

    # Execute benchmark in the background, wait 2 seconds, then start and redirect output to results.txt
    (sleep 5 && echo "--------------------Benchmark starting--------------------" && \ 
    echo $(nvidia-smi --query-gpu=timestamp --format=csv,noheader,nounits) >> "$times_file" && \ 
    build/mixbench-cuda -b $size> results.txt && echo "--------------------Benchmark done!--------------------" && \ 
    echo $(nvidia-smi --query-gpu=timestamp --format=csv,noheader,nounits) >> "$times_file" && sleep 5) &

    # Store the PID of the background process
    bg_pid=$!

    echo "Timestamp (s), Power (W), Performance State, Block Size $size"
    echo "Timestamp (s), Power (W), Performance State, Block Size $size" > "$log_file"

    # Infinite loop to continously monitor GPU
    while true; do

        gpu_info=$(nvidia-smi --query-gpu=timestamp,power.draw,pstate --format=csv,noheader,nounits)

        timestamp=$(echo "$gpu_info" | cut -d ',' -f 1)

        power=$(echo "$gpu_info" | cut -d ',' -f 2)

        pstate=$(echo "$gpu_info" | cut -d ',' -f 3)

        echo "$timestamp, $power, $pstate"

        echo "$timestamp, $power, $pstate" >> "$log_file"

        sleep $delay

        if ! ps -p $bg_pid > /dev/null; then
            echo "Background process for size $size has terminated."
            break
        fi

    done

done
