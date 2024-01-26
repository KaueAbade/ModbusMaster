#include <ModbusMaster.h>
#include <SoftwareSerial.h>

#define period 1000
#define baudRate 9600
#define MAX485_DE 4
#define MAX485_RE 5
#define rxPin 2
#define txPin 3

ModbusMaster node;
SoftwareSerial mySerial(rxPin, txPin);

unsigned short cmd;
unsigned short slaveId;
unsigned int lastReadMillis;
unsigned int lastPrintMillis;

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

void slaveCheck() {
  for (int i = 1; i <= 15; i++) {
    uint8_t result = node.readHoldingRegisters(i, 0x0000, 1);
    if (result == node.ku8MBSuccess) {
      // Serial.print("Slave ");        //DEBUG
      // Serial.print(i);               //DEBUG
      // Serial.println(" available");  //DEBUG
      slaveId = i;
      return;
    }
  }
  return;
}

short getHoldingRegistersAddress() {
  switch (cmd) {
    case 0:
      {
        return 0;
      }
    case 1:
      {
        return 15;
      }
    case 2:
      {
        return 31;
      }
  }
  return;
}

short getDataSize() {
  switch (cmd) {
    case 0:
      {
        return 8;
      }
    case 1:
      {
        return 16;
      }
    case 2:
      {
        return 6;
      }
  }
  return;
}

uint8_t getSerialData() {
  node.clearResponseBuffer();
  return node.readHoldingRegisters(slaveId, getHoldingRegistersAddress(), getDataSize());
}

void storeSerialData() {
  unsigned short dataSize = getDataSize();

  unsigned int data[dataSize];
  for (int i = 0; i < dataSize; i++)
    data[i] = node.getResponseBuffer(i);

  switch (cmd) {
    case 0:
      {
        Bat.mA = data[0];
        Bat.mV = (data[1] * 10);
        Bat.SOC = data[2];
        Bat.SOH = data[3];
        Bat.remainCapacity = data[4];
        Bat.fullCapacity = data[5];
        Bat.cycleCount = data[7];
        break;
      }
    case 1:
      {
        for (int i = 0; i < 15; i++)
          Bat.cellmV[i] = (data[i] * 10);
        break;
      }
    case 2:
      {
        for (int i = 0; i < 4; i++)
          Bat.cellTemp[i] = (data[i]*10);
        Bat.mosfetTemp = (data[5]*10);
        Bat.envTemp = data[6];
        break;
      }
  }
  return;
}

void loopCmd() {
  cmd++;
  if (cmd == 3)
    cmd = 0;
  return;
}

// void printStatus() {  //DEBUG
//   Serial.print("Bateria: ");
//   Serial.print(Bat.mA);
//   Serial.print(" - ");
//   Serial.print(Bat.mV);
//   Serial.print(" - ");
//   Serial.print(Bat.SOC);
//   Serial.print(" - ");
//   Serial.print(Bat.SOH);
//   Serial.print(" - ");
//   Serial.print(Bat.remainCapacity);
//   Serial.print(" - ");
//   Serial.print(Bat.fullCapacity);
//   Serial.print(" - ");
//   Serial.print(Bat.cycleCount);
//   for (int i = 0; i < 16; i++) {
//     Serial.print(" - ");
//     Serial.print(Bat.cellmV[i]);
//   }
//   for (int i = 0; i < 4; i++) {
//     Serial.print(" - ");
//     Serial.print(Bat.cellTemp[i]);
//   }
//   Serial.print(" - ");
//   Serial.print(Bat.mosfetTemp);
//   Serial.print(" - ");
//   Serial.print(Bat.envTemp);
//   Serial.println();
// }

void setup() {
  Serial.begin(baudRate);
  // Serial.println("Starting...\n");

  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

  mySerial.begin(baudRate);

  pinMode(MAX485_RE, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE, 0);
  digitalWrite(MAX485_DE, 0);

  node.begin(baudRate, mySerial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  // Serial.println("Checking connected slaves:");

  slaveCheck();

  // Serial.println("Slave check finished\n");

  lastReadMillis = millis();
  lastPrintMillis = millis();

  Serial.println("Started!!\n");
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - lastReadMillis >= period1) {
    uint8_t result = getSerialData();
    if (result == node.ku8MBSuccess) {
      // Serial.print("Read: ");  //DEBUG
      // Serial.print(cmd);       //DEBUG
      // Serial.println();        //DEBUG
      storeSerialData();
      loopCmd();
      lastReadMillis = currentMillis;
    }
  }

  // if (currentMillis - lastPrintMillis >= period2) {
  //   printStatus();  //DEBUG
  //   lastPrintMillis = currentMillis;
  // }
}
