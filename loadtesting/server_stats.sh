#!/bin/bash

# echo "Timestamp, CPU_Utilization, NLWP" >> "$output_file/util.log"

while true; do
    timestamp=$(date +%s)
    vmstat_output=$(vmstat 1 2 | tail -n 1)
    cpu_utilization=$(echo "$vmstat_output" | awk '{print 100 - $15}')
    nlwp=$(ps -L -C server --no-headers | wc -l)
    printf "%s, %s, %s\n" "$timestamp" "$cpu_utilization" "$nlwp" >> "cpu_util.log"
    sleep 1
done
