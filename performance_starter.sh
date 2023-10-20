#!/bin/bash

if [ "$#" -ne 7 ]; then
    echo "Usage: $0 <initial_clients> <localhost> <port> <c_file> <loopNum> <sleepTimeSeconds> <timeout>"
    exit 1
fi

initial_clients="$1"
localhost="$2"
port="$3"
c_file="$4"
loopNum="$5"
sleepTimeSeconds="$6"
timeout="$7"
analysis_file="analysis.txt"
cpu_file="cpu_utilization.txt"
threads_file="active_threads.txt"

export output_file="util/"
rm -rf "$output_file"

# Clear the analysis file if it already exists or create a new one
echo "Load(M),Average_Resp_Time,Throughput,Successful_response,Error_Rate,Request_Sent_Rate,Average_CPU,Average_Threads" > "$analysis_file"

# Run the performance test for a range of initial clients (e.g., from 1 to 20)
for ((i = 1; i <= 20; i++)); do
    echo "Running iteration $i with $initial_clients clients..."
    
    ./util.sh &

    vmstat_pid=$!



    #vmstat 1 10 > "$cpu_file" &
    #vmstat_pid=$!

    # Start capturing active threads using ps eLf for 10 seconds
    #ps -eLf | grep "./server" > "$threads_file" &
    #threads_pid=$!

    # Run the performance test and capture the output
    output=$(./performance.sh "$localhost" "$port" "$c_file" "$initial_clients" "$loopNum" "$sleepTimeSeconds" "$i" "$timeout")


    kill "$vmstat_pid"
    #kill "$threads_pid"

    # Extract and record the metrics (throughput and average response time)
    Average_Response_Time=$(echo "$output" | grep 'Average Response Time:' | awk '{print $NF}')
    Throughput_Rate=$(echo "$output" | grep 'Total throughput:' | awk '{print $NF}')
    Successful_Request_Rate=$(echo "$output" | grep 'Total Success Response Rate:' | awk '{print $NF}')
    Error_Rate=$(echo "$output" | grep 'Total Error Rate:' | awk '{print $NF}')
    Request_Sent_Rate=$(echo "$output" | grep 'Total Request Sent Rate:' | awk '{print $NF}')


    #Average_CPU=$(awk 'NR > 3 {sum += $13} END {print sum / (NR-3)}' "$cpu_file")
    #Average_Threads=$(awk 'END {print NR-1}' "$threads_file") 
    cpu_average=$(awk -F', ' '{sum+=$2} END {print sum/(NR)}' "util/util.log")
    nlwp_average=$(awk -F', ' '{sum+=$3} END {print sum/(NR)}' "util/util.log")

    # Append the metrics to the analysis file
    #echo "$Average_Response_Time,$Throughput_Rate,$Successful_Request_Rate,$Error_Rate,$Request_Sent_Rate,$Average_CPU,$Average_Threads" >> "$analysis_file"
    echo "$initial_clients,$Average_Response_Time,$Throughput_Rate,$Successful_Request_Rate,$Error_Rate,$Request_Sent_Rate,$cpu_average,$nlwp_average" >> "$analysis_file"
    # Increment the number of initial clients for the next iteration (you can adjust this increment as needed)
    initial_clients=$((initial_clients + 2))
done

echo "Analysis complete. Results saved in $analysis_file"

