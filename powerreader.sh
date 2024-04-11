#!/bin/bash

#

# This shell scipt just reads the power level in watts fron nvidia-smi and writes it to a file.

# 10 milliseconds sampling rate
sampling_rate_ms=10

delay=$(echo "scale=3; $sampling_rate_ms / 1000" | bc)

log_file="results/$1_gpu_sensor_log.txt"

times_file="results/$1_times.txt"

# Execute benchmark in the background, wait 2 seconds, then start and redirect output to results.txt
(sleep 5 && echo "--------------------Benchmark starting--------------------" && echo $(nvidia-smi --query-gpu=timestamp --format=csv,noheader,nounits) >> "$times_file" && build/mixbench-cuda -b $1> results.txt && echo "--------------------Benchmark done!--------------------" && echo $(nvidia-smi --query-gpu=timestamp --format=csv,noheader,nounits) >> "$times_file" && sleep 5 && kill $$) &


echo "Timestamp (s), Power (W), Performance State, Block Size $1"
echo "Timestamp (s), Power (W), Performance State, Block Size $1" > "$log_file"

# Infinite loop to continously monitor GPU
while true; do

    gpu_info=$(nvidia-smi --query-gpu=timestamp,power.draw,pstate --format=csv,noheader,nounits)

    timestamp=$(echo "$gpu_info" | cut -d ',' -f 1)

    power=$(echo "$gpu_info" | cut -d ',' -f 2)

    pstate=$(echo "$gpu_info" | cut -d ',' -f 3)

    echo "$timestamp, $power, $pstate"

    echo "$timestamp, $power, $pstate" >> "$log_file"

    sleep $delay


done
