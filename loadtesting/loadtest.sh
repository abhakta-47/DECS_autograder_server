#!/bin/bash

function exit_handler(){
    
    echo "Killing clients"
    for i in "${client_ids[@]}"
    do
        kill $i 2> /dev/null
    done

    exit
}

trap exit_handler SIGINT


numClients=$1
serverIP=$2
serverPort=$3
c_file=$4
loopNum=$5
sleepTime=$6
timout=$7

if [ $# -ne 7 ]
then
    echo "Usage: $0 <numClients> <serverIP> <serverPort> <c_submission_file> <loopNum> <sleepTime> <timeout>"
    exit 1
fi

output_dir="$(pwd)/rounds"
mkdir -p $output_dir
client_ids=()

# Start clients in the background
for ((i=1; i<=$numClients; i++))
do
    ./client $serverIP $serverPort $c_file $loopNum $sleepTime $timout > "$output_dir/output_$i.txt" &
    client_ids+=("$!")
done

# Wait for all clients to finish
wait

# Calculate overall throughput and average response time
totalThroughput=0
totalThroughputLoop=0
totalResponseTime=0
totalSuccessRate=0

for ((i=1; i<=$numClients; i++))
do
    # Read output file
    output=$(cat "$output_dir/output_$i.txt")

    # Extract throughput and response time
    throughput=$(echo "$output" | grep "Throughput:" | awk '{print $NF}')
    responseTime=$(echo "$output" | grep "Average Response Time:" | awk '{print $NF}')
    successRate=$(echo "$output" | grep "Success Rate:" | awk '{print $NF}')
    throughputLoop=$(echo "$output" | grep "Throughput looptime:" | awk '{print $NF}')

    # Calculate total throughput and response time
    totalThroughput=$(echo "$totalThroughput + $throughput" | bc)
    totalThroughputLoop=$(echo "$totalThroughputLoop + $throughputLoop" | bc)
    totalResponseTime=$(echo "$totalResponseTime + ($responseTime * $loopNum)" | bc)
    totalSuccessRate=$(echo "$totalSuccessRate + $successRate" | bc)
done

# Calculate average response time
averageResponseTime=$(echo "scale=2; $totalResponseTime / ($loopNum * $numClients)" | bc)
averageSuccessRate=$(echo "scale=2; $totalSuccessRate / $numClients" | bc)

# Print overall throughput and average response time
echo "Overall Throughput: $totalThroughput"
echo "Overall Throughput Looptime: $totalThroughputLoop"
echo "Average Response Time: $averageResponseTime"
echo "Average Success Rate: $averageSuccessRate"


exit_handler