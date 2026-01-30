//---------------------------------------------------------------------------
//
// Name:        OnkyoRI.h
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

#ifndef ONKYORI_H
#define ONKYORI_H 

#include <gpiod.h>
#include <string>

class OnkyoRI
{
  public:
    OnkyoRI() : _chip(nullptr), _line(nullptr), _lineOffset(0) {};
    OnkyoRI(int lineOffset, const std::string &chipName);

    ~OnkyoRI();
    
    //send command message to device
    void send(int command);   
  
  private:
    gpiod_chip *_chip;
    gpiod_line *_line;
    int _lineOffset;
    
    //write message header 
    void writeHeader();
    //write message bit
    void writeBit(bool level);
    //write message footer
    void writeFooter();
    void setLineValue(int value);
    void sleepMicros(int micros);
    void sleepMillis(int millis);
};

#endif
