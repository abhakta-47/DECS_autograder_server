#!/bin/bash

# Check if version argument is provided
if [ $# -ne 1 ]; then
    echo "Usage: $0 <version>"
    exit 1
fi

function exit_handler() {
    echo "Caught SIGINT. Cleaning up..."
    clean_runtime_folders
    exit 0
}

trap exit_handler SIGINT

# Set the version from the argument
version=$1
root_path="/home/abdev/projects/cs744/DECS_Server"
runtime_dir="$root_path/loadtesting/runtimes/$version $(date +"%F %H:%M:%S")"
loadtest_path="$root_path/loadtesting"
server_pid=0
server_port=8080
server_ip="localhost"
server_num_threads=10

if [[ $1 == "clean" ]]; then
    rm -rf runtimes/*
    exit 0
fi

function build_binaries() {
    # Build the server binary
    echo
    echo "Building server&client binaries..."
    cd "$root_path/$version"
    make all

    # Check if server build was successful
    if [ $? -ne 0 ]; then
        echo "Server build failed."
        exit 1
    fi
    echo "Server&client build successful."
    echo
    cd ..
}

function create_runtime_folders() {
    echo
    echo "Creating runtime folders..."
    mkdir -p "$runtime_dir"
    cd "$runtime_dir"
    mkdir -p server_runtime/status/ server_runtime/submissions/ server_runtime/compiled/ server_runtime/outputs/ server_runtime/diff/
    touch server_runtime/server_logs.txt
    echo -n "1 2 3 4 5 6 7 8 9 10 "> expected_output.txt
    cd ..
}

function move_binaries() {
    # Move the server binary to loadtesting directory
    echo
    echo "Moving server&client binaries to loadtesting directory..."
    cd "$root_path/$version"
    cp build/{server,client} "$runtime_dir"
    cd ..
}

function clean_runtime_folders() {
    echo
    echo "Cleaning up..."
    # cd "$root_path/loadtesting"
    # rm -rf "$runtime_dir"
    kill_server
    cd "$runtime_dir/loadtesting"
    echo "Deleting runtime files"
    grep -w -v -E -f .dont-delete <(ls -1) | xargs rm -rf
}

function set_open_port() {
    isfree=$(netstat -taln | grep $server_port)

    while [[ -n "$isfree" ]]; do
        server_port=$[server_port+1]
        echo trying $server_port
        isfree=$(netstat -taln | grep $server_port)
    done

    echo "Usable server_Port: $server_port"
}

function run_server() {
    # Run the server
    echo
    echo "Running server..."
    cd "$runtime_dir"
    # while ./server return 1 increate portnum+1 and tryt o run in loop
    set_open_port
    ./server $server_port $server_num_threads > ./server.logs 2>./server_err.logs &
    server_pid=$!
    echo "Server running on port $server_port with pid $server_pid"
    cd ..
}

function kill_server() {
    # Kill the server
    echo "Killing server..."
    kill -9 $server_pid
}

function run_client() {
    echo
    echo "Running client..."
    cd "$runtime_dir"
    for file in $(ls $loadtest_path/test_cfiles/*.c); do
        echo "Submitting $file..."
        ./client $server_ip $server_port $file 1 1 10
        echo
    done
    cd ..
}


build_binaries
create_runtime_folders
move_binaries
run_server
run_client
clean_runtime_folders