# Define the build directory
BUILD_DIR := build

# Default target
all: $(BUILD_DIR) $(BUILD_DIR)/server $(BUILD_DIR)/client

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Rule to build the server executable
$(BUILD_DIR)/server: server.c
	gcc  -o $@ $<

# Rule to compile client.c
$(BUILD_DIR)/client: client.c
	gcc -o $@ $<

# Rule to clean up
clean:
	rm -rf $(BUILD_DIR)