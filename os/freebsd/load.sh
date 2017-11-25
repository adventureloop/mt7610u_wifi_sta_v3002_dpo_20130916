#!/bin/zsh

echo "building kernel modules"

pushd run_mtfw
make 
popd

echo "loading kernel modules"

sudo kldload ./run_mtfw/run_mtfw.ko
sudo kldload ./run_mt.ko

echo "creating interface"
sleep 10

sudo ifconfig wlan0 create wlandev run0 wlanmode monitor

sleep 10
echo "shutting down"
sudo kldunload run_mtfw
sudo kldunload run_mt
