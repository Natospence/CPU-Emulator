#include "cs3421_emul.h"

//create memory, must free with cleanup
int Memory::create(int memSize) {
	size = memSize;
	memory = (unsigned char*)malloc(size);
	return 0;
}

//set a specific byte in memory
int Memory::set(int address, unsigned char byte) {
	memory[address] = byte;
	return 0;
}

//reset all memory to 0
int Memory::reset() {
	for (int i = 0; i < size; i++) {
		memory[i] = 0;
	}
	return 0;
}

int Memory::dump(int startAddress, int count) {
	printf("Addr 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");

	//Determine the starting row
	int startRow = startAddress / 16;

	//Print appropriate leading whitespace for first row
	printf("0x%02X", startRow);
	for (int i = (startRow * 16); i < startAddress; i++) {
		printf("   ");
	}

	//For all of the data
	for (int i = 1; i <= count; i++) {

		//Print it	
		printf(" %02X", memory[startAddress + (i - 1)]);

		//If you've reached the end of a row, start the next row
		if ((startAddress + i) % 16 == 0 && i != count) {
			printf("\n0x%02X", startAddress + i);
		}


	}

	printf("\n\n");

	return 0;
}

//free previously initialized memory
int Memory::cleanup() {
	free(memory);
	return 0;
}

int Memory::startTick() {
	tick++;
	return 0;
}

//called repeatedly until no more work to do by any components
int Memory::doCycleWork() {

	switch (state) {
	case MEM_WORKING:

		//if we've finished enough ticks
		if (tick == 5) {
			state = MEM_MOVE;
			workToDo = true;
		}

		break;

	case MEM_IDLE:
		workToDo = false;
		break;

	case MEM_MOVE: //once we're ready to move memory
		if (transferType == MEM_FETCH) {

			for (int i = 0; i < currentCount; i++) {
				currentForeignPointer[i] = memory[currentAddress + i];
			}

			if(!usedByIO)getCache().finishCacheFetch();
			usedByIO = false;
		}

		else if (transferType == MEM_STORE) { //Not deprecated since cache dumps can be disabled

			for (int i = 0; i < currentCount; i++) {
				memory[currentAddress + i] = currentForeignPointer[i];
			}

		}

		if(currentMemoryDonePointer) *currentMemoryDonePointer = true; //Notify the CPU, or NULL if from cache
		state = MEM_IDLE;

		break;

	case MEM_CACHE_FLUSH:
		workToDo = false;

		if (tick == currentTickRequirement) {
			state = MEM_IDLE;
			getCache().finishFlush();
		}

		break;
	}

	return 0;
}

bool Memory::isMoreCycleWorkNeeded() {
	return workToDo;
}

//start a memory fetch
int Memory::startFetch(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr) {

	//Update state variables
	state = MEM_WORKING;
	transferType = MEM_FETCH;
	tick = 1;

	//save all the information from the call
	currentAddress = address;
	currentCount = count;
	currentForeignPointer = dataPtr;
	currentMemoryDonePointer = memDonePtr;

	//Set the "Done flag" to false if cache is off
	if(currentMemoryDonePointer) *currentMemoryDonePointer = false;

	return 0;
}

//start a memory store
int Memory::startStore(unsigned int address, unsigned int count, unsigned char* dataPtr, bool* memDonePtr) {

	//update state variables
	state = MEM_WORKING;
	transferType = MEM_STORE;
	tick = 1;

	//save all information from the call
	currentAddress = address;
	currentCount = count;
	currentForeignPointer = dataPtr;
	currentMemoryDonePointer = memDonePtr;

	//Set the "Done flag" to false if cache is off
	*currentMemoryDonePointer = false;

	return 0;

}

//Called by the cache to manually pause memory to perform partial block transfers
int Memory::acceptCacheFlush(int ticks) {

	//update state variables
	state = MEM_CACHE_FLUSH;
	tick = 1;

	//Get information from function call
	currentTickRequirement = ticks;

	//Set the "Done flag" to false

	return 0;
}


Memory& getMemory() {
	static Memory memory;
	return memory;
};


