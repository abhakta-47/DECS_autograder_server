import matplotlib.pyplot as plt

num_clients = []
overall_throughput = []
average_response_time = []
active_threads = []
cpu_utilization = []

with open("analysis.txt", "r") as file:
    for line in file:
        parts = line.strip().split(',')
        if len(parts) == 3:
            try:
                num = int(parts[0])
                thr = float(parts[1])
                avg_rt = float(parts[2])
                num_clients.append(num)
                overall_throughput.append(thr)
                average_response_time.append(avg_rt)
            except ValueError:
                pass

with open("active_threads.txt", "r") as file:
    for line in file:
        at = line.strip()
        try:
            active_threads.append(float(at))
        except ValueError:
            pass

with open("cpu_utilization.txt", "r") as file:
    for line in file:
        cu = line.strip()
        try:
            cpu_utilization.append(float(cu))
        except ValueError:
            pass

min_length = min(len(num_clients), len(active_threads), len(cpu_utilization))
num_clients = num_clients[:min_length]
active_threads = active_threads[:min_length]
cpu_utilization = cpu_utilization[:min_length]

plt.figure(figsize=(16, 12))

plt.subplot(3, 2, 1)
plt.plot(num_clients, overall_throughput[:min_length], marker='o', linestyle='-', color='b')
plt.xlabel('Number of Clients (M)')
plt.ylabel('Overall Throughput')
plt.title('Overall Throughput vs Number of Clients')

plt.subplot(3, 2, 2)
plt.plot(num_clients, average_response_time[:min_length], marker='o', linestyle='-', color='r')
plt.xlabel('Number of Clients (M)')
plt.ylabel('Average Response Time (s)')
plt.title('Average Response Time vs Number of Clients')

plt.subplot(3, 2, 3)
plt.plot(num_clients, active_threads, marker='o', linestyle='-', color='g')
plt.xlabel('Number of Clients (M)')
plt.ylabel('Average Active Threads')
plt.title('Average Active Threads vs Number of Clients')

plt.subplot(3, 2, 4)
plt.plot(num_clients, cpu_utilization, marker='o', linestyle='-', color='m')
plt.xlabel('Number of Clients (M)')
plt.ylabel('CPU Utilization')
plt.title('CPU Utilization vs Number of Clients')

plt.tight_layout()

plt.savefig("loadtest_results_extended.png")

plt.show()
