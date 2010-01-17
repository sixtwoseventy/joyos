#!/usr/bin/env python

import sys
import os

def port2stdout(portname):
    import serial
    port = serial.Serial(portname,19200)

    while 1:
        data = port.read()
        if data != '':
            sys.stdout.write(data)
            sys.stdout.flush()
        else:
            break

    port.close()

def stdin2files():
    import subprocess
    teams = eval(open('/mit/6.270/term/teams', 'r').read())
    c = subprocess.call
    def p(x):
        #print ' '.join(x)
        c(x)
    subprocess.call = p
    N = len(teams)
    files = {}
    for k,v in teams.items():
        d = '/mit/6.270/term/%d' % int(k)
        subprocess.call(['mkdir', '-p', d])
        subprocess.call(['touch', '%s/term' % d])
        acl = sum([[name, 'rld'] for name in v], [])
        subprocess.call(['fs', 'sa', '-d', d, '-a'] + acl)
        files[int(k)] = open('%s/term' % d, 'a')
    while 1:
        data = sys.stdin.readline()
        if data != '':
            if ord(data[0]) in files:
                f = files[ord(data[0])]
                f.write(data[1:])
                f.flush()
                print 'Team %02d: %s' % (ord(data[0]), data[1:].strip())
            else:
                print 'Out-of-sync data: %s' % repr(data)
            sys.stdout.flush()
    for f in file:
        f.close()

def main(argv=None):
    if argv is None:
        argv = sys.argv
    if len(argv) > 1:
        port2stdout(argv[1])
    else:
        stdin2files()

if __name__ == "__main__":
    sys.exit(main())
