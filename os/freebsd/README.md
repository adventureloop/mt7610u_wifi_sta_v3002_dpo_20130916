= FreeBSD Port =

Here I am going to stick any links and documentation I find.

http://adrianchadd.blogspot.co.uk/2015/09/porting-wifi-driver-from-openbsd-ar9170.html

Approach

	* ~~Get run building as a module out of tree~~
	* ~~remove most of the driver~~
	* Attach and probe
	* add bits in

Add

product MEDIATEK RT7610U    0x7610  MediaTek Wi-Fi
sys/dev/usb/usbdevs

Wikidevi Says"
	" The firmware uploaded MUST be firmware specifically for the RT7610U.
	Trying to upload firmware for the RT7601U will not work.

	The firmware freely available with the sha1sum of
	f48eb7211d9f0304198c3a5896997bbb31e6cac2 is known to work for the RT7610U,
	so would be a good baseline to use for writing an initial driver. "

https://wikidevi.com/wiki/MediaTek_MT7610U

Which is here:
mcu/bin/MT7610_formal_2.6.bin
