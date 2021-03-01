import random
import sys
import numpy as np

'''
object_count = 100
item_count = 100
nb_clone = 20
filename = str(sys.argv[1])
'''

def generate(output, object_count, item_count, nb_clone):
    itemset_list = np.array([])

    # add few itemset combinaison into a list : [97 100, 18 20] for exemple
    # then add these items into one and only one line 
    # [97, 100, 18, 20] must not be added elsewhere 
    # --> we have a clone

    clone_list = []
    tmp_clone_list = []
    while len(clone_list) < (nb_clone * 2):
        a = random.randint(1, object_count)
        b = random.randint(1, object_count)
        if (a not in clone_list) and (b not in clone_list) and (a != b):
            clone_list.append(a)
            clone_list.append(b)
            tmp_clone_list.append(a)
            tmp_clone_list.append(b)

    #print(clone_list)

    f = open(output, "w")
    for i in range(item_count):
        
        n = random.randint(0, object_count)
        
        for j in range(n):
            # write random number on [1 30]
            x = random.randint(1, object_count)        
            if x not in clone_list:
                itemset_list = np.append(itemset_list, x)
            #else:
            #    print(x, "is in", clone_list)                

        if(len(tmp_clone_list)):
            a = tmp_clone_list.pop(0)
            b = tmp_clone_list.pop(0)
            itemset_list = np.append(itemset_list, a)
            itemset_list = np.append(itemset_list, b)
            #print(tmp_clone_list)

        if(len(itemset_list)):
            itemset_list = np.unique(itemset_list)
            itemset_list = itemset_list.astype(int)
            itemset_list = np.sort(itemset_list)
            line = ' '.join(map(str, itemset_list)) 
            f.write(line + '\n')

        for k in range(n):
            itemset_list = itemset_list[itemset_list != k]

        '''
        tmp = []
        for k in range(n):
            tmp.append(k)
        
        

        indexes_to_remove = []
        for j in itemset_list:
            if j in tmp:
                indexes_to_remove.append(j)
        
        print(indexes_to_remove)
        itemset_list = np.delete(itemset_list, indexes_to_remove)
        '''
        
    f.close()
'''
# start program
if(len(sys.argv) != 5):
    print("usage :  python3 ./generator.py \<outputfile> <nb_object> <nb_items> <nb_clones>")
else:
    filename = str(sys.argv[1])
    object_count = int(sys.argv[2])
    item_count = int(sys.argv[3])
    nb_clone = int(sys.argv[4])
    generate(filename, object_count, item_count, nb_clone)
'''
#generate("gen_10_100_0.txt", 10, 100, 0)
#generate("gen_20_100_0.txt", 20, 100, 0)
#generate("gen_30_100_0.txt", 30, 100, 0)

#generate("gen_10_100_10.txt", 10, 100, 10)
#generate("gen_20_100_10.txt", 20, 100, 10)
#generate("gen_30_100_10.txt", 30, 100, 10)

generate("gen_100_100_0.txt", 100, 100, 0)
generate("gen_100_200_0.txt", 100, 200, 0)
generate("gen_100_300_0.txt", 100, 300, 0)

generate("gen_100_100_50.txt", 100, 100, 50)
generate("gen_100_200_50.txt", 100, 200, 50)
generate("gen_100_300_50.txt", 100, 300, 50)

