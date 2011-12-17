#!/bin/env python
import sys, socket, json


s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((sys.argv[1], int(sys.argv[2])))

con_start = {"action":"report", "status":"consumer-start"}
s.send(json.dumps(con_start))
s.recv(1024)

get_num = {"action":"get", "num":2}
while True:
    s.send(json.dumps(get_num))
    msg = s.recv(1024)
    try:
        reply = json.loads(msg)
    except:
        print msg
    
    print reply
    
    put_req = {"action":"put", "integers":[reply["integers"][0] + reply["integers"][1]] }
    s.send(json.dumps(put_req))
    s.recv(1024)
    

