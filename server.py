import socket

ser = socket.socket()
host = ''
port = 2233
ser.bind((host, port))
ser.listen(5)
print('listening 2233')
while True:
    cli, addr = ser.accept()
    print('Accept ip: [{ip}], port: [{port}]'.format(ip=addr[0], port=addr[1]))
    while True:
        try:
            msg = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz'
            count = cli.send(msg.encode())
            print('Send {cnt} bytes'.format(cnt=count))
            msg = cli.recv(1024)
            if len(msg) == 0:
                break
            print('Received:', msg.decode())
        except:
            break
    print('close connection')
    cli.close()