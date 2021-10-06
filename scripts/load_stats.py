import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

#----------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------

# disjonctive miner with all branches - disj-miner

df_disj_miner = pd.read_csv("disj_miner.csv", delimiter=';')
df_disj_miner.index = df_disj_miner["filename"]
df_disj_miner.index.name = "filename"
if ('filename.1' in df_disj_miner.columns):
    del df_disj_miner['filename.1']
# drop duplicate columns with df_cpp_cut_branches
df_disj_miner = df_disj_miner.drop(['filename'], axis=1)
df_disj_miner = df_disj_miner.rename(columns = {"minerMinimalTransverseCount":"disjMinerMtCount"})
df_disj_miner = df_disj_miner.rename(columns = {"minerTime":"disjMinerTime"})
#print(df_disj_miner)
#print(df_disj_miner.shape)

#----------------------------------------------------------------------------------------------

# disjunctive miner without all branches - disj-min-miner
"""
df_disj_min_miner = pd.read_csv("disj_min_miner.csv", delimiter=';')
df_disj_min_miner.index = df_disj_min_miner["filename"]
df_disj_min_miner.index.name = "filename"
if ('filename.1' in df_disj_min_miner.columns):
    del df_disj_min_miner['filename.1']
# drop duplicate columns 
df_disj_min_miner = df_disj_min_miner.drop(['filename', 'sparcity', 'cloneCount', 'itemCount', 'objectCount'], axis=1)
df_disj_min_miner = df_disj_min_miner.rename(columns = {"minerTime":"m-disjMinerTime"})
df_disj_min_miner = df_disj_min_miner.rename(columns = {"minerMinimalTransverseCount":"m-disjMtCount"})
df_disj_min_miner = df_disj_min_miner.rename(columns = {"MinimalSizeOfTransverse":"m-disjMinimalSizeOfTransverse"})
#print(df_disj_min_miner.head())
#print(df_disj_min_miner.shape)
"""
#----------------------------------------------------------------------------------------------

# consjonctive miner with all branches - consj-miner
"""
df_consj_miner = pd.read_csv("consj_miner.csv", delimiter=';')
df_consj_miner.index = df_consj_miner["filename"]
df_consj_miner.index.name = "filename"
if ('filename.1' in df_consj_miner.columns):
    del df_consj_miner['filename.1']
# drop duplicate columns with df_cpp_cut_branches
df_consj_miner = df_consj_miner.drop(['filename'], axis=1)
df_consj_miner = df_consj_miner.rename(columns = {"minerTime": "consjMinerTime"})
df_consj_miner = df_consj_miner.rename(columns = {"minerMinimalTransverseCount": "consjMinerMtCount"})
#print(df_consj_miner.head())
#print(df_consj_miner.shape)
"""
#----------------------------------------------------------------------------------------------
"""
# consjonctive miner without all branches - consj-min-miner

df_consj_min_miner = pd.read_csv("consj_min_miner.csv", delimiter=';')
df_consj_min_miner.index = df_consj_min_miner["filename"]
df_consj_min_miner.index.name = "filename"
if ('filename.1' in df_consj_min_miner.columns):
    del df_consj_min_miner['filename.1']
# drop duplicate columns with df_cpp_cut_branches
df_consj_min_miner = df_consj_min_miner.drop(['filename', 'sparcity', 'cloneCount', 'itemCount', 'objectCount'], axis=1)
df_consj_min_miner = df_consj_min_miner.rename(columns = {"minerTime": "m-consjMinerTime"})
df_consj_min_miner = df_consj_min_miner.rename(columns = {"minerMinimalTransverseCount":"m-consjMtCount"})
df_consj_min_miner = df_consj_min_miner.rename(columns = {"MinimalSizeOfTransverse":"m-consjMinimalSizeOfTransverse"})
#print(df_consj_min_miner.head())
#print(df_consj_min_miner.shape)
"""
#----------------------------------------------------------------------------------------------

df_shd = pd.read_csv("shd_log_file.csv", delimiter=';')
df_shd.index = df_shd["filename"]
df_shd.index.name = "filename"
if ('filename.1' in df_shd.columns):
    del df_shd['filename.1']
