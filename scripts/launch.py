import subprocess
import sys
import os
import pandas as pd
import time
from time import perf_counter
from shutil import copyfile
import fileinput
import compil as cp

def run_miner(dataset_folder, cpp_log_file, minimal_option, consjonctive_option, threshold):
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
                cp.compil_and_run_miner(fullpath, cpp_log_file, minimal_option, consjonctive_option, threshold)
                #time.sleep(0.25)
                end = perf_counter()
                print("Elapsed time for compilation and Miner execution: ", end - start, " sec")
                print("\n")

        os.chdir(current_path)

        if os.path.isfile(cpp_log_file):
            df_cpp = pd.read_csv(cpp_log_file, delimiter=';', header=None)
            if minimal_option == "false":
                df_cpp.columns = ['filename', 'sparcity', 'cloneCount', 'itemCount', 'objectCount', 'minerMinimalTransverseCount', 'minimalSizeOfTransverse', 'minerTime']
            else:
                df_cpp.columns = ['filename', 'sparcity', 'cloneCount', 'itemCount', 'objectCount', 'minerMinimalTransverseCount', 'MinimalSizeOfTransverse', 'minerTime']
            df_cpp.index = df_cpp["filename"]
            df_cpp.index.name = "filename"
            #df = df.drop(["filename"], axis=1)
            df_cpp.to_csv(cpp_log_file, sep=';')
        else:
            print("no data found")


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


def run_pmmcs(dataset_folder, log_file):
    onlyfiles = [f for f in os.listdir(dataset_folder) if os.path.isfile(os.path.join(dataset_folder, f))]
    if(len(onlyfiles) == 0):
        print("no dataset in ", dataset_folder)
    else:        
        current_path = os.getcwd()

        log_file = os.path.join(current_path, log_file)
        if os.path.isfile(log_file):
            os.remove(log_file)

        data_pmmcs = []  
        for file in onlyfiles:
            if file.lower().endswith(('.txt', '.dat')):
                fullpath = os.path.join(dataset_folder, file)

                print("processing data file:", fullpath)

                print("pMMCS execution...")                    
                os.chdir("../Minimal-Hitting-Set-Algorithms")
                start = perf_counter()
                res = subprocess.run(["./agdmhs", fullpath, "out.dat", "-a", "pmmcs"], stdout=subprocess.PIPE)                
                cpt = 0
                with fileinput.FileInput("out.dat") as file:
                    for line in file:
                        cpt = cpt+1

                #time.sleep(0.5)
                end = perf_counter()
                total = end - start
                print("Elapsed time for pMMCS execution:", total, " sec")
                print("\n")
                    
                # append shd value
                data_pmmcs.append([fullpath, cpt, total])        
        
        os.chdir(current_path)

        df_pmmcs = pd.DataFrame(data_pmmcs, columns=['filename', 'pMMCSMinimalTransverseCount', 'pMMCSTime'])
        df_pmmcs.index = df_pmmcs["filename"]
        df_pmmcs.index.name = "filename"
        df_pmmcs.to_csv(log_file, sep=';')


def run_prs(dataset_folder, log_file):
    onlyfiles = [f for f in os.listdir(dataset_folder) if os.path.isfile(os.path.join(dataset_folder, f))]
    if(len(onlyfiles) == 0):
        print("no dataset in ", dataset_folder)
    else:        
        current_path = os.getcwd()

        log_file = os.path.join(current_path, log_file)
        if os.path.isfile(log_file):
            os.remove(log_file)

        data_pmmcs = []  
        for file in onlyfiles:
            if file.lower().endswith(('.txt', '.dat')):
                fullpath = os.path.join(dataset_folder, file)

                print("processing data file:", fullpath)

                print("pMMCS execution...")                    
                os.chdir("../Minimal-Hitting-Set-Algorithms")
                start = perf_counter()
                res = subprocess.run(["./agdmhs", fullpath, "out.dat", "-a", "prs"], stdout=subprocess.PIPE)                
                cpt = 0
                with fileinput.FileInput("out.dat") as file:
                    for line in file:
                        cpt = cpt+1

                #time.sleep(0.5)
                end = perf_counter()
                total = end - start
                print("Elapsed time for pMMCS execution:", total, " sec")
                print("\n")
                    
                # append shd value
                data_pmmcs.append([fullpath, cpt, total])        
        
        os.chdir(current_path)

        df_pmmcs = pd.DataFrame(data_pmmcs, columns=['filename', 'prsMinimalTransverseCount', 'prsSTime'])
        df_pmmcs.index = df_pmmcs["filename"]
        df_pmmcs.index.name = "filename"
        df_pmmcs.to_csv(log_file, sep=';')


