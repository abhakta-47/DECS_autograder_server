# external deppendency (v4 only)
- installl libuuid `sudo apt install uuid-dev`

# for v1, v2, v3, v4
- make the server & client binaries using `make`
- copy the binaries `client` to `loadtesting/`
- (optional) copy the `server` binary to separate server
    - kindly make sure the folder structure is copied with server binary, server doesnt handle not existing directory
- run `./server <port>`
- update `loadtesting/.env` as required (must update `serverIP` and `serverPort`). all the variables roles are as discussed in class.
- run `./loadtest-rounds.sh` script to start the load test
- raw load test results are saved in `loadtesting/rounds_result.txt`
- plots are saved in `loadtesting/load_metrics.png`

# for plotting (with server)
- copy cpu_util.log from server to local
- `python3 server_plot.py`