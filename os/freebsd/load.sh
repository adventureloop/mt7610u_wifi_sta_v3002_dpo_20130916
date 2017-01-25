#!/bin/sh

echo "loading kernel modules"

sudo kldload ./run_mtfw/run_mtfw.ko
sudo kldload ./run_mt.ko


echo "creating interface"
sleep 2

sudo ifconfig wlan0 create wlandev run0 wlanmode monitor
