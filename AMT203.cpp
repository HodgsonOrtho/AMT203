// Include required libraries
#include "Arduino.h"
#include "SPI.h"
#include "AMT203.h"


// Allocate storage for the encoder's command response
uint8_t response = nop;  // response byte init as no operation
uint8_t mostSigByte;  // position MSB
uint8_t leastSigByte;  // position LSB
uint16_t position;  // rotational position
uint16_t shiftInCache;  // 16-bit unsigned integer memory space for position
uint8_t timeoutCounter;  // counter of cycles waiting for rd_pos response
bool waiting = true;  // status if waiting for a position response from encoder

// init an AMT203 object
AMT203::AMT203(int amtSCK, int amtMOSI, int amtMISO, int amtCS, int clockRate)
{
  // assign pinModes as i/o based on SPI standards
  pinMode(amtSCK, OUTPUT);
  pinMode(amtMOSI, OUTPUT);
  pinMode(amtMISO, INPUT);
  pinMode(amtCS, OUTPUT);
  // define pins within scope of library
  _amtSCK = amtSCK;
  _amtMOSI = amtMOSI;
  _amtMISO = amtMISO;
  _amtCS = amtCS;
  _clockRate = clockRate;
}

// recieve a byte from AMT203
uint8_t AMT203::getByte(uint8_t sendByte)
{
  digitalWrite(_amtCS, LOW);
  response = SPI.transfer(sendByte);
  digitalWrite(_amtCS, HIGH);
  return response;
}

// set encoder position to zero (datum)
bool AMT203::setZero()
{
  timeoutCounter = 0;  // reset timeoutCounter
  getByte(set_zero_point);  // send set_zero_point command
  // shift register until successful zero point set
  do {
    response = getByte(nop);
    timeoutCounter = timeoutCounter + 1;
  } while ((response != set_zero_point_success) && (timeoutCounter < timeoutLimit));
  // return the success of sending the set_zero_point command
  if (response == set_zero_point_success) {
    return true;
  } else {
    return false;
  }
}

// shift two 8-bit uint into one 16-bit uint
uint16_t AMT203::shiftIn8_16(uint8_t firstByte, uint8_t secondByte)
{
  shiftInCache = (firstByte & 0x0F) << 8;  // shift in MSB and remove two leading bits
  shiftInCache |= secondByte;  // add LSB to the end of the cache
  return shiftInCache;  // return the combined 16-bit unsigned integer
}

// start SPI stream and init AMT203
void AMT203::open()
{
  // SPI initialize with SPISettings of 0.5MHz clock, Most Sig Byte First, Mode 0
  SPI.beginTransaction(SPISettings(_clockRate, MSBFIRST, SPI_MODE0)); 
  // Define starting state for _amtCS pin
  digitalWrite(_amtCS, HIGH);
}

// Define the command function for the AMT203
uint8_t AMT203::command(uint8_t amtCommand)
{
  // call the private getByte function and send amtCommand as a register shift
  response = getByte(amtCommand);
  return response;
}

// Define the getPosition function for the AMT203
uint16_t AMT203::getPosition() 
{
  timeoutCounter = 0;  // reset the timeoutCounter
  response = getByte(rd_pos);  // send a rd_pos command and recieve cach
  // until the rd_pos command is returned or operation times out
  while ((response != rd_pos) && (timeoutCounter < timeoutLimit))
  {
    response = getByte(nop);  // send a no operation byte to shift register
    timeoutCounter = timeoutCounter + 1;  // increment the timeoutCounter
  } 
  // if the operation times out
  if (timeoutCounter >= timeoutLimit && response != rd_pos)
  {
    position = rd_failed;  // returns a read failure byte
  } else {  // else recieve the 16-bit response as MSB, LSB
    mostSigByte = getByte(nop);  // using getByte(rd_pos) would maximize throughput
    leastSigByte = getByte(nop);
    // combine MSB and LSB to 12-bit position value
    position = shiftIn8_16(mostSigByte, leastSigByte);
  }
  return position;
}