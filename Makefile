# Directories
ROOT_DIR := .

BUILD_DIR := build
COMMON_DIR := common
LOGGER_DIR := logger
CLIENT_DIR := client
SERVER_DIR := server
THREAD_POOL_DIR := thread_pool

# Flags
CFLAGS := -I$(ROOT_DIR) -g

# Object files
OBJS = $(BUILD_DIR)/server.o $(BUILD_DIR)/myqueue.o $(BUILD_DIR)/common.o \
       $(BUILD_DIR)/gen_uuid.o $(BUILD_DIR)/thread_pool.o \
       $(BUILD_DIR)/req_handler_threads.o $(BUILD_DIR)/worker_threads.o \
       $(BUILD_DIR)/logger.o

# Targets
all: server client

$(BUILD_DIR)/%.o: $(COMMON_DIR)/%.c
	gcc -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(THREAD_POOL_DIR)/%.c
	gcc -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SERVER_DIR)/%.c
	gcc -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(LOGGER_DIR)/%.c
	gcc -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/server: $(OBJS) constants.h
	gcc $(CFLAGS) $(OBJS) -o $@ -luuid

$(BUILD_DIR)/client: $(BUILD_DIR)/common.o $(CLIENT_DIR)/client.c constants.h
	gcc $(CFLAGS) $(BUILD_DIR)/common.o -o $@ $(CLIENT_DIR)/client.c

server: $(BUILD_DIR)/server

client: $(BUILD_DIR)/client

clean:
	rm -f $(OBJS) $(BUILD_DIR)/server $(BUILD_DIR)/client
	rm -f analysis.txt diff_output.txt error_output.txt out_gen.txt server_to_client_response_file.txt c_code_server.c diff_out.txt
	rm -rf $(CLIENT_DIR)/client_outputs $(CLIENT_DIR)/util
	pkill server
	pkill util.sh
