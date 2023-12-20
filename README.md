[![ESP CI](https://github.com/ioio2995/twai_slcan/actions/workflows/esp-idf.yml/badge.svg)](https://github.com/ioio2995/twai_slcan/actions/workflows/esp-idf.yml)

# twai_slacn
ENGLISH 
## Introduction
This repo is based on the work of [cvra/CAN-USB-dongle-fw](https://github.com/cvra/CAN-USB-dongle-fw)

## Usage

use with SocketCAN
```
sudo modprobe can
sudo modprobe can-raw
sudo modprobe slcan

# configure CAN interface
sudo slcand -f -o -s8 -t hw -S 3000000 ttyACM0 slcan0
sudo ip link set can1 up
# info CAN interface
ip -s link 

# use with can-utils
cansend slcan0 123#2a
candump slcan0
```

## Building & flashing
### Prerequisites

Before you can use this project, make sure to install the ESP-IDF framework. 
[Standard Toolchain Setup for Linux and macOS](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/linux-macos-setup.html)

### Building

```
#config project
idf.py set-target esp32s3
# change values in menuconfig:
#"APP - TWAI settings" 
#"APP - Led RGB settings"
#Enable : Component config → TinyUSB Stack → Communication Device Class (CDC)
idf.py menuconfig

# building
idf.py build

# flashing using USB
#To do so, press and hold the ``BOOT`` button and then press the ``RESET`` button once. 
#After that release the ``BOOT`` button.
idf.py -p /dev/ttyUSB0 flash
```

## Compatibility
 - Linux-can/SocketCAN

## Documentation
- [LAWICEL CanUSB](http://www.can232.com/docs/canusb_manual.pdf)
- [USBtin - USB to CAN interface](https://www.fischl.de/usbtin/#socketcan) 
## Done
- [x] 
## Todo
- [ ] 
