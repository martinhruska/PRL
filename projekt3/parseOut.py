import sys

f = open(sys.argv[1],'r')

r = {}
for l in f:
    if ':' in l:
        l = l.rstrip()
        p = l.split(':')
        r[int(p[0])] = p[1]
res = ""
for c in r.keys():
    res = r[c] + res
print res
