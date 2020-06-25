import random
import numpy as np

object_count = 1000
item_count = 30
nb_clone = 30

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

print(clone_list)

f = open("generated.txt", "w")
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
        print(tmp_clone_list)

    if(len(itemset_list)):
        itemset_list = np.unique(itemset_list)
        itemset_list = itemset_list.astype(int)
        itemset_list = np.sort(itemset_list)
        line = ' '.join(map(str, itemset_list)) 
        f.write(line + '\n')

    tmp = []
    for k in range(n):
        tmp.append(k)
    itemset_list = np.delete(itemset_list, tmp)
    
f.close()