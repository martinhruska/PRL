#! /usr/bin/python

import sys

f = open(sys.argv[1], 'r')

num = 0
line1 = []
line2 = []
for i in f:
    if num == 0:
        line1 = i.split(' ')
    elif num == 1:
        line2 = i.split(' ')
    num += 1
line1.remove('\n')
line2.remove('\n')
if line1 == line2:
    print "[OK]"
else:
    print "[Fail]"
    print(line1)
    print(line2)
