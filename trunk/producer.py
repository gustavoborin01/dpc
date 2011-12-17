#!/bin/env python
import sys, socket, json

print sys.argv[1]

prod_start = {"action":"report", "status":"producer-start"}

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((sys.argv[1], int(sys.argv[2])))
s.send(json.dumps(prod_start))
s.recv(1024)

for i in xrange(2000):
    print i
    put_req = {"action":"put", "integers":range(i*5, i*5+5) }
    s.send(json.dumps(put_req))
    s.recv(1024)

print "sending finish..."
prod_fin = {"action":"report", "status":"producer-finish"}
s.send(json.dumps(prod_fin))
s.recv(1024)

print "finish"
msg = json.loads(s.recv(1024))
print msg

