#!/bin/bash


function exit_handler() {
    echo "Killing server"
    kill $server_pid

    echo "Killing loadtest"
    kill $loadtest_id

    # delete everything execept file in files.txt
    echo "Deleting runtime files"
    grep -w -v -E -f .dont-delete <(ls -1) | xargs rm -rf

    exit
}

trap exit_handler SIGINT

threadPoolSize=10
if [ $# -eq 9 ]
then
    echo "Using command line arguments"
    startClients=$1
    incrementClients=$2
    finalClients=$3
    serverIP=$4
    serverPort=$5
    c_file=$6
    loopNum=$7
    sleepTime=$8
    timout=$9
elif [ -f .env ]
then
    if [ $# -lt 2 ]
    then
        echo "Using .env file but <serverIP> and <serverPort> not provided"
        echo "with .env usage $0 <serverIP> <serverPort> <optional:threadpoolsize>"
        exit 1
    fi
    serverIP=$1
    serverPort=$2
    if [ $# -eq 3 ]
    then
        threadPoolSize=$3
    fi
    source .env
    echo "Using .env file"
else
    echo "Usage: $0 <startClients> <incrementClients> <finalClients> <serverIP> <serverPort> <c_submission_file> <loopNum> <sleepTime> <timeout>"
    echo "OR"
    echo "Please create a .env file with the following variables:"
    echo "startClients=1"
    echo "incrementClients=1"
    echo "finalClients=1"
    echo "serverIP=localhost"
    echo "serverPort=8080"
    echo "c_file=client.c"
    echo "loopNum=1"
    echo "sleepTime=1"
    echo "timout=2"

    exit 1
fi

# start server
# ./server $serverPort $threadPoolSize > server.logs 2> server_err.logs &
# server_pid="$!"

runtime_dir="runtime"
mkdir -p $runtime_dir
mkdir -p server_runtime/status/ server_runtime/submissions/ server_runtime/compiled/ server_runtime/outputs/ server_runtime/diff/
touch server_runtime/server_logs.txt
echo -n "1 2 3 4 5 6 7 8 9 10 "> expected_output.txt


result_file="rounds_result.txt"
echo "num_clients,throughput,throughput_loop,responsetime,success_rate,start_time,end_time" > $result_file

for ((i=$startClients; i<=$finalClients; i+=$incrementClients))
do
    echo "Running loadtest round with $i clients"
    starttime=$(date +%s)
    output=$(./loadtest.sh $i $serverIP $serverPort $c_file $loopNum $sleepTime $timout)
    endtime=$(date +%s)
    loadtest_id="$!"
    throughput=$(echo "$output" | grep "Overall Throughput:" | awk '{print $NF}')
    throughputLoop=$(echo "$output" | grep "Overall Throughput Looptime:" | awk '{print $NF}')
    responsetime=$(echo "$output" | grep "Average Response Time:" | awk '{print $NF}')
    success_rate=$(echo "$output" | grep "Average Success Rate:" | awk '{print $NF}')
    echo "$i,$throughput,$throughputLoop,$responsetime,$success_rate,$starttime,$endtime" >> $result_file
done

if [ -f "server_runtime/server_logs.txt" ]
then
    mv server_runtime/server_logs.txt saveditems/server_logs.txt
fi

# load plots
echo "Generating load plots"
# python3 load_plot.py


exit_handler
