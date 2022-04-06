import sys
import numpy as np
import matplotlib.pyplot as plt

results_file = 'results-master.csv'

width = 0.5

data = np.genfromtxt(results_file, delimiter=',', skip_header=1)
chunks = data[:,0]
execs = data[:,1]

fig, ax = plt.subplots()
ax.plot(chunks, execs, width)
ax.set_xscale('log')
ax.set_xlabel('Chunk Size')
ax.set_ylabel('Execution Time (s)')
ax.legend(loc='upper center')

# plt.show()
plt.savefig('results-master.png', format='png')


