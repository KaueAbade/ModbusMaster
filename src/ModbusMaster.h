/**
@file
Arduino library for communicating with Modbus slaves over RS232/485 (via RTU protocol).

@defgroup setup ModbusMaster Object Instantiation/Initialization
@defgroup buffer ModbusMaster Buffer Management
@defgroup discrete Modbus Function Codes for Discrete Coils/Inputs
@defgroup register Modbus Function Codes for Holding/Input Registers
@defgroup constant Modbus Function Codes, Exception Codes
*/
/*

  ModbusMaster.h - Arduino library for communicating with Modbus slaves
  over RS232/485 (via RTU protocol).

  Library:: ModbusMaster

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

#pragma once
#ifndef ModbusMaster_h
#define ModbusMaster_h

/**
@def __MODBUSMASTER_DEBUG__ (0)
Set to 1 to enable debugging features within class:
  - PIN A cycles for each byte read in the Modbus response
  - PIN B cycles for each millisecond timeout during the Modbus response

#define __MODBUSMASTER_DEBUG__
#define __MODBUSMASTER_DEBUG_PIN_A__ 4
#define __MODBUSMASTER_DEBUG_PIN_B__ 5
*/


/* _____STANDARD INCLUDES____________________________________________________ */
// include types & constants of Wiring core API
#include <Arduino.h>

/* _____PROJECT INCLUDES_____________________________________________________ */


/* _____CLASS DEFINITIONS____________________________________________________ */
#include "util/word.h"    // functions to manipulate words
#include "util/crc16.h"   // functions to calculate Modbus Application Data Unit CRC
/**
Arduino class library for communicating with Modbus slaves over 
RS232/485 (via RTU protocol).
*/
class ModbusMaster
{
  protected:
	  uint16_t baudRate;

    // timing
	  uint32_t T1_5; // inter character timeout
	  uint32_t T3_5; // frame delay (silent interval)

  public:
    ModbusMaster();
   
    void begin(uint16_t, Stream &serial);
    void idle(void (*)());
    void preTransmission(void (*)());
    void postTransmission(void (*)());

    // Modbus exception codes
    /**
    Modbus protocol illegal function exception.
    
    The function code received in the query is not an allowable action for
    the server (or slave). This may be because the function code is only
    applicable to newer devices, and was not implemented in the unit
    selected. It could also indicate that the server (or slave) is in the
    wrong state to process a request of this type, for example because it is
    unconfigured and is being asked to return register values.
    
    @ingroup constant
    */
    static const uint8_t ku8MBIllegalFunction PROGMEM     = 0x01;

    /**
    Modbus protocol illegal data address exception.
    
    The data address received in the query is not an allowable address for 
    the server (or slave). More specifically, the combination of reference 
    number and transfer length is invalid. For a controller with 100 
    registers, the ADU addresses the first register as 0, and the last one 
    as 99. If a request is submitted with a starting register address of 96 
    and a quantity of registers of 4, then this request will successfully 
    operate (address-wise at least) on registers 96, 97, 98, 99. If a 
    request is submitted with a starting register address of 96 and a 
    quantity of registers of 5, then this request will fail with Exception 
    Code 0x02 "Illegal Data Address" since it attempts to operate on 
    registers 96, 97, 98, 99 and 100, and there is no register with address 
    100. 
    
    @ingroup constant
    */
    static const uint8_t ku8MBIllegalDataAddress PROGMEM  = 0x02;
    
    /**
    Modbus protocol illegal data value exception.
    
    A value contained in the query data field is not an allowable value for 
    server (or slave). This indicates a fault in the structure of the 
    remainder of a complex request, such as that the implied length is 
    incorrect. It specifically does NOT mean that a data item submitted for 
    storage in a register has a value outside the expectation of the 
    application program, since the MODBUS protocol is unaware of the 
    significance of any particular value of any particular register.
    
    @ingroup constant
    */
    static const uint8_t ku8MBIllegalDataValue PROGMEM   = 0x03;
    
    /**
    Modbus protocol slave device failure exception.
    
    An unrecoverable error occurred while the server (or slave) was
    attempting to perform the requested action.
    
    @ingroup constant
    */
    static const uint8_t ku8MBSlaveDeviceFailure PROGMEM = 0x04;

    // Class-defined success/exception codes
    /**
    ModbusMaster success.
    
    Modbus transaction was successful; the following checks were valid:
      - slave ID
      - function code
      - response code
      - data
      - CRC
      
    @ingroup constant
    */
    static const uint8_t ku8MBSuccess PROGMEM            = 0x00;
    
    /**
    ModbusMaster invalid response slave ID exception.
    
    The slave ID in the response does not match that of the request.
    
    @ingroup constant
    */
    static const uint8_t ku8MBInvalidSlaveID PROGMEM     = 0xE0;
    
    /**
    ModbusMaster invalid response function exception.
    
    The function code in the response does not match that of the request.
    
    @ingroup constant
    */
    static const uint8_t ku8MBInvalidFunction PROGMEM    = 0xE1;
    
    /**
    ModbusMaster response timed out exception.
    
    The entire response was not received within the timeout period, 
    ModbusMaster::ku8MBResponseTimeout. 
    
    @ingroup constant
    */
    static const uint8_t ku8MBResponseTimedOut PROGMEM   = 0xE2;
    
