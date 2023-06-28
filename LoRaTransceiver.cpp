#include "Arduino.h"
#include "LoRaTransceiver.h"
#include <LoRa.h>

LoRaTransceiver::LoRaTransceiver(int ss, int rst, int dio0, int maxNackCount, int timeout) {
  _ss = ss;
  _rst = rst;
  _dio0 = dio0;
  _maxNackCount = maxNackCount;
  _timeout = timeout;
  _waitingForAck = false;
  _lastSendTime = 0;
  _sendAttempts = 0;
  _nackCount = 0;
  _lastControlMessage = "";
  _bufferedMessage = "";
}

void LoRaTransceiver::begin(long frequency) {
  LoRa.setPins(_ss, _rst, _dio0);
  if (!LoRa.begin(frequency)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initializing OK!");
}

void LoRaTransceiver::send(String message) {
  if (!_waitingForAck) {
    sendPacket(message);
    _lastSent = message;
    _waitingForAck = true;
    _lastSendTime = millis();
    _sendAttempts = 1;
  } else if (millis() - _lastSendTime > _timeout) {
    if (_sendAttempts < _maxNackCount) {
      sendPacket(_lastSent);
      _lastSendTime = millis();
      _sendAttempts++;
    } else {
      _waitingForAck = false;
      Serial.println("Failed to send message");
    }
  }
}

String LoRaTransceiver::receive() {
  if (_bufferedMessage != "") {
    String message = _bufferedMessage;
    _bufferedMessage = "";
    return message;
  }
  String received = receivePacket();
  if ((received == "ACK" || received == "NACK") && !_waitingForAck) {
    _lastControlMessage = received;
    return "";
  }
  return received;
}

bool LoRaTransceiver::checkForAck() {
  String received = _lastControlMessage;
  _lastControlMessage = "";
  if (received == "") {
    received = receivePacket();
  }
  if (received == "ACK") {
    _waitingForAck = false;
    _nackCount = 0;
    return true;
  } else if (received == "NACK") {
    if (_nackCount < _maxNackCount) {
      sendPacket(_lastSent);
      _lastSendTime = millis();
      _nackCount++;
    } else {
      _waitingForAck = false;
      _nackCount = 0;
      Serial.println("Too many consecutive NACKs, giving up on message");
    }
    return false;
  } else if (received != "") {
    _bufferedMessage = received;
  }
  return false;
}

void LoRaTransceiver::sendPacket(String message) {
  byte checksum = 0;
  for (int i = 0; i < message.length(); i++) {
    checksum ^= message.charAt(i);
  }
  message += (char)checksum;

  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}

String LoRaTransceiver::receivePacket() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";
    byte checksum = 0;
    while (LoRa.available()) {
      char c = (char)LoRa.read();
      received += c;
      checksum ^= c;
    }
    byte receivedChecksum = received.charAt(received.length() - 1);
    received = received.substring(0, received.length() - 1);
    if (checksum == receivedChecksum) {
      return received;
    } else {
      sendPacket("NACK");
    }
  }
  return "";
}
