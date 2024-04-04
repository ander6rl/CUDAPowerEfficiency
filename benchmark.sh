#!/bin/bash

sampling_rate_ms=15

delay=$(echo "scale=3; $sampling_rate_ms / 1000" | bc)

log_file="gpu_sensor_log.txt"

echo "Timestamp (s), Power (W), Performance State" > "$log_file"
max_block_size=$((1024*1024*64))
block=1
while true; do
    echo "srun --gres=gpu ./mixbench-cuda -b $block"
    gpu_info=$(nvidia-smi --query-gpu=timestamp,power.draw,pstate --format=csv,noheader,nounits)


    timestamp=$(echo "$gpu_info" | cut -d ',' -f 1)
    power=$(echo "$gpu_info" | cut -d ',' -f 2)
    pstate=$(echo "$gpu_info" | cut -d ',' -f 3)

    echo "$timestamp, $power, $pstate" >> "$log_file"
    block=$(($block*2))
    if [[$block > $max_block_size]]
    then
        break
    sleep $delay
done
