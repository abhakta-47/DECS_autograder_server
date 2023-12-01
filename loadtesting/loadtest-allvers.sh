#!/bin/bash
# This script runs the loadtest for all versions of the app

source .env

server_path="~/Downloads/decsruntime/auto/"
decs_server_port=7070

function build_and_move {
    # Build the app
    cd ../$1
    make
    # Run the app
    cp build/{server,client} ../loadtesting
    # Run the loadtest
    cd ../loadtesting
}

function loadtest {
    # Run the loadtest
    ./loadtest-rounds.sh $ip $decs_server_port
}

function move_server_to_server {
    # Move the server to the server
    sshpass -p $password ssh $username@$ip -p $ssh_port "rm -rf $server_path; mkdir -p $server_path; 
    mkdir -p server_runtime/status/ server_runtime/submissions/ server_runtime/compiled/ server_runtime/outputs/ server_runtime/diff/;
    touch server_runtime/server_logs.txt;
    echo -n \"1 2 3 4 5 6 7 8 9 10 \"> expected_output.txt;"
    sshpass -p $password scp -P $ssh_port server $username@$ip:$server_path
    sshpass -p $password scp -P $ssh_port server_stats.sh $username@$ip:$server_path
}

function retrieve_results {
    # Retrieve the results
    sshpass -p $password scp -P $ssh_port $username@$ip:$server_path/cpu_util.log cpu_util.log
    cp rounds_result.txt results/$1_rounds.txt
    cp cpu_util.log results/$1_cpu_util.log
}

function run_server {
    # Run the server
    decs_server_port=$((decs_server_port+1));
    sshpass -p $password ssh $username@$ip -p $ssh_port "cd $server_path; ./server_stats.sh & ./server $decs_server_port" &
}

function kill_server {
    # Kill the server
    sshpass -p $password ssh $username@$ip -p $ssh_port "pkill server & pkill server_stats.sh"
}

for i in {v1,}; do
    echo $i
    build_and_move $i
    move_server_to_server
    run_server
    sleep 1
    loadtest
    kill_server
    sleep 1
    retrieve_results $i
    python3 server_plot.py
    cp load_metrics.png results/$i_server_plot.png
done