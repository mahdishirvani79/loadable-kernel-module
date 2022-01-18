make
sudo insmod ./lkmmodule.ko
MAJOR= sudo cat /proc/devices | grep iut_device | cut -d" " -f 1
sudo mknod /dev/iutnode c $MAJOR 0

sudo python3 pytest.py

sudo rm /dev/iutnode
sudo rmmod lkmmodule
make clean
