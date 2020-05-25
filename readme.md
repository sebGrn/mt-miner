# MT-Miner

Minimal transversal Miner
https://fr.wikipedia.org/wiki/Probl%C3%A8me_de_couverture_par_sommets

Up to now, the SHD algorithm implemented by Takeaki Uno is a standard de facto (http://research.nii.ac.jp/~uno/codes.htm). The aim of this project is to implement as efficient as possible the SBI algorithm dedicated to the extraction of the minimal transversals and to beat the SHD algorithm whenever handling large datasets. The datasets to be considered for performance evaluation are available at (http://research.nii.ac.jp/~uno/dualization.html).

# How to use
##  Dependancies
- BOOST library
Used for *dynamic_biset* type
  - windows 
    - download boost from https://www.boost.org/
    - add boost installation path into ($BOOST_PATH) environement variable to PATH
  - linux 
```
curl --location --silent https://dl.bintray.com/boostorg/release/1.72.0/source/boost_1_72_0.tar.gz --output boost_1_72_0.tar.gz
tar -x -f boost_1_72_0.tar.gz
cd  /setup/boost_1_72_0
./bootstrap.sh --with-libraries=regex,program_options
./b2 -j$THREADS cxxflags="-fPIC" runtime-link=static variant=release link=static install
```


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
## use

```
./MT-Miner ../../data/test.txt --verbose=<true/false> --log=<true/false> --output=<true/false> --use-clone=<true/false>
```

## Options
- *--verbose* : print log on console
- *--log* : write log into *test.txt.log* file
- *--output* : write results into *test.txt.out* file
- *--use-clone* : use of clone optimisation


# More info

## Paper associated

To come

## Research lab

- http://www.ciad-lab.fr/

## Authors

Amira Mouakher (<amira.mouakher@u-bourgogne.fr>)
Sebastien Gerin (<sebastien.gerin@sayens.fr>)