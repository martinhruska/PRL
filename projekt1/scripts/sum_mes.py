#! /usr/bin/python

fs = ["mes1","mes2","mes3","mes4","mes5"]
ds = {}

for i in fs:
    f = open(i,'r')
    for l in f:
        t = l.split()
        k = int(t[0])
        if k in ds:
            ds[k] += float(t[1])
        else:
            ds[k] = float(t[1])
    f.close()
        
for i in ds.keys():
    print i,ds[i]/len(fs)
