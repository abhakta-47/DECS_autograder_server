import matplotlib.pyplot as plt

# Read the data from the file
with open('analysis.txt', 'r') as file:
    lines = file.readlines()

# Initialize lists to store the data
successful_response = []
error_rate = []
request_sent_rate = []
average_cpu = []
average_threads = []
throughput = []
average_resp_time = []
load = []

# Extract the data from the lines
for line in lines:
    if line.startswith('Average_Resp_Time') or line.startswith('Time:'):
        continue  # Skip the header lines
    elif line.strip() == '' or line.startswith('Rate:') or line.startswith('throughput:'):
        continue
    data = line.strip().split(',')
    average_resp_time.append(float(data[0]))
    throughput.append(float(data[1]))
    successful_response.append(float(data[2]))
    error_rate.append(float(data[3]))
    request_sent_rate.append(float(data[4]))
    average_cpu.append(float(data[5]))
    average_threads.append(float(data[6]))
    load.append(len(load) * 2 + 2)

# Plotting the graphs
fig, axs = plt.subplots(3, 2, figsize=(12, 15))

axs[0, 0].plot(load, average_resp_time, marker='o', linestyle='--', color='m')
axs[0, 0].set_title('Average Response Time vs Load')
axs[0, 0].set_xlabel('Load (M)')
axs[0, 0].set_ylabel('Average Response Time')

axs[1, 0].plot(load, throughput, marker='o', linestyle='--', color='c')
axs[1, 0].set_title('Throughput vs Load')
axs[1, 0].set_xlabel('Load (M)')
axs[1, 0].set_ylabel('Throughput')

axs[1, 1].plot(load, successful_response, marker='o', linestyle='--', color='b')
axs[1, 1].set_title('Successful Response vs Load')
axs[1, 1].set_xlabel('Load (M)')
axs[1, 1].set_ylabel('Successful Response')

axs[2, 0].plot(load, error_rate, marker='o', linestyle='--', color='r')
axs[2, 0].set_title('Error Rate vs Load')
axs[2, 0].set_xlabel('Load (M)')
axs[2, 0].set_ylabel('Error Rate')

axs[2, 1].plot(load, request_sent_rate, marker='o', linestyle='--', color='g')
axs[2, 1].set_title('Request Sent Rate vs Load')
axs[2, 1].set_xlabel('Load (M)')
axs[2, 1].set_ylabel('Request Sent Rate')

plt.tight_layout()
plt.show()
