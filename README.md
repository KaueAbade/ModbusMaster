# ModbusMaster
[![en](https://img.shields.io/badge/lang-en-red.svg)][en]
[![GitHub release](https://img.shields.io/github/release/KaueAbade/ModbusMaster.svg?maxAge=3600)][GitHub release]
[![license](https://img.shields.io/github/license/KaueAbade/ModbusMaster.svg?maxAge=3600)][license]

[en]:  https://github.com/KaueAbade/ModbusMaster/blob/master/README.en.md
[GitHub release]:   https://github.com/KaueAbade/ModbusMaster
[license]:          LICENSE


## Visão geral
Este é um "fork" da biblioteca [ModbusMaster](https://github.com/4-20ma/ModbusMaster) para o Arduino, feita para comunicação com dispositivos por Modbus via RS232/485 (protocolo RTU).

Essa modificação foi feita para adicionar uma chamada de "Slave Id" a cada função Modbus e um atraso T1.5 e T3.5 adequado no processo de comunicação, de acordo com o [Guia de especificação e implementação para MODBUS em linha serial] (https://www.modbus.org/docs/Modbus_over_serial_line_V1_02.pdf).

Agora a função `begin` usa o baudRate no lugar do ID do escravo (`_begin(baudRate, &serial)`) e as funções em si usam o ID do escravo como em (`_readHoldingRegisters(slaveId, ReadAddress, ReadQty)`).

Partes dessa descrição são traduzidas do repositório original.


## Recursos
As seguintes funções Modbus estão disponíveis:

Bobinas/Flags discretos

  - 0x01 - Ler bobinas
  - 0x02 - Ler entradas discretas
  - 0x05 - Gravar bobina única
  - 0x0F - Gravar várias bobinas

Registros

  - 0x03 - Ler registros de retenção
  - 0x04 - Ler registros de entrada
  - 0x06 - Gravar um registro
  - 0x10 - Gravar vários registros
  - 0x16 - Registro de escrita de máscara
  - 0x17 - Leitura e gravação de vários registros

Há suporte para transceptores RS232/485 full-duplex e half-duplex. As funções de retorno são fornecidas para alternar os pinos `Data Enable (DE)` e `Receiver Enable (/RE)`.

## Instalação

#### Biblioteca Zip
Consulte Tutoriais do Arduino > Bibliotecas [Importando uma biblioteca .zip] (https://www.arduino.cc/en/Guide/Libraries#toc4).

#### Manual
Consulte Tutoriais do Arduino > Bibliotecas [Instalação manual] (https://www.arduino.cc/en/Guide/Libraries#toc5).


## Hardware
Esta versão da biblioteca foi testada com um Arduino [Uno] (http://www.arduino.cc/en/Main/ArduinoBoardDuemilanove) e um controlador de gerenciamento de bateria, conectado via RS485 usando um transceptor Maxim [MAX488EPA] (http://www.maxim-ic.com/quick_view2.cfm/qv_pk/1111).


## Advertências
Como a biblioteca original, esse "fork" está em conformidade com a especificação da biblioteca Arduino IDE 1.5 v2.1, que requer o Arduino IDE >= 1.5.

Os Arduinos anteriores ao Mega têm apenas uma porta serial que deve ser conectada ao USB (FTDI) para carregar esboços e ao dispositivo/rede RS232/485 para executar esboços. Você precisará desconectar o pino 0 (RX) durante o carregamento dos esboços. Após um upload bem-sucedido, você pode reconectar o pino 0.


## Exemplo
A biblioteca contém algumas modificações dos esboços originais de "Doc Walker" para acomodar as modificações feitas na biblioteca "ModbusMaster". Você pode encontrá-las na pasta [examples](https://github.com/KaueAbade/ModbusMaster/tree/master/examples).

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

_Projeto original inspirado por [Arduino Modbus Master](http://sites.google.com/site/jpmzometa/arduino-mbrt/arduino-modbus-master)._


## Licença & Autores

- Autor: Doc Walker ([4-20ma@wvfans.net](mailto:4-20ma@wvfans.net))
- Autor: Ag Primatic ([agprimatic@gmail.com](mailto:agprimatic@gmail.com))
- Autor: Marius Kintel ([marius@kintel.net](mailto:marius@kintel.net))

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

## Autor da modificação

- Autor:: Kauê Abade ([kaue.abade@outlook.com](mailto:kaue.abade@outlook.com))
