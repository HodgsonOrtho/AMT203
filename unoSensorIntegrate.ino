/*
  Arduino Sensor Integration Test
  By:  Armaan Krishnar
        Mechatronic Systems Engineering
        Research and Development Co-Op
  For: Hodgson Orthopedic Group
  
  A sketch to intake the data from a rotary encoder via its onboard SPI controller.
  
  Intended Functionality
    Simplify the use of an SPI-controlled absolute rotary encoder to simple functional calls.
*/

// SPI Library
#include <SPI.h>

// AMT203 functionality
#include "AMT203.h"

// Define baud rate for serial communications
#define baudRate 9600
// #define timeoutLimit 100

// Define pinModes for the board
#define rotaryEncoderSCK 13  // Orange
#define rotaryEncoderMOSI 11  // Green
#define rotaryEncoderMISO 12  // Yellow
#define rotaryEncoderCS 10  // Brown
#define setZeroButton 9 // button to set position zero

uint8_t encoderIntake;  // allocate memory to contain encoder response
uint16_t encoderPosition;  // allocate memory to contain encoder rotational position
float encoderPositionDeg;  // allocate memory to contain encoder rotational position as degrees

AMT203 encoder(rotaryEncoderSCK, rotaryEncoderMOSI, rotaryEncoderMISO, rotaryEncoderCS, baudRate);  // create object encoder

void setup()
{
  Serial.begin(baudRate);  // open serial connection
  encoder.open();  // connect encoder to SPI datastream

  pinMode(setZeroButton,INPUT_PULLUP);  // set digital 9 pin to input for set_zero signal
}

void loop()
{
  // set current position as zero if setZeroButton depressed
  if (digitalRead(setZeroButton) == LOW) {
    encoder.setZero();
  }
  // collect current encoder position as encoderPosition
  encoderPosition = encoder.getPosition();
  // convert rotational position to degrees
  encoderPositionDeg = (((float)encoderPosition)/(4096)) * 360;
  // Serial.println("Min:0,Max:360");  // uncomment to demonstrate using serial plotter
  Serial.println(encoderPositionDeg);
}