df_shd = df_shd.drop(['filename', 'shdMinimalTransverseCount'], axis=1)    
print(df_shd)

#----------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------
"""
# merge disj_miner and disj_min_miner

df_miner_disj = pd.merge(df_disj_miner, df_disj_min_miner, left_index=True, right_index=True)
df_miner_disj = df_miner_disj.append(df_disj_min_miner)
df_miner_disj = df_miner_disj[~df_miner_disj.index.duplicated(keep='first')]
df_miner_disj = df_miner_disj.drop(['minimalSizeOfTransverse'], axis=1)
df_miner_disj = df_miner_disj.fillna(0)
#print(df_miner_disj.head())
#print(df_miner_disj.shape)

# merge columns
#df_miner["m-disjMinimalMtSize"] = df_miner['MinimalSizeOfTransverse'] + df_miner['minimalSizeOfTransverse']
#del df_miner['MinimalSizeOfTransverse']
#del df_miner['minimalSizeOfTransverse']

# renaming

#print(df_miner.head())
#print(df_miner.shape)
"""
#----------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------
"""
# merge consj_miner and consj_min_miner

df_miner_consj = pd.merge(df_consj_miner, df_consj_min_miner, left_index=True, right_index=True)
df_miner_consj = df_miner_consj.append(df_consj_min_miner)
df_miner_consj = df_miner_consj[~df_miner_consj.index.duplicated(keep='first')]
df_miner_consj = df_miner_consj.drop(['minimalSizeOfTransverse'], axis=1)
df_miner_consj = df_miner_consj.fillna(0)
#print(df_miner_consj.head())
#print(df_miner_consj.shape)
"""
#----------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------
"""
df_miner = pd.merge(df_miner_disj, df_miner_consj, left_index=True, right_index=True)
df_miner = df_miner.drop(['cloneCount_y', 'objectCount_y', 'sparcity_y', 'itemCount_y'], axis=1)
#df_miner = df_miner.append(df_miner)
df_miner = df_miner[~df_miner.index.duplicated(keep='first')]
df_miner = df_miner.fillna(0)
df_miner = df_miner.rename(columns = {"cloneCount_x": "cloneCount"})
df_miner = df_miner.rename(columns = {"objectCount_x": "objectCount"})
df_miner = df_miner.rename(columns = {"sparcity_x": "sparcity"})
df_miner = df_miner.rename(columns = {"objectCount_x": "objectCount"})
df_miner = df_miner.rename(columns = {"itemCount_x": "itemCount"})

df_miner = pd.merge(df_miner, df_shd, left_index=True, right_index=True)
df_miner = df_miner.append(df_shd)
df_miner = df_miner[~df_miner.index.duplicated(keep='first')]
df_miner = df_miner.fillna(0)
"""

df_miner = df_miner[['sparcity', 'cloneCount', 'itemCount', 'objectCount', 'shdTime', 'disjMinerTime', 'm-disjMinerTime', 'consjMinerTime', 'm-consjMinerTime', 'disjMinerMtCount', 'm-disjMtCount', 'm-disjMinimalSizeOfTransverse', 'consjMinerMtCount', 'm-consjMtCount', 'm-consjMinimalSizeOfTransverse']]
df_miner['m-consjMinimalSizeOfTransverse'] = df_miner['m-consjMinimalSizeOfTransverse'].astype(int)
df_miner['m-disjMinimalSizeOfTransverse'] = df_miner['m-disjMinimalSizeOfTransverse'].astype(int)
df_miner['m-consjMtCount'] = df_miner['m-consjMtCount'].astype(int)
df_miner['m-disjMtCount'] = df_miner['m-disjMtCount'].astype(int)
df_miner['disjMinerMtCount'] = df_miner['disjMinerMtCount'].astype(int)
df_miner['consjMinerMtCount'] = df_miner['consjMinerMtCount'].astype(int)
df_miner['itemCount'] = df_miner['itemCount'].astype(int)
df_miner['cloneCount'] = df_miner['cloneCount'].astype(int)
df_miner['objectCount'] = df_miner['objectCount'].astype(int)
df_miner = df_miner.round(3)
df_miner.to_csv('log.csv', sep=';')