print("usage :  python3 ./launch.py \"../data/\" <all/all_miner/min_miner/shd>")
print("<all> : compute all minimal transverses with miner, then compute only minimal transverses with minimal size with miner, then compute all minimal transverses with shd")
print("<disj_miner> : compute all minimal transverses with miner only, disjonctive mode")
print("<disj_min_miner> : compute only minimal transverses with minimal size with miner only")
print("<consj_miner> : compute all minimal transverses with miner only, consjonctive mode")
print("<consj_min_miner> : compute only minimal transverses with minimal size with miner only, consjonctive mode")
print("<shd> : compute all minimal transverses with shd only")
print("<pmmcs> : compute all minimal transverses with pmmcs only")
print("<prs> : compute all minimal transverses with prs only")
print("<threshold> : minimal transverse threshold")

if(len(sys.argv) != 4):
    print("usage :  python3 ./launch.py \"../data/\" <all> 1.0")
else:
    dataset_folder = str(sys.argv[1])
    option = str(sys.argv[2])
    threshold = float(sys.argv[3])

    if option == "all" or option == "disj_miner":
        print("compute all minimal transverses with miner")
        log_file = "miner_log_all.csv"
        run_miner(dataset_folder, log_file, "false", "false", threshold)
        output = dataset_folder + log_file
        if os.path.isfile(output):
            copyfile(log_file, output)

    if option == "all" or option == "disj_min_miner":
        print("compute only minimal transverses with minimal size with miner")
        log_file = "miner_log_cut.csv"
        run_miner(dataset_folder, log_file, "true", "false", threshold)
        output = dataset_folder + log_file
        if os.path.isfile(output):
            copyfile(log_file, output)

    if option == "all" or option == "consj_miner":
        print("compute all minimal transverses with miner")
        log_file = "miner_log_all_consjonctive.csv"
        run_miner(dataset_folder, log_file, "false", "true", threshold)
        output = dataset_folder + log_file
        if os.path.isfile(output):
            copyfile(log_file, output)

    if option == "all" or option == "consj_min_miner":
        print("compute all minimal transverses with miner")
        log_file = "miner_log_min_consjonctive.csv"
        run_miner(dataset_folder, log_file, "true", "true", threshold)
        output = dataset_folder + log_file
        if os.path.isfile(output):
            copyfile(log_file, output)

    if option == "all" or option == "shd":    
        print("compute all minimal transverses with shd")
        log_file = "shd_log_file.csv"
        run_shd(dataset_folder, log_file)
        output = dataset_folder + log_file
        if os.path.isfile(output):
            copyfile(log_file, output) 

    if option == "all" or option == "pmmcs":
        print("compute all minimal transverses with pMMCS")
        log_file = "pmmcs_log_file.csv"
        run_pmmcs(dataset_folder, log_file)
        output = dataset_folder + log_file
        if os.path.isfile(output):
            copyfile(log_file, output) 

    if option == "all" or option == "prs":
        print("compute all minimal transverses with prs")
        log_file = "prs_log_file.csv"
        run_pmmcs(dataset_folder, log_file)
        output = dataset_folder + log_file
        if os.path.isfile(output):
            copyfile(log_file, output) 