import sys
import numpy as np
import matplotlib.pyplot as plt

results_file = 'results-cyclic.csv'

width = 0.5

data = np.genfromtxt(results_file, delimiter=',', skip_header=1)
ranks = data[:,0]
calcs = data[:,1]
waits = data[:,2]
comms = data[:,3]

fig, ax = plt.subplots()
ax.bar(ranks, calcs, width, label='Calculation')
ax.bar(ranks, waits, width, bottom=calcs, label='Waiting')
ax.bar(ranks, comms, width, bottom=calcs+waits, label='Communication')
ax.set_xticks(np.arange(ranks.size))
plt.ylim(0,50)
ax.set_xlabel('Process #')
ax.set_ylabel('Time (s)')
ax.legend(loc='upper center')

# plt.show()
plt.savefig('results-cyclic.png', format='png')
