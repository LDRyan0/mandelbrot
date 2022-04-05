import sys
import numpy as np
import maptlotlib.pyplot as pyplot

file = 'block-results.csv'

width = 0.5

data = np.genfromtxt(sys.argv[1], delimiter=',')
ranks = res[:,0]
calcs = res[:,1]
waits = res[:,2]
comms = res[:,3]

fig, ax = plt.subplots()
ax.bar(ranks, calcs, width, label='Communication')

plt.show()