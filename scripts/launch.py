import subprocess
import sys
import os
import pandas as pd
import time
from time import perf_counter
from shutil import copyfile

import compil as cp

def run_miner(dataset_folder, cpp_log_file, minimal_option):
    onlyfiles = [f for f in os.listdir(dataset_folder) if os.path.isfile(os.path.join(dataset_folder, f))]
    if(len(onlyfiles) == 0):
        print("no dataset in ", dataset_folder)
    else:        
        current_path = os.getcwd()
        
        cpp_log_file = os.path.join(current_path, cpp_log_file)
        if os.path.isfile(cpp_log_file):
            os.remove(cpp_log_file)
                        
        for file in onlyfiles:
            if file.lower().endswith(('.txt', '.dat')):
                fullpath = os.path.join(dataset_folder, file)

                print("processing data file:", fullpath)                
                print("Miner execution...")                

                os.chdir("../MT-Miner")
                start = perf_counter()            
                cp.compil_and_run_miner(fullpath, cpp_log_file, minimal_option)
                #time.sleep(0.25)
                end = perf_counter()
                print("Elapsed time for compilation and Miner execution: ", end - start, " sec")
                print("\n")

        os.chdir(current_path)

        df_cpp = pd.read_csv(cpp_log_file, delimiter=';', header=None)
        df_cpp.columns = ['filename', 'sparcity', 'cloneCount', 'itemCount', 'objectCount', 'minerMinimalTransverseCount', 'minimalSizeOfTransverse', 'minerTime']
        df_cpp.index = df_cpp["filename"]
        df_cpp.index.name = "filename"
        #df = df.drop(["filename"], axis=1)
        df_cpp.to_csv(cpp_log_file, sep=';')


def run_shd(dataset_folder, shd_log_file):
    onlyfiles = [f for f in os.listdir(dataset_folder) if os.path.isfile(os.path.join(dataset_folder, f))]
    if(len(onlyfiles) == 0):
        print("no dataset in ", dataset_folder)
    else:        
        current_path = os.getcwd()

        shd_log_file = os.path.join(current_path, shd_log_file)
        if os.path.isfile(shd_log_file):
            os.remove(shd_log_file)

        data_shd = []  
        for file in onlyfiles:
            if file.lower().endswith(('.txt', '.dat')):
                fullpath = os.path.join(dataset_folder, file)

                print("processing data file:", fullpath)

                print("SHD execution...")                    
                os.chdir("../shd")
                start = perf_counter()
                res = subprocess.run(["./shd", "0", fullpath], stdout=subprocess.PIPE)
                res = str(res).split()[4]
                res = res[res.find('\'')+1:res.find('\\')]
                #time.sleep(0.5)
                end = perf_counter()
                total = end - start
                print("Elapsed time for SHD execution:", total, " sec")
                print("\n")
                    
                # append shd value
                data_shd.append([fullpath, int(res), total])        
        
        os.chdir(current_path)

        df_shd = pd.DataFrame(data_shd, columns=['filename', 'shdMinimalTransverseCount', 'shdTime'])
        df_shd.index = df_shd["filename"]
        df_shd.index.name = "filename"
        df_shd.to_csv(shd_log_file, sep=';')
                

print("usage :  python3 ./launch.py \"../data/\" <all/all_miner/min_miner/shd>")
print("<all> : compute all minimal transverses with miner, then compute only minimal transverses with minimal size with miner, then compute all minimal transverses with shd")
print("<all_miner> : compute all minimal transverses with miner only")
print("<min_miner> : compute only minimal transverses with minimal size with miner only")
print("<shd> : compute all minimal transverses with shd only")

if(len(sys.argv) != 3):
    print("usage :  python3 ./launch.py \"../data/\" <all/all_miner/min_miner/shd>")
else:
    dataset_folder = str(sys.argv[1])
    option = str(sys.argv[2])

    if option == "all" or option == "all_miner":
        print("compute all minimal transverses with miner")
        run_miner(dataset_folder, "cpp_log_all.csv", "false")
        output = dataset_folder + "cpp_log_all.csv"
        copyfile("cpp_log_all.csv", output)

    if option == "all" or option == "min_miner":
        print("compute only minimal transverses with minimal size with miner")
        run_miner(dataset_folder, "cpp_log_cut.csv", "true")
        output = dataset_folder + "cpp_log_all.csv"
        copyfile("cpp_log_cut.csv", output)

    if option == "all" or option == "shd":    
        print("compute all minimal transverses with shd")
        run_shd(dataset_folder, "shd_log_file.csv")
        output = dataset_folder + "shd_log_file.csv"   
        copyfile("shd_log_file.csv", output) 

