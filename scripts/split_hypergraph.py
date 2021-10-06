from itertools import zip_longest
import os
import sys
import launch as miner

def file_len(fname):
    with open(fname) as f:
        for i, l in enumerate(f):
            pass
    return i + 1

def grouper(n, iterable, fillvalue=None):
    "Collect data into fixed-length chunks or blocks"
    # grouper(3, 'ABCDEFG', 'x') --> ABC DEF Gxx
    args = [iter(iterable)] * n
    return zip_longest(fillvalue=fillvalue, *args)

def split_hypergraph(nb_split, filen, output_folder):
    nb_lines = file_len(file)
    n = int(nb_lines / nb_split) + 1

    with open(file) as f:
        for i, g in enumerate(grouper(n, f, fillvalue=''), 1):
            with open(output_folder + '/small_file_{0}'.format(i * n), 'w') as fout:
                fout.writelines(g)

# --------------------------------------------------------------------------------------------------------- #

# --------------------------------------------------------------------------------------------------------- #

if(len(sys.argv) != 3):
    print("usage :  python3 ./split_hypergraph.py 10 ../data/ac_130k.dat")
else:
    nb_split = int(sys.argv[1])
    file = str(sys.argv[2])

    head, tail = os.path.split(file)
    filename, file_extension = os.path.splitext(tail)
    dataset_folder = head
    print(dataset_folder)
    output_folder = "split_" + filename
    if(os.path.exists(output_folder) == False):
        os.mkdir(output_folder)
    
    print("split hypergraph file")
    split_hypergraph(nb_split, file, output_folder)

    print("compute all minimal transverses with miner")
    log_file = "disj_miner.csv"
    miner.run_miner(dataset_folder, log_file, "false", "false", 1.0)
    
