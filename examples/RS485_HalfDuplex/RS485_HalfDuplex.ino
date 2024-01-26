/*
  RS485_HalfDuplex.pde - example using ModbusMaster library to communicate
  with a Battery Managment controller using a half-duplex RS485 transceiver.

  This example is tested against a Battery Managment controller.

  Library:: ModbusMaster
  Author:: Marius Kintel <marius at kintel dot net>
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
#include <SoftwareSerial.h>
#define period 5000          //ie 5 seconds
#define baudRate 9600

/*
  I've used a MAX485-compatible RS485 Transceiver through Software Serial.
  Rx/Tx and Data Enable and Receiver Enable pins are hooked up as follows:
*/


#define MAX485_DE 4
#define MAX485_RE 5
#define rxPin 2
#define txPin 3

//Start ModbusMaster node and SoftwareSerial
ModbusMaster node;
SoftwareSerial mySerial(rxPin, txPin);

unsigned int lastReadMillis;

//It was originally used for multiple attributes, I removed them to keep the example simple
class Bat {
public:
  unsigned long mV;
};

Bat Bat;

/*
  Necessary pre and post transmission functions for swapping the MAX485
  compatible RS485 Transceiver between RX and TX mode
  HIGH is transmiting and LOW is receiving
*/

void preTransmission() {
  digitalWrite(MAX485_RE, 1);
  digitalWrite(MAX485_DE, 1);
  return;
}

void postTransmission() {
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);
  return;
}

/*
  Clear response buffer to avoid error and requests just the register
  located in 0x01 from the first slave
*/

uint8_t getSerialData() {
  node.clearResponseBuffer();
  return node.readHoldingRegisters(1, 0x01, 1);
}

//Get the response buffer and store it in the global Bat object
void storeSerialData() {
  unsigned short dataSize = 1;  //Datasize is the same as the number of register requested in my case

  //It's excessive for one register, but it's an example of how you can retrieve a long byte
  unsigned int data[dataSize];
  for (int i = 0; i < dataSize; i++)
    data[i] = node.getResponseBuffer(i);

  Bat.mV = (data[0] * 10);  //For some reason the battery returned the number missing one zero
}

void printStatus() {  //DEBUG
  Serial.print("Battery Voltage: ");
  Serial.print(Bat.mV);
  Serial.println("mV");
}

void setup() {
  //Starts Serial 0 for debug
  Serial.begin(baudRate);
  Serial.println("Starting...\n");

  //Setups and starts SoftwareSerial using the RS485 transceiver
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  mySerial.begin(baudRate);

  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE, 0);  //Starts as receiver
  digitalWrite(MAX485_DE, 0);

  //Starts the ModbusMaster node with the SoftwareSerial and functions stated before
  node.begin(baudRate, mySerial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  lastReadMillis = millis();

  Serial.println("Started!!\n");
}

void loop() {
  unsigned long currentMillis = millis();

  //It's true every 5 seconds or so
  if (currentMillis - lastReadMillis >= period) {
    uint8_t result = getSerialData();

    //Just go through if the request was successful
    if (result == node.ku8MBSuccess) {
      storeSerialData();
      printStatus();  //DEBUG
      lastReadMillis = currentMillis;
    }
  }
}
