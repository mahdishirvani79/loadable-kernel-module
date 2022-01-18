make clean
sudo rmmod lkmmodule.
sudo rm /dev/iutnode
make
sudo insmod ./lkmmodule.ko
sudo mknod /dev/iutnode c 240 0
