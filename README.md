# esp_mqtt_ports

Control up to 512 digital pins [with your smartphone](http://www.openhab.org/) and one esp8266 (with 8 GPIOS).

This can be done by connecting 4*8 [mcp23017 (I²C version)](http://www.microchip.com/wwwproducts/Devices.aspx?dDocName=en023499) port expanders to the GPIO ports like this.

I²C group | sda | scl | mcp23017
----------|-----|-----|--------------
0         |GPIO0+10KΩ pullup|GPIO2 +10KΩ pullup| 8*mcp23017 with address 0..7
1         |GPIO4+10KΩ pullup|GPIO5 |8*mcp23017 with address 0..7
2         |GPIO12+10KΩ pullup|GPIO13 |8*mcp23017 with address 0..7
3         |GPIO14 +10KΩ pullup|GPIO15+10KΩ **pulldown**|8*mcp23017 with address 0..7

Be careful with GPIO4 and GPIO5, they are sometimes mislabeled on the silkscreen.

You can add pullups to the ```scl``` pins too, but they are always driven by the esp so that's not required.

Take care with the boot mode pins of the esp, GPIO0, GPIO2 should be high and GPIO15 should be low at normal bootup (did i get that right?). 

This is only the default pinout, it's really easy to change.

###dependencies

I'm pulling in several git subtree projects :
* [tuanpmt's mqtt project: esp_mqtt](https://github.com/tuanpmt/esp_mqtt) 
* [The mcp23017 driver](https://github.com/eadf/esp8266_mcp23017)
* [The i2c driver](https://github.com/eadf/esp8266_i2c_driver)
* [gpio helper library](https://github.com/eadf/esp8266_easygpio)
* [esp8266_stdout](https://github.com/eadf/esp8266_stdout)

The makefile is copied from [esp_mqtt.](https://github.com/tuanpmt/esp_mqtt)

###mqtt format

topic: ```/deviceId/portexpander```  deviceId is unique to each esp, look in the console. 

message: ```GABBBBBBBBBBBBBBBB``` where:
* G = I²C group [0..3]
* A = Address within I²C group [0..7]
* BBBBBBBBBBBBBBBB = binary of pin status, msb first
 
If you want to set the first pin high (rest low) on the second mcp23017 (address 1) in the third I²C group (2) you send this message:```210000000000000001``` (as a string)

### todo
* tests, it seems to work - but i have not tested a full compliment of mcp23017:s.
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

You will have to edit the ```include/user_config.h``` file to suit your WiFi and mqtt settings. Alternatively you can copy ```include/user_config.h``` into ```localinclude/user_config.h``` and git will not bother you about modified files.

To make a clean build, flash and connect to the esp console you just do this in a shell:
```
source setenv.sh # This is only needed once per session
make clean && make test
```

You won't be needing esptool, the makefile only uses esptool.py (provided by [esp_open_sdk](https://github.com/pfalcon/esp-open-sdk))

I have tested this with sdk v0.9.5 and v0.9.4 (linux & mac)

### Processing demo

I've created a simple processing sketch that sends mqtt messages to the esp.
You will have to adjust the address of your mqtt broker and the topic your esp listens to.

The sketch uses [ControlP5](http://www.sojamo.de/libraries/controlP5/) and [processing-mqtt](https://github.com/256dpi/processing-mqtt)
