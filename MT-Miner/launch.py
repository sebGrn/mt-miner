import subprocess
import sys
from time import perf_counter

if(len(sys.argv) != 2):
    print("usage :  python3 ./launch.py \"../data/win800.dat\"")
else:
    filename = str(sys.argv[1])

    print(" --- Miner ---")
    start = perf_counter()
    subprocess.run(["./mt_miner", filename, "--verbose=false", "--log=false", "--output=false", "--use-clone=true"])
    end = perf_counter()
    print(" --- Elapsed time for Miner:", end - start, "sec ---") 

    print(" -----------")

    print(" --- SHD ---")
    start = perf_counter()
    subprocess.run(["./shd", "0", filename])
    end = perf_counter()
    print(" --- Elapsed time for SHD:", end - start, "sec ---") 
