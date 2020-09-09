import subprocess
from time import perf_counter

start = perf_counter()
#subprocess.run(["./shd", "0", "../data/generated.txt"])
#subprocess.run(["./shd", "0", "../data/test_clone.txt"])
subprocess.run(["./../x64/Release/MT-Miner.exe", "../data/win800.dat", "--verbose=true", "--log=false", "--output=true", "--use-clone=true"])
end = perf_counter()

print("Elapsed time:", end - start, "sec") 
