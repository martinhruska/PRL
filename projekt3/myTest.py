import random
import sys

def getBin(numStr):
    xr = ""
    for i in xb:
        if i == 0:
            xr += chr(0)
        else:
            xr += chr(1)
    return xr


i = int(sys.argv[1])

f = open("numbers",'w')
x = random.randint(pow(2,i),pow(2,i+1)-1)
y = random.randint(pow(2,i),pow(2,i+1)-1)
xb = "{0:b}".format(x)
yb = "{0:b}".format(y)
x = lambda x,y: (bin(int(x,2)+int(y,2))[2:])
#f.write(getBin(xb))
f.write(xb)
f.write('\n')
#f.write(getBin(yb))
f.write(yb)
op = open("operands",'w')
op.write(xb)
op.write('\n')
op.write(yb)
op.close()
s = x(xb,yb)
if s > len(xb):
    print s[1:]
else:
    print s
f.close()
