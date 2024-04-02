#!/bin/bash

sampling_rate_ms=15

delay=$(echo "scale=3; $sampling_rate_ms / 1000" | bc)

log_file="gpu_sensor_log.txt"

echo "Timestamp (s), Power (W), Performance State" > "$log_file"

while true; do

    gpu_info=$(nvidia-smi --query-gpu=timestamp,power.draw,pstate --format=csv,noheader,nounits)


    timestamp=$(echo "$gpu_info" | cut -d ',' -f 1)
    power=$(echo "$gpu_info" | cut -d ',' -f 2)
    pstate=$(echo "$gpu_info" | cut -d ',' -f 3)

    echo "$timestamp, $power, $pstate" >> "$log_file"
    sleep $delay
done
