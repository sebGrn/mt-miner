import subprocess
import sys
import os
from time import perf_counter

import compil as cp

if(len(sys.argv) != 3):
    print("usage :  python3 ./launch.py \"../data/\" log.txt")
else:
    dataset_folder = str(sys.argv[1])
    log_file = str(sys.argv[2])
    cpp_log_file = "tmp_log.txt"

    #if os.path.exists(log_file):
    #    os.remove(log_file)

    onlyfiles = [f for f in os.listdir(dataset_folder) if os.path.isfile(os.path.join(dataset_folder, f))]
    if(len(onlyfiles) == 0):
        print("no dataset in ", dataset_folder)
    else:        
        current_path = os.getcwd()
        log_file = os.path.join(current_path, log_file)
        cpp_log_file = os.path.join(current_path, cpp_log_file)
        
        with open(log_file, "w") as writer:

            for file in onlyfiles:
                if file.lower().endswith(('.txt', '.dat')):
                    fullpath = os.path.join(dataset_folder, file)

                    print("--------------------------------------------------------------------------------")            
                    print("processing data file:", fullpath)
                    writer.write("--------------------------------------------------------------------------------\n")
                    writer.write("--------------------------------------------------------------------------------\n")
                    writer.write("processing data file:" + fullpath + "\n")
                    writer.write("\nMiner execution...\n")
                    
                    print("Miner execution...")
                    os.chdir("../MT-Miner")
                    start = perf_counter()            
                    cp.compil_and_run_miner(fullpath, cpp_log_file)
                    end = perf_counter()
                    #print("Elapsed time for compilation and Miner execution:", end - start, "sec")
                    print("\n")
                    
                    os.chdir(current_path)

                    # open log from cpp and append to python log file
                    f = open(cpp_log_file)
                    writer.write(f.read())

                    print("SHD execution...")
                    writer.write("\nSHD execution...\n")
                    os.chdir("../shd")
                    start = perf_counter()
                    subprocess.run(["./shd", "0", fullpath])
                    end = perf_counter()
                    total = end - start
                    writer.write("Elapsed time for SHD:" + str(total) + "sec\n") 
                    print("Elapsed time for SHD:", total, "sec") 
                    print("\n")
                    writer.write("\n")
                    
                    os.chdir(current_path)

                    