print(df_miner.head())
print(df_miner.shape)

'''
# merge with df_consj_min_miner

df_miner = pd.merge(df_consj_min_miner, df_miner, left_index=True, right_index=True)
df_miner = df_miner.append(df_consj_min_miner)
df_miner = df_miner[~df_miner.index.duplicated(keep='first')]
df_miner = df_miner.fillna(0)

#print(df_miner.head())
#print(df_miner.shape)
'''

'''
# shd 
df_shd = pd.read_csv("shd_log.csv", delimiter=';')
#df_shd = pd.DataFrame("shd_log.csv", columns=['filename', 'filename', 'shdMinimalTrasverseCount', 'shdTime'])
df_shd.index = df_shd["filename"]
df_shd.index.name = "filename"
if ('filename.1' in df_shd.columns):
    del df_shd['filename.1']
del df_shd['filename']
#print(df_shd.head())
#print(df_shd.shape)

df = pd.merge(df_miner, df_shd, left_index=True, right_index=True)
# renaming
df = df.rename(columns = {"shdMinimalTrasverseCount": "shdMtCount"})
'''

#----------------------------------------------------------------------------------------------
#----------------------------------------------------------------------------------------------
'''
# organize order
df_miner_disj = df_miner_disj[['sparcity', 'cloneCount', 'itemCount', 'objectCount', 'disjMinerMtCount', 'm-disjMtCount', 'm-disjMinimalSizeOfTransverse', 'disjMinerTime', 'm-disjMinerTime']]
df_miner_disj['m-disjMinimalSizeOfTransverse'] = df_miner_disj['m-disjMinimalSizeOfTransverse'].astype(int)
df_miner_disj['m-disjMtCount'] = df_miner_disj['m-disjMtCount'].astype(int)
df_miner_disj['disjMinerMtCount'] = df_miner_disj['disjMinerMtCount'].astype(int)
df_miner_disj['itemCount'] = df_miner_disj['itemCount'].astype(int)
df_miner_disj['cloneCount'] = df_miner_disj['cloneCount'].astype(int)
df_miner_disj['objectCount'] = df_miner_disj['objectCount'].astype(int)
df_miner_disj = df_miner_disj.round(3)
#print(df_miner_disj.head())
#print(df_miner_disj.shape)
df_miner_disj.to_csv('res_miner_disj.csv', sep=';')

#----------------------------------------------------------------------------------------------

df_miner_consj = df_miner_consj[['sparcity', 'cloneCount', 'itemCount', 'objectCount', 'consjMinerMtCount', 'm-consjMtCount', 'm-consjMinimalSizeOfTransverse', 'consjMinerTime', 'm-consjMinerTime']]
df_miner_consj['m-consjMinimalSizeOfTransverse'] = df_miner_consj['m-consjMinimalSizeOfTransverse'].astype(int)
df_miner_consj['m-consjMtCount'] = df_miner_consj['m-consjMtCount'].astype(int)
df_miner_consj['consjMinerMtCount'] = df_miner_consj['consjMinerMtCount'].astype(int)
df_miner_consj['itemCount'] = df_miner_consj['itemCount'].astype(int)
df_miner_consj['cloneCount'] = df_miner_consj['cloneCount'].astype(int)
df_miner_consj['objectCount'] = df_miner_consj['objectCount'].astype(int)
df_miner_consj = df_miner_consj.round(3)
#print(df_miner_consj.head())
#print(df_miner_consj.shape)
df_miner_consj.to_csv('res_miner_consj.csv', sep=';')
'''
'''
# make plot
max_miner = df['minerTime'].max()
max_shd = df['shdTime'].max()
max_value = max(max_miner, max_shd)

time = np.linspace(0, max_value, len(df.index))
df['minerTime'] = time

ax = df.plot(x="filename", y=["minerTime", "shdTime"], kind="bar")

ax.set_xlabel("files")
ax.set_ylabel("seconds")
plt.show()
'''