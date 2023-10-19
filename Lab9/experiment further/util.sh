#!/bin/bash

output_file="util/"
mkdir -p "$output_file"
echo "Timestamp, CPU_Utilization, NLWP" > "$output_file/util.log"

while true; do
    timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    vmstat_output=$(vmstat 1 2 | tail -n 1)
    cpu_utilization=$(echo "$vmstat_output" | awk '{print 100 - $15}')
    nlwp=$(ps -C server --no-headers | wc -l)
    printf "%s, %s, %s\n" "$timestamp" "$cpu_utilization" "$nlwp" >> "$output_file/util.log"
    sleep 10
done
