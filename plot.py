#Plot code


import matplotlib.pyplot as plt

num_clients = []
throughput = []
average_response_time = []

with open("analysis.txt", "r") as file:
    for line in file:
        parts = line.strip().split(',')
        if len(parts) == 3:
            try:
                num = int(parts[0])
                thr = float(parts[1])
                avg_rt = float(parts[2])
                num_clients.append(num)
                throughput.append(thr)
                average_response_time.append(avg_rt)
            except ValueError:
                pass  

plt.figure(figsize=(12, 6))

plt.subplot(1, 2, 1)
plt.plot(num_clients, throughput, marker='o', linestyle='-', color='b')
plt.xlabel('Number of Clients (M)')
plt.ylabel('Overall Throughput')
plt.title('Overall Throughput vs Number of Clients')

plt.subplot(1, 2, 2)
plt.plot(num_clients, average_response_time, marker='o', linestyle='-', color='r')
plt.xlabel('Number of Clients (M)')
plt.ylabel('Average Response Time (s)')
plt.title('Average Response Time vs Number of Clients')

plt.tight_layout()

plt.savefig("loadtest_results.pdf")

plt.show()

