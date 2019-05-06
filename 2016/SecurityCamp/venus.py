#!/usr/bin/env python
from sc_pwn import *

target     = {'host':'10.1.1.1','port':24154}

#==========
def prob1(cmn):
    cmn.read_until('#1\n')

    stage = 0
    while stage<100:
        stage = int(cmn.read_until('/100\n').split(' ')[-1].split('/')[0])
        expr = cmn.read_until('=')
        v = eval(expr.split('=')[0])
        cmn.sendln(str(v))
        print expr, v

    cmn.read_until('FLAG')
    print "FLAG%s" % cmn.read_until('\n')

def prob2(cmn):
    """
    a b c d e f g
    a+b b+c c+d d+e e+f f+g
    a+2b+c b+2c+d c+2d+e d+2e+f e+2f+g
    a+3b+3c+d b+3c+3d+e c+3d+3e+f d+3e+3f+g
    a+4b+6c+4d+e b+4c+6d+4e+f c+4d+6e+4f+g
    a+5b+10c+10d+5e+f b+5c+10d+10e+5f+g
    a+6b+15c+20d+15e+6f+g
    """
    cmn.read_until('#2\n')

    stage = 0
    while stage<100:
        stage = int(cmn.read_until('/100\n').split(' ')[-1].split('/')[0])
        btm = map(int, cmn.read_until('\n').split(' '))
        v = btm[0]+6*btm[1]+15*btm[2]+20*btm[3]+15*btm[4]+6*btm[5]+btm[6]
        cmn.sendln(str(v))
        print btm, v
        
    cmn.read_until('FLAG')
    print "FLAG%s" % cmn.read_until('\n')

def prob3(cmn):
    import itertools, re
    sigs = list(itertools.product(('+','-','*','/'), repeat=3))
    
    cmn.read_until('#3\n')
    
    stage = 0
    while stage<100:
        stage = int(cmn.read_until('/100\n').split(' ')[-1].split('/')[0])
        nums, v = cmn.read_until('\n').split('=')
        v = int(v)

        for sig in sigs:
            expr = nums.replace('?', '%c') % sig
            if eval(expr)==v:
                cmn.sendln(re.sub(r'\(|\)|\'| ', '', str(sig)))
                print expr, '=', v
                break

    cmn.read_until('FLAG')
    print "FLAG%s" % cmn.read_until('\n')

def prob4(cmn):
    import qrtools
    cmn.read_until('#4\n')

    stage = 0
    while stage<100:
        stage = int(cmn.read_until('/100\n').split(' ')[-1].split('/')[0])
        l_png = cmn.read_until('\n')[2:-2].strip().split()
        s_png = ''
        for c in l_png:
            s_png += chr(int(c,16))

        open('qr.png','wb').write(s_png)
        myCode = qrtools.QR(filename='qr.png')
        myCode.decode()
        data = myCode.data
        cmn.sendln(data)
        print data
        
    cmn.read_until('FLAG')
    print "FLAG%s" % cmn.read_until('\n')

#==========

if __name__=='__main__':
    cmn = Communicate(target,mode='RAW')
    prob1(cmn)
    prob2(cmn)
    prob3(cmn)
    prob4(cmn)
    del(cmn)
    
#==========
