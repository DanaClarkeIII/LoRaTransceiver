# LoRaTransceiver Arduino Library

## Overview
The LoRaTransceiver library provides a simple and robust way to control LoRa devices on Arduino boards. It provides an interface for sending, receiving, and acknowledging LoRa messages, implementing basic error handling and retransmission functionality. 

## Dependencies
This library depends on the [Arduino LoRa](https://github.com/sandeepmistry/arduino-LoRa) library. Please make sure to install it first.

## Installation
Download the zip file of this library and extract it to the Arduino libraries directory on your system.

## Constructor
The constructor `LoRaTransceiver(int ss, int rst, int dio0, int maxNackCount = 5, int timeout = 2000);` is used to create an instance of the LoRaTransceiver class. It accepts pin numbers for SS, RST, and DIO0 along with optional maxNackCount and timeout parameters.

## Functions
The library includes the following public methods:
- `void begin(long frequency)`: Initializes the LoRa module with the given frequency.
- `void send(String message)`: Sends the given message via the LoRa module.
- `String receive()`: Receives and returns a message from the LoRa module.
- `bool checkForAck()`: Checks for an acknowledgement message ("ACK") from the LoRa module.

## Usage
Below is a simple example sketch on how to use the library:

```cpp
#include <LoRaTransceiver.h>

LoRaTransceiver lora(10, 9, 2);

void setup() {
  Serial.begin(9600);
  lora.begin(433E6);
}

void loop() {
  String message = "Hello, LoRa!";
  lora.send(message);
  delay(1000);

  String received = lora.receive();
  if (received != "") {
    Serial.println("Received: " + received);
  }

  if (lora.checkForAck()) {
    Serial.println("Message acknowledged");
  }
}
```


## Error Handling
The library includes some basic error handling capabilities. If a message isn't acknowledged after a predefined number of retransmissions (_maxNackCount), the library will stop trying to send the message. Errors during the initialization of the LoRa module result in an infinite loop.

## Limitations
The current implementation doesn't support LoRaWAN, only raw LoRa communication. The library also doesn't provide any encryption for the messages, so it should not be used for transmitting sensitive information.

The library assumes that an "ACK" or "NACK" response is expected for every sent message, which might not suit every use case. If a "NACK" is received, the library automatically resends the message up to _maxNackCount times.
