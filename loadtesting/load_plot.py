import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
df = pd.read_csv('./rounds_result.txt')
server_df = pd.read_csv('./cpu_util.log', header=None, names=['timestamp', 'cpu_util', 'num_threads'])

# print(df.head())
# print(server_df.head())

# exit(0)

# Extract the columns from the DataFrame
load = df['num_clients']
throughput = df['throughput']
throughput_loop = df['throughput_loop']
average_resp_time = df['responsetime']
success_rate = df['success_rate']


# Define a function to calculate the moving average
def calculate_moving_average(data, window_size):
    if data.size < window_size:
        return data
    return data.rolling(window=window_size).mean()

# Set the window size for the moving average
window_size = 3

# Calculate moving averages
average_resp_time_smooth = calculate_moving_average(average_resp_time, window_size)
throughput_smooth = calculate_moving_average(throughput, window_size)
throughput_loop_smooth = calculate_moving_average(throughput_loop, window_size)
success_rate_smooth = calculate_moving_average(success_rate, window_size)

# Create subplots
fig, axs = plt.subplots(3, 1, figsize=(12, 8))

# Plot Average Response Time vs Load
axs[0].plot(load, average_resp_time_smooth, marker='o', linestyle='--', color='m')
axs[0].set_title('Average Response Time vs Load')
axs[0].set_xlabel('Load (reqs)')
axs[0].set_ylabel('Average Response Time (s)')

# Plot Throughput by loop vs Load
axs[1].plot(load, throughput_loop_smooth, marker='o', linestyle='--', color='c')
axs[1].set_title('Throuput (looptime) vs Load')
axs[1].set_xlabel('Load (reqs)')
axs[1].set_ylabel('Throuput (looptime) (reqs/sec)')

# Plot Success rate vs Load
axs[2].plot(load, success_rate_smooth, marker='o', linestyle='--', color='g')
axs[2].set_title('Success Rate vs Load')
axs[2].set_xlabel('Load (reqs)')
axs[2].set_ylabel('Success Rate (%)')



# Adjust layout and display the plot
plt.tight_layout()
plt.savefig('load_metrics.png')
plt.show()
