
# MT-Miner

Minimal transversal Miner
https://fr.wikipedia.org/wiki/Probl%C3%A8me_de_couverture_par_sommets
https://en.wikipedia.org/wiki/Vertex_cover_in_hypergraphs

Up to now, the SHD algorithm implemented by Takeaki Uno is a standard de facto (http://research.nii.ac.jp/~uno/codes.htm). The aim of this project is to implement as efficient as possible the SBI algorithm dedicated to the extraction of the minimal transversals and to beat the SHD algorithm whenever handling large datasets. The datasets to be considered for performance evaluation are available at (http://research.nii.ac.jp/~uno/dualization.html).

# How to use

## Hypergraph file

A hypergraph could be represented through the triplet K = (ξ, X, R) where ξ and X denote, respectively, finite set of hyperedges (or transactions) and vertex (or items), and R ⊆ ξ x X is the binary relation linking vertex to hyperedges

test.txt: 
```
1 2 
2 3 7 
3 4 5 
4 6 
6 7 8 
7 
```

Hypergraph file is converted into a formal context
A formal context is a triple K = (G, M, I), where G is a set of objects (lines), M is a set of attributes (columns), and I ⊆ G × M is a binary relation called incidence that expresses which objects have which attributes.
```
  1 2 3 4 5 6 7 8
1 1 1 0 0 0 0 0 0
2 0 1 1 0 0 0 1 0
3 0 0 1 1 1 0 0 0
3 0 0 0 1 0 1 0 0
4 0 0 0 0 0 1 1 1
5 0 0 0 0 0 0 1 0
```
## dependancies

### python 3.9
 - https://www.python.org/downloads/

### gcc
 - ```sudo apt update```
 - ```sudo apt install build-essential```

### docker
 - https://docs.docker.com/engine/install/ubuntu/


## use

### with python scripts

1. go to ./scripts/ folder
2. lauch python script ```python3 ./launch.py <data_folder> <algo_to_use> <threshold>```
 - data_folder : folder containing transactional bases (.dat or .txt files), all files will be parsed
 - algo_to_use :
   - all : compute all minimal traverses with miner, then compute only minimal traverses with minimal size with miner, then compute all minimal traverses with shd
   - disj_miner : compute all minimal traverses with miner only, disjonctive mode
   - disj_min_miner : compute only minimal traverses with minimal size with miner only
   - consj_miner : compute all minimal traverses with miner only, consjonctive mode
   - consj_min_miner : compute only minimal traverses with minimal size with miner only, consjonctive mode
   - shd : compute all minimal traverses with shd only
   - pmmcs : compute all minimal traverses with pmmcs only
   - prs : compute all minimal traverses with prs only
 - threshhold : double value, percent threshold to cover

 ex : ```python3 ./launch.py ./data disj_miner 1.0```

### with docker

1. use python scripts located in script folder
2. put the files you want to analyze in your data folder
3. to build the docker image 
-- ```docker build -t miner . ```
4. to run the docker container
- to compute all minimal trasverses with mt-miner:
  - ```docker run -v <your_data_folder_path>:/app/data miner disj_miner 1.0```
- to compute only minimal traverses with minimal size with mt-miner:
  - ```docker run -v <your_data_folder_path>:/app/data miner disj_miner 1.0```
- to compute all minimal traverses with shd:
  - ```docker run -v <your_data_folder_path>:/app/data miner shd 1.0```
- to use all algorithms:
  - ```docker run -v <your_data_folder_path>:/app/data miner all 1.0```

## Logs
- all results logs will be located into ./data/ and ./scripts/ folders  

# More info

## Paper associated

To come

## Authors

Amira Mouakher (<mouakheramira@gmail.com>)
Sebastien Gerin (<gerin.seb@gmail.com>)
