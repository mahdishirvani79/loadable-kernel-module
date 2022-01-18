import os
from threading import th
from random import randint

def gateway(fd):
    for i in range(1000000):
        get_from = randint(1,99)
        command = "e,{},0,1".format(get_from)
        os.write(fd, command)

path = "/dev/iutnode"
fd = os.open(path, os.O_RDWR)
data = os.read(fd, 2000)
print(f'Number of bytes read: {len(data)}')
data = data.decode()
balances = data.split(",")
balances = balances[:-1]
balances = [int(i) for i in balances]
os.close(fd)