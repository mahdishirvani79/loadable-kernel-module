import os
from threading import th
from random import randint

def gateway(fd: int):
    for i in range(1000000):
        get_from = randint(1,99)
        command = "e,{},0,1".format(get_from)
        os.write(fd, command)

path = "/dev/iutnode"
fd = os.open(path, os.O_RDWR)


t1 = th.Thread(target=gateway, args=(fd,))
t2 = th.Thread(target=gateway, args=(fd,))
print("started")

# start two threades
t1.start()
t2.start()
t1.join()
t2.join()

data = os.read(fd, 2000)
#print(f'Number of bytes read: {len(data)}')
data = data.decode()
balances = data.split(",")
balances = balances[:-1]
balances = [int(i) for i in balances]
if balances[0] != 4000000:
    print("false")
for i in balances[1:]:
    if balances > 2000000:
        print("false")
if sum(balances) != 200000000:
    print("false")
os.close(fd)
