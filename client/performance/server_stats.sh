#!/bin/bash

output_file="server_stats"
mkdir -p "$output_file"
echo "Timestamp,DateTime,CPU_Utilization,NLWP" > "$output_file/stats.log"

while true; do
    datetime=$(date +"%Y-%m-%d %H:%M:%S")
    timestamp=$(date +%s)
    vmstat_output=$(vmstat 1 2 | tail -n 1)
    cpu_utilization=$(echo "$vmstat_output" | awk '{print 100 - $15}')
    nlwp=$(ps -L -C server --no-headers | wc -l)
    printf "%s,%s,%s,%s\n" "$timestamp" "$datetime" "$cpu_utilization" "$nlwp" >> "$output_file/stats.log"
    sleep 1
done
