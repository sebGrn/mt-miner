# -------------------------------------------------------------------------------------------------- #
# get opencascade code source and compil it 
FROM ubuntu:18.04

WORKDIR /

# get dependencies packages
# python3 : python library
RUN apt-get update && apt install -y git cmake wget gcc g++
RUN apt install -y  python3-pip

# -------------------------------------------------------------------------------------------------- #
# copy miner code 

WORKDIR /

RUN mkdir app
RUN mkdir app/mt-miner/

# copy all directory into container
COPY MT-Miner/*.cpp app/MT-Miner/
COPY MT-Miner/*.h app/MT-Miner/
COPY MT-Miner/CMakeLists.txt app/MT-Miner/


# -------------------------------------------------------------------------------------------------- #
# copy python script files

WORKDIR /
RUN mkdir /app/scripts/

# copy all directory into  container
COPY scripts/*.py /app/scripts/

# -------------------------------------------------------------------------------------------------- #
# copy shd

WORKDIR /
RUN mkdir /app/shd/
# copy all directory into  container
COPY shd/* /app/shd/

# -------------------------------------------------------------------------------------------------- #

WORKDIR /app/scripts

# install python dependencies
COPY scripts/requirements.txt .
RUN pip3 install -r requirements.txt

WORKDIR /
RUN mkdir app/data/

WORKDIR /app/scripts

RUN alias python3=python3.8

# run launch.py script on startup
# minimal transverse from hypergraphs located in ../data folder will be computed
# results output in cpp_log_all.csv, cpp_log_cut.csv and shd_log_file.csv
ENTRYPOINT ["python3", "./launch.py", "../data"]
CMD ["all"]

#CMD ["python3", "./launch.py", "../data"]

# docker build -t miner . 
# docker run -v d:\dev\repository\MT-Miner\data:/app/data miner
# docker run -v /home/sebastien.gerin/miner/compare_shd_miner/data:./app/data miner

# docker run -it -v /home/sebastien.gerin/miner/compare_shd_miner/data:./app/data miner

# docker run -it miner ../data/