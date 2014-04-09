import socket
import time

cli=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
cli.connect(('localhost',8001))
time.sleep(2)
while True:
    data=raw_input('>>>')
    if not data:
        break
    #data=data.encode('ascii')
    cli.send(data)
    #data=cli.recv(1024)
    print(data)

cli.close()


    