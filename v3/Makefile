# Define the build directory
BUILD_DIR := build

# List of object files
OBJS := $(BUILD_DIR)/server.o $(BUILD_DIR)/myqueue.o $(BUILD_DIR)/logger.o

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

# Rule to compile server.c
$(BUILD_DIR)/server.o: server.c | $(BUILD_DIR)
	gcc -c $< -o $@

# Rule to build the server executable
$(BUILD_DIR)/server: $(OBJS)
	gcc  -o $@ $(OBJS)

# Rule to compile client.c
$(BUILD_DIR)/client: client.c
	gcc -o $@ $<

# Rule to clean up
clean:
	rm -rf $(BUILD_DIR)