#!/bin/bash

if [ "$#" -ne 8 ]; then
    echo "Usage: $0 <localhost> <port> <c_file> <numClients> <loopNum> <sleepTimeSeconds> <load_value> <timeout>"
    exit 1
fi

localhost="$1"
port="$2"
c_file="$3"
numClients="$4"
loopNum="$5"
sleepTime="$6"
load_value="$7"
timeout="$8"

output_folder="client_outputs/load_$load_value"
mkdir -p "$output_folder"

for ((i = 1; i <= numClients; i++)); do
    ./client "$localhost" "$port" "$c_file" "$loopNum" "$sleepTime" "$timeout"> "$output_folder/client_$i.txt" &
done

wait

total_Success_response=0
total_Error_Rate=0
total_Request_Sent_Rate=0
total_Throughput=0
total_avg_response_time=0

for file in "$output_folder"/*; do
    if [ -f "$file" ]; then
        Success_response=$(grep 'Successful Request Rate in res/Sec:' "$file" | awk '{print $NF}')
        Error_Rate=$(grep 'Error Rate in res/Sec:' "$file" | awk '{print $NF}')
        Request_Sent_Rate=$(grep 'Request Sent Rate in res/microSec:' "$file" | awk '{print $NF}')
        Throughput=$(grep 'Throughput is:' "$file" | awk '{print $NF}')
        avg_response_time=$(grep 'Average response time in Sec:' "$file" | awk '{print $NF}')

        total_avg_response_time=$(echo "$total_avg_response_time + $avg_response_time" | bc)
        total_Throughput=$(echo "$total_Throughput + $Throughput" | bc)
        total_Success_response=$(echo "$total_Success_response + $Success_response" | bc)
        total_Error_Rate=$(echo "$total_Error_Rate + $Error_Rate" | bc)
        total_Request_Sent_Rate=$(echo "$total_Request_Sent_Rate + $Request_Sent_Rate" | bc)
        echo "Success_response: $Success_response Error_Rate: $Error_Rate Request_Sent_Rate: $Request_Sent_Rate"
    fi
done

    total_avg_response_time_var=$(echo "scale=6; $total_avg_response_time / $total_Success_response" | bc)
    total_Throughput_var=$(echo "scale=6; $total_Throughput" | bc)
    total_Success_response_var=$(echo "scale=6; $total_Success_response" | bc)
    total_Error_Rate_var=$(echo "scale=6; $total_Error_Rate" | bc)
    total_Request_Sent_Rate_var=$(echo "scale=6; $total_Request_Sent_Rate" | bc)


echo "Average Response Time: $total_avg_response_time_var"
echo "Total throughput: $total_Throughput_var"
echo "Total Success Response Rate: $total_Success_response_var"
echo "Total Error Rate: $total_Error_Rate_var"
echo "Total Request Sent Rate: $total_Request_Sent_Rate_var"