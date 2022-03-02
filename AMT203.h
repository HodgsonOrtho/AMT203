#ifndef AMT203_h
#define AMT203_h

#include "Arduino.h"

// Define AMT203 configuration
#define positionsPerRotation = 4096  // reported positions per rotation
// Define SPI configuration
#define timeoutLimit 100  // max cycles before rd_pos command failure
// SPI Commands for the AMT20
#define nop 0x00  // no operation (idle)
#define rd_pos 0x10  // read position
#define rd_failed 0x0000  // read position failed
#define set_zero_point 0x70  // set zero point OR set origin
#define set_zero_point_success 0x80  // response confirming zero point reset

class AMT203
{
 public:
  AMT203(int amtSCK, int amtMOSI, int amtMISO, int amtCS, int clockRate);
  void open();
  bool setZero();
  uint8_t command(uint8_t amtCommand);
  uint16_t getPosition();
 
 private:
  int _amtSCK, _amtMOSI, _amtMISO, _amtCS, _clockRate;
  uint8_t getByte(uint8_t sendByte);
  uint16_t shiftIn8_16(uint8_t firstByte, uint8_t secondByte);
};

#endif
