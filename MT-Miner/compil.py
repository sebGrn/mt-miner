import sys
import os
import shutil 
import subprocess
import argparse
import fileinput
from time import perf_counter

def get_object_count(filename):
    object_count = 0
    with fileinput.FileInput(filename) as file:
        for line in file:
            list_line = [int(n) for n in line.split()]
            for elt in list_line:
                object_count = max(int(elt), object_count)
    return object_count


def get_items_count(filename):
    transaction_count = sum(1 for line in open(filename))
    return transaction_count


def update_bitset_count(transation_count):
    # replace BITSET_SIZE with transaction_count into Item.h
    # from https://stackoverflow.com/questions/17140886/how-to-search-and-replace-text-in-a-file
    new_line = "#define BITSET_SIZE " + str(transation_count)
    filename = "Item.h"
    with fileinput.FileInput(filename, inplace=True) as file:
        for line in file:
            if '#define BITSET_SIZE' in line:                
                print(line.replace(line, new_line))
            else:
                print(line, end='')

                
def run_compilation(transation_count):
    # run cmake and make
    # create .out
    folder_path = "build" + str(transation_count)
    if not os.path.exists(folder_path):
        os.makedirs(folder_path)
        
        os.chdir(folder_path)

        p = subprocess.Popen(["cmake", "..", "-DCMAKE_BUILD_TYPE=Release"])
        p.wait()

        p = subprocess.Popen(["make"])
        p.wait()

        os.chdir("..")

        
def run_miner(file, transation_count):
    # run mt miner with file 
    
    folder_path = "build" + str(transation_count)

    #subprocess.run(["./shd", "0", "../data/generated.txt"])
    #subprocess.run(["./shd", "0", "../data/test_clone.txt"])
    
    # verbose : print logs into console
    # log : print logs into file
    # output : save results into file
    # use clone : use clone optimisation
    p = subprocess.Popen(["./" + folder_path + "/mt_miner", file, "--verbose=true", "--log=false", "--output=false", "--use-clone=true"])
    p.wait()


def run(filename):
    start = perf_counter()

    file = filename
    transaction_count = get_items_count(file)
    print("items count :", transaction_count)

    update_bitset_count(transaction_count)
    run_compilation(transaction_count)
    run_miner(file, transaction_count)

    end = perf_counter()
    print("Elapsed time:", end - start, "sec") 


# start program
if(len(sys.argv) != 2):
    print("usage :  python3 ./compil.py \"../data/win800.dat\"")
else:
    filename = str(sys.argv[1])
    print("loading hypergraph from file", filename)
    run(filename)
