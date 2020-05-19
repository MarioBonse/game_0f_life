import numpy as np
import matplotlib.pyplot as plt
file = 'times.txt'
def give_times(file):
    times = []
    with open(file) as fp: 
        while True: 
            line = fp.readline() 
            if not line: 
                break
            time = float(line)
            times.append(time)
    return times
seq = 64110000
# i = i += 4
par_for = np.array(give_times(file))
times = np.array(range(1,256,4))
par_for = seq/par_for
# multiple line plot
plt.title("10e9 to 10e10 primes")
plt.plot( times, times, markerfacecolor='blue', color='skyblue', label = "perfect speedup")
plt.plot( times, par_for, color='green', linewidth=2,  label="par for speedup")
plt.legend()
plt.show()