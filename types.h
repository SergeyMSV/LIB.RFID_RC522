#pragma once

#include <cstring>

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#define F(x) x

#define PROGMEM

#define SS 0
#define MSBFIRST 0
#define SPI_MODE0 0

extern std::chrono::steady_clock::time_point g_time_start;

typedef unsigned char byte;
typedef char __FlashStringHelper;

std::vector<uint8_t> SPITransfer(const std::vector<uint8_t>& tx);

enum DigitType
{
	HEX = false,
	DEC = true,
};

struct tSerial
{
	static void print(const std::string& val);
	static void print(byte val);
	static void print(byte val, DigitType stat);

	static void println(const std::string& val);
	static void println(byte val);
	static void println(byte val, DigitType stat);
	static void println();
};

extern tSerial Serial;

void delay(int val); // in ms
void delayMicroseconds(int val);

enum tGPIO
{
	SS_PIN,
	RST_PIN,
};

enum GPIOMode
{
	INPUT,
	OUTPUT,
};

void pinMode(byte pin, GPIOMode mode);

enum GPIOState
{
	LOW,
	HIGH,
};

void digitalWrite(byte pin, GPIOState state);
GPIOState digitalRead(byte pin);

//Returns the number of milliseconds passed since the Arduino board began running the current program.This number will overflow(go back to zero), after approximately 50 days.
uint32_t millis();

//Passes control to other tasks when called.Ideally yield() should be used in functions that will take awhile to complete.
void yield();

byte pgm_read_byte(const void* val);
