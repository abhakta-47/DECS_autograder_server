#!/bin/bash

# always run in projectroot

# get arg1
if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    echo "Usage: ./runtime_manager.sh [create|clear|clean]"
    echo "create: create runtime folders"
    echo "clear: remove runtime items but keep folders and executables"
    echo "clean: purge runtime items and executables and objects"
    exit 1
fi

# bash check if arg1 is "create"

if [ "$1" == "create" ]
  then
    mkdir -p build/
    cd build/
    mkdir -p ouput/
    mkdir -p server_runtime/status/ server_runtime/submissions/ server_runtime/compiled/ server_runtime/outputs/ server_runtime/diff/
    echo -n "1 2 3 4 5 6 7 8 9 10 "> expected_output.txt
    cd ..
    make
    exit 0
fi

if [ "$1" == "clear" ]
  then
    cd build/
    rm -rf server_runtime/status/** server_runtime/submissions/** server_runtime/compiled/** server_runtime/outputs/** server_runtime/diff/**
    rm server_runtime/server_logs.txt
    exit 0
fi

if [ "$1" == "clean" ]
  then
    rm -rf build/
    exit 0
fi