    /**
    ModbusMaster invalid response CRC exception.
    
    The CRC in the response does not match the one calculated.
    
    @ingroup constant
    */
    static const uint8_t ku8MBInvalidCRC PROGMEM         = 0xE3;
    
    uint16_t getResponseBuffer(uint8_t);
    void     clearResponseBuffer();
    uint8_t  setTransmitBuffer(uint8_t, uint16_t);
    void     clearTransmitBuffer();
    
    void beginTransmission(uint16_t);
    uint8_t requestFrom(uint16_t, uint16_t);
    void sendBit(bool);
    void send(uint8_t);
    void send(uint16_t);
    void send(uint32_t);
    uint8_t available(void);
    uint16_t receive(void);
    
    
    uint8_t  readCoils(uint8_t, uint16_t, uint16_t);
    uint8_t  readDiscreteInputs(uint8_t, uint16_t, uint16_t);
    uint8_t  readHoldingRegisters(uint8_t, uint16_t, uint16_t);
    uint8_t  readInputRegisters(uint8_t, uint16_t, uint8_t);
    uint8_t  writeSingleCoil(uint8_t, uint16_t, uint8_t);
    uint8_t  writeSingleRegister(uint8_t, uint16_t, uint16_t);
    uint8_t  writeMultipleCoils(uint8_t, uint16_t, uint16_t);
    uint8_t  writeMultipleCoils();
    uint8_t  writeMultipleRegisters(uint8_t, uint16_t, uint16_t);
    uint8_t  writeMultipleRegisters();
    uint8_t  maskWriteRegister(uint8_t, uint16_t, uint16_t, uint16_t);
    uint8_t  readWriteMultipleRegisters(uint8_t, uint16_t, uint16_t, uint16_t, uint16_t);
    uint8_t  readWriteMultipleRegisters(uint8_t, uint16_t, uint16_t);
    
  private:
    Stream* _serial;                                             ///< reference to serial port object
    uint8_t  _u8MBSlave;                                         ///< Modbus slave (1..255) initialized in begin()
    static const uint8_t ku8MaxBufferSize PROGMEM        = 64;   ///< size of response/transmit buffers    
    uint16_t _u16ReadAddress;                                    ///< slave register from which to read
    uint16_t _u16ReadQty;                                        ///< quantity of words to read
    uint16_t _u16ResponseBuffer[ku8MaxBufferSize];               ///< buffer to store Modbus slave response; read via GetResponseBuffer()
    uint16_t _u16WriteAddress;                                   ///< slave register to which to write
    uint16_t _u16WriteQty;                                       ///< quantity of words to write
    uint16_t _u16TransmitBuffer[ku8MaxBufferSize];               ///< buffer containing data to transmit to Modbus slave; set via SetTransmitBuffer()
    uint16_t* txBuffer; // from Wire.h -- need to clean this up Rx
    uint8_t _u8TransmitBufferIndex;
    uint16_t u16TransmitBufferLength;
    uint16_t* rxBuffer; // from Wire.h -- need to clean this up Rx
    uint8_t _u8ResponseBufferIndex;
    uint8_t _u8ResponseBufferLength;
    
    // Modbus function codes for bit access
    static const uint8_t ku8MBReadCoils PROGMEM                  = 0x01; ///< Modbus function 0x01 Read Coils
    static const uint8_t ku8MBReadDiscreteInputs PROGMEM         = 0x02; ///< Modbus function 0x02 Read Discrete Inputs
    static const uint8_t ku8MBWriteSingleCoil PROGMEM            = 0x05; ///< Modbus function 0x05 Write Single Coil
    static const uint8_t ku8MBWriteMultipleCoils PROGMEM         = 0x0F; ///< Modbus function 0x0F Write Multiple Coils

    // Modbus function codes for 16 bit access
    static const uint8_t ku8MBReadHoldingRegisters PROGMEM       = 0x03; ///< Modbus function 0x03 Read Holding Registers
    static const uint8_t ku8MBReadInputRegisters PROGMEM         = 0x04; ///< Modbus function 0x04 Read Input Registers
    static const uint8_t ku8MBWriteSingleRegister PROGMEM        = 0x06; ///< Modbus function 0x06 Write Single Register
    static const uint8_t ku8MBWriteMultipleRegisters PROGMEM     = 0x10; ///< Modbus function 0x10 Write Multiple Registers
    static const uint8_t ku8MBMaskWriteRegister PROGMEM          = 0x16; ///< Modbus function 0x16 Mask Write Register
    static const uint8_t ku8MBReadWriteMultipleRegisters PROGMEM = 0x17; ///< Modbus function 0x17 Read Write Multiple Registers
    
    // Modbus timeout [milliseconds]
    static const uint16_t ku16MBResponseTimeout PROGMEM          = 500; ///< Modbus timeout [milliseconds]
    
    // master function that conducts Modbus transactions
    uint8_t ModbusMasterTransaction(uint8_t u8MBFunction);
    
    // idle callback function; gets called during idle time between TX and RX
    void (*_idle)();
    // preTransmission callback function; gets called before writing a Modbus message
    void (*_preTransmission)();
    // postTransmission callback function; gets called after a Modbus message has been sent
    void (*_postTransmission)();
};
#endif

/**
@example examples/Basic/Basic.pde
@example examples/PhoenixContact_nanoLC/PhoenixContact_nanoLC.pde
@example examples/RS485_HalfDuplex/RS485_HalfDuplex.ino
*/
