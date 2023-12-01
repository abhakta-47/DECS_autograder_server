# Define the build directory
BUILD_DIR := build

# List of object files
OBJS := $(BUILD_DIR)/myqueue.o $(BUILD_DIR)/logger.o

# Default target
all: $(BUILD_DIR) $(BUILD_DIR)/server $(BUILD_DIR)/client

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/logger.o: logger/logger.c | $(BUILD_DIR)
	gcc -c $< -o $@

# Rule to compile myqueue.c
$(BUILD_DIR)/myqueue.o: myqueue.c | $(BUILD_DIR)
	gcc -c $< -o $@

# Rule to build the server executable
$(BUILD_DIR)/server: server.c $(OBJS)
	gcc  -o $@ $< $(OBJS)

# Rule to compile client.c
$(BUILD_DIR)/client: client.c $(BUILD_DIR)/logger.o | $(BUILD_DIR)
	gcc -o $@ client.c $(BUILD_DIR)/logger.o

# Rule to clean up
clean:
	rm -rf $(BUILD_DIR)