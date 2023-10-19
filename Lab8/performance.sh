#!/bin/bash

if [ "$#" -ne 8 ]; then
    echo "Usage: $0 <localhost> <port> <c_file> <numClients> <loopNum> <sleepTimeSeconds> <load_val> <timeout>"
    exit 1
fi

localhost="$1"
port="$2"
c_file="$3"
numClients="$4"
loopNum="$5"
sleepTime="$6"
load_val="$7"
timeout="$8"

output_folder="client_outputs/load_$load_val"
mkdir -p "$output_folder"


#./util.sh > "$output_folder/util.log" &
#vmstat_pid=$!

for ((i = 1; i <= numClients; i++)); do
    ./client "$localhost" "$port" "$c_file" "$loopNum" "$sleepTime" "$timeout" > "$output_folder/client_$i.txt" &
done

wait

#kill "$vmstat_pid"

temp_throughput=0.0
temp_avg_response_time=0.0
temp_successful_request_rate=0.0
temp_total_response_time=0.0
temp_timeout_rate=0.0
temp_error_rate=0.0
temp_request_rate=0.0


#Average_CPU_util=0.0
#Average_Threads=0
#total_lines=0
#for file in "$output_folder"/util.log; do
#    if [ -f "$file" ]; then
#        while IFS=',' read -r Timestamp CPU_Utilization NLWP; do
#            Average_CPU_Util=$(echo "$Average_CPU_Util + $CPU_Utilization" | bc)
#            Average_Threads=$(echo "$Average_Threads + $NLWP" | bc)
#            total_lines=$((total_lines + 1))
#        done < "$file"


#    fi
#done

#Average_CPU_Util=$(echo "scale=2; $Average_CPU_Util / $total_lines" | bc)
#Average_Threads=$((Average_Threads / total_lines))





for file in "$output_folder"/*.txt; do
    if [ -f "$file" ]; then

        throughput=$(grep 'Throughput is :' "$file" | awk '{print $NF}')
        avg_response_time=$(grep 'Average Response Time is :' "$file" | awk '{print $NF}')
        successful_request_rate=$(grep 'Successful Request Rate is :' "$file" | awk '{print $NF}')
        total_response_time=$(grep 'Total Response Time is :' "$file" | awk '{print $NF}')
        timeout_rate=$(grep 'Timeout Rate is :' "$file" | awk '{print $NF}')
        error_rate=$(grep 'Error rate is :' "$file" | awk '{print $NF}')
        request_rate=$(grep 'Request rate is :' "$file" | awk '{print $NF}')


        temp_throughput=$(bc <<< "$temp_throughput + $throughput")
        temp_avg_response_time=$(bc <<< "($temp_avg_response_time + $avg_response_time)/2.0")
        temp_successful_request_rate=$(bc <<< "($temp_successful_request_rate + $successful_request_rate)/2.0")
        temp_total_response_time=$(bc <<< "$temp_total_response_time + $total_response_time")
        temp_timeout_rate=$(bc <<< "($temp_timeout_rate + $timeout_rate)/2.0")
        temp_error_rate=$(bc <<< "($temp_error_rate + $error_rate)/2.0")
        temp_request_rate=$(bc <<< "($temp_request_rate + $request_rate)/2.0")


        #throughput=$(grep 'throughput:' "$file" | awk '{print $NF}')
        #response_time=$(grep 'Average Response Time:' "$file" | awk '{print $NF}')
        #successful_res=$(grep 'successfulRes:' "$file" | awk '{print $NF}')
	#echo "throughput: $throughput response_time: $response_time successful_res: $successful_res"
        #total_throughput=$(bc <<< "$total_throughput + $throughput")
        #total_response_time=$(bc <<< "$total_response_time + ($successful_res * $response_time)")
        #total_successful_res=$(bc <<< "$total_successful_res + $successful_res")
    fi
done


final_throughput=$temp_throughput
final_avg_response_time=$temp_avg_response_time
final_successful_request_rate=$temp_successful_request_rate
final_total_response_time=$temp_total_response_time
final_timeout_rate=$temp_timeout_rate
final_error_rate=$temp_error_rate
final_request_rate=$temp_request_rate

echo "Throughput : $final_throughput"
echo "Average Response Time : $final_avg_response_time"
echo "Successful Request Rate : $final_successful_request_rate"
echo "Total Response Time : $final_total_response_time"
echo "Timeout Rate : $final_timeout_rate"
echo "Error Rate : $final_error_rate"
echo "Request Rate : $final_request_rate"




#echo "Average CPU Utilization: $Average_CPU_Util"
#echo "Average Threads: $Average_Threads"
#overall_throughput="$total_throughput"
#average_response_time=$(bc <<< "scale=6; $total_response_time / $total_successful_res")

#echo "Overall throughput: $overall_throughput"
#echo "Average response time: $average_response_time"
