# esp_mqtt_ports

Control up to 512 digital pins [with your smartphone](http://www.openhab.org/) and one esp8266 (with 8 GPIOS).

This can be done by connecting 4*8 [mcp23017](http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en023499) port expanders to the GPIO ports like this.

I²C group | scl | sda
----------|-----|-----
0         |GPIO0|GPIO2
1         |GPIO4|GPIO5
2         |GPIO12|GPI13
3         |GPIO14|GPI15

###mqtt format

topic: ```"/deviceId/portexpander"```  deviceId is unique to each esp, look in the console. 

message: ```"GABBBBBBBBBBBBBBBB"``` where:
* G = I²C group [0..3]
* A = Address within I²C group [0..7]
* BBBBBBBBBBBBBBBB = binary of pin status, msb first
 
If you want to set the first pin high (rest low) on the second mcp23017 (address 1) in the third I²C group (2) you send this message:```"210000000000000001"``` 

### todo
* tests, it seems to work. but i have not tested a full compliment of mcp23017:s.
* input mode

###License
GPL v3

The makefile is copied from [esp_mqtt.](https://github.com/tuanpmt/esp_mqtt)

###Building and installing:

First you need to install the sdk and the easy way of doing that is to use [esp_open_sdk.](https://github.com/pfalcon/esp-open-sdk)

You can put that anywhere you like (/opt/local/esp-open-sdk, /esptools etc etc)

Then you could create a small ```setenv.sh``` file, containing the location of your newly compiled sdk and other platform specific info;
```
export SDK_BASE=/opt/local/esp-open-sdk/sdk
export PATH=${SDK_BASE}/../xtensa-lx106-elf/bin:${PATH}
export ESPPORT=/dev/ttyO0  
```
(or setup your IDE to do the same)

To make a clean build, flash and connect to the esp console you just do this in a shell:
```
source setenv.sh # This is only needed once per session
make clean && make test
```

You won't be needing esptool, the makefile only uses esptool.py (provided by [esp_open_sdk](https://github.com/pfalcon/esp-open-sdk))

I have tested this with sdk v0.9.5 and v0.9.4 (linux & mac)
