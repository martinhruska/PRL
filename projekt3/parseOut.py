#! /usr/bin/python

import sys

f = open(sys.argv[1],'r')

r = {}
o = False
for l in f:
    if ':' in l:
        l = l.rstrip()
        p = l.split(':')
        r[int(p[0])] = p[1]
    if "overflow" in l:
        o = True
res = ""
for c in r.keys():
    res = r[c] + res
print res
if o:
    print "overflow"
