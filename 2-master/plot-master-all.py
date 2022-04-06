import sys
import numpy as np
import matplotlib.pyplot as plt

results_file = 'results-master-all.csv' # chunkSize, rank, calcTime, commTime
width = 0.5

data = np.genfromtxt(results_file, delimiter=',', skip_header=1)
data = data.reshape(-1, 9, 4)

fig, ax = plt.subplots()
ax.bar(data[0,:,1], data[0,:,2], width, label='Calculation')
ax.bar(data[0,:,1], data[0,:,3], width, bottom=data[0,:,2], label='Communication')
ax.set_xticks(np.arange(data[0,:,1].size))
plt.ylim(0,60)
ax.set_xlabel('Process #')
ax.set_ylabel('Time (s)')
ax.legend(loc='upper center')

# plt.show()
plt.savefig('results-master-all.png', format='png')
