#ifndef LoRaTransceiver_h
#define LoRaTransceiver_h

#include "Arduino.h"
#include <LoRa.h>

class LoRaTransceiver {
  public:
    LoRaTransceiver(int ss, int rst, int dio0, int maxNackCount = 5, int timeout = 2000);
    void begin(long frequency);
    void send(String message);
    String receive();
    bool checkForAck();
  private:
    int _ss;
    int _rst;
    int _dio0;
    int _maxNackCount;
    int _timeout;
    String _lastSent;
    bool _waitingForAck;
    unsigned long _lastSendTime;
    int _sendAttempts;
    int _nackCount;
    void sendPacket(String message);
    String receivePacket();
    String _lastControlMessage;
    String _bufferedMessage;
};

#endif
