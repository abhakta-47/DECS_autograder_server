import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
df = pd.read_csv('./analysis.txt')

# Extract the columns from the DataFrame
load = df['Load(M)']
average_resp_time = df['Average_Resp_Time']
throughput = df['Throughput']
successful_response = df['Successful_response']
error_rate = df['Error_Rate']
request_sent_rate = df['Request_Sent_Rate']
avg_cpu = df['Average_CPU']

# Create subplots
fig, axs = plt.subplots(3, 2, figsize=(12, 8))

# Plot Average Response Time vs Load
axs[0, 0].plot(load, average_resp_time, marker='o', linestyle='--', color='m')
axs[0, 0].set_title('Average Response Time vs Load')
axs[0, 0].set_xlabel('Load (M)')
axs[0, 0].set_ylabel('Average Response Time')

# Plot Average CPU vs Load
axs[0, 1].plot(load, avg_cpu, marker='o', linestyle='--', color='m')
axs[0, 1].set_title('Average CPU vs Load')
axs[0, 1].set_xlabel('Load (M)')
axs[0, 1].set_ylabel('Average CPU')

# Plot Throughput vs Load
axs[1, 0].plot(load, throughput, marker='o', linestyle='--', color='c')
axs[1, 0].set_title('Throughput vs Load')
axs[1, 0].set_xlabel('Load (M)')
axs[1, 0].set_ylabel('Throughput')

# Plot Successful Response vs Load
axs[1, 1].plot(load, successful_response, marker='o', linestyle='--', color='b')
axs[1, 1].set_title('Successful Response vs Load')
axs[1, 1].set_xlabel('Load (M)')
axs[1, 1].set_ylabel('Successful Response')

# Plot Error Rate vs Load
axs[2, 0].plot(load, error_rate, marker='o', linestyle='--', color='r')
axs[2, 0].set_title('Error Rate vs Load')
axs[2, 0].set_xlabel('Load (M)')
axs[2, 0].set_ylabel('Error Rate')

# Plot Request Sent Rate vs Load
axs[2, 1].plot(load, request_sent_rate, marker='o', linestyle='--', color='g')
axs[2, 1].set_title('Request Sent Rate vs Load')
axs[2, 1].set_xlabel('Load (M)')
axs[2, 1].set_ylabel('Request Sent Rate')

# Adjust layout and display the plot
plt.tight_layout()
plt.savefig('server_metrics.png')
plt.show()
