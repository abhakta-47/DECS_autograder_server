#!/bin/bash

output_file="client_outputs/load_$1/cpu_utilization.log"  # Modified line
mkdir -p "$(dirname "$output_file")"
echo "Timestamp, User_CPU, System_CPU, NLWP" > "$output_file"

while true; do
    timestamp=$(date +"%Y-%m-%d %H:%M:%S")
    vmstat_output=$(vmstat 1 10 | tail -n 1)
    user_cpu_utilization=$(echo "$vmstat_output" | awk '{print $13}' | sed 's/us//')
    system_cpu_utilization=$(echo "$vmstat_output" | awk '{print $14}')
    nlwp=$(ps -e -o nlwp,cmd --no-header | grep "./server" | awk '{s+=$1} END {print s}')
    printf "%s,%s,%s,%s\n" "$timestamp" "$user_cpu_utilization" "$system_cpu_utilization" "$nlwp" >> "$output_file"
    #sleep 5
done
