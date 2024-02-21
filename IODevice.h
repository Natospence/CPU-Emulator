#pragma once
#include "cs3421_emul.h"

//The IODevice simulates an IODevice plugged into the system
class IODevice
{

private:

	//The one byte data register that the IO device has
	unsigned char reg = 0;

	//The address of the last instruction loaded by the IO device
	int maxInstruction = 0;

	//The address of the current instruction
	int currentInstruction = 0;

	//An address to store a value to be saved to memory, since a pointer must be passed
	unsigned char immediateValue = 0;

	//A data type for instructions
	typedef struct IODeviceInstruction
	{
		int startTick;
		bool write;
		unsigned char address;
		unsigned char value;
	};

	struct IODeviceInstruction instructions[100];

public:
	int reset();
	int loadInstruction(int clockTick, bool write, unsigned char address, unsigned char value);
	int dump();
	int startTick();

};

IODevice& getIODevice();

