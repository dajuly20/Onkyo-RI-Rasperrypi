//---------------------------------------------------------------------------
//
// Name:        OnkyoRI.cpp
// Author:      Vita Tucek
// Created:     19.1.2016
// License:     MIT
// Description: Library for control Onkyo devices with RI port.
//              Message is composed from header, 12 databits and footer.
//
//              For timing delay() function is used. That means until send
//              is completed program is blocked (max send() duration is 61ms).
//
//---------------------------------------------------------------------------

#include "OnkyoRI.h"
#include <chrono>
#include <stdexcept>
#include <thread>

OnkyoRI::OnkyoRI(int lineOffset, const std::string &chipName)
    : _chip(nullptr), _line(nullptr), _lineOffset(lineOffset)
{
  _chip = gpiod_chip_open_by_name(chipName.c_str());
  if (!_chip)
  {
    throw std::runtime_error("Failed to open GPIO chip: " + chipName);
  }

  _line = gpiod_chip_get_line(_chip, _lineOffset);
  if (!_line)
  {
    gpiod_chip_close(_chip);
    throw std::runtime_error("Failed to get GPIO line " + std::to_string(_lineOffset));
  }

  if (gpiod_line_request_output(_line, "onkyori", 0) != 0)
  {
    gpiod_chip_close(_chip);
    throw std::runtime_error("Failed to request GPIO line output for line " + std::to_string(_lineOffset));
  }
}

OnkyoRI::~OnkyoRI()
{
  if (_line)
  {
    gpiod_line_release(_line);
    _line = nullptr;
  }
  if (_chip)
  {
    gpiod_chip_close(_chip);
    _chip = nullptr;
  }
}

void OnkyoRI::setLineValue(int value)
{
  if (_line && gpiod_line_set_value(_line, value) != 0)
  {
    throw std::runtime_error("Failed to set GPIO line value.");
  }
}

void OnkyoRI::sleepMicros(int micros)
{
  std::this_thread::sleep_for(std::chrono::microseconds(micros));
}

void OnkyoRI::sleepMillis(int millis)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

/// send command message to device
///
/// \param command  command to device
///
void OnkyoRI::send(int command)
{
  writeHeader();
  
  for(int i=0;i<12;i++)
  {
    bool level = command & 0x800;
    command <<= 1;
    writeBit(level);
  }

  writeFooter();
}
   
/// write message header 
void OnkyoRI::writeHeader()
{
  setLineValue(1);
  sleepMicros(3000);
  setLineValue(0);
  sleepMicros(1000);
}

/// write message bit
///
/// \param level  requested bit level (0/1)
///
void OnkyoRI::writeBit(bool level)
{
  setLineValue(1);
  sleepMicros(1000);
  setLineValue(0);
    
  if(level)
    sleepMicros(2000);
  else
    sleepMicros(1000);
}

/// write message footer
void OnkyoRI::writeFooter()
{
  setLineValue(1);
  sleepMicros(1000);
  setLineValue(0);
  sleepMillis(20);
}
