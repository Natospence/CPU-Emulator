#include "cs3421_emul.h"

//create memory, must free with cleanup
//Still unsure of what to do with the remaining part of the bits
int IMemory::create(int memSize) {
	size = memSize;
	memory = (unsigned int*)malloc(sizeof(int) * memSize);
	return 0;
}

//Setting a specific value into instruction memory
int IMemory::set(int address, unsigned int value) {
	memory[address] = value;
	return 0;
}

//Reset all memory to 0
int IMemory::reset() {
	for (int i = 0; i < size; i++) {
		memory[i] = 0; //Will this do what I expect it to (or do I need to multiply it by 3?
	}
	return 0;
}

int IMemory::dump(int startAddress, int count) {
	printf("Addr     0     1     2     3     4     5     6     7\n");

	//Determine the starting row
	int startRow = startAddress - (startAddress % 8);

	//Print appropriate leading whitespace for first row
	printf("0x%02X", startRow);
	for (int i = startRow; i < startAddress; i++) {
		printf("      ");
	}

	//For all the data
	for (int i = 1; i <= count; i++) {

		//Print it
		printf(" %05X", memory[startAddress + (i - 1)]);

		//If you've reached the end of a row, start the next row
		if ((startAddress + i) % 8 == 0 && i != count) {
			printf("\n0x%02X", startAddress + i);
		}
	}

	printf("\n\n");

	return 0;
}

int IMemory::cleanup() {
	free(memory);
	return 0;
}

int IMemory::startTick() {

	return 0;
}

int IMemory::doCycleWork() {

	return 0;
}

bool IMemory::isMoreCycleWorkNeeded() {

	return 0;
}

int IMemory::startFetch(unsigned int address, unsigned int count, unsigned int* memPtr, bool* memDonePtr) {

	for (int i = 0; i < count; i++) {
		memPtr[i] = memory[address + i];
	}

	return 0;

}

IMemory& getIMemory() {
	static IMemory iMemory;
	return iMemory;
};
