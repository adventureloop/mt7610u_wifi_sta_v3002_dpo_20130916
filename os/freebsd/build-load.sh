#!/bin/sh                                               
                                                        
set -e                                                  
                                                        
echo "building kernel modules"                          
echo "  - building firmware module"                     
                                                        
cd run_mtfw                                             
make                                                    
cd ..                                                   
                                                        
echo "  - building kernel driver"                       
make CONF_CFLAGS=-march=armv7a                          
sleep 1                                                 
                                                        
echo "loading kernel modules"                           
sudo kldload ./run_mtfw/run_mtfw.ko                     
sudo kldload ./run_mt.ko                                
                                                        
echo "creating interface"                               
sleep 2                                                 
sudo ifconfig wlan0 create wlandev run0 wlanmode monitor
