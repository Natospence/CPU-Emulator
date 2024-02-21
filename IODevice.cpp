#include "IODevice.h"

//Resets the IO device
int IODevice::reset() {
	maxInstruction = -1;
	currentInstruction = 0;
	reg = 0;
	return 0;
}

//Loads a single instruction into the IO device
int IODevice::loadInstruction(int clockTick, bool write, unsigned char address, unsigned char value) {
	
	if (maxInstruction < 0) maxInstruction = 0; //If the IODevice is currently sleeping, wake it up

	instructions[maxInstruction].startTick = clockTick;
	instructions[maxInstruction].write = write;
	instructions[maxInstruction].address = address;
	instructions[maxInstruction].value = value;

	maxInstruction++;

	return 0;
}

//Dump the contents of the IO device register
int IODevice::dump() {
	printf("IO Device: 0x%02X\n\n", reg);
	return 0;
}

//Check to see whether the current tick executed by clock requires the IO device to execute
int IODevice::startTick() {

	//If we're at the correct tick, execute the IO instruction
	if (instructions[currentInstruction].startTick == getClock().getTick()) {

		if (instructions[currentInstruction].write) {
			immediateValue = instructions[currentInstruction].value;
			bool throwaway; //This is simply used to avoid trying to dereference a NULL pointer
			getMemory().startStore(instructions[currentInstruction].address, 1, &immediateValue, &throwaway);
		}
		else {
			bool throwaway; //Used to avoid trying to dereference a NULL pointer
			getMemory().usedByIO = true;
			getMemory().startFetch(instructions[currentInstruction].address, 1, &reg, &throwaway);
		}

		currentInstruction++;
	}

	return 0;
}

//getter function for the IO Device
IODevice& getIODevice() {
	static IODevice ioDevice;
	return ioDevice;
}