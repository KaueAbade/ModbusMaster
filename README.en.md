# ModbusMaster
[![pt-br](https://img.shields.io/badge/lang-pt--br-green.svg)][pt-br]
[![GitHub release](https://img.shields.io/github/release/KaueAbade/ModbusMaster.svg?maxAge=3600)][GitHub release]
[![license](https://img.shields.io/github/license/KaueAbade/ModbusMaster.svg?maxAge=3600)][license]

[pt-br]:  https://github.com/KaueAbade/ModbusMaster/blob/master/README.md
[GitHub release]:   https://github.com/KaueAbade/ModbusMaster
[license]:          LICENSE


## Overview
This is a fork of the [ModbusMaster](https://github.com/4-20ma/ModbusMaster) Arduino library for communicating with Modbus slaves over RS232/485 (via RTU protocol).

It adds an Slave Id call to each Modbus function call and a proper T1.5 and T3.5 delay acording to the [Specification and Implementation Guide for MODBUS over serial line](https://www.modbus.org/docs/Modbus_over_serial_line_V1_02.pdf).

Now the `begin` function takes the baudRate in place of the slave id (`_begin(baudRate, &serial)`) and the functions in itself the slave id as in (`_readHoldingRegisters(slaveId, ReadAddress, ReadQty)`).


## Features
The following Modbus functions are available:

Discrete Coils/Flags

  - 0x01 - Read Coils
  - 0x02 - Read Discrete Inputs
  - 0x05 - Write Single Coil
  - 0x0F - Write Multiple Coils

Registers

  - 0x03 - Read Holding Registers
  - 0x04 - Read Input Registers
  - 0x06 - Write Single Register
  - 0x10 - Write Multiple Registers
  - 0x16 - Mask Write Register
  - 0x17 - Read Write Multiple Registers

Both full-duplex and half-duplex RS232/485 transceivers are supported. Callback functions are provided to toggle Data Enable (DE) and Receiver Enable (/RE) pins.


## Installation

#### Zip Library
Refer to Arduino Tutorials > Libraries [Importing a .zip Library](https://www.arduino.cc/en/Guide/Libraries#toc4).

#### Manual
Refer to Arduino Tutorials > Libraries [Manual Installation](https://www.arduino.cc/en/Guide/Libraries#toc5).


## Hardware
This for of the original library has been tested with an Arduino [Uno](http://www.arduino.cc/en/Main/ArduinoBoardDuemilanove) and a Battery Managment controller, connected via RS485 using a Maxim [MAX488EPA](http://www.maxim-ic.com/quick_view2.cfm/qv_pk/1111) transceiver.


## Caveats
As the original library, this fork conforms to Arduino IDE 1.5 Library Specification v2.1 which requires Arduino IDE >= 1.5.

Arduinos prior to the Mega have one serial port which must be connected to USB (FTDI) for uploading sketches and to the RS232/485 device/network for running sketches. You will need to disconnect pin 0 (RX) while uploading sketches. After a successful upload, you can reconnect pin 0.


## Example
The library contains a few modifications of the original sketches from `Doc Walker` to accommodate the modifications made in the `ModbusMaster` library. You can find these in the [examples](https://github.com/KaueAbade/ModbusMaster/tree/master/examples) folder.

``` cpp
/*

  Basic.pde - example using the ModbusMaster library

  Library:: ModbusMaster
  Author:: Doc Walker <4-20ma@wvfans.net>
  Modified by:: Kauê Abade <kaue.abade@outlook.com>

  Copyright:: 2009-2016 Doc Walker

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

*/

#include <ModbusMaster.h>


// instantiate ModbusMaster object
ModbusMaster node;


void setup()
{
  // use Serial (port 0); initialize Modbus communication baud rate
  Serial.begin(19200);

  // communicate with Modbus slave over Serial (port 0) with 19200 baud rate
  node.begin(19200, Serial);
}


void loop()
{
  static uint32_t i;
  uint8_t j, result;
  uint16_t data[6];
  
  i++;
  
  // set word 0 of TX buffer to least-significant word of counter (bits 15..0) 
  node.setTransmitBuffer(0, lowWord(i));
  
  // set word 1 of TX buffer to most-significant word of counter (bits 31..16)
  node.setTransmitBuffer(1, highWord(i));
  
  // slave: write TX buffer to (2) 16-bit registers starting at register 0 for the slave 1
  result = node.writeMultipleRegisters(1, 0, 2);
  
  // slave: read (6) 16-bit registers starting at register 2 to RX buffer for the slave 1
  result = node.readHoldingRegisters(1, 2, 6);
  
  // do something with data if read is successful
  if (result == node.ku8MBSuccess)
  {
    for (j = 0; j < 6; j++)
    {
      data[j] = node.getResponseBuffer(j);
    }
  }
}
```

_Project inspired by [Arduino Modbus Master](http://sites.google.com/site/jpmzometa/arduino-mbrt/arduino-modbus-master)._


## License & Authors

- Author:: Doc Walker ([4-20ma@wvfans.net](mailto:4-20ma@wvfans.net))
- Author:: Ag Primatic ([agprimatic@gmail.com](mailto:agprimatic@gmail.com))
- Author:: Marius Kintel ([marius@kintel.net](mailto:marius@kintel.net))

```
Copyright:: 2009-2016 Doc Walker

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

## Modification Author

- Author:: Kauê Abade ([kaue.abade@outlook.com](mailto:kaue.abade@outlook.com))
