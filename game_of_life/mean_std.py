import numpy as np

def main():
    files = ["seq.txt", "parfor.txt", "par.txt"]
    for file in files:
        with open(file) as f:
            times = f.readlines()
            times = np.array([int(t) for t in times])
            times = times*1e-6
            print("for {} we have {} +- {}".format(file, round(times.mean(), 4), round(times.std(), 4)))

if __name__ == "__main__":
    main()