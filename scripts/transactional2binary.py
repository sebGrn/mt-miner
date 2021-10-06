import fileinput
import sys
import os
import numpy as np
import json

# true if indexes start at 1
def is_zero_indexed(filename):
    zero_indexed = False
    with fileinput.FileInput(filename) as file:
        for line in file:
            list_line = [int(n) for n in line.split()]
            for elt in list_line:
                if elt == 0:
                    zero_indexed = True
                    return zero_indexed
    return zero_indexed


def get_object_count(filename):
    hypergraph = []
    object_count = 0
    with fileinput.FileInput(filename) as file:
        for line in file:
            list_line = [int(n) for n in line.split()]
            if list_line:
                hypergraph.append(list_line)
            for elt in list_line:
                object_count = max(int(elt), object_count)
    return hypergraph, object_count


def get_item_count(filename):
    transaction_count = sum(1 for line in open(filename))
    return transaction_count


def get_format_context(filename): 
    item_count = get_item_count(filename)
    hypergraph, object_count = get_object_count(filename)

    print(hypergraph)

    formal_context = []

    zero_indexed = is_zero_indexed(filename)

    row_nb = 0
    for transactions in hypergraph:
        if zero_indexed:
            row = [0] * (object_count + 1)
        else:
            row = [0] * object_count
        for i in transactions:
            if zero_indexed:
                row[i] = 1
            else:
                row[i - 1] = 1
        formal_context.append(row)

    print(formal_context)
    return formal_context


def save_formal_context(output_file, formal_context):
    file = open(output_file, "w")
    for i in range(len(formal_context)):
        transactions = formal_context[i]
        string_ints = [str(int) for int in transactions]
        str_of_ints = " ".join(string_ints)
        file.write(str_of_ints)
        if i != (len(formal_context) - 1):
            file.write("\n")
    file.close()


def convert(input_filename, output_filename):
    formal_context = get_format_context(input_filename)
    save_formal_context(output_filename, formal_context)

if(len(sys.argv) != 3):
    print("usage :  python3 ./transactional2binary.py transactional_base.dat binary_base.txt")
else:
    input_filename = str(sys.argv[1])
    output_filename = str(sys.argv[2])

    convert(input_filename, output_filename)