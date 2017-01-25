#!/bin/sh

echo "loading kernel modules"

kldload ./run_mtfw/run_mtfw.ko
kldload ./run_mt.ko


echo "creating interface"
sleep(2)

ifconfig wlan0 create wlandev run0 wlanmode monitor

