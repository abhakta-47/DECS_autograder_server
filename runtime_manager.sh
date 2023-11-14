#!/bin/bash

# always run in projectroot

# get arg1
if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    echo "Usage: ./runtime_manager.sh [create|clean]"
    echo "create: create runtime folders"
    echo "clean: clean runtime folders"
    exit 1
fi

# bash check if arg1 is "create"

if [ "$1" == "create" ]
  then
    mkdir -p build/
    cd build/
    mkdir -p server_runtime/status/ server_runtime/submissions/ server_runtime/compiled/ server_runtime/outputs/ server_runtime/diff/
    echo -n "1 2 3 4 5 6 7 8 9 10 "> expected_output.txt
    exit 0
fi

if [ "$1" == "clean" ]
  then
    rm -rf build/
    exit 0
fi