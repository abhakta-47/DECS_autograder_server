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

# Clear the analysis file if it already exists or create a new one
echo "Throughput, Average Response Time, Successful Request Rate, Total Response Time, Timeout Rate, Error Rate, Request Rate" > "$analysis_file"

# Run the performance test for a range of initial clients (e.g., from 1 to 20)
for ((i = 1; i <= 20; i++)); do
    echo "Running iteration $i with $initial_clients clients..."
    

    ./util.sh &
    vmstat_pid=$!



    # Run the performance test and capture the output
    output=$(./performance.sh "$localhost" "$port" "$c_file" "$initial_clients" "$loopNum" "$sleepTimeSeconds" "$i" "$timeout")
    
    kill "$vmstat_pid"

    Throughput=$(echo "$output" | grep 'Throughput :' | awk '{print $NF}')
    Average_Response_Time=$(echo "$output" | grep 'Average Response Time :' | awk '{print $NF}')
    Successful_Request_Rate=$(echo "$output" | grep 'Successful Request Rate :' | awk '{print $NF}')
    Total_Response_Time=$(echo "$output" | grep 'Total Response Time :' | awk '{print $NF}')
    Timeout_Rate=$(echo "$output" | grep 'Timeout Rate :' | awk '{print $NF}')
    Error_Rate=$(echo "$output" | grep 'Error Rate :' | awk '{print $NF}')
    Request_Rate=$(echo "$output" | grep 'Request Rate :' | awk '{print $NF}')



    #Average_CPU_Utilization=$(echo "$output" | grep 'Average CPU Utilization:' | awk '{print $NF}')
    #Average_Threads=$(echo "$output" | grep 'Average Threads:' | awk '{print $NF}')

    echo "$Throughput, $Average_Response_Time, $Successful_Request_Rate,$Total_Response_Time,$Timeout_Rate,  $Error_Rate, $Request_Rate, $Average_CPU_Utilization, $Average_Threads " >> "$analysis_file"
    # Extract and record the metrics (throughput and average response time)
    #overall_throughput=$(echo "$output" | grep 'Overall throughput:' | awk '{print $NF}')
    #average_response_time=$(echo "$output" | grep 'Average response time:' | awk '{print $NF}')
    
    # Append the metrics to the analysis file
    #echo "$initial_clients,$overall_throughput,$average_response_time" >> "$analysis_file"
    
    # Increment the number of initial clients for the next iteration (you can adjust this increment as needed)
    initial_clients=$((initial_clients + 2))
done

echo "Analysis complete. Results saved in $analysis_file"

